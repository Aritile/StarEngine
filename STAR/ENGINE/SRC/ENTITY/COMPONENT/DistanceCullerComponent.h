#pragma once

#include "SimpleMath.h"

struct DistanceCullerComponent
{
public:
	float distance = 5.0f;
	bool IsValidDistance(DirectX::SimpleMath::Vector3 _CameraPos);
	void Render();
	bool activeComponent = true;

	DirectX::SimpleMath::Vector3 oldCamPos;
	DirectX::SimpleMath::Vector3 oldPos;
	bool oldValidDistance = false;
	bool reverse = false;
};