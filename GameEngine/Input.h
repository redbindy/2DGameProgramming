#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <GameInput.h>

#include <cassert>
#include <iostream>

#include "Constants.h"

enum
{
	MAX_KEY_ROLLOVER = 16
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

	void OnVibratingController(const float duration);
	void VibrateController(const float frameTime);

	bool IsKeyPressed(const UCHAR vk) const;

private:
	HWND mhWnd;

	IGameInput* mpGameInput;
	
	IGameInputDevice* mpKeyboard;
	GameInputKeyState mKeyStates[MAX_KEY_ROLLOVER];
	int mKeyStateCount;

	IGameInputDevice* mpGamepad;
	GameInputGamepadState mGamepadState;
	bool mbVibrating;
	float mVibrationTimeLeft;

	int mMouseX;
	int mMouseY;

	bool mbMouseCaptured;
	bool mbLButton;
	bool mbRButton;
};