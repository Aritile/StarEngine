#pragma once

#include <d3d11.h>
#include <stdio.h>

class D3D11RenderTarget
{
public:
	D3D11RenderTarget() {}

public:
	static D3D11RenderTarget* Create(UINT _Width, UINT _Height, UINT _Count);
	void Set();
	void ClearRenderTarget(float _R, float _G, float _B, float _A);
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	void Release();

private:
	bool CreateRenderTargetView(UINT _Width, UINT _Height, UINT _Count);
	bool CreateDepthStencilView(UINT _Width, UINT _Height, UINT _Count);

private:
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
};