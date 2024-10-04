#include <stdint.h>
#include "params.h"
#include "reduce.h"

int16_t montgomery_reduce(int32_t a)
{
    int16_t t;

    t = (int16_t)a * QINV;
    t = (a - (int32_t)t * Q) >> 16;
    return t;
}

int16_t barrett_reduce(int16_t a)
{
    int32_t t;

    t = v * a;
    t >>= 26;
    t *= Q;
    return a - t;
}

int16_t fqcsubq(int16_t a) {
    a += (a >> 15)& Q;
    a -= Q;
    a += (a >> 15)& Q;
    return a;
}

int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}
