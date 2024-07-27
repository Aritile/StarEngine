#include "ScriptingSystem.h"
#include "../HELPERS/Helpers.h"
#include "../EDITOR/WINDOW/Console.h"
#include <fstream>
#include <filesystem>
#include "../EDITOR/WINDOW/Assets.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../GAME/Game.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../ENTITY/COMPONENT/TextMeshComponent.h"
#include "ProjectSceneSystem.h"
#include "PlayerPrefs.h"
#include "PhysicsSystem.h"

#include "LUA/LuaBoundingBox.h"
#include "LUA/LuaConsole.h"
#include "LUA/LuaInput.h"
#include "LUA/LuaMatrix.h"
#include "LUA/LuaQuaternion.h"
#include "LUA/LuaTime.h"
#include "LUA/LuaVector2.h"
#include "LUA/LuaVector3.h"
#include "LUA/LuaVector4.h"
#include "LUA/LuaEngine.h"
#include "LUA/LuaEntity.h"

#define INCLUDE_LUASOCKET
#define COMPONENT_ERROR "Failed to get %s because it was not found!"

ScriptingSystem* ScriptingSystem::GetSingleton()
{
	static ScriptingSystem scriptingSystem;
	return &scriptingSystem;
}

/* ----------------------------------- */

static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static Game* game = Game::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static ProjectSceneSystem* projectSceneSystem = ProjectSceneSystem::GetSingleton();
static PlayerPrefs* playerPrefs = PlayerPrefs::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

bool ScriptingSystem::Init()
{
	/* libs */
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::package);
	lua.open_libraries(sol::lib::coroutine);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::os);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::debug);
	lua.open_libraries(sol::lib::bit32);
	lua.open_libraries(sol::lib::io);

#if defined INCLUDE_LUASOCKET
	std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
	std::string path = R"(package.path = package.path .. ';)" + exe + R"(/data/lua/luasocket/LDIR/?.lua')";
	std::string cpath = R"(package.cpath = package.cpath .. ';)" + exe + R"(/data/lua/luasocket/CDIR/?.dll')";

	path = StarHelpers::SlashesFix(path);
	cpath = StarHelpers::SlashesFix(cpath);

	lua.script(path);
	lua.script(cpath);
#endif

	LuaBoundingBox::LuaAdd(lua);
	LuaConsole::LuaAdd(lua);
	LuaInput::LuaAdd(lua);
	LuaMatrix::LuaAdd(lua);
	LuaQuaternion::LuaAdd(lua);
	LuaTime::LuaAdd(lua);
	LuaVector2::LuaAdd(lua);
	LuaVector3::LuaAdd(lua);
	LuaVector4::LuaAdd(lua);
	LuaEngine::LuaAdd(lua);
	LuaEntity::LuaAdd(lua);

	GeneralComponent::LuaAdd(lua);
	TransformComponent::LuaAdd(lua);
	CameraComponent::LuaAdd(lua);
	RigidBodyComponent::LuaAdd(lua);
	MeshComponent::LuaAdd(lua);
	TextMeshComponent::LuaAdd(lua);
	PlayerPrefs::LuaAdd(lua);

	return true;
}
void ScriptingComponent::AddScript(const char* path)
{
	std::filesystem::path file(path);
	if (file.extension().string() != LUA)
		return;

	for (size_t i = 0; i < scripts.size(); i++)
		if (scripts[i].fileName == file.stem().string())
			return;

	scriptingSystem->CompileScript(path);
	ScriptComponent scriptComponent;

	{
		scriptComponent.checksum = StarHelpers::GetChecksum(path);

		// old
		//scriptBuffer.filePath = path;

		// new
		std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
		std::string model = StarHelpers::GetParent(path);
		std::string dir = StarHelpers::GetRelativePath(model, exe);
		std::string full = dir + "\\" + StarHelpers::GetFileNameFromPath(path) + StarHelpers::GetFileExtensionFromPath(path);
		scriptComponent.filePath = full;

		scriptComponent.fileName = file.stem().string();
		scriptComponent.fileNameToUpper = scriptComponent.fileName;
		std::transform(
			scriptComponent.fileNameToUpper.begin(),
			scriptComponent.fileNameToUpper.end(),
			scriptComponent.fileNameToUpper.begin(),
			[](unsigned char c) { return std::toupper(c); });
	}
	scripts.push_back(scriptComponent);
}
void ScriptingComponent::Render()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			scripts[i].Render();
		}
		ImGui::PopID();
	}
}
void ScriptingComponent::lua_call_start()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["Start"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::lua_call_update()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["Update"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::RecompileScriptsChecksum()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].RecompileScriptsChecksum();
	}
}
bool ScriptingSystem::CompileScript(const char* filename)
{
	sol::load_result result = scriptingSystem->GetState().load_file(filename);
	if (!result.valid())
	{
		sol::error error = result;
		consoleWindow->AddErrorMessage("%s", error.what());
		return false;
	}
	else
	{
		//printf("CompileScript()\n");
		scriptingSystem->GetState().script_file(filename);
		return true;
	}

	return false;
}
void ScriptingSystem::CreateScript(const char* filename, const char* name)
{
	std::string buffer = std::string(name) + " = {}\n"
		"\n"
		"-- Use this for initialization\n"
		"function " + std::string(name) + ":Start()\n"
		"end\n"
		"\n"
		"-- Update is called once per frame\n"
		"function " + std::string(name) + ":Update()\n"
		"end";

	std::ofstream script;
	script.open(filename);
	script << buffer;
	script.close();
}
void ScriptingComponent::RecompileScripts()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].RecompileScript();
	}
}
void ScriptingComponent::lua_add_entity_from_component()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	EntityX entityX; entityX.entity = entity;
	scriptingSystem->GetState().set("entity", entityX);
}
sol::state& ScriptingSystem::GetState()
{
	return lua;
}

