#include <iostream>
#include <cassert>

#include <Windows.h>
#include <GameInput.h>

#include <thread>

#pragma comment(lib, "GameInput.lib")
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

inline float GetDeltaTime(const LARGE_INTEGER& start, const LARGE_INTEGER& end, const LARGE_INTEGER& timerFreq)
{
	const float frameTime = static_cast<float>(end.QuadPart - start.QuadPart) / static_cast<float>(timerFreq.QuadPart);

	return frameTime;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow
)
{
	const HANDLE mutex = CreateMutex(
		nullptr,
		true,
		TEXT("Use different string here for each program")
	);

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
	wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));

	const WCHAR* CLASS_NAME = TEXT("Hello World");

	wcx.lpszClassName = CLASS_NAME;

	if (RegisterClassEx(&wcx) == 0)
	{
		return false;
	}

	HWND hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<HWND>(nullptr),
		static_cast<HMENU>(nullptr),
		hInstance,
		nullptr
	);

	if (hWnd == nullptr)
	{
		return false;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	IGameInput* pGameInput = nullptr;
	const HRESULT hr = GameInputCreate(&pGameInput);
	assert(SUCCEEDED(hr));

	LARGE_INTEGER timerFreq;
	QueryPerformanceFrequency(&timerFreq);

	LARGE_INTEGER timeStart;
	QueryPerformanceCounter(&timeStart);

	IGameInputDevice* pKeyboard = nullptr;
	IGameInputDevice* pMouse = nullptr;
	IGameInputDevice* pGamepad = nullptr;

	LARGE_INTEGER vibrationStart = { 0, };
	bool bVibrating = false;

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

		LARGE_INTEGER timeEnd;
		QueryPerformanceCounter(&timeEnd);

		const float frameTime = GetDeltaTime(timeStart, timeEnd, timerFreq);
		if (frameTime < 0.f)
		{
			continue;
		}

		timeStart = timeEnd;

		IGameInputReading* pReading = nullptr;

		// 키보드 입력 처리
		HRESULT hr = pGameInput->GetCurrentReading(
			GameInputKindKeyboard,
			pKeyboard, // nullptr 대입시 모든 디바이스
			&pReading
		);

		if (SUCCEEDED(hr))
		{
			if (pKeyboard == nullptr)
			{
				pReading->GetDevice(&pKeyboard);
			}

			constexpr int SIZE = 16;

			GameInputKeyState keyStates[SIZE];
			ZeroMemory(keyStates, sizeof(keyStates));

			pReading->GetKeyState(SIZE, keyStates);
			const int count = pReading->GetKeyCount();

			pReading->Release();

			// 맵 인덱싱 방식이 아닌 앞에서부터 채워주는 방식이므로 검사
			for (int i = 0; i < count; ++i)
			{
				const GameInputKeyState& ks = keyStates[i];

				switch (ks.virtualKey)
				{
				case VK_RIGHT:
					std::cout << "right" << std::endl;
					break;

				case VK_LEFT:
					std::cout << "left" << std::endl;
					break;

				case VK_UP:
					std::cout << "up" << std::endl;
					break;

				case VK_DOWN:
					std::cout << "down" << std::endl;
					break;

				case 'A':
					std::cout << 'A' << std::endl;
					break;

				case 'S':
					std::cout << 'S' << std::endl;
					break;

				case 'D':
					std::cout << 'D' << std::endl;
					break;

				case 'W':
					std::cout << 'W' << std::endl;
					break;

				default:
					break;
				}
			}

		}

		// 마우스
		hr = pGameInput->GetCurrentReading(GameInputKindMouse, pMouse, &pReading);
		if (SUCCEEDED(hr))
		{
			if (pMouse == nullptr)
			{
				pReading->GetDevice(&pMouse);
			}

			GameInputMouseState state;
			pReading->GetMouseState(&state);

			// 델타값
			// 절대 좌표는 안 줌
			std::cout << state.positionX << ' ' << state.positionY << std::endl;
			switch (state.buttons)
			{
			case GameInputMouseLeftButton:
				std::cout << "LButton" << std::endl;
				break;

			case GameInputMouseRightButton:
				std::cout << "RButton" << std::endl;
				break;

			default:
				break;
			}
		}

		// 게임 패드
		hr = pGameInput->GetCurrentReading(GameInputKindGamepad, pGamepad, &pReading);
		if (SUCCEEDED(hr))
		{
			if (pGamepad == nullptr)
			{
				pReading->GetDevice(&pGamepad);
			}

			GameInputGamepadState state;
			pReading->GetGamepadState(&state);
			pReading->Release();

			switch (state.buttons)
			{
			case GameInputGamepadA:
				std::cout << 'A' << std::endl;
				break;

			case GameInputGamepadB:
				std::cout << 'B' << std::endl;
				break;

			case GameInputGamepadX:
				std::cout << 'X' << std::endl;
				break;

			case GameInputGamepadY:
				std::cout << 'Y' << std::endl;
				break;

			default:
				break;
			}

			// 기울어진 정도를 [-1, 1]로 표현
			std::cout << state.leftThumbstickX << std::endl;

			// 눌린 강도를 [0, 1]로 표현
			std::cout << state.rightTrigger << std::endl;

			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);

			const float term = GetDeltaTime(vibrationStart, currentTime, timerFreq);

			if (term > 1.f)
			{
				GameInputRumbleParams params;

				if (bVibrating)
				{
					ZeroMemory(&params, sizeof(params));
				}
				else
				{
					params.lowFrequency = 0.f;
					params.highFrequency = 0.1f;
					params.leftTrigger = 0.1f;
					params.rightTrigger = 0.1f;
				}

				bVibrating = !bVibrating;

				pGamepad->SetRumbleState(&params);

				vibrationStart = currentTime;
			}
		}
	}

	pGamepad->Release();
	pMouse->Release();
	pKeyboard->Release();
	pGameInput->Release();

	return static_cast<int>(msg.wParam);
}

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC = nullptr;

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		{

		}
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
