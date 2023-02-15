#pragma once

#include "PixelShader.h"

class DefaultPixelShader : public PixelShader
{
public:
	using PSIn = Vec3;

	void Main(Vec3 pixel) override {}
};