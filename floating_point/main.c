#include "return_codes.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#define EXP_F 127
#define EXP_H 15
#define MANTISSA_MASK_F 0x7FFFFF
#define MANTISSA_MASK_H 0x3FF

typedef struct floatingPointNumber
{
	uint8_t sign;
	int32_t exponent;
	uint64_t mantissa;
} floatingPointNumber;

void init(floatingPointNumber *float_num, int64_t num, char format)
{
	if (format == 'f')
	{
		float_num->sign = (num >> 31) & 1;
		float_num->exponent = ((num >> 23) & 0xFF) - 127;
		float_num->mantissa = num & 0x7FFFFF;
	}
	else if (format == 'h')
	{
		float_num->sign = (num >> 15) & 1;
		float_num->exponent = ((num >> 10) & 0x1F) - 15;
		float_num->mantissa = num & 0x3FF;
	}
}

void print_zero(char format, uint8_t result_sign)
{
	if (format == 'f')
	{
		printf("%s0x0.000000p+0", result_sign ? "-" : "");
	}
	else
	{
		printf("%s0x0.000p+0", result_sign ? "-" : "");
	}
}

int check_zero(floatingPointNumber num)
{
	if (num.exponent == -127 && num.mantissa == 0)
	{
		return num.sign ? -1 : 1;
	}
	return 0;
}

bool is_nan(floatingPointNumber num, char format)
{
	if (format == 'f')
	{
		return (num.exponent == 128) && (num.mantissa != 0);
	}
	else if (format == 'h')
	{
		return (num.exponent == 31) && (num.mantissa != 0);
	}
	return false;
}

int check_inf(floatingPointNumber num, char format)
{
	if ((format == 'f' && num.exponent == -127 && num.mantissa != 0) || (format == 'f' && num.exponent == 128) ||
		(format == 'h' && num.exponent == -30))
	{
		return num.sign ? -1 : 1;
	}
	return 0;
}

void adjust_exponent_and_mantissa(int32_t *expToAdjust, int32_t targetExp, uint64_t *mant, uint32_t *move)
{
	uint64_t cnst = 1;
	while (*expToAdjust != targetExp)
	{
		*move += (*mant & 0x1) * cnst;
		*mant >>= 1;
		(*expToAdjust)++;
		cnst *= 2;
	}
}

int8_t is_normalized(int32_t exp, char format)
{
	uint16_t cnst = format == 'f' ? 255 : 31;
	if (exp == cnst)	// 31
	{
		return -1;
	}
	return exp == 0 ? 0 : 1;
}

int16_t check_mantissa_bits(uint64_t mantissa, uint8_t sign)
{
	if (mantissa == 0)
	{
		return sign == 1 ? -1 : 1;
	}
	return 0;
}

void normalize(uint64_t *mantissa, int32_t *exp, uint32_t *move, char format, uint16_t *cnt)
{
	*cnt = 0;
	uint64_t cnst = 1;
	int16_t val = format == 'f' ? -127 : -16;
	uint16_t shift = format == 'f' ? 23 : 10;
	if (*exp == val)
	{
		*exp = val + 1;
	}
	if (*mantissa >> shift == 0)
	{
		while ((*mantissa >> shift) != 1)
		{
			*mantissa <<= 1;
			(*exp)--;
		}
		return;
	}
	else
	{
		while ((*mantissa >> shift) != 1)
		{
			*move += (*mantissa & 0x1) * cnst;
			*mantissa >>= 1;
			(*cnt)++;
			(*exp)++;
			cnst *= 2;
		}
	}
}

void align_exponents(int32_t *exp1, int32_t *exp2, uint64_t *mant1, uint64_t *mant2, uint32_t *move)
{
	if (*exp1 > *exp2)
	{
		adjust_exponent_and_mantissa(exp2, *exp1, mant2, move);
	}
	else if (*exp1 < *exp2)
	{
		adjust_exponent_and_mantissa(exp1, *exp2, mant1, move);
	}
}

int16_t checker(int32_t exp, uint64_t mantissa, uint8_t sign, char format)
{
	if (is_normalized(exp, format) == -1)
	{
		int16_t check = check_mantissa_bits(mantissa, sign);
		return check;
	}
	return 2;
}

