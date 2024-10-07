#include <string.h>
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "rng.h"
#include "fips202.h"
#include "verify.h"

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk)
{
    int i;

    cpapke_keypair(pk, sk); /* First put the actual secret key into sk */
    sk += CPAPKE_SECRETKEYBYTES;

    for (i = 0; i < CPAPKE_PUBLICKEYBYTES; i++) /* Append the public key for re-encryption */
        sk[i] = pk[i];
    sk += CPAPKE_PUBLICKEYBYTES;

    randombytes(sk, SYMBYTES); /* Append the value z for pseudo-random output on reject */

    return 0;
}

int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk)
{
    unsigned char m[SYMBYTES + SSBYTES];
    unsigned char buf[SSBYTES + SYMBYTES];
    int i;

    randombytes(buf, SYMBYTES);

    shake256(m + SYMBYTES, SSBYTES, buf, SYMBYTES);
    for (i = 0; i < SYMBYTES; i++)
        m[i] = pk[i];

    shake256(buf, SSBYTES + SYMBYTES, m, SYMBYTES + SSBYTES);

    cpapke_enc(ct, m + SYMBYTES, pk, buf + SSBYTES);
    for (i = 0; i < SSBYTES; i++){
        ss[i] = buf[i];
    }

    return 0;
}

int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk)
{
    int i, fail;
    unsigned char ct2[CCAKEM_CIPHERTEXTBYTES];
    unsigned char buf[SSBYTES + SYMBYTES];
    unsigned char m[SYMBYTES + SSBYTES];
    unsigned char rj[CCAKEM_PUBLICKEYBYTES + SYMBYTES + CCAKEM_CIPHERTEXTBYTES];
    const unsigned char* pk = sk + CPAPKE_SECRETKEYBYTES;

    cpapke_dec(m + SYMBYTES, ct, sk);

    for (i = 0; i < SYMBYTES; i++)
        m[i] = pk[i];
    
    shake256(buf, SSBYTES + SYMBYTES, m, SYMBYTES + SSBYTES);

    cpapke_enc(ct2, m + SYMBYTES, pk, buf + SSBYTES);

    fail = verify(ct, ct2, CCAKEM_CIPHERTEXTBYTES);

    cmov(ss, buf, SSBYTES, 1);

    cmov(rj, pk, CCAKEM_PUBLICKEYBYTES, 1);
    cmov(rj, sk + CPAPKE_SECRETKEYBYTES + CPAPKE_PUBLICKEYBYTES, SSBYTES, 1);
    cmov(rj, ct, CCAKEM_CIPHERTEXTBYTES, 1);
    shake256(rj, SSBYTES, rj, CCAKEM_PUBLICKEYBYTES + SYMBYTES + CCAKEM_CIPHERTEXTBYTES);

    cmov(ss, rj, SSBYTES, fail);

    return 0;
}
