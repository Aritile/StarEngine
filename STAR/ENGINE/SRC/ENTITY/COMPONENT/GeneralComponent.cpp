#include "GeneralComponent.h"
#include "../../ENTITY/COMPONENT/MeshComponent.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

static Entity* ecs = Entity::GetSingleton();

void GeneralComponent::Render()
{
	if (ImGui::CollapsingHeader("GENERAL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("GeneralComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Active");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Name");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Static");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Tag");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				bool _Active = IsActive();
				if (ImGui::Checkbox("##ActiveGeneralComponent", &_Active))
					if (_Active) SetActive(true); else SetActive(false);

				//ImVec2 size = ImGui::GetItemRectSize();
				//Star::AddLog("%f", size.y);

				std::string _Name = GetName();
				if (ImGui::InputText("##NameGeneralComponent", &_Name, ImGuiInputTextFlags_EnterReturnsTrue))
					SetName(_Name);

				bool _Static = IsStatic();
				if (ImGui::Checkbox("##StaticGeneralComponent", &_Static))
					if (_Static) SetStatic(true); else SetStatic(false);

				std::string _Tag = GetTag();
				if (ImGui::InputText("##TagGeneralComponent", &_Tag, ImGuiInputTextFlags_EnterReturnsTrue))
					SetTag(_Tag);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void GeneralComponent::SetName(std::string _Name)
{
	if (!_Name.compare(""))
		return;

	nameEntity = _Name;
}
void GeneralComponent::SetTag(std::string _Tag)
{
	if (!_Tag.compare(""))
		return;

	tagEntity = _Tag;
}
std::vector<entt::entity>& GeneralComponent::GetChildren()
{
	return childrenEntity;
}
entt::entity GeneralComponent::GetParent()
{
	return parentEntity;
}
bool GeneralComponent::IsStatic()
{
	return staticEntity;
}
bool GeneralComponent::IsActive()
{
	return activeEntity;
}
std::string GeneralComponent::GetTag()
{
	return tagEntity;
}
std::string GeneralComponent::GetName()
{
	return nameEntity;
}
bool GeneralComponent::HasChildren()
{
	if (childrenEntity.empty()) return true; else return false;
}

void GeneralComponent::SetActiveAll(entt::entity _Entity, bool _Active)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(_Entity);
	generalComp.activeEntity = _Active;

	for (int i = 0; i < generalComp.GetChildren().size(); i++)
		SetActiveAll(generalComp.GetChildren()[i], _Active);
}
void GeneralComponent::SetActive(bool _Active)
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	SetActiveAll(entity, _Active);
}
void GeneralComponent::SetStaticAll(entt::entity _Entity, bool _Static)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(_Entity);
	generalComp.staticEntity = _Static;

	for (int i = 0; i < generalComp.GetChildren().size(); i++)
		SetStaticAll(generalComp.GetChildren()[i], _Static);
}
void GeneralComponent::SetStatic(bool _Static)
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	SetStaticAll(entity, _Static);
}

void GeneralComponent::EntitySkip(entt::entity _Entity, int& _Skip)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(_Entity);
	entt::entity thisEntity = entt::to_entity(ecs->registry, *this);

	std::vector<entt::entity>::iterator find = std::find(generalComp.childrenEntity.begin(), generalComp.childrenEntity.end(), thisEntity);
	if (find != generalComp.childrenEntity.end())
		_Skip++;

	for (int i = 0; i < generalComp.childrenEntity.size(); i++)
		EntitySkip(generalComp.childrenEntity[i], _Skip);
}

