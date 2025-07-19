#include "Image.h"

Image::Image(
	Sprite* const pSprite,
	Graphics* const pGraphics,
	const int frameEntryX,
	const int frameEntryY,
	const int frameCount
)
	: mpSprite(pSprite)
	, mpGraphics(pGraphics)
	, mX(0)
	, mY(0)
	, mRotationAngle(0)
	, mScalingX(1)
	, mScalingY(1)
	, mFrameEntryX(frameEntryX)
	, mFrameEntryY(frameEntryY)
	, mFrameCount(frameCount)
	, mFrameDelay(0.2f)
	, mAnimationTimer(0)
	, mbAnimating(true)
	, mCurrentFrameNumber(frameCount / 2)
{
	std::cout << "CreateImage" << std::endl;
}

Image::Image(Sprite* const pSprite, Graphics* const pGraphics)
	: Image(pSprite, pGraphics, -1, -1, -1)
{

}

void Image::Draw() const
{
	RECT sourceRect;
	sourceRect.left = (mFrameEntryX + mCurrentFrameNumber) * PATCH_SIZE;
	sourceRect.top = mFrameEntryY * PATCH_SIZE;
	sourceRect.right = sourceRect.left + PATCH_SIZE;
	sourceRect.bottom = sourceRect.top + PATCH_SIZE;

	constexpr XMFLOAT2 origin(PATCH_SIZE * 0.5f, PATCH_SIZE * 0.5f);

	XMFLOAT2 screenPosition(mX, mY);
	XMFLOAT2 scaling(mScalingX, mScalingY);

	ID3D11ShaderResourceView* const pSpriteGPU = mpSprite->GetSpriteGPU();

	mpGraphics->DrawSprite(
		mpSprite->GetSpriteGPU(),
		screenPosition,
		&sourceRect,
		Colors::White,
		mRotationAngle,
		origin,
		scaling,
		SpriteEffects_None,
		0.f
	);
}

void Image::Update(const float frameTime)
{
	assert(frameTime >= 0.f);

	if (mFrameCount == -1 || !mbAnimating)
	{
		return;
	}

	mAnimationTimer += frameTime;
	if (mAnimationTimer > mFrameDelay)
	{
		mAnimationTimer -= mFrameDelay;
		mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mFrameCount;
	}
}
