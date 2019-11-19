#include "ECC.h"

void __p256int_add(p256_int *out, p256_int *a, p256_int *b);
void __p256int_sub(p256_int *out, p256_int *a, p256_int *b);
void __p256int_mul(p256_double_int *out, p256_int *a, p256_int *b);


void p256int_to_mpz(mpz_t out, p256_int *in)
{
    for(int i=0;i<(in->len);i++)
        out->_mp_d[i] = in->data[i];

    out->_mp_size = in->len;
}


void mpz_to_p256int(p256_int *out, mpz_t in)
{
    // in->_mp_size < 0  or  in->_mp_size > P256_MAX_WORD_LEN 이면 ERROR
    for(int i=0;i<(in->_mp_size);i++)
        out->data[i] = in->_mp_d[i];

    out->len = in->_mp_size;
}


void p256int_cpy(p256_int *dst, p256_int *src)
{
    dst->len = src->len;
    for(int i=0;i<(src->len);i++)
        dst->data[i] = src->data[i];
}


// 1:a>b, 0:a==b, -1:a<b
int p256int_cmp(p256_int *a, p256_int *b)
{
    if ((a->len) > (b->len)) return 1;
    if ((a->len) < (b->len)) return -1;

    for(int i=(a->len-1);i>=0;i--)
    {
        if(a->data[i] == b->data[i])
            continue;
        
        if(a->data[i] > b->data[i])
            return 1;
        if(a->data[i] < b->data[i])
            return -1;
    }
    return 0;
}


// c = a+b mod prime
void p256int_add(p256_int *out, p256_int *a, p256_int *b)
{
    __p256int_add(out, a, b);
    if(p256int_cmp(&p256_prime, out) < 0)
        __p256int_sub(out, out, &p256_prime);
}


void p256int_sub(p256_int *out, p256_int *a, p256_int *b)
{
    if(p256int_cmp(a, b) >= 0)
        __p256int_sub(out, a, b);
    else
    {
        __p256int_sub(out, b, a);
        __p256int_sub(out, &p256_prime, out);
    }
}


