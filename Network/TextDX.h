#pragma once

#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

#include "Graphics.h"

class TextDX final
{
public:
	TextDX(Graphics* const pGraphics);
	virtual ~TextDX();

	inline float GetFontSize() const;

	void Print(const TCHAR* str, const float x, const float y);
	void Print(const TCHAR* str, D2D1_RECT_F rect, DWRITE_TEXT_ALIGNMENT alignmentMode);

private:
	Graphics* mpGraphics;

	ID2D1SolidColorBrush* mpBrush;

	IDWriteFactory* mpWriteFactory;
	IDWriteTextFormat* mpTextFormat;

	float mFontSize;
};

inline float TextDX::GetFontSize() const
{
	return mFontSize;
}
