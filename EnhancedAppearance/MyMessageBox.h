#pragma once

#include "Graphics.h"
#include "Input.h"
#include "TextDX.h"

class MyMessageBox
{
public:
	MyMessageBox(Graphics* const pGraphics, Input* const pInput, TextDX* const pTextDX);
	virtual ~MyMessageBox();

	virtual void Update();
	virtual void Draw() const;

protected:
	Graphics* mpGraphics;
	Input* mpInput;
	TextDX* mpTextDX;

	ID2D1SolidColorBrush* mpBoxBrush;
	ID2D1SolidColorBrush* mpButtonBrush;

	D2D1_ROUNDED_RECT mBoxRect;
	D2D1_ROUNDED_RECT mButtonRect;

	bool mbVisible;

private:
};