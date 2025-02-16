#include "SpriteStorage.h"
#include "../DX/DX.h"
#include <sstream>
#include "../STAR/Star.h"

static DX* dx = DX::GetSingleton();

SpriteStorage* SpriteStorage::GetSingleton()
{
	static SpriteStorage spriteStorage;
	return &spriteStorage;
}
void SpriteStorage::Init()
{
	spriteBatch = new DirectX::SpriteBatch(dx->dxDeviceContext);
}
void SpriteStorage::Release()
{
	if (spriteBatch)
	{
		delete spriteBatch;
		spriteBatch = nullptr;
	}

	for (SpriteStorageBuffer spriteStorageBuffer : spriteStorageBuffers)
		if (spriteStorageBuffer.spriteFont)
			delete spriteStorageBuffer.spriteFont;

	spriteStorageBuffers.clear();
}
DirectX::SpriteFont* SpriteStorage::LoadSpriteFont(const char* _Filename)
{
	std::wstringstream _wstringstream;
	_wstringstream << _Filename;

	// load old
	for (size_t i = 0; i < spriteStorageBuffers.size(); i++)
	{
		SpriteStorageBuffer* spriteStorageBuffer = &spriteStorageBuffers.at(i);
		if (spriteStorageBuffer->path == _Filename)
		{
			Star::AddLog("[SpriteStorage] -> Sprite font is already loaded.");
			return spriteStorageBuffer->spriteFont; // return old
		}
	}

	// load new
	spriteStorageBuffers.emplace_back();
	SpriteStorageBuffer* spriteStorageBuffer = &spriteStorageBuffers.back();
	Star::AddLog("[SpriteStorage] -> Loading new sprite font..");
	spriteStorageBuffer->spriteFont = new DirectX::SpriteFont(dx->dxDevice, _wstringstream.str().c_str());
	spriteStorageBuffer->path = _Filename;
	return spriteStorageBuffer->spriteFont; // return new
}
DirectX::SpriteBatch* SpriteStorage::GetSpriteBatch()
{
	return spriteBatch;
}