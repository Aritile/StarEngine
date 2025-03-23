#include "DistanceCullerComponent.h"
#include "../Entity.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

using namespace DirectX::SimpleMath;

static Entity* ecs = Entity::GetSingleton();

bool DistanceCullerComponent::IsValidDistance(Vector3 _CameraPos)
{
	constexpr float epsilon = 0.0001f;

	// get entity position
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	auto& posComp = ecs->GetComponent<TransformComponent>(entity);
	Vector3 pos = posComp.GetPosition();

	// do not recalculate
	if (Vector3::DistanceSquared(oldCamPos, _CameraPos) < epsilon)
	{
		// do not recalculate
		if (Vector3::DistanceSquared(oldPos, pos) < epsilon)
			return oldValidDistance;
	}

	// calculate squared distance
	float squaredDistance = (_CameraPos - pos).LengthSquared();
	float squaredCullDistance = distance * distance;

	// save previous values
	oldCamPos = _CameraPos;
	oldPos = pos;

	// return result
	if (reverse)
		oldValidDistance = squaredDistance >= squaredCullDistance;
	else
		oldValidDistance = squaredDistance <= squaredCullDistance;
	return oldValidDistance;
}

void DistanceCullerComponent::Render()
{
	ImGui::Checkbox("##DISTANCECULLER", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("DISTANCECULLER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("DistanceCullerComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Distance");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Reverse");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				ImGui::DragFloat("##Distance", &distance, 0.1f);
				ImGui::Checkbox("##Reverse", &reverse);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}