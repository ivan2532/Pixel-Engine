#pragma once

#include "Vec3.h"
#include "Colors.h"

class DefaultShaderProgram
{
public:
	struct VSIn
	{
		Vec3 m_Position;

		VSIn(Vec3 position)
			:
			m_Position(position)
		{
		}
	};

	struct VSOut
	{
		Vec3 m_Position;
		float m_Test;

		VSOut(Vec3 position, float test)
			:
			m_Position(position),
			m_Test(test)
		{
		}

		friend VSOut operator+(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position + rhs.m_Position,
				lhs.m_Test + rhs.m_Test
			};
		}
		VSOut& operator+=(const VSOut& rhs) { return *this = *this + rhs; }

		friend VSOut operator-(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position - rhs.m_Position,
				lhs.m_Test - rhs.m_Test
			};
		}
		VSOut& operator-=(const VSOut& rhs) { return *this = *this - rhs; }

		friend VSOut operator*(const VSOut& lhs, float rhs)
		{
			return
			{
				lhs.m_Position * rhs,
				lhs.m_Test * rhs
			};
		}
		friend VSOut operator*(float lhs, const VSOut& rhs) { return rhs * lhs; }
		VSOut& operator*=(float rhs) { return *this = *this * rhs; }

		static VSOut Lerp(VSOut v1, VSOut v2, float t) { return v1 + (v2 - v1) * t; }
	};

	class VertexShader
	{
	public:
		VSOut Main(const VSIn& vIn)
		{
			return VSOut(vIn.m_Position, vIn.m_Position.x);
		}
	};

	struct PSOut
	{
		Vei2 m_Position;
		float z;

		Color m_Color;
	};

	class PixelShader
	{
	public:
		PSOut Main(const VSOut& fragment)
		{
			return
			{
				{
					static_cast<int>(fragment.m_Position.x),
					static_cast<int>(fragment.m_Position.y)
				},
				0.0f,
				Colors::Yellow
			};
		}
	};
};