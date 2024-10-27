#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

uint32_t factorial(uint32_t num)
{
	uint32_t result = 1;
	for (uint32_t i = 2; i <= num; i++)
	{
		result = (uint32_t)((uint64_t)result * i % INT32_MAX);
	}
	return result;
}

uint32_t find_max(uint32_t start, uint32_t end)
{
	uint32_t maxi = 1;
	uint32_t fact = factorial(start);
	for (int32_t i = start; i <= end; i++)
	{
		if (i != start)
		{
			fact = (uint32_t)((uint64_t)fact * i % INT32_MAX);
		}
		if (maxi < fact)
		{
			maxi = fact;
		}
	}
	return maxi;
}

uint8_t find_length(uint32_t num)
{
	uint8_t len = 1;
	while (num >= 10)
	{
		num /= 10;
		len++;
	}
	return len;
}

void create_border(uint8_t left, uint8_t right)
{
	printf("+");
	for (uint8_t i = 0; i < left; i++)
	{
		printf("-");
	}
	printf("+");
	for (uint8_t i = 0; i < right; i++)
	{
		printf("-");
	}
	printf("+\n");
}

void mid_table(int8_t align, uint8_t left_column_length, uint8_t right_column_length)
{
	switch (align)
	{
	case -1:
		printf("| %-*s | %-*s|\n", left_column_length - 2, "n", right_column_length - 1, "n!");
		break;
	case 1:
		printf("| %*s | %*s |\n", left_column_length - 2, "n", right_column_length - 2, "n!");
		break;
	case 0:
		printf("| %*s", (((left_column_length - 3) >> 1) + ((left_column_length - 3) & 1)) + 1, "n");
		printf("%*s", ((left_column_length - 1) >> 1) + 2, " | ");
		printf("%*s", (((right_column_length - 4) >> 1) + ((right_column_length - 4) & 1)) + 2, "n!");
		printf("%*s", ((right_column_length - 4) >> 1) + 3, " |\n");
		break;
	default:
		return;
	}
}

void create_table_top_full(uint8_t left_column_length, uint8_t right_column_length, uint8_t align)
{
	create_border(left_column_length, right_column_length);
	mid_table(align, left_column_length, right_column_length);
	create_border(left_column_length, right_column_length);
}

void create_row(uint16_t n, uint32_t factorial, uint8_t left_width, uint8_t right_width, int8_t align)
{
	uint8_t n_length = find_length(n);
	uint8_t fact_length = find_length(factorial);
	switch (align)
	{
	case -1:
		printf("| %-*u| %-*u|\n", left_width - 1, n, right_width - 1, factorial);
		break;
	case 1:
		printf("|%*u |%*u |\n", left_width - 1, n, right_width - 1, factorial);
		break;
	case 0:
		printf("| %*u", (((left_width - n_length) >> 1) + ((left_width - n_length) & 1)) + n_length - 1, n);
		printf("%*s", ((left_width - n_length) >> 1) + 2, " | ");
		printf("%*u", (((right_width - fact_length) >> 1) + ((right_width - fact_length) & 1)) + fact_length - 1, factorial);
		printf("%*s", ((right_width - fact_length) >> 1) + 2, " |\n");
		break;
	default:
		return;
	}
}

void fill_table(uint32_t start, uint32_t end, uint32_t fact, uint8_t left, uint8_t right, uint8_t align)
{
	for (int32_t i = start; i <= end; i++)
	{
		if (i != start)
		{
			fact = (uint32_t)((uint64_t)fact * i % INT32_MAX);
		}
		create_row(i, fact, left, right, align);
	}
}

int main(void)
{
	int32_t n_start, n_end;
	uint8_t align;
	int32_t max_idx;
	scanf("%" SCNd32 "%" SCNd32 "%" SCNu8, &n_start, &n_end, &align);
	if (n_start < 0 || n_end < 0)
	{
		fprintf(stderr, "Error");
		return 1;
	}
	max_idx = n_start > n_end ? n_start : n_end;
	uint8_t left_column_length = find_length(max_idx) + 2;
	if (n_start <= n_end)
	{
		uint32_t maxi = find_max(n_start, n_end);
		uint8_t right_column_length = find_length(maxi) + 2 >= 4 ? find_length(maxi) + 2 : 4;
		create_table_top_full(left_column_length, right_column_length, align);
		uint32_t fact = factorial(n_start);
		fill_table(n_start, n_end, fact, left_column_length, right_column_length, align);
		create_border(left_column_length, right_column_length);
	}
	else
	{
		uint32_t temp1 = find_max(n_start, UINT16_MAX);
		uint32_t temp2 = find_max(0, n_end);
		uint32_t maxi = temp1 > temp2 ? temp1 : temp2;
		uint8_t right_column_length = find_length(maxi) + 2 >= 4 ? find_length(maxi) + 2 : 4;
		create_table_top_full(left_column_length, right_column_length, align);
		uint32_t fact = factorial(n_start);
		fill_table(n_start, UINT16_MAX, fact, left_column_length, right_column_length, align);
		fact = factorial(0);
		fill_table(0, n_end, fact, left_column_length, right_column_length, align);
		create_border(left_column_length, right_column_length);
	}
	return 0;
}
