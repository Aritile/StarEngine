#pragma once

#include "../../STORAGE/TextureStorage.h"

struct ImageComponent
{
public:
	void Render();
	TextureStorageBuffer* GetTextureStorageBuffer();
	void SetActive(bool _Active);
	bool IsActive();

private:
	bool activeComponent = true;
	TextureStorageBuffer* textureStorageBuffer = nullptr;
};