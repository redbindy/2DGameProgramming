#pragma once

#include <cctype>

#include "Graphics.h"
#include "resource.h"
#include "Sprite.h"

enum EImageConstant
{
	PATCH_SIZE = 16,
	PATCH_SIZE_HALF = PATCH_SIZE / 2
};

class Image
{
public:
	Image(
		Sprite* const pSprite, Graphics*
		const pGraphics,
		const int sourceFrameX,
		const int sourceFrameY,
		const int animationFrameCount,
		const int directionalFrameCount
	);
	Image(Sprite* const pSprite, Graphics* const pGraphics);
	virtual ~Image() = default;

	inline float GetX() const;
	inline void SetX(const float x);

	inline float GetY() const;
	inline void SetY(const float y);

	inline XMFLOAT2 GetPosition() const;
	inline void SetPosition(const float x, const float y);
	inline void SetPosition(const XMFLOAT2 position);

	inline float GetRotationAngle() const;
	inline void SetRotationAngle(const float rotationAngle);

	inline float GetScaleX() const;
	inline void SetScaleX(const float scale);

	inline float GetScaleY() const;
	inline void SetScaleY(const float scale);

	inline XMFLOAT2 GetScale() const;
	inline void SetScale(const float scale);

	inline void SetAnimationState(const bool bAnimating);

	void Draw() const;
	void DrawPatch() const;

	virtual void Update(const float frameTime);

protected:
	Graphics* mpGraphics;

private:
	Sprite* mpSprite;

	XMFLOAT2 mPosition;
	float mRotationAngle;
	XMFLOAT2 mScale;

	int mSourceFrameX;
	int mSourceFrameY;
	int mAnimationFrameCount;
	int mDirectionalFrameCount;
	int mCurrentAnimationFrameIndex;
	int mCurrentDirectionFrameIndex;
	float mFrameDelay;
	float mAnimationTimer;
	bool mbAnimating;
};

inline float Image::GetX() const
{
	return mPosition.x;
}

inline void Image::SetX(const float x)
{
	mPosition.x = x;
}

inline float Image::GetY() const
{
	return mPosition.y;
}

inline void Image::SetY(const float y)
{
	mPosition.y = y;
}

inline XMFLOAT2 Image::GetPosition() const
{
	return mPosition;
}

inline void Image::SetPosition(const float x, const float y)
{
	SetX(x);
	SetY(y);
}

inline void Image::SetPosition(const XMFLOAT2 position)
{
	SetPosition(position.x, position.y);
}

inline float Image::GetRotationAngle() const
{
	return mRotationAngle;
}

inline void Image::SetRotationAngle(const float rotationAngle)
{
	mRotationAngle = rotationAngle;
}

inline float Image::GetScaleX() const
{
	return mScale.x;
}

inline void Image::SetScaleX(const float scale)
{
	mScale.x = scale;
}

inline float Image::GetScaleY() const
{
	return mScale.y;
}

inline void Image::SetScaleY(const float scale)
{
	mScale.y = scale;
}

inline XMFLOAT2 Image::GetScale() const
{
	return mScale;
}

inline void Image::SetScale(const float scale)
{
	mScale = XMFLOAT2(scale, scale);
}

inline void Image::SetAnimationState(const bool bAnimating)
{
	mbAnimating = bAnimating;
}