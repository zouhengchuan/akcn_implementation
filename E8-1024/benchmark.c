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

#include "ds_benchmark.h"

#define BENCH_SUCCESS          0
#define BENCH_FAIL            -1

int
main()
{
    char                fn_req[32], fn_rsp[32];
    FILE                *fp_req, *fp_rsp;
    unsigned char       seed[48];
    unsigned char       entropy_input[48];
    unsigned char       ct[CRYPTO_CIPHERTEXTBYTES], ss[CRYPTO_BYTES], ss1[CRYPTO_BYTES];
    int                 count;
    int                 done;
    unsigned char       pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];

    randombytes(seed, 48);
    randombytes_init(seed, NULL, 256);

    PRINT_TIMER_HEADER
    DEFINE_TIMER_VARIABLES
    INITIALIZE_TIMER
    START_TIMER
    TIME_OPERATION_ITERATIONS(crypto_kem_keypair(pk, sk), "Gen", 1000)
    TIME_OPERATION_ITERATIONS(crypto_kem_enc(ct, ss, pk), "Enc", 1000)
    TIME_OPERATION_ITERATIONS(crypto_kem_dec(ss1, ct, sk), "Dec", 1000)
    STOP_TIMER
    FINALIZE_TIMER
    PRINT_TIMER_AVG("benchmark")
    PRINT_TIMER_FOOTER
    
    return BENCH_SUCCESS;
}

