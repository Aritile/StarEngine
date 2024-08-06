#pragma once

#include <string>
#include <d3d11.h>
#include "../STORAGE/TextureStorage.h"

struct Material
{
	std::string name;
	std::string diffuse;
	TextureStorageBuffer* diffuseTexture = nullptr;

	void SerializeMaterial(const char* path);
	void DeserializeMaterial(const char* path);
};
