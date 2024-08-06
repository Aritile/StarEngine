#include "Settings.h"
#include "../../IMGUI/imgui.h"
#include "../../SYSTEM/PlayerPrefs.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../STRDX/Widgets.h"
#include <fstream>
#include "../../STAR/Star.h"
#include <filesystem>

static PlayerPrefs* playerPrefs = PlayerPrefs::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();
static Widgets* widgets = Widgets::GetSingleton();
ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();

SettingsWindow* SettingsWindow::GetSingleton()
{
	static SettingsWindow settingsWindow;
	return &settingsWindow;
}

void SettingsWindow::Render()
{
	if (!renderSettings) return;

	ImGui::Begin("Settings", &renderSettings);
	{
        // Left
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
            ImGui::BeginChild("left pane", ImVec2(150, 0), true);
            for (size_t i = 0; i < list.size(); i++)
            {
                if (ImGui::Selectable(list[i].c_str(), selected == i))
                    selected = i;
            }
            ImGui::EndChild();
            ImGui::PopStyleVar(2);
        }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view");
            {
                if (selected == 0)
                    RenderGrid();
                if (selected == 1)
                    RenderCamera();
                if (selected == 2)
                    RenderPlayerPrefs();
                if (selected == 3)
                    RenderPhysics();
				if (selected == 4)
					RenderLuaEditor();
            }
            ImGui::EndChild();
            ImGui::EndGroup();
        }
	}
	ImGui::End();
}

void SettingsWindow::Init()
{
    list.push_back("Grid");
    list.push_back("Camera");
    list.push_back("PlayerPrefs");
    list.push_back("Physics");
    list.push_back("Lua Editor");
}

void SettingsWindow::RenderGrid()
{
    ImGui::SeparatorText("Grid");
	if (ImGui::BeginTable("Grid", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Render");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Position");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Size");
		}
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			bool render = widgets->GetRenderGrid();
			Vector3 setPos = widgets->GetGridPos();
			int setSize = widgets->GetGridSize();

			if (ImGui::Checkbox("##RenderGrid", &render))
				widgets->SetRenderGrid(render);
			if (ImGui::DragFloat3("##PosGrid", (float*)&setPos, 1.0f))
				widgets->SetGridPos(setPos);
			if (ImGui::DragInt("##SizeGrid", &setSize, 1, 0, 100))
				widgets->SetGridSize(setSize);
		}
		ImGui::PopItemWidth();
		ImGui::EndTable();
	}
}
void SettingsWindow::RenderCamera()
{
    ImGui::SeparatorText("Camera");
	if (ImGui::BeginTable("Camera", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Near");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Far");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Fov");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Speed");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Boost Speed");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Position Lerp");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
			ImGui::Text("Rotation Lerp");
		}
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			float _near = viewportWindow->GetNear();
			float _far = viewportWindow->GetFar();
			float _fov = viewportWindow->GetFov();
			float _speed = viewportWindow->GetSpeed();
			float _boostSpeed = viewportWindow->GetBoostSpeed();
			float _posLerp = viewportWindow->GetPosLerp();
			float _rotLerp = viewportWindow->GetRotLerp();

			if (ImGui::DragFloat("##NearCamera", &_near, 0.1f, 0.1f, FLT_MAX))
				viewportWindow->SetNear(_near);
			if (ImGui::DragFloat("##FarCamera", &_far, 1.0f, 1.0f, FLT_MAX))
				viewportWindow->SetFar(_far);
			if (ImGui::DragFloat("##FovCamera", &_fov, 1.0f, 1.0f, 180.0f))
				viewportWindow->SetFov(_fov);
			if (ImGui::DragFloat("##SpeedCamera", &_speed, 1.0f, 0.0f, FLT_MAX))
				viewportWindow->SetSpeed(_speed);
			if (ImGui::DragFloat("##BoostSpeedCamera", &_boostSpeed, 1.0f, 0.0f, FLT_MAX))
				viewportWindow->SetBoostSpeed(_boostSpeed);
			if (ImGui::DragFloat("##PosLerpCamera", &_posLerp, 1.0f, 0.0f, FLT_MAX))
				viewportWindow->SetPosLerp(_posLerp);
			if (ImGui::DragFloat("##RotLerpCamera", &_rotLerp, 1.0f, 0.0f, FLT_MAX))
				viewportWindow->SetRotLerp(_rotLerp);
		}
		ImGui::PopItemWidth();
		ImGui::EndTable();
	}
}
void SettingsWindow::RenderPlayerPrefs()
{
    ImGui::SeparatorText("PlayerPrefs");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
	ImGui::Text("All data will be deleted.");
	ImGui::PopStyleColor();
	if (ImGui::BeginTable("PlayerPrefs", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Delete All");
		}
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			if (ImGui::Button("Process"))
			{
				playerPrefs->DeleteAll();
				playerPrefs->Save();
			}
		}
		ImGui::PopItemWidth();
		ImGui::EndTable();
	}
}
void SettingsWindow::RenderPhysics()
{
    ImGui::SeparatorText("Physics");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
	ImGui::Text("Restart program to apply settings.");
	ImGui::PopStyleColor();
	if (ImGui::BeginTable("Physics", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Use GPU");
		}
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			PhysicsProcesor physicsProcesor = physicsSystem->GetProcesor();
			bool procesor = false;
			if (physicsProcesor == PhysicsProcesor::xCPU) procesor = false;
			if (physicsProcesor == PhysicsProcesor::xGPU) procesor = true;
			if (ImGui::Checkbox("##Use GPU", &procesor))
			{
				if (procesor == false) physicsSystem->SetProcesor(xCPU);
				if (procesor == true) physicsSystem->SetProcesor(xGPU);
			}
		}
		ImGui::PopItemWidth();
		ImGui::EndTable();
	}
}

