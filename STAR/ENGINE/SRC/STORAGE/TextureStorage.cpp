#include "TextureStorage.h"
#include "../STAR/Star.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../DX/DX.h"
#include <WICTextureLoader11.h>

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
bool TextureStorage::CreateTexture(const void* _Data, UINT _Size, UINT _Width, UINT _Height, bool _IsCompressed, TextureStorageBuffer** _Texture)
{
	textureStorageBuffers.push_back(TextureStorageBuffer());
	TextureStorageBuffer* textureStorageBuffer = &textureStorageBuffers.back();

	if (_IsCompressed)
	{
		if (FAILED(DirectX::CreateWICTextureFromMemory(dx->dxDevice,
			dx->dxDeviceContext,
			reinterpret_cast<const uint8_t*>(_Data),
			_Size,
			nullptr,
			&textureStorageBuffer->texture)))
		{
			printf("Failed.\n");
			return false;
		}

		*_Texture = textureStorageBuffer;
		return true;
	}
	else
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = _Width;
		desc.Height = _Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = _Data;
		initData.SysMemPitch = _Width * 4; // 4 bytes per pixel (RGBA)

		ID3D11Texture2D* texture = nullptr;
		if (FAILED(dx->dxDevice->CreateTexture2D(&desc, &initData, &texture)))
		{
			printf("Failed.\n");
			return false;
		}

		if (FAILED(dx->dxDevice->CreateShaderResourceView(texture, nullptr, &textureStorageBuffer->texture)))
		{
			printf("Failed.\n");
			return false;
		}

		texture->Release();

		*_Texture = textureStorageBuffer;
		return true;
	}

	textureStorageBuffers.pop_back();
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