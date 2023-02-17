#pragma once

#include <cassert>
#include <memory>
#include <cmath>

#include "Graphics.h"

template <class ShaderProgram>
class GraphicsPipeline
{
	typedef typename ShaderProgram::VertexShader VertexShader;
	typedef typename ShaderProgram::PixelShader PixelShader;
	typedef typename ShaderProgram::VSIn VSIn;
	typedef typename ShaderProgram::VSOut VSOut;
	typedef typename ShaderProgram::PSOut PSOut;

public:
	GraphicsPipeline(Graphics& graphics);

	void BindIndices(const std::vector<size_t>& value);
	void BindVertices(const std::vector<VSIn>& value);
	void Draw();

private:
	Graphics& m_Graphics;

	VertexShader m_VertexShader;
	PixelShader m_PixelShader;

	std::vector<size_t> m_InputIndices;
	std::vector<VSIn> m_InputVertices;

	std::vector<VSOut> m_TransformedVertices;

	#pragma region Pipeline stages

	void VertexProcessing();
	void TriangleAssembly();
	void Clipping(VSOut v1, VSOut v2, VSOut v3);
	void ScreenMapping(VSOut v1, VSOut v2, VSOut v3);
	void Rasterization(VSOut v1, VSOut v2, VSOut v3);
	void PixelProcessing(VSOut fragment);
	void Merging(PSOut p);

	#pragma endregion

	#pragma region Helper functions

	void ClipSpaceToScreenSpace(VSOut& v);
	void DrawFlatTopTriangle(VSOut& v1, VSOut& v2, VSOut& v3);
	void DrawFlatBottomTriangle(VSOut& v1, VSOut& v2, VSOut& v3);
	void DrawFlatTriangle(VSOut& bottom, VSOut& top, VSOut& leftFrom, VSOut& leftTo, VSOut& rightFrom, VSOut& rightTo);

	#pragma endregion
};

#pragma region Definitions