void multiply(floatingPointNumber *float_num, uint8_t sign1, uint8_t sign2, int32_t exp1, int32_t exp2, uint64_t mant1, uint64_t mant2, char format)
{
	if (format == 'f')
	{
		mant1 |= 1ULL << 23;
		mant2 |= 1ULL << 23;
	}
	else
	{
		mant1 |= 1 << 10;
		mant2 |= 1 << 10;
	}
	uint64_t result = mant1 * mant2;
	int newExp = exp1 + exp2;
	int shift;
	if (format == 'f')
	{
		if (result >= (1ULL << 47))
		{
			shift = 24;
			newExp++;
		}
		else
		{
			shift = 23;
		}
		uint64_t extra_bits = result & ((1ULL << shift) - 1);
		result >>= shift;
		bool should_round_up = extra_bits > (1ULL << (shift - 1));
		if (should_round_up)
		{
			result++;
		}
		float_num->mantissa = result & ((1ULL << 23) - 1);
	}
	else if (format == 'h')
	{
		result >>= 10;
		if (result >= (1 << 11))
		{
			result >>= 1;
			newExp++;
		}
		float_num->mantissa = result & ((1 << 10) - 1);
	}
	float_num->sign = sign1 ^ sign2;
	float_num->exponent = newExp;
}

bool handle_multiplication_special_cases(floatingPointNumber num1, floatingPointNumber num2, char format)
{
	bool num1_is_inf = check_inf(num1, format) == 1;
	bool num2_is_inf = check_inf(num2, format) == 1;
	bool num1_is_zero = check_zero(num1) != 0;
	bool num2_is_zero = check_zero(num2) != 0;
	bool num1_is_neg_inf = check_inf(num1, format) == -1;
	bool num2_is_neg_inf = check_inf(num2, format) == -1;

	if (((num1_is_inf || num1_is_neg_inf) && num2_is_zero) || (num1_is_zero && (num2_is_inf || num2_is_neg_inf)))
	{
		printf("nan");
		return true;
	}
	else if (num1_is_inf && num2_is_inf)
	{
		printf(num1.sign == num2.sign ? "inf" : "-inf");
		return true;
	}
	else if (num1_is_zero || num2_is_zero)
	{
		uint8_t result_sign = num1_is_zero ? num1.sign : num2.sign;
		print_zero(format, result_sign);
		return true;
	}
	else if (num1_is_inf || num2_is_inf)
	{
		printf("inf");
		return true;
	}
	else if (num1_is_neg_inf || num2_is_neg_inf)
	{
		printf("-inf");
		return true;
	}
	return false;
}

void add(floatingPointNumber *float_num, uint8_t sign1, uint8_t sign2, int32_t exp1_res, int32_t exp2_res, uint64_t mant1_res, uint64_t mant2_res, uint32_t *move)
{
	align_exponents(&exp1_res, &exp2_res, &mant1_res, &mant2_res, move);
	float_num->exponent = exp1_res >= exp2_res ? exp1_res : exp2_res;
	if (sign1 == sign2)
	{
		float_num->sign = sign1;
		float_num->mantissa = mant1_res + mant2_res;
	}
	else
	{
		if (mant1_res >= mant2_res)
		{
			float_num->sign = sign1;
			float_num->mantissa = mant1_res - mant2_res;
		}
		else
		{
			float_num->sign = sign2;
			float_num->mantissa = mant2_res - mant1_res;
		}
	}
}

bool handle_addition_special_cases(floatingPointNumber num1, floatingPointNumber num2, char format)
{
	bool num1_is_inf = check_inf(num1, format) == 1;
	bool num2_is_inf = check_inf(num2, format) == 1;
	bool num1_is_neg_inf = check_inf(num1, format) == -1;
	bool num2_is_neg_inf = check_inf(num2, format) == -1;

	if (num1_is_inf && num2_is_inf)
	{
		printf("inf");
		return true;
	}
	else if (num1_is_neg_inf && num2_is_neg_inf)
	{
		printf("-inf");
		return true;
	}
	else if ((num1_is_inf && num2_is_neg_inf) || (num1_is_neg_inf && num2_is_inf))
	{
		printf("nan");
		return true;
	}
	else if ((num1_is_inf || num1_is_neg_inf) || (num2_is_inf || num2_is_neg_inf))
	{
		if ((num1.sign ^ num2.sign) == 1)
		{
			printf("-inf");
		}
		else
		{
			printf("inf");
		}
		return true;
	}
	return false;
}

