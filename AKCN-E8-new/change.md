# 改动

## params.h
- N = 768, Q = 3457, logQ = 12

## poly.c
- poly_tobytes 每四个系数(12bit)变成6个 char
- poly_frombytes 每6个 char 变成四个系数
- poly_uniform 不知道怎么改
- poly_sample 改成 B_3  B_2 两种
- poly_basemul 根据 ntt 修改


## reduce.h
- QINV, MONT

## ntt.c
- zeta[256], zetas_inv[256] ？
- 具体实现改一下, 6层基-2, 一层基-3