#pragma once

#include <d3d11.h>
#include <Windows.h>

class DX
{
public:
	DXGI_FORMAT dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D_DRIVER_TYPE dxDriverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL dxFeatureLevel = D3D_FEATURE_LEVEL_11_1;
	DXGI_SWAP_EFFECT dxSwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
public:
	IDXGISwapChain* dxSwapChain = NULL;
	ID3D11Device* dxDevice = NULL;
	ID3D11DeviceContext* dxDeviceContext = NULL;
	ID3D11RenderTargetView* dxRenderTargetView = NULL;
	ID3D11DepthStencilView* dxDepthStencilView = NULL;
	ID3D11BlendState* dxBlendState = NULL;

public:
	static DX* GetSingleton();

public:
	bool CreateContext(UINT _Width, UINT _Height);
	bool CreateRenderTargetView();
	bool CreateDepthStencilView(UINT _Width, UINT _Height);
	bool ResizeBuffer(UINT _Width, UINT _Height);
	void Release();
	void UnbindAll(UINT StartSlot, UINT NumBuffers);
	void ReportLiveObjects();
	void CreateBlendState();
};