#include <stdio.h>
#include <string.h>
#include "params.h"
#include "poly.h"

void print_poly_all(poly* r, const char* str)
{
    printf("%s:\n", str);
    for (int i = 0; i < N; i++)
        printf("%d,", r->coeffs[i]);
    printf("\n\n");
}

void print_str(char* a, int len, const char* str)
{
    printf("%s:\n", str);
    for (int i = 0; i < len; i++)
        printf("%02X", a[i]);
    printf("\n\n");
}
