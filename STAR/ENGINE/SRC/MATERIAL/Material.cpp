#include "Material.h"
#include "../STAR/Star.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

void Material::SerializeMaterial(const char* path)
{
	if (!path)
		return;

	Star::AddLog("Serializing material.. %s", path);
	YAML::Emitter out;

	Star::BeginFormat(out);
	{
		out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
		{
			out << YAML::Key << "Name" << YAML::Value << name;
			out << YAML::Key << "Diffuse" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Path" << YAML::Value << diffuse;
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}
	Star::EndFormat(out);

	if (!out.good())
		Star::AddLog("%s", out.GetLastError().c_str());

	std::ofstream stream(path);
	stream << out.c_str();
	stream.close();
}
void Material::DeserializeMaterial(const char* path)
{
	if (!path)
		return;

	if (!Star::FileExist(path))
		return;

	Star::AddLog("Deserializing material.. %s", path);

	YAML::Node in = YAML::LoadFile(path);
	if (!Star::CheckSignature(in))
		return;

	YAML::Node _Material = in["Star"]["Data"]["Material"];
	name = _Material["Name"].as<std::string>();
	YAML::Node _Diffuse = _Material["Diffuse"];
	diffuse = _Diffuse["Path"].as<std::string>();
}