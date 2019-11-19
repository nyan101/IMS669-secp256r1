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

void test_p256_add_sub();
void test_p256_mul();
void test_p256_ADD_DBL();
void test_p256_SMUL();


int main(void)
{
	test_p256_SMUL();

    return 0;
}

void test_p256_SMUL()
{
    p256_AF_pt P11, R;
    p256_int eleven;

    // P11 = 11*P
    eleven.len = 1;
    eleven.data[0] = 11;
    P11.at_infinity = 1;
    for(int i=0;i<11;i++)
        p256_AF_add(&P11, &P11, &p256_base_point);

    // test #2
    printf("SMUL Binary Test #1: (11 * P = P11) ... ");
    p256_AF_binary_smul(&R, &eleven, &p256_base_point);
    if(p256_AF_cmp(&P11, &R)==0)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");

    printf("SMUL Binary Test #2: (order * P = O) ... ");
    p256_AF_binary_smul(&R, &p256_order, &p256_base_point);
    if(R.at_infinity)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");

    printf("SMUL Modified M-ary Test #1: (11 * P = P11) ... ");
    p256_AF_binary_smul(&R, &eleven, &p256_base_point);
    if(p256_AF_cmp(&P11, &R)==0)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");
    printf("SMUL Modified M-ary Test #2: (order * P = O) ... ");
    p256_AF_M_m_ary_smul(&R, &p256_order, &p256_base_point);
    if(R.at_infinity)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");
}


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


void test_p256_mul()
{
    p256_int a, b, c;
    mpz_t p256_prime_mpz, a_mpz, b_mpz, c_mpz, d_mpz;
    gmp_randstate_t state;
    int num_test, num_correct;

    // init
    num_test = 10000;
    num_correct = 0;
    mpz_init2(a_mpz, 256); mpz_init2(b_mpz, 256);
    mpz_init2(c_mpz, 256); mpz_init2(d_mpz, 256);
    mpz_init2(p256_prime_mpz, 256);
    p256int_to_mpz(p256_prime_mpz, &p256_prime);
    gmp_randinit_default(state);

    // correct test
    printf("* Mul Correct Test Begin...");
    for(int i=0;i<num_test;i++)
    {
        mpz_urandomm(a_mpz, state, p256_prime_mpz);
        mpz_urandomm(b_mpz, state, p256_prime_mpz);

        // kminchul : c = a * b
        mpz_to_p256int(&a, a_mpz);
        mpz_to_p256int(&b, b_mpz);
        p256int_mul(&c, &a, &b);
        p256int_to_mpz(c_mpz, &c);

        // gmp : d = a * b
        mpz_mul(d_mpz, a_mpz, b_mpz);
        mpz_mod(d_mpz, d_mpz, p256_prime_mpz);


        // test
        if(mpz_cmp(c_mpz, d_mpz)!=0)
        {
            printf("<Mul Test Failed>\n");
            gmp_printf("my  : %d, %Zx\n", c.len, c_mpz);
            gmp_printf("mpz : %d, %Zx\n", d_mpz->_mp_size, d_mpz);
        }
        else
            num_correct++;
    }
    printf("End (%d/%d)\n\n", num_correct, num_test);

    num_test = 10000000;
    mpz_urandomm(a_mpz, state, p256_prime_mpz);
    mpz_urandomm(b_mpz, state, p256_prime_mpz);
    mpz_to_p256int(&a, a_mpz);
    mpz_to_p256int(&b, b_mpz);
    printf("* Speed Test with %d Run\n", num_test);
    printf("word size : %d\n", WSIZE);
    gmp_printf("a : %Zx \n", a_mpz);
    gmp_printf("b : %Zx \n", b_mpz);
    
    // speed test #1
	START_WATCH;
    for(int i=0;i<num_test;i++)
    {
        mpz_mul(d_mpz, a_mpz, b_mpz);
        mpz_mod(d_mpz, d_mpz, p256_prime_mpz);
    }
	STOP_WATCH;
	PRINT_TIME("mpz_mul_mod time");

    // speed test #2
	START_WATCH;
    for(int i=0;i<num_test;i++)
        p256int_mul(&c, &a, &b);
	STOP_WATCH;
	PRINT_TIME("p256_mul_mod time");
}


void test_p256_ADD_DBL()
{
    p256_AF_pt P, Q, R, P2;
    p256_int x1, x2, x3, y1, y2, y3;
    mpz_t x1_mpz, x2_mpz, x3_mpz, y1_mpz, y2_mpz, y3_mpz;

    // set values. P=(x1,y1), Q=2P, R=3P
    mpz_init_set_str(x1_mpz, "83852be5a3ee4d1515bed279cef585d86fe8a730816db6dd00e358c1bccbf133", 16);
    mpz_init_set_str(y1_mpz, "e3ef1726821ec6586a790fe72631048bce00c41043035aeef8474c884e60b67a", 16);
    mpz_init_set_str(x2_mpz, "77d12488bbd94b3e4db310972e4d3269d5dccfa28ad82b5e0d3816c5935a1c7d", 16);
    mpz_init_set_str(y2_mpz, "b6f09c6a49ffa993c03b2e892056fe662266f463f2b74e84423333a115ed2387", 16);
    mpz_init_set_str(x3_mpz, "757e761357843d8bd5d809acabf5b918802c37168f4da9a89bf8698aefa97140", 16);
    mpz_init_set_str(y3_mpz, "64dd79cc54a3788441f19d550e03f956f65aec497594b0a531a42ed4fc88b583", 16);
    // set p256_int
    mpz_to_p256int(&x1, x1_mpz); mpz_to_p256int(&y1, y1_mpz);
    mpz_to_p256int(&x2, x2_mpz); mpz_to_p256int(&y2, y2_mpz);
    mpz_to_p256int(&x3, x3_mpz); mpz_to_p256int(&y3, y3_mpz);
    // set AF points
    p256int_cpy(&(P.x), &x1); p256int_cpy(&(P.y), &y1);
    p256int_cpy(&(Q.x), &x2); p256int_cpy(&(Q.y), &y2);
    P.at_infinity = Q.at_infinity = 0;

    // doubling test
    printf("Doubling Test (P2 = P + P) ... ");
    p256_AF_add(&P2, &P, &P);
    if(p256int_cmp(&(P2.x), &x2)==0 && p256int_cmp(&(P2.y), &y2)==0)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");

    // add test
    printf("Addition Test (R = P + Q = 3P) ... ");
    p256_AF_add(&R, &P, &Q);
    if(p256int_cmp(&(R.x), &x3)==0 && p256int_cmp(&(R.y), &y3)==0)
        printf("<PASSED>\n");
    else
        printf("<FAILED>\n");

    // clear
    mpz_clear(x1_mpz); mpz_clear(y1_mpz);
    mpz_clear(x2_mpz); mpz_clear(y2_mpz);
    mpz_clear(x3_mpz); mpz_clear(y3_mpz);
}