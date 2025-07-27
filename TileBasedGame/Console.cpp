#include "Console.h"

Console::Console(Graphics* const pGraphics, Input* const pInput, TextDX* pTextDX)
	: mpGraphics(pGraphics)
	, mpInput(pInput)
	, mpTextDX(pTextDX)
	, mpBrush(nullptr)
	, mWidth(500.f)
	, mHeight(500.f)
	, mX(pGraphics->GetWidth() - mWidth)
	, mY(0)
	, mbVisible(false)
	, mPromptBuffer{ '\0', }
{
	ID2D1RenderTarget* const pRenderTarget = mpGraphics->GetD2DRenderTarget();

	constexpr float brightness = 0.5f;

	const D2D1_COLOR_F backgroundColor = D2D1::ColorF(brightness, brightness, brightness, 0.8f);
	pRenderTarget->CreateSolidColorBrush(backgroundColor, &mpBrush);
}

Console::~Console()
{
	while (!mHistory.empty())
	{
		TCHAR* p = mHistory.back();
		mHistory.pop_back();

		delete[] p;
	}

	SafeRelease(mpBrush);
}

void Console::Draw()
{
	if (!mbVisible)
	{
		return;
	}

	ID2D1RenderTarget* const pRenderTarget = mpGraphics->GetD2DRenderTarget();

	D2D1_RECT_F rect;
	rect.right = static_cast<float>(mpGraphics->GetWidth());
	rect.bottom = mHeight;
	rect.left = rect.right - mWidth;
	rect.top = 0;

	pRenderTarget->FillRectangle(&rect, mpBrush);

	const float fontSize = mpTextDX->GetFontSize();

	TCHAR inputStr[EInputConstant::MAX_STR_LEN + 1];
	mpInput->GetInputStr(inputStr);

	_stprintf(mPromptBuffer, TEXT("> %s"), inputStr);

	const float TEXT_HEIGHT = fontSize * 1.5f;

	float textY = rect.bottom - TEXT_HEIGHT;

	mpTextDX->Print(mPromptBuffer, rect.left, textY);

	for (int i = static_cast<int>(mHistory.size()); i > 0; --i)
	{
		textY -= TEXT_HEIGHT;

		TCHAR* pHistory = mHistory.back();
		mHistory.pop_back();

		mpTextDX->Print(pHistory, rect.left, textY);

		mHistory.push_front(pHistory);
	}
}

void Console::OnInput(const WPARAM wParam)
{
	switch (wParam)
	{
	case VK_RETURN:
		{
			TCHAR* pStrBuffer = nullptr;
			if (mHistory.size() == EConsoleConstant::HISTORY_COUNT)
			{
				pStrBuffer = mHistory.front();
				mHistory.pop_front();
			}
			else
			{
				pStrBuffer = new TCHAR[EInputConstant::MAX_STR_LEN + 1];
			}

			mpInput->GetInputStr(pStrBuffer);
			mHistory.push_back(pStrBuffer);

			mpInput->ClearStr();
		}
		break;

	default:
		mpInput->OnCharIn(static_cast<TCHAR>(wParam));
		break;
	}
}