// 64-bit only
void p256int_mul(p256_int *out, p256_int *a, p256_int *b)
{
    p256_double_int mul;
    llint uv=0;

    for(int i=0;i<(a->len);i++)
    {
        uv = (llint)(a->data[i])*(b->data[0]) + (uv >> (WSIZE*8));
        mul.data[i] = (lint)uv;
    }
    mul.data[a->len] = uv >> (WSIZE*8);

    for(int j=1;j<(b->len);j++)
    {
        uv = 0;
        for(int i=0;i<(a->len);i++)
        {
            uv = (llint)(a->data[i])*(b->data[j]) + (uv >> (WSIZE*8)) + (mul.data[i+j]);
            mul.data[i+j] = (lint)uv;
        }
        mul.data[j+(a->len)] = uv >> (WSIZE*8);
    }

    mul.len = (a->len) + (b->len);

    for(int i=mul.len;i<P256_MAX_DOUBLE_BUF_LEN;i++)
        mul.data[i] = 0;

    p256_int res, tes, s1, s2, s3, s4, s5, s6, s7, s8, s9; // ans = (res - tes) % p256

    lint t1, t2, t3, t4, t5, t6, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t7;
    t1  = mul.data[7];                              //  1, (15,14)
    t2  = mul.data[6];                              //  2, (13,12)
    t3  = mul.data[5];                              //  3, (11,10)
    t4  = mul.data[4];                              //  4, ( 9, 8)
    t5  = t2 & 0xffffffff00000000LL;                //  5, (13,__)
    t6  = t2 << 32;                                 //  6, (12,__)
    t7 = t3 & 0xffffffff00000000LL;                 // 19, (11,__)
    t8  = t4 & 0xffffffff00000000LL;                //  7, ( 9,__)
    t9  = t1 >> 32;                                 //  8, (__,15)
    t10  = t3 & 0xffffffffLL;                       //  9, (__,10)
    t11 = t2 >> 32;                                 // 10, (__,13)
    t12 = (t1 << 32) | t11;                         // 11, (14,13)
    t13 = t5 | (t3 >> 32);                          // 12, (13,11)
    t14 = t6 | (t3 >> 32);                          // 13, (12,11)
    t15 = (t3 & 0xffffffff00000000LL) | (t4 >> 32); // 14, (11, 9)
    t16 = (t3 << 32) | (t4 >> 32);                  // 15, (10, 9)
    t17 = (t3 << 32) | (t4 & 0xffffffffLL);         // 16, (10, 8)
    t18 = (t4 << 32) | t9;                          // 17, ( 8,15)
    t19 = (t4 << 32) | t11;                         // 18, ( 8,13)
    
    // formula : http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.394.3037&rep=rep1&type=pdf (p.46)
    // s2 = (15,14,  13,12,  11,__,  __,__)
    s2.len = P256_MAX_WORD_LEN;
    s2.data[3] = t1; s2.data[2] = t2;  s2.data[1] = t7; s2.data[0] = 0;
    // s3 = (__,15,  14,13,  12,__,  __,__)
    s3.len = P256_MAX_WORD_LEN;
    s3.data[3] = t9; s3.data[2] = t12; s3.data[1] = t6; s3.data[0] = 0;
    p256int_add(&res, &s2, &s3);
    p256int_add(&res, &res, &res);
    // s4 = (15,14,  __,__,  __,10,  9, 8)
    s4.len = P256_MAX_WORD_LEN;
    s4.data[3] = t1; s4.data[2] = 0; s4.data[1] = t10; s4.data[0] = t4;
    p256int_add(&res, &res, &s4);
    // s5 = ( 8,13,  15,14,  13,11,  10, 9)
    s5.len = P256_MAX_WORD_LEN;
    s5.data[3] = t19; s5.data[2] = t1; s5.data[1] = t13; s5.data[0] = t16;
    p256int_add(&res, &res, &s5);
    // s1 = ( 7, 6, 5, 4, 3, 2, 1, 0)
    s1.len = P256_MAX_WORD_LEN;
    s1.data[3] = mul.data[3]; s1.data[2] = mul.data[2]; s1.data[1] = mul.data[1]; s1.data[0] = mul.data[0];
    p256int_add(&res, &res, &s1);
    // s6 = (10, 8,  __,__,  __,13,  12,11)
    s6.len = P256_MAX_WORD_LEN;
    s6.data[3] = t17; s6.data[2] = 0; s6.data[1] = t11; s6.data[0] = t14;
    // s7 = (11, 9,  __,__,  15,14,  13,12)
    s7.len = P256_MAX_WORD_LEN;
    s7.data[3] = t15; s7.data[2] = 0; s7.data[1] = t1; s7.data[0] = t2;
    p256int_add(&tes, &s6, &s7);
    // s8 = (12,__,  10, 9,   8,15,  14,13)
    s8.len = P256_MAX_WORD_LEN;
    s8.data[3] = t6; s8.data[2] = t16; s8.data[1] = t18; s8.data[0] = t12;
    p256int_add(&tes, &tes, &s8);
    // s9 = (13,__,  11,10,   9,__,  15,14)
    s9.len = P256_MAX_WORD_LEN;
    s9.data[3] = t5; s9.data[2] = t3; s9.data[1] = t8; s9.data[0] = t1;
    p256int_add(&tes, &tes, &s9);

    // res = res - tes
    p256int_sub(&res, &res, &tes);
    p256int_cpy(out, &res);
}


// return -1 for 0^(-1)
int p256int_inv(p256_int *out, p256_int *a)
{
    if(a->len == 0)
        return -1;

    mpz_t a_mpz, res_mpz, mod_mpz;

    mpz_init2(a_mpz, 256);
    mpz_init2(res_mpz, 256);
    mpz_init2(mod_mpz, 256);

    p256int_to_mpz(a_mpz, a);
    p256int_to_mpz(mod_mpz, &p256_prime);

    mpz_invert(res_mpz, a_mpz, mod_mpz);

    mpz_to_p256int(out, res_mpz);

    mpz_clear(a_mpz);
    mpz_clear(res_mpz);
    mpz_clear(mod_mpz);

    return 0;
}


