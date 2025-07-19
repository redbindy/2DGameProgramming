#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d3d11.h>

#include <iostream>
#include <cassert>

#include "Constants.h"

class Graphics final
{
public:
	Graphics(const HWND hWnd, const int width, const int height);
	virtual ~Graphics();

	void Render();
	void Present();

	void OnResize(const int width, const int height);

private:
	HWND mhWnd;
	int mWidth;
	int mHeight;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain* mpSwapChain;

	ID3D11RenderTargetView* mpRenderTargetViewGPU;

private:
	void cleanupResources();
	void resetResources();
};