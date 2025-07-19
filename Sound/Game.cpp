#include "Game.h"

Game::Game(const HWND hWnd)
	: mpGraphics(nullptr)
	, mpInput(nullptr)
	, mhWnd(nullptr)
	, mTimeStart({ 0, })
	, mTimeEnd({ 0, })
	, mTimerFreq({ 0, })
	, mFps(0)
	, mFrameTime(0)
	, mbPaused(false)
	, mpXAudio(nullptr)
	, mpMasteringVoice(nullptr)
{
	std::cout << "Initialize Game" << std::endl;

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	mpGraphics = new Graphics(hWnd, clientRect.right, clientRect.bottom);

	mpInput = new Input(hWnd, false);

	const bool bTimerQueryResult = QueryPerformanceFrequency(&mTimerFreq);
	if (!bTimerQueryResult)
	{
		std::cerr << __LINE__ << ' ' << "QueryPerformanceFrequency" << std::endl;

		DebugBreak();
	}

	QueryPerformanceCounter(&mTimeStart);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		std::cerr << "CoInitializeEx" << std::endl;

		DebugBreak();
	}

	hr = XAudio2Create(&mpXAudio, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		std::cerr << "XAudio2Create" << std::endl;

		DebugBreak();
	}

	hr = mpXAudio->CreateMasteringVoice(&mpMasteringVoice);
	if (FAILED(hr))
	{
		std::cerr << "CreateMasteringVoice" << std::endl;
	}
}

Game::~Game()
{
	CoUninitialize();

	delete mpGraphics;
	delete mpInput;

	ShowCursor(true);
}

LRESULT Game::HandleWndMessage(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) const
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		mpInput->OnMouseInput(lParam);
		break;

	case WM_LBUTTONDOWN:
		mpInput->OnLButtonEvent(true);
		mpInput->OnMouseInput(lParam);
		break;

	case WM_LBUTTONUP:
		mpInput->OnLButtonEvent(false);
		mpInput->OnMouseInput(lParam);
		break;

	case WM_RBUTTONDOWN:
		mpInput->OnRButtonEvent(true);
		mpInput->OnMouseInput(lParam);
		break;

	case WM_RBUTTONUP:
		mpInput->OnRButtonEvent(false);
		mpInput->OnMouseInput(lParam);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

void Game::Run()
{
	QueryPerformanceCounter(&mTimeEnd);
	mFrameTime = static_cast<float>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<float>(mTimerFreq.QuadPart);

	if (mFrameTime < MIN_FRAME_TIME)
	{
		Sleep(0);

		return;
	}

	if (mFrameTime > 0.f)
	{
		mFps = (mFps * 0.99f) + (0.01f / mFrameTime);
	}
	else
	{
		mFrameTime = MAX_FRAME_TIME;
	}

	mTimeStart = mTimeEnd;

	if (!mbPaused)
	{
		mpInput->PollGameInputs();

		update();
		updateAI();
		handleCollision();

		mpInput->VibrateController(mFrameTime);
	}

	mpGraphics->BeginRendering();
	{
		render();
	}
	mpGraphics->EndRendering();
}