// 0: P == Q , otherwise: P != Q
int p256_AF_cmp(p256_AF_pt *P, p256_AF_pt *Q)
{
    if(P->at_infinity == Q->at_infinity)
    {
        if(P->at_infinity)
            return 0;

        return p256int_cmp(&P->x, &Q->x) || p256int_cmp(&P->y, &Q->y);
    }
    return 1;
}

void p256_AF_cpy(p256_AF_pt *R, p256_AF_pt *P)
{
    p256int_cpy(&R->x, &P->x);
    p256int_cpy(&R->y, &P->y);
    R->at_infinity = P->at_infinity;
}

int p256_AF_dbl(p256_AF_pt *R, p256_AF_pt *P)
{
    if(P->at_infinity || P->y.len==0)
    {
        R->at_infinity = 1;
        return 0;
    }

    static p256_int lambda, dx, dy, x3, y3, tmp;
    p256int_add(&dx, &P->y, &P->y);
    p256int_mul(&tmp, &P->x, &P->x);
    p256int_add(&dy, &tmp, &tmp);
    p256int_add(&dy, &dy, &tmp);
    p256int_add(&dy, &dy, &p256_coef_a);
    
    p256int_inv(&dx, &dx);
    p256int_mul(&lambda, &dx, &dy);
    p256int_mul(&x3, &lambda, &lambda);
    p256int_sub(&x3, &x3, &P->x);
    p256int_sub(&x3, &x3, &P->x);
    p256int_sub(&y3, &P->x, &x3);
    p256int_mul(&y3, &y3, &lambda);
    p256int_sub(&y3, &y3, &P->y);

    p256int_cpy(&R->x, &x3);
    p256int_cpy(&R->y, &y3);
    R->at_infinity = 0;

    return 0;
}

int p256_AF_add(p256_AF_pt *R, p256_AF_pt *P, p256_AF_pt *Q)
{
    if(P->at_infinity)
    {
        p256_AF_cpy(R, Q);
        return 0;
    }
    if(Q->at_infinity)
    {
        p256_AF_cpy(R, P);
        return 0;
    }
    if(p256int_cmp(&P->x, &Q->x)==0)
    {
        if(p256int_cmp(&P->y, &Q->y)==0)
            p256_AF_dbl(R, P);
        else
            R->at_infinity = 1;

        return 0;
    }

    static p256_int lambda, dx, dy, x3, y3;
    
    p256int_sub(&dx, &P->x, &Q->x);
    p256int_sub(&dy, &P->y, &Q->y);
    p256int_inv(&dx, &dx);
    p256int_mul(&lambda, &dx, &dy);
    p256int_mul(&x3, &lambda, &lambda);
    p256int_sub(&x3, &x3, &P->x);
    p256int_sub(&x3, &x3, &Q->x);
    p256int_sub(&y3, &P->x, &x3);
    p256int_mul(&y3, &y3, &lambda);
    p256int_sub(&y3, &y3, &P->y);

    p256int_cpy(&R->x, &x3);
    p256int_cpy(&R->y, &y3);
    R->at_infinity = 0;
    return 0;
}


int p256_AF_binary_smul(p256_AF_pt *R, p256_int *k, p256_AF_pt *P)
{
    p256_AF_pt res;

    res.at_infinity = 1;
    for(int i=(k->len)-1;i>=0;i--)
    {
        for(int j=WSIZE*8-1;j>=0;j--)
        {
            p256_AF_dbl(&res, &res);
            if((k->data[i]>>j) & 0x1)
                p256_AF_add(&res, &res, P);
        }
    }
    p256_AF_cpy(R, &res);
    return 0;
}


