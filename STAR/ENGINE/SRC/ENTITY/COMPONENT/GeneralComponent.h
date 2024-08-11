#pragma once

#include <string>
#include "entt/entt.hpp"
#include "../../IMGUI/imgui.h"
#include "../../ENTITY/Entity.h"
#include "../../STAR/Star.h"
#include "../../IMGUI/imgui_stdlib.h"
#include <yaml-cpp/yaml.h>
#include <sol/sol.hpp>

struct GeneralComponent
{
private:
	std::string nameEntity = "Entity";
	std::string tagEntity = "Empty";
	bool activeEntity = true;
	bool staticEntity = false;
	entt::entity parentEntity = entt::null;
	std::vector<entt::entity> childrenEntity;

public:
	void Render();

public:
	void SetName(std::string _Name);
	void SetTag(std::string _Tag);
	void SetActive(bool _Active);
	void SetStatic(bool _Static);
	void AddChild(entt::entity _Child);

public:
	bool HasChildren();
	std::string GetName();
	std::string GetTag();
	bool IsActive();
	bool IsStatic();
	entt::entity GetParent();
	std::vector<entt::entity>& GetChildren();
	void ClearVector();

private:
	void EntitySkip(entt::entity _Entity, int& _Skip);
	void SetActiveAll(entt::entity _Entity, bool _Active);
	void SetStaticAll(entt::entity _Entity, bool _Static);

public:
	void MoveUp();
	void MoveDown();

public:
	void SerializeComponent(YAML::Emitter& _Out);
	void DeserializeComponent(YAML::Node& _In);

private:
	std::vector<entt::entity> toDestroy;

public:
	static void LuaAdd(sol::state& state);
};