#include "Graphics.h"

#define LOG_IF_FAILED(hr, func) if (FAILED((hr))) { std::cerr << __LINE__ << ' ' << (func) << std::endl; }

Graphics::Graphics(const HWND hWnd, const int width, const int height)
	: mhWnd(hWnd)
	, mWidth(width)
	, mHeight(height)
	, mbWindowMode(true)
	, mpDevice(nullptr)
	, mpDeviceContext(nullptr)
	, mpSwapChain(nullptr)
	, mpRenderTargetView(nullptr)
{
	assert(hWnd != nullptr);
	assert(width >= 0);
	assert(height >= 0);

	UINT creationFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0
	};

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 120;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = mbWindowMode;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mpSwapChain,
		&mpDevice,
		nullptr,
		&mpDeviceContext
	);

	LOG_IF_FAILED(hr, "CreateDeviceAndSwapChain");

	OnResize(width, height);
}

Graphics::~Graphics()
{
	safeRelease(mpSwapChain);
	safeRelease(mpDeviceContext);
	safeRelease(mpDevice);
}

void Graphics::Render()
{
	mpDeviceContext->OMSetRenderTargets(1, &mpRenderTargetView, nullptr);

	const float clearColor[] = { 1.f, 0.f, 1.f, 1.f };
	mpDeviceContext->ClearRenderTargetView(mpRenderTargetView, clearColor);

	mpSwapChain->Present(1, 0);
}

void Graphics::OnResize(const int width, const int height)
{
	if (mpSwapChain == nullptr)
	{
		return;
	}

	mpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	safeRelease(mpRenderTargetView);

	HRESULT hr = mpSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	LOG_IF_FAILED(hr, "ResizeBuffer");

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = mpSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	);
	assert(pBackBuffer != nullptr);

	LOG_IF_FAILED(hr, "GetBuffer");

	hr = mpDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&mpRenderTargetView
	);
	safeRelease(pBackBuffer);

	LOG_IF_FAILED(hr, "CreateRenderTargetView");

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	mpDeviceContext->RSSetViewports(1, &vp);
}