int p256_AF_M_m_ary_smul(p256_AF_pt *R, p256_int *k, p256_AF_pt *P)
{
    const int BITNUM = 4;
    const int MASK = (1<<BITNUM)-1;
    static p256_AF_pt P2; // 2P
    static p256_AF_pt kP[8]; // 1<<(BITNUM-1). kP[i] = (2*i+1)P, or i*P = kP[i/2];
    static int odd[16] =  {-1, 0, 0, 1, 0, 2, 1, 3, 0, 4, 2, 5, 1, 6, 3, 7}; // 1<<BITNUM. num = (2*odd[num]+1) << base[num]
    static int base[16] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
    p256_AF_pt res;

    p256_AF_cpy(&kP[0], P);
    p256_AF_dbl(&P2, P);
    for(int i=1;i<(1<<(BITNUM-1));i++)
        p256_AF_add(&kP[i], &kP[i-1], &P2);
    
    res.at_infinity = 1;
    for(int i=(k->len)-1;i>=0;i--)
    {
        for(int j=(WSIZE*8-BITNUM);j>=0;j-=BITNUM)
        {
            int num = (k->data[i] >> j) & MASK;
            for(int cnt=BITNUM;cnt>base[num];cnt--)
                p256_AF_dbl(&res, &res);

            if(num)
            {
                p256_AF_add(&res, &res, &kP[odd[num]]);
                for(int cnt=base[num];cnt>0;cnt--)
                    p256_AF_dbl(&res, &res);
            }
        }
    }
    p256_AF_cpy(R, &res);

    return 0;
}


void __p256int_add(p256_int *out, p256_int *a, p256_int *b)
{
    if((a->len) < (b->len))
    {
        p256_int *t = a; a = b; b = t;
    }

    p256_int res;
    lint carry=0;

    for(int i=0;i<(b->len);i++)
    {
        res.data[i] = a->data[i] + b->data[i] + carry;
        carry = (res.data[i] < (a->data[i])) || (carry==1 && res.data[i] == (a->data[i]));
    }
    for(int i=(b->len);i<(a->len);i++)
    {
        res.data[i] = a->data[i] + carry;
        carry = (carry==1 && res.data[i]==0);
    }
    res.data[a->len] = carry;
    res.len = a->len + carry;

    p256int_cpy(out, &res);
}


// c = a-b (WLOG, a >= b)
void __p256int_sub(p256_int *out, p256_int *a, p256_int *b)
{
    p256_int res;
    lint borrow=0;

    for(int i=0;i<(b->len);i++)
    {
        res.data[i] = a->data[i] - b->data[i] - borrow;
        borrow = (a->data[i] < b->data[i]) || (borrow==1 && (a->data[i]==b->data[i]));
    }
    for(int i=(b->len);i<(a->len);i++)
    {
        res.data[i] = a->data[i] - borrow;
        borrow = (borrow==1 && a->data[i]==0);
    }
    res.len = a->len;

    while(res.len>0 && res.data[res.len-1]==0)
        res.len--;

    p256int_cpy(out, &res);
}


void __p256int_mul(p256_double_int *out, p256_int *a, p256_int *b)
{
    llint uv=0;

    for(int i=0;i<(a->len);i++)
    {
        uv = (llint)(a->data[i])*(b->data[0]) + (uv >> (WSIZE*8));
        out->data[i] = (lint)uv;
    }
    out->data[a->len] = uv >> (WSIZE*8);

    for(int j=1;j<(b->len);j++)
    {
        uv = 0;
        for(int i=0;i<(a->len);i++)
        {
            uv = (llint)(a->data[i])*(b->data[j]) + (uv >> (WSIZE*8)) + (out->data[i+j]);
            out->data[i+j] = (lint)uv;
        }
        out->data[j+(a->len)] = uv >> (WSIZE*8);
    }

    out->len = (a->len) + (b->len);
    if((out->len) > 0 && out->data[out->len-1]==0)
        out->len -= 1;
}


void __p256doubleint_to_mpz(mpz_t out, p256_double_int *in)
{
    for(int i=0;i<(in->len);i++)
        out->_mp_d[i] = in->data[i];

    out->_mp_size = in->len;
}