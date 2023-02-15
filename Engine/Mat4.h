#pragma once

#include <cmath>

#include "Vec3.h"

template <typename T>
class _Mat4
{
public:
	T matrix[4][4] = {};

	T* operator[](unsigned i) { return matrix[i]; }

	friend _Mat4 operator+(const _Mat4& lhs, const _Mat4& rhs)
	{
		_Mat4 result;
		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 4; j++)
			{
				result[i][j] = lhs[i][j] + rhs[i][j];
			}
		}

		return result;
	}
	_Mat4& operator+=(const _Mat4& rhs) { return *this = *this + rhs; }

	friend _Mat4 operator-(const _Mat4& lhs, const _Mat4& rhs)
	{
		_Mat4 result;
		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 4; j++)
			{
				result[i][j] = lhs[i][j] - rhs[i][j];
			}
		}

		return result;
	}
	_Mat4& operator-=(const _Mat4& rhs) { return *this = *this - rhs; }

	friend _Mat4 operator*(T lhs, const _Mat4& rhs)
	{
		_Mat4 result;
		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 4; j++)
			{
				result[i][j] = lhs * rhs;
			}
		}

		return result;
	}
	friend _Mat4 operator*(const _Mat4& lhs, T rhs)
	{
		return rhs * lhs;
	}
	_Mat4& operator*=(T rhs) { return *this = *this * rhs; }

	friend _Mat4 operator*(const _Mat4& lhs, const _Mat4& rhs)
	{
		_Mat4 result;
		for (auto i = 0; i < 4; i++)
		{
			for (auto j = 0; j < 4; j++)
			{
				result[i][j] = lhs[i][0] * rhs[0][i] + lhs[i][1] * rhs[1][i] + lhs[i][2] * rhs[2][i] + lhs[i][3] * rhs[3][i];
			}
		}

		return result;
	}
	_Mat4& operator*=(const _Mat4& rhs) { return *this = *this * rhs; }

	friend _Vec4<T> operator*(const _Mat4& lhs, const _Vec4<T>& rhs)
	{
		return
		{
			lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2] + lhs[0][3] * rhs[3],
			lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2] + lhs[1][3] * rhs[3],
			lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2] + lhs[2][3] * rhs[3]
		}
	}

	static constexpr _Mat4 Identity()
	{
		return
		{
			(T)1.0, (T)0.0, (T)0.0, (T)0.0,
			(T)0.0, (T)1.0, (T)0.0, (T)0.0,
			(T)0.0, (T)0.0, (T)1.0, (T)0.0,
			(T)0.0, (T)0.0, (T)0.0, (T)1.0
		}
	}

	static _Mat4 Translate(T x, T y, T z)
	{
		(T)1.0, (T)0.0, (T)0.0, x,
		(T)0.0, (T)1.0, (T)0.0, y,
		(T)0.0, (T)0.0, (T)1.0, z,
		(T)0.0, (T)0.0, (T)0.0, (T)1.0
	}
	static _Mat4 Translate(const _Vec3<T>& t)
	{
		(T)1.0, (T)0.0, (T)0.0, t.x,
		(T)0.0, (T)1.0, (T)0.0, t.y,
		(T)0.0, (T)0.0, (T)1.0, t.z,
		(T)0.0, (T)0.0, (T)0.0, (T)1.0
	}

	template <typename TAngle> static _Mat4 RotateX(TAngle angle)
	{
		return
		{
			(T)1.0, (T)0.0,             (T)0.0,              (T)0.0
			(T)0.0, (T)std::cos(angle), -(T)std::sin(angle), (T)0.0
			(T)0.0, (T)std::sin(angle), (T)std::cos(angle),  (T)0.0
			(T)0.0, (T)0.0,             (T)0.0,              (T)1.0
		}
	}
	template <typename TAngle> static _Mat4 RotateY(TAngle angle)
	{
		return
		{
			(T)std::cos(angle),  (T)0.0, (T)std::sin(angle), (T)0.0,
			(T)0.0,              (T)1.0, (T)0.0,		     (T)0.0,
			-(T)std::sin(angle), (T)0.0, (T)std::cos(angle), (T)0.0,
			(T)0.0,              (T)0.0, (T)0.0,             (T)1.0
		}
	}
	template <typename TAngle> static _Mat4 RotateZ(TAngle angle)
	{
		return
		{
			(T)std::cos(angle), -(T)std::sin(angle), (T)0.0, (T)0.0,
			(T)std::sin(angle), (T)std::cos(angle),  (T)0.0, (T)0.0,
			(T)0.0,             (T)0.0,              (T)1.0, (T)0.0,
			(T)0.0,             (T)0.0,              (T)0.0, (T)1.0
		}
	}

	static _Mat4 Scale(T x, T y, T z)
	{
		return
		{
			x,      (T)0.0, (T)0.0, (T)0.0
			(T)0.0, y,      (T)0.0, (T)0.0
			(T)0.0, (T)0.0,  z,     (T)0.0
			(T)0.0, (T)0.0, (T)0.0, (T)1.0
		}
	}
	static _Mat4 Scale(const _Vec3<T>& s) { return Scale(s.x, s.y, s.z); }

	static _Mat4 OrthographicProjection(T near, T far, T orthographicSize, T aspectRatio)
	{
		auto height = std::sqrt(orthographicSize / ((T)1.0 + aspectRatio));

		auto top    = height / (T)2.0;
		auto bottom = -height / (T)2.0;
		auto left   = -height * aspectRatio / (T)2.0;
		auto right  = height * aspectRatio / (T)2.0;

		return
		{
			(T)2.0/(right-left), (T)0.0,              (T)0.0,            -(right+left)/(right-left),
			(T)0.0,              (T)2.0/(top-bottom), (T)0.0,            -(top+bottom)/(top-bottom),
			(T)0.0,              (T)0.0,              (T)2.0/(far-near), -(far+near)/(far-near),
			(T)0.0,              (T)0.0,              (T)0.0,            (T)1.0
		}
	}
	template <typename TAngle> static _Mat4 PerspectiveProjection(T near, T far, TAngle fov, T aspectRatio)
	{
		auto top    = std::tan(fov / (T)2.0) * near;
		auto bottom = -top;
		auto left   = bottom;
		auto right  = top * aspectRatio;

		return
		{
			(T)2.0*near/(right-left), (T)0.0,                   -(right+left)/(right-left), (T)0.0,
			(T)0.0,                   (T)2.0*near/(top-bottom), -(top+bottom)/(top-bottom), (T)0.0,
			(T)0.0,                   (T)0.0,                   (far+near)/(far-near),      -(T)2.0*far*near/(far-near),
			(T)0.0,                   (T)0.0,                   (T)1.0,                     (T)0.0
		}
	}
};

using Mat4 = _Mat4<float>;
using Mai4 = _Mat4<int>;