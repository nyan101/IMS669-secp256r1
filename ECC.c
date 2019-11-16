#include "ECC.h"

void __p256int_add(p256_int *out, p256_int *a, p256_int *b);
void __p256int_sub(p256_int *out, p256_int *a, p256_int *b);


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


// c = a-b mod prime (WLOG, a >= b)
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