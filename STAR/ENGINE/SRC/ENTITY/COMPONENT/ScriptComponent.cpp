#include "ScriptComponent.h"
#include "../../STAR/Star.h"
#include "../../SYSTEM/ScriptingSystem.h"
#include "../../IMGUI/imgui.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../GAME/Game.h"

static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static Game* game = Game::GetSingleton();

void ScriptComponent::RecompileScript()
{
	const char* path = filePath.c_str();
	if (scriptingSystem->CompileScript(path))
		checksum = Star::GetChecksum(path);
}
void ScriptComponent::RecompileScriptsChecksum()
{
	if (Star::GetChecksum(filePath.c_str()) != checksum)
		RecompileScript();
}
void ScriptComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Script" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "IsActive" << YAML::Value << activeComponent;
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
bool ScriptComponent::CallLuaFunction(const char* _FunctionName)
{
	// get function
	sol::function function = scriptingSystem->GetState()[fileName.c_str()][_FunctionName];
	if (function)
	{
		// call function
		sol::protected_function_result result = function();

		// get result
		if (!result.valid())
		{
			sol::error error = result;
			consoleWindow->AddErrorMessage("%s", error.what());
			game->StopGame();
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}
bool ScriptComponent::LuaFunctionIsValid(const char* _FunctionName)
{
	sol::optional<sol::object> maybe_object = scriptingSystem->GetState()[fileName.c_str()][_FunctionName];
	if (maybe_object)
		if (maybe_object.value().is<sol::function>())
			return true;

	return false;
}
bool ScriptComponent::IsActive()
{
	return activeComponent;
}