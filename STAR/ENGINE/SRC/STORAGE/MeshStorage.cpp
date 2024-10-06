#include "MeshStorage.h"
#include "../STAR/Star.h"
#include "../DX/DX.h"

static DX* dx = DX::GetSingleton();

bool ModelStorageBuffer::LoadMesh(UINT _Index, MeshStorageBuffer** _MeshStorageBuffer)
{
	if (IsMeshAlreadyLoaded(_Index))
	{
		Star::AddLog("[MeshStorage] -> Mesh is already loaded.");
		GetMesh(_Index, _MeshStorageBuffer);
		return true;
	}
	else
	{
		Star::AddLog("[MeshStorage] -> Loading mesh.. %i", _Index);

		// copy & paste fix
		meshStorageBuffers.emplace_back();
		MeshStorageBuffer* meshStorageBuffer = &meshStorageBuffers.back();

		if (!meshStorageBuffer->LoadMesh(importer.GetScene(), _Index))
		{
			Star::AddLog("[MeshStorage] -> LoadMesh() Failed.");
			meshStorageBuffers.pop_back();
			return false;
		}

		if (!meshStorageBuffer->SetupMesh())
		{
			Star::AddLog("[MeshStorage] -> SetupMesh() Failed.");
			meshStorageBuffers.pop_back();
			return false;
		}

		if (!meshStorageBuffer->CreateMaterial(path.c_str(), importer.GetScene(), _Index))
		{
			Star::AddLog("[MeshStorage] -> CreateMaterial() Error.");
			meshStorageBuffers.pop_back();
			return false;
		}

		std::string name = importer.GetScene()->mMeshes[_Index]->mName.C_Str();
		meshStorageBuffer->name = name;
		meshStorageBuffer->index = _Index;
		*_MeshStorageBuffer = meshStorageBuffer;
		return true;
	}

	return false;
}
bool ModelStorageBuffer::GetMesh(UINT _Index, MeshStorageBuffer** _MeshStorageBuffer)
{
	// find mesh
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->index == _Index)
		{
			*_MeshStorageBuffer = &(*it);
			return true;
		}
	}

	// mesh missing
	Star::AddLog("[MeshStorage] -> Mesh missing.");
	return false;
}
bool ModelStorageBuffer::ReleaseMesh(UINT _Index)
{
	// find mesh
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->index == _Index)
		{
			it->Release();
			meshStorageBuffers.erase(it);
			return true;
		}
	}

	// mesh missing
	Star::AddLog("[MeshStorage] -> Mesh missing.");
	return false;
}
void ModelStorageBuffer::ReleaseAllMeshes()
{
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		it->Release();
	}

	meshStorageBuffers.clear();
}
bool ModelStorageBuffer::IsMeshAlreadyLoaded(UINT _Index)
{
	// find model
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->index == _Index)
			return true;
	}

	// mesh missing
	return false;
}
void ModelStorageBuffer::OpenModel(const char* _Path, unsigned int flags)
{
	if (!importer.GetScene())
	{
		Star::AddLog("[MeshStorage] -> Opening model.. %s", _Path);
		importer.ReadFile(_Path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | flags);
	}
	else
	{
		Star::AddLog("[MeshStorage] -> Model is already opened.");
	}
}
void ModelStorageBuffer::CloseModel()
{
	if (importer.GetScene())
	{
		Star::AddLog("[MeshStorage] -> Closing model..");
		importer.FreeScene();
	}
	else
	{
		Star::AddLog("[MeshStorage] -> Model is already closed.");
	}
}

void MeshStorageBuffer::Release()
{
	if (!vertices.empty())
		vertices.clear();
	if (!indices.empty())
		indices.clear();

	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();
}
void MeshStorageBuffer::AddVertex(Vertex vertex)
{
	vertices.push_back(vertex);
}
void MeshStorageBuffer::AddIndex(UINT index)
{
	indices.push_back(index);
}
bool MeshStorageBuffer::SetupMesh()
{
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, &vertexBuffer)))
			return false;
	}

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, &indexBuffer)))
			return false;
	}

	return true;
}
bool MeshStorageBuffer::LoadMesh(const aiScene* _Scene, UINT _Index)
{
	if (_Scene->mNumMeshes <= _Index)
	{
		Star::AddLog("[MeshStorage] -> Out of range.");
		return false;
	}

	aiMesh* mesh = _Scene->mMeshes[_Index];

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
			vertex.textureCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.textureCoord = Vector2(0.0f, 0.0f);
		}
		
		AddVertex(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			AddIndex(face.mIndices[j]);
	}

	return true;
}
bool MeshStorageBuffer::CreateMaterial(const char* _Path, const aiScene* _Scene, UINT _Index)
{
	if (!_Scene)
	{
		Star::AddLog("[MeshStorage] -> Scene is null.");
		return false;
	}

	if (_Scene->mNumMeshes <= _Index)
	{
		Star::AddLog("[MeshStorage] -> Out of range.");
		return false;
	}

	aiMesh* mesh = _Scene->mMeshes[_Index];
	unsigned int materialIndex = mesh->mMaterialIndex;

	if (materialIndex >= _Scene->mNumMaterials)
	{
		Star::AddLog("[MeshStorage] -> Out of range.");
		return false;
	}

	aiMaterial* material = _Scene->mMaterials[materialIndex];
	if (material)
	{
		aiString name;
		aiString diffuse;
		material->Get(AI_MATKEY_NAME, name);
		material->GetTexture(aiTextureType_DIFFUSE, NULL, &diffuse);

		this->material.name = name.C_Str();
		this->material.diffuse = Star::GetParent(_Path) + "\\" + diffuse.C_Str();
	}

	return true;
}

