#include "d3d11_rasterizer_state.h"
#include "d3d11_context.h"

static D3D11Context* context = D3D11Context::GetSingleton();

D3D11RasterizerState* D3D11RasterizerState::Create()
{
    D3D11RasterizerState* rasterizerState = new D3D11RasterizerState();

    D3D11_RASTERIZER_DESC  desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_NONE;
    desc.FrontCounterClockwise = false;
    desc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
    desc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
    desc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = true;
    desc.AntialiasedLineEnable = true;

    if (FAILED(context->GetDevice()->CreateRasterizerState(&desc, &rasterizerState->rasterizerState)))
        printf("create rasterizer state failed\n");

    return rasterizerState;
}
void D3D11RasterizerState::Set()
{
    context->GetDeviceContext()->RSSetState(rasterizerState);
}
void D3D11RasterizerState::Unset()
{
    context->GetDeviceContext()->RSSetState(NULL);
}
ID3D11RasterizerState* D3D11RasterizerState::Get()
{
    return rasterizerState;
}
void D3D11RasterizerState::Release()
{
    if (rasterizerState) rasterizerState->Release();
    delete this;
}