#pragma once

#include <cassert>
#include <memory>
#include <cmath>
#include <limits>

#include "stb_image.h"

#include "Vec3.h"
#include "Graphics.h"

template <class TShaderProgram>
class GraphicsPipeline
{
	typedef typename TShaderProgram::VertexShader VertexShader;
	typedef typename TShaderProgram::PixelShader PixelShader;
	typedef typename TShaderProgram::VSIn VSIn;
	typedef typename TShaderProgram::VSOut VSOut;
	typedef typename TShaderProgram::PSOut PSOut;

public:
	GraphicsPipeline(Graphics& graphics);
	~GraphicsPipeline();

	VertexShader& GetVertexShader() { return m_VertexShader; }
	PixelShader& GetPixelShader() { return m_PixelShader; }

	void BindIndices(const std::vector<size_t>& indices);
	void BindVertices(const std::vector<VSIn>& input);

	void LoadTexture(const std::string& path);
	void UnloadTexture();

	void Draw();
	void ClearZBuffer();

private:
	Graphics& m_Graphics;

	VertexShader m_VertexShader;
	PixelShader m_PixelShader;

	std::vector<size_t> m_InputIndices;
	std::vector<VSIn> m_InputVertices;

	std::vector<VSOut> m_TransformedVertices;

	float** zBuffer;

	unsigned char* m_TextureData = nullptr;
	int m_TextureWidth = 0;
	int m_TextureHeight = 0;
	int m_TextureNumOfChannels = 0;

	#pragma region Pipeline stages

	void VertexProcessing();
	void TriangleAssembly();
	void Clipping(VSOut& v1, VSOut& v2, VSOut& v3);
	void ScreenMapping(VSOut v1, VSOut v2, VSOut v3);
	void Rasterization(const VSOut& v1, const VSOut& v2, const VSOut& v3);
	void PixelProcessing(int screenX, int screenY, VSOut& fragment);
	void Merging(int screenX, int screenY, PSOut p);

	#pragma endregion

	#pragma region Helper functions

	void ClipSpaceToNDCSpaceVertex(VSOut& v);
	void ClipSpaceToNDCSpaceTriangle(VSOut& v1, VSOut& v2, VSOut& v3);
	void NDCSpaceToScreenSpaceVertex(VSOut& v);
	void NDCSpaceToScreenSpaceTriangle(VSOut& v1, VSOut& v2, VSOut& v3);

	void DrawFlatTopTriangle(const VSOut& v1, const VSOut& v2, const VSOut& v3);
	void DrawFlatBottomTriangle(const VSOut& v1, const VSOut& v2, const VSOut& v3);
	void DrawFlatTriangle(const VSOut& leftEdgeFrom, const VSOut& leftEdgeTo, const VSOut& rightEdgeFrom, const VSOut& rightEdgeTo);

	#pragma endregion
};

#pragma region Definitions

template<class TShaderProgram>
inline GraphicsPipeline<TShaderProgram>::GraphicsPipeline(Graphics& graphics)
	:
	m_Graphics(graphics)
{
	zBuffer = new float*[Graphics::ScreenHeight];
	for (int i = 0; i < Graphics::ScreenHeight; i++)
	{
		zBuffer[i] = new float[Graphics::ScreenWidth];
	}
}

