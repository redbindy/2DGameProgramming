#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include "Sprite.h"
#include "Image.h"
#include "Hero.h"

class MyGame final : public Game
{
public:
	MyGame(const HWND hWnd);
	virtual ~MyGame() = default;

private:
	Sprite mBasicTiles;
	Image mBackground;

	Sprite mCharacterSheet;
	Hero mCharacter;
	Hero mStatic;
	Hero mDynamic;

private:
	virtual void update() override;
	virtual void updateAI() override;
	virtual void handleCollision() override;
	virtual void render() override;

	void DrawBackgroundRecursive(const float x, const float y);
};