template<class ShaderProgram>
inline GraphicsPipeline<ShaderProgram>::GraphicsPipeline(Graphics& graphics)
	:
	m_Graphics(graphics)
{
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::BindIndices(const std::vector<size_t>& value)
{
	assert(value.size() % 3 == 0);
	m_InputIndices = value;
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::BindVertices(const std::vector<VSIn>& value)
{
	m_InputVertices = value;
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Draw()
{
	VertexProcessing();
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::VertexProcessing()
{
	m_TransformedVertices.clear();

	for (const auto& vertex : m_InputVertices)
	{
		m_TransformedVertices.push_back(m_VertexShader.Main(vertex));
	}

	TriangleAssembly();
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::TriangleAssembly()
{
	for (auto it = m_TransformedVertices.begin(); it != m_TransformedVertices.end(); std::advance(it, 3))
	{
		Clipping(*it, *(it + 1), *(it + 2));
	}
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Clipping(VSOut v1, VSOut v2, VSOut v3)
{
	/*
	 Geometric clipping against the near plane, for left and right planes we'll
	 use raster clipping, and we'll ignore far plane clipping.
	 Here we will have vertex positions in range from (-1, -1, -1) to (1, 1, 1).
	*/

	constexpr float nearPlane = -1.0f;

	if (v1.m_Position.z < nearPlane && v2.m_Position.z < nearPlane && v3.m_Position.z < nearPlane) return;
	
	// Sort vertices by x (descending)
	if (v1.m_Position.x < v2.m_Position.x) std::swap(v1, v2);
	if (v2.m_Position.x < v3.m_Position.x) std::swap(v2, v3);
	if (v1.m_Position.x < v2.m_Position.x) std::swap(v1, v2);

	// First scenario: two vertices are behind the nearPlane plane
	if (v1.m_Position.z < nearPlane && v2.m_Position.z < nearPlane)
	{
		const float t1 = (nearPlane - v1.m_Position.z) / (v3.m_Position.z - v1.m_Position.z);
		const float t2 = (nearPlane - v2.m_Position.z) / (v3.m_Position.z - v2.m_Position.z);
		v1 = VSOut::Lerp(v1, v3, t1);
		v2 = VSOut::Lerp(v2, v3, t2);

		ScreenMapping(v1, v2, v3);
		return;
	}
	else if (v2.m_Position.z < nearPlane && v3.m_Position.z < nearPlane)
	{
		const float t2 = (nearPlane - v2.m_Position.z) / (v1.m_Position.z - v2.m_Position.z);
		const float t3 = (nearPlane - v3.m_Position.z) / (v1.m_Position.z - v3.m_Position.z);
		v2 = VSOut::Lerp(v2, v1, t2);
		v3 = VSOut::Lerp(v3, v1, t3);

		ScreenMapping(v1, v2, v3);
		return;
	}
	else if (v1.m_Position.z < nearPlane && v3.m_Position.z < nearPlane)
	{
		const float t1 = (nearPlane - v1.m_Position.z) / (v2.m_Position.z - v1.m_Position.z);
		const float t3 = (nearPlane - v3.m_Position.z) / (v2.m_Position.z - v3.m_Position.z);
		v1 = VSOut::Lerp(v1, v2, t1);
		v3 = VSOut::Lerp(v3, v2, t3);

		ScreenMapping(v1, v2, v3);
		return;
	}

	// Second scenario: one vertex is behind the nearPlane plane
	// Here we can mess up the order of vertices (bottom to top)
	if (v1.m_Position.z < nearPlane)
	{
		const float tBottom = (nearPlane - v1.m_Position.z) / (v2.m_Position.z - v1.m_Position.z);
		const float tTop = (nearPlane - v1.m_Position.z) / (v3.m_Position.z - v1.m_Position.z);
		VSOut vBottomIntersect = VSOut::Lerp(v1, v2, tBottom);
		VSOut vTopIntersect = VSOut::Lerp(v1, v3, tTop);

		ScreenMapping(vBottomIntersect, vTopIntersect, v3);
		ScreenMapping(vBottomIntersect, v2, v3);
		return;
	}
	else if (v2.m_Position.z < nearPlane)
	{
		const float tBottom = (nearPlane - v2.m_Position.z) / (v1.m_Position.z - v2.m_Position.z);
		const float tTop = (nearPlane - v2.m_Position.z) / (v3.m_Position.z - v2.m_Position.z);
		VSOut vBottomIntersect = VSOut::Lerp(v2, v1, tBottom);
		VSOut vTopIntersect = VSOut::Lerp(v2, v3, tTop);

		ScreenMapping(vBottomIntersect, vTopIntersect, v3);
		ScreenMapping(v1, vBottomIntersect, v3);
		return;
	}
	else if (v3.m_Position.z < nearPlane)
	{
		const float tBottom = (nearPlane - v3.m_Position.z) / (v1.m_Position.z - v3.m_Position.z);
		const float tTop = (nearPlane - v3.m_Position.z) / (v2.m_Position.z - v3.m_Position.z);
		VSOut vBottomIntersect = VSOut::Lerp(v3, v1, tBottom);
		VSOut vTopIntersect = VSOut::Lerp(v3, v2, tTop);

		ScreenMapping(vBottomIntersect, v2, vTopIntersect);
		ScreenMapping(v1, vBottomIntersect, v2);
		return;
	}

	// Third scenario: no vertex is behind the nearPlane plane
	ScreenMapping(v1, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::ScreenMapping(VSOut v1, VSOut v2, VSOut v3)
{
	ClipSpaceToScreenSpace(v1);
	ClipSpaceToScreenSpace(v2);
	ClipSpaceToScreenSpace(v3);
	Rasterization(v1, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Rasterization(VSOut v1, VSOut v2, VSOut v3)
{
	/*m_Graphics.PutPixel(v1.m_Position.x, v1.m_Position.y, Colors::White);
	m_Graphics.PutPixel(v2.m_Position.x, v2.m_Position.y, Colors::White);
	m_Graphics.PutPixel(v3.m_Position.x, v3.m_Position.y, Colors::White);*/

	// Sort vertices by y (from bottom to top on screen)
	// because the order could be messed up from clipping
	if (v1.m_Position.y < v2.m_Position.y) std::swap(v1, v2);
	if (v2.m_Position.y < v3.m_Position.y) std::swap(v2, v3);
	if (v1.m_Position.y < v2.m_Position.y) std::swap(v1, v2);

	if (v1.m_Position.y == v2.m_Position.y)
	{
		DrawFlatBottomTriangle(v1, v2, v3);
		return;
	}
	if (v2.m_Position.y == v3.m_Position.y)
	{
		DrawFlatTopTriangle(v1, v2, v3);
		return;
	}

	// Split triangle into flat top, and flat bottom triangles
	const float t = (v1.m_Position.y - v2.m_Position.y) / (v1.m_Position.y - v3.m_Position.y);
	VSOut vSplit = VSOut::Lerp(v1, v3, t);

	// Draw the triangles
	DrawFlatTopTriangle(v1, vSplit, v2);
	DrawFlatBottomTriangle(vSplit, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::PixelProcessing(VSOut fragment)
{
	Merging(m_PixelShader.Main(fragment));
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Merging(PSOut p)
{
	m_Graphics.PutPixel(p.m_Position.x, p.m_Position.y, p.m_Color);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::ClipSpaceToScreenSpace(VSOut& v)
{
	v.m_Position.x = Graphics::ScreenWidth / 2.0f * (1 + v.m_Position.x);
	v.m_Position.y = Graphics::ScreenHeight / 2.0f * (1 - v.m_Position.y);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatTopTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	DrawFlatTriangle
	(
		v1, v2, 
		v1, v2.m_Position.x < v3.m_Position.x ? v2 : v3,
		v1, v2.m_Position.x < v3.m_Position.x ? v3 : v2
	);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatBottomTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	DrawFlatTriangle
	(
		v1, v3,
		v1.m_Position.x < v2.m_Position.x ? v1 : v2, v3,
		v1.m_Position.x < v2.m_Position.x ? v2 : v1, v3
	);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatTriangle(VSOut& bottom, VSOut& top, VSOut& leftFrom, VSOut& leftTo, VSOut& rightFrom, VSOut& rightTo)
{
	// Round because of point sampling (draw pixel if it's center is inside the triangle)
	int startY = static_cast<int>(std::roundf(bottom.m_Position.y));
	int endY = static_cast<int>(std::roundf(top.m_Position.y));
	// Raster clipping
	startY = std::clamp(startY, 0, Graphics::ScreenHeight);
	endY = std::clamp(endY, 0, Graphics::ScreenHeight);

	if (startY == endY) return;

	const float deltaY = static_cast<float>(endY - startY);

	for (int curY = startY; curY >= endY; curY--)
	{
		const float tY = (curY - startY) / deltaY;
		VSOut left = VSOut::Lerp(leftFrom, leftTo, tY);
		VSOut right = VSOut::Lerp(rightFrom, rightTo, tY);

		int startX = static_cast<int>(std::roundf(left.m_Position.x));
		int endX = static_cast<int>(std::roundf(right.m_Position.x));
		startX = std::clamp(startX, 0, Graphics::ScreenWidth);
		endX = std::clamp(endX, 0, Graphics::ScreenWidth);

		if (startX == endX) continue;

		const float deltaX = static_cast<float>(endX - startX);

		for (int curX = startX; curX <= endX; curX++)
		{
			const float tX = (curX - startX) / deltaX;
			VSOut fragment = VSOut::Lerp(left, right, tX);
			fragment.m_Position.x = static_cast<float>(curX);
			fragment.m_Position.y = static_cast<float>(curY);

			PixelProcessing(fragment);
		}
	}
}

#pragma endregion
