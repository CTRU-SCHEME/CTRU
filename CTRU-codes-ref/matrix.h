#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include "params.h"

#define CTRU_MATRIX_N (2 * CTRU_ALPHA) 

int compute_baseinv(int16_t b[CTRU_MATRIX_N], const int16_t a[CTRU_MATRIX_N], int16_t zeta);

#endif