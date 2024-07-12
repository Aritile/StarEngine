#pragma once

#include <vector>
#include <d3d11.h>
#include <Windows.h>

// strdx
#include "strdx.h"
#include "Topology.h"
#include "d3d/d3d11_context.h"
#include "shader.h"

// classes
class ConstantBuffer;
class RasterizerState;
class RenderTarget;
class SamplerState;
class Shader;

class Context
{
public:
	Context() {}

public:
	static Context* GetSingleton();

public:
	void SetViewport(UINT _Width, UINT _Height);
	void SetPrimitiveTopology(PrimitiveTopology _PrimitiveTopology);
	void SetRenderTarget();
	void UnsetRenderTarget();
	void ClearRenderTarget(float _R, float _G, float _B, float _A);
	void Present(bool _Vsync);
	bool CheckMultisampleQualityLevels(DXGI_FORMAT _Format, UINT _SampleCount, UINT* _QualityLevels);
	bool SetVertexConstantBuffer(ConstantBuffer* _ConstantBuffer, UINT _Slot);
	bool SetPixelConstantBuffer(ConstantBuffer* _ConstantBuffer, UINT _Slot);
	template <typename T>
	bool UpdateVertexBuffer(Shader* _Shader, std::vector<T>& _Vertices)
	{
#if defined(RENDERER_D3D11)
		D3D11Context* context = D3D11Context::GetSingleton();

		if (_Shader)
			return context->UpdateVertexBuffer<T>((D3D11Shader*)_Shader->Get(), _Vertices);
#endif

		return false;
	}
	bool SetVertexShaderResource(ShaderResourceID* _ShaderResource, UINT _Slot);
	bool SetPixelShaderResource(ShaderResourceID* _ShaderResource, UINT _Slot);
	bool SetVertexSampler(SamplerState* _SamplerState, UINT _Slot);
	bool SetPixelSampler(SamplerState* _SamplerState, UINT _Slot);
	void ReleaseVertexShaderResources(UINT _Slot);
	void ReleasePixelShaderResources(UINT _Slot);
	bool UpdateIndexBuffer(Shader* _Shader);
	bool Draw(Shader* _Shader);
	bool Set(Shader* _Shader);
};