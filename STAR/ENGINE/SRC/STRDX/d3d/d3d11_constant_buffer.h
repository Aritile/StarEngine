#pragma once

#include <d3d11.h>
#include <stdio.h>

class D3D11ConstantBuffer
{
public:
	D3D11ConstantBuffer() {}

public:
	static D3D11ConstantBuffer* Create(UINT _SizeOf);
	bool Update(const void* _ConstantBuffer);
	ID3D11Buffer* Get();
	void Release();

private:
	ID3D11Buffer* constant_buffer = nullptr;
};