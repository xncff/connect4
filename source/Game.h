#pragma once
#include "raylib.h"
#include <string>
#include <array>
#include <vector>
#include <limits>
#include <iostream>

class Game
{
	struct Move
	{
		int _i;
		int _j;
		int _value;
	};

	static const int _ROWS = 6;
	static const int _COLS = 7;
	using Grid = std::array<std::array<int, _COLS>, _ROWS>;
	
	enum GameValues { PLAYER1 = 1, PLAYER2 = 2, DRAW = 3 };

	Grid _grid;

	const int _depth;

	// Game Flow 
	bool _firstPlayerTurn;
	bool _startingMenu;
	bool _vsComputer;
	int _completed;
	bool _running;

	// Interface objects' sizes 
	int _windowX;
	int _windowY;
	float _sideSpace;

	float _cellSize;
	float _stoneSize;

	Vector2 _gridSize;
	Vector2 _boardSize;

	float _restartSize;

	// Interface objects' coordinates 
	Vector2 _boardPos;
	Vector2 _gridPos;
	Vector2 _restartPos;

	// Colors
	Color _backColor;
	Color _mainColor;
	Color _auxColor;

	Color _player1Color;
	Color _player2Color;

	// Textures
	Texture2D _restart;

	// Functions 
	void drawStartingMenu();
	void loadTextures();
	void showResult();
	void DrawBoard();
	void reset();

	int complete4(const Grid& state);
	int canPlay(const Grid& state, int col);

	void humanPlays(const Vector2& mousePos);
	void computerPlays();

	int minimax(/*const Grid& state, */int depth, int alpha, int beta, bool maximizingPlayer);
	int tEvaluate(const Grid& state);
	int sEvaluate(const Grid& state);
	int evaluateWindow(const std::array<int, 4> window);
	std::vector<Move> getAllMoves(const Grid& state, bool maximizingPlayer);

	const int _evalTable[_ROWS][_COLS] = { {3, 4, 5, 7, 5, 4, 3},
										   {4, 6, 8, 10, 8, 6, 4},
										   {5, 8, 11, 13, 11, 8, 5},
										   {5, 8, 11, 13, 11, 8, 5},
										   {4, 6, 8, 10, 8, 6, 4},
										   {3, 4, 5, 7, 5, 4, 3} };
public:
	Game(float windowX, float windowY, int depth);
	~Game();

	void Update();
	void Draw();
};

