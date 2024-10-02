#include "Sky.h"
#include <fstream>
#include "../DX/DX.h"
#include "../EDITOR/Editor.h"
#include "../MODEL/AssimpLoader.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../EDITOR/WINDOW/Viewport.h"
#include "../JOB/Job.h"

Sky* Sky::GetSingleton()
{
	static Sky sky;
	return &sky;
}

static DX* dx = DX::GetSingleton();
static AssimpLoader* assimpLoader = AssimpLoader::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();
static Job* job = Job::GetSingleton();

struct ConstantBuffer
{
	DirectX::XMMATRIX WVP = DirectX::XMMatrixIdentity();
	float exposure = 1.0f;
};

static ConstantBuffer cb;

bool Sky::Init()
{
	if (FAILED(Star::CompileShaderFromFile(L"data\\shader\\sky\\vertex.hlsl", "main", VS_VERSION, &VS)))
		return false;
	if (FAILED(Star::CompileShaderFromFile(L"data\\shader\\sky\\pixel.hlsl", "main", PS_VERSION, &PS)))
		return false;
	if (FAILED(dx->dxDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &pVS)))
		return false;
	if (FAILED(dx->dxDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &pPS)))
		return false;

	D3D11_INPUT_ELEMENT_DESC idx[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (UINT)IM_OFFSETOF(Vertex, textureCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(dx->dxDevice->CreateInputLayout(idx, ARRAYSIZE(idx), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout)))
		return false;

	VS->Release();
	PS->Release();

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ConstantBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		if (FAILED(dx->dxDevice->CreateBuffer(&desc, nullptr, &pConstantBuffer)))
			return false;
	}

	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(dx->dxDevice->CreateRasterizerState(&desc, &pCullNone)))
			return false;
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		if (FAILED(dx->dxDevice->CreateDepthStencilState(&desc, &pLessEqual)))
			return false;
	}

	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		if (FAILED(dx->dxDevice->CreateSamplerState(&samplerDesc, &pSamplerState)))
			return false;
	}

	meshStorage->LoadModel("data\\model\\sphere.obj", &modelStorageBuffer);
	modelStorageBuffer->LoadMesh(0, &meshStorageBuffer);
	return true;
}

void Sky::Render(DirectX::XMMATRIX _View, DirectX::XMMATRIX _Projection)
{
	if (!type == SkyNone)
	{
		DirectX::XMMATRIX skyMatrix = DirectX::XMMatrixIdentity() * _View * _Projection;
		cb.WVP = skyMatrix;
		cb.exposure = exposure;

		dx->dxDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
		dx->dxDeviceContext->VSSetShader(pVS, 0, 0);
		dx->dxDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer); // for WVP
		dx->dxDeviceContext->PSSetShader(pPS, 0, 0);
		dx->dxDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer); // for mode
		dx->dxDeviceContext->IASetInputLayout(pLayout);
		dx->dxDeviceContext->RSSetState(pCullNone);
		dx->dxDeviceContext->OMSetDepthStencilState(pLessEqual, 0);
		dx->dxDeviceContext->PSSetSamplers(0, 1, &pSamplerState);

		if (type == SkyTexture)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			{
				dx->dxDeviceContext->IASetVertexBuffers(0, 1, &meshStorageBuffer->vertexBuffer, &stride, &offset);
				dx->dxDeviceContext->IASetIndexBuffer(meshStorageBuffer->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				dx->dxDeviceContext->PSSetShaderResources(0, 1, &texture);
				dx->dxDeviceContext->DrawIndexed((UINT)meshStorageBuffer->indices.size(), 0, 0);
			}
			viewportWindow->RefreshRenderState();
		}

		dx->UnbindAll(0, 1);
	}
}

void Sky::Release()
{
	if (pVS)             pVS->Release();
	if (pPS)             pPS->Release();
	if (pLayout)         pLayout->Release();
	if (pConstantBuffer) pConstantBuffer->Release();
	if (pCullNone)       pCullNone->Release();
	if (pLessEqual)      pLessEqual->Release();
	if (pSamplerState)   pSamplerState->Release();
	if (texture)         texture->Release();
}
void Sky::SetNone()
{
	type = SkyNone;
}
void Sky::SetSolidColor(Vector3 _SolidColor)
{
	type = SkySolidColor;
	solidColor = _SolidColor;
}
void Sky::LoadTexture(const char* _Path)
{
	type = SkyTexture;

	if (Star::GetFileExtensionFromPath(_Path).compare(HDR) == 0)
	{
		Star::AddLog("[Sky] -> Loading texture.. %s", _Path);

		job->CreateJob<ID3D11ShaderResourceView*>(
			[_Path]() -> ID3D11ShaderResourceView*
			{
				ID3D11ShaderResourceView* texture = nullptr;
				DirectX::ScratchImage scratchImage;
				DirectX::TexMetadata texMetadata;

				// load the hdr file
				if (FAILED(DirectX::LoadFromHDRFile(Star::ConvertString(_Path).c_str(), &texMetadata, scratchImage)))
				{
					Star::AddLog("[Sky] -> Failed to load texture.");
					return nullptr;
				}

				// create the shaderResourceView
				if (FAILED(DirectX::CreateShaderResourceView(
					dx->dxDevice, scratchImage.GetImages(), scratchImage.GetImageCount(), texMetadata, &texture)))
				{
					Star::AddLog("[Sky] -> Failed to create shader resource view.");
					return nullptr;
				}

				scratchImage.Release();
				return texture;
			},
			[this](ID3D11ShaderResourceView* _Texture)
			{
				if (_Texture)
				{
					this->texture = _Texture;
					Star::AddLog("[Sky] -> Texture successfully loaded.");
				}
			}
		);
	}
}
void Sky::ReleaseTexture()
{
	if (texture)
		texture->Release();
	texture = nullptr;
}
SkyType Sky::GetType()
{
	return type;
}
Vector3 Sky::GetSolidColor()
{
	return solidColor;
}