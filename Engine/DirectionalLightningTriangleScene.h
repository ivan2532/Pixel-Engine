#pragma once

#include "MainWindow.h"
#include "Scene.h"
#include "DirectionalLightningShaderProgram.h"

class DirectionalLightningTriangleScene : public Scene<DirectionalLightningShaderProgram>
{
public:
	DirectionalLightningTriangleScene(Graphics& graphics, MainWindow& window);

	void Start() override;
	void Update() override;
	void Draw() override;

private:
	MainWindow& m_Window;
	GraphicsPipeline<DirectionalLightningShaderProgram> m_Pipeline;

	Entity m_Suzanne;
	std::vector<DirectionalLightningShaderProgram::VSIn> m_TriangleInput;
};