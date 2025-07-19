#pragma once

#include <cctype>

#include "Graphics.h"
#include "resource.h"
#include "Sprite.h"

enum EImageConstant
{
	PATCH_SIZE = 16
};

class Image final
{
public:
	Image(Sprite* const pSprite, Graphics* const pGraphics, const int frameEntryX, const int frameEntryY, const int frameCount);
	Image(Sprite* const pSprite, Graphics* const pGraphics);
	virtual ~Image() = default;

	inline float GetX() const;
	inline void SetX(const float x);

	inline float GetY() const;
	inline void SetY(const float y);

	inline void SetPosition(const float x, const float y);

	inline float GetRotationAngle() const;
	inline void SetRotationAngle(const float rotationAngle);

	inline float GetScalingX() const;
	inline void SetScalingX(const float scale);

	inline float GetScalingY() const;
	inline void SetScalingY(const float scale);

	inline void SetScale(const float scale);

	inline void SetAnimationState(const bool bAnimating);

	void Draw() const;

	void Update(const float frameTime);

private:
	Graphics* mpGraphics;
	Sprite* mpSprite;

	float mX;
	float mY;
	float mRotationAngle;
	float mScalingX;
	float mScalingY;

	int mFrameEntryX;
	int mFrameEntryY;
	int mFrameCount;
	float mFrameDelay;
	float mAnimationTimer;
	bool mbAnimating;
	int mCurrentFrameNumber;
};

inline float Image::GetX() const
{
	return mX;
}

inline void Image::SetX(const float x)
{
	if (x <= 0.f)
	{
		mX = 0;

		return;
	}

	const int screenWidth = mpGraphics->GetWidth();
	if (x >= screenWidth)
	{
		mX = static_cast<float>(screenWidth);

		return;
	}

	mX = x;
}

inline float Image::GetY() const
{
	return mY;
}

inline void Image::SetY(const float y)
{
	if (y <= 0.f)
	{
		mY = 0;

		return;
	}

	const int screenHeight = mpGraphics->GetHeight();
	if (y >= screenHeight)
	{
		mY = static_cast<float>(screenHeight);

		return;
	}

	mY = y;
}

inline void Image::SetPosition(const float x, const float y)
{
	SetX(x);
	SetY(y);
}

inline float Image::GetRotationAngle() const
{
	return mRotationAngle;
}

inline void Image::SetRotationAngle(const float rotationAngle)
{
	mRotationAngle = rotationAngle;
}

inline float Image::GetScalingX() const
{
	return mScalingX;
}

inline void Image::SetScalingX(const float scale)
{
	mScalingX = scale;
}

inline float Image::GetScalingY() const
{
	return mScalingY;
}

inline void Image::SetScalingY(const float scale)
{
	mScalingY = scale;
}

inline void Image::SetScale(const float scale)
{
	SetScalingX(scale);
	SetScalingY(scale);
}

inline void Image::SetAnimationState(const bool bAnimating)
{
	mbAnimating = bAnimating;
}