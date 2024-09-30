#ifndef PARAMS_H
#define PARAMS_H

#define N 768
#define Q 3457
#define LOGQ 12
#define COMPRESSEDY 10
#define COMPRESSEDV 3

#define SYMBYTES 32
#define SSBYTES  48
#define POLYBYTES ((LOGQ*N)/8)  // 1152
#define POLYYBYTES    ((COMPRESSEDY*N)/8) // 960
#define POLYVBYTES    ((COMPRESSEDV*N)/8) // 288

#define CPAPKE_PUBLICKEYBYTES  (POLYBYTES + SYMBYTES) // pk + seed
#define CPAPKE_SECRETKEYBYTES  (POLYBYTES) // sk
#define CPAPKE_CIPHERTEXTBYTES (POLYYBYTES + POLYVBYTES) // y + v

#define CPAKEM_PUBLICKEYBYTES CPAPKE_PUBLICKEYBYTES
#define CPAKEM_SECRETKEYBYTES CPAPKE_SECRETKEYBYTES
#define CPAKEM_CIPHERTEXTBYTES CPAPKE_CIPHERTEXTBYTES

#define CCAKEM_PUBLICKEYBYTES CPAPKE_PUBLICKEYBYTES  // pk + seed
#define CCAKEM_SECRETKEYBYTES (CPAPKE_SECRETKEYBYTES + CPAPKE_PUBLICKEYBYTES + 2*SYMBYTES) // sk + pk + H(pk) + s
#define CCAKEM_CIPHERTEXTBYTES (CPAPKE_CIPHERTEXTBYTES + SYMBYTES) // y + v + k

#endif
