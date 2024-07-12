#pragma once

#include "../../IMGUI/imgui.h"
#include "../../HELPERS/Helpers.h"

#define MIN_FOV 1.0f
#define MAX_FOV 128.0f

enum CameraType
{
	Perspective,
	Orthographic
};

struct CameraComponent
{
public:
	void Render();

private:
	bool          activeComponent = true;
	CameraType    cameraType = CameraType::Perspective;
	const char*   items[2] = { "Perspective", "Orthographic" };
	int           item_current = 0;
	float         scale = 5.0f;
	float         fovView = 45.0f;
	float         nearView = 0.1f;
	float         farView = 100.0f;
	bool          automaticAspect = true;
	float         smoothOrthographic = 0.001f;
	Vector2       aspectView = Vector2(0.0f, 0.0f);
	Matrix        projectionMatrix = Matrix::Identity;
	Matrix        viewMatrix = Matrix::Identity;

private:
	void CreateNewViewMatrix();
	void CreateNewProjectionMatrix();

public:
	void          SetFov(float _Fov);
	float         GetFov();
	void          SetNear(float _Near);
	float         GetNear();
	void          SetFar(float _Far);
	float         GetFar();
	void          SetCameraType(CameraType _CameraType);
	CameraType GetCameraType();
	void          SetActive(bool _Active);
	bool          IsActive();
	void          SetScale(float _Scale);
	float         GetScale();
	void          SetAspect(Vector2 _Aspect);
	Vector2       GetAspect();
	Matrix        GetViewMatrix();
	Matrix        GetProjectionMatrix();
	Vector2       GetFixedProjectionValue(CameraType _CameraType);

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};