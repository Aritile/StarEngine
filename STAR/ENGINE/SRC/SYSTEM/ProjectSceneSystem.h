#pragma once

#include <yaml-cpp/yaml.h>
#include <entt/entt.hpp>

class ProjectSceneSystem
{
public:
	void NewScene();
	void OpenScene(const char* path);
	void SaveScene();
	void SaveAsScene();
	void ClearScene();

private:
	void SaveFile(YAML::Emitter& out, const char* filename);
	void SerializeHierarchy(YAML::Emitter& out);
	void SerializeEntity(YAML::Emitter& out, entt::entity entity);
	void DeserializeHierarchy(YAML::Node& in);
	void DeserializeEntity(YAML::Node& in, entt::entity parent);

private:
	std::string save;

public:
	static ProjectSceneSystem* GetSingleton();
};