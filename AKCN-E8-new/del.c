#include <string.h>
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "rng.h"
#include "fips202.h"
#include "verify.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define Q 3457

static uint16_t coeff_freeze(uint16_t x)
{
    uint16_t m, r;
    int16_t c;
    r = x % Q;

    m = r - Q;
    c = m;
    c >>= 15;
    r = m ^ ((r ^ m) & c);

    return r;
}

int main()
{
    int16_t a[2] = {2156, 555};
    printf("a0 = %02X\n", a[0]);
    printf("a1 = %02X\n", a[1]);

    uint16_t t[2];
    unsigned char r[3];

    t[0] = coeff_freeze(a[0]);
    t[1] = coeff_freeze(a[1]);
    printf("t0 = %02X\n", t[0]);
    printf("t1 = %02X\n", t[1]);

    r[0] = (t[0])      & 0xff;
    r[1] = (t[0] >> 8) | ((t[1] & 0x0f) << 4);
    r[2] = (t[1] >> 4);

    for (int i = 0; i < 3; i++){
        printf("r = %02X\n", r[i]);
    }

    int16_t b[2];
    b[0] = (r[0])      | (((uint16_t)r[1] & 0x0f) << 8);
    b[1] = (r[1] >> 4) | (((uint16_t)r[2]       ) << 4);
    printf("b0 = %02X\n", b[0]);
    printf("b1 = %02X\n", b[1]);
    
    return 0;
}
