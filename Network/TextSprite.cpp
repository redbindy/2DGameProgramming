#include "TextSprite.h"

TextSprite::TextSprite(Graphics* const pGraphics)
	: mpGraphics(pGraphics)
	, mFontSprite(IDB_FONT, pGraphics)
	, mX(0)
	, mY(0)
	, mDegree(0)
	, mScale(1.f)
{
	std::cout << "Initialize TextSprite" << std::endl;
}

void TextSprite::Print(const char* str) const
{
	Print(str, mX, mY);
}

void TextSprite::Print(const char* str, const float x, const float y) const
{
	constexpr int PATCH_SIZE = 16;
	constexpr int FONT_GAP = 2;

	constexpr int COL_COUNT = 13;

	int lineNumber = 0;
	int charNumber = 0;
	for (const char* pStr = str; *pStr != '\0'; ++pStr)
	{
		const char ch = *pStr;
		assert(ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z' || isspace(ch));

		RECT sourceRect;

		++charNumber;

		int descenderLength = 0;
		if (ch >= 'A' && ch <= 'Z')
		{
			// ch &= ~' ';

			const int charIndex = ch - 'A';

			const int row = charIndex / COL_COUNT;
			const int col = charIndex % COL_COUNT;

			sourceRect.left = col * PATCH_SIZE;
			sourceRect.top = row * (PATCH_SIZE + FONT_GAP);
		}
		else if (ch >= 'a' && ch <= 'z')
		{
			constexpr int LOWER_CASE_ROW_OFFSET = 2;

			const int charIndex = ch - 'a';

			const int row = charIndex / COL_COUNT;
			const int col = charIndex % COL_COUNT;

			descenderLength = 7;

			sourceRect.left = col * PATCH_SIZE;
			sourceRect.top = LOWER_CASE_ROW_OFFSET * (PATCH_SIZE + FONT_GAP) + row * (PATCH_SIZE + descenderLength + FONT_GAP);
		}
		else
		{
			if (ch == '\n')
			{
				lineNumber += 2;
				charNumber = 0;
			}

			continue;
		}

		sourceRect.right = sourceRect.left + PATCH_SIZE;
		sourceRect.bottom = sourceRect.top + PATCH_SIZE + descenderLength;

		const XMFLOAT2 origin(PATCH_SIZE * 0.5f, PATCH_SIZE * 0.5f);

		const int strIndex = charNumber * PATCH_SIZE;
		const int lineIndex = lineNumber * PATCH_SIZE;

		const XMFLOAT2 position(x + strIndex, y + lineIndex);
		const XMFLOAT2 scale(mScale, mScale);

		ID3D11ShaderResourceView* const pSpriteGPU = mFontSprite.GetSpriteGPU();

		SpriteBatch* const pSprite = mpGraphics->GetSpriteBatch();
		pSprite->Draw(
			pSpriteGPU,
			position,
			&sourceRect,
			Colors::White,
			mDegree,
			origin,
			scale,
			SpriteEffects_None,
			0.f
		);
	}
}