MeshStorage* MeshStorage::GetSingleton()
{
	static MeshStorage meshStorage;
	return &meshStorage;
}
bool MeshStorage::IsModelAlreadyLoaded(const char* _Path)
{
	// find model
	std::list<ModelStorageBuffer>::iterator it;
	for (it = modelStorageBuffers.begin(); it != modelStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
			return true;
	}

	// model missing
	return false;
}
bool MeshStorage::ReleaseModel(const char* _Path)
{
	// find model
	std::list<ModelStorageBuffer>::iterator it;
	for (it = modelStorageBuffers.begin(); it != modelStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
		{
			it->ReleaseAllMeshes();
			modelStorageBuffers.erase(it);
			return true;
		}
	}

	// model missing
	Star::AddLog("[MeshStorage] -> Model missing.");
	return false;
}
void MeshStorage::ReleaseAllModels()
{
	std::list<ModelStorageBuffer>::iterator it;
	for (it = modelStorageBuffers.begin(); it != modelStorageBuffers.end(); ++it)
	{
		it->ReleaseAllMeshes();
	}

	modelStorageBuffers.clear();
}
bool MeshStorage::LoadModel(const char* _Path, ModelStorageBuffer** _ModelStorageBuffer, unsigned int flags)
{
	if (IsModelAlreadyLoaded(_Path))
	{
		Star::AddLog("[MeshStorage] -> Model is already loaded.");
		GetModel(_Path, _ModelStorageBuffer);
		return true;
	}
	else
	{
		modelStorageBuffers.emplace_back();
		ModelStorageBuffer* modelStorageBuffer = &modelStorageBuffers.back();
		modelStorageBuffer->OpenModel(_Path, flags);
		if (modelStorageBuffer->importer.GetScene())
		{
			modelStorageBuffer->path = _Path;
			UINT meshNum = modelStorageBuffer->importer.GetScene()->mNumMeshes;
			modelStorageBuffer->meshNum = meshNum;
			*_ModelStorageBuffer = modelStorageBuffer;
			return true;
		}
		else
		{
			Star::AddLog("[MeshStorage] -> Failed to open model.");
			modelStorageBuffers.pop_back();
			return false;
		}
	}

	return false;
}
bool MeshStorage::GetModel(const char* _Path, ModelStorageBuffer** _ModelStorageBuffer)
{
	// find model
	std::list<ModelStorageBuffer>::iterator it;
	for (it = modelStorageBuffers.begin(); it != modelStorageBuffers.end(); ++it)
	{
		if (it->path.compare(_Path) == 0)
		{
			*_ModelStorageBuffer = &(*it);
			return true;
		}
	}

	// model missing
	Star::AddLog("[MeshStorage] -> Model missing.");
	return false;
}
void MeshStorage::CloseAllOpenedModels()
{
	std::list<ModelStorageBuffer>::iterator it;
	for (it = modelStorageBuffers.begin(); it != modelStorageBuffers.end(); ++it)
	{
		it->CloseModel();
	}
}
bool MeshStorage::BuildMesh(const char* _Name, std::vector<Vertex>* vertices, std::vector<UINT>* indices, MeshStorageBuffer** _MeshStorageBuffer)
{
	if (!vertices)
		return false;
	if (!indices)
		return false;

	// release old mesh
	if (IsMeshAlreadyBuild(_Name))
		ReleaseMesh(_Name);

	// create new mesh
	meshStorageBuffers.emplace_back();
	MeshStorageBuffer* meshStorageBuffer = &meshStorageBuffers.back();

	meshStorageBuffer->vertices.resize(vertices->size());
	meshStorageBuffer->indices.resize(indices->size());
	std::copy(vertices->begin(), vertices->end(), meshStorageBuffer->vertices.begin());
	std::copy(indices->begin(), indices->end(), meshStorageBuffer->indices.begin());

	meshStorageBuffer->SetupMesh();
	meshStorageBuffer->name = _Name;
	*_MeshStorageBuffer = meshStorageBuffer;
	return true;
}
void MeshStorage::ReleaseAllMeshes()
{
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		it->Release();
	}

	meshStorageBuffers.clear();
}
bool MeshStorage::ReleaseMesh(const char* _Name)
{
	// find mesh
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->name.compare(_Name) == 0)
		{
			it->Release();
			meshStorageBuffers.erase(it);
			return true;
		}
	}

	// mesh missing
	Star::AddLog("[MeshStorage] -> Mesh missing.");
	return false;
}
bool MeshStorage::IsMeshAlreadyBuild(const char* _Name)
{
	// find mesh
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->name.compare(_Name) == 0)
			return true;
	}

	// mesh missing
	return false;
}
bool MeshStorage::GetMesh(const char* _Name, MeshStorageBuffer** _MeshStorageBuffer)
{
	// find mesh
	std::list<MeshStorageBuffer>::iterator it;
	for (it = meshStorageBuffers.begin(); it != meshStorageBuffers.end(); ++it)
	{
		if (it->name.compare(_Name) == 0)
		{
			*_MeshStorageBuffer = &(*it);
			return true;
		}
	}

	// mesh missing
	Star::AddLog("[MeshStorage] -> Mesh missing.");
	return false;
}