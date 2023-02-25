#include "ModelPreviewScene.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>

ModelPreviewScene::ModelPreviewScene(Graphics& graphics, MainWindow& window)
	:
	m_Pipeline(graphics),
	m_Window(window),
	m_Model("models/box.obj")
{
	graphics.SetBackgroundColor(200u);

	auto vertices = m_Model.GetVertices();
	auto uvCoordinates = m_Model.GetUvCoordinates();
	auto normals = m_Model.GetNormals();

	for (int i = 0; i < vertices.size(); i++)
	{
		m_TriangleInput.push_back({ vertices[i], normals[i], uvCoordinates[i]});
	}

	m_Pipeline.LoadTexture("models/boxTexture.png");
}

void ModelPreviewScene::Start()
{
	m_Model.SetPosition({ 0.0f, 0.0f, 1.65f });
	m_Model.SetRotation({ 0.95f, 0.0f, 0.0f });
	m_CameraPosition = { 0.0f, 0.0f, 5.0f };
}

void ModelPreviewScene::Update()
{
	if (m_Window.kbd.KeyIsPressed('W')) m_Model.Translate(Vec3(0.0f, 0.0f, -0.05f));
	if (m_Window.kbd.KeyIsPressed('S')) m_Model.Translate(Vec3(0.0f, 0.0f, 0.05f));
	if (m_Window.kbd.KeyIsPressed('A')) m_Model.Translate(Vec3(-0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('D')) m_Model.Translate(Vec3(0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('Q')) m_Model.Translate(Vec3(0.0f, 0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('E')) m_Model.Translate(Vec3(0.0f, -0.05f, 0.0f));

	if (m_Window.kbd.KeyIsPressed(VK_LEFT)) m_Model.Rotate(Vec3(0.0f, -0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_RIGHT)) m_Model.Rotate(Vec3(0.0f, 0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_UP)) m_Model.Rotate(Vec3(-0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_DOWN)) m_Model.Rotate(Vec3(0.05f, 0.0f, 0.0f));

	m_Model.UpdateModelTransform();
}

void ModelPreviewScene::Draw()
{
	m_Pipeline.ClearZBuffer();
	Mat4 model = m_Model.GetModelTransform();
	Mat4 view = Mat4::Translate(-Vec3(0.0f, 0.0f, 5.0f));
	Mat4 projection = Mat4::PerspectiveProjection(0.1f, 100.0f, 90.0f * (static_cast<float>(M_PI) / 180.0f), Graphics::AspectRatio);

	m_Pipeline.BindIndices(m_Model.GetIndices());
	m_Pipeline.BindVertices(m_TriangleInput);

	m_Pipeline.GetVertexShader().SetMVP(projection * view * model);
	m_Pipeline.GetVertexShader().SetMV(view * model);
	m_Pipeline.GetVertexShader().SetP(projection);
	m_Pipeline.Draw();
}
