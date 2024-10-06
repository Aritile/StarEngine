#pragma once

#include <list>
#include <string>
#include <d3d11.h>
#include <Windows.h>
#include "../VERTEX/Vertex.h"
#include "../MATERIAL/Material.h"
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

struct MeshStorageBuffer
{
	MeshStorageBuffer() {}

	void AddVertex(Vertex _Vertex);
	void AddIndex(UINT _Index);
	bool LoadMesh(const aiScene* _Scene, UINT _Index);
	bool SetupMesh();
	bool CreateMaterial(const char* _Path, const aiScene* _Scene, UINT _Index);
	void Release();

	std::string name;
	UINT index = 0;
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	Material material;
};

struct ModelStorageBuffer
{
	ModelStorageBuffer() {}

	Assimp::Importer importer;
	std::string path;
	UINT meshNum = 0;
	std::list<MeshStorageBuffer> meshStorageBuffers;

	bool LoadMesh(UINT _Index, MeshStorageBuffer** _MeshStorageBuffer);
	bool GetMesh(UINT _Index, MeshStorageBuffer** _MeshStorageBuffer);
	bool IsMeshAlreadyLoaded(UINT _Index);
	bool ReleaseMesh(UINT _Index);
	void ReleaseAllMeshes();

	void OpenModel(const char* _Path, unsigned int flags = 0);
	void CloseModel();
};

class MeshStorage
{
public:
	bool LoadModel(const char* _Path, ModelStorageBuffer** _ModelStorageBuffer, unsigned int flags = 0);
	bool GetModel(const char* _Path, ModelStorageBuffer** _ModelStorageBuffer);
	bool IsModelAlreadyLoaded(const char* _Path);
	bool ReleaseModel(const char* _Path);
	void ReleaseAllModels();
	void CloseAllOpenedModels();

	// raw mesh
	bool BuildMesh(const char* _Name, std::vector<Vertex>* vertices, std::vector<UINT>* indices, MeshStorageBuffer** _MeshStorageBuffer);
	bool ReleaseMesh(const char* _Name);
	void ReleaseAllMeshes();
	bool IsMeshAlreadyBuild(const char* _Name);
	bool GetMesh(const char* _Name, MeshStorageBuffer** _MeshStorageBuffer);

public:
	static MeshStorage* GetSingleton();

private:
	std::list<ModelStorageBuffer> modelStorageBuffers;

	// raw mesh
	std::list<MeshStorageBuffer> meshStorageBuffers;
};