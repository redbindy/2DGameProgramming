#include "Input.h"

Input::Input(const HWND hWnd, const bool bMouseCapture)
	: mhWnd(hWnd)
	, mpGameInput(nullptr)
	, mpKeyboard(nullptr)
	, mKeyStates{}
	, mKeyStateCount(0)
	, mpGamepad(nullptr)
	, mGamepadState{}
	, mbVibrating(false)
	, mVibrationTimeLeft(0)
	, mMouseX(0)
	, mMouseY(0)
	, mbMouseCaptured(bMouseCapture)
	, mbLButton(false)
	, mbRButton(false)
{
	std::cout << "Initialize Input" << std::endl;

	assert(hWnd != nullptr);

	HRESULT hr = GameInputCreate(&mpGameInput);

	if (FAILED(hr))
	{
		std::cerr << __LINE__ << ' ' << "GameInputCreate" << std::endl;

		ExitProcess(0);
	}

	IGameInputReading* pReading = nullptr;
	hr = mpGameInput->GetCurrentReading(GameInputKindKeyboard, nullptr, &pReading);
	if (SUCCEEDED(hr))
	{
		pReading->GetDevice(&mpKeyboard);
	}

	hr = mpGameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &pReading);
	if (SUCCEEDED(hr))
	{
		pReading->GetDevice(&mpGamepad);
	}
}

Input::~Input()
{
	SafeRelease(mpKeyboard);
	SafeRelease(mpGamepad);
	SafeRelease(mpGameInput);
}

void Input::PollGameInputs()
{
	IGameInputReading* pReading = nullptr;

	HRESULT hr = mpGameInput->GetCurrentReading(GameInputKindKeyboard, mpKeyboard, &pReading);
	if (SUCCEEDED(hr))
	{
		pReading->GetKeyState(MAX_KEY_ROLLOVER, mKeyStates);
		mKeyStateCount = pReading->GetKeyCount();
	}

	hr = mpGameInput->GetCurrentReading(GameInputKindGamepad, mpGamepad, &pReading);
	if (SUCCEEDED(hr))
	{
		pReading->GetGamepadState(&mGamepadState);
	}
}

void Input::OnMouseInput(const LPARAM lParam)
{
	mMouseX = GET_X_LPARAM(lParam);
	mMouseY = GET_Y_LPARAM(lParam);
}

void Input::OnLButtonEvent(const bool bClicked)
{
	mbLButton = bClicked;
}

void Input::OnRButtonEvent(const bool bClicked)
{
	mbRButton = bClicked;
}

void Input::OnVibratingController(const float duration)
{
	mbVibrating = true;

	mVibrationTimeLeft = duration;
}

void Input::VibrateController(const float frameTime)
{
	if (mpGamepad == nullptr || !mbVibrating)
	{
		return;
	}

	mVibrationTimeLeft -= frameTime;
	if (mVibrationTimeLeft < 0.f)
	{
		GameInputRumbleParams params;
		ZeroMemory(&params, sizeof(params));

		mpGamepad->SetRumbleState(&params);

		mVibrationTimeLeft = 0.f;
	}
}

bool Input::IsKeyPressed(const UCHAR vk) const
{
	for (int i = 0; i < mKeyStateCount; ++i)
	{
		if (mKeyStates[i].virtualKey == vk)
		{
			return true;
		}
	}

	return false;
}
