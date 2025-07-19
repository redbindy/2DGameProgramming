#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	}

	return static_cast<int>(msg.wParam);
}

static bool sVKKeys[256];

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TCHAR sCh = ' ';
	static TEXTMETRIC sTm;
	HDC hDC = nullptr;

	constexpr int SHIFTED = 0x8000;

	WPARAM nVirtKey;

	switch (uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		{
			GetTextMetrics(hDC, &sTm);
		}
		ReleaseDC(hWnd, hDC);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case VK_BACK:
		case VK_TAB:
		case VK_RETURN:
		case VK_ESCAPE:
			MessageBeep(static_cast<UINT>(-1));
			break;

		default:
			sCh = static_cast<TCHAR>(wParam);
			InvalidateRect(hWnd, nullptr, true);
			break;
		}
		break;

	case WM_KEYDOWN:
		sVKKeys[wParam] = true;
		switch (wParam)
		{
		case VK_SHIFT:
			nVirtKey = GetKeyState(VK_LSHIFT);
			if (nVirtKey & SHIFTED)
			{
				sVKKeys[VK_LSHIFT] = true;
			}

			nVirtKey = GetKeyState(VK_RSHIFT);
			if (nVirtKey & SHIFTED)
			{
				sVKKeys[VK_RSHIFT] = true;
			}
			break;

		case VK_CONTROL:
			nVirtKey = GetKeyState(VK_LCONTROL);
			if (nVirtKey & SHIFTED)
			{
				sVKKeys[VK_LCONTROL] = true;
			}

			nVirtKey = GetKeyState(VK_RCONTROL);
			if (nVirtKey & SHIFTED)
			{
				sVKKeys[VK_RCONTROL] = true;
			}
			break;

		default:
			break;
		}
		InvalidateRect(hWnd, nullptr, true);
		break;

	case WM_KEYUP:
		sVKKeys[wParam] = false;
		switch (wParam)
		{
		case VK_SHIFT:
			nVirtKey = GetKeyState(VK_LSHIFT);
			if (!(nVirtKey & SHIFTED))
			{
				sVKKeys[VK_LSHIFT] = false;
			}

			nVirtKey = GetKeyState(VK_RSHIFT);
			if (!(nVirtKey & SHIFTED))
			{
				sVKKeys[VK_RSHIFT] = false;
			}
			break;

		case VK_CONTROL:
			nVirtKey = GetKeyState(VK_LCONTROL);
			if (!(nVirtKey & SHIFTED))
			{
				sVKKeys[VK_LCONTROL] = false;
			}

			nVirtKey = GetKeyState(VK_RCONTROL);
			if (!(nVirtKey & SHIFTED))
			{
				sVKKeys[VK_RCONTROL] = false;
			}
			break;

		default:
			break;
		}
		InvalidateRect(hWnd, nullptr, true);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		{
			RECT rect;
			GetClientRect(hWnd, &rect);

			TextOut(hDC, 0, 0, &sCh, 1);

			for (int r = 0; r < 16; ++r)
			{
				const int rowIndex = r * 16;
				for (int c = 0; c < 16; ++c)
				{
					const int index = rowIndex + c;

					const int x = c * sTm.tmAveCharWidth + sTm.tmAveCharWidth * 2;
					const int y = r * sTm.tmHeight + sTm.tmHeight * 2;

					const TCHAR* pOutputStr = nullptr;
					if (sVKKeys[index])
					{
						SetBkMode(hDC, OPAQUE);
						pOutputStr = TEXT("T ");
					}
					else
					{
						SetBkMode(hDC, TRANSPARENT);
						pOutputStr = TEXT("F ");
					}

					TextOut(hDC, x, y, pOutputStr, 2);
				}
			}
		}
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
