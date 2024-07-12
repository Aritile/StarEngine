#include "context.h"

#include "constant_buffer.h"
#include "rasterizer_state.h"
#include "render_target.h"
#include "sampler_state.h"
#include "shader.h"

static D3D11Context* context = D3D11Context::GetSingleton();

Context* Context::GetSingleton()
{
	static Context context;
	return &context;
}

void Context::SetViewport(UINT _Width, UINT _Height)
{
#if defined(RENDERER_D3D11)
	context->SetViewport(_Width, _Height);
#endif
}
void Context::SetPrimitiveTopology(PrimitiveTopology _PrimitiveTopology)
{
#if defined(RENDERER_D3D11)
	context->SetPrimitiveTopology(_PrimitiveTopology);
#endif
}
void Context::SetRenderTarget()
{
#if defined(RENDERER_D3D11)
	context->SetRenderTarget();
#endif
}
void Context::UnsetRenderTarget()
{
#if defined(RENDERER_D3D11)
	context->UnsetRenderTarget();
#endif
}
void Context::ClearRenderTarget(float _R, float _G, float _B, float _A)
{
#if defined(RENDERER_D3D11)
	context->ClearRenderTarget(_R, _G, _B, _A);
#endif
}
void Context::Present(bool _Vsync)
{
#if defined(RENDERER_D3D11)
	context->Present(_Vsync);
#endif
}
bool Context::CheckMultisampleQualityLevels(DXGI_FORMAT _Format, UINT _SampleCount, UINT* _QualityLevels)
{
#if defined(RENDERER_D3D11)
	return context->CheckMultisampleQualityLevels(_Format, _SampleCount, _QualityLevels);
#endif

	return false;
}
bool Context::SetVertexConstantBuffer(ConstantBuffer* _ConstantBuffer, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_ConstantBuffer)
		return context->SetVertexConstantBuffer((D3D11ConstantBuffer*)_ConstantBuffer->Get(), _Slot);
#endif

	return false;
}
bool Context::SetPixelConstantBuffer(ConstantBuffer* _ConstantBuffer, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_ConstantBuffer)
		return context->SetPixelConstantBuffer((D3D11ConstantBuffer*)_ConstantBuffer->Get(), _Slot);
#endif

	return false;
}
bool Context::SetVertexShaderResource(ShaderResourceID* _ShaderResource, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_ShaderResource)
		return context->SetVertexShaderResource((ID3D11ShaderResourceView*)_ShaderResource, _Slot);
#endif

	return false;
}
bool Context::SetPixelShaderResource(ShaderResourceID* _ShaderResource, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_ShaderResource)
		return context->SetPixelShaderResource((ID3D11ShaderResourceView*)_ShaderResource, _Slot);
#endif

	return false;
}
bool Context::SetVertexSampler(SamplerState* _SamplerState, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_SamplerState)
		return context->SetVertexSampler((D3D11SamplerState*)_SamplerState->Get(), _Slot);
#endif

	return false;
}
bool Context::SetPixelSampler(SamplerState* _SamplerState, UINT _Slot)
{
#if defined(RENDERER_D3D11)
	if (_SamplerState)
		return context->SetPixelSampler((D3D11SamplerState*)_SamplerState->Get(), _Slot);
#endif

	return false;
}
void Context::ReleaseVertexShaderResources(UINT _Slot)
{
#if defined(RENDERER_D3D11)
	context->ReleaseVertexShaderResources(_Slot);
#endif
}
void Context::ReleasePixelShaderResources(UINT _Slot)
{
#if defined(RENDERER_D3D11)
	context->ReleasePixelShaderResources(_Slot);
#endif
}
bool Context::UpdateIndexBuffer(Shader* _Shader)
{
#if defined(RENDERER_D3D11)
	return context->UpdateIndexBuffer((D3D11Shader*)_Shader->Get());
#endif

	return false;
}
bool Context::Draw(Shader* _Shader)
{
#if defined(RENDERER_D3D11)
	if (_Shader)
		return context->Draw((D3D11Shader*)_Shader->Get());
#endif

	return false;
}
bool Context::Set(Shader* _Shader)
{
#if defined(RENDERER_D3D11)
	if (_Shader)
		return context->Set((D3D11Shader*)_Shader->Get());
#endif

	return false;
}