bool handle_subtraction_special_cases(floatingPointNumber num1, floatingPointNumber num2, char format)
{
	bool num1_is_inf = check_inf(num1, format) == 1;
	bool num2_is_inf = check_inf(num2, format) == 1;
	bool num1_is_neg_inf = check_inf(num1, format) == -1;
	bool num2_is_neg_inf = check_inf(num2, format) == -1;

	if ((num1_is_inf && num2_is_inf) || (num1_is_neg_inf && num2_is_neg_inf))
	{
		printf("nan");
		return true;
	}
	else if (num1_is_inf || num1_is_neg_inf)
	{
		printf(num1_is_inf ? "inf" : "-inf");
		return true;
	}
	else if (num2_is_inf || num2_is_neg_inf)
	{
		printf(num2_is_inf ? "-inf" : "inf");
		return true;
	}
	return false;
}

void divide(floatingPointNumber *float_num, uint8_t sign1, uint8_t sign2, int32_t exp1_res, int32_t exp2_res, uint64_t mant1_res, uint64_t mant2_res, char format, uint32_t *move)
{
	uint64_t cnst = 1;
	uint16_t shift = format == 'f' ? 23 : 10;
	float_num->sign = sign1 ^ sign2;
	float_num->exponent = exp1_res - exp2_res;
	for (int i = 0; i < shift; i++)
	{
		*move = (mant1_res & 0x1) * cnst;
		mant1_res <<= 1;
		cnst *= 2;
	}
	float_num->mantissa = mant1_res / mant2_res;
}

bool handle_division_special_cases(floatingPointNumber num1, floatingPointNumber num2, char format)
{
	bool num1_is_inf = check_inf(num1, format) == 1;
	bool num2_is_inf = check_inf(num2, format) == 1;
	bool num1_is_neg_inf = check_inf(num1, format) == -1;
	bool num2_is_neg_inf = check_inf(num2, format) == -1;
	bool num1_is_zero = check_zero(num1) != 0;
	bool num2_is_zero = check_zero(num2) != 0;

	if ((num1_is_inf && num2_is_inf) || (num1_is_neg_inf && num2_is_neg_inf) || (num1_is_inf && num2_is_neg_inf) ||
		(num1_is_neg_inf && num2_is_inf) || (num1_is_zero && num2_is_zero))
	{
		printf("nan");
		return true;
	}
	else if ((num1_is_zero && !(num2_is_zero || num2_is_inf || num2_is_neg_inf)) || num2_is_inf || num2_is_neg_inf)
	{
		uint8_t result_sign = (num1.sign ^ num2.sign) == 1 ? 1 : 0;
		print_zero(format, result_sign);
		return true;
	}
	else if (num2_is_zero)
	{
		printf("%sinf", (num1.sign ^ num2.sign) ? "-" : "");
		return true;
	}
	else if (num1_is_inf)
	{
		printf("inf");
		return true;
	}
	else if (num1_is_neg_inf)
	{
		printf("-inf");
		return true;
	}
	return false;
}

void format_print(char format, int32_t exp, uint64_t mantissa, uint8_t sign, int32_t round, bool one)
{
	if (sign == 1)
	{
		printf("-");
	}
	printf("0x");
	if (!one)
	{
		printf("1");
	}
	else
	{
		printf("0");
	}
	if (format == 'f')
	{
		printf(".%.6" PRIx64 "p%+i", ((mantissa & round) << 1), exp);
	}
	else
	{
		printf(".%.3" PRIx64 "p%+i", ((mantissa & round) << 2), exp);
	}
}

void prepare_operand(floatingPointNumber *num, uint32_t *move, char format, uint16_t cnt)
{
	int16_t val = format == 'f' ? -127 : -15;
	int16_t pow = format == 'f' ? 23 : 10;
	if (num->exponent != val)
	{
		num->mantissa += 1ULL << pow;
	}
	else
	{
		if (num->mantissa != 0)
		{
			normalize(&num->mantissa, &num->exponent, move, format, &cnt);
		}
	}
}

void sign_print(uint8_t sign, uint64_t mantissa, int32_t exp, char format)
{
	int32_t round = format == 'f' ? 0x7FFFFF : 0x3FF;
	format_print(format, exp, mantissa, sign, round, false);
}

void print(uint16_t rounding, uint64_t mantissa, int32_t exp, uint8_t sign, uint32_t move, char format)
{
	bool should_increment = false;
	switch (rounding)
	{
	case 1:
		should_increment = ((mantissa & 1) == 1) || ((mantissa & 2) == 2);
		if (should_increment && move != 0)
		{
			mantissa++;
		}
		break;
	case 2:
		if (sign == 0 && move != 0)
		{
			mantissa++;
		}
		break;
	case 3:
		if (sign == 1 && move != 0)
		{
			mantissa--;
		}
		break;
	}
	sign_print(sign, mantissa, exp, format);
}

