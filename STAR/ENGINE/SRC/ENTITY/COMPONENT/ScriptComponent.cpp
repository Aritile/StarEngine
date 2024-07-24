#include "ScriptComponent.h"
#include "../../HELPERS/Helpers.h"
#include "../../SYSTEM/ScriptingSystem.h"
#include "../../IMGUI/imgui.h"

static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();

void ScriptComponent::RecompileScript()
{
	const char* path = filePath.c_str();
	if (scriptingSystem->CompileScript(path))
		checksum = StarHelpers::GetChecksum(path);
}
void ScriptComponent::RecompileScriptsChecksum()
{
	if (StarHelpers::GetChecksum(filePath.c_str()) != checksum)
		RecompileScript();
}
void ScriptComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Script" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Name" << YAML::Value << fileName.c_str();
	out << YAML::Key << "Path" << YAML::Value << filePath.c_str();
	out << YAML::EndMap;
	out << YAML::EndMap;
}

void ScriptComponent::Render()
{
	ImGui::Checkbox("##SCRIPT", &activeComponent);
	ImGui::SameLine();

	std::string buffer = fileNameToUpper + " (SCRIPT)";
	if (ImGui::CollapsingHeader(buffer.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		/* code */
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Copy")) {}
		if (ImGui::MenuItem("Paste")) {}
		ImGui::Separator();
		if (ImGui::MenuItem("Remove"))
		{
			// you are normal?
			// this is not enough, you need also delete script from lua context
			//scripts.erase(scripts.begin() + i);
		}
		ImGui::EndPopup();
	}
}