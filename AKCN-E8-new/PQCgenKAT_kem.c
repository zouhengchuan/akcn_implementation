
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
#include "rng.h"
#include "api.h"
#include "poly.h"
#include "print.h"

#define	MAX_MARKER_LEN		50
#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

int		FindMarker(FILE *infile, const char *marker);
int		ReadHex(FILE *infile, unsigned char *A, int Length, char *str);
void	fprintBstr(FILE *fp, char *S, unsigned char *A, unsigned long long L);

void schoolbook(poly* c, const poly* a, const poly* b);

int
main()
{
    // unsigned char    pk[CPAKEM_PUBLICKEYBYTES], sk[CPAKEM_SECRETKEYBYTES];
    // unsigned char    ct[CPAKEM_CIPHERTEXTBYTES];
    // unsigned char    z[SYMBYTES], buf[2 * SYMBYTES], buff[2 * SYMBYTES];

    // cpapke_keypair(pk, sk);

    // randombytes(buf, SYMBYTES);
    // shake256(buf, 2 * SYMBYTES, buf, SYMBYTES);
    // for (int i = 0; i < SYMBYTES; i++)
    //     printf("%02X", buf[i]);
    // printf("\n");

    // cpapke_enc(ct, buf, pk, z);

    // cpapke_dec(buff, ct, sk);
    
    // for (int i = 0; i < SYMBYTES; i++)
    //     printf("%02X", buff[i]);
    // printf("\n");



    char                fn_req[32], fn_rsp[32];
    FILE                *fp_req, *fp_rsp;
    unsigned char       seed[48];
    unsigned char       entropy_input[48];
    unsigned char       ct[CRYPTO_CIPHERTEXTBYTES], ss[CRYPTO_BYTES], ss1[CRYPTO_BYTES];
    int                 count;
    int                 done;
    unsigned char       pk[CRYPTO_PUBLICKEYBYTES], sk[CRYPTO_SECRETKEYBYTES];
    int                 ret_val;

    // Create the REQUEST file
    sprintf(fn_req, "PQCkemKAT_%d.req", CRYPTO_SECRETKEYBYTES);
    if ( (fp_req = fopen(fn_req, "w")) == NULL ) {
        printf("Couldn't open <%s> for write\n", fn_req);
        return KAT_FILE_OPEN_ERROR;
    }
    sprintf(fn_rsp, "PQCkemKAT_%d.rsp", CRYPTO_SECRETKEYBYTES);
    if ( (fp_rsp = fopen(fn_rsp, "w")) == NULL ) {
        printf("Couldn't open <%s> for write\n", fn_rsp);
        return KAT_FILE_OPEN_ERROR;
    }

    for (int i=0; i<48; i++)
        entropy_input[i] = i;

    randombytes_init(entropy_input, NULL, 256);
    for (int i=0; i<100; i++) {
        fprintf(fp_req, "count = %d\n", i);
        randombytes(seed, 48);
        fprintBstr(fp_req, "seed = ", seed, 48);
        fprintf(fp_req, "pk =\n");
        fprintf(fp_req, "sk =\n");
        fprintf(fp_req, "ct =\n");
        fprintf(fp_req, "ss =\n\n");
    }
    fclose(fp_req);

    //Create the RESPONSE file based on what's in the REQUEST file
    if ( (fp_req = fopen(fn_req, "r")) == NULL ) {
        printf("Couldn't open <%s> for read\n", fn_req);
        return KAT_FILE_OPEN_ERROR;
    }

    fprintf(fp_rsp, "# %s\n\n", CRYPTO_ALGNAME);
    done = 0;
    do {
        if ( FindMarker(fp_req, "count = ") )
            fscanf(fp_req, "%d", &count);
        else {
            done = 1;
            break;
        }
        fprintf(fp_rsp, "count = %d\n", count);

        if ( !ReadHex(fp_req, seed, 48, "seed = ") ) {
            printf("ERROR: unable to read 'seed' from <%s>\n", fn_req);
            return KAT_DATA_ERROR;
        }
        fprintBstr(fp_rsp, "seed = ", seed, 48);

        randombytes_init(seed, NULL, 256);

        // Generate the public/private keypair
        if ( (ret_val = crypto_kem_keypair(pk, sk)) != 0) {
            printf("crypto_kem_keypair returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }
        fprintBstr(fp_rsp, "pk = ", pk, CRYPTO_PUBLICKEYBYTES);
        fprintBstr(fp_rsp, "sk = ", sk, CRYPTO_SECRETKEYBYTES);

        if ( (ret_val = crypto_kem_enc(ct, ss, pk)) != 0) {
            printf("crypto_kem_enc returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }
        fprintBstr(fp_rsp, "ct = ", ct, CRYPTO_CIPHERTEXTBYTES);
        fprintBstr(fp_rsp, "ss = ", ss, CRYPTO_BYTES);

        fprintf(fp_rsp, "\n");

        if ( (ret_val = crypto_kem_dec(ss1, ct, sk)) != 0) {
            printf("crypto_kem_dec returned <%d>\n", ret_val);
            return KAT_CRYPTO_FAILURE;
        }

        if ( memcmp(ss, ss1, CRYPTO_BYTES) ) {
            printf("crypto_kem_dec returned bad 'ss' value\n");
            fprintBstr(fp_rsp, "ss1 = ", ss1, CRYPTO_BYTES);
            return KAT_CRYPTO_FAILURE;
        }

    } while ( !done );

    fclose(fp_req);
    fclose(fp_rsp);

    printf("sueeess!!!");
    return KAT_SUCCESS;

    
    
    
    // ntt is true
    // poly ahat, bhat, chat, c_school;

    // unsigned char z[2 * SYMBYTES];
    // unsigned char* seed = z;
    // unsigned char* seed2 = z + SYMBYTES;

    // for (int i = 0; i < 9; i++){
    //     randombytes(z, 2 * SYMBYTES);
    // }

    // // ahat.coeffs[0] = 2010;
    // // for (int i = 1; i < N; i++){
    // //     ahat.coeffs[i] = 2000;
    // // }
    // // ahat.coeffs[2 * 1] = 1;
    // // ahat.coeffs[2 * 1 + 1] = 1;
    // poly_uniform(&ahat, seed);
    // poly_reduce(&ahat);

    // // bhat.coeffs[0] = 1;
    // // for (int i = 1; i < N; i++){
    // //     bhat.coeffs[i] = 1;
    // // }
    // poly_uniform(&bhat, seed2);
    // poly_reduce(&bhat);
    // print_poly_all(&bhat, "b");

    // schoolbook(&c_school, &ahat, &bhat);
    // print_poly_all(&c_school,"c_school");
    // poly_ntt(&c_school);
    // print_poly_all(&c_school,"ntt c_school");

    // poly_ntt(&ahat);
    // print_poly_all(&ahat,"ntt_a");

    // poly_ntt(&bhat);
    // print_poly_all(&bhat,"ntt_b");

    // poly_basemul(&chat, &ahat, &bhat);
    // print_poly_all(&chat,"ntt_a * ntt_b");
    // poly_invntt(&chat);
    // poly_reduce(&chat);

    // print_poly_all(&chat,"chat");
    // poly_ntt(&chat);
    // print_poly_all(&chat,"ntt chat");

    // for (int i = 0; i < N; i++){
    //     if (c_school.coeffs[i] != chat.coeffs[i]){
    //         printf("fail in i = %d\n", i);
    //         return;
    //     };
    // }
    // printf("success\n");
}



//
// ALLOW TO READ HEXADECIMAL ENTRY (KEYS, DATA, TEXT, etc.)
//
//
// ALLOW TO READ HEXADECIMAL ENTRY (KEYS, DATA, TEXT, etc.)
//
int
FindMarker(FILE *infile, const char *marker)
{
	char	line[MAX_MARKER_LEN];
	int		i, len;
	int curr_line;

	len = (int)strlen(marker);
	if ( len > MAX_MARKER_LEN-1 )
		len = MAX_MARKER_LEN-1;

	for ( i=0; i<len; i++ )
	  {
	    curr_line = fgetc(infile);
	    line[i] = curr_line;
	    if (curr_line == EOF )
	      return 0;
	  }
	line[len] = '\0';

	while ( 1 ) {
		if ( !strncmp(line, marker, len) )
			return 1;

		for ( i=0; i<len-1; i++ )
			line[i] = line[i+1];
		curr_line = fgetc(infile);
		line[len-1] = curr_line;
		if (curr_line == EOF )
		    return 0;
		line[len] = '\0';
	}

	// shouldn't get here
	return 0;
}

//
// ALLOW TO READ HEXADECIMAL ENTRY (KEYS, DATA, TEXT, etc.)
//
int
ReadHex(FILE *infile, unsigned char *A, int Length, char *str)
{
	int			i, ch, started;
	unsigned char	ich;

	if ( Length == 0 ) {
		A[0] = 0x00;
		return 1;
	}
	memset(A, 0x00, Length);
	started = 0;
	if ( FindMarker(infile, str) )
		while ( (ch = fgetc(infile)) != EOF ) {
			if ( !isxdigit(ch) ) {
				if ( !started ) {
					if ( ch == '\n' )
						break;
					else
						continue;
				}
				else
					break;
			}
			started = 1;
			if ( (ch >= '0') && (ch <= '9') )
				ich = ch - '0';
			else if ( (ch >= 'A') && (ch <= 'F') )
				ich = ch - 'A' + 10;
			else if ( (ch >= 'a') && (ch <= 'f') )
				ich = ch - 'a' + 10;
            else // shouldn't ever get here
                ich = 0;

			for ( i=0; i<Length-1; i++ )
				A[i] = (A[i] << 4) | (A[i+1] >> 4);
			A[Length-1] = (A[Length-1] << 4) | ich;
		}
	else
		return 0;

	return 1;
}

void
fprintBstr(FILE *fp, char *S, unsigned char *A, unsigned long long L)
{
	unsigned long long  i;

	fprintf(fp, "%s", S);

	for ( i=0; i<L; i++ )
		fprintf(fp, "%02X", A[i]);

	if ( L == 0 )
		fprintf(fp, "00");

	fprintf(fp, "\n");
}


void
schoolbook(poly* c, const poly* a, const poly* b)
{
    int16_t temp;
    for (int i = 0; i < N; i++){
        c -> coeffs[i] = 0;
    }
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (i + j < N){
                temp = fqmul(a -> coeffs[i], b -> coeffs[j]);
                temp = fqmul(temp, 867);
                c -> coeffs[i + j] = barrett_reduce(c -> coeffs[i + j] + temp);
            }else if (i + j < 3 * N / 2){
                temp = fqmul(a -> coeffs[i], b -> coeffs[j]);
                temp = fqmul(temp, 867);
                c -> coeffs[i + j - N] = barrett_reduce(c -> coeffs[i + j - N] - temp);
                c -> coeffs[i + j - N / 2] = barrett_reduce(c -> coeffs[i + j - N / 2] + temp);
            }else{
                temp = fqmul(a -> coeffs[i], b -> coeffs[j]);
                temp = fqmul(temp, 867);
                c -> coeffs[i + j - 3 * N / 2] = barrett_reduce(c -> coeffs[i + j - 3 * N / 2] - temp);
            } 
        }
    }
    poly_reduce(c);
}