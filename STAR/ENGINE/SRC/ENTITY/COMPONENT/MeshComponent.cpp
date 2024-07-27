#include "MeshComponent.h"
#include "../../HELPERS/Helpers.h"
#include "../../EDITOR/WINDOW/Viewport.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../SYSTEM/ModelSystem.h"
#include "GeneralComponent.h"
#include <fstream>

static DX* dx = DX::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static ModelSystem* modelSystem = &ModelSystemClass();

void MeshComponent::Render()
{
	ImGui::Checkbox("##MESH", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("MESH", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				entt::entity entity = entt::to_entity(ecs->registry, *this);
				ecs->registry.get<MeshComponent>(entity).ClearCache();
				ecs->registry.remove<MeshComponent>(entity);
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("MeshComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Vertices");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Indices");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Faces");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Material");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Diffuse");
			}
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("%u", GetNumVertices());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("%u", indices.size());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("%u", GetNumFaces());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				if (meshMaterial.name.empty())
					ImGui::Button("None");
				else
					ImGui::Button(meshMaterial.name.c_str());
				/*
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs payload_n = *(FILEs*)payload->Data;
						if (payload_n.file_type == MAT)
						{
							std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
							AddMeshMaterial(buffer);
						}
					}
					ImGui::EndDragDropTarget();
				}
				*/

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				if (diffuse_texture)
					ImGui::Image((void*)diffuse_texture, ImVec2(100, 100));
				else
					ImGui::ImageButton((void*)assetsWindow->imageTexture, ImVec2(50, 50));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs payload_n = *(FILEs*)payload->Data;
						if (payload_n.file_type == PNG || payload_n.file_type == JPEG || payload_n.file_type == DDS)
						{
							std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
							std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
							std::string x = StarHelpers::GetRelativePath(buffer, exe);
							LoadTexture(x.c_str(), &diffuse_texture);

							if (meshMaterial.name.empty())
							{
								entt::entity entity = entt::to_entity(ecs->registry, *this);
								if (ecs->HasComponent<GeneralComponent>(entity))
								{
									auto& generalComponent = ecs->GetComponent<GeneralComponent>(entity);
									SetMaterialName(generalComponent.GetName().c_str());

									std::string string = "assets\\" + GetMaterialName() + ".mat";
									SetMaterialPath(string.c_str());
									SerializeMaterial(string.c_str());
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
				if (!meshMaterial.diffuse.empty())
					ImGui::Text(StarHelpers::GetFileNameFromPath(meshMaterial.diffuse).c_str());
			}
			ImGui::EndTable();
		}
	}
}

std::string MeshComponent::GetMaterialName()
{
	return meshMaterial.name;
}

void MeshComponent::AddVertices(Vertex add)
{
	vertices.push_back(add);
}
void MeshComponent::AddIndices(UINT add)
{
	indices.push_back(add);
}
UINT MeshComponent::GetNumVertices()
{
	return (UINT)vertices.size();
}
UINT MeshComponent::GetNumFaces()
{
	return (UINT)indices.size() / 3;
}
void MeshComponent::ClearCache()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
}

bool MeshComponent::SetupMesh()
{
	CreateBoundingBox();

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

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf())))
			return false;
	}

	//////////////////////////////////////////////////////////////

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

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf())))
			return false;
	}

	//////////////////////////////////////////////////////////////

	return true;
}

