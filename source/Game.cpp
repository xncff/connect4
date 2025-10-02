#include "Game.h"

Game::Game(float windowX, float windowY, int depth) : 
	_windowX(windowX), _windowY(windowY), _depth(depth)
{
	_sideSpace = 0.1f * _windowX;
	float boardSizeX = _windowX - 2 * _sideSpace;
	_boardSize = { boardSizeX, 0.92f * boardSizeX };

	float _innerSpace = 0.02f * _boardSize.x;
	float gridSizeX = _boardSize.x - 2 * _innerSpace;
	_gridSize = { gridSizeX, (1 - 1/_COLS) * gridSizeX };

	_cellSize = _gridSize.x / _COLS;
	_stoneSize = _cellSize * 0.9f;

	_restartSize = _cellSize;

	_boardPos = { _sideSpace, _windowY - _boardSize.y };
	_gridPos = { _boardPos.x + _innerSpace, _boardPos.y + _innerSpace };
	_restartPos = { _windowX - _sideSpace - _restartSize, _sideSpace / 2 };

	_backColor = { 180, 180, 180, 255 };
	_auxColor = WHITE;
	_mainColor = { 28, 98, 241, 255 };

	_player1Color = { 255, 0, 0, 200 };
	_player2Color = { 255, 255, 0, 200 };
	
	loadTextures();

	_running = false;
	_startingMenu = true;
}

Game::~Game()
{
	UnloadTexture(_restart);
}

void Game::Update()
{
	const bool spaceDown = IsKeyDown(KEY_SPACE);
	const bool leftClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	const Vector2 mousePos = leftClicked ? GetMousePosition() : Vector2{ 0,0 };

	if (_startingMenu)
	{
		if (leftClicked)
		{
			const float half = static_cast<float>(_windowX * 0.5f);
			_vsComputer = mousePos.x < half ? false : true;
			_startingMenu = false;
			reset();
		}
		return;
	}

	if (!_running)
	{
		if (leftClicked || spaceDown)
		{
			reset();
		}
		return;
	}

	_completed = complete4(_grid);
	if (_completed != 0)
	{
		_running = false;
		return;
	}

	if (_vsComputer && !_firstPlayerTurn) 
	{
		computerPlays();
		return;
	}

	if (!leftClicked) return;
	if (CheckCollisionPointRec(mousePos, { _gridPos.x, _gridPos.y,
											_gridSize.x, _gridSize.y }))
	{
		humanPlays(mousePos);
	}
	else if (CheckCollisionPointRec(mousePos, { _restartPos.x, _restartPos.y,
												_restartSize, _restartSize }))
	{
		reset();
	}
}

void Game::Draw()
{
	ClearBackground(_backColor);

	if (_startingMenu)
	{
		drawStartingMenu();
		return;
	}

	if (_completed != 0)
	{
		showResult();
	}
	else
	{
		DrawTextureV(_restart, _restartPos, WHITE);

		Color nextPlayer = _firstPlayerTurn ? _player1Color : _player2Color;
		DrawCircle(0, 0, 2 * _stoneSize, nextPlayer);
	}

	DrawBoard();
}

int Game::complete4(const Grid& state)
{
	bool full = true;
	for (int r = 0; r < _ROWS; r++) 
	{
		for (int c = 0; c < _COLS; c++) 
		{
			int player = state[r][c];
			if (player == 0) 
			{
				full = false;
				continue;
			}

			// Check right (horizontal)
			if (c + 3 < _COLS &&
				player == state[r][c + 1] &&
				player == state[r][c + 2] &&
				player == state[r][c + 3])
			{
				return player;
			}

			// Check down (vertical)
			if (r + 3 < _ROWS &&
				player == state[r + 1][c] &&
				player == state[r + 2][c] &&
				player == state[r + 3][c])
			{
				return player;
			}

			// Check diagonal down-right
			if (r + 3 < _ROWS && c + 3 < _COLS &&
				player == state[r + 1][c + 1] &&
				player == state[r + 2][c + 2] &&
				player == state[r + 3][c + 3])
			{
				return player;
			}

			// Check diagonal down-left
			if (r + 3 < _ROWS && c - 3 >= 0 &&
				player == state[r + 1][c - 1] &&
				player == state[r + 2][c - 2] &&
				player == state[r + 3][c - 3])
			{
				return player;
			}
		}
	}
	return full ? DRAW : 0;
}

