#pragma once

#include "Graphics.h"
#include "Sprite.h"

class Text final
{
public:
	Text(Graphics* const pGraphics);
	virtual ~Text() = default;



private:
	Graphics* mpGraphics;
	Sprite mFontSprite;
};