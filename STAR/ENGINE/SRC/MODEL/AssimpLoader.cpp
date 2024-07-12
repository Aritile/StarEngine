#include "AssimpLoader.h"
#include <filesystem>
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../EDITOR/WINDOW/Console.h"

static AssimpLoader assimpLoader;

AssimpLoader& AssimpLoaderClass()
{
	return assimpLoader;
}

///////////////////////////////////////////////////////////////

std::vector<Mesh> AssimpLoader::LoadRawModel(std::string path)
{
	assert(!path.empty());
	assert(std::filesystem::exists(path));

	std::vector<Mesh> buffer;

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return buffer;

	ProcessRawNode(pScene->mRootNode, pScene, buffer);
	return buffer;
}
void AssimpLoader::ProcessRawNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& arg)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		arg.push_back(ProcessRawMesh(node, mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		ProcessRawNode(node->mChildren[i], scene, arg);
}
Mesh AssimpLoader::ProcessRawMesh(aiNode* node, aiMesh* mesh, const aiScene* scene)
{
	Mesh buffer;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// positions
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// normals
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// texture coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoords = Vector2(0.0f, 0.0f);
		}

		buffer.vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			buffer.indices.push_back(face.mIndices[j]);
	}

	return buffer;
}

/* ------------------------------------ */

static Entity* ecs = Entity::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();

/*
bool AssimpLoader::LoadModel(std::string path, entt::entity entity)
{
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_FBX_CONVERT_TO_M, FALSE);
	const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr) return false;

	if (pScene->HasMaterials())
	{
		std::string buffer = assetsWindow->GetNowDirPath() + "\\" + std::filesystem::path(path).stem().string().c_str();
		assetsWindow->CreateDir(buffer);

		for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
		{
			aiMaterial* material = pScene->mMaterials[i];
			aiString       mat_name = material->GetName();
			std::string    full_path = buffer + "\\" + mat_name.C_Str();
			aiString       diff_name;
			material->GetTexture(aiTextureType_DIFFUSE, NULL, &diff_name);
			Material mat_buff;
			if (!std::string(diff_name.C_Str()).empty())
				mat_buff.diffuse = std::string("../") + diff_name.C_Str();
			consoleWindow->AddDebugMessage("Creating material... %s%s", full_path.c_str(), MAT);
			assetsWindow->SaveMaterialFile(full_path, mat_buff);
		}
	}

	entt::entity parent = ecs->registry.create();
	ecs->registry.emplace<GeneralComponent>(parent);
	ecs->registry.emplace<TransformComponent>(parent);
	ecs->registry.get<GeneralComponent>(parent).SetName(std::filesystem::path(path).stem().string().c_str());
	ecs->registry.get<GeneralComponent>(entity).AddChild(parent);

	ProcessNode(pScene->mRootNode, pScene, parent, path);
	return true;
}

void AssimpLoader::ProcessNode(aiNode* node, const aiScene* scene, entt::entity entity, std::string path)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(node, mesh, scene, entity, path);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, entity, path);
	}
}

void AssimpLoader::ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, entt::entity entity, std::string path)
{
	entt::entity child = ecs->registry.create();
	ecs->registry.emplace<GeneralComponent>(child);
	ecs->registry.emplace<TransformComponent>(child);
	ecs->registry.emplace<MeshComponent>(child);
	auto& gen_comp = ecs->registry.get<GeneralComponent>(child);
	auto& mesh_comp = ecs->registry.get<MeshComponent>(child);
	gen_comp.SetName(mesh->mName.C_Str());
	mesh_comp.SetFileName(path);
	mesh_comp.SetMeshName(mesh->mName.C_Str());
	ecs->registry.get<GeneralComponent>(entity).AddChild(child);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// positions
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// normals
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// texture coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoords = Vector2(0.0f, 0.0f);
		}

		mesh_comp.AddVertices(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			mesh_comp.AddIndices(face.mIndices[j]);
	}

	if (!mesh_comp.SetupMesh())
		StarHelpers::AddLog("Assimp << Setup << Mesh << Failed");

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::string buffer = std::filesystem::path(path).parent_path().string() + "\\" + std::filesystem::path(path).stem().string() + "\\" + material->GetName().C_Str() + MAT;
	mesh_comp.AddMeshMaterial(buffer);
}
*/

void AssimpLoader::LoadModel(const char* path, entt::entity entity)
{
	if (!path)
		return;

	if (!ecs->IsValid(entity))
		return;

	entt::entity parent = ecs->CreateEntity();
	std::string name = StarHelpers::GetFileNameFromPath(path);
	ecs->GetComponent<GeneralComponent>(parent).SetName(name);
	ecs->GetComponent<GeneralComponent>(entity).AddChild(parent);
	{
		Assimp::Importer importer;
		const aiScene* scene = StarHelpers::OpenModel(&importer, path);
		if (scene)
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				entt::entity child = ecs->CreateEntity();
				ecs->AddComponent<MeshComponent>(child);
				auto& meshComponent = ecs->GetComponent<MeshComponent>(child);

				meshComponent.SetModelPath(path);
				meshComponent.SetMeshIndex(i);

				meshComponent.LoadMesh(scene);
				meshComponent.SetupMesh();

				meshComponent.LoadMaterial(scene);
				meshComponent.SetupMaterial(path);

				std::string folder = StarHelpers::GetParent(path) + "\\" + StarHelpers::GetFileNameFromPath(path);
				StarHelpers::CreateFolder(folder);

				std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
				std::string model = StarHelpers::GetParent(path);
				std::string dir = StarHelpers::GetRelativePath(model, exe);
				std::string full = dir + "\\" + StarHelpers::GetFileNameFromPath(path) + "\\" + meshComponent.meshMaterial.name + ".mat";
				meshComponent.SerializeMaterial(full.c_str());
				meshComponent.SetMaterialPath(full.c_str());

				ecs->GetComponent<GeneralComponent>(child).SetName(meshComponent.GetName(scene));
				ecs->GetComponent<GeneralComponent>(parent).AddChild(child);
			}
		}
	}
}