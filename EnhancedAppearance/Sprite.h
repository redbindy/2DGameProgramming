#pragma once

#include <d3d11.h>
#include "Graphics.h"

class Sprite final
{
public:
	Sprite(const int resourceId, Graphics* const pGraphics);
	~Sprite();

	inline ID3D11ShaderResourceView* GetSpriteGPU() const;

	inline float GetWidth() const;
	inline float GetHeight() const;

private:
	ID3D11ShaderResourceView* mpSpriteGPU;

	float mWidth;
	float mHeight;
};

inline ID3D11ShaderResourceView* Sprite::GetSpriteGPU() const
{
	return mpSpriteGPU;
}

inline float Sprite::GetWidth() const
{
	return mWidth;
}

inline float Sprite::GetHeight() const
{
	return mHeight;
}