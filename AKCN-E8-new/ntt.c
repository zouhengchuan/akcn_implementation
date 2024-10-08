#include <stdint.h>
#include "params.h"
#include "reduce.h"
#include "ntt.h"

int16_t zetas[256] = { 1033, 886, 1937, 1886, 624, 704, 1742, 813, 1197, 2946, 2863, 2255, 2500, 3441, 137, 3257, 2569, 2987, 2728, 1039, 963, 2682, 3395, 2412, 2429, 1854, 978, 2433, 1909, 115, 1392, 3166, 78, 88, 975, 1100, 2041, 3455, 3042, 3432, 2281, 2928, 2585, 2030, 341, 562, 2534, 111, 2328, 765, 1444, 920, 2147, 1004, 910, 2179, 2662, 2826, 2162, 755, 1668, 1350, 108, 3047, 152, 3008, 1900, 3030, 2825, 1503, 2471, 3231, 2229, 1717, 1935, 2449, 1285, 2868, 506, 1280, 1003, 2018, 438, 1026, 1470, 3254, 1090, 2648, 1631, 3347, 1374, 2082, 1770, 1731, 1383, 2624, 636, 2579, 1036, 2853, 2814, 2377, 605, 328, 1048, 1271, 2729, 331, 919, 2721, 2845, 1171, 3378, 620, 741, 836, 1966, 1243, 376, 3438, 1457, 2087, 2656, 160, 674, 3331, 1511, 1882, 1434, 3139, 647, 1086, 3155, 2902, 640, 2939, 2645, 614, 164, 1034, 2953, 2050, 221, 761, 3089, 125, 3344, 1269, 10, 316, 2314, 3291, 1515, 2933, 1414, 2507, 1894, 390, 3381, 364, 2555, 1227, 80, 2528, 513, 1000, 2553, 3238, 3394, 2849, 1324, 3120, 1749, 2722, 941, 2771, 2594, 1768, 2882, 2572, 418, 1455, 1312, 1358, 1101, 913, 1719, 1774, 1041, 2474, 1663, 2770, 3044, 2160, 3302, 2016, 2247, 3248, 23, 2801, 3363, 1178, 1756, 869, 2445, 1208, 2282, 897, 1456, 1760, 898, 2795, 2215, 854, 915, 1258, 1972, 1472, 2071, 452, 1224, 3417, 451, 1115, 713, 406, 435, 3375, 1277, 252, 270, 1618, 1602, 151, 2878, 3137, 1948, 1405, 2740, 159, 709, 971, 2275, 2750, 2705, 2510, 220, 38, 153, 3452, 2217, 2300, 691, 1785, 184, 1666 };

int16_t zetas_inv[256] = { 1975, 1791, 2363, 1672, 3374, 1157, 158, 5, 182, 3419, 195, 947, 2982, 707, 3195, 2486, 2581, 3298, 543, 2052, 3198, 320, 1451, 3306, 2109, 1839, 1025, 3205, 517, 82, 307, 3051, 2793, 2342, 1264, 40, 1619, 3005, 500, 1985, 3114, 2199, 1361, 2603, 1560, 662, 3153, 1697, 1385, 2560, 1237, 2249, 887, 2588, 2185, 2279, 679, 656, 2456, 209, 1286, 1441, 884, 1297, 2350, 687, 2024, 983, 3402, 1683, 188, 2544, 3411, 2099, 2420, 2002, 310, 885, 826, 1689, 1627, 686, 2484, 735, 1661, 337, 545, 608, 2772, 219, 2970, 2457, 1009, 929, 1328, 2230, 3017, 3093, 1504, 3067, 2364, 950, 2039, 524, 2480, 166, 3151, 3141, 2075, 2188, 2964, 3332, 2917, 2696, 903, 1407, 2587, 2423, 2031, 2843, 1158, 518, 253, 555, 3018, 2371, 1752, 318, 1575, 1946, 126, 2783, 3297, 801, 1370, 2000, 19, 3081, 2214, 1491, 2621, 2716, 2837, 79, 2286, 612, 736, 2538, 3126, 728, 2186, 2409, 3129, 2852, 1080, 643, 604, 2421, 878, 2821, 833, 2074, 1726, 1687, 1375, 2083, 110, 1826, 809, 2367, 203, 1987, 2431, 3019, 1439, 2454, 2177, 2951, 589, 2172, 1008, 1522, 1740, 1228, 226, 986, 1954, 632, 427, 1557, 449, 3305, 410, 3349, 2107, 1789, 2702, 1295, 631, 795, 1278, 2547, 2453, 1310, 2537, 2013, 2692, 1129, 3346, 923, 2895, 3116, 1427, 872, 529, 1176, 25, 415, 2, 1416, 2357, 2482, 3369, 3379, 291, 2065, 3342, 1548, 1024, 2479, 1603, 1028, 1045, 62, 775, 2494, 2418, 729, 470, 888, 200, 3320, 16, 957, 1202, 594, 511, 2260, 2644, 1715, 2753, 2833, 1571, 1520, 1665, 2571 };

void basemul(int16_t c[2],
    const int16_t a[2],
    const int16_t b[2],
    int16_t zeta)
{
    c[0] = fqmul(a[1], b[1]);
    c[0] = fqmul(c[0], zeta);
    c[0] += fqmul(a[0], b[0]);

    c[1] = fqmul(a[1], b[0]);
    c[1] += fqmul(a[0], b[1]);
}

