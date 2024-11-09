#include "ImageComponent.h"
#include "../../IMGUI/imgui.h"
#include "../../EDITOR/WINDOW/Assets.h"

static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();

void ImageComponent::Render()
{
	ImGui::Checkbox("##IMAGE", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("IMAGE", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("ImageComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Image");
			}
			ImGui::TableNextColumn();
			{
				if (textureStorageBuffer)
					ImGui::Image((void*)textureStorageBuffer->texture, ImVec2(100, 100));
				else
					ImGui::ImageButton((void*)assetsWindow->imageTexture, ImVec2(50, 50));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs* payload_n = (FILEs*)payload->Data;
						if (Star::ImageFormatCheck(payload_n->file_type.c_str()))
						{
							std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n->file_name;
							std::string exe = Star::GetParent(Star::GetExecutablePath());
							std::string x = Star::GetRelativePath(buffer, exe);

							textureStorage->LoadTexture(x.c_str(), &textureStorageBuffer);
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (textureStorageBuffer)
					ImGui::Text(Star::GetFileNameFromPath(textureStorageBuffer->path).c_str());
			}
			ImGui::EndTable();
		}
	}
}

TextureStorageBuffer* ImageComponent::GetTextureStorageBuffer()
{
	return textureStorageBuffer;
}