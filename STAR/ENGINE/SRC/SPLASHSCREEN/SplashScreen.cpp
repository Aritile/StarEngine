#include "SplashScreen.h"
#include "../DX/DX.h"
#include "../STAR/Star.h"
#include "../ENGINE/Engine.h"

static Engine* engine = Engine::GetSingleton();
static DX* dx = DX::GetSingleton();

SplashScreen* SplashScreen::GetSingleton()
{
	static SplashScreen splashScreen;
	return &splashScreen;
}

bool SplashScreen::ShowSplashScreen()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = DefWindowProc;
	wcex.cbClsExtra = NULL;
	wcex.cbWndExtra = NULL;
	wcex.hInstance = engine->hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = name.c_str();
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
		return false;

	int x = (Star::GetDisplayWidth() - width) / 2;
	int y = (Star::GetDisplayHeight() - height) / 2;

	hwnd = CreateWindowEx(
		WS_EX_TOPMOST,
		name.c_str(),
		name.c_str(),
		WS_POPUP,
		x,
		y,
		width,
		height,
		NULL,
		NULL,
		engine->hInstance,
		NULL);

	if (!hwnd)
		return false;

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

	ShowWindow(hwnd, SW_SHOW);

	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));

	GetClientRect(hwnd, &rect);
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	HDC hdc = GetDC(hwnd);
	HBRUSH brush = CreatePatternBrush((HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, width, height, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	FillRect(hdc, &rect, brush);

	{
		DeleteObject(brush);
		ReleaseDC(hwnd, hdc);
	}

	return true;
}

void SplashScreen::HideSplashScreen()
{
	DestroyWindow(hwnd);
	UnregisterClass(name.c_str(), engine->hInstance);
}