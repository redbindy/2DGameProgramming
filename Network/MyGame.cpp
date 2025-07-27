#include "MyGame.h"

MyGame::MyGame(const HWND hWnd)
	: Game(hWnd)
	, mBasicTiles(IDB_BACKGROUND, Game::mpGraphics)
	, mBackground(&mBasicTiles, Game::mpGraphics)
	, mCharacterSheet(IDB_CHARACTER, Game::mpGraphics)
	, mStatic(&mCharacterSheet, this, 0, 5, 3, 4)
	, mServerHero(&mCharacterSheet, this, 3, 0, 3, 4)
	, mClientHero(&mCharacterSheet, this, 6, 0, 3, 4)
{
	const float centerX = mpGraphics->GetWidth() * 0.5f;
	const float centerY = mpGraphics->GetHeight() * 0.5f;

	mStatic.SetPosition(centerX, centerY);
	mStatic.SetScale(3);

#if SERVER
	mNetwork.CreateServer();
#elif CLIENT
	mNetwork.CreateClient();
#endif

	mServerHero.SetScale(5);
	mClientHero.SetScale(5);
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

	HeroDTO dto;
	ZeroMemory(&dto, sizeof(dto));
#if SERVER
	mServerHero.AddDeltaV(deltaX, deltaY);

	dto = mServerHero.GetDTO();

	mNetwork.TrySend(reinterpret_cast<const char*>(&dto), sizeof(dto));

	HeroDTO clientDTO;
	int resultBytes;
	if (mNetwork.TryReceive(reinterpret_cast<char*>(&clientDTO), sizeof(clientDTO), resultBytes))
	{
		mClientHero.SetDTO(clientDTO);
	}

#elif CLIENT
	mClientHero.AddDeltaV(deltaX, deltaY);

	dto = mClientHero.GetDTO();

	mNetwork.TrySend(reinterpret_cast<const char*>(&dto), sizeof(dto));

	HeroDTO serverDTO;
	int resultBytes;
	if (mNetwork.TryReceive(reinterpret_cast<char*>(&serverDTO), sizeof(serverDTO), resultBytes))
	{
		mServerHero.SetDTO(serverDTO);
	}
#endif

	mServerHero.Update(mFrameTime);
	mClientHero.Update(mFrameTime);
}

void MyGame::updateAI()
{

}

void MyGame::handleCollision()
{
	XMFLOAT2 collisionVector;

	const bool bCollidingServerStatic = mServerHero.TestCollision(mStatic, collisionVector);
	if (bCollidingServerStatic)
	{
		mServerHero.Bounce(collisionVector, mStatic);
	}

	const bool bCollidingDynamicStatic = mClientHero.TestCollision(mStatic, collisionVector);
	if (bCollidingDynamicStatic)
	{
		mClientHero.Bounce(collisionVector, mStatic);
	}

	const bool bCollidingCharacterDynamic = mServerHero.TestCollision(mClientHero, collisionVector);
	if (bCollidingCharacterDynamic)
	{
		mServerHero.Bounce(collisionVector, mClientHero);

		const XMFLOAT2 reverseCollisionVector = { -collisionVector.x, -collisionVector.y };

		mClientHero.Bounce(reverseCollisionVector, mServerHero);
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
	mServerHero.DrawPatch();
	mClientHero.DrawPatch();
}
