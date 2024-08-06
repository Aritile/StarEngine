#pragma once

#include "../XTK/MATH/SimpleMath.h"

using namespace DirectX::SimpleMath;

struct Vertex
{
	Vector3 position = Vector3(0.0f);
	Vector3 normal = Vector3(0.0f);
	Vector2 textureCoord = Vector2(0.0f);
	//Vector3 tangent = Vector3(0.0f);
	//Vector3 bitangent = Vector3(0.0f);
};
