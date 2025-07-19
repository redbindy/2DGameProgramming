#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include "Sprite.h"
#include "Image.h"

class MyGame : public Game
{
public:
	MyGame(const HWND hWnd);
	virtual ~MyGame() = default;

private:
	Sprite mCharacterSheet;
	Image mCharacter;

private:
	virtual void update();
	virtual void updateAI();
	virtual void handleCollision();
	virtual void render();
};