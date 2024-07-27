#include "LuaEntity.h"
#include "../../ENTITY/Entity.h"

static Entity* ecs = Entity::GetSingleton();

void LuaEntity::LuaAdd(sol::state& state)
{
	sol::usertype<EntityX> entity = state.new_usertype<EntityX>(
		"Entity");
	entity["CreateEntity"] = &EntityX::CreateEntity;
	entity["AddComponent"] = &EntityX::AddComponent;
	entity["GetComponent"] = &EntityX::GetComponent;
	entity["HasComponent"] = &EntityX::HasComponent;
	entity["RemoveComponent"] = &EntityX::RemoveComponent;
}
EntityX LuaEntity::FindByName(const char* name)
{
	EntityX entityX;
	entityX.entity = ecs->FindByName(name, true);
	return entityX;
}
EntityX LuaEntity::FindByTag(const char* tag)
{
	EntityX entityX;
	entityX.entity = ecs->FindByTag(tag, true);
	return entityX;
}