#pragma once

#include <memory>

template <class TVertexShader, class TPixelShader>
class GraphicsPipeline
{
	typedef typename TVertexShader::VSIn VSIn;
	typedef typename TVertexShader::VSOut VSOut;
	typedef typename TPixelShader::PSIn PSIn;

public:
	void BindIndices(const std::vector<size_t>& value) { m_InputIndices = value; }
	void BindVertices(const std::vector<VSIn>& value) { m_InputVertices = value; }
	void Draw() { VertexProcessing(); };

private:
	TVertexShader m_VertexShader;
	TPixelShader m_PixelShader;

	std::vector<size_t> m_InputIndices;
	std::vector<VSIn> m_InputVertices;

	std::vector<VSOut> m_TransformedVertices;

	void VertexProcessing();
	void TriangleAssembly();
	void Clipping(VSOut& v1, VSOut& v2, VSOut& v3);
	void ScreenMapping(VSOut& v1, VSOut& v2, VSOut& v3);
	void Rasterization(VSOut& v1, VSOut& v2, VSOut& v3);
	void PixelProcessing(PSIn& p);
};

#pragma region Definitions

template<class TVertexShader, class TPixelShader>
inline void GraphicsPipeline<TVertexShader, TPixelShader>::VertexProcessing()
{
	for (auto vertex : m_InputVertices)
	{
		m_TransformedVertices.push_back(m_VertexShader.Main(vertex));
	}

	TriangleAssembly();
}

template<class TVertexShader, class TPixelShader>
inline void GraphicsPipeline<TVertexShader, TPixelShader>::TriangleAssembly()
{
	for (auto it = m_TransformedVertices.begin(); it != m_TransformedVertices.end(); std::advance(it, 3))
	{
		Clipping(*it, *(it + 1), *(it + 2));
	}
}

template<class TVertexShader, class TPixelShader>
inline void GraphicsPipeline<TVertexShader, TPixelShader>::Clipping(VSOut& v1, VSOut& v2, VSOut& v3)
{
	// TODO: Clipping
}

#pragma endregion
