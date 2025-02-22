#pragma once

#include <string>
#include <vector>
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include "../ENTITY/COMPONENT/MeshComponent.h"

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
};

class AssimpLoader
{
public:
	std::vector<Mesh> LoadRawModel(std::string path);
	void LoadModel(const char* path, entt::entity entity, unsigned int flags = 0);

private:
	void ProcessRawNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& arg);
	Mesh ProcessRawMesh(aiNode* node, aiMesh* mesh, const aiScene* scene);
	void ProcessMatrix(aiNode* _Node, const aiScene* _Scene, aiMatrix4x4 parentTransform, ModelStorageBuffer* modelStorageBuffer);

public:
	static AssimpLoader* GetSingleton();
};