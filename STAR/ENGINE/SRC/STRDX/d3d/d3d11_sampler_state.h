#pragma once

#include <d3d11.h>
#include <stdio.h>

class D3D11SamplerState
{
public:
	D3D11SamplerState() {}

public:
	static D3D11SamplerState* Create();
	ID3D11SamplerState* Get();
	void Release();

private:
	ID3D11SamplerState* samplerState = nullptr;
};