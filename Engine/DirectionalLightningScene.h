#pragma once

#include "MainWindow.h"
#include "Scene.h"
#include "TexturedDirectionalLightningShaderProgram.h"

class DirectionalLightningScene : public Scene<TexturedDirectionalLightningShaderProgram>
{
public:
	DirectionalLightningScene(Graphics& graphics, MainWindow& window);

	void Start() override;
	void Update() override;
	void Draw() override;

private:
	MainWindow& m_Window;
	GraphicsPipeline<TexturedDirectionalLightningShaderProgram> m_Pipeline;

	Entity m_Model;
	std::vector<TexturedDirectionalLightningShaderProgram::VSIn> m_TriangleInput;
};