void Game::DrawBoard()
{
	DrawRectangle(_boardPos.x, _boardPos.y, _boardSize.x, _boardSize.y, _mainColor);
	for (int i = 0; i < _ROWS; ++i)
	{
		for (int j = 0; j < _COLS; ++j)
		{
			int current = _grid[i][j];
			float currX = _gridPos.x + j * _cellSize + _cellSize / 2;
			float currY = _gridPos.y + i * _cellSize + _cellSize / 2;

			Color checkerColor = _backColor;
			if (current == PLAYER1)
			{
				checkerColor = _player1Color;
			}
			else if (current == PLAYER2)
			{
				checkerColor = _player2Color;
			}
			else
			{
				if (_running && 
					((_vsComputer && _firstPlayerTurn) || !_vsComputer))
				{
					Vector2 mousePos = GetMousePosition();
					if (CheckCollisionPointCircle(mousePos, { currX, currY },
												  _stoneSize / 2))
					{
						checkerColor = GRAY;
					}
					else
					{
						checkerColor = _backColor;
					}
				}
			}
			DrawCircle(currX, currY, _stoneSize / 2, checkerColor);
		}
	}
}

void Game::showResult()
{
	Color overlayColor;
	std::string message;

	if (_completed == PLAYER1)
	{
		overlayColor = _player1Color;
		message = "Player 1 won!";
	}
	else if (_completed == PLAYER2)
	{
		overlayColor = _player2Color;
		message = "Player 2 won!";
	}
	else // _completed == DRAW
	{
		overlayColor = Color{ 0, 0, 0, 150 };
		message = "Draw!";
	}

	DrawRectangle(0, 0, _windowX, _windowY, overlayColor);

	int fontSize = static_cast<int>(_windowY * 0.1f);
	DrawText(message.c_str(),
		(_windowX - MeasureText(message.c_str(), fontSize)) / 2,
		(_windowY - _boardSize.y - fontSize) / 2,
		fontSize,
		WHITE
	);
}

void Game::drawStartingMenu()
{
	DrawRectangle(0, 0, _windowX / 2, _windowY, RED);
	DrawRectangle(_windowX / 2, 0, _windowX / 2, _windowY, BLUE);

	int fontSize = 50;
	int spacingY = fontSize + 10; // vertical gap between lines

	DrawText("VS",
		_windowX / 4 - MeasureText("VS", fontSize) / 2,
		_windowY / 2 - spacingY,
		fontSize, WHITE);

	DrawText("HUMAN",
		_windowX / 4 - MeasureText("HUMAN", fontSize) / 2,
		_windowY / 2,
		fontSize, WHITE);

	DrawText("VS",
		3 * _windowX / 4 - MeasureText("VS", fontSize) / 2,
		_windowY / 2 - spacingY,
		fontSize, WHITE);

	DrawText("COMPUTER",
		3 * _windowX / 4 - MeasureText("COMPUTER", fontSize) / 2,
		_windowY / 2,
		fontSize, WHITE);
}

int Game::canPlay(const Grid& state, int col)
{
	for (int i = 5; i >= 0; --i)
	{
		if (state[i][col] == 0)
		{
			return i;
		}
	}
	return -1;
}

void Game::humanPlays(const Vector2& mousePos)
{
	int col = static_cast<int>((mousePos.x - _gridPos.x) / _cellSize);

	if (col < 0 || col >= _COLS)
	{
		return;
	}

	int row = canPlay(_grid, col);
	if (row == -1)
	{
		return;
	}

	_grid[row][col] = _firstPlayerTurn ? PLAYER1 : PLAYER2;

	_firstPlayerTurn = !_firstPlayerTurn;
}

void Game::computerPlays()
{
	int bestScore = std::numeric_limits<int>::min();
	Move bestMove = { -1, -1, 0 };

	for (const Move& move : getAllMoves(_grid, true))
	{
		_grid[move._i][move._j] = move._value;
		int score = minimax(_depth - 1, std::numeric_limits<int>::min(),													  std::numeric_limits<int>::max(), false);
		_grid[move._i][move._j] = 0;

		if (score > bestScore)
		{
			bestScore = score;
			bestMove = move;
		}
	}

	if (bestMove._i != -1)
	{
		_grid[bestMove._i][bestMove._j] = bestMove._value;
	}
	_firstPlayerTurn = !_firstPlayerTurn;
}

