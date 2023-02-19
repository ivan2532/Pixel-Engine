#pragma once

#include "Vec3.h"

template <typename T>
class _Vec4 : public _Vec3<T>
{
public:
	T w;

	_Vec4(T xx = (T)0.0, T yy = (T)0.0, T zz = (T)0.0, T ww = (T)1.0)
		:
		_Vec3(xx, yy, zz),
		w(ww)
	{
	}
	_Vec4(_Vec2 vec2)
		:
		_Vec3(vec2.x, vec2.y, (T)0.0),
		w((T)1.0)
	{
	}
	_Vec4(_Vec3 vec3)
		:
		_Vec3(vec3.x, vec3.y, vec3.z),
		w((T)1.0)
	{
	}

	friend _Vec4 operator+(const _Vec4& lhs, const _Vec4& rhs)
	{
		_Vec4 result;
		result.x = lhs.x + rhs.x;
		result.y = lhs.y + rhs.y;
		result.z = lhs.z + rhs.z;

		return result;
	}
	_Vec4& operator+=(const _Vec4& rhs)
	{
		T originalW = this->w;
		*this = *this + rhs;
		this->w = originalW;

		return *this;
	}

	friend _Vec4 operator-(const _Vec4& lhs, const _Vec4& rhs)
	{
		_Vec4 result;
		result.x = lhs.x - rhs.x;
		result.y = lhs.y - rhs.y;
		result.z = lhs.z - rhs.z;

		return result;
	}
	_Vec4& operator-=(const _Vec4& rhs)
	{
		T originalW = this->w;
		*this = *this - rhs;
		this->w = originalW;

		return *this;
	}

	_Vec4 operator-() { return { -x, -y, -z }; }

	friend _Vec4 operator*(T lhs, const _Vec4& rhs) { return _Vec4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }
	friend _Vec4 operator*(const _Vec4& lhs, T rhs) { return rhs * lhs; }
	_Vec4& operator*=(T rhs)
	{
		T originalW = this->w;
		*this = *this * rhs;
		this->w = originalW;

		return *this;
	}

	friend _Vec4 operator/(const _Vec4& lhs, T rhs) { return lhs * ((T)1.0 / rhs); }
	_Vec4& operator/=(T rhs) { return *this = *this * ((T)1.0 / rhs); }

	static auto Magnitude(const _Vec4& value)
	{
		return static_cast<T>(std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z));
	}
	static _Vec4 Normalize(const _Vec4& value)
	{
		float magnitude = _Vec4::Magnitude(value);
		return value / magnitude;
	}

	static auto Dot(const _Vec4& lhs, const _Vec4& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	static _Vec4 Hadamard(const _Vec4& lhs, const _Vec4& rhs, bool ignoreW = false)
	{
		return _Vec4(
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z,
			ignoreW ? (T)1.0 : lhs.w * rhs.w
		);
	}
	static _Vec4 Cross(const _Vec4& lhs, const _Vec4& rhs)
	{
		return _Vec3::Cross(lhs, rhs);
	}

	static constexpr _Vec4 Up() { return _Vec3<T>::Up(); }
	static constexpr _Vec4 Down() { return _Vec3<T>::Down(); }
	static constexpr _Vec4 Left() { return _Vec3<T>::Left(); }
	static constexpr _Vec4 Right() { return _Vec3<T>::Right(); }
	static constexpr _Vec4 Zero() { return _Vec3<T>::Zero(); }
	static constexpr _Vec4 One() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
};

using Vec4 = _Vec4<float>;
using Vei4 = _Vec4<int>;