#include "Image.h"

Image::Image(
	Sprite* const pSprite,
	Graphics* const pGraphics,
	const int sourceFrameX,
	const int sourceFrameY,
	const int animationFrameCount,
	const int directionalFrameCount
)
	: mpSprite(pSprite)
	, mpGraphics(pGraphics)
	, mPosition(0.f, 0.f)
	, mRotationAngle(0)
	, mScale(1.f, 1.f)
	, mSourceFrameX(sourceFrameX)
	, mSourceFrameY(sourceFrameY)
	, mAnimationFrameCount(animationFrameCount)
	, mDirectionalFrameCount(directionalFrameCount)
	, mCurrentAnimationFrameIndex(0)
	, mCurrentDirectionFrameIndex(0)
	, mFrameDelay(0.2f)
	, mAnimationTimer(0)
	, mbAnimating(false)
{
	std::cout << "CreateImage" << std::endl;
}

Image::Image(Sprite* const pSprite, Graphics* const pGraphics)
	: Image(pSprite, pGraphics, 0, 0, 0, 0)
{

}

void Image::Draw() const
{
	RECT sourceRect;
	sourceRect.left = (mSourceFrameX + mCurrentAnimationFrameIndex) * PATCH_SIZE;
	sourceRect.top = (mSourceFrameY + mCurrentDirectionFrameIndex) * PATCH_SIZE;
	sourceRect.right = sourceRect.left + PATCH_SIZE;
	sourceRect.bottom = sourceRect.top + PATCH_SIZE;

	constexpr XMFLOAT2 origin(PATCH_SIZE_HALF, PATCH_SIZE_HALF);

	ID3D11ShaderResourceView* const pSpriteGPU = mpSprite->GetSpriteGPU();

	mpGraphics->DrawSprite(
		pSpriteGPU,
		mPosition,
		&sourceRect,
		Colors::White,
		mRotationAngle,
		origin,
		mScale,
		SpriteEffects_None,
		0.f
	);
}

void Image::Update(const float frameTime)
{
	assert(frameTime >= 0.f);

	if (mAnimationFrameCount == 0 || !mbAnimating)
	{
		return;
	}

	mAnimationTimer += frameTime;
	if (mAnimationTimer > mFrameDelay)
	{
		mAnimationTimer -= mFrameDelay;
		mCurrentAnimationFrameIndex = (mCurrentAnimationFrameIndex + 1) % mAnimationFrameCount;
	}
}
