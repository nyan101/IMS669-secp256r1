#include "p256_int.h"
#include "p256_AF.h"
#include "p256_ECC.h"

int pub_key_validation(p256_AF_pt *Q)
{
    if(Q->at_infinity)  return FALSE;
    if(p256int_cmp(&Q->x, &p256_prime) >= 0) return FALSE;
    if(p256int_cmp(&Q->y, &p256_prime) >= 0) return FALSE;
    
    p256_int yt, xt, ax;

    p256int_mul(&yt, &Q->y, &Q->y);

    p256int_mul(&xt, &Q->x, &Q->x);
    p256int_mul(&xt, &xt, &Q->x);
    p256int_mul(&ax, &Q->x, &p256_coef_a);
    p256int_add(&xt, &xt, &ax);
    p256int_add(&xt, &xt, &p256_coef_b);
    
    if(p256int_cmp(&xt, &yt)!=0)
        return FALSE;

    // it's okay to omit cofactor check nQ = O in this case
    return TRUE;
}


int p256_AF_DH_PK_gen(p256_AF_pt *Q, p256_int *x, p256_int *k)
{
    // x = k % order
    p256int_mod_order(x, k, &p256_order);
    // Q = xP
    p256_AF_M_m_ary_smul(Q, x, &p256_base_point);
    return 0;
}


int p256_AF_DH_SS_gen(p256_AF_pt *R, p256_AF_pt *Q, p256_int *x)
{
    if(pub_key_validation(Q)==FALSE)
        return -1;

    p256_AF_M_m_ary_smul(R, x, Q);
    return 0;
}


int p256_ECDSA_sign(unsigned char *_r,   // sign value 1 (output)
                    unsigned char *_s,   // sign value 2 (output)
                    unsigned char *_k,   // random seed  (input)
                    unsigned char *_d,   // private key  (input)
                    unsigned char *_hm)  // message hash (input)
{
    p256_int r, s, k, d, hm, kinv;
    p256_AF_pt Q;

    bytes_to_p256int(&k, _k, 32);
    bytes_to_p256int(&d, _d, 32);
    bytes_to_p256int(&hm, _hm, 32);
    
    p256_AF_binary_smul(&Q, &k, &p256_base_point);
    p256int_mod_order(&r, &Q.x, &p256_order);
    
    if(r.len==0) return -1;

    p256int_inv_order(&kinv, &k, &p256_order);
    p256int_mul_order(&s, &r, &d, &p256_order);
    p256int_add_order(&s, &s, &hm, &p256_order);
    p256int_mul_order(&s, &s, &kinv, &p256_order);

    if(s.len==0) return -1;

    int rlen, slen;
    p256int_to_bytes(_r, &rlen, &r);
    p256int_to_bytes(_s, &slen, &s);
    
    for(int i=0;i<rlen;i++)
        _r[31-i] = _r[rlen-i-1];
    for(int i=0;i<32-rlen;i++)
        _r[i] = 0;

    for(int i=0;i<slen;i++)
        _s[31-i] = _s[slen-i-1];
    for(int i=0;i<32-slen;i++)
        _s[i] = 0;

    return 0;
}


int p256_ECDSA_verify(unsigned char *_r,   // sign value 1 (input)
                      unsigned char *_s,   // sign value 2 (input)
                      unsigned char *_hm,  // message hash (input)
                      unsigned char *_Qx,  // public key   (input)
                      unsigned char *_Qy)
{
    p256_int r, s, hm, u1, u2, v;
    p256_AF_pt Q, X, Xt;

    bytes_to_p256int(&Q.x, _Qx, 32);
    bytes_to_p256int(&Q.y, _Qy, 32);
    Q.at_infinity = 0;
    
    if(pub_key_validation(&Q)==FALSE)
        return FALSE;

    bytes_to_p256int(&r, _r, 32);
    bytes_to_p256int(&s, _s, 32);
    bytes_to_p256int(&hm, _hm, 32);

    if(r.len==0 || p256int_cmp(&r, &p256_order) >= 0) return FALSE;
    if(s.len==0 || p256int_cmp(&s, &p256_order) >= 0) return FALSE;
    
    p256int_inv_order(&s, &s, &p256_order);
    p256int_mul_order(&u1, &hm, &s, &p256_order);
    p256int_mul_order(&u2, &r, &s, &p256_order);
    p256_AF_binary_smul(&X, &u1, &p256_base_point);
    p256_AF_binary_smul(&Xt, &u2, &Q);
    p256_AF_add(&X, &X, &Xt);

    if(X.at_infinity) return FALSE;

    p256int_mod_order(&v, &X.x, &p256_order);

    if(p256int_cmp(&r, &v) != 0) return FALSE;

    return TRUE;
}