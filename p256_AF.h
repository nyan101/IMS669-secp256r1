#ifndef __P256_AF_H_
#define __P256_AF_H_
#include <gmp.h>
#include "p256_int.h"
#include "p256_config.h"

typedef struct _p256_AF_pt {
    p256_int x;
    p256_int y;
    int at_infinity;
} p256_AF_pt;

int p256_AF_add(p256_AF_pt *R, p256_AF_pt *P, p256_AF_pt *Q);
int p256_AF_dlb(p256_AF_pt *R, p256_AF_pt *P);
int p256_AF_binary_smul(p256_AF_pt *R, p256_int *k, p256_AF_pt *P);
int p256_AF_M_m_ary_smul(p256_AF_pt *R, p256_int *k, p256_AF_pt *P);
void p256_AF_cpy(p256_AF_pt *R, p256_AF_pt *P);
int p256_AF_cmp(p256_AF_pt *P, p256_AF_pt *Q);                      // 0: P == Q , otherwise: P != Q

int pub_key_validation(p256_AF_pt *Q);                              // return TRUE or FALSE
int p256_AF_DH_PK_gen(p256_AF_pt *Q, p256_int *x, p256_int *k);     // (Q,x): (pub, priv) / k: random
int p256_AF_DH_SS_gen(p256_AF_pt *R, p256_AF_pt *Q, p256_int *x);



static p256_int p256_coef_a = P256_COEF_A;
static p256_int p256_coef_b = P256_COEF_B;

static p256_int p256_order = P256_ORDER;
static p256_AF_pt p256_base_point = P256_BASE_POINT;

#endif