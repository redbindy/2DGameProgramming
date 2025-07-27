#define WIN32_LEAN_AND_MEAN
#define _CRTDBG_MAP_ALLOC

#include <Windows.h>
#include <crtdbg.h>

#include "Graphics.h"
#include "Game.h"
#include "MyGame.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const TCHAR* CLASS_NAME = TEXT("Game Engine");

Game* gpGame = nullptr;

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow
)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	/*const HANDLE mutex = CreateMutex(
		nullptr,
		true,
		CLASS_NAME
	);*/

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return false;
	}

	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(wcx));

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WndProc;
	wcx.hInstance = hInstance;
	wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcx.lpszClassName = CLASS_NAME;
	wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);

	if (RegisterClassEx(&wcx) == 0)
	{
		return 0;
	}

	RECT rect = { 0, 0, 1024, 1024 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	const HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (hWnd == nullptr)
	{
		return false;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	gpGame = new MyGame(hWnd);

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gpGame->Run();
		}
	}

	delete gpGame;

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (gpGame == nullptr)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return gpGame->HandleWndMessage(hWnd, uMsg, wParam, lParam);
}