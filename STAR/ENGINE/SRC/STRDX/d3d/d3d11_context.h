#pragma once

#include <d3d11.h>
#include "strdxwrl.h"
#include "d3d11_shader.h"
#include "../Topology.h"
#include <vector>

class D3D11Context
{
public:
	D3D11Context() {}

public:
	static D3D11Context* GetSingleton();

public:
	void SetViewport(UINT _Width, UINT _Height);
	void SetPrimitiveTopology(PrimitiveTopology _PrimitiveTopology);
	void SetRenderTarget();
	void UnsetRenderTarget();
	void ClearRenderTarget(float _R, float _G, float _B, float _A);
	void Present(bool _Vsync);
	bool CheckMultisampleQualityLevels(DXGI_FORMAT _Format, UINT _SampleCount, UINT* _QualityLevels);
	bool SetVertexConstantBuffer(D3D11ConstantBuffer* _ConstantBuffer, UINT _Slot);
	bool SetPixelConstantBuffer(D3D11ConstantBuffer* _ConstantBuffer, UINT _Slot);
	template <typename T>
	bool UpdateVertexBuffer(D3D11Shader* _Shader, std::vector<T>& _Vertices)
	{
		if (!_Shader)
		{
			printf("shader is null\n");
			return false;
		}

		if (_Vertices.empty())
		{
			printf("vertices data is empty\n");
			return false;
		}

		if (!_Shader->GetVertexBuffer())
		{
			printf("vertex buffer is null\n");
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		if (FAILED(GetDeviceContext()->Map(_Shader->GetVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
			return false;

		memcpy(resource.pData, _Vertices.data(), (sizeof(T) * _Vertices.size()));
		GetDeviceContext()->Unmap(_Shader->GetVertexBuffer().Get(), 0);

		return true;
	}
	bool SetVertexShaderResource(ID3D11ShaderResourceView* _ShaderResource, UINT _Slot);
	bool SetPixelShaderResource(ID3D11ShaderResourceView* _ShaderResource, UINT _Slot);
	bool SetVertexSampler(D3D11SamplerState* _SamplerState, UINT _Slot);
	bool SetPixelSampler(D3D11SamplerState* _SamplerState, UINT _Slot);
	void ReleaseVertexShaderResources(UINT _Slot);
	void ReleasePixelShaderResources(UINT _Slot);
	bool UpdateIndexBuffer(D3D11Shader* _Shader);
	bool Draw(D3D11Shader* _Shader);
	bool Set(D3D11Shader* _Shader);

public:
	IDXGISwapChain* GetSwapChain();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
};