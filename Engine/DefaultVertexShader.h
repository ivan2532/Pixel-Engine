#pragma once

#include "Vec3.h"
#include "VertexShader.h"

struct DefaultVSIn
{
	Vec3 m_Position;

	DefaultVSIn(Vec3 position)
		:
		m_Position(position)
	{
	}
};

struct DefaultVSOut
{
	Vec3 m_Position;
	float m_Test;

	DefaultVSOut(Vec3 position, float test)
		:
		m_Position(position),
		m_Test(test)
	{
	}

	friend DefaultVSOut operator+(const DefaultVSOut& lhs, const DefaultVSOut& rhs)
	{
		return
		{
			lhs.m_Position + rhs.m_Position,
			lhs.m_Test + rhs.m_Test
		};
	}
	DefaultVSOut& operator+=(const DefaultVSOut& rhs) { return *this = *this + rhs; }

	friend DefaultVSOut operator-(const DefaultVSOut& lhs, const DefaultVSOut& rhs)
	{
		return
		{
			lhs.m_Position - rhs.m_Position,
			lhs.m_Test - rhs.m_Test
		};
	}
	DefaultVSOut& operator-=(const DefaultVSOut& rhs) { return *this = *this - rhs; }
	
	friend DefaultVSOut operator*(const DefaultVSOut& lhs, float rhs)
	{
		return
		{
			lhs.m_Position * rhs,
			lhs.m_Test * rhs
		};
	}
	friend DefaultVSOut operator*(float lhs, const DefaultVSOut& rhs) { return rhs * lhs; }
	DefaultVSOut& operator*=(float rhs) { return *this = *this * rhs; }

	static DefaultVSOut Lerp(DefaultVSOut v1, DefaultVSOut v2, float t) { return v1 + (v2 - v1) * t; }
};

class DefaultVertexShader : public VertexShader<DefaultVSIn, DefaultVSOut>
{
public:
	using VSIn = DefaultVSIn;
	using VSOut = DefaultVSOut;

	VSOut Main(const VSIn& vIn) override
	{
		return VSOut(vIn.m_Position * 5.0f, vIn.m_Position.x);
	}
};