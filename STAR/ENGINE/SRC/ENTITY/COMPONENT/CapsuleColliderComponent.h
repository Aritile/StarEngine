#pragma once

#include "physx/PxPhysics.h"
#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include "../../XTK/MATH/SimpleMath.h"
#include "PhysicsMaterialComponent.h"

using namespace DirectX::SimpleMath;

// this is not component for entity, do not use AddComponent
struct CapsuleColliderComponent
{
public:
	bool activeComponent = true;

public:
	bool CreateShape(float _Radius, float _Height);
	void Render(std::vector<CapsuleColliderComponent>* ccc, size_t index);
	void SetCenter(Vector3 value);
	Vector3 GetCenter() const;
	void SetRadius(float value);
	float GetRadius();
	void Update();
	void SetExtent(float value);
	float GetExtent();
	void SetHeight(float value);
	float GetHeight();

private:
	physx::PxShape* pxShape = NULL;
	PhysicsMaterialComponent physicsMaterialComponent;
	float lastRadius = 1.0f;
	float lastExtent = 1.0f;
	float lastHeight = 1.0f;

public:
	physx::PxShape* GetShape();
	void Release();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};