#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"


typedef struct {
    int16_t coeffs[N];
} poly;

void poly_reduce(poly* a);
//void poly_freeze(poly* a);
//
void poly_add(poly* c, const poly* a, const poly* b);
void poly_sub(poly* r, const poly* a, const poly* b);

void poly_ntt(poly* a);
void poly_invntt(poly* a);
void poly_basemul(poly* c, const poly* a, const poly* b);

void poly_sample(poly* r, const unsigned char* seed, unsigned char nonce);
void poly_uniform(poly* a, const unsigned char* seed);

void poly_frombytes(poly* r, const unsigned char* a);
void poly_tobytes(unsigned char* r, const poly* p);
void poly_compress_y(uint8_t *r, poly *a);
void poly_decompress_y(poly *r, const uint8_t *a);
void poly_compress_v(uint8_t *r, poly *a);
void poly_decompress_v(poly *r, const uint8_t *a);
void poly_frommsg(poly* r, const unsigned char* msg);
void poly_tomsg(unsigned char* msg, const poly* x);

#endif
