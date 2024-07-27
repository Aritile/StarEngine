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

class ScriptingSystem
{
public:
	bool Init();
	sol::state& GetState();
	void CreateScript(const char* filename, const char* name);
	bool CompileScript(const char* filename);

private:
	sol::state lua;

private:

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
	void lua_call_late_update();
	void lua_call_fixed_update();

private:
	void lua_add_entity_from_component();

private:
	std::vector<ScriptComponent> scripts;

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};