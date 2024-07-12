#pragma once

#include "CB.h"
#include "shader.h"
#include "constant_buffer.h"
#include "rasterizer_state.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"

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

public:
	void RenderBoundingBoxWidget();
	void RenderGridWidget();
	void RenderPerspectiveFrustumWidget();
	void RenderOrthographicFrustumWidget();
	void SetRasterizerState();
	void UnsetRasterizerState();

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
	CB cb;
	RasterizerState* rasterizerState = nullptr;
};