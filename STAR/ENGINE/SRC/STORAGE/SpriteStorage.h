#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <vector>
#include <string>

struct SpriteStorageBuffer
{
	DirectX::SpriteFont* spriteFont;
	std::string path;
};

class SpriteStorage
{
public:
	static SpriteStorage* GetSingleton();
	void Init();
	void Release();
	DirectX::SpriteFont* LoadSpriteFont(const char* _Filename);
	DirectX::SpriteBatch* GetSpriteBatch();

private:
	DirectX::SpriteBatch* spriteBatch = nullptr;
	std::vector<SpriteStorageBuffer> spriteStorageBuffers;
};