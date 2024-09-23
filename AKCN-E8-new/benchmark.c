//
//  PQCgenKAT_kem.c
//
//  Created by Bassham, Lawrence E (Fed) on 8/29/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "rng.h"
#include "api.h"
#include "cpapke.h"
#include "poly.h"
#include "fips202.h"

#include "ds_benchmark.h"

#define BENCH_SUCCESS          0
#define BENCH_FAIL            -1

int
main()
{
    unsigned char       seed[48];
    unsigned char       ct[CRYPTO_CIPHERTEXTBYTES], ss[CRYPTO_BYTES], ss1[CRYPTO_BYTES];
    unsigned char       pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];
    poly f,g,h;
    // poly512 a, b,c ,d, e, j;

    randombytes(seed, 48);
    randombytes_init(seed, NULL, 256);

    poly_sample(&f, seed, 0);
    poly_sample(&g, seed, 1);

    printf("\n==================================================  UnifiedHope Ref 768  ===================================================\n");
    PRINT_TIMER_HEADER
    DEFINE_TIMER_VARIABLES
    INITIALIZE_TIMER
    START_TIMER
    TIME_OPERATION_ITERATIONS(crypto_kem_keypair(pk, sk), "Gen", 1000)
    TIME_OPERATION_ITERATIONS(crypto_kem_enc(ct, ss, pk), "Enc", 1000)
    TIME_OPERATION_ITERATIONS(crypto_kem_dec(ss1, ct, sk), "Dec", 1000)
    printf("------------------------------ | ----------:| --------------:| ---------------:| ----------:| ----------------:| ----------:\n");
    //TIME_OPERATION_ITERATIONS(hntt_mul(&h, &f, &g), "Hntt", 1000)
    TIME_OPERATION_ITERATIONS(poly_ntt(&f), "Ntt", 1000)
    TIME_OPERATION_ITERATIONS(poly_invntt(&f), "Invntt", 1000)
    TIME_OPERATION_ITERATIONS(poly_basemul(&h, &f, &g), "poly_basemul", 1000)
    // TIME_OPERATION_ITERATIONS(poly_reduce(&a), "Poly Reduce n=512", 1000)
    //TIME_OPERATION_ITERATIONS(combine(&f, &a, &b), "Combine", 1000)
    printf("------------------------------ | ----------:| --------------:| ---------------:| ----------:| ----------------:| ----------:\n");
    TIME_OPERATION_ITERATIONS(poly_sample(&f, seed, 0), "poly_sample", 1000)
    TIME_OPERATION_ITERATIONS(poly_uniform(&f, seed), "poly_uniform", 1000)
    TIME_OPERATION_ITERATIONS(gen_a(&f, seed), "Gen_a", 1000)
    STOP_TIMER
    FINALIZE_TIMER
    PRINT_TIMER_AVG("benchmark")
    PRINT_TIMER_FOOTER

    return BENCH_SUCCESS;
}
