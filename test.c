#include <stdio.h>
#include <time.h>
#include <gmp.h>
#include "ECC.h"

clock_t elapsed; float sec;

#define START_WATCH \
{\
 elapsed = -clock(); \
}\

#define STOP_WATCH \
{\
 elapsed += clock();\
 sec = (float)elapsed/CLOCKS_PER_SEC;\
}\

#define PRINT_TIME(qstr) \
{\
 printf("\n[%s: %.5f s]\n",qstr,sec);\
}\


void test_p256_add_sub()
{
    p256_int a, b, c, d;
    mpz_t p256_prime_mpz, a_mpz, b_mpz, c_mpz, d_mpz;
    gmp_randstate_t state;
    int num_test, num_correct;

    // init
    mpz_init2(a_mpz, 256); mpz_init2(b_mpz, 256);
    mpz_init2(c_mpz, 256); mpz_init2(d_mpz, 256);
    mpz_init2(p256_prime_mpz, 256);
    gmp_randinit_default(state);

    // generate random numbers for test
    p256int_to_mpz(p256_prime_mpz, &p256_prime);

    // add test
    num_test = 1000;
    num_correct = 0;
    printf("Add Test Begin...");
    for(int i=0;i<num_test;i++)
    {
        mpz_urandomm(a_mpz, state, p256_prime_mpz);
        mpz_urandomm(b_mpz, state, p256_prime_mpz);

        // my code : c = (a+b) mod p256
        mpz_to_p256int(&a, a_mpz);
        mpz_to_p256int(&b, b_mpz);
        p256int_add(&c, &a, &b);
        p256int_to_mpz(c_mpz, &c);

        // gmp : d = (a+b) mod p256
        mpz_add(d_mpz, a_mpz, b_mpz);
        mpz_mod(d_mpz, d_mpz, p256_prime_mpz);

        // test
        if(mpz_cmp(c_mpz, d_mpz)!=0)
            printf("<Add Test Failed>\n");
        else
            num_correct += 1;
    }
    printf("End (%d/%d)\n", num_correct, num_test);

    // sub test
    num_test = 1000;
    num_correct = 0;
    printf("Sub Test Begin...");
    for(int i=0;i<num_test;i++)
    {
        mpz_urandomm(a_mpz, state, p256_prime_mpz);
        mpz_urandomm(b_mpz, state, p256_prime_mpz);

        // my code : c = (a+b) mod p256
        mpz_to_p256int(&a, a_mpz);
        mpz_to_p256int(&b, b_mpz);
        p256int_sub(&c, &a, &b);
        p256int_to_mpz(c_mpz, &c);

        // gmp : d = (a+b) mod p256
        mpz_sub(d_mpz, a_mpz, b_mpz);
        mpz_mod(d_mpz, d_mpz, p256_prime_mpz);

        // test
        if(mpz_cmp(c_mpz, d_mpz)!=0)
            printf("<Sub Test Failed>\n");
        else
            num_correct += 1;
    }
    printf("End (%d/%d)\n", num_correct, num_test);
}


int main(void)
{
	test_p256_add_sub();

    return 0;
}