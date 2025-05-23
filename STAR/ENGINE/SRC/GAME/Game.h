#pragma once

#include <string>
#include <d3d11.h>
#include <Windows.h>
#include "../IMGUI/imgui.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

enum GameState : unsigned char
{
	GameNone = 0x00,
	GamePlay = 0x01,
	GameStop = 0x02,
};

class Game
{
private:
	std::wstring name = L"StarGame";
	int width = 1280; /* 1920 */
	int height = 720; /* 800 */
	//int width = 1600; /* 1920 */
	//int height = 900; /* 800 */
	//UINT antialiasing = 8;

private:
	HWND hwnd = NULL;
public:
	IDXGISwapChain* gameSwapChain = NULL;
private:
	ID3D11RenderTargetView* gameRenderTargetView = NULL;
	ID3D11DepthStencilView* gameDepthStencilView = NULL;

public:
	bool InitGame(bool _ShowWindow, HWND parent);
	bool StartGame();
	void StopGame();
	unsigned char GetGameState();
	bool GameResizeBuffer();
	HWND GetHandle();

public:
	D3D11_VIEWPORT GetViewport();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	IDXGISwapChain* GetSwapChain();

private:
	bool GameCreateWindow(bool _ShowWindow);
	bool GameCreateContext(HWND parent);
	bool GameCreateRenderTargetView();
	bool GameCreateDepthStencilView();

public:
	UINT GameGetContextWidth();
	UINT GameGetContextHeight();
	UINT GameGetWindowWidth();
	UINT GameGetWindowHeight();

private:
	unsigned char gameState = GameState::GameNone;

	/**** INPUT ****/
	bool isCursorLocked = false;

private:
	LPDIRECTINPUT8 gDirectInput;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	bool InitInput();
	void ReleaseInput();

public:
	bool GetKeyDown(unsigned char key);
	Vector2 GetCursorAxis();

	/**** TIME ****/
private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	float frameTime, deltaTime, elapsedTime;
	unsigned int frameCount;

	bool timeState = false;

public:
	void StartTime();
	void StopTime();
	void ResetTime();

public:
	void InitTime();
	void BeginTime();
	void EndTime();

public:
	float GetFrameTime();
	float GetDeltaTime();
	float GetElapsedTime();
	unsigned int GetFrameCount();

	void LockCursor(bool _Lock);
	bool IsCursorLocked();

public:
	void SetWindowState(unsigned int state);

public:
	static Game* GetSingleton();

	bool focus = false;
	bool lastIsCursorShowed = false;
	bool lastIsCursorLocked = false;
};