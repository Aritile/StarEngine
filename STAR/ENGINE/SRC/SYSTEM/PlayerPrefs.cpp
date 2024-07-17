#include "PlayerPrefs.h"
#include <fstream>
#include "../HELPERS/Helpers.h"

PlayerPrefs* PlayerPrefs::GetSingleton()
{
	static PlayerPrefs playerPrefs;
	return &playerPrefs;
}
void PlayerPrefs::Test()
{
	StarHelpers::BeginFormat(out);
	{
		SetFloat("float", 5.0f);
		SetInt("int", 5);
		SetString("string", "string");
	}
	StarHelpers::EndFormat(out);

	Save("PlayerPrefs");
}
bool PlayerPrefs::Load(const char* path)
{
	in = YAML::LoadFile(path);
	if (!StarHelpers::CheckSignature(in))
		return false;

	return true;
}
void PlayerPrefs::DeleteAll()
{
	in = YAML::Node();
}
void PlayerPrefs::DeleteKey(const char* key)
{
	if (HasKey(key))
		in.remove(key);
}
bool PlayerPrefs::HasKey(const char* key)
{
	if (in[key]) return true;
	else return false;
}
float PlayerPrefs::GetFloat(const char* key)
{
	if (HasKey(key))
		return in[key].as<float>();

	return 0.0f;
}
int PlayerPrefs::GetInt(const char* key)
{
	if (HasKey(key))
		return in[key].as<int>();

	return 0;
}
std::string PlayerPrefs::GetString(const char* key)
{
	if (HasKey(key))
		return in[key].as<std::string>();

	return "";
}
void PlayerPrefs::SetFloat(const char* key, float value)
{
	out << YAML::Key << key << YAML::Value << value;
}
void PlayerPrefs::SetInt(const char* key, int value)
{
	out << YAML::Key << key << YAML::Value << value;
}
void PlayerPrefs::SetString(const char* key, std::string value)
{
	out << YAML::Key << key << YAML::Value << value;
}
bool PlayerPrefs::Save(const char* path)
{
	if (!out.good())
		return false;

	std::ofstream stream(path);
	stream << out.c_str();
	stream.close();

	return true;
}
void PlayerPrefs::LuaAdd(sol::state& state)
{
	state["PlayerPrefs"] = sol::new_table();
	state["PlayerPrefs"]["DeleteAll"] = PlayerPrefs::DeleteAll;
	state["PlayerPrefs"]["DeleteKey"] = PlayerPrefs::DeleteKey;
	state["PlayerPrefs"]["HasKey"] = PlayerPrefs::HasKey;
	state["PlayerPrefs"]["GetFloat"] = PlayerPrefs::GetFloat;
	state["PlayerPrefs"]["GetInt"] = PlayerPrefs::GetInt;
	state["PlayerPrefs"]["GetString"] = PlayerPrefs::GetString;
	state["PlayerPrefs"]["SetFloat"] = PlayerPrefs::SetFloat;
	state["PlayerPrefs"]["SetInt"] = PlayerPrefs::SetInt;
	state["PlayerPrefs"]["SetString"] = PlayerPrefs::SetString;
}