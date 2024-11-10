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
	DirectX::XMFLOAT2 iResolution;
	int sampleCount;
	DirectX::XMMATRIX padding;
};

struct CBImage
{
	DirectX::XMMATRIX iWorld;
	DirectX::XMMATRIX iView;
	DirectX::XMMATRIX iProjection;
	float iOpacity = 1.0f;
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

	// UI
	void InitImage();
	void RenderImage(entt::entity entity);
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
	CB cb;
	CBRectangle cbRectangle;
	CBImage cbImage;
	ConstantBuffer* imageConstantBuffer = nullptr;
	ConstantBuffer* rectangleConstantBuffer = nullptr;
	RasterizerState* rasterizerState = nullptr;
	Vector2 bufferSize;
	SamplerState* samplerState = nullptr;
};