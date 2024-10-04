#include <stdint.h>
#include "params.h"
#include "fips202.h"
#include "reduce.h"
#include "ntt.h"
#include "poly.h"

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

void poly_reduce(poly* a)
{
    unsigned int i;
    for (i = 0; i < N; ++i)
        a->coeffs[i] = (int16_t)barrett_reduce(a->coeffs[i]);
}

void poly_add(poly* c, const poly* a, const poly* b)
{
    unsigned int i;
    for (i = 0; i < N; ++i)
        c->coeffs[i] = (a->coeffs[i] + b->coeffs[i] + 4 * Q) % Q;
}

void poly_sub(poly* r, const poly* a, const poly* b)
{
    int i;
    for (i = 0; i < N; i++)
        r->coeffs[i] = (a->coeffs[i] - b->coeffs[i] + 4 * Q) % Q;
}

void poly_tomont(poly *a)
{
const int16_t t = (MONT * MONT) % Q;
for (int i = 0; i < N; ++i)
    a->coeffs[i] = fqmul(a->coeffs[i], t);
}

void poly_ntt(poly* a)
{
    // poly_reduce(a);
    ntt(a->coeffs);
}

void poly_invntt(poly* a)
{
    // poly_reduce(a);
    invntt(a->coeffs);
}

void poly_basemul(poly* c, const poly* a, const poly* b)
{
    unsigned int i;
    for (i = 0; i < N / 2; ++i) {
        basemul(c->coeffs + 2 * i, a->coeffs + 2 * i, b->coeffs + 2 * i,
            zetas_base[i]);
    }
    // poly_reduce(c);
}

void poly_sample_3(poly* r, const unsigned char* seed, unsigned char nonce)
{
    int i;
    unsigned char a, b;
    unsigned char extseed[SYMBYTES + 1], buf[3 * N / 4];
    for (i = 0; i < SYMBYTES; i++)
        extseed[i] = seed[i];
    extseed[SYMBYTES] = nonce;
    shake256(buf, 3 * N / 4, extseed, SYMBYTES + 1);

    for (i = 0; i < N / 4; i++)
    {
        a = ((buf[3*i] >> 0) & 1) + ((buf[3*i] >> 1) & 1) + ((buf[3*i] >> 2) & 1);
        b = ((buf[3*i] >> 3) & 1) + ((buf[3*i] >> 4) & 1) + ((buf[3*i] >> 5) & 1);
        r->coeffs[4*i+0] = a + Q - b;

        a = ((buf[3*i]   >> 6) & 1) + ((buf[3*i]   >> 7) & 1) + ((buf[3*i+1] >> 0) & 1);
        b = ((buf[3*i+1] >> 1) & 1) + ((buf[3*i+1] >> 2) & 1) + ((buf[3*i+1] >> 3) & 1);
        r->coeffs[4*i+1] = a + Q - b;

        a = ((buf[3*i+1] >> 4) & 1) + ((buf[3*i+1] >> 5) & 1) + ((buf[3*i+1] >> 6) & 1);
        b = ((buf[3*i+1] >> 7) & 1) + ((buf[3*i+2] >> 0) & 1) + ((buf[3*i+2] >> 1) & 1);
        r->coeffs[4*i+2] = a + Q - b;

        a = ((buf[3*i+2] >> 2) & 1) + ((buf[3*i+2] >> 3) & 1) + ((buf[3*i+2] >> 4) & 1);
        b = ((buf[3*i+2] >> 5) & 1) + ((buf[3*i+2] >> 6) & 1) + ((buf[3*i+2] >> 7) & 1);
        r->coeffs[4*i+3] = a + Q - b;
    }
}

void poly_sample_2(poly* r, const unsigned char* seed, unsigned char nonce)
{
    int i;
    unsigned char a, b;
    unsigned char extseed[SYMBYTES + 1], buf[N / 2];
    for (i = 0; i < SYMBYTES; i++)
        extseed[i] = seed[i];
    extseed[SYMBYTES] = nonce;
    shake256(buf, N / 2, extseed, SYMBYTES + 1);

    for (i = 0; i < N / 2; i++)
    {
        a = ((buf[i] >> 0) & 1) + ((buf[i] >> 1) & 1);
        b = ((buf[i] >> 2) & 1) + ((buf[i] >> 3) & 1);
        r->coeffs[2*i+0] = a + Q - b;

        a = ((buf[i] >> 4) & 1) + ((buf[i] >> 5) & 1);
        b = ((buf[i] >> 6) & 1) + ((buf[i] >> 7) & 1);
        r->coeffs[2*i+1] = a + Q - b;
    }
}

