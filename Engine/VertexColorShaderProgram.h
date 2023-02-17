#pragma once

#include "Vec3.h"
#include "Colors.h"

class VertexColorShaderProgram
{
public:
	struct VSIn
	{
		Vec3 m_Position;
		Vec3 m_Color;

		VSIn(Vec3 position, Vec3 color)
			:
			m_Position(position),
			m_Color(color)
		{
		}
	};

	struct VSOut
	{
		Vec3 m_Position;
		Vec3 m_Color;

		VSOut(Vec3 position, Vec3 color)
			:
			m_Position(position),
			m_Color(color)
		{
		}

		friend VSOut operator+(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position + rhs.m_Position,
				lhs.m_Color + rhs.m_Color
			};
		}
		VSOut& operator+=(const VSOut& rhs) { return *this = *this + rhs; }

		friend VSOut operator-(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position - rhs.m_Position,
				lhs.m_Color - rhs.m_Color
			};
		}
		VSOut& operator-=(const VSOut& rhs) { return *this = *this - rhs; }

		friend VSOut operator*(const VSOut& lhs, float rhs)
		{
			return
			{
				lhs.m_Position * rhs,
				lhs.m_Color * rhs
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
			return VSOut(vIn.m_Position, vIn.m_Color);
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
				Color
				(
					static_cast<unsigned char>(fragment.m_Color.x * 255),
					static_cast<unsigned char>(fragment.m_Color.y * 255),
					static_cast<unsigned char>(fragment.m_Color.z * 255)
				)
			};
		}
	};
};