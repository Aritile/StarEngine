#pragma once

#include <string>
#include <zlib/zlib.h>
#include <yaml-cpp/yaml.h>

// this is not component for entity, do not use AddComponent
struct ScriptComponent
{
public:
	std::string filePath;
	std::string fileName;
	std::string fileNameToUpper;
	uLong checksum = 0;

public:
	bool activeComponent = true;
	bool IsActive();

public:
	void RecompileScript();
	void RecompileScriptsChecksum();
	void Render();

	bool CallLuaFunction(const char* _FunctionName);
	bool LuaFunctionIsValid(const char* _FunctionName);

public:
	void SerializeComponent(YAML::Emitter& out);
};