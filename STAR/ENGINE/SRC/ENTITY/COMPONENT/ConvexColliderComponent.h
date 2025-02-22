#pragma once

#include "physx/PxPhysics.h"

struct ConvexColliderComponent
{
	void Render();
	bool Create();
	physx::PxShape* pxShape = nullptr;
	bool activeComponent = true;
};