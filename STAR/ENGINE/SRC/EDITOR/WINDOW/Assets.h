#pragma once

#include <string>
#include <vector>
#include <thread>
#include <d3d11.h>
#include <filesystem>
#include "../Editor.h"
#include "../../DX/DX.h"
#include "../../SKY/Sky.h"
#include "../../IMGUI/imgui.h"
#include <DirectXTex.h>
#include <SimpleMath.h>
#include <WICTextureLoader11.h>
#include <DDSTextureLoader11.h>

#define LUA ".lua" // script
#define JPEG ".jpeg" // image
#define JPG ".jpg" // this format is just jpeg, why? because people have created their own shortened form
#define PNG ".png" // image
#define DDS ".dds" // image
#define HDR ".hdr" // image
#define WAV ".wav" // audio
#define TXT ".txt" // text
#define OBJ ".obj" // model
#define FBX ".fbx" // model
#define TTF ".ttf" // font
#define SKY ".sky" // sky
#define MAT ".mat" // material
#define SPRITEFONT ".spritefont" // sprite font
#define GLTF ".gltf" // model, ascii
#define GLB ".glb" // same as gltf but in binary format

#define FOLDER "\\assets"

struct FILEs
{
	//// FILE/DIR ////
	std::string file_name;
	std::string file_name_only;
	std::string file_type;
	bool is_dir = false;
	bool is_selected = false;
	int file_size = 0;
	//// IMG INFO ////
	ID3D11ShaderResourceView* texture = nullptr;

	FILEs(std::string file_name, std::string file_name_only, std::string file_type, bool is_dir, int file_size)
	{
		this->file_name = file_name;
		this->file_name_only = file_name_only;
		this->file_type = file_type;
		this->is_dir = is_dir;
		this->file_size = file_size;
	}
};

struct dir_list
{
	std::string dir_name;
	std::vector<dir_list> dir_child;
	dir_list()
	{
		/* code */
	}
};

class AssetsWindow
{
public:
	void Init();
	void Render();
	void Shutdown();
	bool renderAssets = true;

public:
	void CreateDir(std::string path);
	void OpenDir(std::string path);
	void RemoveAsset(std::string path);
	void GoBackDir();
	void RefreshDir();
	FILEs* GetSelFile();
	bool GetOutCore();
	unsigned int GetSafeName(std::string path, std::string type);
	void GetFileNameFromProjectDir(std::string path, std::string extension, std::vector<std::pair<std::string, std::string>>& data);

public:
	std::string GetAppDirPath();
	std::string GetProjectDirPath();
	std::string GetNowDirPath();

public:
	ID3D11ShaderResourceView* folderTexture = nullptr;
	ID3D11ShaderResourceView* imageTexture = nullptr;
	ID3D11ShaderResourceView* fileTexture = nullptr;
	ID3D11ShaderResourceView* luaTexture = nullptr;
	ID3D11ShaderResourceView* modelTexture = nullptr;
	ID3D11ShaderResourceView* ttfTexture = nullptr;
	ID3D11ShaderResourceView* audioTexture = nullptr;
	ID3D11ShaderResourceView* skyTexture = nullptr;
	ID3D11ShaderResourceView* materialTexture = nullptr;

private:
	std::string projectDirPath;
	std::string nowDirPath;
	std::vector<FILEs> files;
	float size = 64.0f;
	void OutCore(std::string path);
	bool outCore = true;
	void ReleaseTexture();
	FILEs* selectedFile = nullptr;
	void HideSelected();
	bool thumbnail = true;
	dir_list dirs;
	void RenderDirList(dir_list dir);
	void FillDirList(dir_list* dir);

public:
	static AssetsWindow* GetSingleton();
};