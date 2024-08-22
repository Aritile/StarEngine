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

private:
	physx::PxShape* pxShape = NULL;
	PhysicsMaterialComponent physicsMaterialComponent;

public:
	physx::PxShape* GetShape();
	void Release();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};