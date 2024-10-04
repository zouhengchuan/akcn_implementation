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
#define QINV 12929 // q^(-1) mod 2^16
#define Q 3457

int16_t montgomery_reduce(int32_t a)
{
    int32_t t;
    int16_t u;

    u = a * QINV;
    t = (int32_t)u * Q;
    t = a - t;
    t >>= 16;
    return t;
}

int main()
{
    int32_t a = 3456;
    int16_t b;
    int ctr = 0;
    for (int i = 1; i < 1 * 3457; i++){
        b = montgomery_reduce(i);
        if (b != (b + 3457) % 3457){
            printf("b = %d ", b);
            printf("i = %d\n", i);
            ctr++;
        }
    }
    printf("%d\n", ctr);
    
    return 0;
}
