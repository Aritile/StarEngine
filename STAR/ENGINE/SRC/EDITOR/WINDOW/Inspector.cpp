#include "Inspector.h"

// entity
#include "../../ENTITY/Entity.h"

// components
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../ENTITY/COMPONENT/MeshComponent.h"
#include "../../ENTITY/COMPONENT/CameraComponent.h"
#include "../../ENTITY/COMPONENT/TextMeshComponent.h"
#include "../../ENTITY/COMPONENT/RigidDynamicComponent.h"
#include "../../ENTITY/COMPONENT/ImageComponent.h"
#include "../../ENTITY/COMPONENT/OpacityComponent.h"
#include "../../ENTITY/COMPONENT/RectangleComponent.h"
#include "../../ENTITY/COMPONENT/TextComponent.h"
#include "../../ENTITY/COMPONENT/ConvexColliderComponent.h"
#include "../../ENTITY/COMPONENT/TriangleColliderComponent.h"
#include "../../ENTITY/COMPONENT/PhysicsMaterialComponent.h"
#include "../../ENTITY/COMPONENT/RigidStaticComponent.h"

// stuff
#include "../../SYSTEM/ScriptingSystem.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../SYSTEM/PhysicsSystem.h"

InspectorWindow* InspectorWindow::GetSingleton()
{
	static InspectorWindow inspectorWindow;
	return &inspectorWindow;
}

///////////////////////////////////////////////////////////////

static Entity* ecs = Entity::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();

void InspectorWindow::Render()
{
	if (!renderInspector)
		return;

	ImGui::Begin("Entity");
	{
		if (ecs->selected != entt::null)
		{
			RenderComponent<GeneralComponent>(ecs->selected);
			RenderComponent<TransformComponent>(ecs->selected);
			RenderComponent<MeshComponent>(ecs->selected);
			RenderComponent<CameraComponent>(ecs->selected);
			RenderComponent<TextMeshComponent>(ecs->selected);
			RenderComponent<RigidDynamicComponent>(ecs->selected);
			RenderComponent<ScriptingComponent>(ecs->selected);
			RenderComponent<PhysicsComponent>(ecs->selected);
			RenderComponent<ImageComponent>(ecs->selected);
			RenderComponent<OpacityComponent>(ecs->selected);
			RenderComponent<RectangleComponent>(ecs->selected);
			RenderComponent<TextComponent>(ecs->selected);
			RenderComponent<ConvexColliderComponent>(ecs->selected);
			RenderComponent<TriangleColliderComponent>(ecs->selected);
			RenderComponent<PhysicsMaterialComponent>(ecs->selected);
			RenderComponent<RigidStaticComponent>(ecs->selected);
			RenderAdd();
		}
	}
	ImGui::End();
}

void InspectorWindow::RenderAdd()
{
	ImGui::Separator();

	if (ImGui::Button("Add"))
		ImGui::OpenPopup("addComp");

	if (ImGui::BeginPopup("addComp", ImGuiWindowFlags_NoMove))
	{
		ImGui::SeparatorText("Mesh");

		if (ImGui::Selectable("Mesh"))
		{
			if (!ecs->HasComponent<MeshComponent>(ecs->selected))
				ecs->AddComponent<MeshComponent>(ecs->selected);
		}
		if (ImGui::Selectable("TextMesh"))
		{
			if (!ecs->HasComponent<TextMeshComponent>(ecs->selected))
				ecs->AddComponent<TextMeshComponent>(ecs->selected);
		}
		ImGui::Separator();
		{
			if (ImGui::BeginMenu("UI"))
			{
				if (ImGui::MenuItem("Rectangle"))
					ecs->AddComponent<RectangleComponent>(ecs->selected);
				if (ImGui::MenuItem("Image"))
					ecs->AddComponent<ImageComponent>(ecs->selected);
				if (ImGui::MenuItem("Text"))
					ecs->AddComponent<TextComponent>(ecs->selected);
				ImGui::SeparatorText("Effects");
				if (ImGui::MenuItem("Opacity"))
					ecs->AddComponent<OpacityComponent>(ecs->selected);
				ImGui::EndMenu();
			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Camera"))
		{
			if (!ecs->HasComponent<CameraComponent>(ecs->selected))
				ecs->AddComponent<CameraComponent>(ecs->selected);
		}
		ImGui::SeparatorText("Physics");
		if (ImGui::Selectable("RigidStatic"))
		{
			if (!ecs->HasComponent<RigidStaticComponent>(ecs->selected))
			{
				ecs->AddComponent<RigidStaticComponent>(ecs->selected);
				ecs->GetComponent<RigidStaticComponent>(ecs->selected).CreateActor();
			}
		}
		if (ImGui::Selectable("RigidDynamic"))
		{
			if (!ecs->HasComponent<RigidDynamicComponent>(ecs->selected))
			{
				ecs->AddComponent<RigidDynamicComponent>(ecs->selected);
				ecs->GetComponent<RigidDynamicComponent>(ecs->selected).CreateActor();
			}
		}

		if (ImGui::Selectable("Material"))
		{
			if (!ecs->HasComponent<PhysicsMaterialComponent>(ecs->selected))
			{
				ecs->AddComponent<PhysicsMaterialComponent>(ecs->selected);
				ecs->GetComponent<PhysicsMaterialComponent>(ecs->selected).CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
			}
		}

		if (ImGui::BeginMenu("Collider"))
		{
			if (ImGui::MenuItem("Box"))
				ecs->GetComponent<PhysicsComponent>(ecs->selected).AddBoxCollider();
			if (ImGui::MenuItem("Sphere"))
				ecs->GetComponent<PhysicsComponent>(ecs->selected).AddSphereCollider();
			if (ImGui::MenuItem("Capsule"))
				ecs->GetComponent<PhysicsComponent>(ecs->selected).AddCapsuleCollider();
			if (ImGui::MenuItem("Convex"))
			{
				if (!ecs->HasComponent<ConvexColliderComponent>(ecs->selected))
				{
					ecs->AddComponent<ConvexColliderComponent>(ecs->selected);
					ecs->GetComponent<ConvexColliderComponent>(ecs->selected).Create();
				}
			}
			if (ImGui::MenuItem("Triangle"))
			{
				if (!ecs->HasComponent<TriangleColliderComponent>(ecs->selected))
				{
					ecs->AddComponent<TriangleColliderComponent>(ecs->selected);
					ecs->GetComponent<TriangleColliderComponent>(ecs->selected).Create();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		static std::vector<std::pair<std::string, std::string>> data;

		ImGui::SeparatorText("Lua");
		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::IsWindowAppearing())
				assetsWindow->GetFileNameFromProjectDir(assetsWindow->GetProjectDirPath(), LUA, data);
			for (size_t i = 0; i < data.size(); i++)
				if (ImGui::MenuItem(data[i].second.c_str()))
					ecs->registry.get<ScriptingComponent>(ecs->selected).AddScript(data[i].first.c_str());
			ImGui::EndMenu();
		}
		else
		{
			data.clear();
		}

		/****/
		ImGui::EndPopup();
	}
}

template <typename T>
void InspectorWindow::RenderComponent(entt::entity entity)
{
	if (ecs->registry.any_of<T>(entity))
		ecs->registry.get<T>(entity).Render();
}