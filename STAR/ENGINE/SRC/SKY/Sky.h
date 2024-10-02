#pragma once

#include "../MODEL/AssimpLoader.h"
#include "../XTK/TEX/DirectXTex.h"
#include <mutex>

using namespace DirectX::SimpleMath;

enum SkyType
{
	SkyNone,
	SkySolidColor,
	SkyTexture
};

class Sky
{
public:
	bool Init();
	void Render(DirectX::XMMATRIX _View, DirectX::XMMATRIX _Projection);
	void Release();
	void SetNone();
	void SetSolidColor(Vector3 _SolidColor);
	void LoadTexture(const char* _Path);
	void ReleaseTexture();
	SkyType GetType();
	Vector3 GetSolidColor();

private:
	ID3DBlob*                VS              = nullptr;   // raw shader data
	ID3DBlob*                PS              = nullptr;   // raw shader data
	ID3D11VertexShader*      pVS             = nullptr;   // shader options
	ID3D11PixelShader*       pPS             = nullptr;   // shader options
	ID3D11InputLayout*       pLayout         = nullptr;   // -//-
	ID3D11Buffer*            pConstantBuffer = nullptr;   // bridge for data (CPU -> GPU)
	ID3D11RasterizerState*   pCullNone       = nullptr;   // disable culling
	ID3D11DepthStencilState* pLessEqual      = nullptr;   // view other vertices
	ID3D11SamplerState*      pSamplerState   = nullptr;   // -//-

private:
	SkyType type = SkyNone;

private:
	ID3D11ShaderResourceView* texture = nullptr;
	Vector3 solidColor;
	ModelStorageBuffer* modelStorageBuffer = nullptr;
	MeshStorageBuffer* meshStorageBuffer = nullptr;

public:
	float exposure = 1.0f;
	static Sky* GetSingleton();
};