#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <d2d1_1.h>
#pragma comment(lib, "d2d1.lib")

#include <iostream>
#include <cassert>
#include <SpriteBatch.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

#include "Constants.h"

using namespace DirectX;

class Graphics final
{
public:
	Graphics(const HWND hWnd, const int width, const int height);
	virtual ~Graphics();

	void BeginRendering();
	void EndRendering();

	void OnResize(const int width, const int height);

	inline int GetWidth() const;
	inline int GetHeight() const;

	inline ID3D11Device* GetDevice() const;
	inline ID3D11DeviceContext* GetDeviceContext() const;
	inline IDXGISwapChain* GetSwapChain() const;
	inline SpriteBatch* GetSpriteBatch() const;
	inline ID2D1RenderTarget* GetD2DRenderTarget() const;

private:
	HWND mhWnd;
	int mWidth;
	int mHeight;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain* mpSwapChain;

	ID3D11RenderTargetView* mpRenderTargetViewGPU;

	SpriteBatch* mpSpriteBatch;
	ID3D11ShaderResourceView* mpTextureResourceViewGPU;
	CommonStates* mpCommonStates;

	ID2D1Factory1* mpD2DFactory;
	ID2D1RenderTarget* mpD2DRenderTarget;

private:
	void cleanupResources();
	void resetResources();
};

inline int Graphics::GetWidth() const
{
	return mWidth;
}

inline int Graphics::GetHeight() const
{
	return mHeight;
}

inline ID3D11Device* Graphics::GetDevice() const
{
	return mpDevice;
}

inline ID3D11DeviceContext* Graphics::GetDeviceContext() const
{
	return mpDeviceContext;
}

inline IDXGISwapChain* Graphics::GetSwapChain() const
{
	return mpSwapChain;
}

inline SpriteBatch* Graphics::GetSpriteBatch() const
{
	return mpSpriteBatch;
}

inline ID2D1RenderTarget* Graphics::GetD2DRenderTarget() const
{
	return mpD2DRenderTarget;
}