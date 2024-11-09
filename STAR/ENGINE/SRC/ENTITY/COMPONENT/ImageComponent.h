#pragma once

#include "../../STORAGE/TextureStorage.h"

struct ImageComponent
{
public:
	void Render();
	TextureStorageBuffer* GetTextureStorageBuffer();

private:
	bool activeComponent = true;
	TextureStorageBuffer* textureStorageBuffer = nullptr;
};