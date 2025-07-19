#pragma once

#include <d3d11.h>
#include "Graphics.h"

class Sprite final
{
public:
	Sprite(const int resourceId, Graphics* const pGraphics);
	~Sprite();

	inline ID3D11ShaderResourceView* GetSpriteGPU() const;

private:
	ID3D11ShaderResourceView* mpSpriteGPU;
};

inline ID3D11ShaderResourceView* Sprite::GetSpriteGPU() const
{
	return mpSpriteGPU;
}