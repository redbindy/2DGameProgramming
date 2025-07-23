#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <GameInput.h>
#pragma comment(lib, "GameInput.lib")

#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <cassert>
#include <iostream>

#include "Constants.h"

enum EInputConstant
{
	MAX_KEY_ROLLOVER = 16, 
	MAX_STR_LEN = 64
};

class Input final
{
public:
	Input(const HWND hWnd, const bool bMouseCapture);
	virtual ~Input();

	void PollGameInputs();

	void OnMouseInput(const LPARAM lParam);
	void OnLButtonEvent(const bool bClicked);
	void OnRButtonEvent(const bool bClicked);

	inline int GetMouseX() const;
	inline int GetMouseY() const;
	inline bool IsLButtonClicked() const;
	inline bool IsRButtonClicked() const;

	inline GameInputGamepadState GetGamepadState() const;
	void OnVibratingController(const float duration);
	void VibrateController(const float frameTime);

	void OnCharIn(const WPARAM ch);
	bool IsKeyPressed(const UCHAR vk) const;

	void GetInputStr(TCHAR* pOutputBuffer) const;
	void ClearStr();

private:
	HWND mhWnd;

	IGameInput* mpGameInput;

	GameInputKeyState mKeyStates[MAX_KEY_ROLLOVER];
	int mKeyStateCount;

	GameInputGamepadState mGamepadState;
	bool mbVibrating;
	float mVibrationTimeLeft;

	int mMouseX;
	int mMouseY;

	bool mbMouseCaptured;
	bool mbLButton;
	bool mbRButton;

	TCHAR mStrBuffer[MAX_STR_LEN + 1];
	int mStrSize;
};

inline GameInputGamepadState Input::GetGamepadState() const
{
	return mGamepadState;
}

inline int Input::GetMouseX() const
{
	return mMouseX;
}

inline int Input::GetMouseY() const
{
	return mMouseY;
}

inline bool Input::IsLButtonClicked() const
{
	return mbLButton;
}

inline bool Input::IsRButtonClicked() const
{
	return mbRButton;
}
