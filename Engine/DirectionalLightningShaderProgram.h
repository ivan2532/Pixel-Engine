#pragma once

#include "Vec3.h"
#include "Mat4.h"
#include "Colors.h"

class DirectionalLightningShaderProgram
{
public:
	struct VSIn
	{
		Vec3 m_Position;
		Vec3 m_Normal;

		VSIn(Vec3 position, Vec3 normal)
			:
			m_Position(position),
			m_Normal(normal)
		{
		}
	};

	struct VSOut
	{
		Vec4 m_Position;
		Vec3 m_WorldPosition;
		Vec3 m_Normal;

		VSOut(Vec4 position, Vec3 worldPosition, Vec3 normal)
			:
			m_Position(position),
			m_WorldPosition(worldPosition),
			m_Normal(normal)
		{
		}

		friend VSOut operator+(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position + rhs.m_Position,
				lhs.m_WorldPosition + rhs.m_WorldPosition,
				lhs.m_Normal + rhs.m_Normal
			};
		}
		VSOut& operator+=(const VSOut& rhs) { return *this = *this + rhs; }

		friend VSOut operator-(const VSOut& lhs, const VSOut& rhs)
		{
			return
			{
				lhs.m_Position - rhs.m_Position,
				lhs.m_WorldPosition + rhs.m_WorldPosition,
				lhs.m_Normal - rhs.m_Normal
			};
		}
		VSOut& operator-=(const VSOut& rhs) { return *this = *this - rhs; }

		friend VSOut operator*(const VSOut& lhs, float rhs)
		{
			return
			{
				lhs.m_Position * rhs,
				lhs.m_WorldPosition + rhs,
				lhs.m_Normal * rhs
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
			Mat4 normalTransform = m_MV;
			normalTransform[0][3] = 0.0f;
			normalTransform[1][3] = 0.0f;
			normalTransform[2][3] = 0.0f;

			return VSOut
			(
				m_MVP * vIn.m_Position,
				m_MV * vIn.m_Position,
				Vec3::Normalize(normalTransform * vIn.m_Normal)
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
			//Vec3 lightDirection = Vec3::Normalize((static_cast<Vec3>(fragment.m_WorldPosition) - Vec3::Zero()));
			Vec3 lightDirection = Vec3(0.0f, 0.0f, 1.0f);
			Vec3 ambientLight(0.15f, 0.15f, 0.15f);
			Vec3 resultColor = Vec3::Dot(lightDirection, fragment.m_Normal) * Vec3::One();
			resultColor.x = std::max(ambientLight.x, resultColor.x);
			resultColor.y = std::max(ambientLight.y, resultColor.y);
			resultColor.z = std::max(ambientLight.z, resultColor.z);

			return
			{
				{
					static_cast<int>(fragment.m_Position.x),
					static_cast<int>(fragment.m_Position.y)
				},
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