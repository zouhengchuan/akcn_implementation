#include <stdio.h>
//#include "api.h"
#include "poly.h"
#include "rng.h"
#include "fips202.h"
#include "cpapke.h"

/*************************************************
* Name:        encode_pk
*
* Description: Serialize the public key as concatenation of the
*              serialization of the polynomial pk and the public seed
*              used to generete the polynomial a.
*
* Arguments:   unsigned char *r:          pointer to the output serialized public key
*              const poly *pk:            pointer to the input public-key polynomial
*              const unsigned char *seed: pointer to the input public seed
**************************************************/
static void encode_pk(unsigned char* r, const poly* pk, const unsigned char* seed)
{
    int i;
    poly_tobytes(r, pk);
    for (i = 0; i < SYMBYTES; i++)
        r[POLYBYTES + i] = seed[i];
}

/*************************************************
* Name:        decode_pk
*
* Description: De-serialize the public key; inverse of encode_pk
*
* Arguments:   poly *pk:               pointer to output public-key polynomial
*              unsigned char *seed:    pointer to output public seed
*              const unsigned char *r: pointer to input byte array
**************************************************/
static void decode_pk(poly* pk, unsigned char* seed, const unsigned char* r)
{
    int i;
    poly_frombytes(pk, r);
    for (i = 0; i < SYMBYTES; i++)
        seed[i] = r[POLYBYTES + i];
}

/*************************************************
* Name:        encode_c
*
* Description: Serialize the ciphertext as concatenation of the
*              serialization of the polynomial b and serialization
*              of the compressed polynomial v
*
* Arguments:   - unsigned char *r: pointer to the output serialized ciphertext
*              - const poly *b:    pointer to the input polynomial b
*              - const poly *v:    pointer to the input polynomial v
**************************************************/
static void encode_c(unsigned char* r, poly* b, poly* v)
{
    poly_compress_y(r, b);
    poly_compress_v(r+POLYYBYTES, v);
}

/*************************************************
* Name:        decode_c
*
* Description: de-serialize the ciphertext; inverse of encode_c
*
* Arguments:   - poly *b:                pointer to output polynomial b
*              - poly *v:                pointer to output polynomial v
*              - const unsigned char *r: pointer to input byte array
**************************************************/
static void decode_c(poly* b, poly* v, const unsigned char* r)
{
    poly_decompress_y(b, r);
    poly_decompress_v(v, r+POLYYBYTES);
}

/*************************************************
* Name:        gen_a
*
* Description: Deterministically generate public polynomial a from seed
*
* Arguments:   - poly *a:                   pointer to output polynomial a
*              - const unsigned char *seed: pointer to input seed
**************************************************/
void gen_a(poly* a, const unsigned char* seed)
{
    poly_uniform(a, seed);
}


/*************************************************
* Name:        cpapke_keypair
*
* Description: Generates public and private key
*              for the CPA public-key encryption scheme underlying
*              the NewHope KEMs
*
* Arguments:   - unsigned char *pk: pointer to output public key
*              - unsigned char *sk: pointer to output private key
**************************************************/
void cpapke_keypair(unsigned char* pk,
    unsigned char* sk)
{
    poly ahat, ehat, ahat_shat, bhat, shat;
    unsigned char z[2 * SYMBYTES];
    unsigned char* publicseed = z;
    unsigned char* noiseseed = z + SYMBYTES;

    randombytes(z, SYMBYTES);
    shake256(z, 2 * SYMBYTES, z, SYMBYTES);

    gen_a(&ahat, publicseed); // ntt形式

    // poly_invntt(&ahat);
    // poly_ntt(&ahat); // 相当于乘 R

    poly_sample_2(&shat, noiseseed, 0);
    poly_ntt(&shat);

    poly_sample_3(&ehat, noiseseed, 1);
    poly_ntt(&ehat);

    poly_tomont(&ahat);
    poly_basemul(&ahat_shat, &shat, &ahat); // 恰好消掉 R
    poly_add(&bhat, &ehat, &ahat_shat);

    poly_tobytes(sk, &shat); // ntt形式
    encode_pk(pk, &bhat, publicseed); // ntt形式
}

/*************************************************
* Name:        cpapke_enc
*
* Description: Encryption function of
*              the CPA public-key encryption scheme underlying
*              the NewHope KEMs
*
* Arguments:   - unsigned char *c:          pointer to output ciphertext
*              - const unsigned char *m:    pointer to input message (of length SYMBYTES bytes)
*              - const unsigned char *pk:   pointer to input public key
*              - const unsigned char *coin: pointer to input random coins used as seed
*                                           to deterministically generate all randomness
**************************************************/
void cpapke_enc(unsigned char* c,
    const unsigned char* m,
    const unsigned char* pk,
    const unsigned char* coin)
{
    poly sprime, eprime, vprime, ahat, bhat, eprimeprime, uhat, v;
    unsigned char publicseed[SYMBYTES];

    poly_frommsg(&v, m);

    decode_pk(&bhat, publicseed, pk);
    
    gen_a(&ahat, publicseed);

    poly_sample_2(&sprime, coin, 0);
    poly_sample_3(&eprime, coin, 1);
    poly_sample_3(&eprimeprime, coin, 2);

    poly_ntt(&sprime);

    poly_basemul(&uhat, &ahat, &sprime);
    poly_invntt(&uhat); // 正好消掉base里的R^{-1}
    poly_add(&uhat, &uhat, &eprime);

    poly_basemul(&vprime, &bhat, &sprime);
    poly_invntt(&vprime); // 正好消掉base里的R^{-1}

    poly_add(&vprime, &vprime, &eprimeprime);
    poly_add(&vprime, &vprime, &v); // add message

    encode_c(c, &uhat, &vprime);
}


/*************************************************
* Name:        cpapke_dec
*
* Description: Decryption function of
*              the CPA public-key encryption scheme underlying
*              the NewHope KEMs
*
* Arguments:   - unsigned char *m:        pointer to output decrypted message
*              - const unsigned char *c:  pointer to input ciphertext
*              - const unsigned char *sk: pointer to input secret key
**************************************************/
void cpapke_dec(unsigned char* m,
    const unsigned char* c,
    const unsigned char* sk)
{
    poly vprime, uhat, tmp, shat;

    poly_frombytes(&shat, sk);

    decode_c(&uhat, &vprime, c);

    poly_ntt(&uhat);
    poly_basemul(&tmp, &shat, &uhat);
    poly_invntt(&tmp); // 正好消掉base里的R^{-1}

    poly_sub(&tmp, &tmp, &vprime);

    poly_tomsg(m, &tmp);
}
