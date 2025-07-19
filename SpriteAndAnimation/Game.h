#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <xaudio2.h>

#include <cctype>

#include "Graphics.h"
#include "Input.h"
#include "constants.h"

class Game
{
public:
	Game(const HWND hWnd);
	virtual ~Game();
	
	LRESULT HandleWndMessage(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) const;

	virtual void Run();

protected:
	Graphics* mpGraphics;
	Input* mpInput;
	HWND mhWnd;

	LARGE_INTEGER mTimeStart;
	LARGE_INTEGER mTimeEnd;
	LARGE_INTEGER mTimerFreq;

	float mFps;
	float mFrameTime;

	bool mbPaused;

	virtual void update() = 0;
	virtual void updateAI() = 0;
	virtual void handleCollision() = 0;
	virtual void render() = 0;

private:
};