/*
 *
 * Created by rgctlln on 19.04.2023
 *
 */

#ifndef CT_C24_LW_LIBRARIES_RGCTLLN_FFTW3_FUNCTIONS_H
#define CT_C24_LW_LIBRARIES_RGCTLLN_FFTW3_FUNCTIONS_H

#include <fftw3.h>
#include <stdint.h>

static void clean(fftw_plan plan_forward1, fftw_plan plan_forward2, fftw_plan plan_backward, fftw_complex(*complex_in1), double *correlation);

int32_t correlation_fftw(double *x, double *y, size_t size, uint8_t *error_code);

static void
	init_all(fftw_complex **complex_in1, fftw_complex **complex_in2, fftw_complex **complex_out1, fftw_complex **complex_out2, size_t size, uint8_t *error_code);

#endif
// Перевод на новую строку, просто гит ее затирает