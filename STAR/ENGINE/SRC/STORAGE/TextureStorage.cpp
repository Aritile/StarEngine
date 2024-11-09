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

		if (Star::ImageFormatCheck(type.c_str()))
		{
			Star::AddLog("[TextureStorage] -> Loading texture.. %s", _Path);

			textureStorageBuffers.emplace_back();
			TextureStorageBuffer* textureStorageBuffer = &textureStorageBuffers.back();
			DirectX::ScratchImage image; // trash

			if (FAILED(DirectX::LoadFromWICFile(Star::ConvertString(_Path).c_str(),
				                                DirectX::WIC_FLAGS_NONE,
				                                &textureStorageBuffer->texMetadata,
				                                image)))
			{
				Star::AddLog("[TextureStorage] -> Failed to load texture.");
				textureStorageBuffers.pop_back();
;				return false;
			}
			else
			{
				if (FAILED(DirectX::CreateShaderResourceView(dx->dxDevice,
					                                         image.GetImages(),
					                                         image.GetImageCount(), 
					                                         textureStorageBuffer->texMetadata,
					                                         &textureStorageBuffer->texture)))
				{
					Star::AddLog("[TextureStorage] -> Failed to create texture.");
					textureStorageBuffers.pop_back();
					return false;
				}
				else
				{
					textureStorageBuffer->path = _Path;
					image.Release();
					*_Texture = textureStorageBuffer;
					return true;
				}
			}
		}
		else if (type.compare(DDS) == 0)
		{
			Star::AddLog("[TextureStorage] -> Loading texture.. %s", _Path);

			textureStorageBuffers.push_back(TextureStorageBuffer());
			TextureStorageBuffer* textureStorageBuffer = &textureStorageBuffers.back();
			DirectX::ScratchImage image; // trash

			if (FAILED(DirectX::LoadFromDDSFile(Star::ConvertString(_Path).c_str(),
				                                DirectX::DDS_FLAGS_NONE,
				                                &textureStorageBuffer->texMetadata,
				                                image)))
			{
				Star::AddLog("[TextureStorage] -> Failed to load texture.");
				textureStorageBuffers.pop_back();
				return false;
			}
			else
			{
				if (FAILED(DirectX::CreateShaderResourceView(dx->dxDevice,
					image.GetImages(),
					image.GetImageCount(),
					textureStorageBuffer->texMetadata,
					&textureStorageBuffer->texture)))
				{
					Star::AddLog("[TextureStorage] -> Failed to create texture.");
					textureStorageBuffers.pop_back();
					return false;
				}
				else
				{
					textureStorageBuffer->path = _Path;
					image.Release();
					*_Texture = textureStorageBuffer;
					return true;
				}
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