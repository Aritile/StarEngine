#include "d3d11_context.h"
#include "../../DX/DX.h"

static DX* dx = DX::GetSingleton();

D3D11Context* D3D11Context::GetSingleton()
{
	static D3D11Context context;
	return &context;
}
void D3D11Context::SetViewport(UINT _Width, UINT _Height)
{
    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = (float)_Width;
    vp.Height = (float)_Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    dx->dxDeviceContext->RSSetViewports(1, &vp);
}
void D3D11Context::SetPrimitiveTopology(PrimitiveTopology _PrimitiveTopology)
{
    if (_PrimitiveTopology == PT_NONE)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
    else if (_PrimitiveTopology == PT_POINTLIST)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    else if (_PrimitiveTopology == PT_LINELIST)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    else if (_PrimitiveTopology == PT_LINESTRIP)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    else if (_PrimitiveTopology == PT_TRIANGLELIST)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    else if (_PrimitiveTopology == PT_TRIANGLESTRIP)
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    else printf("primitive topology is wrong\n");
}
void D3D11Context::SetRenderTarget()
{
    dx->dxDeviceContext->OMSetRenderTargets(1, &dx->dxRenderTargetView, dx->dxDepthStencilView);
}
void D3D11Context::UnsetRenderTarget()
{
    dx->dxDeviceContext->OMSetRenderTargets(0, NULL, NULL);
}
void D3D11Context::ClearRenderTarget(float _R, float _G, float _B, float _A)
{
    float clear_color[4] = { _R, _G, _B, _A };
    dx->dxDeviceContext->ClearRenderTargetView(dx->dxRenderTargetView, clear_color);
    dx->dxDeviceContext->ClearDepthStencilView(dx->dxDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void D3D11Context::Present(bool _Vsync)
{
    dx->dxSwapChain->Present(_Vsync ? 1 : 0, 0);
}
bool D3D11Context::CheckMultisampleQualityLevels(DXGI_FORMAT _Format, UINT _SampleCount, UINT* _QualityLevels)
{
    if (!dx->dxDevice)
    {
        printf("device is null\n");
        return false;
    }

    if (FAILED(dx->dxDevice->CheckMultisampleQualityLevels(_Format, _SampleCount, _QualityLevels)))
    {
        printf("check multisample quality levels failed\n");
        return false;
    }

    return true;
}
bool D3D11Context::SetVertexConstantBuffer(D3D11ConstantBuffer* _ConstantBuffer, UINT _Slot)
{
    if (!_ConstantBuffer)
    {
        printf("vertex constant buffer is null\n");
        return false;
    }

    dx->dxDeviceContext->VSSetConstantBuffers(_Slot, 1, _ConstantBuffer->Get().GetAddressOf());
    return true;
}
bool D3D11Context::SetPixelConstantBuffer(D3D11ConstantBuffer* _ConstantBuffer, UINT _Slot)
{
    if (!_ConstantBuffer)
    {
        printf("pixel constant buffer is null\n");
        return false;
    }

    dx->dxDeviceContext->PSSetConstantBuffers(_Slot, 1, _ConstantBuffer->Get().GetAddressOf());
    return true;
}
bool D3D11Context::SetVertexShaderResource(ID3D11ShaderResourceView* _ShaderResource, UINT _Slot)
{
    if (!_ShaderResource)
    {
        printf("vertex shader resource is null\n");
        return false;
    }

    dx->dxDeviceContext->VSSetShaderResources(_Slot, 1, &_ShaderResource);
    return true;
}
bool D3D11Context::SetPixelShaderResource(ID3D11ShaderResourceView* _ShaderResource, UINT _Slot)
{
    if (!_ShaderResource)
    {
        printf("pixel shader resource is null\n");
        return false;
    }

    dx->dxDeviceContext->PSSetShaderResources(_Slot, 1, &_ShaderResource);
    return true;
}
bool D3D11Context::SetVertexSampler(D3D11SamplerState* _SamplerState, UINT _Slot)
{
    if (!_SamplerState)
    {
        printf("vertex sampler state is null\n");
        return false;
    }

    dx->dxDeviceContext->VSSetSamplers(_Slot, 1, _SamplerState->Get().GetAddressOf());
    return true;
}
bool D3D11Context::SetPixelSampler(D3D11SamplerState* _SamplerState, UINT _Slot)
{
    if (!_SamplerState)
    {
        printf("vertex sampler state is null\n");
        return false;
    }

    dx->dxDeviceContext->PSSetSamplers(_Slot, 1, _SamplerState->Get().GetAddressOf());
    return true;
}
void D3D11Context::ReleaseVertexShaderResources(UINT _Slot)
{
    ID3D11ShaderResourceView* shaderResourceView = NULL;
    dx->dxDeviceContext->VSSetShaderResources(_Slot, 1, &shaderResourceView);
}
void D3D11Context::ReleasePixelShaderResources(UINT _Slot)
{
    ID3D11ShaderResourceView* shaderResourceView = NULL;
    dx->dxDeviceContext->PSSetShaderResources(_Slot, 1, &shaderResourceView);
}
bool D3D11Context::UpdateIndexBuffer(D3D11Shader* _Shader)
{
    // error check
    if (!_Shader)
    {
        printf("shader is null\n");
        return false;
    }
    else if (_Shader->GetIndices().empty())
    {
        printf("indices data is empty\n");
        return false;
    }
    else if (!_Shader->GetIndexBuffer())
    {
        printf("index buffer is null\n");
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE resource;
    if (FAILED(dx->dxDeviceContext->Map(_Shader->GetIndexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
        return false;

    memcpy(resource.pData, _Shader->GetIndices().data(), (sizeof(UINT) * (UINT)_Shader->GetIndices().size()));
    dx->dxDeviceContext->Unmap(_Shader->GetIndexBuffer().Get(), 0);

    _Shader->SetIndicesSize((UINT)_Shader->GetIndices().size());
    _Shader->ClearIndices();
    return true;
}
bool D3D11Context::Draw(D3D11Shader* _Shader)
{
    // error check
    if (!_Shader)
    {
        printf("shader is null\n");
        return false;
    }
    else if (!_Shader->GetVertexShader())
    {
        printf("vertex shader is null\n");
        return false;
    }
    else if (!_Shader->GetPixelShader())
    {
        printf("pixel shader is null\n");
        return false;
    }

    if (_Shader->GetIndexBuffer()) dx->dxDeviceContext->DrawIndexed(_Shader->GetIndicesSize(), 0, 0);
    else dx->dxDeviceContext->Draw(_Shader->GetVerticesSize(), 0);

    return true;
}
bool D3D11Context::Set(D3D11Shader* _Shader)
{
    // error check
    if (!_Shader)
    {
        printf("shader is null\n");
        return false;
    }
    else if (!_Shader->GetVertexLayout())
    {
        printf("vertex layout is null\n");
        return false;
    }
    else if (!_Shader->GetVertexBuffer())
    {
        printf("vertex buffer is null\n");
        return false;
    }
    else if (!_Shader->GetVertexShader())
    {
        printf("vertex shader is null\n");
        return false;
    }
    else if (!_Shader->GetPixelShader())
    {
        printf("pixel shader is null\n");
        return false;
    }

    dx->dxDeviceContext->IASetInputLayout(_Shader->GetVertexLayout().Get());

    UINT stride = _Shader->GetSizeOf();
    UINT offset = 0;
    dx->dxDeviceContext->IASetVertexBuffers(0, 1, _Shader->GetVertexBuffer().GetAddressOf(), &stride, &offset);

    if (_Shader->GetIndexBuffer())
        D3D11Context::GetSingleton()->GetDeviceContext()->IASetIndexBuffer(_Shader->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

    dx->dxDeviceContext->VSSetShader(_Shader->GetVertexShader().Get(), NULL, 0);
    dx->dxDeviceContext->PSSetShader(_Shader->GetPixelShader().Get(), NULL, 0);

    return true;
}
IDXGISwapChain* D3D11Context::GetSwapChain()
{
    return dx->dxSwapChain;
}
ID3D11Device* D3D11Context::GetDevice()
{
    return dx->dxDevice;
}
ID3D11DeviceContext* D3D11Context::GetDeviceContext()
{
    return dx->dxDeviceContext;
}
ID3D11RenderTargetView* D3D11Context::GetRenderTargetView()
{
    return dx->dxRenderTargetView;
}
ID3D11DepthStencilView* D3D11Context::GetDepthStencilView()
{
    return dx->dxDepthStencilView;
}