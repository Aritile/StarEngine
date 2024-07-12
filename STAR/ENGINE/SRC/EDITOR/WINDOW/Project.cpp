#include "Project.h"
#include "../../IMGUI/imgui.h"
#include "../../IMGUI/imgui_stdlib.h"

void ProjectWindow::Render()
{
	ImGui::SetNextWindowSize(ImVec2(387, 292));
	ImGui::Begin("Project", NULL);
	{
		// Left
		{
			ImGui::BeginChild("Recent", ImVec2(150, 0));
			{
				ImGui::Text("Recent");
				ImGui::Separator();
				ImGui::BeginChild("ProjectList", ImVec2(0, 200));
				{
					ImGui::Selectable("Project 1");
					ImGui::Selectable("Project 2");
					ImGui::Selectable("Project 3");
					ImGui::Selectable("Project 4");
					ImGui::Button("Change");
				}
				ImGui::EndChild();
				ImGui::Separator();
				ImGui::Button("Open");
				ImGui::SameLine();
				ImGui::Button("Remove");
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right
		{
			ImGui::BeginGroup();
			{
				ImGui::BeginChild("Project", ImVec2(0, 0));
				{
					static std::string lol;
					ImGui::Text("Folder");
					ImGui::PushItemWidth(150);
					ImGui::InputText("##Folder", &lol);
					ImGui::SameLine();
					ImGui::Button("Change");
					static std::string xd;
					ImGui::Text("Name");
					ImGui::InputText("##Name", &xd);
					ImGui::PopItemWidth();
					ImGui::Button("Create");
					ImGui::SameLine();
					ImGui::Button("Open");
				}
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}

			/*
			ImGui::Text("Recent");
			ImGui::Separator(); static std::string lol;
			ImGui::Text("Folder");
			ImGui::PushItemWidth(150);
			ImGui::InputText("##Folder", &lol);
			ImGui::SameLine();
			ImGui::Button("Change");
			static std::string xd;
			ImGui::Text("Name");
			ImGui::InputText("##Name", &xd);
			ImGui::PopItemWidth();
			ImGui::Button("Create");
			ImGui::SameLine();
			ImGui::Button("Open");
			ImGui::BeginChild("ProjectList");
			{
				ImGui::Selectable("Project 1");
				ImGui::Selectable("Project 2");
				ImGui::Selectable("Project 3");
				ImGui::Selectable("Project 4");
			}
			ImGui::EndChild();
			ImGui::Separator();
			ImGui::Button("Open");
			ImGui::SameLine();
			ImGui::Button("Remove");
		}
		ImGui::SameLine();
		ImGui::BeginChild("Project");
		{
			static std::string lol;
			ImGui::Text("Folder");
			ImGui::PushItemWidth(150);
			ImGui::InputText("##Folder", &lol);
			ImGui::SameLine();
			ImGui::Button("Change");
			static std::string xd;
			ImGui::Text("Name");
			ImGui::InputText("##Name", &xd);
			ImGui::PopItemWidth();
			ImGui::Button("Create");
			ImGui::SameLine();
			ImGui::Button("Open");
		}
		ImGui::EndChild();
		*/

		/*
		if (ImGui::BeginTable("CameraComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::Text("Recent");
				ImGui::Separator();
				ImGui::BeginChild("ProjectList", ImVec2(100, 200));
				{
					ImGui::Selectable("Project 1");
					ImGui::Selectable("Project 2");
					ImGui::Selectable("Project 3");
					ImGui::Selectable("Project 4");
				}
				ImGui::EndChild();
				ImGui::Separator();
				ImGui::Button("Open");
				ImGui::SameLine();
				ImGui::Button("Remove");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				static std::string lol;
				ImGui::Text("Folder");
				ImGui::PushItemWidth(150);
				ImGui::InputText("##Folder", &lol);
				ImGui::SameLine();
				ImGui::Button("Change");
				static std::string xd;
				ImGui::Text("Name");
				ImGui::InputText("##Name", &xd);
				ImGui::PopItemWidth();
				ImGui::Button("Create");
				ImGui::SameLine();
				ImGui::Button("Open");
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
		*/
		}
		
	
	ImGui::End();
}

ProjectWindow* ProjectWindow::GetSingleton()
{
	static ProjectWindow projectWindow;
	return &projectWindow;
}