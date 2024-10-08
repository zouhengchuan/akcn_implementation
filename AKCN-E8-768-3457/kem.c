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
    unsigned char m[PREFIX + SYMBYTES];
    unsigned char buf[2 * SYMBYTES];
    int i;

    randombytes(buf, SYMBYTES);

    shake256(m + PREFIX, SYMBYTES, buf, SYMBYTES);

    for (i = 0; i < PREFIX; i++)
        m[i] = pk[i];

    shake256(buf, 2 * SYMBYTES, m, PREFIX + SYMBYTES);

    shake256(buf + SYMBYTES, SYMBYTES, buf + SYMBYTES, SYMBYTES);

    cpapke_enc(ct, m + PREFIX, pk, buf + SYMBYTES);
    for (i = 0; i < SYMBYTES; i++){
        ss[i] = buf[i];
    }

    return 0;
}

int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk)
{
    int i, fail;
    unsigned char ct2[CCAKEM_CIPHERTEXTBYTES + SYMBYTES];
    unsigned char buf[2 * SYMBYTES];
    unsigned char m[PREFIX + SYMBYTES];
    const unsigned char* pk = sk + CPAPKE_SECRETKEYBYTES;

    cpapke_dec(m + PREFIX, ct, sk);

    for (i = 0; i < PREFIX; i++)
        m[i] = pk[i];
    
    shake256(buf, 2 * SYMBYTES, m, PREFIX + SYMBYTES);

    shake256(buf + SYMBYTES, SYMBYTES, buf + SYMBYTES, SYMBYTES);

    cpapke_enc(ct2, m + PREFIX, pk, buf + SYMBYTES);

    fail = verify(ct, ct2, CCAKEM_CIPHERTEXTBYTES);

    cmov(ss, buf, SYMBYTES, 1);

    for(i = 0; i < CCAKEM_CIPHERTEXTBYTES; ++i)
    {
        ct2[i] = ct[i];
    }
    for(i = 0; i < SYMBYTES; ++i)
    {
        ct2[i + CCAKEM_CIPHERTEXTBYTES] = sk[i + CPAPKE_SECRETKEYBYTES + CPAPKE_PUBLICKEYBYTES];
    }

    shake256(buf, SYMBYTES, ct2, SYMBYTES + CCAKEM_CIPHERTEXTBYTES);

    cmov(ss, buf, SYMBYTES, fail);

    return 0;
}
