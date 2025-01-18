#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include "../../DX/DX.h"
#include "../../IMGUI/imgui.h"
#include "../../IMGUIZMO/ImGuizmo.h"
#include "../../ENTITY/COMPONENT/MeshComponent.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

enum RenderState
{
	Triangle,
	Line,
	Point,
	Position,
	Normal
};

struct TransformComponent;

// buffer
#define MIN_XY 16

class ViewportWindow
{
public:
	void Render();
	void ReleaseBuffer();
	bool renderViewport = true;
	void SetDefaultCam();

public:
	D3D11_VIEWPORT GetViewport();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11ShaderResourceView* GetShaderResourceView();
	float GetNear();
	float GetFar();
	float GetFov();
	float GetSpeed();
	float GetBoostSpeed();
	void SetNear(float _Near);
	void SetFar(float _Far);
	void SetFov(float _Fov);
	void SetSpeed(float _Speed);
	void SetBoostSpeed(float _BoostSpeed);
	float GetPosLerp();
	float GetRotLerp();
	void SetPosLerp(float _Value);
	void SetRotLerp(float _Value);

private:
	ID3D11RenderTargetView* s_RenderTargetView = NULL;
	ID3D11DepthStencilView* s_DepthStencilView = NULL;
	ID3D11ShaderResourceView* s_ShaderResourceView = NULL;

private:
	bool IsBufferResized();
	bool cursorOnWindow = false;
	bool CreateBuffer(ImVec2 resolution);
	ImVec2 bufferSize = ImVec2(NULL, NULL);
	ImVec2 windowPos = ImVec2(NULL, NULL);
	ImVec2 windowSize = ImVec2(NULL, NULL);
	ImVec2 windowSizeAvail = ImVec2(NULL, NULL);
	ImVec2 windowCursorPos = ImVec2(NULL, NULL);

public:
	ImVec2 GetBufferSize();

private:
	Matrix projection = Matrix::Identity;
	Matrix view = Matrix::Identity;

private:
	Vector3 pos;
	Quaternion rot;
	Vector3 sPos;
	Quaternion sRot;

	void SetPerspectiveProjectionMatrix(ImVec2 size, float fov, float NearZ, float FarZ);

public:
	Matrix GetPerspectiveProjectionMatrix();
	Matrix GetPerspectiveViewMatrix();

public:
	void LookAt(Vector3 targetPos, DirectX::BoundingBox boundingBox, float distanceFactor, float offset);
	void LookAtEntity(entt::entity entity);

	POINT cursorPosition = { 0 };
	bool off = false;

public:
	void UpdateMovement();
	void SetRenderState(RenderState _RenderState);
	RenderState GetRenderState();
	void RefreshRenderState();

private:
	float camNearZ = 0.1f;
	float camFarZ = 1000.0f;
	float camFov = 45.0f;
	float camSpeed = 8.0f;
	float camBoostSpeed = 16.0f;
	float camSensitivity = 0.001f;
	float posLerp = 8.0f;
	float rotLerp = 16.0f;
	RenderState renderState = RenderState::Triangle;

private:
	void RenderWidget();
	void RenderManipulateWidget(TransformComponent& transformComponent);

private:
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::LOCAL;

public:
	void SetOperation(ImGuizmo::OPERATION arg);
	void SetMode(ImGuizmo::MODE arg);
	ImGuizmo::OPERATION GetOperation();
	ImGuizmo::MODE GetMode();

public:
	bool useSnap = false;
	float snap = 1.0f;

private:
	void RayVector(float mouseX, float mouseY, DirectX::XMVECTOR& pickRayInWorldSpacePos, DirectX::XMVECTOR& pickRayInWorldSpaceDir);
	bool PointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point);
	float PickMesh(DirectX::XMVECTOR pickRayInWorldSpacePos, DirectX::XMVECTOR pickRayInWorldSpaceDir, const std::vector<Vertex>& vertPosArray, const std::vector<UINT>& indexPosArray, DirectX::XMMATRIX worldSpace);
	entt::entity RunRay(bool select);
	void GuizmoSystem(int _Value, float _Offset, Matrix _Backup);

	public:
	void Init();

public:
	static ViewportWindow* GetSingleton();
};