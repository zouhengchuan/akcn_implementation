#include <stdint.h>
#include <stdio.h>

#define Q 7681

int16_t barrett_reduce(int16_t a)
{
    int32_t t;
    const int32_t v = (1U << 26) / Q + 1;

    t = v * a;
    t >>= 26;
    t *= Q;
    return a - t;
}

int main()
{
    int16_t   c=barrett_reduce(32767 + 32767);
    printf("%d", c);
    return 0;
}
