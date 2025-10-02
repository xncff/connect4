#include <iostream>

#include "Game.h"

int main()
{
	// Window size
	const float windowX = 800;
	const float  windowY = 800;

	// Window
	InitWindow(windowX, windowY, "Connect 4");

	// FPS
	SetTargetFPS(144);

	// Game Object  
	Game game(windowX, windowY, 5);

	// Game Loop
	while (WindowShouldClose() == false)
	{
		// 1. Input 

		// 2. Positions
		game.Update();

		// 3. Drawing 
		BeginDrawing();

		game.Draw();

		EndDrawing();
	}
	CloseWindow();
}
