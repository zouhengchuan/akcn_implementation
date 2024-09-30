#include <string.h>
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "rng.h"
#include "fips202.h"
#include "verify.h"

/*************************************************
* Name:        crypto_kem_keypair
*
* Description: Generates public and private key
*              for CCA secure NewHope key encapsulation
*              mechanism
*
* Arguments:   - unsigned char *pk: pointer to output public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*              - unsigned char *sk: pointer to output private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_kem_keypair(unsigned char* pk, unsigned char* sk)
{
    size_t i;

    cpapke_keypair(pk, sk);                                                   /* First put the actual secret key into sk */
    sk += CPAPKE_SECRETKEYBYTES;

    for (i = 0; i < CPAPKE_PUBLICKEYBYTES; i++)                              /* Append the public key for re-encryption */
        sk[i] = pk[i];
    sk += CPAPKE_PUBLICKEYBYTES;

    shake256(sk, SYMBYTES, pk, CPAPKE_PUBLICKEYBYTES);        /* Append the hash of the public key */
    sk += SYMBYTES;

    randombytes(sk, SYMBYTES);                                        /* Append the value s for pseudo-random output on reject */

    return 0;
}

/*************************************************
* Name:        crypto_kem_enc
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - unsigned char *ct:       pointer to output cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *pk: pointer to input public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk)
{
    unsigned char k_coins_d[3 * SYMBYTES];                                                /* Will contain key, coins, qrom-hash */
    unsigned char buf[2 * SYMBYTES];
    int i;

    randombytes(buf, SYMBYTES);

    shake256(buf, SYMBYTES, buf, SYMBYTES);                                        /* Don't release system RNG output */
    shake256(buf + SYMBYTES, SYMBYTES, pk, CCAKEM_PUBLICKEYBYTES);        /* Multitarget countermeasure for coins + contributory KEM */
    shake256(k_coins_d, 3 * SYMBYTES, buf, 2 * SYMBYTES);

    cpapke_enc(ct, buf, pk, k_coins_d + SYMBYTES);                                        /* coins are in k_coins_d+SYMBYTES */

    for (i = 0; i < SYMBYTES; i++)
        ct[i + CPAPKE_CIPHERTEXTBYTES] = k_coins_d[i + 2 * SYMBYTES];                   /* copy Targhi-Unruh hash into ct */

    shake256(k_coins_d + SYMBYTES, SYMBYTES, ct, CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c) */
    shake256(ss, SYMBYTES, k_coins_d, 2 * SYMBYTES);                              /* hash concatenation of pre-k and h(c) to ss */
    return 0;
}

/*************************************************
* Name:        crypto_kem_dec
*
* Description: Generates shared secret for given
*              cipher text and private key
*
* Arguments:   - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *ct: pointer to input cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - const unsigned char *sk: pointer to input private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 for sucess or -1 for failure
*
* On failure, ss will contain a randomized value.
**************************************************/
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk)
{
    int i, fail;
    unsigned char ct_cmp[CCAKEM_CIPHERTEXTBYTES];
    unsigned char buf[2 * SYMBYTES];
    unsigned char k_coins_d[3 * SYMBYTES];                                                /* Will contain key, coins, qrom-hash */
    const unsigned char* pk = sk + CPAPKE_SECRETKEYBYTES;

    cpapke_dec(buf, ct, sk);

    for (i = 0; i < SYMBYTES; i++)                                                             /* Use hash of pk stored in sk */
        buf[SYMBYTES + i] = sk[CCAKEM_SECRETKEYBYTES - 2 * SYMBYTES + i];
    shake256(k_coins_d, 3 * SYMBYTES, buf, 2 * SYMBYTES);

    cpapke_enc(ct_cmp, buf, pk, k_coins_d + SYMBYTES);                                    /* coins are in k_coins_d+SYMBYTES */

    for (i = 0; i < SYMBYTES; i++)
        ct_cmp[i + CPAPKE_CIPHERTEXTBYTES] = k_coins_d[i + 2 * SYMBYTES];

    fail = verify(ct, ct_cmp, CCAKEM_CIPHERTEXTBYTES);

    shake256(k_coins_d + SYMBYTES, SYMBYTES, ct, CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c)  */
    cmov(k_coins_d, sk + CCAKEM_SECRETKEYBYTES - SYMBYTES, SYMBYTES, fail); /* Overwrite pre-k with z on re-encryption failure */
    shake256(ss, SYMBYTES, k_coins_d, 2 * SYMBYTES);                              /* hash concatenation of pre-k and h(c) to k */

    return 0;
}
