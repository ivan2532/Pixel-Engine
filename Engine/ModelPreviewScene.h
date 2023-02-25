#pragma once

#include "MainWindow.h"
#include "Scene.h"
#include "TexturedDirectionalLightningShaderProgram.h"

class ModelPreviewScene : public Scene<TexturedDirectionalLightningShaderProgram>
{
public:
	ModelPreviewScene(Graphics& graphics, MainWindow& window);

	void Start() override;
	void Update() override;
	void Draw() override;

private:
	MainWindow& m_Window;
	GraphicsPipeline<TexturedDirectionalLightningShaderProgram> m_Pipeline;

	Entity m_Model;
	std::vector<TexturedDirectionalLightningShaderProgram::VSIn> m_TriangleInput;
};