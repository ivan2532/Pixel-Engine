#pragma once

#include <vector>

#include "GraphicsPipeline.h"
#include "Entity.h"

template<class TShaderProgram>
class Scene
{
public:
	virtual void Start() { }
	virtual void Update() { }

	virtual void Draw() = 0;

public:
	Vec3 m_CameraPosition;
	Vec3 m_CameraEulerAngles;
};