void MeshComponent::DrawMesh(DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	if (!modelSystem->pVS)
	{
		printf("pVS is null\n");
		return;
	}
	if (!modelSystem->pPS)
	{
		printf("pPS is null\n");
		return;
	}

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		auto& transComp = ecs->registry.get<TransformComponent>(entity);

		cb.sProjection = DirectX::XMMatrixTranspose(projection);
		cb.sView = DirectX::XMMatrixTranspose(view);
		cb.sModel = DirectX::XMMatrixTranspose(transComp.GetTransform());
		if (viewportWindow->GetRenderState() == RenderState::Pos)
			cb.renderState = 3;
		else if (viewportWindow->GetRenderState() == RenderState::Normal)
			cb.renderState = 4;
		else
			cb.renderState = 0;

		if (diffuse_texture) cb.hasTexture = true;
		else cb.hasTexture = false;

		dx->dxDeviceContext->UpdateSubresource(modelSystem->pConstantBuffer, 0, nullptr, &cb, 0, 0);
		dx->dxDeviceContext->VSSetShader(modelSystem->pVS, 0, 0);
		dx->dxDeviceContext->VSSetConstantBuffers(0, 1, &modelSystem->pConstantBuffer);
		dx->dxDeviceContext->PSSetShader(modelSystem->pPS, 0, 0);
		dx->dxDeviceContext->PSSetConstantBuffers(0, 1, &modelSystem->pConstantBuffer);
		dx->dxDeviceContext->PSSetSamplers(0, 1, &modelSystem->pSamplerState);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		dx->dxDeviceContext->IASetInputLayout(modelSystem->pLayout);
		dx->dxDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		dx->dxDeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dx->dxDeviceContext->PSSetShaderResources(0, 1, &diffuse_texture);

		dx->dxDeviceContext->DrawIndexed((UINT)indices.size(), 0, 0);
	}
}

void MeshComponent::SetActive(bool arg)
{
	activeComponent = arg;
}
bool MeshComponent::IsActive()
{
	return activeComponent;
}

///////////////////////////////////////////////////////////////

const std::vector<Vertex>& MeshComponent::GetVertices()
{
	return vertices;
}
const std::vector<UINT>& MeshComponent::GetIndices()
{
	return indices;
}

void MeshComponent::CreateBoundingBox()
{
	float min_x = 0.0f; float min_y = 0.0f; float min_z = 0.0f;
	float max_x = 0.0f; float max_y = 0.0f; float max_z = 0.0f;

	min_x = max_x = vertices.at(0).position.x;
	min_y = max_y = vertices.at(0).position.y;
	min_z = max_z = vertices.at(0).position.z;

	for (size_t i = 0; i < vertices.size(); i++)
	{
		// x-axis
		if (vertices.at(i).position.x < min_x)
			min_x = vertices.at(i).position.x;
		if (vertices.at(i).position.x > max_x)
			max_x = vertices.at(i).position.x;

		// y-axis
		if (vertices.at(i).position.y < min_y)
			min_y = vertices.at(i).position.y;
		if (vertices.at(i).position.y > max_y)
			max_y = vertices.at(i).position.y;

		// z-axis
		if (vertices.at(i).position.z < min_z)
			min_z = vertices.at(i).position.z;
		if (vertices.at(i).position.z > max_z)
			max_z = vertices.at(i).position.z;
	}

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		DirectX::BoundingBox boundingBox = DirectX::BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
		Vector3 min = Vector3(min_x, min_y, min_z);
		Vector3 max = Vector3(max_x, max_y, max_z);
		Vector3 center = (max + min) / 2.0f;
		boundingBox.Center = center;
		boundingBox.Extents = (max - min) / 2.0f;
		boundingBox.Extents.x = abs(boundingBox.Extents.x);
		boundingBox.Extents.y = abs(boundingBox.Extents.y);
		boundingBox.Extents.z = abs(boundingBox.Extents.z);
		ecs->registry.get<TransformComponent>(entity).SetBoundingBox(boundingBox);
	}
}

bool MeshComponent::GetState()
{
	if (vertexBuffer.Get() && indexBuffer.Get()) return true; return false;
}

/*
void MeshComponent::AddMeshMaterial(std::string path)
{
	// how this works? i forgot
	Material mat_buff;
	assetsWindow->OpenMaterialFile(path, mat_buff);
	material_name = std::filesystem::path(path).stem().string();
	path = std::filesystem::path(path).parent_path().string();

	while (true)
	{
		std::size_t found = mat_buff.diffuse.find("..\\");
		if (found != std::string::npos)
		{
			mat_buff.diffuse.erase(0, 3);
			path = std::filesystem::path(path).parent_path().string();
		}
		else
		{
			std::size_t found = mat_buff.diffuse.find("\\");
			if (found != std::string::npos)
			{
				std::string dir = mat_buff.diffuse;
				dir.resize(found);
				path.append("\\");
				path.append(dir);
				mat_buff.diffuse.erase(0, (found + 1));
			}
			else
			{
				break;
			}
		}
	}

	path.append("\\");
	path.append(mat_buff.diffuse);

	size_t pos = path.find_last_of(".");
	if (pos != -1)
	{
		std::string buffer = path.substr(pos);

		if (buffer == PNG || buffer == JPEG)
		{
			if (diffuse_texture) diffuse_texture->Release();
			DirectX::CreateWICTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				StarHelpers::ConvertString(path).c_str(),
				nullptr,
				&diffuse_texture);
		}

		if (buffer == DDS)
		{
			if (diffuse_texture) diffuse_texture->Release();
			DirectX::CreateDDSTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				StarHelpers::ConvertString(path).c_str(),
				nullptr,
				&diffuse_texture);
		}
	}
	else
	{
		StarHelpers::AddLog("Error!");
	}
}
*/

