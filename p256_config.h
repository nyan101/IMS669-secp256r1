#ifndef __P256_CONFIG_H_
#define __P256_CONFIG_H_

#define TRUE 1
#define FALSE 0

#define WSIZE 8 // 64-bit

#if WSIZE == 8
    #define P256_MAX_WORD_LEN 4
    #define P256_MAX_BUF_LEN 5
    #define P256_MAX_DOUBLE_BUF_LEN 9

    typedef unsigned long long lint; // surely 64-bit integer (for both 32/64-bit)
    typedef unsigned __int128 llint; // 128-bit extension (for most versions of GCC)

    #define P256_PRIME {{0xffffffffffffffffLL, 0x00000000ffffffffLL, 0x0000000000000000LL, 0xffffffff00000001LL, 0LL}, P256_MAX_WORD_LEN}
    #define P256_COEF_A {{0xfffffffffffffffcLL, 0x00000000ffffffffLL, 0x0000000000000000LL, 0xffffffff00000001LL, 0LL}, P256_MAX_WORD_LEN}
    #define P256_COEF_B {{0x3bce3c3e27d2604bLL, 0x651d06b0cc53b0f6LL, 0xb3ebbd55769886bcLL, 0x5ac635d8aa3a93e7LL, 0LL}, P256_MAX_WORD_LEN}
    #define P256_ORDER {{0xf3b9cac2fc632551LL, 0xbce6faada7179e84LL, 0xffffffffffffffffLL, 0xffffffff00000000LL, 0LL}, P256_MAX_WORD_LEN}
    #define P256_BASE_POINT { \
                        {{0xf4a13945d898c296LL, 0x77037d812deb33a0LL, 0xf8bce6e563a440f2LL, 0x6b17d1f2e12c4247LL, 0LL}, P256_MAX_WORD_LEN}, \
                        {{0xcbb6406837bf51f5LL, 0x2bce33576b315eceLL, 0x8ee7eb4a7c0f9e16LL, 0x4fe342e2fe1a7f9bLL, 0LL}, P256_MAX_WORD_LEN}, \
                        0 };
#endif
#if WSIZE == 4
    #define P256_MAX_WORD_LEN 8
    #define P256_MAX_BUF_LEN 9
    #define P256_MAX_DOUBLE_BUF_LEN 17

    typedef unsigned int lint; // surely 32-bit integer (for both 32/64-bit)
    typedef unsigned long long llint; // 64-bit extension (for most versions of GCC)

    #define P256_PRIME {{0xffffffff,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,0x00000001,0xffffffff,0}, P256_MAX_WORD_LEN}
    #define P256_COEF_A {{0xfffffffc,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,0x00000001,0xffffffff,0},   P256_MAX_WORD_LEN}
    #define P256_COEF_B {{0x27d2604b,0x3bce3c3e,0xcc53b0f6,0x651d06b0,0x769886bc,0xb3ebbd55,0xaa3a93e7,0x5ac635d8,0},	P256_MAX_WORD_LEN}
    #define P256_ORDER {{0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad, 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff,0 }, P256_MAX_WORD_LEN}
    #define P256_BASE_POINT { \
                    {{ 0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81, 0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2,0 }, P256_MAX_WORD_LEN},\
                    {{ 0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357, 0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2,0 }, P256_MAX_WORD_LEN},\
                    0 };
#endif

#endif