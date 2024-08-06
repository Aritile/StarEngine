#pragma once

#include <string>
#include <Windows.h>

class MainWindow
{
public:
	static MainWindow* GetSingleton();

public:
	bool CreateMainWindow(std::wstring _Name,
		int _Width,
		int _Height);
	static LRESULT CALLBACK WindowProc(HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
	RECT GetClientRect();
	UINT GetContextWidth();
	UINT GetContextHeight();
	HWND GetHandle();

private:
	HWND hwnd = NULL;
};