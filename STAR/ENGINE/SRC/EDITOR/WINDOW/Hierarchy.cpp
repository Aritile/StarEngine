#include "Hierarchy.h"

#include "../../ENTITY/Entity.h"
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static AssimpLoader* assimpLoader = AssimpLoader::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();

HierarchyWindow* HierarchyWindow::GetSingleton()
{
	static HierarchyWindow hierarchyWindow;
	return &hierarchyWindow;
}

///////////////////////////////////////////////////////////////

static Entity* ecs = Entity::GetSingleton();

void HierarchyWindow::Render()
{
	if (!renderHierarchy)
		return;

	ImGui::Begin("Hierarchy");
	{
		auto view = ecs->registry.view<GeneralComponent>();

		for (auto entity : view)
		{
			if (entity == ecs->root)
				RenderTree(entity);
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				ecs->selected = entt::null;
	}
	ImGui::End();
}

void HierarchyWindow::RenderTree(entt::entity entity)
{
	if (!ecs->IsValid(entity))
		return;

	auto& generalComponent = ecs->registry.get<GeneralComponent>(entity);

	///////////////////////////////////////////////////////////

	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_DefaultOpen;

	///////////////////////////////////////////////////////////

	if (generalComponent.HasChildren())
		tree_flags |= ImGuiTreeNodeFlags_Leaf;

	if (ecs->selected == entity)
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	///////////////////////////////////////////////////////////

	if (!generalComponent.IsActive())
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)entity, tree_flags, generalComponent.GetName().c_str());

	if (!generalComponent.IsActive())
		ImGui::PopStyleColor(1);

	///////////////////////////////////////////////////////////

	static const char* type = "DND_DEMO_CELL";

	//// DRAG ////
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(type, &entity, sizeof(entt::entity));
		ImGui::Text(generalComponent.GetName().c_str());
		ImGui::EndDragDropSource();
	}

	//// DROP ENTITY ////
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
		{
			entt::entity payload_n = *(entt::entity*)payload->Data;
			generalComponent.AddChild(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	//// DROP ASS ////
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
		{
			FILEs payload_n = *(FILEs*)payload->Data;
			if (payload_n.file_type == OBJ || payload_n.file_type == FBX)
			{
				std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
				std::string exe = Star::GetParent(Star::GetExecutablePath());
				std::string x = Star::GetRelativePath(buffer, exe);

				assimpLoader->LoadModel(x.c_str(), entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	///////////////////////////////////////////////////////////

	if (ImGui::IsItemClicked() && entity != ecs->root)
		ecs->selected = entity;

	if (ImGui::IsItemHovered())
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			viewportWindow->LookAtEntity(entity);

	///////////////////////////////////////////////////////////

	if (node_open)
	{
		for (size_t i = 0; i < generalComponent.GetChildren().size(); i++)
			RenderTree(generalComponent.GetChildren()[i]);
		ImGui::TreePop();
	}
}