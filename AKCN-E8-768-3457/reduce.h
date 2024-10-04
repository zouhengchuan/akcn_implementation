#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

#define MONT 3310 // 2^16 % Q (-1:2775)
#define QINV 12929 // q^(-1) mod 2^16
#define v 19412 // v = (1U << 26) / Q + 1

int16_t montgomery_reduce(int32_t a);
int16_t barrett_reduce(int16_t a);
//int16_t fqred16(int16_t a);
int16_t fqcsubq(int16_t a);
int16_t fqmul(int16_t a, int16_t b);
//int16_t fqinv(int16_t a);
//int16_t fquniform(void);

#endif
