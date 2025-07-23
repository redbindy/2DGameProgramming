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
	ID3D11ShaderResourceView* const pSpriteGPU = mpSprite->GetSpriteGPU();

	const LONG spriteWidth = static_cast<LONG>(mpSprite->GetWidth());
	const LONG spriteHeight = static_cast<LONG>(mpSprite->GetHeight());

	RECT sourceRect;
	sourceRect.left = 0;
	sourceRect.top = 0;
	sourceRect.right = sourceRect.left + spriteWidth;
	sourceRect.bottom = sourceRect.top + spriteHeight;

	RECT destinationRect;
	destinationRect.left = static_cast<LONG>(mPosition.x);
	destinationRect.top = static_cast<LONG>(mPosition.y);

	const int screenWidth = mpGraphics->GetWidth();
	const int screenHeight = mpGraphics->GetHeight();

	destinationRect.right = destinationRect.left + screenWidth;
	destinationRect.bottom = destinationRect.top + screenHeight;

	SpriteBatch* const pSpriteBatch = mpGraphics->GetSpriteBatch();

	pSpriteBatch->Draw(pSpriteGPU, destinationRect, &sourceRect);
}

void Image::DrawPatch(const float x, const float y, const float alpha, const SpriteEffects effect) const
{
	RECT sourceRect;
	sourceRect.left = (mSourceFrameX + mCurrentAnimationFrameIndex) * PATCH_SIZE;
	sourceRect.top = (mSourceFrameY + mCurrentDirectionFrameIndex) * PATCH_SIZE;
	sourceRect.right = sourceRect.left + PATCH_SIZE;
	sourceRect.bottom = sourceRect.top + PATCH_SIZE;

	const XMFLOAT2 position(x, y);
	constexpr XMFLOAT2 origin(PATCH_SIZE_HALF, PATCH_SIZE_HALF);

	ID3D11ShaderResourceView* const pSpriteGPU = mpSprite->GetSpriteGPU();

	SpriteBatch* const pSpriteBatch = mpGraphics->GetSpriteBatch();

	pSpriteBatch->Draw(
		pSpriteGPU,
		position,
		&sourceRect,
		Colors::White * alpha,
		mRotationAngle,
		origin,
		mScale,
		effect,
		0.f
	);
}

void Image::DrawPatch() const
{
	DrawPatch(mPosition.x, mPosition.y, 1.f, SpriteEffects_None);
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