void MeshComponent::SerializeComponent(YAML::Emitter& out)
{
	// no texture? no material
	if (!meshMaterial.diffuse.empty())
		SerializeMaterial(materialPath.c_str());

	out << YAML::Key << "MeshComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "IsActive" << YAML::Value << IsActive();
		out << YAML::Key << "ModelPath" << YAML::Value << modelPath;
		out << YAML::Key << "MaterialPath" << YAML::Value << materialPath;
		out << YAML::Key << "MeshIndex" << YAML::Value << meshIndex;
		out << YAML::Key << "VerticesSize" << YAML::Value << vertices.size();
		out << YAML::Key << "IndicesSize" << YAML::Value << indices.size();
		out << YAML::Key << "FacesSize" << YAML::Value << GetNumFaces();
	}
	out << YAML::EndMap;
}
void MeshComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node meshComponent = in["MeshComponent"];
	if (meshComponent)
	{
		SetActive(meshComponent["IsActive"].as<bool>());
		modelPath = meshComponent["ModelPath"].as<std::string>();
		materialPath = meshComponent["MaterialPath"].as<std::string>();
		meshIndex = meshComponent["MeshIndex"].as<unsigned int>();
		//vertices.resize(meshComponent["VerticesSize"].as<size_t>());
		//indices.resize(meshComponent["IndicesSize"].as<size_t>());
	}
}

void MeshComponent::SetFileName(std::string name)
{
	fileName = name;
}
std::string MeshComponent::GetFileName()
{
	return fileName;
}
void MeshComponent::SetMeshName(std::string name)
{
	meshName = name;
}
std::string MeshComponent::GetMeshName()
{
	return meshName;
}

