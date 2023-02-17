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
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	pipeline( gfx )
{
	pipeline.BindIndices({ 0, 1, 2 });
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
}

void Game::ComposeFrame()
{
	if (wnd.kbd.KeyIsPressed('C'))
	{
		pipeline.BindVertices
		(
			{
				{ Vec3{ 0.2f, -0.2f, -2.0f }, Vec3{ 0.0f, 1.0f, 0.0f } },
				{ Vec3{ 0.0f, 0.2f, 0.0f }, Vec3{ 0.0f, 0.0f, 1.0f } },
				{ Vec3{ -0.2f, -0.2f, 0.0f }, Vec3{ 1.0f, 0.0f, 0.0f } }
			}
		);
		pipeline.Draw();
	}
	else
	{
		pipeline.BindVertices
		(
			{
				{ Vec3{ 0.2f, -0.2f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f } },
				{ Vec3{ 0.0f, 0.2f, 0.0f }, Vec3{ 0.0f, 0.0f, 1.0f } },
				{ Vec3{ -0.2f, -0.2f, 0.0f }, Vec3{ 1.0f, 0.0f, 0.0f } }
			}
		);
		pipeline.Draw();
	}
}
