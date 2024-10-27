#include "quat.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>
#include <limits>

#pragma warning(disable : 4305)

template< typename T >
bool compare_n(const T* actual, T* expected, size_t n, T delta = 1e-2)
{
	for (size_t i = 0; i < n; ++i)
		if (abs(actual[i] - expected[i]) > delta)
			return false;
	return true;
}

TEST(ctor, defaults)
{
	float expected[4]{ 0, 0, 0, 0 };
	EXPECT_EQ(0, std::memcmp(Quat< float >().data(), expected, 4 * sizeof(float)));
}

TEST(ctor, zero_float)
{
	float expected[4]{ 0, 0, 0, 0 };
	EXPECT_EQ(0, std::memcmp(Quat< float >(0, 0, 0, 0).data(), expected, 4 * sizeof(float)));
}

TEST(ctor, zero_double)
{
	double expected[4]{ 0, 0, 0, 0 };
	EXPECT_EQ(0, std::memcmp(Quat< double >(0, 0, 0, 0).data(), expected, 4 * sizeof(double)));
}

TEST(ctor, ctor_double)
{
	double expected[4] = { 1, 2, 3, 0 };
	EXPECT_EQ(0, std::memcmp(Quat< double >(0, 1, 2, 3).data(), expected, 4 * sizeof(double)));
}

TEST(ctor, ctor_special)
{
	float nan = std::numeric_limits< float >::quiet_NaN();
	float expected[4]{ nan, std::numeric_limits< float >::max(), std::numeric_limits< float >::infinity(), 0.f };
	EXPECT_EQ(
		0,
		std::memcmp(
			Quat< float >(0.f, nan, std::numeric_limits< float >::max(), std::numeric_limits< float >::infinity()).data(),
			expected,
			4 * sizeof(float)));
}

TEST(ctor, angled_radians)
{
	float expected[4]{ 0.22649, 0.75497, 0.52848, 0.31532 };
	float actual[4];
	std::memcpy(actual, Quat< float >(2.5, true, { 0.15, 0.5, 0.35 }).data(), 4 * sizeof(float));
	EXPECT_EQ(true, compare_n(actual, expected, 4));
}

TEST(ctor, angled_degrees)
{
	float expected[4] = { 0.005206, 0.017355, 0.012148, 0.99976 };
	float actual[4];
	std::memcpy(actual, Quat< float >(2.5, false, { 0.15, 0.5, 0.35 }).data(), 4 * sizeof(float));
	EXPECT_EQ(true, compare_n< float >(actual, expected, 4, 1e-3));
}

TEST(operators, plus_quat)
{
	float expected[4]{ 3, 5, 7, 0 };
	const Quat< float > a(0, 1, 2, 3), b(0, 2, 3, 4);
	const Quat< float >& c = a + b;
	ASSERT_FALSE(std::is_reference_v< decltype(a + b) >);
	EXPECT_EQ(0, std::memcmp(c.data(), expected, 4 * sizeof(float)));
}

TEST(operators, plus_quat_assignment)
{
	float expected[4]{ 3, 5, 7, 0 };
	Quat< float > a(0, 1, 2, 3), b(0, 2, 3, 4);
	a += b;
	EXPECT_EQ(0, std::memcmp(a.data(), expected, 4 * sizeof(float)));
}

TEST(operators, minus_quat)
{
	float expected[4]{ 9, 8, 9, 10 };
	const Quat< float > a(10, 11, 12, 13), b(0, 2, 4, 4);
	const Quat< float >& c = a - b;
	ASSERT_FALSE(std::is_reference_v< decltype(a - b) >);
	EXPECT_EQ(0, std::memcmp(c.data(), expected, 4 * sizeof(float)));
}

TEST(operators, minus_quat_assignment)
{
	float expected[4]{ 9, 8, 9, 10 };
	Quat< float > a(10, 11, 12, 13), b(0, 2, 4, 4);
	a -= b;
	EXPECT_EQ(0, std::memcmp(a.data(), expected, 4 * sizeof(float)));
}

TEST(operators, chain_quat_assignment)
{
	float expected[4]{ 3, 0, 7, -1 };
	Quat< float > a(0, 1, 2, 3), b(0, 2, 3, 4), c(1, 0, 5, 0);
	a += b -= c;
	ASSERT_TRUE(std::is_lvalue_reference_v< decltype(a += b) >);
	ASSERT_TRUE(std::is_lvalue_reference_v< decltype(b -= c) >);
	ASSERT_FALSE(std::is_const_v< std::remove_reference_t< decltype(a += b) > >);
	ASSERT_FALSE(std::is_const_v< std::remove_reference_t< decltype(b -= c) > >);
	EXPECT_EQ(0, std::memcmp(a.data(), expected, 4 * sizeof(float)));
}
TEST(operators, conj_float)
{
	float expected[4]{ -1, -2, -3, -2 };
	const Quat< float > a(-2, 1, 2, 3);
	Quat< float > c = ~a;
	ASSERT_FALSE(std::is_reference_v< decltype(~a) >);
	EXPECT_EQ(0, std::memcmp(c.data(), expected, 4 * sizeof(float)));
}

TEST(operators, conj_double)
{
	double expected[4]{ -1, -2, -3, 1 };
	Quat< double > a(1, 1, 2, 3);
	Quat< double > c = ~a;
	EXPECT_EQ(0, std::memcmp(c.data(), expected, 4 * sizeof(double)));
}

