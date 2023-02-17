/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#define _USE_MATH_DEFINES
#include <cmath>

#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	pipeline( gfx ),
	camPosition(0.0f, 0.0f, 5.0f)
{
	pipeline.BindIndices({ 0, 1, 2 });
	pipeline.BindVertices
	(
		{
			{ Vec3{ 0.5f, -0.5f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f } },
			{ Vec3{ 0.0f, 0.5f, 0.0f }, Vec3{ 0.0f, 0.0f, 1.0f } },
			{ Vec3{ -0.5f, -0.5f, 0.0f }, Vec3{ 1.0f, 0.0f, 0.0f } }
		}
	);
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (wnd.kbd.KeyIsPressed('W')) trianglePosition.z -= 0.05f;
	if (wnd.kbd.KeyIsPressed('S')) trianglePosition.z += 0.05f;
	if (wnd.kbd.KeyIsPressed('A')) trianglePosition.x -= 0.05f;
	if (wnd.kbd.KeyIsPressed('D')) trianglePosition.x += 0.05f;
	if (wnd.kbd.KeyIsPressed('Q')) trianglePosition.y += 0.05f;
	if (wnd.kbd.KeyIsPressed('E')) trianglePosition.y -= 0.05f;

	if (wnd.kbd.KeyIsPressed(VK_LEFT)) triangleEulerAngles.y += 0.05f;
	if (wnd.kbd.KeyIsPressed(VK_RIGHT)) triangleEulerAngles.y -= 0.05f;
	if (wnd.kbd.KeyIsPressed(VK_UP)) triangleEulerAngles.x += 0.05f;
	if (wnd.kbd.KeyIsPressed(VK_DOWN)) triangleEulerAngles.x -= 0.05f;
}

void Game::ComposeFrame()
{
	Mat4 model = Mat4::Translate(trianglePosition) *Mat4::RotateX(triangleEulerAngles.x)* Mat4::RotateY(triangleEulerAngles.y)* Mat4::RotateZ(triangleEulerAngles.z);
	Mat4 view = Mat4::Translate(-camPosition);
	Mat4 projection = Mat4::PerspectiveProjection(0.1f, 100.0f, 90.0f * (static_cast<float>(M_PI) / 180.0f), Graphics::AspectRatio);

	pipeline.GetVertexShader().SetMVP(projection * view * model);
	pipeline.Draw();
}