template<class TShaderProgram>
inline GraphicsPipeline<TShaderProgram>::~GraphicsPipeline()
{
	for (int i = 0; i < Graphics::ScreenHeight; i++)
	{
		delete[] zBuffer[i];
	}

	delete[] zBuffer;

	UnloadTexture();
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::BindIndices(const std::vector<size_t>& indices)
{
	assert(indices.size() % 3 == 0);
	m_InputIndices = indices;
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::BindVertices(const std::vector<VSIn>& input)
{
	m_InputVertices = input;
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::LoadTexture(const std::string& path)
{
	UnloadTexture();
	m_TextureData = stbi_load(path.c_str(), &m_TextureWidth, &m_TextureHeight, &m_TextureNumOfChannels, 3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::UnloadTexture()
{
	stbi_image_free(m_TextureData);
	m_TextureData = nullptr;
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::Draw()
{
	VertexProcessing();
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::VertexProcessing()
{
	m_TransformedVertices.clear();

	for (const auto& vertex : m_InputVertices)
	{
		m_TransformedVertices.push_back(m_VertexShader.Main(vertex));
	}

	TriangleAssembly();
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::TriangleAssembly()
{
	for (auto it = m_TransformedVertices.begin(); it != m_TransformedVertices.end(); std::advance(it, 3))
	{
		Clipping(*it, *(it + 1), *(it + 2));
	}
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::Clipping(VSOut& v1, VSOut& v2, VSOut& v3)
{
	// Clip triangles completley out of the view volume
	// Left and right plane
	if (v1.m_Position.x <= -v1.m_Position.w && v2.m_Position.x <= -v2.m_Position.w && v3.m_Position.x <= -v3.m_Position.w) return;
	if (v1.m_Position.x >= v1.m_Position.w && v2.m_Position.x >= v2.m_Position.w && v3.m_Position.x >= v3.m_Position.w) return;

	// Bottom and top plane
	if (v1.m_Position.y <= -v1.m_Position.w && v2.m_Position.y <= -v2.m_Position.w && v3.m_Position.y <= -v3.m_Position.w) return;
	if (v1.m_Position.y >= v1.m_Position.w && v2.m_Position.y >= v2.m_Position.w && v3.m_Position.y >= v3.m_Position.w) return;

	// Near and far plane
	if (v1.m_Position.z <= -v1.m_Position.w && v2.m_Position.z <= -v2.m_Position.w && v3.m_Position.z <= -v3.m_Position.w) return;
	if (v1.m_Position.z >= v1.m_Position.w && v2.m_Position.z >= v2.m_Position.w && v3.m_Position.z >= v3.m_Position.w) return;
	
	// Sort vertices by x (descending)
	if (v1.m_Position.x < v2.m_Position.x) std::swap(v1, v2);
	if (v2.m_Position.x < v3.m_Position.x) std::swap(v2, v3);
	if (v1.m_Position.x < v2.m_Position.x) std::swap(v1, v2);

	constexpr float nearPlaneNDC = -1.0f;

	// First scenario: two vertices are behind the nearPlane plane
	auto TwoVerticesOutClipping = [this, nearPlaneNDC](VSOut& vOut1, VSOut& vOut2, VSOut& vIn)
	{
		// First transform our vertices to NDC space, so we can clip
		ClipSpaceToNDCSpaceTriangle(vOut1, vOut2, vIn);

		const float t1 = (nearPlaneNDC - vOut1.m_Position.z) / (vIn.m_Position.z - vOut1.m_Position.z);
		const float t2 = (nearPlaneNDC - vOut2.m_Position.z) / (vIn.m_Position.z - vOut2.m_Position.z);
		vOut1 = VSOut::Lerp(vOut1, vIn, t1);
		vOut2 = VSOut::Lerp(vOut2, vIn, t2);

		ScreenMapping(vOut1, vOut2, vIn);
	};

	if (v1.m_Position.z <= -v1.m_Position.w && v2.m_Position.z <= -v2.m_Position.w)
	{
		TwoVerticesOutClipping(v1, v2, v3);
		return;
	}
	else if (v2.m_Position.z <= -v2.m_Position.w && v3.m_Position.z <= -v3.m_Position.w)
	{
		TwoVerticesOutClipping(v2, v3, v1);
		return;
	}
	else if (v1.m_Position.z <= -v1.m_Position.w && v3.m_Position.z <= -v3.m_Position.w)
	{
		TwoVerticesOutClipping(v1, v3, v2);
		return;
	}

	// Second scenario: one vertex is behind the nearPlane plane
	auto OneVertexOutClipping = [this, nearPlaneNDC](VSOut& vOut, VSOut& vIn1, VSOut& vIn2)
	{
		// First transform our vertices to NDC space, so we can clip
		ClipSpaceToNDCSpaceTriangle(vOut, vIn1, vIn2);

		const float tBottom = (nearPlaneNDC - vOut.m_Position.z) / (vIn1.m_Position.z - vOut.m_Position.z);
		const float tTop = (nearPlaneNDC - vOut.m_Position.z) / (vIn2.m_Position.z - vOut.m_Position.z);
		VSOut vBottomIntersect = VSOut::Lerp(vOut, vIn1, tBottom);
		VSOut vTopIntersect = VSOut::Lerp(vOut, vIn2, tTop);

		ScreenMapping(vBottomIntersect, vTopIntersect, vIn2);
		ScreenMapping(vBottomIntersect, vIn1, vIn2);
	};

	if (v1.m_Position.z <= -v1.m_Position.w)
	{
		OneVertexOutClipping(v1, v2, v3);
		return;
	}
	else if (v2.m_Position.z <= -v2.m_Position.w)
	{
		OneVertexOutClipping(v2, v1, v3);
		return;
	}
	else if (v3.m_Position.z <= -v3.m_Position.w)
	{
		OneVertexOutClipping(v3, v1, v2);
		return;
	}

	// Third scenario: no vertex is behind the nearPlane plane
	ClipSpaceToNDCSpaceTriangle(v1, v2, v3);
	ScreenMapping(v1, v2, v3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::ScreenMapping(VSOut v1, VSOut v2, VSOut v3)
{
	NDCSpaceToScreenSpaceTriangle(v1, v2, v3);
	Rasterization(v1, v2, v3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::Rasterization(const VSOut& v1, const VSOut& v2, const VSOut& v3)
{
	// Sort vertices by y (from bottom to top on screen)
	// because the order could be messed up from clipping
	const VSOut* pv1 = &v1;
	const VSOut* pv2 = &v2;
	const VSOut* pv3 = &v3;
	if (pv1->m_Position.y < pv2->m_Position.y) std::swap(pv1, pv2);
	if (pv2->m_Position.y < pv3->m_Position.y) std::swap(pv2, pv3);
	if (pv1->m_Position.y < pv2->m_Position.y) std::swap(pv1, pv2);

	if (pv1->m_Position.y == pv2->m_Position.y)
	{
		DrawFlatBottomTriangle(*pv1, *pv2, *pv3);
		return;
	}
	if (pv2->m_Position.y == pv3->m_Position.y)
	{
		DrawFlatTopTriangle(*pv1, *pv2, *pv3);
		return;
	}

	// Split triangle into flat top, and flat bottom triangles
	const float t = (pv1->m_Position.y - pv2->m_Position.y) / (pv1->m_Position.y - pv3->m_Position.y);
	VSOut vSplit = VSOut::Lerp(*pv1, *pv3, t);

	// Draw the triangles
	DrawFlatTopTriangle(*pv1, vSplit, *pv2);
	DrawFlatBottomTriangle(vSplit, *pv2, *pv3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::PixelProcessing(int screenX, int screenY, VSOut& fragment)
{
	if (fragment.m_Position.z >= zBuffer[screenY][screenX]) return;

	zBuffer[screenY][screenX] = fragment.m_Position.z;

	if (m_TextureData != nullptr)
	{
		Vei2 textureCoordinates
		(
			static_cast<int>(fragment.m_UvCoordinates.x * m_TextureWidth),
			static_cast<int>((1.0f - fragment.m_UvCoordinates.y) * m_TextureHeight)
		);

		int yOffset = textureCoordinates.y * m_TextureWidth;
		int xOffset = textureCoordinates.x;

		int textureArrayIndex = std::min((yOffset + xOffset) * 3, (m_TextureWidth * m_TextureHeight) * 3 - 1);

		fragment.m_Color = Vec3
		(
			static_cast<float>(m_TextureData[textureArrayIndex]) / 255.0f,
			static_cast<float>(m_TextureData[textureArrayIndex + 1]) / 255.0f,
			static_cast<float>(m_TextureData[textureArrayIndex + 2]) / 255.0f
		);
	}
	else
	{
		fragment.m_Color = Vec3::One();
	}

	Merging(screenX, screenY, m_PixelShader.Main(fragment));
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::Merging(int screenX, int screenY, PSOut p)
{
	m_Graphics.PutPixel(screenX, screenY, p.m_Color);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::ClipSpaceToNDCSpaceVertex(VSOut& v)
{
	const auto wInverse = 1.0f / v.m_Position.w;
	v *= wInverse;
	v.m_Position.w = wInverse;
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::ClipSpaceToNDCSpaceTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	ClipSpaceToNDCSpaceVertex(v1);
	ClipSpaceToNDCSpaceVertex(v2);
	ClipSpaceToNDCSpaceVertex(v3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::NDCSpaceToScreenSpaceVertex(VSOut& v)
{
	v.m_Position.x = Graphics::ScreenWidth / 2.0f * (1 + v.m_Position.x);
	v.m_Position.y = Graphics::ScreenHeight / 2.0f * (1 - v.m_Position.y);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::NDCSpaceToScreenSpaceTriangle(VSOut& v1, VSOut& v2, VSOut& v3)
{
	NDCSpaceToScreenSpaceVertex(v1);
	NDCSpaceToScreenSpaceVertex(v2);
	NDCSpaceToScreenSpaceVertex(v3);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::ClearZBuffer()
{
	for (int i = 0; i < Graphics::ScreenHeight; i++)
	{
		for (int j = 0; j < Graphics::ScreenWidth; j++)
		{
			zBuffer[i][j] = std::numeric_limits<float>::max();
		}
	}
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::DrawFlatTopTriangle(const VSOut& v1, const VSOut& v2, const VSOut& v3)
{
	const VSOut* pv2 = &v2;
	const VSOut* pv3 = &v3;
	if (pv2->m_Position.x > pv3->m_Position.x)
		std::swap(pv2, pv3);

	DrawFlatTriangle
	(
		*pv2, v1,
		*pv3, v1
	);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::DrawFlatBottomTriangle(const VSOut& v1, const VSOut& v2, const VSOut& v3)
{
	const VSOut* pv1 = &v1;
	const VSOut* pv2 = &v2;
	if (pv1->m_Position.x > pv2->m_Position.x)
		std::swap(pv1, pv2);

	DrawFlatTriangle
	(
		v3, *pv1,
		v3, *pv2
	);
}

template<class TShaderProgram>
inline void GraphicsPipeline<TShaderProgram>::DrawFlatTriangle(const VSOut& leftEdgeFrom, const VSOut& leftEdgeTo, const VSOut& rightEdgeFrom, const VSOut& rightEdgeTo)
{
	// We're always going to send leftEdgeFrom and rightEdgeFrom to be at a lower y coordinate,
	// which means they are always going to be on the "top" of the triangle

	auto leftEdgeInterpolant = leftEdgeFrom;
	auto rightEdgeInterpolant = rightEdgeFrom;

	const float deltaY = leftEdgeTo.m_Position.y - leftEdgeFrom.m_Position.y;
	const auto leftStep = (leftEdgeTo - leftEdgeFrom) / deltaY;
	const auto rightStep = (rightEdgeTo - rightEdgeFrom) / deltaY;

	int startY = std::max(static_cast<int>(std::ceilf(leftEdgeFrom.m_Position.y - 0.5f)), 0);
	int endY = std::min(static_cast<int>(std::ceilf(leftEdgeTo.m_Position.y - 0.5f)), Graphics::ScreenHeight - 1);

	// Prestep
	leftEdgeInterpolant += (static_cast<float>(startY) + 0.5f - leftEdgeFrom.m_Position.y) * leftStep;
	rightEdgeInterpolant += (static_cast<float>(startY) + 0.5f - rightEdgeFrom.m_Position.y) * rightStep;

	for (int curY = startY; curY < endY; curY++, leftEdgeInterpolant += leftStep, rightEdgeInterpolant += rightStep)
	{
		auto xInterpolant = leftEdgeInterpolant;

		const float deltaX = rightEdgeInterpolant.m_Position.x - leftEdgeInterpolant.m_Position.x;
		const auto xStep = (rightEdgeInterpolant - leftEdgeInterpolant) / deltaX;

		int startX = std::max(static_cast<int>(std::ceilf(leftEdgeInterpolant.m_Position.x - 0.5f)), 0);
		int endX = std::min(static_cast<int>(std::ceilf(rightEdgeInterpolant.m_Position.x - 0.5f)), Graphics::ScreenWidth - 1);

		xInterpolant += (static_cast<float>(startX) + 0.5f - leftEdgeInterpolant.m_Position.x) * xStep;

		for (int curX = startX; curX < endX; curX++, xInterpolant += xStep)
		{
			auto fragment = xInterpolant;
			const float w = 1.0f / fragment.m_Position.w;
			fragment *= w;

			PixelProcessing(curX, curY, fragment);
		}
	}
}

#pragma endregion
