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

public:
	void RecompileScript();
	void RecompileScriptsChecksum();
	void Render();

public:
	void SerializeComponent(YAML::Emitter& out);
};