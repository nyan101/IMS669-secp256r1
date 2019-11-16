#include <stdio.h>
#include "ECC.h"

int main()
{
    p256_int a, b, c, d;
    mpz_t p256_prime_mpz, a_mpz, b_mpz, c_mpz, d_mpz;
    gmp_randstate_t state;

    // init
    mpz_init2(a_mpz, 256); mpz_init2(b_mpz, 256);
    mpz_init2(c_mpz, 256); mpz_init2(d_mpz, 256);
    mpz_init2(p256_prime_mpz, 256);
    gmp_randinit_default(state);

    // generate random numbers for test
    p256int_to_mpz(p256_prime_mpz, &p256_prime);
    mpz_urandomm(a_mpz, state, p256_prime_mpz);
    mpz_urandomm(b_mpz, state, p256_prime_mpz);

    // kminchul : c = (a+b) mod p256
    mpz_to_p256int(&a, a_mpz);
    mpz_to_p256int(&b, b_mpz);
    p256int_add(&c, &a, &b);
    p256int_to_mpz(c_mpz, &c);

    // gmp : d = (a+b) mod p256
    mpz_add(d_mpz, a_mpz, b_mpz);
    mpz_mod(d_mpz, d_mpz, p256_prime_mpz);

    // test
    if(mpz_cmp(c_mpz, d_mpz)==0)
        printf("<Test Passed>\n");
    else
        printf("<Test Failed>\n");

    gmp_printf("a : %Zd\n", a_mpz);
    gmp_printf("b : %Zd\n", b_mpz);
    gmp_printf("my answer  : %Zd\n", c_mpz);
    gmp_printf("gmp answer : %Zd\n", d_mpz);

    return 0;
}


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

    if(p256int_cmp(&p256_prime, &res) < 0)
    {
        // res -= p256_prime;
        mpz_t res_mpz, p256_prime_mpz;
        
        mpz_init2(res_mpz, 256);
        p256int_to_mpz(res_mpz, &res);

        mpz_init2(p256_prime_mpz, 256);
        p256int_to_mpz(p256_prime_mpz, &p256_prime);

        mpz_sub(res_mpz, res_mpz, p256_prime_mpz);

        mpz_to_p256int(&res, res_mpz);
        
        mpz_clear(res_mpz);
        mpz_clear(p256_prime_mpz);
    }

    p256int_cpy(out, &res);
}