#pragma once

#include "Graphics.h"
#include "Sprite.h"
#include "resource.h"

class TextSprite final
{
public:
	TextSprite(Graphics* const pGraphics);
	virtual ~TextSprite() = default;

	inline float GetX() const;
	inline float GetY() const;
	inline void SetPosition(const float x, const float y);

	inline float GetDegree() const;
	inline void SetDegree(const float degree);

	inline float GetScale() const;
	inline void SetScale(const float scale);

	void Print(const char* str) const;
	void Print(const char* str, const float x, const float y) const;

private:
	Graphics* mpGraphics;
	Sprite mFontSprite;

	float mX;
	float mY;

	float mDegree;
	float mScale;
};

inline float TextSprite::GetX() const
{
	return mX;
}

inline float TextSprite::GetY() const
{
	return mY;
}

inline void TextSprite::SetPosition(const float x, const float y)
{
	mX = x;
	mY = y;
}

inline float TextSprite::GetDegree() const
{
	return mDegree;
}

inline void TextSprite::SetDegree(const float degree)
{
	mDegree = degree;
}

inline float TextSprite::GetScale() const
{
	return mScale;
}

inline void TextSprite::SetScale(const float scale)
{
	mScale = scale;
}