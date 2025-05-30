#pragma once

#include "d3d/d3d11_constant_buffer.h"
#include "strdx.h"

class ConstantBuffer
{
public:
	ConstantBuffer() : d3d11_constant_buffer(NULL) {}

public:
	static ConstantBuffer* Create(UINT _SizeOf);
	bool Update(const void* _ConstantBuffer)
	{
#if defined(RENDERER_D3D11)
		if (d3d11_constant_buffer)
			return d3d11_constant_buffer->Update(_ConstantBuffer);
#endif

		return false;
	}
	ConstantBufferID* Get();
	void Release();

private:
	D3D11ConstantBuffer* d3d11_constant_buffer;
};