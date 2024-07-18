#pragma once

#include <string>
#include <sol/sol.hpp>

// 2024
// Custom file format

struct KeyX
{
	KeyX() {}
	template<typename T>
	KeyX(std::string keyName, T keyValue) : keyName(keyName), keyValue(keyValue) {}
	std::string keyName;
	std::variant<float, int, std::string> keyValue;
};

class PlayerPrefs
{
public:
	static PlayerPrefs* GetSingleton();

public:
	bool Load();

public:
	void DeleteAll();
	bool DeleteKey(const char* key);
	bool HasKey(const char* key);
	float GetFloat(const char* key);
	int GetInt(const char* key);
	std::string GetString(const char* key);
	void SetFloat(const char* key, float value);
	void SetInt(const char* key, int value);
	void SetString(const char* key, std::string value);
	bool Save();

private:
	bool WriteToFile(const void* _Buffer, size_t _Size, std::FILE* _File);
	bool WriteFloat(KeyX* _Float, std::FILE* _File);
	bool WriteInt(KeyX* _Int, std::FILE* _File);
	bool WriteString(KeyX* _String, std::FILE* _File);

private:
	bool ReadFromFile(void* _Buffer, size_t _Size, std::FILE* _File);
	bool ReadFloat(KeyX* _Float, std::FILE* _File);
	bool ReadInt(KeyX* _Int, std::FILE* _File);
	bool ReadString(KeyX* _String, std::FILE* _File);

private:
	std::vector<KeyX> keyX;
	uint32_t numFloat = 0;
	uint32_t numInt = 0;
	uint32_t numString = 0;

private:
	const char* filename = "playerprefs";

public:
	static void LuaAdd(sol::state& state);
};