/********TFNTT_ntt********/
static void ntt_384(int16_t a[384]) {
	unsigned int start, j, k;
	int16_t t, t_1, s, s_1, zeta, rho = zetas[0];

	for (j = 0; j < 192; ++j) {
		t = fqmul(zetas[1], a[j + 192]);
		a[j + 192] = a[j] + a[j + 192] - t;
		a[j] = a[j] + t;
	}

	k = 2;

    for (start = 0; start < 384; start = j + 96) {
		zeta = zetas[k++];
		for (j = start; j < start + 96; ++j) {
			t = fqmul(zeta, a[j + 96]);
			a[j + 96] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 48) {
		zeta = zetas[k++];
		for (j = start; j < start + 48; ++j) {
			t = fqmul(zeta, a[j + 48]);
            a[j + 48] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 24) {
		zeta = zetas[k++];
		for (j = start; j < start + 24; ++j) {
			t = fqmul(zeta, a[j + 24]);
            a[j + 24] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 12) {
		zeta = zetas[k++];
		for (j = start; j < start + 12; ++j) {
			t = fqmul(zeta, a[j + 12]);
			a[j + 12] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 6) {
		zeta = zetas[k++];
		for (j = start; j < start + 6; ++j) {
			t = fqmul(zeta, a[j + 6]);
            a[j + 6] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 3) {
		zeta = zetas[k++];
		for (j = start; j < start + 3; ++j) {
			t = fqmul(zeta, a[j + 3]);
            a[j + 3] = barrett_reduce(a[j] - t);
			a[j] = barrett_reduce(a[j] + t);
		}
	}
    for (start = 0; start < 384; start = j + 2) {
		zeta = zetas[k++];
		for (j = start; j < start + 1; ++j) {
			t = fqmul(zeta, a[j + 1]);
			t_1 = fqmul(rho, t);
			s = fqmul(zeta, a[j + 2]);
			s = fqmul(zeta, s);
			s_1 = fqmul(rho, s);
			a[j + 1] = barrett_reduce(a[j] + t_1 - s - s_1);
			a[j + 2] = barrett_reduce(a[j] + s_1 - t - t_1);
			a[j] = barrett_reduce(a[j] + t + s);
		}
	}
}

void ntt(int16_t a[768])
{
	int i;
	int16_t a0[384], a1[384];

	for (i = 0; i < 384; i++)
	{
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1];
	}
	ntt_384(a0);
	ntt_384(a1);

	for (i = 0; i < 384; i++)
	{
		a[2 * i] = a0[i];
		a[2 * i + 1] = a1[i];
	}

}



/********TFNTT_invntt********/
static void invntt_256(int16_t a[384]) {
	unsigned int start, j, k;
	int16_t t, t_1, s, s_1, zeta, rho = zetas_inv[255];

	k = 0;

    for (start = 0; start < 384; start = j + 2) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 1; ++j) {
			t = a[j];
			t_1 = a[j + 1];
			s = fqmul(rho, a[j + 1]);
			s_1 = fqmul(rho, a[j + 2]);

            a[j] = t + t_1 + a[j + 2];

			a[j + 1] = t + s - s_1 - a[j + 2];
			a[j + 1] = fqmul(zeta, a[j + 1]);

			a[j + 2] = t - t_1 - s + s_1;
			a[j + 2] = fqmul(zeta, a[j + 2]);
			a[j + 2] = fqmul(zeta, a[j + 2]);
		}
	}
    for (start = 0; start < 384; start = j + 3) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 3; ++j) {
			t = a[j];
			a[j] = barrett_reduce(t + a[j + 3]);
			a[j + 3] = t - a[j + 3];
			a[j + 3] = fqmul(zeta, a[j + 3]);
		}
	}
    for (start = 0; start < 384; start = j + 6) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 6; ++j) {
			t = a[j];
            a[j] = t + a[j + 6];
			a[j + 6] = t - a[j + 6];
			a[j + 6] = fqmul(zeta, a[j + 6]);
		}
	}
    for (start = 0; start < 384; start = j + 12) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 12; ++j) {
			t = a[j];
			a[j] = barrett_reduce(t + a[j + 12]);
			a[j + 12] = t - a[j + 12];
			a[j + 12] = fqmul(zeta, a[j + 12]);
		}
	}
    for (start = 0; start < 384; start = j + 24) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 24; ++j) {
			t = a[j];
            a[j] = t + a[j + 24];
			a[j + 24] = t - a[j + 24];
			a[j + 24] = fqmul(zeta, a[j + 24]);
		}
	}
    for (start = 0; start < 384; start = j + 48) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 48; ++j) {
			t = a[j];
			a[j] = barrett_reduce(t + a[j + 48]);
			a[j + 48] = t - a[j + 48];
			a[j + 48] = fqmul(zeta, a[j + 48]);
		}
	}
    for (start = 0; start < 384; start = j + 96) {
		zeta = zetas_inv[k++];
		for (j = start; j < start + 96; ++j) {
			t = a[j];
            a[j] = t + a[j + 96];
			a[j + 96] = t - a[j + 96];
			a[j + 96] = fqmul(zeta, a[j + 96]);
		}
	}

	for (j = 0; j < 192; ++j) {
		t = a[j] - a[j + 192];
		t = fqmul(zetas_inv[254], t);
		a[j] = a[j] + a[j + 192];
		a[j] = a[j] - t;
		a[j] = fqmul(2568, a[j]); // 2^32 * 384^{-1} mod Q
		a[j + 192] = fqmul(1679, t); // 2^32 * 192^{-1} mod Q
	}
}

void invntt(int16_t a[768]) // 蒙哥马利域上的intt，同时退出蒙哥马利域
{
	int i;
	int16_t a0[384], a1[384];

	for (i = 0; i < 384; i++)
	{
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1];
	}
	invntt_256(a0);
	invntt_256(a1);

	for (i = 0; i < 384; i++)
	{
		a[2 * i] = a0[i];
		a[2 * i + 1] = a1[i];
	}
}
