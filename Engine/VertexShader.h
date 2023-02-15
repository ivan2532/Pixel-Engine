#pragma once

template <class VSIn, class VSOut>
class VertexShader
{
public:
	virtual VSOut Main(const VSIn& vIn) = 0;
};