void GeneralComponent::AddChild(entt::entity _Entity)
{
	auto& genComp = ecs->registry.get<GeneralComponent>(_Entity);

	//// if move root so return code ////
	if (_Entity == ecs->root)
		return;

	//// 2. issue fix ////
	if (genComp.parentEntity == entt::null)
	{
		genComp.parentEntity = entt::to_entity(ecs->registry, *this);
		childrenEntity.push_back(_Entity);
	}
	else
	{
		//// 3. issue fix ////
		int size = 0;
		EntitySkip(_Entity, size);
		if (size != 0)
			return;

		auto& parent = ecs->registry.get<GeneralComponent>(genComp.parentEntity);

		//// 4. issue fix ////
		std::vector<entt::entity>::iterator old = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), _Entity);
		if (old != parent.childrenEntity.end())
		{
			auto index = std::distance(parent.childrenEntity.begin(), old);
			parent.childrenEntity.erase(parent.childrenEntity.begin() + index);
		}

		//// 5. issue fix ////
		std::vector<entt::entity>::iterator back = std::find(childrenEntity.begin(), childrenEntity.end(), _Entity);
		if (back == childrenEntity.end())
		{
			genComp.parentEntity = entt::to_entity(ecs->registry, *this);
			childrenEntity.push_back(_Entity);
		}
	}
}

void GeneralComponent::MoveUp()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this); /* get entity from component */
	auto& parent = ecs->registry.get<GeneralComponent>(GetParent()); /* get parent from this entity */
	std::vector<entt::entity>::iterator iterator = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);
	if (iterator != parent.childrenEntity.end())
	{
		size_t index = std::distance(parent.childrenEntity.begin(), iterator);
		if (index == NULL) /* completely up so return */
			return;

		std::swap(parent.childrenEntity[index], parent.childrenEntity[index - 1]);
	}
}
void GeneralComponent::MoveDown()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this); /* get entity from component */
	auto& parent = ecs->registry.get<GeneralComponent>(GetParent()); /* get parent from this entity */
	std::vector<entt::entity>::iterator iterator = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);
	if (iterator != parent.childrenEntity.end())
	{
		size_t index = std::distance(parent.childrenEntity.begin(), iterator);
		if (index == parent.childrenEntity.size() - 1) /* completely down so return */
			return;

		std::swap(parent.childrenEntity[index], parent.childrenEntity[index + 1]);
	}
}

void GeneralComponent::SerializeComponent(YAML::Emitter& _Out)
{
	_Out << YAML::Key << "GeneralComponent";
	_Out << YAML::BeginMap;
	{
		_Out << YAML::Key << "Name" << YAML::Value << GetName();
		_Out << YAML::Key << "Tag" << YAML::Value << GetTag();
		_Out << YAML::Key << "Active" << YAML::Value << IsActive();
		_Out << YAML::Key << "Static" << YAML::Value << IsStatic();
	}
	_Out << YAML::EndMap;
}
void GeneralComponent::DeserializeComponent(YAML::Node& _In)
{
	YAML::Node generalComponent = _In["GeneralComponent"];
	if (generalComponent)
	{
		SetName(generalComponent["Name"].as<std::string>());
		SetTag(generalComponent["Tag"].as<std::string>());
		SetActive(generalComponent["Active"].as<bool>());
		SetStatic(generalComponent["Static"].as<bool>());
	}
}

void GeneralComponent::LuaAdd(sol::state& state)
{
	sol::usertype<GeneralComponent> component = state.new_usertype<GeneralComponent>(
		"GeneralComponent");
	component["SetName"] = &GeneralComponent::SetName;
	component["GetName"] = &GeneralComponent::GetName;
	component["SetTag"] = &GeneralComponent::SetTag;
	component["GetTag"] = &GeneralComponent::GetTag;
	component["SetActive"] = &GeneralComponent::SetActive;
	component["GetActive"] = &GeneralComponent::IsActive;
	component["SetStatic"] = &GeneralComponent::SetStatic;
	component["GetStatic"] = &GeneralComponent::IsStatic;
	component["MoveUp"] = &GeneralComponent::MoveUp;
	component["MoveDown"] = &GeneralComponent::MoveDown;
	component["GetParentEntity"] = &GeneralComponent::GetParent;
}