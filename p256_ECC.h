#ifndef __P256_ECC_H_
#define __P256_ECC_H_
#include "p256_config.h"
#include "p256_int.h"
#include "p256_AF.h"

int pub_key_validation(p256_AF_pt *Q);                              // return TRUE or FALSE
int p256_AF_DH_PK_gen(p256_AF_pt *Q, p256_int *x, p256_int *k);     // (Q,x): (pub, priv) / k: random
int p256_AF_DH_SS_gen(p256_AF_pt *R, p256_AF_pt *Q, p256_int *x);
int p256_ECDSA_sign(unsigned char *r,   // sign value 1 (output)
                    unsigned char *s,   // sign value 2 (output)
                    unsigned char *k,   // random seed  (input)
                    unsigned char *d,   // private key  (input)
                    unsigned char *hm,  // message hash (input)
                    int mode);

int p256_ECDSA_verify(unsigned char *_r,   // sign value 1 (input)
                      unsigned char *_s,   // sign value 2 (input)
                      unsigned char *_hm,  // message hash (input)
                      unsigned char *_Qx,  // public key   (input)
                      unsigned char *_Qy,
                      int mode);

#endif