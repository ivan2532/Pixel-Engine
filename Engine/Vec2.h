#pragma once

template <typename T>
class _Vec2
{
public:
	T x;
	T y;

	_Vec2(T xx = (T)0.0, T yy = (T)0.0)
		:
		x(xx),
		y(yy)
	{
	}

	friend _Vec2 operator+(const _Vec2& lhs, const _Vec2& rhs)
	{
		_Vec2 result;
		result.x = lhs.x + rhs.x;
		result.y = lhs.y + rhs.y;

		return result;
	}
	_Vec2& operator+=(const _Vec2& rhs) { return *this = *this + rhs; }

	friend _Vec2 operator-(const _Vec2& lhs, const _Vec2& rhs)
	{
		_Vec2 result;
		result.x = lhs.x - rhs.x;
		result.y = lhs.y - rhs.y;

		return result;
	}
	_Vec2& operator-=(const _Vec2& rhs) { return *this = *this - rhs; }

	_Vec2 operator-() { return { -x, -y }; }

	friend _Vec2 operator*(T lhs, const _Vec2& rhs) { return _Vec2(lhs * rhs.x, lhs * rhs.y); }
	friend _Vec2 operator*(const _Vec2& lhs, T rhs) { return rhs * lhs; }
	_Vec2& operator*=(T rhs) { return *this = *this * rhs; }

	static auto Dot(const _Vec2& lhs, const _Vec2& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
	static _Vec2 Hadamard(const _Vec2& lhs, const _Vec2& rhs) { return _Vec2(lhs.x * rhs.x, lhs.y * rhs.y); }

	static constexpr _Vec2 Up()    { return _Vec2((T)0.0, (T)1.0); }
	static constexpr _Vec2 Down()  { return _Vec2((T)0.0, (T)-1.0); }
	static constexpr _Vec2 Left()  { return _Vec2((T)-1.0, (T)0.0); }
	static constexpr _Vec2 Right() { return _Vec2((T)1.0, (T)0.0); }
	static constexpr _Vec2 Zero()  { return _Vec2((T)0.0, (T)0.0); }
	static constexpr _Vec2 One()   { return _Vec2((T)1.0, (T)1.0); }
};

using Vec2 = _Vec2<float>;
using Vei2 = _Vec2<int>;