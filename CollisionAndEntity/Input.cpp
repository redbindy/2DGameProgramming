#include "Input.h"

Input::Input(const HWND hWnd, const bool bMouseCapture)
	: mhWnd(hWnd)
	, mpGameInput(nullptr)
	, mKeyStates{}
	, mKeyStateCount(0)
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

		DebugBreak();
	}
}

Input::~Input()
{
	SafeRelease(mpGameInput);
}

void Input::PollGameInputs()
{
	IGameInputReading* pReading = nullptr;

	HRESULT hr = mpGameInput->GetCurrentReading(GameInputKindKeyboard | GameInputKindGamepad, nullptr, &pReading);
	if (FAILED(hr))
	{
		return;
	}

	pReading->GetKeyState(MAX_KEY_ROLLOVER, mKeyStates);
	mKeyStateCount = pReading->GetKeyCount();

	pReading->GetGamepadState(&mGamepadState);

	pReading->Release();
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
	if (!mbVibrating)
	{
		return;
	}

	IGameInputReading* pReading = nullptr;
	HRESULT hr = mpGameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &pReading);
	if (FAILED(hr))
	{
		return;
	}

	IGameInputDevice* pGamepad = nullptr;
	pReading->GetDevice(&pGamepad);

	GameInputRumbleParams params;
	params.lowFrequency = 0.1f;
	params.highFrequency = 0.3f;
	params.leftTrigger = 0.2f;
	params.rightTrigger = 0.2f;

	mVibrationTimeLeft -= frameTime;
	if (mVibrationTimeLeft < 0.f)
	{
		ZeroMemory(&params, sizeof(params));

		mVibrationTimeLeft = 0.f;
		mbVibrating = false;
	}

	pGamepad->SetRumbleState(&params);

	pGamepad->Release();
	pReading->Release();
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
