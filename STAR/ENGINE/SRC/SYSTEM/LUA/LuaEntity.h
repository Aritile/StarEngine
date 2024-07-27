#pragma once

#include <sol/sol.hpp>
#include <entt/entt.hpp>

struct EntityX
{
	entt::entity entity = entt::null;

	void CreateEntity();
	void AddComponent(const char* component_name);
	sol::object GetComponent(const char* component_name);
	bool HasComponent(const char* component_name);
	void RemoveComponent(const char* component_name);
};

struct LuaEntity
{
	static void LuaAdd(sol::state& state);
	static EntityX FindByName(const char* name);
	static EntityX FindByTag(const char* tag);
};