#pragma once

#include <string>
#include <d3d11.h>
#include "../STORAGE/TextureStorage.h"
#include <SimpleMath.h>

struct Material
{
	std::string name;
	std::string diffuse;
	TextureStorageBuffer* diffuseTexture = nullptr;
	DirectX::SimpleMath::Vector4 baseColor;

	void SerializeMaterial(const char* path);
	void DeserializeMaterial(const char* path);
};
