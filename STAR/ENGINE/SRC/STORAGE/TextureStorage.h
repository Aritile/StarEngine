#pragma once

#include <list>
#include <string>
#include <d3d11.h>

struct TextureStorageBuffer
{
	TextureStorageBuffer() {}

	std::string path = "";
	ID3D11ShaderResourceView* texture = nullptr;
};

class TextureStorage
{
public:
	bool LoadTexture(const char* _Path, TextureStorageBuffer** _Texture);
	bool GetTexture(const char* _Path, TextureStorageBuffer** _Texture);
	bool ReleaseTexture(const char* _Path);
	void ReleaseAllTextures();
	bool IsTextureAlreadyLoaded(const char* _Path);

public:
	static TextureStorage* GetSingleton();

private:
	std::list<TextureStorageBuffer> textureStorageBuffers;
};