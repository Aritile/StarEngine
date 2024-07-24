#pragma once

#include "physx/PxPhysics.h"

// this is not component for entity, do not use AddComponent
struct PhysicsMaterialComponent
{
public:
	void CreateMaterial(float StaticFriction, float DynamicFriction, float Restitution);
	void ReleaseMaterial();
	void RenderLeft(bool begin);
	void RenderRight();
	void SetStaticFriction(float value);
	float GetStaticFriction();
	void SetDynamicFriction(float value);
	float GetDynamicFriction();
	void SetRestitution(float value);
	float GetRestitution();
	physx::PxMaterial* GetMaterial();

private:
	physx::PxMaterial* pxMaterial = NULL;
};