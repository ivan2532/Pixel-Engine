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
	void Clipping(VSOut& v1, VSOut& v2, VSOut& v3);
	void ScreenMapping(VSOut& v1, VSOut& v2, VSOut& v3);
	void Rasterization(VSOut& v1, VSOut& v2, VSOut& v3);
	void PixelProcessing(VSOut& fragment);
	void Merging(PSOut& p);

	#pragma endregion

	#pragma region Helper functions

	void ClipToScreenCoordinates(VSOut& v);
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

	for (auto vertex : m_InputVertices)
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
inline void GraphicsPipeline<ShaderProgram>::Clipping(VSOut& v1, VSOut& v2, VSOut& v3)
{
	// Here we will have vertex positions in range from (-1, -1, -1) to (1, 1, 1)
	// 
	// TODO: Clipping

	ScreenMapping(v1, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::ScreenMapping(VSOut& v1, VSOut& v2, VSOut& v3)
{
	// Here we will have vertex positions in range from (-1, -1, -1) to (1, 1, 1)
	ClipToScreenCoordinates(v1);
	ClipToScreenCoordinates(v2);
	ClipToScreenCoordinates(v3);
	Rasterization(v1, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Rasterization(VSOut& v1, VSOut& v2, VSOut& v3)
{
	// Sort vertices by y (from bottom to top on screen)
	if (v2.m_Position.y > v1.m_Position.y && v2.m_Position.y > v3.m_Position.y) std::swap(v2, v1);
	if (v3.m_Position.y > v2.m_Position.y) std::swap(v3, v2);

	if (v1.m_Position.y == v2.m_Position.y)
	{
		DrawFlatBottomTriangle(v1, v2, v3);
	}
	if (v2.m_Position.y == v3.m_Position.y)
	{
		DrawFlatTopTriangle(v1, v2, v3);
	}

	// Split triangle into flat top, and flat bottom triangles
	const float t = (v1.m_Position.y - v2.m_Position.y) / (v1.m_Position.y - v3.m_Position.y);
	VSOut splitVertex = VSOut::Lerp(v1, v3, t);

	// Draw the triangles
	DrawFlatTopTriangle(v1, splitVertex, v2);
	DrawFlatBottomTriangle(splitVertex, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::PixelProcessing(VSOut& fragment)
{
	Merging(m_PixelShader.Main(fragment));
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::Merging(PSOut& p)
{
	m_Graphics.PutPixel(p.m_Position.x, p.m_Position.y, p.m_Color);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::ClipToScreenCoordinates(VSOut& v)
{
	v.m_Position.x = Graphics::ScreenWidth / 2.0f * (1 + v.m_Position.x);
	v.m_Position.y = Graphics::ScreenHeight / 2.0f * (1 - v.m_Position.y);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatTopTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	DrawFlatTriangle(v1, v2, v1, v2, v1, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatBottomTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	DrawFlatTriangle(v1, v3, v1, v3, v2, v3);
}

template<class ShaderProgram>
inline void GraphicsPipeline<ShaderProgram>::DrawFlatTriangle(VSOut& bottom, VSOut& top, VSOut& leftFrom, VSOut& leftTo, VSOut& rightFrom, VSOut& rightTo)
{
	// Round because of point sampling (draw pixel if it's center is inside the triangle)
	const int startY = static_cast<int>(std::roundf(bottom.m_Position.y));
	const int endY = static_cast<int>(std::roundf(top.m_Position.y));
	if (startY == endY) return;

	const float deltaY = static_cast<float>(endY - startY);

	for (int curY = startY; curY >= endY; curY--)
	{
		const float tY = (curY - startY) / deltaY;
		VSOut left = VSOut::Lerp(leftFrom, leftTo, tY);
		VSOut right = VSOut::Lerp(rightFrom, rightTo, tY);

		const int startX = static_cast<int>(std::roundf(left.m_Position.x));
		const int endX = static_cast<int>(std::roundf(right.m_Position.x));
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