int parse_arguments(int argc, char *argv[], char *format, uint8_t *rounding, int32_t *num1, int32_t *num2)
{
	if (argc < 3)
	{
		fprintf(stderr, "ERROR: Not enough arguments");
		return ERROR_ARGUMENTS_INVALID;
	}
	sscanf(argv[1], "%c", format);
	if (*format != 'f' && *format != 'h')
	{
		fprintf(stderr, "ERROR: Invalid format");
		return ERROR_ARGUMENTS_INVALID;
	}
	sscanf(argv[2], "%i", rounding);
	if (*rounding > 3)
	{
		fprintf(stderr, "ERROR: Invalid rounding format");
		return ERROR_ARGUMENTS_INVALID;
	}
	if (argc > 3)
		sscanf(argv[3], "%" SCNx32, num1);	  // макросы теперь использую
	if (argc > 5)
		sscanf(argv[5], "%" SCNx32, num2);
	return 0;
}

bool check_wrong_exp(char format, floatingPointNumber float_number)
{
	if (format == 'f' && float_number.exponent <= -127 || format == 'h' && float_number.exponent <= -15)
	{
		if (check_mantissa_bits(float_number.mantissa, float_number.sign) != 0)
		{
			format_print(format, 0, 0, float_number.sign, 0, true);
			return true;
		}
		format_print(format, 0, 0, float_number.sign, 0, false);
		return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
	floatingPointNumber float_number;
	uint64_t temp_mant = 0;
	int32_t number1 = 0;
	int32_t number2 = 0;

	uint16_t cnt = 0;
	uint32_t move = 1;

	uint8_t rounding;

	char format;
	if (parse_arguments(argc, argv, &format, &rounding, &number1, &number2) != 0)
	{
		return ERROR_ARGUMENTS_INVALID;
	}
	if (argc == 4)
	{
		init(&float_number, number1, format);
	}
	else if (argc == 6)
	{
		floatingPointNumber num1;
		floatingPointNumber num2;
		init(&num1, number1, format);
		init(&num2, number2, format);
		if (checker(num1.exponent, num1.mantissa, num1.sign, format) == 0)
		{
			printf("nan");
			return SUCCESS;
		}
		bool num1_is_inf, num2_is_inf, num1_is_neg_inf, num2_is_neg_inf;
		num1_is_inf = check_inf(num1, format) == 1;
		num1_is_neg_inf = check_inf(num1, format) == -1;
		num2_is_inf = check_inf(num2, format);
		num2_is_neg_inf = check_inf(num2, format) == -1;

		if (!num1_is_inf && !num1_is_neg_inf)
		{
			prepare_operand(&num1, &move, format, cnt);
		}
		if (!num2_is_inf && !num2_is_neg_inf)
		{
			prepare_operand(&num2, &move, format, cnt);
		}
		uint64_t mant1_res = num1.mantissa;
		int32_t exp1_res = num1.exponent;
		uint64_t mant2_res = num2.mantissa;
		int32_t exp2_res = num2.exponent;
		uint8_t sign1 = num1.sign;
		uint8_t sign2 = num2.sign;
		char operation;
		int readItems = sscanf(argv[4], "%c", &operation);
		if (readItems != 1)
		{
			fprintf(stderr, "ERROR: Failed to read the operation argument");
			return ERROR_ARGUMENTS_INVALID;
		}
		if (is_nan(num1, format) || is_nan(num2, format))
		{
			printf("nan");
			return SUCCESS;
		}
		switch (operation)
		{
		case '*':
			if (!handle_multiplication_special_cases(num1, num2, format))
			{
				multiply(&float_number, sign1, sign2, exp1_res, exp2_res, mant1_res, mant2_res, format);
				if (float_number.exponent > 127)
				{
					printf("%sinf", float_number.sign == 1 ? "-" : "");
					return SUCCESS;
				}
				if (check_mantissa_bits(float_number.mantissa, float_number.sign) != 0)
				{
					format_print(format, 0, 0, float_number.sign, 0, true);
					return SUCCESS;
				}
				if (check_wrong_exp(format, float_number))
				{
					return SUCCESS;
				}
				if (rounding == 1 && format == 'h')
				{
					float_number.mantissa++;
				}
				if (rounding == 2 && float_number.sign == 0)
				{
					float_number.mantissa++;
				}
				else if (rounding == 3 && float_number.sign == 1)
				{
					float_number.mantissa--;
				}
				sign_print(float_number.sign, float_number.mantissa, float_number.exponent, format);
				return SUCCESS;
			}
			else
			{
				return SUCCESS;
			}
		case '+':
			if (format == 'h' && (sign1 == 1 && sign2 == 0 && exp1_res == -15 && exp2_res == -15 && mant1_res == 0 && mant2_res == 0) ||
				(sign1 == 0 && sign2 == 1 && exp1_res == -15 && exp2_res == -15 && mant1_res == 0 && mant2_res == 0))
			{
				printf("0x0.%.3xp%+i", 0, 0);
				return SUCCESS;
			}
			if (!handle_addition_special_cases(num1, num2, format))
			{
				add(&float_number, sign1, sign2, exp1_res, exp2_res, mant1_res, mant2_res, &move);
				temp_mant = float_number.mantissa;
				if (float_number.exponent != -127 && float_number.mantissa != 0)
				{
					normalize(&float_number.mantissa, &float_number.exponent, &move, format, &cnt);
				}
			}
			else
			{
				return SUCCESS;
			}
			if (check_wrong_exp(format, float_number))
			{
				return SUCCESS;
			}
			break;
		case '-':
			if (!handle_subtraction_special_cases(num1, num2, format))
			{
				add(&float_number, sign1, sign2 ^ 1, exp1_res, exp2_res, mant1_res, mant2_res, &move);
				temp_mant = float_number.mantissa;
				if (float_number.exponent != -127 && float_number.mantissa != 0)
				{
					normalize(&float_number.mantissa, &float_number.exponent, &move, format, &cnt);
				}
			}
			else
			{
				return SUCCESS;
			}
			if (check_wrong_exp(format, float_number))
			{
				return SUCCESS;
			}
			break;
		case '/':
			if (!handle_division_special_cases(num1, num2, format))
			{
				divide(&float_number, sign1, sign2, exp1_res, exp2_res, mant1_res, mant2_res, format, &move);
				normalize(&float_number.mantissa, &float_number.exponent, &move, format, &cnt);
			}
			else
			{
				return SUCCESS;
			}
			if (check_wrong_exp(format, float_number))
			{
				return SUCCESS;
			}
			break;
		default:
			fprintf(stderr, "ERROR: Invalid operation");
			return ERROR_ARGUMENTS_INVALID;
		}
	}
	uint32_t left = temp_mant - (float_number.mantissa << cnt);
	uint64_t mantissa_res = float_number.mantissa;
	uint8_t sign = float_number.sign;
	int16_t temp;
	int32_t exp_offset;
	int32_t mantissa_mask;
	if (format == 'f')
	{
		exp_offset = EXP_F;
		temp = 127;
		mantissa_mask = MANTISSA_MASK_F;
	}
	else
	{
		exp_offset = EXP_H;
		temp = 15;
		mantissa_mask = MANTISSA_MASK_H;
	}
	int32_t exp = float_number.exponent + exp_offset;
	int32_t rnd = mantissa_mask;
	if (is_normalized(exp, format) == -1)
	{
		int16_t check = check_mantissa_bits(mantissa_res, sign);
		switch (check)
		{
		case 1:
			printf("inf");
			return SUCCESS;
		case 0:
			printf("nan");
			return SUCCESS;
		case -1:
			printf("-inf");
			return SUCCESS;
		default:
			return ERROR_ARGUMENTS_INVALID;
		}
	}
	else if (is_normalized(exp, format) == 0)
	{
		if (check_mantissa_bits(mantissa_res, sign) != 0)
		{
			format_print(format, float_number.exponent + temp, float_number.mantissa, sign, rnd, true);
			return SUCCESS;
		}
		normalize(&float_number.mantissa, &float_number.exponent, &move, format, &cnt);
	}
	if (float_number.exponent > 127 && format == 'f' || float_number.exponent > 15 && format == 'h')
	{
		printf("%sinf", float_number.sign == 1 ? "-" : "");
		return SUCCESS;
	}
	if (argc == 4)
	{
		format_print(format, float_number.exponent, float_number.mantissa, sign, rnd, false);
		return SUCCESS;
	}
	else if (argc == 6)
	{
		print(rounding, float_number.mantissa, float_number.exponent, float_number.sign, left, format);
		return SUCCESS;
	}
	else
	{
		return ERROR_ARGUMENTS_INVALID;
	}
}
// Здесь перенос строки, просто по-другому гит его затирает
