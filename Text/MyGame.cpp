#include "MyGame.h"

MyGame::MyGame(const HWND hWnd)
	: Game(hWnd)
	, mCharacterSheet(IDB_CHARACTER, mpGraphics)
	, mCharacter(&mCharacterSheet, this, 3, 0, 3, 4)
	, mStatic(&mCharacterSheet, this, 0, 5, 3, 4)
	, mDynamic(&mCharacterSheet, this, 6, 0, 3, 4)
{
	mCharacter.SetPosition(300, 300);
	// mCharacter.SetScale(5);
	// mCharacter.SetMass(300.f);

	mStatic.SetPosition(500, 500);
	// mStatic.SetScale(5);
	// mStatic.SetMass(500.f);

	mDynamic.SetPosition(700, 700);
	// mDynamic.SetScale(5);
}

void MyGame::update()
{
	const float DELTA = mFrameTime * 100.f;

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

	mCharacter.AddDeltaV(deltaX, deltaY);
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
	mStatic.Draw();
	mDynamic.Draw();
	mCharacter.Draw();
}
