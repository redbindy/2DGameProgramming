#include "Sprite.h"

Sprite::Sprite(const int resourceId, Graphics* const pGraphics)
	: mpSpriteGPU(nullptr)
	, mWidth(0.f)
	, mHeight(0.f)
{
	std::cout << "CreateSprite" << std::endl;

	const HRSRC hPngInfo = FindResource(nullptr, MAKEINTRESOURCE(resourceId), TEXT("PNG"));
	if (hPngInfo == nullptr)
	{
		std::cerr << __LINE__ << ' ' << "FindResource" << std::endl;

		DebugBreak();
	}
	assert(hPngInfo != nullptr);

	const HGLOBAL hPng = LoadResource(nullptr, hPngInfo);
	{
		if (hPng == nullptr)
		{
			std::cerr << __LINE__ << ' ' << "LoadResource" << std::endl;

			DebugBreak();
		}
		assert(hPng != nullptr);

		const void* pPngData = LockResource(hPng);
		if (pPngData == nullptr)
		{
			std::cerr << __LINE__ << ' ' << "LockResource" << std::endl;

			DebugBreak();
		}
		assert(pPngData);

		const DWORD imageSize = SizeofResource(nullptr, hPngInfo);

		ID3D11Device* const pDevice = pGraphics->GetDevice();
		ID3D11DeviceContext* const pDeviceContext = pGraphics->GetDeviceContext();

		ID3D11Resource* pResource = nullptr;
		const HRESULT hr = CreateWICTextureFromMemory(
			pDevice,
			pDeviceContext,
			static_cast<const uint8_t*>(pPngData),
			imageSize,
			&pResource,
			&mpSpriteGPU
		);

		ID3D11Texture2D* pTexture = nullptr;
		pResource->QueryInterface<ID3D11Texture2D>(&pTexture);

		D3D11_TEXTURE2D_DESC desc;
		pTexture->GetDesc(&desc);

		mWidth = static_cast<float>(desc.Width);
		mHeight = static_cast<float>(desc.Height);

		if (FAILED(hr))
		{
			std::cerr << __LINE__ << ' ' << "CreateWICTextureFromMemory" << std::endl;

			DebugBreak();
		}

		pTexture->Release();
		pResource->Release();
	}
	FreeResource(hPng);
}

Sprite::~Sprite()
{
	SafeRelease(mpSpriteGPU);
}
