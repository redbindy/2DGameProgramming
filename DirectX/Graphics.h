#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d3d11.h>

#include <iostream>
#include <cassert>

class Graphics final
{
public:
	Graphics(const HWND hWnd, const int width, const int height);
	virtual ~Graphics();

	void Render();

	void OnResize(const int width, const int height);

private:
	HWND mhWnd;
	int mWidth;
	int mHeight;
	bool mbWindowMode;

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
	IDXGISwapChain* mpSwapChain;

	ID3D11RenderTargetView* mpRenderTargetView;

private:
	template<class COM>
	inline void safeRelease(COM*& p)
	{
		if (p != nullptr)
		{
			p->Release();
			p = nullptr;
		}
	}
};