#pragma once

#include "Vec3.h"
#include "Mat4.h"
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
		Vec4 m_Position;
		Vec3 m_Color;

		VSOut(Vec4 position, Vec3 color)
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

		template <typename T> friend VSOut operator*(const VSOut& lhs, T rhs)
		{
			return
			{
				lhs.m_Position * rhs,
				lhs.m_Color * rhs
			};
		}
		template <typename T> friend VSOut operator*(T lhs, const VSOut& rhs) { return rhs * lhs; }
		template <typename T> VSOut& operator*=(T rhs) { return *this = *this * rhs; }

		template <typename T> friend VSOut operator/(const VSOut& lhs, T rhs) { return lhs * (1.0f / rhs); }
		template <typename T> VSOut& operator/=(T rhs) { return *this = *this / rhs; }

		template <typename T> static VSOut Lerp(VSOut v1, VSOut v2, T t) { return v1 + (v2 - v1) * t; }
	};

	class VertexShader
	{
	public:
		VSOut Main(const VSIn& vIn)
		{
			return VSOut
			(
				m_MVP * vIn.m_Position,
				vIn.m_Color
			);
		}

		void SetMVP(const Mat4& value) { m_MVP = value; }
		void SetMV(const Mat4& value) { m_MV = value; }

	private:
		Mat4 m_MVP;
		Mat4 m_MV;
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
				fragment.m_Position.z,
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