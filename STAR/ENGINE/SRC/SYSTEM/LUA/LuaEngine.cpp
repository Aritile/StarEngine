#include "LuaEngine.h"
#include "../ProjectSceneSystem.h"
#include "LuaEntity.h"

static ProjectSceneSystem* projectSceneSystem = ProjectSceneSystem::GetSingleton();

void LuaEngine::LuaAdd(sol::state& state)
{
	state["Engine"] = sol::new_table();
	state["Engine"]["FindByName"] = [](const char* name) { return LuaEntity::FindByName(name); };
	state["Engine"]["FindByTag"] = [](const char* tag) { return LuaEntity::FindByTag(tag); };
	state["Engine"]["LoadScene"] = [](const char* path) { projectSceneSystem->OpenScene(path); };
}