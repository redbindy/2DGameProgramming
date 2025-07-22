#include "MyGame.h"

MyGame::MyGame(const HWND hWnd)
	: Game(hWnd)
	, mBasicTiles(IDB_BACKGROUND, Game::mpGraphics)
	, mBackground(&mBasicTiles, Game::mpGraphics)
	, mCharacterSheet(IDB_CHARACTER, Game::mpGraphics)
	, mCharacter(&mCharacterSheet, this, 3, 0, 3, 4)
	, mStatic(&mCharacterSheet, this, 0, 5, 3, 4)
	, mDynamic(&mCharacterSheet, this, 6, 0, 3, 4)
{
	mCharacter.SetPosition(mpGraphics->GetWidth() * 0.5f, mpGraphics->GetHeight() * 0.5f);
	mCharacter.SetScale(5);
	// mCharacter.SetMass(300.f);

	mStatic.SetPosition(500, 500);
	// mStatic.SetScale(5);
	// mStatic.SetMass(500.f);

	mDynamic.SetPosition(700, 700);
	// mDynamic.SetScale(5);
}

void MyGame::update()
{
	const float DELTA = mFrameTime * 1000.f;

	float deltaX = 0;
	float deltaY = 0;
	bool bAnimainting = false;

	if (mpInput->IsKeyPressed(VK_RIGHT))
	{
		deltaX += DELTA;
		bAnimainting = true;
	}

	if (mpInput->IsKeyPressed(VK_LEFT))
	{
		deltaX -= DELTA;
		bAnimainting = true;
	}

	if (mpInput->IsKeyPressed(VK_UP))
	{
		deltaY -= DELTA;
		bAnimainting = true;
	}

	if (mpInput->IsKeyPressed(VK_DOWN))
	{
		deltaY += DELTA;
		bAnimainting = true;
	}

	if (mpInput->IsKeyPressed(VK_SPACE))
	{
		mCharacter.OnShield();
	}

	const GameInputGamepadState gamepadState = mpInput->GetGamepadState();

	constexpr float DEAD_ZONE = 0.2f;
	if (gamepadState.leftThumbstickX > DEAD_ZONE)
	{
		deltaX += DELTA;
		bAnimainting = true;
	}
	else if (gamepadState.leftThumbstickX < -DEAD_ZONE)
	{
		deltaX -= DELTA;
		bAnimainting = true;
	}

	if (gamepadState.leftThumbstickY > DEAD_ZONE)
	{
		deltaY -= DELTA;
		bAnimainting = true;
	}
	else if (gamepadState.leftThumbstickY < -DEAD_ZONE)
	{
		deltaY += DELTA;
		bAnimainting = true;
	}

	if (gamepadState.leftTrigger > 0.f || gamepadState.rightTrigger > 0.f)
	{
		mpInput->OnVibratingController(0.5f);
	}

	const float screenWidth = static_cast<float>(mpGraphics->GetWidth());
	const float screenHeight = static_cast<float>(mpGraphics->GetHeight());

	const float nextX = mBackground.GetX() - deltaX;
	if (nextX > screenWidth)
	{
		mBackground.SetX(nextX - screenWidth);
	}
	else if (nextX + screenWidth < 0)
	{
		mBackground.SetX(nextX + screenWidth * 2);
	}
	else
	{
		mBackground.SetX(nextX);
	}

	const float nextY = mBackground.GetY() - deltaY;
	if (nextY > screenHeight)
	{
		mBackground.SetY(nextY - screenHeight);
	}
	else if (nextY + screenHeight < 0)
	{
		mBackground.SetY(nextY + screenHeight * 2);
	}
	else
	{
		mBackground.SetY(nextY);
	}

	/*mCharacter.SetX(mCharacter.GetX() + deltaX);
	mCharacter.SetY(mCharacter.GetY() + deltaY);
	mCharacter.AddDeltaV(deltaX, deltaY);*/
	mCharacter.SetAnimationState(bAnimainting);

	mCharacter.CalculateGravityForce(mStatic, mFrameTime);

	mStatic.Update(mFrameTime);
	mDynamic.Update(mFrameTime);
	mCharacter.Update(mFrameTime);
}

void MyGame::updateAI()
{

}

void MyGame::handleCollision()
{
	XMFLOAT2 collisionVector;

	const bool bCollidingCharacterStatic = mCharacter.TestCollision(mStatic, collisionVector);
	if (bCollidingCharacterStatic)
	{
		mCharacter.Bounce(collisionVector, mStatic);
	}

	const bool bCollidingDynamicStatic = mDynamic.TestCollision(mStatic, collisionVector);
	if (bCollidingDynamicStatic)
	{
		mDynamic.Bounce(collisionVector, mStatic);
	}

	const bool bCollidingCharacterDynamic = mCharacter.TestCollision(mDynamic, collisionVector);
	if (bCollidingCharacterDynamic)
	{
		mCharacter.Bounce(collisionVector, mDynamic);

		const XMFLOAT2 reverseCollisionVector = { -collisionVector.x, -collisionVector.y };

		mDynamic.Bounce(reverseCollisionVector, mCharacter);
	}
}

void MyGame::render()
{
	mBackground.Draw();

	const float screenWidth = static_cast<float>(mpGraphics->GetWidth());
	const float screenHeight = static_cast<float>(mpGraphics->GetHeight());

	const float backgroundX = mBackground.GetX();
	const float backgroundY = mBackground.GetY();

	const float backgroundRight = backgroundX + screenWidth;
	if (backgroundRight < screenWidth)
	{
		mBackground.SetX(backgroundRight);
		mBackground.Draw();
	}
	else
	{
		mBackground.SetX(-(screenWidth - backgroundX));
		mBackground.Draw();
	}

	const float backgroundBottom = backgroundY + screenHeight;

	const bool bUpShift = backgroundBottom < screenHeight;
	if (bUpShift)
	{
		mBackground.SetY(backgroundBottom);
		mBackground.Draw();
	}
	else
	{
		mBackground.SetY(-(screenHeight - backgroundY));
		mBackground.Draw();
	}

	mBackground.SetX(backgroundX);
	if (bUpShift)
	{
		mBackground.SetY(backgroundBottom);
		mBackground.Draw();
	}
	else
	{
		mBackground.SetY(-(screenHeight - backgroundY));
		mBackground.Draw();
	}

	mBackground.SetY(backgroundY);

	mStatic.DrawPatch();
	mDynamic.DrawPatch();
	mCharacter.DrawPatch();
}
