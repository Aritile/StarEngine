#pragma once

#include "CB.h"
#include "shader.h"
#include "constant_buffer.h"
#include "rasterizer_state.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "Vertex.h"
#include "render_target.h"
#include "sampler_state.h"

struct CBRectangle
{
	DirectX::XMFLOAT2 iResolution = DirectX::XMFLOAT2(0.0f, 0.0f);
	int sampleCount = 0;
	DirectX::XMMATRIX padding = DirectX::XMMatrixIdentity();
};
struct CBImage
{
	DirectX::XMMATRIX iWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX iView = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX iProjection = DirectX::XMMatrixIdentity();
	float iOpacity = 1.0f;
};
struct CBRectangle2
{
	DirectX::XMMATRIX iWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX iView = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX iProjection = DirectX::XMMatrixIdentity();
	float iOpacity = 1.0f;
	DirectX::XMFLOAT3 iColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class Widgets
{
public:
	static Widgets* GetSingleton();

public:
	void Init();
	void InitBoundingBoxWidget();
	void InitGridWidget();
	void InitPerspectiveFrustumWidget();
	void InitOrthographicFrustumWidget();
	void InitCanvas();

	// UI
	void InitImage();
	void InitRectangle();
	void RenderImage(entt::entity entity);
	void RenderRectangle(entt::entity entity);
	Shader* imageShader = nullptr;

	void InitRenderTarget(unsigned int _MultisamplingCount);
	void SetRenderTarget();
	void UnsetRenderTarget();
	ShaderResourceID GetRenderTargetShaderResource();
	RenderTargetID GetRenderTarget();
	DepthStencilID GetDepthStencil();
	void ResizeRenderTarget(Vector2 _Size, unsigned int _MultisamplingCount);
	void RecreateRenderTarget(Vector2 _Size, unsigned int _MultisamplingCount);

public:
	void RenderBoundingBoxWidget();
	void RenderGridWidget();
	void RenderPerspectiveFrustumWidget();
	void RenderOrthographicFrustumWidget();
	void SetRasterizerState();
	void UnsetRasterizerState();
	void RenderRectangle(bool _EnableMultisampling, unsigned int _MultisamplingCount);
	void RenderCanvas();

public:
	void SetRenderGrid(bool _Render);
	void SetGridSize(int _Size);
	void SetGridPos(Vector3 _Pos);
	bool GetRenderGrid();
	int GetGridSize();
	Vector3 GetGridPos();

private:
	bool renderGrid = true;
	int gridSize = 15;
	Vector3 gridPos;

	std::vector<POSCOL> BuildGrid();

private:
	bool IsGood();
	Matrix CreateNoScaleMatrix(TransformComponent& transformComponent);

public:
	void Release();

private:
	Shader* boundingBoxShader = nullptr;
	Shader* gridShader = nullptr;
	Shader* perspectiveFrustum = nullptr;
	Shader* orthographicFrustum = nullptr;
	ConstantBuffer* constantBuffer = nullptr;
	RenderTarget* renderTarget = nullptr;
	Shader* rectangle = nullptr;
	Shader* rectangle2 = nullptr;
	CB cb;
	CBRectangle cbRectangle;
	CBImage cbImage;
	CBRectangle2 cbRectangle2;
	ConstantBuffer* imageConstantBuffer = nullptr;
	ConstantBuffer* rectangleConstantBuffer = nullptr;
	ConstantBuffer* rectangle2ConstantBuffer = nullptr;
	RasterizerState* rasterizerState = nullptr;
	Vector2 bufferSize;
	SamplerState* samplerState = nullptr;
	Shader* canvasShader = nullptr;
};