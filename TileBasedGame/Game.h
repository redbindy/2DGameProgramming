#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <xaudio2.h>

#include <cctype>
#include <tchar.h>

#include "Graphics.h"
#include "Input.h"
#include "constants.h"
#include "TextSprite.h"
#include "TextDX.h"
#include "Console.h"
#include "MyMessageBox.h"

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
	bool mbFpsDisplayOn;

	float mFrameTime;

	bool mbPaused;

	IXAudio2* mpXAudio;
	IXAudio2MasteringVoice* mpMasteringVoice;

	TextSprite* mpText;
	TextDX* mpTextDX;

	Console* mpConsole;

	MyMessageBox* mpMessageBox;

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