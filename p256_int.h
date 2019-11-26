#ifndef __P256_INT_H_
#define __P256_INT_H_
#include <gmp.h>
#include "p256_config.h"

typedef struct _p256_int {
    lint data[P256_MAX_BUF_LEN];
    int           len;
} p256_int;

typedef struct _p256_double_int{
    lint data[P256_MAX_DOUBLE_BUF_LEN];
    int   len;
} p256_double_int;

void p256int_print(p256_int *a);
void mpz_to_p256int(p256_int *out, mpz_t in);
void p256int_to_mpz(mpz_t out, p256_int *in);
void p256int_to_bytes(unsigned char *out, int *outlen, p256_int *in);
void bytes_to_p256int(p256_int *out, unsigned char *in, int inlen);

void p256int_add(p256_int *out, p256_int *a, p256_int *b);
void p256int_sub(p256_int *out, p256_int *a, p256_int *b);
void p256int_mul(p256_int *out, p256_int *a, p256_int *b);  // 64-bit version only
int  p256int_inv(p256_int *out, p256_int *a);               // return -1 for 0^(-1)
void p256int_cpy(p256_int *dst, p256_int *src);
int p256int_cmp(p256_int *a, p256_int *b);                  // 1:a>b, 0:a==b, -1:a<b

void p256int_mod_order(p256_int *out, p256_int *a, p256_int *mod);
void p256int_add_order(p256_int *out, p256_int *a, p256_int *b, p256_int *mod);
void p256int_sub_order(p256_int *out, p256_int *a, p256_int *b, p256_int *mod);
void p256int_mul_order(p256_int *out, p256_int *a, p256_int *b, p256_int *mod);
int  p256int_inv_order(p256_int *out, p256_int *a, p256_int *mod);

// p256_prime = 2^256 - 2^224 + 2^192 + 2^96 - 1
static p256_int p256_prime  = P256_PRIME;

#endif