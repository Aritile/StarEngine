#pragma once

#include <d3d11.h>
#include <stdio.h>

class D3D11RasterizerState
{
public:
	D3D11RasterizerState() {}

public:
	static D3D11RasterizerState* Create();
	void Set();
	void Unset();
	ID3D11RasterizerState* Get();
	void Release();

private:
	ID3D11RasterizerState* rasterizerState = nullptr;
};