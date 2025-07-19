#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Game.h"

class MyGame : public Game
{
public:
	MyGame(const HWND hWnd);
	virtual ~MyGame() = default;

private:
	virtual void update();
	virtual void updateAI();
	virtual void HandleCollision();
	virtual void render();
};