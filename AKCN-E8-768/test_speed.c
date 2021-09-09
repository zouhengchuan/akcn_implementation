#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "params.h"
#include "cpapke.h"
#include "poly.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 10000

uint64_t t[NTESTS];
__attribute__((aligned(32)))
uint8_t seed[SYMBYTES] = {0};
__attribute__((aligned(32)))
uint8_t publicseed[SYMBYTES+SYMBYTES] = {0};

int main()
{
  unsigned int i;
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key[CRYPTO_BYTES] = {0};
  poly s;

  printf("PUBLICKEYBYTES: %d\n", CRYPTO_PUBLICKEYBYTES);
  printf("SECRETKEYBYTES: %d\n", CRYPTO_SECRETKEYBYTES);
  printf("CIPHERTEXTBYTES: %d\n", CRYPTO_CIPHERTEXTBYTES);
  printf("CRYPTO_BYTES: %d\n", CRYPTO_BYTES);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    poly_ntt(&s);
  }
  print_results("poly_ntt: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    poly_invntt(&s);
  }
  print_results("poly_invntt: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_keypair(pk, sk);
  }
  print_results("keypair: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_enc(ct, key, pk);
  }
  print_results("encaps: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    crypto_kem_dec(key, ct, sk);
  }
  print_results("decaps: ", t, NTESTS);

  return 0;
}
