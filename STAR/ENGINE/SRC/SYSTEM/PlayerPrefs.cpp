#include "PlayerPrefs.h"
#include <fstream>
#include "../HELPERS/Helpers.h"

#define READ_MODE "rb" // read binary
#define WRITE_MODE "wb" // write binary

bool PlayerPrefs::WriteToFile(const void* _Buffer, size_t _Size, std::FILE* _File)
{
	if (!_File)
		return false;

	fwrite(_Buffer, 1, _Size, _File);
	if (ferror(_File))
		return false;

	return true;
}
bool PlayerPrefs::WriteFloat(KeyX* _Float, std::FILE* _File)
{
	if (!_File)
		return false;

	float keyValue = std::get<float>(_Float->keyValue);
	uint32_t keyLength = _Float->keyName.length();
	const char* keyName = _Float->keyName.c_str();

	WriteToFile(&keyValue, sizeof(keyValue), _File);
	WriteToFile(&keyLength, sizeof(keyLength), _File);
	WriteToFile(keyName, keyLength, _File);

	return true;
}
bool PlayerPrefs::WriteInt(KeyX* _Int, std::FILE* _File)
{
	if (!_File)
		return false;

	int keyValue = std::get<int>(_Int->keyValue);
	uint32_t keyLength = _Int->keyName.length();
	const char* keyName = _Int->keyName.c_str();

	WriteToFile(&keyValue, sizeof(keyValue), _File);
	WriteToFile(&keyLength, sizeof(keyLength), _File);
	WriteToFile(keyName, keyLength, _File);

	return true;
}
bool PlayerPrefs::WriteString(KeyX* _String, std::FILE* _File)
{
	if (!_File)
		return false;

	uint32_t valueLength = std::get<std::string>(_String->keyValue).length();
	const char* valueName = std::get<std::string>(_String->keyValue).c_str();
	uint32_t keyLength = _String->keyName.length();
	const char* keyName = _String->keyName.c_str();

	WriteToFile(&valueLength, sizeof(valueLength), _File);
	WriteToFile(valueName, valueLength, _File);
	WriteToFile(&keyLength, sizeof(keyLength), _File);
	WriteToFile(keyName, keyLength, _File);

	return true;
}
PlayerPrefs* PlayerPrefs::GetSingleton()
{
	static PlayerPrefs playerPrefs;
	return &playerPrefs;
}
bool PlayerPrefs::Load()
{
	std::FILE* file = std::fopen(filename, READ_MODE);
	{
		if (!file)
			return false;

		ReadFromFile(&numFloat, sizeof(numFloat), file);
		ReadFromFile(&numInt, sizeof(numInt), file);
		ReadFromFile(&numString, sizeof(numString), file);

		for (size_t i = 0; i < numFloat; i++)
		{
			KeyX key;
			ReadFloat(&key, file);
			keyX.push_back(key);
		}

		for (size_t i = 0; i < numInt; i++)
		{
			KeyX key;
			ReadInt(&key, file);
			keyX.push_back(key);
		}

		for (size_t i = 0; i < numString; i++)
		{
			KeyX key;
			ReadString(&key, file);
			keyX.push_back(key);
		}
	}
	std::fclose(file);
	return true;
}
void PlayerPrefs::DeleteAll()
{
	if (!keyX.empty())
	{
		keyX.clear();
		numFloat = 0;
		numInt = 0;
		numString = 0;
	}
}
bool PlayerPrefs::DeleteKey(const char* key)
{
	for (size_t i = 0; i < keyX.size(); i++)
		if (keyX[i].keyName.compare(key) == 0)
		{
			if (std::holds_alternative<float>(keyX[i].keyValue)) numFloat--;
			if (std::holds_alternative<int>(keyX[i].keyValue)) numInt--;
			if (std::holds_alternative<std::string>(keyX[i].keyValue)) numString--;

			keyX.erase(keyX.begin() + i);
		}

	return true;
}
bool PlayerPrefs::HasKey(const char* key)
{
	for (size_t i = 0; i < keyX.size(); i++)
		if (keyX[i].keyName.compare(key) == 0)
			return true;

	return false;
}
float PlayerPrefs::GetFloat(const char* key)
{
	for (size_t i = 0; i < keyX.size(); i++)
		if (std::holds_alternative<float>(keyX[i].keyValue))
			if (keyX[i].keyName.compare(key) == 0)
				return std::get<float>(keyX[i].keyValue);

	return 0.0f;
}
int PlayerPrefs::GetInt(const char* key)
{
	for (size_t i = 0; i < keyX.size(); i++)
		if (std::holds_alternative<int>(keyX[i].keyValue))
			if (keyX[i].keyName.compare(key) == 0)
				return std::get<int>(keyX[i].keyValue);

	return 0;
}
std::string PlayerPrefs::GetString(const char* key)
{
	for (size_t i = 0; i < keyX.size(); i++)
		if (std::holds_alternative<std::string>(keyX[i].keyValue))
			if (keyX[i].keyName.compare(key) == 0)
				return std::get<std::string>(keyX[i].keyValue);

	return "";
}
void PlayerPrefs::SetFloat(const char* key, float value)
{
	if (HasKey(key))
		DeleteKey(key);
	keyX.push_back(KeyX(key, value));
	numFloat++;
}
void PlayerPrefs::SetInt(const char* key, int value)
{
	if (HasKey(key))
		DeleteKey(key);
	keyX.push_back(KeyX(key, value));
	numInt++;
}
void PlayerPrefs::SetString(const char* key, std::string value)
{
	if (HasKey(key))
		DeleteKey(key);
	keyX.push_back(KeyX(key, value));
	numString++;
}
bool PlayerPrefs::Save()
{
	std::FILE* file = std::fopen(filename, WRITE_MODE);
	{
		if (!file)
			return false;

		WriteToFile(&numFloat, sizeof(numFloat), file);
		WriteToFile(&numInt, sizeof(numInt), file);
		WriteToFile(&numString, sizeof(numString), file);
		
		for (size_t i = 0; i < keyX.size(); i++)
		{
			if (std::holds_alternative<float>(keyX[i].keyValue))
				WriteFloat(&keyX[i], file);
			if (std::holds_alternative<int>(keyX[i].keyValue))
				WriteInt(&keyX[i], file);
			if (std::holds_alternative<std::string>(keyX[i].keyValue))
				WriteString(&keyX[i], file);
		}
		
	}
	std::fclose(file);
	return true;
}
void PlayerPrefs::LuaAdd(sol::state& state)
{
	state["PlayerPrefs"] = sol::new_table();
	state["PlayerPrefs"]["DeleteAll"] = &PlayerPrefs::DeleteAll;
	state["PlayerPrefs"]["DeleteKey"] = &PlayerPrefs::DeleteKey;
	state["PlayerPrefs"]["HasKey"] = &PlayerPrefs::HasKey;
	state["PlayerPrefs"]["GetFloat"] = &PlayerPrefs::GetFloat;
	state["PlayerPrefs"]["GetInt"] = &PlayerPrefs::GetInt;
	state["PlayerPrefs"]["GetString"] = &PlayerPrefs::GetString;
	state["PlayerPrefs"]["SetFloat"] = &PlayerPrefs::SetFloat;
	state["PlayerPrefs"]["SetInt"] = &PlayerPrefs::SetInt;
	state["PlayerPrefs"]["SetString"] = &PlayerPrefs::SetString;
	state["PlayerPrefs"]["Save"] = &PlayerPrefs::Save;
}
bool PlayerPrefs::ReadFromFile(void* _Buffer, size_t _Size, std::FILE* _File)
{
	if (!_File)
		return false;

	fread(_Buffer, 1, _Size, _File);
	if (ferror(_File))
		return false;

	return true;
}
bool PlayerPrefs::ReadFloat(KeyX* _Float, std::FILE* _File)
{
	if (!_File)
		return false;

	float keyValue;
	uint32_t keyLength;
	std::string keyName;

	ReadFromFile(&keyValue, sizeof(keyValue), _File);
	ReadFromFile(&keyLength, sizeof(keyLength), _File);
	keyName.resize(keyLength);
	ReadFromFile(&keyName[0], keyLength, _File);

	_Float->keyValue = keyValue;
	_Float->keyName = keyName;

	return true;
}
bool PlayerPrefs::ReadInt(KeyX* _Int, std::FILE* _File)
{
	if (!_File)
		return false;

	int keyValue;
	uint32_t keyLength;
	std::string keyName;

	ReadFromFile(&keyValue, sizeof(keyValue), _File);
	ReadFromFile(&keyLength, sizeof(keyLength), _File);
	keyName.resize(keyLength);
	ReadFromFile(&keyName[0], keyLength, _File);

	_Int->keyValue = keyValue;
	_Int->keyName = keyName;

	return true;
}
bool PlayerPrefs::ReadString(KeyX* _String, std::FILE* _File)
{
	if (!_File)
		return false;

	uint32_t valueLength;
	std::string valueName;
	uint32_t keyLength;
	std::string keyName;

	ReadFromFile(&valueLength, sizeof(valueLength), _File);
	valueName.resize(valueLength);
	ReadFromFile(&valueName[0], valueLength, _File);
	ReadFromFile(&keyLength, sizeof(keyLength), _File);
	keyName.resize(keyLength);
	ReadFromFile(&keyName[0], keyLength, _File);

	_String->keyValue = valueName;
	_String->keyName = keyName;

	return true;
}