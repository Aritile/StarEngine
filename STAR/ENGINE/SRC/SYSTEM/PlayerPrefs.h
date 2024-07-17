#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <sol/sol.hpp>

class PlayerPrefs
{
public:
	static PlayerPrefs* GetSingleton();

public:
	void Test();

public:
	bool Load(const char* path);

public:
	void DeleteAll();
	void DeleteKey(const char* key);
	bool HasKey(const char* key);
	float GetFloat(const char* key);
	int GetInt(const char* key);
	std::string GetString(const char* key);
	void SetFloat(const char* key, float value);
	void SetInt(const char* key, int value);
	void SetString(const char* key, std::string value);
	bool Save(const char* path);

private:
	YAML::Emitter out; // save
	YAML::Node in; // load

public:
	static void LuaAdd(sol::state& state);
};