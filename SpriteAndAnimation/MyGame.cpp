#include "MyGame.h"

MyGame::MyGame(const HWND hWnd)
	: Game(hWnd)
	, mCharacterSheet(IDB_CHARACTER, Game::mpGraphics)
	, mCharacter(&mCharacterSheet, Game::mpGraphics, 3, 0, 3)
{
	mCharacter.SetPosition(300, 300);
	mCharacter.SetScale(5);
}

void MyGame::update()
{
	const float DELTA = mFrameTime * 100.f;

	float deltaX = 0;
	float deltaY = 0;
	bool bAnimaiting = false;
	if (mpInput->IsKeyPressed(VK_RIGHT))
	{
		deltaX += DELTA;
		bAnimaiting = true;
	}

	if (mpInput->IsKeyPressed(VK_LEFT))
	{
		deltaX -= DELTA;
		bAnimaiting = true;
	}

	if (mpInput->IsKeyPressed(VK_UP))
	{
		deltaY -= DELTA;
		bAnimaiting = true;
	}

	if (mpInput->IsKeyPressed(VK_DOWN))
	{
		deltaY += DELTA;
		bAnimaiting = true;
	}

	const GameInputGamepadState gamepadState = mpInput->GetGamepadState();
	if (gamepadState.leftThumbstickX > 0.2f)
	{
		deltaX += DELTA;
	}
	else if (gamepadState.leftThumbstickX < -0.2f)
	{
		deltaX -= DELTA;
	}

	if (gamepadState.leftThumbstickY > 0.2f)
	{
		deltaY -= DELTA;
	}
	else if (gamepadState.leftThumbstickY < -0.2f)
	{
		deltaY += DELTA;
	}

	if (gamepadState.leftTrigger > 0.f || gamepadState.rightTrigger > 0.f)
	{
		mpInput->OnVibratingController(0.5f);
	}

	const float characterX = mCharacter.GetX();
	const float characterY = mCharacter.GetY();

	mCharacter.SetPosition(characterX + deltaX, characterY + deltaY);
	mCharacter.SetAnimationState(bAnimaiting);

	mCharacter.Update(mFrameTime);
}

void MyGame::updateAI()
{

}

void MyGame::handleCollision()
{

}

void MyGame::render()
{
	mCharacter.Draw();
}