void SettingsWindow::Save()
{
	YAML::Emitter out;

	Star::BeginFormat(out);
	{
		out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
		{
			out << YAML::Key << "Grid" << YAML::Value << YAML::BeginMap;
			{
				bool render = widgets->GetRenderGrid();
				Vector3 setPos = widgets->GetGridPos();
				int setSize = widgets->GetGridSize();

				out << YAML::Key << "Render" << YAML::Value << render;
				out << YAML::Key << "Position"; Star::SerializeVector3(out, setPos);
				out << YAML::Key << "Size" << YAML::Value << setSize;
			}
			out << YAML::EndMap;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
			{
				float _near = viewportWindow->GetNear();
				float _far = viewportWindow->GetFar();
				float _fov = viewportWindow->GetFov();
				float _speed = viewportWindow->GetSpeed();
				float _boostSpeed = viewportWindow->GetBoostSpeed();
				float _posLerp = viewportWindow->GetPosLerp();
				float _rotLerp = viewportWindow->GetRotLerp();

				out << YAML::Key << "Near" << YAML::Value << _near;
				out << YAML::Key << "Far" << YAML::Value << _far;
				out << YAML::Key << "Fov" << YAML::Value << _fov;
				out << YAML::Key << "Speed" << YAML::Value << _speed;
				out << YAML::Key << "BoostSpeed" << YAML::Value << _boostSpeed;
				out << YAML::Key << "PositionLerp" << YAML::Value << _posLerp;
				out << YAML::Key << "RotationLerp" << YAML::Value << _rotLerp;
			}
			out << YAML::EndMap;

			out << YAML::Key << "Physics" << YAML::Value << YAML::BeginMap;
			{
				PhysicsProcesor physicsProcesor = physicsSystem->GetProcesor();
				out << YAML::Key << "UseGpu" << YAML::Value << (physicsProcesor == xCPU ? false : true);
			}
			out << YAML::EndMap;

			out << YAML::Key << "LuaEditor" << YAML::Value << YAML::BeginMap;
			{
				if (programSelected == 0)
					out << YAML::Key << "Program" << YAML::Value << programList[0];
				else if (programSelected == 1)
					out << YAML::Key << "Program" << YAML::Value << programList[1];
				else if (programSelected == 2)
					out << YAML::Key << "Program" << YAML::Value << programList[2];
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}
	Star::EndFormat(out);

	if (!out.good())
		return;
	std::ofstream stream(filename);
	stream << out.c_str();
	stream.close();
}
void SettingsWindow::Load()
{
	if (!std::filesystem::exists(filename))
		return;

	YAML::Node in = YAML::LoadFile(filename);
	if (!Star::CheckSignature(in))
		return;

	YAML::Node settings = in["Star"]["Data"]["Settings"];

	if (settings["Grid"])
	{
		YAML::Node grid = settings["Grid"];
		widgets->SetRenderGrid(grid["Render"].as<bool>());
		auto position = grid["Position"];
		widgets->SetGridPos(Star::DeserializeVector3(position));
		widgets->SetGridSize(grid["Size"].as<int>());
	}

	if (settings["Camera"])
	{
		YAML::Node camera = settings["Camera"];
		viewportWindow->SetNear(camera["Near"].as<float>());
		viewportWindow->SetFar(camera["Far"].as<float>());
		viewportWindow->SetFov(camera["Fov"].as<float>());
		viewportWindow->SetSpeed(camera["Speed"].as<float>());
		viewportWindow->SetBoostSpeed(camera["BoostSpeed"].as<float>());
		viewportWindow->SetPosLerp(camera["PositionLerp"].as<float>());
		viewportWindow->SetRotLerp(camera["RotationLerp"].as<float>());
	}

	if (settings["Physics"])
	{
		YAML::Node physics = settings["Physics"];
		physicsSystem->SetProcesor(physics["UseGpu"].as<bool>() == false ? xCPU : xGPU);
	}

	if (settings["LuaEditor"])
	{
		YAML::Node luaEditor = settings["LuaEditor"];
		std::string program = luaEditor["Program"].as<std::string>();
		if (program.compare(programList[0]) == 0) programSelected = 0;
		else if (program.compare(programList[1]) == 0) programSelected = 1;
		else if (program.compare(programList[2]) == 0) programSelected = 2;
	}
}
void SettingsWindow::RenderLuaEditor()
{
	ImGui::SeparatorText("Lua Editor");
	if (ImGui::BeginTable("Lua Editor", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Select Program");
		}
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		{
			ImGui::Combo("##SelectProgramLuaEditor", &programSelected, programList, ARRAYSIZE(programList));
		}
		ImGui::PopItemWidth();
		ImGui::EndTable();
	}
}
int SettingsWindow::GetEditorProgram()
{
	return programSelected;
}