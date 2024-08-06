#pragma once

#include <DirectXMath.h>

struct ConstantData
{
	DirectX::XMMATRIX sProjection;
	DirectX::XMMATRIX sView;
	DirectX::XMMATRIX sModel;
	bool hasTexture = false;
	int renderState = 0;
};