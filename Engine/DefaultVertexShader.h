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