void poly_uniform(poly* a, const unsigned char* seed)
{
    unsigned int ctr = 0;
	uint16_t val;
	uint64_t state[25]; // 块大小 1600 bit
	unsigned char buf[13*SHAKE128_RATE]; // 取得足够大
	unsigned char extseed[SYMBYTES + 1];
	int i, j;

	for (i = 0; i < SYMBYTES; i++)
		extseed[i] = seed[i];

	ctr = 0;
	j = 0; 
	extseed[SYMBYTES] =0; 
	shake128_absorb(state, extseed, SYMBYTES+1);
    shake128_squeezeblocks(buf,13,state);

	while (ctr < N && j < 13*SHAKE128_RATE)
	{
		val = (buf[j] | ((uint16_t)buf[j + 1] << 8));
		if (val < 16 * Q) // 2**(16 - logQ) * Q
			a->coeffs[ctr++] = barrett_reduce(val);
		j = j + 2;
	}
	while (ctr < N)
	{
		shake128_squeezeblocks(buf,1,state);
		for (j=0;j < SHAKE128_RATE;j=j+2) 
		{
			val = (buf[j] | ((uint16_t)buf[j + 1] << 8));
			if (val < 16 * Q)	
				a->coeffs[ctr++] = barrett_reduce(val);		
		 }
	}
}

void poly_frombytes(poly* r, const unsigned char* a)
{
    int i;
    for (i = 0; i < N / 2; i++)
    {
        r->coeffs[2*i+0] = (a[3*i+0])      | (((uint16_t)a[3*i+1] & 0x0f) << 8);
        r->coeffs[2*i+1] = (a[3*i+1] >> 4) | (((uint16_t)a[3*i+2]       ) << 4);
    }
}

void poly_tobytes(unsigned char* r, const poly* p)
{
    int i, j;
    uint16_t t[2];
    for (i = 0; i < N / 2; i++)
    {
        for(j = 0; j < 2; j++)
            t[j] = coeff_freeze(p->coeffs[2*i+j]);

        r[3*i+0] = (t[0])      & 0xff;
        r[3*i+1] = (t[0] >> 8) | ((t[1] & 0x0f) << 4);
        r[3*i+2] = (t[1] >> 4);
    }
}

void poly_compress_y(uint8_t *r, poly *a) // 压缩到 $2^{10}$
{
  uint16_t t[4];
  unsigned int i,j;
  poly_reduce(a);
  for(i=0;i<N/4;i++)
  {
    for(j=0;j<4;j++)
      t[j] = ((((uint32_t)a->coeffs[4*i+j] << 10) + Q/2) / Q) & 0x3ff; // $\frac{a_i * 2^{10}+Q/2}{Q}$ 相当于压缩到 $2^{10}$ 上, & 0x3ff 相当于模 $2^{10}$

    r[0] = (t[0] >> 0);
    r[1] = (t[0] >> 8) | (t[1] << 2);
    r[2] = (t[1] >> 6) | (t[2] << 4);
    r[3] = (t[2] >> 4) | (t[3] << 6);
    r[4] = (t[3] >> 2);
    r += 5;
  }
}

void poly_decompress_y(poly *r, const uint8_t *a)
{
  unsigned int i,j;
  uint16_t t[4];
  for(i=0;i<N/4;i++)
  {
    t[0] = (a[0] >> 0) | ((uint16_t)a[1] << 8);
    t[1] = (a[1] >> 2) | ((uint16_t)a[2] << 6);
    t[2] = (a[2] >> 4) | ((uint16_t)a[3] << 4);
    t[3] = (a[3] >> 6) | ((uint16_t)a[4] << 2);
    a += 5;

    for(j=0;j<4;j++)
      r->coeffs[4*i+j] = ((uint32_t)(t[j] & 0x3FF)*Q + 512) >> 10;
  }
}

void poly_compress_v(uint8_t *r, poly *a) // 压缩到 2^3
{
  uint8_t t[8];
  int i,j,k=0;
  poly_reduce(a);
  for(i=0;i<N;i+=8)
  {
    for(j=0;j<8;j++)
      t[j] = ((((uint32_t)a->coeffs[i+j] << 3) + Q/2) / Q) & 7;

    r[k]   =  t[0]       | (t[1] << 3) | (t[2] << 6);
    r[k+1] = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7);
    r[k+2] = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
    k += 3;
  }
}

void poly_decompress_v(poly *r, const uint8_t *a)
{
  int i;
  for(i=0;i<N;i+=8)
  {
    r->coeffs[i+0] =  (((a[0] & 7) * Q) + 4) >> 3;
    r->coeffs[i+1] = ((((a[0] >> 3) & 7) * Q) + 4) >> 3;
    r->coeffs[i+2] = ((((a[0] >> 6) | ((a[1] << 2) & 4)) * Q) + 4) >> 3;
    r->coeffs[i+3] = ((((a[1] >> 1) & 7) * Q) + 4) >> 3;
    r->coeffs[i+4] = ((((a[1] >> 4) & 7) * Q) + 4) >> 3;
    r->coeffs[i+5] = ((((a[1] >> 7) | ((a[2] << 1) & 6)) * Q) + 4) >> 3;
    r->coeffs[i+6] = ((((a[2] >> 2) & 7) * Q) + 4) >> 3;
    r->coeffs[i+7] = ((((a[2] >> 5)) * Q) + 4) >> 3;
    a += 3;
  }
}

