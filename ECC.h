#include <gmp.h>

#define P256_MAX_WORD_LEN 4
#define P256_MAX_BUF_LEN 5
#define P256_MAX_DOUBLE_BUF_LEN 10

typedef unsigned long long lint; // surely 64-bit integer (for both 32/64-bit)

typedef struct _p256_int {
    lint data[P256_MAX_BUF_LEN];
    int           len;
} p256_int;

void mpz_to_p256int(p256_int *out, mpz_t in);
void p256int_to_mpz(mpz_t out, p256_int *in);

void p256int_add(p256_int *out, p256_int *a, p256_int *b);
void p256int_cpy(p256_int *dst, p256_int *src);
int p256int_cmp(p256_int *a, p256_int *b); // 1:a>b, 0:a==b, -1:a<b

// p256_prime = 2^256 - 2^224 + 2^192 + 2^96 - 1
static p256_int p256_prime = {{0xffffffffffffffffLL, 0x00000000ffffffffLL, 0x0000000000000000LL, 0xffffffff00000001LL, 0LL}, P256_MAX_WORD_LEN};