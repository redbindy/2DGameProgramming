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

	inline Graphics* GetGraphics() const;
	inline Input* GetInput() const;
	inline IXAudio2* GetXAudio() const;

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

	IXAudio2* mpXAudio;
	IXAudio2MasteringVoice* mpMasteringVoice;

protected:
	virtual void update() = 0;
	virtual void updateAI() = 0;
	virtual void handleCollision() = 0;
	virtual void render() = 0;

private:
};

inline Graphics* Game::GetGraphics() const
{
	return mpGraphics;
}

inline Input* Game::GetInput() const
{
	return mpInput;
}

inline IXAudio2* Game::GetXAudio() const
{
	return mpXAudio;
}