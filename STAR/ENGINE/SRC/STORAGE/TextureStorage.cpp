#include "TextureStorage.h"
#include "../STAR/Star.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../DX/DX.h"

static DX* dx = DX::GetSingleton();

TextureStorage* TextureStorage::GetSingleton()
{
	static TextureStorage textureStorage;
	return &textureStorage;
}

bool TextureStorage::LoadTexture(const char* _Path, TextureStorageBuffer** _Texture)
{
	if (IsTextureAlreadyLoaded(_Path))
	{
		Star::AddLog("[TextureStorage] -> Texture is already loaded.");
		GetTexture(_Path, _Texture);
		return true;
	}
	else
	{
		std::string type = Star::GetFileExtensionFromPath(_Path);

		if (type.compare(PNG) == 0 || type.compare(JPEG) == 0)
		{
			Star::AddLog("[TextureStorage] -> Loading texture.. %s", _Path);

			textureStorageBuffers.emplace_back();
			TextureStorageBuffer* textureStorageBuffer = &textureStorageBuffers.back();

			if (FAILED(DirectX::CreateWICTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				Star::ConvertString(_Path).c_str(),
				nullptr,
				&textureStorageBuffer->texture)))
			{
				Star::AddLog("[TextureStorage] -> Failed to load texture.");
				textureStorageBuffers.pop_back();
;				return false;
			}
			else
			{
				textureStorageBuffer->path = _Path;
				*_Texture = textureStorageBuffer;
				return true;
			}
		}
		else if (type.compare(DDS) == 0)
		{
			Star::AddLog("[TextureStorage] -> Loading texture.. %s", _Path);

			textureStorageBuffers.push_back(TextureStorageBuffer());
			TextureStorageBuffer* textureStorageBuffer = &textureStorageBuffers.back();

			if (FAILED(DirectX::CreateDDSTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				Star::ConvertString(_Path).c_str(),
				nullptr,
				&textureStorageBuffer->texture)))
			{
				Star::AddLog("[TextureStorage] -> Failed to load texture.");
				textureStorageBuffers.pop_back();
				return false;
			}
			else
			{
				textureStorageBuffer->path = _Path;
				*_Texture = textureStorageBuffer;
				return true;
			}
		}
	}

	return false;
}
bool TextureStorage::GetTexture(const char* _Path, TextureStorageBuffer** _Texture)
{
	std::list<TextureStorageBuffer>::iterator it;
	for (it = textureStorageBuffers.begin(); it != textureStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
		{
			*_Texture = &(*it);
			return true;
		}
	}

	return false;
}
bool TextureStorage::ReleaseTexture(const char* _Path)
{
	std::list<TextureStorageBuffer>::iterator it;
	for (it = textureStorageBuffers.begin(); it != textureStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
		{
			if (it->texture)
				it->texture->Release();
			textureStorageBuffers.erase(it);
			return true;
		}
	}

	return false;
}
void TextureStorage::ReleaseAllTextures()
{
	std::list<TextureStorageBuffer>::iterator it;
	for (it = textureStorageBuffers.begin(); it != textureStorageBuffers.end(); ++it)
	{
		if (it->texture)
			it->texture->Release();
	}

	textureStorageBuffers.clear();
}
bool TextureStorage::IsTextureAlreadyLoaded(const char* _Path)
{
	std::list<TextureStorageBuffer>::iterator it;
	for (it = textureStorageBuffers.begin(); it != textureStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
			return true;
	}

	return false;
}