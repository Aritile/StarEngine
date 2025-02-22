#pragma once

#include "physx/PxPhysics.h"

struct PhysicsMaterialComponent
{
public:
	void CreateMaterial(float _StaticFriction, float _DynamicFriction, float _Restitution);
	void Release();
	void SetStaticFriction(float _Value);
	float GetStaticFriction();
	void SetDynamicFriction(float _Value);
	float GetDynamicFriction();
	void SetRestitution(float _Value);
	float GetRestitution();
	physx::PxMaterial* GetMaterial();
	void Render();

private:
	physx::PxMaterial* pxMaterial = nullptr;
};