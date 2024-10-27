/*
 *
 * Created by rgctlln on 20.04.2023
 *
 */

#include "fftw3_functions.h"

#include "return_codes.h"

#include <math.h>
#include <stdint.h>

static void
	init_all(fftw_complex **complex_in1, fftw_complex **complex_in2, fftw_complex **complex_out1, fftw_complex **complex_out2, size_t size, uint8_t *error_code)
{
	size_t total_size = 4 * size;
	fftw_complex *all_complex = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * total_size);
	if (all_complex == NULL)
	{
		fputs("ERROR: Unable to allocate memory for FFTW arrays", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
		return;
	}
	*complex_in1 = all_complex;
	*complex_in2 = all_complex + size;
	*complex_out1 = all_complex + 2 * size;
	*complex_out2 = all_complex + 3 * size;
}

static void clean(fftw_plan plan_forward1, fftw_plan plan_forward2, fftw_plan plan_backward, fftw_complex(*complex_in1), double *correlation)
{
	if (correlation != NULL)
	{
		fftw_free(correlation);
	}
	if (complex_in1 != NULL)
	{
		fftw_free(complex_in1);
	}
	if (plan_forward1)
	{
		fftw_destroy_plan(plan_forward1);
	}
	if (plan_forward2)
	{
		fftw_destroy_plan(plan_forward2);
	}
	if (plan_backward)
	{
		fftw_destroy_plan(plan_backward);
	}
}

int32_t correlation_fftw(double *x, double *y, size_t size, uint8_t *error_code)
{
	int32_t max_index = -1;
	double *correlation = (double *)fftw_malloc(sizeof(double) * size);
	if (!correlation)
	{
		fputs("ERROR: Unable to allocate memory", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
		return -1;
	}
	for (size_t i = 0; i < size; i++)
	{
		correlation[i] = x[i];
	}
	fftw_complex *complex_in1 = NULL, *complex_in2 = NULL;
	fftw_complex *complex_out1 = NULL, *complex_out2 = NULL;
	init_all(&complex_in1, &complex_in2, &complex_out1, &complex_out2, size, error_code);
	fftw_plan plan_forward1 = fftw_plan_dft_r2c_1d((int32_t)size, correlation, complex_out1, FFTW_ESTIMATE);
	fftw_plan plan_forward2 = fftw_plan_dft_r2c_1d((int32_t)size, y, complex_out2, FFTW_ESTIMATE);
	fftw_plan plan_backward = fftw_plan_dft_c2r_1d((int32_t)size, complex_in1, correlation, FFTW_ESTIMATE);
	if (!complex_in1 || !complex_in2 || !complex_out1 || !complex_out2)
	{
		goto cleanup;
	}
	if (!plan_forward1 || !plan_forward2 || !plan_backward)
	{
		fputs("ERROR: Unable to allocate FFTW Plan", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
		goto cleanup;
	}
	fftw_execute(plan_forward1);
	fftw_execute(plan_forward2);
	for (size_t i = 0; i <= size / 2; i++)
	{
		complex_in1[i][0] = complex_out1[i][0] * complex_out2[i][0] + complex_out1[i][1] * complex_out2[i][1];
		complex_in1[i][1] = complex_out1[i][1] * complex_out2[i][0] - complex_out1[i][0] * complex_out2[i][1];
	}
	fftw_execute(plan_backward);
	for (size_t i = 0; i < size; i++)
	{
		correlation[i] /= (int32_t)size;
	}
	double max_value = -INFINITY;
	for (size_t i = 0; i < size; i++)
	{
		if (correlation[i] > max_value)
		{
			max_value = correlation[i];
			max_index = i;
		}
	}
	if (max_index > size / 2)
	{
		max_index -= (int32_t)size;
	}
	goto cleanup;
cleanup:
	clean(plan_forward1, plan_forward2, plan_backward, complex_in1, correlation);
	return max_index;
}
// Перевод на новую строку, просто гит ее затирает