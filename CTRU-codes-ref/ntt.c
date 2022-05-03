#include <stdint.h>
#include <stdio.h>
#include "params.h"
#include "reduce.h"
#include "ntt.h"
#include "matrix.h"

int16_t zetas[CTRU_NTT_N / 2] = {
    2286, 2424, 1886, 1937, 1715, 2644, 2833, 2753, 16, 2500, 200, 137, 2946, 2260, 2255, 594,
    1024, 978, 2429, 1854, 3166, 2065, 1548, 3342, 729, 2418, 470, 2569, 3395, 2412, 2682, 2494,
    1278, 910, 2147, 1004, 1129, 2692, 2013, 2537, 3349, 410, 2107, 1668, 2826, 795, 755, 1295,
    2, 2041, 25, 3042, 1100, 2482, 3379, 3369, 3116, 2895, 923, 3346, 2585, 2030, 2928, 1176,
    2286, 2845, 919, 2721, 2409, 2186, 728, 3126, 878, 636, 604, 1036, 2377, 643, 328, 2852,
    126, 674, 1575, 1511, 160, 801, 2000, 1370, 2716, 2621, 2837, 3378, 376, 3438, 1243, 1491,
    1008, 1935, 2229, 1717, 1280, 2951, 2172, 589, 986, 226, 1954, 2825, 3008, 3305, 3030, 1557,
    110, 1631, 1375, 1374, 1731, 1687, 2624, 2074, 1987, 203, 2367, 809, 438, 1026, 2018, 2454};

int16_t zetas_inv[CTRU_NTT_N / 2] = {
    1003, 1439, 2431, 3019, 2648, 1090, 3254, 1470, 1383, 833, 1770, 1726, 2083, 2082, 1826, 3347,
    1900, 427, 152, 449, 632, 1503, 3231, 2471, 2868, 1285, 506, 2177, 1740, 1228, 1522, 2449,
    1966, 2214, 19, 3081, 79, 620, 836, 741, 2087, 1457, 2656, 3297, 1946, 1882, 2783, 3331,
    605, 3129, 2814, 1080, 2421, 2853, 2821, 2579, 331, 2729, 1271, 1048, 736, 2538, 612, 1171,
    2281, 529, 1427, 872, 111, 2534, 562, 341, 88, 78, 975, 2357, 415, 3432, 1416, 3455,
    2162, 2702, 2662, 631, 1789, 1350, 3047, 108, 920, 1444, 765, 2328, 2453, 1310, 2547, 2179,
    963, 775, 1045, 62, 888, 2987, 1039, 2728, 115, 1909, 1392, 291, 1603, 1028, 2479, 2433,
    2863, 1202, 1197, 511, 3320, 3257, 957, 3441, 704, 624, 813, 1742, 1520, 1571, 1792, 0};

void ntt_256(int16_t b[CTRU_NTT_N], const int16_t a[CTRU_NTT_N])
{
  unsigned int len, start, j, k;
  int16_t t, zeta;

  for (j = 0; j < 128; ++j)
  {
    t = fqmul(zetas[1], a[j + 128]);
    b[j + 128] = a[j] + a[j + 128] - t;
    b[j] = a[j] + t;
  }

  k = 2;
  for (len = 64; len >= 2; len >>= 1)
  {
    for (start = 0; start < 256; start = j + len)
    {
      zeta = zetas[k++];
      for (j = start; j < start + len; ++j)
      {
        t = fqmul(zeta, b[j + len]);
        b[j + len] = (b[j] - t);
        b[j] = (b[j] + t);
      }
    }
  }
}

void invntt_256(int16_t b[CTRU_NTT_N], const int16_t a[CTRU_NTT_N])
{
  unsigned int start, len, j, k;
  int16_t t, zeta;
  const int16_t n1 = (1U << 25) % CTRU_Q, n2 = (1U << 26) % CTRU_Q;

  for (j = 0; j < 256; ++j)
    b[j] = a[j];

  k = 0;

  len = 2;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = (t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  len = 4;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = (t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  len = 8;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = barrett_reduce(t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  len = 16;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = (t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  len = 32;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = (t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  len = 64;
  for (start = 0; start < 256; start = j + len)
  {
    zeta = zetas_inv[k++];
    for (j = start; j < start + len; ++j)
    {
      t = b[j];
      b[j] = barrett_reduce(t + b[j + len]);
      b[j + len] = t - b[j + len];
      b[j + len] = fqmul(zeta, b[j + len]);
    }
  }

  for (j = 0; j < 128; ++j)
  {
    t = b[j] - b[j + 128];
    t = fqmul(zetas_inv[126], t);
    b[j] = b[j] + b[j + 128];
    b[j] = b[j] - t;
    b[j] = fqmul(n1, b[j]);
    b[j + 128] = fqmul(n2, t);
  }
}

#define CALC_D(a, b, x, y, d) (fqmul((a[x] + a[y]), (b[x] + b[y])) - d[x] - d[y])

void basemul(int16_t c[6], const int16_t a[6], const int16_t b[6], int16_t zeta)
{
  int i;
  int16_t d[2 * CTRU_ALPHA];

  for (i = 0; i < 2 * CTRU_ALPHA; i++)
    d[i] = fqmul(a[i], b[i]);

  c[0] = d[0] + fqmul((CALC_D(a, b, 1, 5, d) + CALC_D(a, b, 2, 4, d) + d[3]), zeta);
  c[1] = CALC_D(a, b, 0, 1, d) + fqmul((CALC_D(a, b, 2, 5, d) + CALC_D(a, b, 3, 4, d)), zeta);
  c[2] = barrett_reduce(CALC_D(a, b, 0, 2, d) + d[1] + fqmul((CALC_D(a, b, 3, 5, d) + d[4]), zeta));
  c[3] = barrett_reduce(CALC_D(a, b, 0, 3, d) + CALC_D(a, b, 1, 2, d) + fqmul(CALC_D(a, b, 4, 5, d), zeta));
  c[4] = barrett_reduce(CALC_D(a, b, 0, 4, d) + CALC_D(a, b, 1, 3, d) + d[2] + fqmul(d[5], zeta));
  c[5] = barrett_reduce(CALC_D(a, b, 0, 5, d) + CALC_D(a, b, 1, 4, d)) + CALC_D(a, b, 2, 3, d);
}

int baseinv(int16_t b[CTRU_MATRIX_N], const int16_t a[CTRU_MATRIX_N], int16_t zeta)
{
  return compute_baseinv(b, a, zeta);
}