bool MeshComponent::LoadMesh(const aiScene* scene)
{
	if (scene->mNumMeshes <= meshIndex)
	{
		printf("out of range\n");
		return false;
	}

	aiMesh* mesh = scene->mMeshes[meshIndex];

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
		
		AddVertices(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			AddIndices(face.mIndices[j]);
	}

	return true;
}
bool MeshComponent::LoadMaterial(const aiScene* scene)
{
	if (scene->mNumMeshes <= meshIndex)
	{
		printf("out of range\n");
		return false;
	}

	aiMesh* mesh = scene->mMeshes[meshIndex];
	unsigned int materialIndex = mesh->mMaterialIndex;

	if (materialIndex >= scene->mNumMaterials)
	{
		printf("out of range\n");
		return false;
	}

	aiMaterial* material = scene->mMaterials[materialIndex];
	if (material)
	{
		aiString name;
		aiString diffuse;
		material->Get(AI_MATKEY_NAME, name);
		material->GetTexture(aiTextureType_DIFFUSE, NULL, &diffuse);

		meshMaterial.name = name.C_Str();
		meshMaterial.diffuse = diffuse.C_Str();
	}

	return true;
}
std::string MeshComponent::GetName(const aiScene* scene)
{
	if (!scene)
	{
		printf("scene is null\n");
		return "";
	}
	if (scene->mNumMeshes <= meshIndex)
	{
		printf("is out of range\n");
		return "";
	}

	aiMesh* mesh = scene->mMeshes[meshIndex];
	if (mesh)
		return mesh->mName.C_Str();
	else
		return "";
}
void MeshComponent::SerializeMaterial(const char* path)
{
	YAML::Emitter out;

	StarHelpers::BeginFormat(out);
	{
		out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
		{
			out << YAML::Key << "Name" << YAML::Value << meshMaterial.name;
			out << YAML::Key << "Diffuse" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Path" << YAML::Value << meshMaterial.diffuse;
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}
	StarHelpers::EndFormat(out);

	if (!out.good())
		StarHelpers::AddLog("%s", out.GetLastError().c_str());

	std::ofstream stream(path);
	stream << out.c_str();
	stream.close();
}
void MeshComponent::DeserializeMaterial(const char* path)
{
	YAML::Node in = YAML::LoadFile(path);
	if (!StarHelpers::CheckSignature(in))
		return;

	YAML::Node _Material = in["Star"]["Data"]["Material"];
	meshMaterial.name = _Material["Name"].as<std::string>();
	YAML::Node _Diffuse = _Material["Diffuse"];
	meshMaterial.diffuse = _Diffuse["Path"].as<std::string>();
}
bool MeshComponent::SetupMaterial(const char* path)
{
	std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
	std::string model = StarHelpers::GetParent(path);
	std::string dir = StarHelpers::GetRelativePath(model, exe);
	std::string full = dir + "\\" + meshMaterial.diffuse;
	meshMaterial.diffuse = full;
	
	LoadTexture(meshMaterial.diffuse.c_str(), &diffuse_texture);

	return true;
}
bool MeshComponent::LoadTexture(const char* path, ID3D11ShaderResourceView** shaderResourceView)
{
	std::string extension = StarHelpers::GetFileExtensionFromPath(path);

	if (extension.compare(PNG) == 0 || extension.compare(JPEG) == 0)
	{
		if (*shaderResourceView)
			(*shaderResourceView)->Release();

		if (FAILED(DirectX::CreateWICTextureFromFile(
			dx->dxDevice,
			dx->dxDeviceContext,
			StarHelpers::ConvertString(path).c_str(),
			nullptr,
			shaderResourceView)))
		{
			printf("failed to load texture\n");
			return false;
		}
		else
		{
			meshMaterial.diffuse = path;
			return true;
		}
	}
	else if (extension.compare(DDS) == 0)
	{
		if (*shaderResourceView)
			(*shaderResourceView)->Release();

		if (FAILED(DirectX::CreateDDSTextureFromFile(
			dx->dxDevice,
			dx->dxDeviceContext,
			StarHelpers::ConvertString(path).c_str(),
			nullptr,
			shaderResourceView)))
		{
			printf("failed to load texture\n");
			return false;
		}
		else
		{
			meshMaterial.diffuse = path;
			return true;
		}
	}

	printf("wrong texture format\n");
	return false;
}
void MeshComponent::SetMeshIndex(unsigned int index)
{
	meshIndex = index;
}
void MeshComponent::SetModelPath(const char* path)
{
	std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
	std::string model = path;
	modelPath = StarHelpers::GetRelativePath(model, exe);
}
void MeshComponent::SetupDiffuseTexture()
{
	std::string path = meshMaterial.diffuse;
	if (!path.empty())
		LoadTexture(meshMaterial.diffuse.c_str(), &diffuse_texture);
}
void MeshComponent::SetMaterialPath(const char* path)
{
	materialPath = path;
}
void MeshComponent::LoadDiffuseTexture(const char* path)
{
	if (!path)
		return;

	std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
	std::string model = StarHelpers::GetParent(path);
	std::string dir = StarHelpers::GetRelativePath(model, exe);
	std::string full = dir + "\\" + StarHelpers::GetFileNameFromPath(path) + StarHelpers::GetFileExtensionFromPath(path);

	LoadTexture(full.c_str(), &diffuse_texture);
}

void MeshComponent::LuaAdd(sol::state& state)
{
	sol::usertype<MeshComponent> component = state.new_usertype<MeshComponent>(
		"MeshComponent");
	component["GetNumVertices"] = &MeshComponent::GetNumVertices;
	component["GetNumFaces"] = &MeshComponent::GetNumFaces;
	component["AddVertices"] = &MeshComponent::AddVertices;
	component["AddIndices"] = &MeshComponent::AddIndices;
	component["SetupMesh"] = &MeshComponent::SetupMesh;

	sol::usertype<Vertex> vertex = state.new_usertype<Vertex>(
		"Vertex");
	vertex["position"] = &Vertex::position;
	vertex["normal"] = &Vertex::normal;
	vertex["texCoords"] = &Vertex::texCoords;
}

void MeshComponent::SetMaterialName(const char* name)
{
	meshMaterial.name = name;
}