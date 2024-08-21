#include "MeshComponent.h"
#include "../../STAR/Star.h"
#include "../../EDITOR/WINDOW/Viewport.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../SYSTEM/ModelSystem.h"
#include "GeneralComponent.h"
#include <fstream>
#include "../../STORAGE/TextureStorage.h"

static DX* dx = DX::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static ModelSystem* modelSystem = &ModelSystemClass();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();

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
				//entt::entity entity = entt::to_entity(ecs->registry, *this);
				//ecs->registry.get<MeshComponent>(entity).ClearCache();
				//ecs->registry.remove<MeshComponent>(entity);
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
				ImGui::Text("%u", GetNumIndices());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("%u", GetNumFaces());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);

				if (meshStorageBuffer)
				{
					if (meshStorageBuffer->material.name.empty())
						ImGui::Button("None");
					else
						ImGui::Button(meshStorageBuffer->material.name.c_str());
				}
				else
					ImGui::Button("None");

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);

				bool render = true;
				if (meshStorageBuffer)
				{
					if (meshStorageBuffer->material.diffuseTexture)
					{
						if (meshStorageBuffer->material.diffuseTexture->texture)
						{
							ImGui::Image((void*)meshStorageBuffer->material.diffuseTexture->texture, ImVec2(100, 100));
							render = false;
						}
					}
				}
				if (render)
					ImGui::ImageButton((void*)assetsWindow->imageTexture, ImVec2(50, 50));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs* payload_n = (FILEs*)payload->Data;
						if (Star::ImageFormatCheck(payload_n->file_type.c_str()))
						{
							entt::entity entity = entt::to_entity(ecs->registry, *this);
							std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n->file_name;
							std::string exe = Star::GetParent(Star::GetExecutablePath());
							std::string x = Star::GetRelativePath(buffer, exe);

							if (meshStorageBuffer)
							{
								textureStorage->LoadTexture(x.c_str(), &meshStorageBuffer->material.diffuseTexture);
								meshStorageBuffer->material.diffuse = x;

								if (ecs->HasComponent<GeneralComponent>(entity))
								{
									auto& generalComponent = ecs->GetComponent<GeneralComponent>(entity);
									if (meshStorageBuffer->material.name.empty())
										meshStorageBuffer->material.name = generalComponent.GetName();
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
				if (meshStorageBuffer)
					if (!meshStorageBuffer->material.diffuse.empty())
						ImGui::Text(Star::GetFileNameFromPath(meshStorageBuffer->material.diffuse).c_str());
			}
			ImGui::EndTable();
		}
	}
}

