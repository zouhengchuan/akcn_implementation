#include <stdint.h>
#include <stdio.h>

#define Q 3457
#define QINV 12929 // q^(-1) mod 2^16

int16_t barrett_reduce(int16_t a)
{
    int32_t t;
    const int32_t v = (1U << 26) / Q + 1;

    t = v * a;
    t >>= 26;
    t *= Q;
    return a - t;
}

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
    int32_t a;
    for (int i = -Q*Q+1; i < Q*Q; i++){
        a = (int32_t) i;
        a = montgomery_reduce(a);
        if (a < -1910 || a > 1910){
            printf("a = %d, i = %d\n", a, i);
        }
    }
    return 0;
}
