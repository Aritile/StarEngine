#pragma once

#include "physx/PxPhysics.h"
#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include <SimpleMath.h>
#include "PhysicsMaterialComponent.h"

using namespace DirectX::SimpleMath;

// this is not component for entity, do not use AddComponent
struct SphereColliderComponent
{
public:
	bool activeComponent = true;

public:
	void CreateShape(float _Radius);
	void Render(std::vector<SphereColliderComponent>* bcc, size_t index);

private:
	physx::PxShape* pxShape = NULL;
	float lastRadius = 1.0f;
	float lastExtent = 1.0f;
	PhysicsMaterialComponent physicsMaterialComponent;

public:
	physx::PxShape* GetShape() const;
	void SetCenter(Vector3 value);
	Vector3 GetCenter() const;
	void SetExtent(float value);
	float GetExtent();
	void Update();
	void SetRadius(float value);
	float GetRadius();
	void Release();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};;