TEST(operators, norm_float)
{
	float expected = 7.23071957f;
	const Quat< float > a(2.57, 5.28, 2.2, 3.6);
	float actual = float(a);
	ASSERT_FALSE(std::is_reference_v< decltype(float(a)) >);
	EXPECT_EQ(true, compare_n< float >(&actual, &expected, 1, 1e-2));
}

TEST(operators, norm_double)
{
	double expected = 7.23071957;
	Quat< double > a(2.57, 5.28, 2.2, 3.6);
	double actual = double(a);
	ASSERT_FALSE((std::is_convertible_v< Quat< double >, double >));
	EXPECT_EQ(true, compare_n(&actual, &expected, 1, 1e-2));
}

TEST(operators, eq_float_true)
{
	const Quat< float > a(0, 1, 2, 3), b(0, 1, 2, 3);
	EXPECT_EQ(true, a == b && b == a);
}

TEST(operators, eq_float_false)
{
	Quat< float > a(0, 1, 2, 3), b(0, 10, 2, 3);
	EXPECT_EQ(false, a == b);
}

TEST(operators, ne_double_true)
{
	const Quat< double > a(0, 1, 2, 3), b(0, 1, 2, 3);
	EXPECT_EQ(false, a != b && b != a);
}

TEST(operators, ne_double_false)
{
	Quat< double > a(0, 1, 2, 3), b(0, 10, 2, 3);
	EXPECT_EQ(true, a != b);
}

TEST(operators, multiply_scalar)
{
	float expected[4]{ 2.5, 5, 7.5, 25 };
	const Quat< float > a(10, 1, 2, 3);
	Quat< float > actual = a * 2.5f;
	ASSERT_FALSE(std::is_reference_v< decltype(a * 2.5f) >);
	EXPECT_EQ(true, compare_n< float >(actual.data(), expected, 4, 1e-5));
}

TEST(operators, multiply_vector)
{
	float expected[4]{ 10, 20, 30, -14 };
	const Quat< float > a(10, 1, 2, 3);
	const vector3_t< float > vec{ 1, 2, 3 };
	Quat< float > actual = a * vec;
	ASSERT_FALSE(std::is_reference_v< decltype(a * vec) >);
	EXPECT_EQ(true, compare_n< float >(actual.data(), expected, 4, 1e-5));
}

TEST(operators, multiply_quat)
{
	float expected[4]{ 20, 34, 42, -10 };
	const Quat< float > a(10, 1, 2, 3), b(1, 2, 3, 4);
	Quat< float > actual = a * b;
	ASSERT_FALSE(std::is_reference_v< decltype(a * b) >);
	EXPECT_EQ(true, compare_n< float >(actual.data(), expected, 4, 1e-5));
}

TEST(extra_functions, apply)
{
	vector3_t< float > expected{ 1.f, 0.f, 0.f };
	const vector3_t< float > vec{ 0.f, 0.f, 1.f };
	const Quat< float > a(0.f, 0.707f, 0.f, 0.707f);
	vector3_t< float > actual = a.apply(vec);
	EXPECT_EQ(true, compare_n< float >((float*)&actual, (float*)&expected, 3, 1e-3));
}

TEST(extra_functions, apply2)
{
	vector3_t< double > expected{ -0.2714, 0.3571, 0.3857 };
	const vector3_t< double > vec{ 0.5, 0.1, 0.3 };
	const Quat< double > a(0, .1, .2, .3);
	vector3_t actual = a.apply(vec);
	EXPECT_EQ(true, compare_n< double >((double*)&actual, (double*)&expected, 3, 1e-3));
}

TEST(extra_functions, angle_degrees)
{
	const Quat< double > a(0.707, 0, 0.707, 0);
	double actual = a.angle(false);
	double expected = 90.;
	EXPECT_EQ(true, compare_n< double >(&actual, &expected, 1, 1e-0));
}

TEST(extra_functions, angle_radians)
{
	Quat< double > a(0, 0.707, 0, 0.707);
	double actual = a.angle();
	double expected = 3.1415;
	EXPECT_EQ(true, compare_n< double >(&actual, &expected, 1, 1e-2));
}

TEST(extra_functions, rotation_matrix)
{
	float expected[16]{ 0.2624f, 0.9645f,  -0.02837f, 0.f, -0.3972f, 0.1348f, 0.9078f, 0.f,
						0.8794f, -0.2270f, 0.4184f,	  0.f, 0.f,		 0.f,	  0.f,	   1.f };
	const Quat< float > a(0.8, 0.5, 0.4, 0.6);
	matrix_t< float > actual = a.rotation_matrix();
	EXPECT_EQ(true, compare_n< float >(actual.data, expected, 16, 1e-3));
}

TEST(extra_functions, rotate_vector)
{
	float expected[4]{ 0, 0.707, 0, 0.707 };

	const Quat< float > orientation(1, 0, 0, 0);
	const vector3_t< float > vec{ 0, 1, 0 };
	float angle = 90.0f;
	Quat< float > rotation(angle, false, vec);

	Quat< float > actual = rotation * orientation;
	EXPECT_EQ(true, compare_n< float >(actual.data(), expected, 4, 1e-2));
}

TEST(extra_functions, matrix)
{
	float expected[16]{ 7.8, -1.5, -2.4, -3.6, 1.5, 7.8, -3.6, 2.4, 2.4, 3.6, 7.8, -1.5, 3.6, -2.4, 1.5, 7.8 };
	const Quat< float > a(7.8, 1.5, 2.4, 3.6);
	matrix_t< float > actual = a.matrix();
	EXPECT_EQ(true, compare_n< float >(actual.data, expected, 16, 1e-2));
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
