#include <stdio.h>
#include <stdint.h>
#include "params.h"
#include "matrix.h"
#include "reduce.h"

static void gen_coeff_matrix(int16_t matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], const int16_t coeffs[CTRU_MATRIX_N], const int16_t zeta)
{
    int16_t i, j, k;
    for (k = 0; k < CTRU_MATRIX_N; k++)
    {
        for (j = 0; j < CTRU_MATRIX_N; j++)
        {
            for (i = 0; i < CTRU_MATRIX_N; i++)
            {
                if ((i + j) % CTRU_MATRIX_N == k)
                {
                    matrix[k * CTRU_MATRIX_N + j] = coeffs[i];
                    if ((i + j) >= CTRU_MATRIX_N)
                    {
                        matrix[k * CTRU_MATRIX_N + j] = fqmul(matrix[k * CTRU_MATRIX_N + j], zeta);
                    }
                    continue;
                }
            }
        }
    }
}

static void replace_column(int16_t output_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t input_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t i)
{
    int16_t j, k = 0;
    for (j = 0; j < CTRU_MATRIX_N; j++)
    {
        if (j == i)
        {
            output_matrix[k] = 1;
            k++;
            continue;
        }
        output_matrix[k] = input_matrix[j];
        k++;
    }
    for (j = CTRU_MATRIX_N; j < CTRU_MATRIX_N * CTRU_MATRIX_N; j++)
    {
        if ((j % CTRU_MATRIX_N) == i)
        {
            output_matrix[k] = 0;
            k++;
            continue;
        }
        output_matrix[k] = input_matrix[j];
        k++;
    }
}

static void gen_child_matrix(int16_t output_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t input_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t m, int16_t i)
{
    int16_t j, k = 0;
    for (j = m; j < m * m; j++)
    {
        if ((j % m) == i)
            continue;
        output_matrix[k] = input_matrix[j];
        k++;
    }
}

static int16_t compute_det(int16_t matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t m)
{
    int16_t det = 0, sign = 1;
    int16_t child[CTRU_MATRIX_N * CTRU_MATRIX_N];

    for (int16_t i = 0; i < m; i++)
    {
        gen_child_matrix(child, matrix, m, i);
        if ((((int16_t)sign) >> 15) & 1)
            det -= fqmul(matrix[i], compute_det(child, m - 1));
        else
            det += fqmul(matrix[i], compute_det(child, m - 1));
        sign = -sign;
    }
    if (m == 0)
        det = 1;
    return det;
}

static int16_t compute_det_of_replace_matrix(int16_t replace_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], int16_t i)
{
    int16_t child[CTRU_MATRIX_N * CTRU_MATRIX_N];
    gen_child_matrix(child, replace_matrix, CTRU_MATRIX_N, i);
    return compute_det(child, CTRU_MATRIX_N - 1);
}

int compute_baseinv(int16_t b[CTRU_MATRIX_N], const int16_t a[CTRU_MATRIX_N], int16_t zeta)
{
    int16_t matrix[CTRU_MATRIX_N * CTRU_MATRIX_N], replace_matrix[CTRU_MATRIX_N * CTRU_MATRIX_N];
    int16_t i, det = 0, sign = 1;
    int r;

    gen_coeff_matrix(matrix, a, zeta);

    for (i = 0; i < CTRU_MATRIX_N; i++)
    {
        replace_column(replace_matrix, matrix, i);
        if ((((int16_t)sign) >> 15) & 1)
            b[i] = -compute_det_of_replace_matrix(replace_matrix, i);
        else
            b[i] = compute_det_of_replace_matrix(replace_matrix, i);
        sign = -sign;
    }
    for (i = 1; i < CTRU_MATRIX_N; i++)
        det += fqmul(a[CTRU_MATRIX_N - i], b[i]);
    det = fqmul(det, zeta);
    det += fqmul(a[0], b[0]);

    det = fqinv(det);

    for (i = 0; i < CTRU_MATRIX_N; i++)
        b[i] = fqmul(b[i], det);

    r = (uint16_t)det;
    r = (uint32_t)(-r) >> 31;
    return r - 1;
}
