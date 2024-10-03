#ifndef NTT_H
#define NTT_H

#include <stdint.h>

extern int16_t zetas[384];
extern int16_t zetas_inv[384];

void ntt(int16_t a[768]);
void invntt(int16_t a[768]);

void basemul(int16_t c[2],
    const int16_t a[2],
    const int16_t b[2],
    int16_t zeta);

#endif
