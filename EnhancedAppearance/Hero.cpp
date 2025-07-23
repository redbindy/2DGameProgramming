#include "Hero.h"

Hero::Hero(
	Sprite* const pSprite,
	Game* const pGame,
	const int sourceFrameX,
	const int sourceFrameY,
	const int animationFrameCount,
	const int directionalFrameCount
)
	: Entity(pSprite, pGame, sourceFrameX, sourceFrameY, animationFrameCount, directionalFrameCount)
	, mShieldImage(pSprite, pGame->GetGraphics(), 9, 0, 3, 3)
	, mbShield(false)
	, mShieldTimer(0.f)
{

}

void Hero::Update(const float frameTime)
{
	Entity::Update(frameTime);

	const float rotationAngle = Image::GetRotationAngle();
	SetRotationAngle(frameTime * rotationAngle);

	const XMFLOAT2 position = GetPosition();
	const XMVECTOR positionVector = XMLoadFloat2(&position);

	const XMFLOAT2 velocity = GetVelocity();
	const XMVECTOR velocityVector = XMLoadFloat2(&velocity);

	const XMVECTOR nextPositionVector = positionVector + frameTime * velocityVector;

	XMFLOAT2 nextPosition;
	XMStoreFloat2(&nextPosition, nextPositionVector);

	const float entityWidth = PATCH_SIZE * GetScaleX();

	const float screenWidth = static_cast<float>(Image::mpGraphics->GetWidth());
	if (nextPosition.x > screenWidth - entityWidth && velocity.x > 0
		|| nextPosition.x < 0 && velocity.x < 0)
	{
		SetVelocity(-velocity.x, velocity.y);
	}
	else
	{
		SetX(nextPosition.x);
	}

	const float entityHeight = PATCH_SIZE * GetScaleY();

	const float screenHeight = static_cast<float>(Image::mpGraphics->GetHeight());
	if (nextPosition.y > screenHeight - entityHeight && velocity.y > 0
		|| nextPosition.y < 0 && velocity.y < 0)
	{
		SetVelocity(velocity.x, -velocity.y);
	}
	else
	{
		SetY(nextPosition.y);
	}

	constexpr float SHIELD_DURATION = 1.f;

	if (mbShield)
	{
		mShieldImage.SetScale(GetScaleX());
		mShieldImage.SetPosition(GetPosition());

		mShieldTimer += frameTime;
		if (mShieldTimer > SHIELD_DURATION)
		{
			mShieldTimer = 0.f;
			mbShield = false;
		}
	}
}

void Hero::UpdateAI(const float frameTime, const Entity& other) const
{

}

void Hero::OnAttacked(const int damage)
{

}

void Hero::DrawPatch() const
{
	// reflection
	Image::DrawPatch(GetX(), GetY() + PATCH_SIZE * GetScaleY(), 0.5f, SpriteEffects_FlipVertically);
	Image::DrawPatch();

	if (mbShield)
	{
		mShieldImage.DrawPatch();
	}
}
