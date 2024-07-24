#pragma once

//#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
//#define SOL_EXCEPTIONS_ALWAYS_UNSAFE 1
//#define SOL_EXCEPTIONS 1

#include <sol/sol.hpp>
#include <entt/entt.hpp>
#include <zlib/zlib.h>
#include <yaml-cpp/yaml.h>
#include "../ENTITY/COMPONENT/ScriptComponent.h"

struct EntityX
{
	entt::entity entity = entt::null;

	void CreateEntity();
	void AddComponent(const char* component_name);
	sol::object GetComponent(const char* component_name);
	bool HasComponent(const char* component_name);
	void RemoveComponent(const char* component_name);
};
class ScriptingSystem
{
public:
	bool Init();
	sol::state& GetState();
	void CreateScript(const char* filename, const char* name);
	bool CompileScript(const char* filename);

private:
	EntityX FindByName(const char* name);
	EntityX FindByTag(const char* tag);

private:
	sol::state lua;

private:
	/* system */
	void lua_add_vector2();
	void lua_add_vector3();
	void lua_add_vector4();
	void lua_add_quaternion();
	void lua_add_matrix();
	void lua_add_console();
	void lua_add_time();
	void lua_add_input();
	void lua_add_bounding_box();

	/* entity */
	void lua_add_entity();
	void lua_add_engine();

public:
	static ScriptingSystem* GetSingleton();
};

struct ScriptingComponent
{
public:
	void Render();
	void AddScript(const char* path);
	void RecompileScripts();
	void RecompileScriptsChecksum();

public:
	void lua_call_start();
	void lua_call_update();

private:
	void lua_add_entity_from_component();

private:
	std::vector<ScriptComponent> scripts;

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};