void MeshComponent::DrawMesh(DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	if (!meshStorageBuffer)
	{
		return;
	}
	if (!modelSystem->pVS)
	{
		printf("VS is null\n");
		return;
	}
	if (!modelSystem->pPS)
	{
		printf("PS is null\n");
		return;
	}

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		auto& transComp = ecs->registry.get<TransformComponent>(entity);

		cb.sProjection = DirectX::XMMatrixTranspose(projection);
		cb.sView = DirectX::XMMatrixTranspose(view);
		cb.sModel = DirectX::XMMatrixTranspose(transComp.GetTransform());
		if (viewportWindow->GetRenderState() == RenderState::Position)
			cb.renderState = 3;
		else if (viewportWindow->GetRenderState() == RenderState::Normal)
			cb.renderState = 4;
		else
			cb.renderState = 0;

		if (meshStorageBuffer->material.diffuseTexture)
			if (meshStorageBuffer->material.diffuseTexture->texture) cb.hasTexture = true;
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
		dx->dxDeviceContext->IASetVertexBuffers(0, 1, &meshStorageBuffer->vertexBuffer, &stride, &offset);
		dx->dxDeviceContext->IASetIndexBuffer(meshStorageBuffer->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		if (meshStorageBuffer->material.diffuseTexture)
			if (meshStorageBuffer->material.diffuseTexture->texture)
				dx->dxDeviceContext->PSSetShaderResources(0, 1, &meshStorageBuffer->material.diffuseTexture->texture);
		dx->dxDeviceContext->DrawIndexed((UINT)meshStorageBuffer->indices.size(), 0, 0);

		dx->UnbindAll(0, 1);
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

void MeshComponent::CreateBoundingBox()
{
	if (!meshStorageBuffer)
	{
		Star::AddLog("Mesh Storage Buffer is null.");
		return;
	}

	float min_x = 0.0f; float min_y = 0.0f; float min_z = 0.0f;
	float max_x = 0.0f; float max_y = 0.0f; float max_z = 0.0f;

	min_x = max_x = meshStorageBuffer->vertices.at(0).position.x;
	min_y = max_y = meshStorageBuffer->vertices.at(0).position.y;
	min_z = max_z = meshStorageBuffer->vertices.at(0).position.z;

	for (size_t i = 0; i < meshStorageBuffer->vertices.size(); i++)
	{
		// x-axis
		if (meshStorageBuffer->vertices.at(i).position.x < min_x)
			min_x = meshStorageBuffer->vertices.at(i).position.x;
		if (meshStorageBuffer->vertices.at(i).position.x > max_x)
			max_x = meshStorageBuffer->vertices.at(i).position.x;

		// y-axis
		if (meshStorageBuffer->vertices.at(i).position.y < min_y)
			min_y = meshStorageBuffer->vertices.at(i).position.y;
		if (meshStorageBuffer->vertices.at(i).position.y > max_y)
			max_y = meshStorageBuffer->vertices.at(i).position.y;

		// z-axis
		if (meshStorageBuffer->vertices.at(i).position.z < min_z)
			min_z = meshStorageBuffer->vertices.at(i).position.z;
		if (meshStorageBuffer->vertices.at(i).position.z > max_z)
			max_z = meshStorageBuffer->vertices.at(i).position.z;
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

void MeshComponent::SerializeComponent(YAML::Emitter& out)
{
	if (!meshStorageBuffer)
		return;

	// no texture? no material
	if (!meshStorageBuffer->material.diffuse.empty())
		meshStorageBuffer->material.SerializeMaterial(GetMaterialPath().c_str());

	out << YAML::Key << "MeshComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "IsActive" << YAML::Value << IsActive();
		out << YAML::Key << "ModelPath" << YAML::Value << modelStorageBuffer->path;
		out << YAML::Key << "MaterialPath" << YAML::Value << GetMaterialPath().c_str();
		out << YAML::Key << "MeshIndex" << YAML::Value << meshStorageBuffer->index;
		out << YAML::Key << "VerticesSize" << YAML::Value << meshStorageBuffer->vertices.size();
		out << YAML::Key << "IndicesSize" << YAML::Value << meshStorageBuffer->indices.size();
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
		std::string path = meshComponent["ModelPath"].as<std::string>();
		UINT index = meshComponent["MeshIndex"].as<UINT>();
		std::string mat = meshComponent["MaterialPath"].as<std::string>();

		// load model
		ModelStorageBuffer* modelStorageBuffer = nullptr;
		meshStorage->LoadModel(path.c_str(), &modelStorageBuffer);

		// load mesh
		MeshStorageBuffer* meshStorageBuffer = nullptr;
		modelStorageBuffer->LoadMesh(index, &meshStorageBuffer);

		// load material
		meshStorageBuffer->material.DeserializeMaterial(mat.c_str());

		// load texture
		TextureStorageBuffer* textureStorageBuffer = nullptr;
		textureStorage->LoadTexture(meshStorageBuffer->material.diffuse.c_str(), &textureStorageBuffer);
		meshStorageBuffer->material.diffuseTexture = textureStorageBuffer;

		ApplyModel(modelStorageBuffer);
		ApplyMesh(meshStorageBuffer);
	}
}

void MeshComponent::LuaAdd(sol::state& state)
{
	sol::usertype<MeshComponent> component = state.new_usertype<MeshComponent>(
		"MeshComponent");
	component["GetNumVertices"] = &MeshComponent::GetNumVertices;
	component["GetNumIndices"] = &MeshComponent::GetNumIndices;
	component["GetNumFaces"] = &MeshComponent::GetNumFaces;

	sol::usertype<Vertex> vertex = state.new_usertype<Vertex>(
		"Vertex");
	vertex["position"] = &Vertex::position;
	vertex["normal"] = &Vertex::normal;
	vertex["textureCoord"] = &Vertex::textureCoord;
}
void MeshComponent::ApplyModel(ModelStorageBuffer* _ModelStorageBuffer)
{
	modelStorageBuffer = _ModelStorageBuffer;
}
void MeshComponent::ApplyMesh(MeshStorageBuffer* _MeshStorageBuffer)
{
	meshStorageBuffer = _MeshStorageBuffer;
}
UINT MeshComponent::GetNumVertices()
{
	if (meshStorageBuffer)
		return (UINT)meshStorageBuffer->vertices.size();

	return 0;
}
UINT MeshComponent::GetNumIndices()
{
	if (meshStorageBuffer)
		return (UINT)meshStorageBuffer->indices.size();

	return 0;
}
UINT MeshComponent::GetNumFaces()
{
	if (meshStorageBuffer)
		return (UINT)(meshStorageBuffer->indices.size() / 3);

	return 0;
}
std::string MeshComponent::GetMaterialPath()
{
	if (!modelStorageBuffer)
		return "";

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<GeneralComponent>(entity))
	{
		auto& generalComponent = ecs->GetComponent<GeneralComponent>(entity);
		std::string path;
		path += Star::GetParent(modelStorageBuffer->path);
		path += "\\";
		path += Star::GetFileNameFromPath(modelStorageBuffer->path);
		path += "\\";
		path += generalComponent.GetName();
		path += ".mat";
		return path;
	}

	return "";
}