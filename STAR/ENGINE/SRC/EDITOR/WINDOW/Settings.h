#pragma once

#include <vector>
#include <string>

class SettingsWindow
{
public:
	void Init();
	void Render();
	bool renderSettings = false;
	void Save();
	void Load();
	int GetEditorProgram();

public:
	static SettingsWindow* GetSingleton();

private:
	size_t selected = 0;
	std::vector<std::string> list;
	const char* filename = "settings.star";
	const char* programList[3] = { "Text Editor", "Visual Studio", "Visual Studio Code" };
	int programSelected = 0;

private:
	void RenderGrid();
	void RenderCamera();
	void RenderPlayerPrefs();
	void RenderPhysics();
	void RenderLuaEditor();
};