#pragma once

#include <cmath>

#include "Vec3.h"

template <typename T>
class _Mat3
{
public:
	T matrix[3][3] = {};

	T* operator[](unsigned i) { return matrix[i]; }

	friend _Mat3 operator+(_Mat3 lhs, _Mat3 rhs)
	{
		_Mat3 result;
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				result[i][j] = lhs[i][j] + rhs[i][j];
			}
		}

		return result;
	}
	_Mat3& operator+=(_Mat3 rhs) { return *this = *this + rhs; }

	friend _Mat3 operator-(_Mat3 lhs, _Mat3 rhs)
	{
		_Mat3 result;
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				result[i][j] = lhs[i][j] - rhs[i][j];
			}
		}

		return result;
	}
	_Mat3& operator-=(_Mat3 rhs) { return *this = *this - rhs; }

	friend _Mat3 operator*(T lhs, _Mat3 rhs)
	{
		_Mat3 result;
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				result[i][j] = lhs * rhs;
			}
		}

		return result;
	}
	friend _Mat3 operator*(_Mat3 lhs, T rhs)
	{
		return rhs * lhs;
	}
	_Mat3& operator*=(T rhs) { return *this = *this * rhs; }

	friend _Mat3 operator*(_Mat3 lhs, _Mat3 rhs)
	{
		_Mat3 result;
		for (auto i = 0; i < 3; i++)
		{
			for (auto j = 0; j < 3; j++)
			{
				result[i][j] = lhs[i][0] * rhs[0][i] + lhs[i][1] * rhs[1][i] + lhs[i][2] * rhs[2][i];
			}
		}

		return result;
	}
	_Mat3& operator*=(_Mat3 rhs) { return *this = *this * rhs; }

	static constexpr _Mat3 Identity()
	{
		return
		{
			(T)1.0, (T)0.0, (T)0.0,
			(T)0.0, (T)1.0, (T)0.0,
			(T)0.0, (T)0.0, (T)1.0
		}
	}

	template <typename TAngle> static _Mat3 RotateX(TAngle angle)
	{
		return
		{
			(T)1.0, (T)0.0,             (T)0.0,
			(T)0.0, (T)std::cos(angle), -(T)std::sin(angle),
			(T)0.0, (T)std::sin(angle), (T)std::cos(angle)
		}
	}
	template <typename TAngle> static _Mat3 RotateY(TAngle angle)
	{
		return
		{
			(T)std::cos(angle),  (T)0.0, (T)std::sin(angle),
			(T)0.0,              (T)1.0, (T)0.0,
			-(T)std::sin(angle), (T)0.0, (T)std::cos(angle)
		}
	}
	template <typename TAngle> static _Mat3 RotateZ(TAngle angle)
	{
		return
		{
			(T)std::cos(angle), -(T)std::sin(angle), (T)0.0,
			(T)std::sin(angle), (T)std::cos(angle),  (T)0.0,
			(T)0.0,             (T)0.0,              (T)1.0
		}
	}

	static _Mat3 Scale(T x, T y, T z)
	{
		return
		{
			x,      (T)0.0, (T)0.0,
			(T)0.0, y,      (T)0.0,
			(T)0.0, (T)0.0,  z
		}
	}
	static _Mat3 Scale(_Vec3<T> s) { return Scale(s.x, s.y, s.z); }
};

using Mat3 = _Mat3<float>;
using Mai3 = _Mat3<int>;