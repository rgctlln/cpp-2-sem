#pragma once

#include <cmath>
#include <cstdint>
#include <iterator>

template< typename T >
struct matrix_t
{
	T data[16];
};

template< typename T >
struct vector3_t
{
	T x, y, z;
};

template< typename T >
class Quat
{
  public:
	Quat() : m_value{ 0, 0, 0, 0 } {}

	Quat(T a, T b, T c, T d) : m_value{ b, c, d, a } {}

	Quat(T angle, bool is_radian_angle, vector3_t< T > vector_3t)
	{
		if (!is_radian_angle)
		{
			angle *= M_PI / 180;
		}
		T normalize_val = std::sqrt(vector_3t.x * vector_3t.x + vector_3t.y * vector_3t.y + vector_3t.z * vector_3t.z);
		if (normalize_val == 0)
		{
			this->m_value[0] = 0, this->m_value[1] = 0, this->m_value[2] = 0, this->m_value[3] = 0;
		}
		vector_3t.x /= normalize_val, vector_3t.y /= normalize_val, vector_3t.z /= normalize_val;
		T sin_val = std::sin(angle / 2);

		this->m_value[0] = vector_3t.x * sin_val;
		this->m_value[1] = vector_3t.y * sin_val;
		this->m_value[2] = vector_3t.z * sin_val;
		this->m_value[3] = std::cos(angle / 2);
	}

	Quat< T > operator*(T num) const
	{
		Quat< T > res = Quat< T >();
		for (uint8_t i = 0; i < 4; ++i)
		{
			res.m_value[i] = this->m_value[i] * num;
		}
		return res;
	}

	Quat< T > operator*(const vector3_t< T > &vector_3t) const
	{
		Quat< T > vector_quat(0, vector_3t.x, vector_3t.y, vector_3t.z);
		return *this * vector_quat;
	}

	Quat< T > operator*(const Quat< T > &quat) const
	{
		T a = m_value[3], b = m_value[0], c = m_value[1], d = m_value[2];

		T w = quat.m_value[3];
		T x = quat.m_value[0];
		T y = quat.m_value[1];
		T z = quat.m_value[2];

		T new_a = a * w - b * x - c * y - d * z;
		T new_b = a * x + b * w + c * z - d * y;
		T new_c = a * y - b * z + c * w + d * x;
		T new_d = a * z + b * y - c * x + d * w;

		return Quat< T >(new_a, new_b, new_c, new_d);
	}

	Quat< T > &operator+=(const Quat< T > &quat)
	{
		apply_operation(quat, 1);
		return *this;
	}

	Quat< T > &operator-=(const Quat< T > &quat)
	{
		apply_operation(quat, -1);
		return *this;
	}

	Quat< T > operator+(const Quat< T > &quat) const { return Quat< T >(*this) += quat; }

	Quat< T > operator-(const Quat< T > &quat) const { return Quat< T >(*this) -= quat; }

	bool operator==(const Quat< T > &quat) const
	{
		return std::equal(std::begin(m_value), std::end(m_value), std::begin(quat.m_value));
	}

	bool operator!=(const Quat< T > &quat) const { return !(*this == quat); }

	explicit operator T() const
	{
		T sum = 0;
		for (uint8_t i = 0; i < 4; ++i)
		{
			sum += this->m_value[i] * this->m_value[i];
		}
		return std::sqrt(sum);
	}

	Quat< T > operator~() const
	{
		Quat< T > res = Quat< T >();
		for (uint8_t i = 0; i < 3; ++i)
		{
			res.m_value[i] = -this->m_value[i];
		}
		res.m_value[3] = this->m_value[3];
		return res;
	}

	T angle(bool flag = true) const
	{
		T angle_radian = 2 * std::acos(this->m_value[3]);
		return flag ? angle_radian : angle_radian * 180 / M_PI;
	}

	matrix_t< T > rotation_matrix() const
	{
		Quat< T > norm_quat = *this;
		T norm = norm_quat.get_norm();
		if (norm != 0)
		{
			for (uint8_t i = 0; i < 4; ++i)
			{
				norm_quat.m_value[i] /= norm;
			}
		}
		vector3_t< T > new_vector_x = norm_quat.apply(vector3_t< T >{ 1, 0, 0 });
		vector3_t< T > new_vector_y = norm_quat.apply(vector3_t< T >{ 0, 1, 0 });
		vector3_t< T > new_vector_z = norm_quat.apply(vector3_t< T >{ 0, 0, 1 });
		matrix_t< T > rotMatrix = {
			new_vector_x.x,
			new_vector_x.y,
			new_vector_x.z,
			0,
			new_vector_y.x,
			new_vector_y.y,
			new_vector_y.z,
			0,
			new_vector_z.x,
			new_vector_z.y,
			new_vector_z.z,
			0,
			0,
			0,
			0,
			1
		};
		return rotMatrix;
	}

	matrix_t< T > matrix() const
	{
		return matrix_t< T >{
			m_value[3], -m_value[0], -m_value[1], -m_value[2], m_value[0], m_value[3],	-m_value[2], m_value[1],
			m_value[1], m_value[2],	 m_value[3],  -m_value[0], m_value[2], -m_value[1], m_value[0],	 m_value[3]
		};
	}

	const T *data() const { return this->m_value; }

	vector3_t< T > apply(const vector3_t< T > &vector_3t) const
	{
		T norm = get_norm();
		if (norm == 0)
		{
			return vector_3t;
		}
		Quat< T > normalized_q(this->m_value[3] / norm, this->m_value[0] / norm, this->m_value[1] / norm, this->m_value[2] / norm);
		Quat< T > quat(0, vector_3t.x, vector_3t.y, vector_3t.z);
		Quat< T > rotated_quat = normalized_q * quat * ~(normalized_q);
		return { rotated_quat.m_value[0], rotated_quat.m_value[1], rotated_quat.m_value[2] };
	}

  private:
	T m_value[4];

	void apply_operation(const Quat< T > &quat2, int16_t sign)
	{
		for (uint8_t i = 0; i < 4; ++i)
		{
			this->m_value[i] += sign * quat2.m_value[i];
		}
	}

	T get_norm() const
	{
		return std::sqrt(this->m_value[0] * this->m_value[0] + this->m_value[1] * this->m_value[1] +
						 this->m_value[2] * this->m_value[2] + this->m_value[3] * this->m_value[3]);
	}
};
//
