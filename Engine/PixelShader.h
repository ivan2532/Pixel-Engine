#pragma once

#include "Vec3.h"

class PixelShader
{
public:
	virtual void Main(Vec3 pixel) = 0;
};