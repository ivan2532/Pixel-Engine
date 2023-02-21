#include "DirectionalLightningTriangleScene.h"

#define _USE_MATH_DEFINES
#include <math.h>

DirectionalLightningTriangleScene::DirectionalLightningTriangleScene(Graphics& graphics, MainWindow& window)
	:
	m_Pipeline(graphics),
	m_Window(window),
	m_Suzanne("models/suzanne.obj")
{
	auto vertices = m_Suzanne.GetVertices();
	auto normals = m_Suzanne.GetNormals();

	for (int i = 0; i < vertices.size(); i++)
	{
		m_TriangleInput.push_back({ vertices[i], normals[i] });
	}
}

void DirectionalLightningTriangleScene::Start()
{
	m_CameraPosition = { 0.0f, 0.0f, 5.0f };
}

void DirectionalLightningTriangleScene::Update()
{
	if (m_Window.kbd.KeyIsPressed('W')) m_Suzanne.Translate(Vec3(0.0f, 0.0f, -0.05f));
	if (m_Window.kbd.KeyIsPressed('S')) m_Suzanne.Translate(Vec3(0.0f, 0.0f, 0.05f));
	if (m_Window.kbd.KeyIsPressed('A')) m_Suzanne.Translate(Vec3(-0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('D')) m_Suzanne.Translate(Vec3(0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('Q')) m_Suzanne.Translate(Vec3(0.0f, 0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed('E')) m_Suzanne.Translate(Vec3(0.0f, -0.05f, 0.0f));

	if (m_Window.kbd.KeyIsPressed(VK_LEFT)) m_Suzanne.Rotate(Vec3(0.0f, 0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_RIGHT)) m_Suzanne.Rotate(Vec3(0.0f, -0.05f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_UP)) m_Suzanne.Rotate(Vec3(0.05f, 0.0f, 0.0f));
	if (m_Window.kbd.KeyIsPressed(VK_DOWN)) m_Suzanne.Rotate(Vec3(-0.05f, 0.0f, 0.0f));

	m_Suzanne.UpdateModelTransform();
}

void DirectionalLightningTriangleScene::Draw()
{
	m_Pipeline.ClearZBuffer();
	Mat4 model = m_Suzanne.GetModelTransform();
	Mat4 view = Mat4::Translate(-Vec3(0.0f, 0.0f, 5.0f));
	Mat4 projection = Mat4::PerspectiveProjection(0.1f, 100.0f, 90.0f * (static_cast<float>(M_PI) / 180.0f), Graphics::AspectRatio);

	m_Pipeline.BindIndices(m_Suzanne.GetIndices());
	m_Pipeline.BindVertices(m_TriangleInput);

	m_Pipeline.GetVertexShader().SetMVP(projection * view * model);
	m_Pipeline.GetVertexShader().SetMV(view * model);
	m_Pipeline.Draw();
}
