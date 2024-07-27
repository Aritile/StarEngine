#pragma once

#include <string>
#include <dwmapi.h>
#include <Windows.h>
#include "../DX/DX.h"
#include "../SKY/Sky.h"
#include "../EDITOR/Editor.h"
#include "../HELPERS/Helpers.h"

void DX11SetReference(HINSTANCE& hInstance,
					  HINSTANCE& hPrevInstance,
	                  PWSTR& pCmdLine,
	                  int& nCmdShow);
bool DX11CreateWindow(std::wstring _Name,
	                  int _Width,
	                  int _Height);
LRESULT CALLBACK DX11WindowProc(HWND hwnd,
	                            UINT uMsg,
	                            WPARAM wParam,
	                            LPARAM lParam);
bool DX11CreateContext();
bool DX11CreateRenderTargetView();
bool DX11CreateDepthStencilView();
bool DX11ResizeBuffer();
void SetRenderTarget(Vector4 _Color);
void EngineStart();
void EngineUpdate();
void EngineProcess();
void EngineShutdown();
void UpdateTransform(entt::entity _Entity);
void GamePlayUpdate();
void GamePlayLateUpdate();
void GamePlayFixedUpdate();
void RenderToMainBuffer();
void RenderEnvironment(Matrix _ProjectionMatrix,
	                   Matrix _ViewMatrix,
	                   Vector4 _Color,
	                   ID3D11RenderTargetView* _RenderTargetView,
	                   ID3D11DepthStencilView* _DepthStencilView,
	                   D3D11_VIEWPORT _Viewport,
	                   IDXGISwapChain* _SwapChain,
	                   bool game);
bool FindGoodCamera(Matrix& _ProjectionMatrix,
	                Matrix& _ViewMatrix);