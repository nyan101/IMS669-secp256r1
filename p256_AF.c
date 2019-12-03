#include "p256_int.h"
#include "p256_AF.h"

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


int p256_AF_comb_precompute(p256_AF_pt *P)
{
    for(int i=0;i<256;i++)
        P_comb[i].at_infinity = 1;

    p256_AF_cpy(&P_comb[1], P);
    for(int e=1;e<combsize;e++)
    {
        p256_AF_cpy(&P_comb[1<<e], &P_comb[1<<(e-1)]);
        for(int i=0;i<(256/combsize);i++)
            p256_AF_dbl(&P_comb[1<<e], &P_comb[1<<e]);
    }
    for(int i=3;i<256;i++)
    {
        if((i&(i-1))==0)
            continue;
        for(int b=1;b<256;b<<=1)
        {
            if(i & b)
                p256_AF_add(&P_comb[i], &P_comb[i], &P_comb[b]);
        }
    }
    return 0;
}


int p256_AF_comb_smul(p256_AF_pt *R, p256_int *k, p256_AF_pt *P)
{
    lint b1 = 0x8000000000000000; int blen1 = 63;
    lint b2 = 0x0000000080000000; int blen2 = 31;
    p256_AF_pt res;
    res.at_infinity = 1;

    for(int i=P256_MAX_WORD_LEN-1;i>=(k->len);i--)
        k->data[i] = 0;

    for(int i=0;i<32;i++)
    {
        int idx = 0;
        for(int j=3;j>=0;j--)
        {
            idx <<= 1;
            idx |= ((k->data[j]) & b1)>>blen1;
            idx <<= 1;
            idx |= ((k->data[j]) & b2)>>blen2;
        }
        b1 >>= 1; b2 >>= 1;
        blen1--;  blen2--;
        p256_AF_dbl(&res, &res);
        p256_AF_add(&res, &res, &P_comb[idx]);
    }
    p256_AF_cpy(R, &res);
    return 0;
}