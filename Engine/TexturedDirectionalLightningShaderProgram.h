#pragma once

#include "Vec3.h"
#include "Mat4.h"
#include "Colors.h"

class TexturedDirectionalLightningShaderProgram
{
public:
	struct VSIn
	{
		Vec3 m_Position;
		Vec3 m_Normal;
		Vec2 m_UvCoordinates;

		VSIn(Vec3 position, Vec3 normal, Vec2 uvCoordinates)
			:
			m_Position(position),
			m_Normal(normal),
			m_UvCoordinates(uvCoordinates)
		{
		}
	};

	struct VSOut
	{
		Vec4 m_Position;
		Vec3 m_WorldPosition;
		Vec3 m_Normal;
		Vec2 m_UvCoordinates;
		Vec3 m_Color;

		VSOut(Vec4 position, Vec3 worldPosition, Vec3 normal, Vec2 uvCoordinates)
			:
			m_Position(position),
			m_WorldPosition(worldPosition),
			m_Normal(normal),
			m_UvCoordinates(uvCoordinates)
		{
		}

		friend VSOut operator+(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position + rhs.m_Position,
				lhs.m_WorldPosition + rhs.m_WorldPosition,
				lhs.m_Normal + rhs.m_Normal,
				lhs.m_UvCoordinates + rhs.m_UvCoordinates
			};
		}
		VSOut& operator+=(const VSOut& rhs) { return *this = *this + rhs; }

		friend VSOut operator-(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position - rhs.m_Position,
				lhs.m_WorldPosition + rhs.m_WorldPosition,
				lhs.m_Normal - rhs.m_Normal,
				lhs.m_UvCoordinates - rhs.m_UvCoordinates
			};
		}
		VSOut& operator-=(const VSOut& rhs) { return *this = *this - rhs; }

		template <typename T> friend VSOut operator*(const VSOut& lhs, T rhs)
		{
			return
			{
				lhs.m_Position * rhs,
				lhs.m_WorldPosition + rhs,
				lhs.m_Normal * rhs,
				lhs.m_UvCoordinates * rhs
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
				m_MV * vIn.m_Position,
				Vec3::Normalize(m_MV * Vec4(vIn.m_Normal.x, vIn.m_Normal.y, vIn.m_Normal.z, 0.0f)),
				vIn.m_UvCoordinates
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
		Vec2 m_UvCoordinates;
		float z;

		Color m_Color;
	};

	class PixelShader
	{
	public:
		PSOut Main(const VSOut& fragment)
		{
			Vec3 lightDirection = Vec3(0.0f, 0.0f, 1.0f);
			float ambientLightning = 0.15f;

			auto lightFactor = std::max(Vec3::Dot(lightDirection, fragment.m_Normal), ambientLightning);
			Vec3 resultColor = lightFactor * fragment.m_Color;

			return
			{
				{
					static_cast<int>(fragment.m_Position.x),
					static_cast<int>(fragment.m_Position.y)
				},
				fragment.m_UvCoordinates,
				fragment.m_Position.z,
				Color
				(
					static_cast<unsigned char>(resultColor.x * 255),
					static_cast<unsigned char>(resultColor.y * 255),
					static_cast<unsigned char>(resultColor.z * 255)
				)
			};
		}
	};
};