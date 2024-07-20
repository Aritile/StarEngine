#pragma once

#include <Windows.h>
#include <string>

class SplashScreen
{
public:
	bool ShowSplashScreen();
	void HideSplashScreen();

private:
	HWND hwnd = NULL;
	std::wstring name = L"StarSplashScreen";
	UINT width = 620;
	UINT height = 300;
	std::wstring path = L"data\\image\\splashscreen.bmp";
};

SplashScreen& SplashScreenClass();