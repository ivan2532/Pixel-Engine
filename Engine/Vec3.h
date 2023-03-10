#pragma once

#include "Vec2.h"

template <typename T>
class _Vec3 : public _Vec2<T>
{
public:
	T z;

	_Vec3(T xx = (T)0.0, T yy = (T)0.0, T zz = (T)0.0)
		:
		_Vec2<T>(xx, yy),
		z(zz)
	{
	}
	_Vec3(_Vec2<T> vec2)
		:
		_Vec2<T>(vec2.x, vec2.y),
		z((T)0.0)
	{
	}

	friend _Vec3 operator+(const _Vec3& lhs, const _Vec3& rhs)
	{
		_Vec3 result;
		result.x = lhs.x + rhs.x;
		result.y = lhs.y + rhs.y;
		result.z = lhs.z + rhs.z;

		return result;
	}
	_Vec3& operator+=(const _Vec3& rhs) { return *this = *this + rhs; }

	friend _Vec3 operator-(const _Vec3& lhs, const _Vec3& rhs)
	{
		_Vec3 result;
		result.x = lhs.x - rhs.x;
		result.y = lhs.y - rhs.y;
		result.z = lhs.z - rhs.z;

		return result;
	}
	_Vec3& operator-=(const _Vec3& rhs) { return *this = *this - rhs; }

	_Vec3 operator-() { return { -x, -y, -z }; }

	friend _Vec3 operator*(T lhs, const _Vec3& rhs) { return _Vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }
	friend _Vec3 operator*(const _Vec3& lhs, T rhs) { return rhs * lhs; }
	_Vec3& operator*=(T rhs) { return *this = *this * rhs; }

	friend _Vec3 operator/(const _Vec3& lhs, T rhs) { return lhs * ((T)1.0 / rhs); }
	_Vec3& operator/=(T rhs) { return *this = *this * ((T)1.0 / rhs); }

	static auto Magnitude(const _Vec3& value)
	{
		return static_cast<T>(std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z));
	}
	static _Vec3 Normalize(const _Vec3& value)
	{
		float magnitude = _Vec3::Magnitude(value);
		return value / magnitude;
	}

	static auto Dot(const _Vec3& lhs, const _Vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	static _Vec3 Hadamard(const _Vec3& lhs, const _Vec3& rhs)
	{
		return _Vec3(
			lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z
		);
	}
	static _Vec3 Cross(const _Vec3& lhs, const _Vec3& rhs)
	{
		return _Vec3(
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		);
	}
	
	static constexpr _Vec3 Up()    { return _Vec2<T>::Up(); }
	static constexpr _Vec3 Down()  { return _Vec2<T>::Down(); }
	static constexpr _Vec3 Left()  { return _Vec2<T>::Left(); }
	static constexpr _Vec3 Right() { return _Vec2<T>::Right(); }
	static constexpr _Vec3 Zero()  { return _Vec2<T>::Zero(); }
	static constexpr _Vec3 One()   { return { 1.0f, 1.0f, 1.0f }; }
};

using Vec3 = _Vec3<float>;
using Vei3 = _Vec3<int>;