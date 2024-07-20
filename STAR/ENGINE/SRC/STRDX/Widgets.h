#pragma once

#include "CB.h"
#include "shader.h"
#include "constant_buffer.h"
#include "rasterizer_state.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "Vertex.h"

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
	CB cb;
	RasterizerState* rasterizerState = nullptr;
};