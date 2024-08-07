#include "DX.h"
#include "../STAR/Star.h"
#include "../WINDOW/MainWindow.h"

static MainWindow* mainWindow = MainWindow::GetSingleton();

DX* DX::GetSingleton()
{
	static DX dx;
	return &dx;
}

bool DX::CreateRenderTargetView()
{
    ID3D11Texture2D* tex = nullptr;
    if (FAILED(dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex)))
        return false;

    /*---*/

    if (FAILED(dxDevice->CreateRenderTargetView(tex, NULL, &dxRenderTargetView)))
        return false;

    /*---*/

    tex->Release();
    return true;
}
bool DX::CreateDepthStencilView(UINT _Width, UINT _Height)
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));

    descDepth.Width = _Width;
    descDepth.Height = _Height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* tex = nullptr;
    if (FAILED(dxDevice->CreateTexture2D(&descDepth, NULL, &tex)))
        return false;

    /*---*/

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if (FAILED(dxDevice->CreateDepthStencilView(tex, &descDSV, &dxDepthStencilView)))
        return false;

    /*---*/

    tex->Release();
    return true;
}
bool DX::ResizeBuffer(UINT _Width, UINT _Height)
{
    Star::AddLog("[Engine] -> Resizing SwapChain Buffer..");
    dxDeviceContext->OMSetRenderTargets(NULL, NULL, NULL);
    if (dxRenderTargetView) dxRenderTargetView->Release();
    if (dxDepthStencilView) dxDepthStencilView->Release();

    if (FAILED(dxSwapChain->ResizeBuffers(2, _Width, _Height, dxFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING)))
        return false;

    /*---*/

    if (!CreateRenderTargetView())
        return false;

    /*---*/

    if (!CreateDepthStencilView(_Width, _Height))
        return false;

    /*---*/

    return true;
}
bool DX::CreateContext(UINT _Width, UINT _Height)
{
    Star::AddLog("[SwapChain] -> Creating main SwapChain..");

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

    sd.BufferCount = 2;
    sd.BufferDesc.Width = _Width;
    sd.BufferDesc.Height = _Height;
    sd.BufferDesc.Format = dxFormat;
    //sd.BufferDesc.RefreshRate.Numerator = 60;
    //sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = mainWindow->GetHandle();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = dxSwapEffect;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    UINT flags = 0;

#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    /*---*/

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, dxDriverType, NULL, flags, NULL, NULL,
        D3D11_SDK_VERSION, &sd, &dxSwapChain, &dxDevice,
        &dxFeatureLevel, &dxDeviceContext)))
        return false;

    /*---*/

    Star::AddLog("[SwapChain] -> Creating render target view..");
    if (!CreateRenderTargetView())
        return false;

    /*---*/

    Star::AddLog("[SwapChain] -> Creating depth stencil view..");
    if (!CreateDepthStencilView(_Width, _Height))
        return false;

    /*---*/

    return true;
}
void DX::Release()
{
    if (dxSwapChain) dxSwapChain->Release();
    if (dxDevice) dxDevice->Release();
    if (dxDeviceContext) dxDeviceContext->Release();
    if (dxRenderTargetView) dxRenderTargetView->Release();
    if (dxDepthStencilView) dxDepthStencilView->Release();
}
void DX::UnbindAll(UINT StartSlot, UINT NumBuffers)
{
    // is this safe?

    ID3D11Buffer* buffer = nullptr;
    dxDeviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, &buffer);
    dxDeviceContext->PSSetConstantBuffers(StartSlot, NumBuffers, &buffer);
    dxDeviceContext->GSSetConstantBuffers(StartSlot, NumBuffers, &buffer);
    dxDeviceContext->CSSetConstantBuffers(StartSlot, NumBuffers, &buffer);

    ID3D11ShaderResourceView* shaderResourceView = nullptr;
    dxDeviceContext->VSSetShaderResources(StartSlot, NumBuffers, &shaderResourceView);
    dxDeviceContext->PSSetShaderResources(StartSlot, NumBuffers, &shaderResourceView);
    dxDeviceContext->GSSetShaderResources(StartSlot, NumBuffers, &shaderResourceView);
    dxDeviceContext->CSSetShaderResources(StartSlot, NumBuffers, &shaderResourceView);

    ID3D11SamplerState* samplerState = nullptr;
    dxDeviceContext->VSSetSamplers(StartSlot, NumBuffers, &samplerState);
    dxDeviceContext->PSSetSamplers(StartSlot, NumBuffers, &samplerState);
    dxDeviceContext->GSSetSamplers(StartSlot, NumBuffers, &samplerState);
    dxDeviceContext->CSSetSamplers(StartSlot, NumBuffers, &samplerState);

    ID3D11InputLayout* inputLayout = nullptr;
    dxDeviceContext->IASetInputLayout(inputLayout);

    UINT stride = 0;
    UINT offset = 0;
    dxDeviceContext->IASetVertexBuffers(StartSlot, NumBuffers, &buffer, &stride, &offset);
    dxDeviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_UNKNOWN, 0);

    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11GeometryShader* geometryShader = nullptr;
    ID3D11ComputeShader* computeShader = nullptr;
    dxDeviceContext->VSSetShader(vertexShader, NULL, 0);
    dxDeviceContext->PSSetShader(pixelShader, NULL, 0);
    dxDeviceContext->GSSetShader(geometryShader, NULL, 0);
    dxDeviceContext->CSSetShader(computeShader, NULL, 0);
}
void DX::ReportLiveObjects()
{
    if (dxDevice)
    {
        ID3D11Debug* pD3DDebug = nullptr;
        if (SUCCEEDED(dxDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&pD3DDebug)))
        {
            if (FAILED(pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)))
                Star::AddLog("ReportLiveDeviceObjects() Failed.");
            pD3DDebug->Release();
        }
    }
}