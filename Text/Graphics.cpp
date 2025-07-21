#include "Graphics.h"

Graphics::Graphics(const HWND hWnd, const int width, const int height)
	: mhWnd(hWnd)
	, mWidth(width)
	, mHeight(height)
	, mpDevice(nullptr)
	, mpDeviceContext(nullptr)
	, mpSwapChain(nullptr)
	, mpRenderTargetViewGPU(nullptr)
	, mpSpriteBatch(nullptr)
	, mpTextureResourceViewGPU(nullptr)
	, mpCommonStates(nullptr)
	, mpD2DRenderTarget(nullptr)
{
	std::cout << "Initialize Graphics" << std::endl;

	assert(hWnd != nullptr);
	assert(width >= 0);
	assert(height >= 0);

	resetResources();

	OnResize(width, height);

	assert(mpDeviceContext != nullptr);
	mpSpriteBatch = new SpriteBatch(mpDeviceContext);

	mpCommonStates = new CommonStates(mpDevice);
}

Graphics::~Graphics()
{
	cleanupResources();
}

void Graphics::BeginRendering()
{
	mpDeviceContext->OMSetRenderTargets(1, &mpRenderTargetViewGPU, nullptr);

	constexpr float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	mpDeviceContext->ClearRenderTargetView(mpRenderTargetViewGPU, clearColor);

	mpSpriteBatch->Begin(SpriteSortMode_Deferred, mpCommonStates->NonPremultiplied());
	mpD2DRenderTarget->BeginDraw();
}

void Graphics::EndRendering()
{
	mpD2DRenderTarget->EndDraw();
	mpSpriteBatch->End();

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

			DebugBreak();
		}
	}
}

void Graphics::DrawSprite(
	ID3D11ShaderResourceView* pSpriteGPU,
	XMFLOAT2 position,
	RECT* sourceRect,
	FXMVECTOR color,
	float rotationAngle,
	XMFLOAT2 origin,
	XMFLOAT2 scaling,
	SpriteEffects spriteEffects,
	float layerDepth
)
{
	mpSpriteBatch->Draw(
		pSpriteGPU,
		position,
		sourceRect,
		color,
		rotationAngle,
		origin,
		scaling,
		spriteEffects,
		layerDepth
	);
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

			DebugBreak();
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

		DebugBreak();
	}

	assert(pBackBufferGPU != nullptr);

	hr = mpDevice->CreateRenderTargetView(
		pBackBufferGPU,
		nullptr,
		&mpRenderTargetViewGPU
	);
	SafeRelease(pBackBufferGPU);

	if (FAILED(hr))
	{
		std::cerr << __LINE__ << ' ' << "CreateRenderTargetView" << std::endl;

		DebugBreak();
	}

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	mpDeviceContext->RSSetViewports(1, &vp);

	IDXGISurface* pDXGISurface = nullptr;
	mpSwapChain->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
	assert(pDXGISurface != nullptr);

	ID2D1Factory1* pFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
	if (FAILED(hr))
	{
		std::cerr << "D2D1CreateFactory" << std::endl;

		DebugBreak();
	}

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	hr = pFactory->CreateDxgiSurfaceRenderTarget(pDXGISurface, props, &mpD2DRenderTarget);
	if (FAILED(hr))
	{
		std::cerr << "CreateDxgiSurfaceRenderTarget" << std::endl;

		DebugBreak();
	}

	pFactory->Release();
	pDXGISurface->Release();
}

void Graphics::cleanupResources()
{
	SafeRelease(mpD2DRenderTarget);
	delete mpCommonStates;
	delete mpSpriteBatch;
	SafeRelease(mpRenderTargetViewGPU);
	SafeRelease(mpSwapChain);
	SafeRelease(mpDeviceContext);
	SafeRelease(mpDevice);
}

void Graphics::resetResources()
{
	cleanupResources();

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // D2D 호환용
#if defined(DEBUG) || defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0
	};

	// 어댑터 스펙 확인용
	UINT sampleCount = 1;
	UINT sampleQuality = 0;

	ID3D11Device* pTempDevice = nullptr;
	ID3D11DeviceContext* pTempDeviceContext = nullptr;
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&pTempDevice,
			nullptr,
			&pTempDeviceContext
		);

		if (FAILED(hr))
		{
			std::cerr << "CreateDevice" << std::endl;

			DebugBreak();
		}

		constexpr UINT preferredSampleCounts[] = { 4, 2 };
		UINT sampleQualityLevels = 0;

		for (const UINT sc : preferredSampleCounts)
		{
			hr = pTempDevice->CheckMultisampleQualityLevels(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				sc,
				&sampleQualityLevels
			);

			if (FAILED(hr))
			{
				std::cerr << "CheckMultisampleQualityLevels" << std::endl;

				DebugBreak();
			}

			if (sampleQualityLevels > 0)
			{
				sampleCount = sc;
				sampleQuality = sampleQualityLevels - 1;

				break;
			}
		}
	}
	SafeRelease(pTempDeviceContext);
	SafeRelease(pTempDevice);

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
	swapChainDesc.SampleDesc.Count = sampleCount;
	swapChainDesc.SampleDesc.Quality = sampleQuality;
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

		DebugBreak();
	}
}
