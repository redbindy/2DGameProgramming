#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>
#include <deque>

#include "Graphics.h"
#include "Input.h"
#include "TextDX.h"

enum EConsoleConstant
{
	HISTORY_COUNT = 16, 
	EXTRA_COUNT = 8
};

class Console final
{
public:
	Console(Graphics* const pGraphics, Input* const mpInput, TextDX* pTextDX);
	virtual ~Console();

	inline float GetWidth() const;
	inline float GetHeight() const;

	inline float GetX() const;
	inline float GetY() const;

	inline bool IsVisible() const;
	inline void SwapVisible();

	void OnInput(const WPARAM wParam);

	void Draw();

private:
	Graphics* mpGraphics;
	Input* mpInput;
	TextDX* mpTextDX;

	ID2D1SolidColorBrush* mpBrush;

	float mWidth;
	float mHeight;

	float mX;
	float mY;

	bool mbVisible;

	TCHAR mPromptBuffer[EInputConstant::MAX_STR_LEN + EXTRA_COUNT];

	std::deque<TCHAR*> mHistory;
};

inline float Console::GetWidth() const
{
	return mWidth;
}

inline float Console::GetHeight() const
{
	return mHeight;
}

inline float Console::GetX() const
{
	return mX;
}

inline float Console::GetY() const
{
	return mY;
}

inline bool Console::IsVisible() const
{
	return mbVisible;
}

inline void Console::SwapVisible()
{
	mbVisible = !mbVisible;
}