int Game::minimax(int depth, int alpha, int beta, bool maximizingPlayer)
{
	int completed = complete4(_grid);
	if (completed != 0)
	{
		if (completed == PLAYER1) return -1000 - depth;
		else if (completed == PLAYER2) return 1000 + depth;
		else return 0;
	}
	if (depth == 0) return sEvaluate(_grid);

	int bestValue = 0;
	if (maximizingPlayer)
	{
		bestValue = std::numeric_limits<int>::min();
		for (const Move& move : getAllMoves(_grid, true))
		{
			_grid[move._i][move._j] = move._value;
			int value = minimax(depth - 1, alpha, beta, false);
			_grid[move._i][move._j] = 0;

			bestValue = std::max(bestValue, value);
			alpha = std::max(alpha, value);
			if (beta <= alpha)
			{
				break;
			}
		}
	}
	else // minimizing player
	{
		bestValue = std::numeric_limits<int>::max();
		for (const Move& move : getAllMoves(_grid, false))
		{
			_grid[move._i][move._j] = move._value;
			int value = minimax(depth - 1, alpha, beta, true);
			_grid[move._i][move._j] = 0;

			bestValue = std::min(bestValue, value);
			beta = std::min(beta, value);
			if (beta <= alpha)
			{
				break;
			}
		}
	}
	return bestValue;
}

std::vector<Game::Move> Game::getAllMoves(const Grid& state, bool maximizingPlayer)
{
	std::vector<Move> res;
	for (int col = 0; col < _COLS; ++col)
	{
		int row = canPlay(state, col);
		if (row != -1)
		{
			//Grid move = state;
			//move[row][col] = maximizingPlayer ? PLAYER2 : PLAYER1;
			//res.push_back(move);
			res.push_back({ row, col, maximizingPlayer ? PLAYER2 : PLAYER1 });
		}
	}
	return res;
}

int Game::tEvaluate(const Grid& state)
{
	int compScore = 0;
	int humanScore = 0;
	for (int i = 0; i < _ROWS; ++i)
	{
		for (int j = 0; j < _COLS; ++j)
		{
			if (state[i][j] == PLAYER1)
			{
				humanScore += _evalTable[i][j];
			}
			else if (state[i][j] == PLAYER2)
			{
				compScore += _evalTable[i][j];
			}
		}
	}
	return compScore - humanScore;
}

int Game::sEvaluate(const Grid& state) 
{
	int score = 0;

	for (int r = 0; r < _ROWS; ++r) 
	{
		for (int c = 0; c < _COLS - 3; ++c) 
		{
			std::array<int, 4> window;
			for (int i = 0; i < window.size(); ++i)
			{
				window[i] = state[r][c + i];
			}
			score += evaluateWindow(window);
		}
	}

	for (int c = 0; c < _COLS; ++c)
	{
		for (int r = 0; r < _ROWS - 3; ++r)
		{
			std::array<int, 4> window;
			for (int i = 0; i < window.size(); ++i)
			{
				window[i] = state[r + i][c];
			}
			score += evaluateWindow(window);
		}
	}

	for (int r = 0; r < _ROWS - 3; ++r)
	{
		for (int c = 0; c < _COLS - 3; ++c)
		{
			std::array<int, 4> window;
			for (int i = 0; i < window.size(); ++i)
			{
				window[i] = state[r + i][c + i];
			}
			score += evaluateWindow(window);
		}
	}

	for (int r = _ROWS - 1; r > 2; --r)
	{
		for (int c = 0; c < _COLS - 3; ++c)
		{
			std::array<int, 4> window;
			for (int i = 0; i < window.size(); ++i)
			{
				window[i] = state[r - i][c + i];
			}
			score += evaluateWindow(window);
		}
	}

	return score;
}

int Game::evaluateWindow(const std::array<int, 4> window)
{
	int score = 0;
	int maxPlayer = PLAYER2;
	int minPlayer = PLAYER1;

	int maxPlayerCount = std::count(window.begin(), window.end(), maxPlayer);
	int minPlayerCount = std::count(window.begin(), window.end(), minPlayer);
	int empty = std::count(window.begin(), window.end(), 0);

	//if (maxPlayerCount == 4) score += 100;
	/*else */if (maxPlayerCount == 3 && empty == 1) score += 5;
	else if (maxPlayerCount == 2 && empty == 2) score += 2;

	//if (minPlayerCount == 4) score -= 100;
	/*else */if (minPlayerCount == 3 && empty == 1) score -= 5;
	else if (minPlayerCount == 2 && empty == 2) score -= 2;

	return score;
}

void Game::reset()
{
	_running = true;
	_firstPlayerTurn = true;
	_completed = 0;
	for (int i = 0; i < _ROWS; ++i)
	{
		for (int j = 0; j < _COLS; ++j)
		{
			_grid[i][j] = 0;
		}
	}
}

void Game::loadTextures()
{
	Image image = LoadImage("graphics/restart.png");
	ImageResize(&image, _restartSize, _restartSize);
	_restart = LoadTextureFromImage(image);
	UnloadImage(image);
}

