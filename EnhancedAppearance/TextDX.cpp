#include "TextDX.h"

TextDX::TextDX(Graphics* const pGraphics)
	: mpGraphics(pGraphics)
	, mpBrush(nullptr)
	, mpWriteFactory(nullptr)
	, mpTextFormat(nullptr)
	, mFontSize(16.f)
{
	std::cout << "Initialize TextDX" << std::endl;

	ID2D1RenderTarget* const pRenderTarget = pGraphics->GetD2DRenderTarget();

	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &mpBrush);

	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mpWriteFactory)
	);
	if (FAILED(hr))
	{
		std::cerr << "DWriteCreateFactory" << std::endl;

		DebugBreak();
	}

	hr = mpWriteFactory->CreateTextFormat(
		TEXT("¸¼Àº °íµñ"),
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		mFontSize,
		TEXT("ko-KR"),
		&mpTextFormat
	);

	if (FAILED(hr))
	{
		std::cout << "CraeteTextFormat" << std::endl;

		DebugBreak();
	}

	hr = mpTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr))
	{
		std::cerr << "SetTextAlignment" << std::endl;
	}
}

TextDX::~TextDX()
{
	SafeRelease(mpTextFormat);
	SafeRelease(mpWriteFactory);
	SafeRelease(mpBrush);
}

void TextDX::Print(const TCHAR* str, const float x, const float y)
{
	assert(str != nullptr);

	D2D1_RECT_F rect;
	rect.left = x;
	rect.top = y;
	rect.right = static_cast<float>(mpGraphics->GetWidth());
	rect.bottom = static_cast<float>(mpGraphics->GetHeight());

	Print(str, rect, DWRITE_TEXT_ALIGNMENT_LEADING);
}

void TextDX::Print(const TCHAR* str, D2D1_RECT_F rect, DWRITE_TEXT_ALIGNMENT alignmentMode)
{
	ID2D1RenderTarget* const pRenderTarget = mpGraphics->GetD2DRenderTarget();

	mpTextFormat->SetTextAlignment(alignmentMode);

	const UINT32 strLength = lstrlen(str);

	pRenderTarget->DrawTextW(
		str,
		strLength,
		mpTextFormat,
		&rect,
		mpBrush
	);
}
