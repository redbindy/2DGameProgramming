#include "Graphics.h"

Graphics::Graphics(const HWND hWnd, const int width, const int height)
	: mhWnd(hWnd)
	, mWidth(width)
	, mHeight(height)
	, mpDevice(nullptr)
	, mpDeviceContext(nullptr)
	, mpSwapChain(nullptr)
	, mpRenderTargetViewGPU(nullptr)
{
	std::cout << "Initialize Graphics" << std::endl;

	assert(hWnd != nullptr);
	assert(width >= 0);
	assert(height >= 0);

	resetResources();

	OnResize(width, height);
}

Graphics::~Graphics()
{
	cleanupResources();
}

void Graphics::Render()
{
	mpDeviceContext->OMSetRenderTargets(1, &mpRenderTargetViewGPU, nullptr);

	const float clearColor[] = { 1.f, 0.f, 1.f, 1.f };

	mpDeviceContext->ClearRenderTargetView(mpRenderTargetViewGPU, clearColor);
}

void Graphics::Present()
{
	HRESULT hr = mpSwapChain->Present(1, 0);
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			resetResources();
		}
		else
		{
			std::cerr << __LINE__ << ' ' << "Failed to handle device lost" << std::endl;

			ExitProcess(0);
		}
	}
}

void Graphics::OnResize(const int width, const int height)
{
	assert(width >= 0);
	assert(height >= 0);
	assert(mpSwapChain != nullptr);

	mWidth = width;
	mHeight = height;

	mpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	SafeRelease(mpRenderTargetViewGPU);

	HRESULT hr = mpSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			resetResources();
		}
		else
		{
			std::cerr << __LINE__ << ' ' << "ResizeBuffer" << std::endl;
			std::cerr << __LINE__ << ' ' << "Failed to handle device lost" << std::endl;

			ExitProcess(0);
		}
	}

	ID3D11Texture2D* pBackBufferGPU = nullptr;
	hr = mpSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBufferGPU)
	);

	if (FAILED(hr))
	{
		std::cerr << __LINE__ << ' ' << "GetBuffer" << std::endl;

		ExitProcess(0);
	}

	hr = mpDevice->CreateRenderTargetView(
		pBackBufferGPU,
		nullptr,
		&mpRenderTargetViewGPU
	);
	SafeRelease(pBackBufferGPU);

	if (FAILED(hr))
	{
		std::cerr << __LINE__ << ' ' << "CreateRenderTargetView" << std::endl;

		ExitProcess(0);
	}

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	mpDeviceContext->RSSetViewports(1, &vp);
}

void Graphics::cleanupResources()
{
	SafeRelease(mpRenderTargetViewGPU);
	SafeRelease(mpSwapChain);
	SafeRelease(mpDeviceContext);
	SafeRelease(mpDevice);
}

void Graphics::resetResources()
{
	cleanupResources();

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
	swapChainDesc.BufferDesc.Width = mWidth;
	swapChainDesc.BufferDesc.Height = mHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = static_cast<UINT>(FRAME_RATE);
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = mhWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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

	if (FAILED(hr))
	{
		std::cerr << __LINE__ << ' ' << "D3D11CreateDeviceAndSwapChain" << std::endl;

		ExitProcess(0);
	}
}
