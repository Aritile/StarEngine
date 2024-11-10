#include "OpacityComponent.h"
#include "../../IMGUI/imgui.h"

void OpacityComponent::SetOpacity(float _Opacity)
{
	opacity = _Opacity;
}
float OpacityComponent::GetOpacity()
{
	return opacity;
}
void OpacityComponent::Render()
{
	if (ImGui::CollapsingHeader("OPACITY", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("OpacityComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Opacity");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				ImGui::DragFloat("##OpacityComponent", &opacity, 0.1f, 0.0f, 1.0f);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}