void EntityX::CreateEntity()
{
	entity = ecs->CreateEntity();
	ecs->GetComponent<GeneralComponent>(ecs->root).AddChild(entity);
}
void EntityX::AddComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to add component because entity is not valid");
		return;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (!ecs->HasComponent<MeshComponent>(entity))
			ecs->AddComponent<MeshComponent>(entity);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (!ecs->HasComponent<TextMeshComponent>(entity))
			ecs->AddComponent<TextMeshComponent>(entity);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (!ecs->HasComponent<CameraComponent>(entity))
			ecs->AddComponent<CameraComponent>(entity);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (!ecs->HasComponent<RigidBodyComponent>(entity))
		{
			ecs->AddComponent<RigidBodyComponent>(entity);
			ecs->GetComponent<RigidBodyComponent>(entity).CreateActor();
		}
	}
	// START TESTING
	else if (strcmp(component_name, "BoxColliderComponent") == 0)
	{
		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			physicsComponent.AddBoxCollider();
		}
	}
	else if (strcmp(component_name, "SphereColliderComponent") == 0)
	{
		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			physicsComponent.AddSphereCollider();
		}
	}
	// END
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}
sol::object EntityX::GetComponent(const char* component_name)
{
	sol::object component;

	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to get component because entity is not valid");
		return component;
	}

	if (strcmp(component_name, "GeneralComponent") == 0)
	{
		if (ecs->HasComponent<GeneralComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<GeneralComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TransformComponent") == 0)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TransformComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (ecs->HasComponent<MeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<MeshComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (ecs->HasComponent<TextMeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TextMeshComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (ecs->HasComponent<CameraComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<CameraComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (ecs->HasComponent<RigidBodyComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<RigidBodyComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	// START TESTING
	else if (strcmp(component_name, "BoxColliderComponent") == 0)
	{
		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			if (!physicsComponent.GetBoxColliders()->empty())
				component = sol::make_object(scriptingSystem->GetState(), &physicsComponent.GetBoxColliders()[0]);
		}
	}
	else if (strcmp(component_name, "SphereColliderComponent") == 0)
	{
		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			if (!physicsComponent.GetSphereColliders()->empty())
				component = sol::make_object(scriptingSystem->GetState(), &physicsComponent.GetSphereColliders()[0]);
		}
	}
	// END
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return component;
}
bool EntityX::HasComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to check if entity has component because entity is not valid");
		return false;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
		return ecs->HasComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		return ecs->HasComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		return ecs->HasComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		return ecs->HasComponent<RigidBodyComponent>(entity);
	// START TESTING
	else if (strcmp(component_name, "BoxColliderComponent") == 0)
		return !ecs->GetComponent<PhysicsComponent>(entity).GetBoxColliders()->empty();
	else if (strcmp(component_name, "SphereColliderComponent") == 0)
		return !ecs->GetComponent<PhysicsComponent>(entity).GetSphereColliders()->empty();
	// END
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return false;
}
void EntityX::RemoveComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to remove component because entity is not valid");
		return;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
		ecs->RemoveComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		ecs->RemoveComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		ecs->RemoveComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		ecs->RemoveComponent<RigidBodyComponent>(entity);
	// START TESTING
	else if (strcmp(component_name, "BoxColliderComponent") == 0)
		ecs->GetComponent<PhysicsComponent>(entity).GetBoxColliders()->clear();
	else if (strcmp(component_name, "SphereColliderComponent") == 0)
		ecs->GetComponent<PhysicsComponent>(entity).GetSphereColliders()->clear();
	// END
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}

void ScriptingComponent::SerializeComponent(YAML::Emitter& out)
{
	if (scripts.empty())
		return;

	out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].SerializeComponent(out);
	}
	out << YAML::EndSeq;
}
void ScriptingComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node scripts = in["Scripts"];
	if (scripts)
	{
		for (const auto& script : scripts)
		{
			YAML::Node scriptNode = script["Script"];
			if (scriptNode)
			{
				AddScript(scriptNode["Path"].as<std::string>().c_str());
				this->scripts.back().activeComponent = scriptNode["IsActive"].as<bool>();
			}
		}
	}
}
void ScriptingComponent::lua_call_late_update()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["LateUpdate"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::lua_call_fixed_update()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["FixedUpdate"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}