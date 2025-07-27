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

	constexpr int DIVISION_SIZE = 16;
	constexpr int PATCH_COL_COUNT = 8;

	int indexBuffer[DIVISION_SIZE][DIVISION_SIZE] = {
		{11, 11, 11, 11, 12, 11, 11, 30, 11, 12, 11, 11, 11, 11, 11, 11},
		{11, 11, 11, 12, 11, 11, 11, 11, 12, 11, 11, 11, 30, 11, 11, 11},
		{11, 12, 11, 11, 11, 11, 11, 11, 11, 11, 30, 11, 11, 11, 12, 11},
		{11, 11, 11, 30, 11, 11, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11},
		{11, 11, 12, 11, 11, 11, 11, 11, 30, 11, 11, 12, 11, 11, 11, 11},
		{11, 30, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 11, 11, 11, 11},
		{11, 11, 11, 11, 12, 11, 30, 11, 11, 11, 11, 11, 11, 12, 11, 11},
		{11, 11, 11, 12, 11, 11, 11, 11, 11, 30, 11, 11, 11, 11, 11, 11},
		{12, 11, 11, 11, 11, 11, 11, 30, 11, 11, 11, 11, 11, 11, 12, 11},
		{11, 11, 11, 11, 11, 12, 11, 11, 11, 11, 30, 11, 11, 11, 11, 11},
		{11, 11, 30, 11, 12, 11, 11, 11, 11, 11, 11, 11, 11, 30, 11, 11},
		{11, 11, 11, 11, 11, 12, 11, 11, 11, 11, 12, 11, 11, 11, 11, 11},
		{11, 12, 11, 11, 11, 30, 11, 11, 12, 11, 11, 11, 11, 11, 11, 11},
		{11, 11, 11, 11, 11, 11, 11, 11, 11, 30, 11, 11, 12, 11, 11, 11},
		{11, 11, 11, 12, 11, 11, 11, 11, 30, 11, 11, 11, 11, 11, 12, 11},
		{11, 11, 11, 11, 30, 11, 12, 11, 11, 11, 11, 11, 11, 30, 11, 11},
	};


	const int screenWidth = mpGraphics->GetWidth();
	const int screenHeight = mpGraphics->GetHeight();

	const int outputWidth = screenWidth / DIVISION_SIZE;
	const int outputHeight = screenHeight / DIVISION_SIZE;

	SpriteBatch* const pSpriteBatch = mpGraphics->GetSpriteBatch();

	for (int y = 0; y < DIVISION_SIZE; ++y)
	{
		for (int x = 0; x < DIVISION_SIZE; ++x)
		{
			const int index = indexBuffer[y][x];

			RECT sourceRect;
			sourceRect.left = index % PATCH_COL_COUNT * PATCH_SIZE;
			sourceRect.top = index / PATCH_COL_COUNT * PATCH_SIZE;
			sourceRect.right = sourceRect.left + PATCH_SIZE;
			sourceRect.bottom = sourceRect.top + PATCH_SIZE;

			const int tileX = static_cast<LONG>(mPosition.x) + x * outputWidth;
			const int tileY = static_cast<LONG>(mPosition.y) + y * outputHeight;

			for (float subHeight = 0; subHeight < outputHeight; subHeight += PATCH_SIZE)
			{
				for (float subWidth = 0; subWidth < outputWidth; subWidth += PATCH_SIZE)
				{
					RECT destinationRect;
					destinationRect.left = tileX + subWidth;
					destinationRect.top = tileY + subHeight;
					destinationRect.right = destinationRect.left + PATCH_SIZE;
					destinationRect.bottom = destinationRect.top + PATCH_SIZE;

					pSpriteBatch->Draw(pSpriteGPU, destinationRect, &sourceRect);
				}
			}
		}
	}
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
