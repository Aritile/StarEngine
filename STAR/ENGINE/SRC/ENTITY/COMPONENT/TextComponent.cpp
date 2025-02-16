#include "TextComponent.h"
#include "../../IMGUI/imgui.h"
#include "../../IMGUI/imgui_stdlib.h"
#include "../../DX/DX.h"
#include <sstream>
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../STORAGE/SpriteStorage.h"

static DX* dx = DX::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static SpriteStorage* spriteStorage = SpriteStorage::GetSingleton();

void TextComponent::Render()
{
	ImGui::Checkbox("##TEXT", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("TEXT", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("TextComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Text");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Font");
				//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				//ImGui::Text("Position");
				//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				//ImGui::Text("Rotation");
				//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				//ImGui::Text("Scale");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Origin");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Color");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				static std::string temp = textData;
				ImGui::InputTextMultiline("##InputTextComponent", &temp, ImVec2(0.0f, 22.0f));
				if (ImGui::IsItemDeactivated())
					textData = temp;

				if (items.empty())
					items.push_back(std::pair<std::string, std::string>("None", "None"));

				const char* combo_preview_value = items.at(item_index).second.c_str();
				if (ImGui::BeginCombo("ComboTextComponent", combo_preview_value))
				{
					for (int n = 0; n < items.size(); n++)
					{
						const bool is_selected = (item_index == n);
						if (ImGui::Selectable(items.at(n).second.c_str(), is_selected))
						{
							item_index = n;
							if (items.at(n).first != "None")
							{
								spriteBatch = spriteStorage->GetSpriteBatch();
								spriteFont = spriteStorage->LoadSpriteFont(items.at(n).first.c_str());
							}
							else
							{
								spriteBatch = nullptr;
								spriteFont = nullptr;
							}
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::IsItemClicked())
				{
					items.clear();
					items.push_back(std::pair<std::string, std::string>("None", "None"));
					assetsWindow->GetFileNameFromProjectDir(assetsWindow->GetProjectDirPath(), SPRITEFONT, items);
				}

				//ImGui::DragFloat2("##PositionTextComponent", (float*)&position, 0.1f);
				//ImGui::DragFloat("##RotationTextComponent", &rotation, 0.1f);
				//ImGui::DragFloat("##ScaleTextComponent", &scale, 0.1f);
				ImGui::DragFloat2("##OriginTextComponent", (float*)&origin, 0.1f);
				ImGui::ColorEdit4("##ColorTextComponent", (float*)&color, 0.1f);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}
void TextComponent::SetActive(bool _Active)
{
	activeComponent = _Active;
}
bool TextComponent::IsActive()
{
	return activeComponent;
}
void TextComponent::Release()
{
	if (spriteBatch)
	{
		delete spriteBatch;
		spriteBatch = nullptr;
	}

	if (spriteFont)
	{
		delete spriteFont;
		spriteFont = nullptr;
	}
}
void TextComponent::RenderText()
{
	if (!activeComponent)
		return;

	if (!spriteBatch)
		return;
	if (!spriteFont)
		return;

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);

	std::wstringstream _wstringstream;
	_wstringstream << textData.c_str();

	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch,
		                   _wstringstream.str().c_str(),
		                   transformComponent.GetPosition(),
		                   color,
		                   transformComponent.GetRotationYawPitchRoll().x,
		                   origin,
		                   transformComponent.GetScale().x);
	spriteBatch->End();
}