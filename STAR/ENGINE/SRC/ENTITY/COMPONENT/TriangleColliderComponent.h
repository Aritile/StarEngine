#pragma once

#include "physx/PxPhysics.h"

struct TriangleColliderComponent
{
	void Render();
	bool Create();
	physx::PxShape* pxShape = nullptr;
	bool activeComponent = true;
};