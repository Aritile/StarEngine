#include "AssimpLoader.h"
#include <filesystem>
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../EDITOR/WINDOW/Console.h"
#include "../STORAGE/MeshStorage.h"

AssimpLoader* AssimpLoader::GetSingleton()
{
	static AssimpLoader assimpLoader;
	return &assimpLoader;
}

static MeshStorage* meshStorage = MeshStorage::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();

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
			vertex.textureCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.textureCoord = Vector2(0.0f, 0.0f);
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
void AssimpLoader::LoadModel(const char* path, entt::entity entity, unsigned int flags)
{
	if (!path)
		return;

	if (!ecs->IsValid(entity))
		return;

	entt::entity parent = ecs->CreateEntity();
	ecs->GetComponent<GeneralComponent>(parent).SetName(Star::GetFileNameFromPath(path));
	ecs->GetComponent<GeneralComponent>(entity).AddChild(parent);

	// load model
	ModelStorageBuffer* modelStorageBuffer = nullptr;
	meshStorage->LoadModel(path, &modelStorageBuffer);
	if (modelStorageBuffer)
	{
		// process matrix
		if (modelStorageBuffer->importer.GetScene())
		{
			const aiScene* scene = modelStorageBuffer->importer.GetScene();
			ProcessMatrix(scene->mRootNode, scene, aiMatrix4x4(), modelStorageBuffer);
		}

		for (UINT i = 0; i < modelStorageBuffer->meshNum; i++)
		{
			entt::entity child = ecs->CreateEntity();
			ecs->AddComponent<MeshComponent>(child);

			// load mesh
			MeshStorageBuffer* meshStorageBuffer = nullptr;
			modelStorageBuffer->LoadMesh(i, &meshStorageBuffer);

			// find matrix
			std::list<MeshMatrix>::iterator it;
			for (it = modelStorageBuffer->meshMatrices.begin(); it != modelStorageBuffer->meshMatrices.end(); ++it)
			{
				if (it->meshName == meshStorageBuffer->name)
				{
					meshStorageBuffer->matrix = &(*it);
					break;
				}
			}

			// try load file texture
			TextureStorageBuffer* textureStorageBuffer = nullptr;
			textureStorage->LoadTexture(meshStorageBuffer->material.diffuse.c_str(), &textureStorageBuffer);

			// get material
			aiMaterial* material = nullptr;
			if (modelStorageBuffer->importer.GetScene())
				material = modelStorageBuffer->importer.GetScene()->mMaterials[meshStorageBuffer->materialIndex];

			// try load base color
			if (material)
			{
				aiColor4D baseColor;
				if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor))
				{
					meshStorageBuffer->material.baseColor.x = baseColor.r;
					meshStorageBuffer->material.baseColor.y = baseColor.g;
					meshStorageBuffer->material.baseColor.z = baseColor.b;
					meshStorageBuffer->material.baseColor.w = baseColor.a;
				}
			}

			// try load embedded texture
			if (!textureStorageBuffer)
			{
				if (material)
				{
					aiString texturePath;
					if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
					{
						const aiTexture* embeddedTexture = modelStorageBuffer->importer.GetScene()->GetEmbeddedTexture(texturePath.C_Str());
						if (embeddedTexture) // load texture from model
						{
							if (embeddedTexture->mHeight == 0) // compressed texture
								textureStorage->CreateTexture(embeddedTexture->pcData, embeddedTexture->mWidth, 0, 0, true, &textureStorageBuffer);
							else // raw texture
								textureStorage->CreateTexture(embeddedTexture->pcData, 0, embeddedTexture->mWidth, embeddedTexture->mHeight, false, &textureStorageBuffer);
						}
					}
				}
			}

			// apply texture
			if (textureStorageBuffer)
				meshStorageBuffer->material.diffuseTexture = textureStorageBuffer;

			// save materials
			std::string y = Star::GetParent(path) + "\\" + Star::GetFileNameFromPath(path);
			Star::CreateFolder(y);
			y += "\\" + meshStorageBuffer->material.name + ".mat";
			meshStorageBuffer->material.SerializeMaterial(y.c_str());

			// apply all
			ecs->GetComponent<MeshComponent>(child).ApplyModel(modelStorageBuffer);
			ecs->GetComponent<MeshComponent>(child).ApplyMesh(meshStorageBuffer);
			ecs->GetComponent<GeneralComponent>(child).SetName(meshStorageBuffer->name);
			if (meshStorageBuffer->matrix)
				ecs->GetComponent<TransformComponent>(child).SetTransform(meshStorageBuffer->matrix->meshMatrix);
			ecs->GetComponent<GeneralComponent>(parent).AddChild(child);
		}

		modelStorageBuffer->CloseModel();
	}
}

void AssimpLoader::ProcessMatrix(aiNode* _Node, const aiScene* _Scene, aiMatrix4x4 parentTransform, ModelStorageBuffer* modelStorageBuffer)
{
	aiMatrix4x4 nodeTransform = _Node->mTransformation;
	aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

	for (UINT i = 0; i < _Node->mNumMeshes; i++)
	{
		aiMesh* mesh = _Scene->mMeshes[_Node->mMeshes[i]];
		MeshMatrix meshMatrix;
		meshMatrix.meshName = mesh->mName.C_Str();
		meshMatrix.meshMatrix = Star::AssimpToMatrix(globalTransform);

		// apply
		modelStorageBuffer->meshMatrices.push_back(meshMatrix);
	}

	for (UINT i = 0; i < _Node->mNumChildren; i++)
		ProcessMatrix(_Node->mChildren[i], _Scene, globalTransform, modelStorageBuffer);
}