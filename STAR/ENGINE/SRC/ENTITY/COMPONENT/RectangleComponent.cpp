#include "RectangleComponent.h"

#include "../../IMGUI/imgui.h"

void RectangleComponent::Render()
{
	ImGui::Checkbox("##RECTANGLE", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("RECTANGLE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("RectangleComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Color");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				ImGui::ColorEdit3("##RectangleComponent", (float*)&color);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void RectangleComponent::SetActive(bool _Active)
{
	activeComponent = _Active;
}

bool RectangleComponent::IsActive()
{
	return activeComponent;
}

void RectangleComponent::SetColor(Vector3 _Color)
{
	color = _Color;
}

Vector3 RectangleComponent::GetColor()
{
	return color;
}