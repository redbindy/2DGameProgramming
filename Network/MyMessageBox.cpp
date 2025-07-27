#include "MyMessageBox.h"

MyMessageBox::MyMessageBox(Graphics* const pGraphics, Input* const pInput, TextDX* const pTextDX)
	: mpGraphics(pGraphics)
	, mpInput(pInput)
	, mpTextDX(pTextDX)
	, mpBoxBrush(nullptr)
	, mpButtonBrush(nullptr)
	, mbVisible(false)
{
	const int screenWidth = mpGraphics->GetWidth();
	const int screenHeight = mpGraphics->GetHeight();

	ID2D1RenderTarget* const pD2DRenderTarget = mpGraphics->GetD2DRenderTarget();

	pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkRed), &mpBoxBrush);
	pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &mpButtonBrush);

	mBoxRect.rect.left = screenWidth * 0.25f;
	mBoxRect.rect.top = screenHeight * 0.25f;
	mBoxRect.rect.right = mBoxRect.rect.left + screenWidth * 0.5f;
	mBoxRect.rect.bottom = mBoxRect.rect.top + screenHeight * 0.5f;
	mBoxRect.radiusX = 30.f;
	mBoxRect.radiusY = 30.f;

	const float boxWidth = mBoxRect.rect.right - mBoxRect.rect.left;
	const float boxHeight = mBoxRect.rect.bottom - mBoxRect.rect.top;

	const float buttonWidth = boxWidth * 0.15f;
	const float buttonHeight = boxWidth * 0.025f;

	mButtonRect.rect.left = mBoxRect.rect.right - buttonWidth;
	mButtonRect.rect.top = mBoxRect.rect.bottom - buttonHeight;
	mButtonRect.rect.right = mButtonRect.rect.left + buttonWidth;
	mButtonRect.rect.bottom = mButtonRect.rect.top + buttonHeight;
}

MyMessageBox::~MyMessageBox()
{
	SafeRelease(mpButtonBrush);
	SafeRelease(mpBoxBrush);
}

void MyMessageBox::Update()
{
	if (mpInput->IsKeyPressed(VK_ESCAPE))
	{
		mbVisible = true;
	}

	const int screenWidth = mpGraphics->GetWidth();
	const int screenHeight = mpGraphics->GetHeight();

	mBoxRect.rect.left = screenWidth * 0.25f;
	mBoxRect.rect.top = screenHeight * 0.25f;
	mBoxRect.rect.right = mBoxRect.rect.left + screenWidth * 0.5f;
	mBoxRect.rect.bottom = mBoxRect.rect.top + screenHeight * 0.5f;
	mBoxRect.radiusX = 30.f;
	mBoxRect.radiusY = 30.f;

	const float boxWidth = mBoxRect.rect.right - mBoxRect.rect.left;
	const float boxHeight = mBoxRect.rect.bottom - mBoxRect.rect.top;

	const float buttonWidth = boxWidth * 0.15f;
	const float buttonHeight = boxWidth * 0.025f;

	mButtonRect.rect.left = mBoxRect.rect.right - buttonWidth;
	mButtonRect.rect.top = mBoxRect.rect.bottom - buttonHeight;
	mButtonRect.rect.right = mButtonRect.rect.left + buttonWidth;
	mButtonRect.rect.bottom = mButtonRect.rect.top + buttonHeight;

	ID2D1RenderTarget* const pD2DRenderTarget = mpGraphics->GetD2DRenderTarget();

	if (!mbVisible || !mpInput->IsLButtonClicked())
	{
		return;
	}

	const int mouseX = mpInput->GetMouseX();
	const int mouseY = mpInput->GetMouseY();

	mbVisible = mouseX < mButtonRect.rect.left
		|| mouseX > mButtonRect.rect.right
		|| mouseY < mButtonRect.rect.top
		|| mouseY > mButtonRect.rect.bottom;
}

void MyMessageBox::Draw() const
{
	if (!mbVisible)
	{
		return;
	}

	ID2D1RenderTarget* const pD2DRenderTarget = mpGraphics->GetD2DRenderTarget();

	pD2DRenderTarget->FillRoundedRectangle(&mBoxRect, mpBoxBrush);
	mpTextDX->Print(TEXT("MessageBox"), mBoxRect.rect, DWRITE_TEXT_ALIGNMENT_CENTER);

	pD2DRenderTarget->FillRoundedRectangle(&mButtonRect, mpButtonBrush);
	mpTextDX->Print(TEXT("OK"), mButtonRect.rect, DWRITE_TEXT_ALIGNMENT_CENTER);
}
