#pragma once

#include "physx/PxPhysics.h"
#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include <SimpleMath.h>
#include "PhysicsMaterialComponent.h"

using namespace DirectX::SimpleMath;

// this is not component for entity, do not use AddComponent
struct BoxColliderComponent
{
public:
	bool activeComponent = true;

public:
	void CreateShape(Vector3 _Size);
	void Render(std::vector<BoxColliderComponent>* bcc, size_t index);

private:
	physx::PxShape* pxShape = NULL;
	Vector3 lastSize = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 lastExtents = Vector3(1.0f, 1.0f, 1.0f);
	PhysicsMaterialComponent physicsMaterialComponent;

public:
	physx::PxShape* GetShape() const;
	void SetCenter(Vector3 value);
	Vector3 GetCenter() const;
	void SetExtents(Vector3 value);
	Vector3 GetExtents();
	void Update();
	void SetSize(Vector3 value);
	Vector3 GetSize();
	void Release();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};