static uint8_t encode_hamming(uint8_t m)
{
    uint8_t a0 = m & 1;
    uint8_t a1 = (m >> 1) & 1;
    uint8_t a2 = (m >> 2) & 1;
    uint8_t a3 = (m >> 3) & 1;
    return ((-a1) & 0x0F) ^ ((-a2) & 0x3c) ^ ((-a3) & 0xF0) ^ ((-a0) & 0x55);
}

static int32_t sqr(int32_t x) {
    return x*x;
}

static uint32_t abs_q(uint32_t x) {
    uint32_t mask = -(((Q - (x<<1))>>31) & 1);
    return (mask & (Q - x)) | ((~mask) & x); 
}

static uint8_t decode_D8_00(uint32_t *cost, uint32_t tmp_cost[8][2])
{
    uint8_t m[2] = {0, 0}, xor_sum = 0, res;
    uint32_t min_diff = ~0U>>2, r;
    uint32_t min_i = 0;
    int i;

    *cost = 0;
    for (i = 0;i < 4;i++) {
        uint32_t c[2];
        c[0] = tmp_cost[i<<1][0] + tmp_cost[i<<1|1][0];
        c[1] = tmp_cost[i<<1][1] + tmp_cost[i<<1|1][1];
        r = ((c[1] - c[0]) >> 31) & 1;
        m[0] |= r << i;
        xor_sum ^= r;
        *cost += c[r];

        uint32_t diff = c[r^1] - c[r];
        r = ((diff - min_diff) >> 31) & 1;
        min_diff = ((-r) & diff) | ((-(r^1)) & min_diff);
        min_i = ((-r) & i) | ((-(r^1)) & min_i);
    }
    m[1] = m[0] ^ (1<<min_i);
    res = m[xor_sum];
    *cost += (-xor_sum) & min_diff;
    return res;
}

static uint8_t decode_D8_10(uint32_t *cost, uint32_t tmp_cost[8][2])
{
    uint8_t m[2] = {0, 0}, xor_sum = 0, res;
    uint32_t min_diff = ~0U>>2, r;
    uint32_t min_i = 0;
    int i;

    *cost = 0;
    for (i = 0;i < 4;i++) {
        uint32_t c[2];
        c[0] = tmp_cost[i<<1][1] + tmp_cost[i<<1|1][0];
        c[1] = tmp_cost[i<<1][0] + tmp_cost[i<<1|1][1];
        r = ((c[1] - c[0]) >> 31) & 1;
        m[0] |= r << i;
        xor_sum ^= r;
        *cost += c[r];

        uint32_t diff = c[r^1] - c[r];
        r = ((diff - min_diff) >> 31) & 1;
        min_diff = ((-r) & diff) | ((-(r^1)) & min_diff);
        min_i = ((-r) & i) | ((-(r^1)) & min_i);
    }
    m[1] = m[0] ^ (1<<min_i);
    res = m[xor_sum];
    *cost += (-xor_sum) & min_diff;
    return res;
}

static uint32_t const_abs(int32_t x) {
    uint32_t mask = x >> 31;
    return (x ^ mask) - mask;
}

static uint8_t decode_E8(uint32_t vec[8])
{
    uint32_t cost[2], r;
    uint8_t m[2], res;
    uint32_t tmp_cost[8][2];
    int i;

    for (i = 0;i < 8;i++) {
        tmp_cost[i][0] = sqr(abs_q(vec[i]));
        tmp_cost[i][1] = sqr(const_abs(vec[i] - (Q>>1)));
    }

    m[0] = decode_D8_00(cost+0, tmp_cost);
    m[1] = decode_D8_10(cost+1, tmp_cost);
    r = ((cost[1] - cost[0]) >> 31) & 1;

    res = m[r];
    res = ((((res ^ (res << 1)) & 0x3) | ((res >> 1) & 4)) << 1) | r;

    return res;
}

void poly_frommsg(poly *r, const unsigned char *msg)
{
    int i;
    uint16_t currbit;
    uint8_t exkey[128];

    for (i = 0;i < 128;i++) {
        uint8_t m = (msg[i>>1] >> ((i&1)<<2)) & 0xF;
        exkey[i] = encode_hamming(m);
    }
    for (i = 0;i < N;i++) {
        currbit = (exkey[i>>3] >> (i & 0x7)) & 1;
        r->coeffs[i] = (Q>>1) & (-currbit);
    }
}

void poly_tomsg(unsigned char *msg, const poly *x)
{
  unsigned int i, j, k;

  for (i = 0;i < 32;i++) {
    msg[i] = 0;
  }

  for(i = 0;i < 64;i++) {
    uint32_t tmp_v[8];
    for (j = 0;j < 8;j++) {
      k = (i<<3) | j;
      tmp_v[j] = (uint32_t)barrett_reduce(x->coeffs[k]);
    }
    msg[i>>1] |= decode_E8(tmp_v) << ((i & 1) << 2);
  }
}
