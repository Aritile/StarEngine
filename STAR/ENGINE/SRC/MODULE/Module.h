#pragma once

#include <Windows.h>
#include "../IMGUI/imgui.h"
#include <sol/sol.hpp>
#include <d3d11.h>

class ModuleBuffer
{
public:
	ModuleBuffer() {}

private:
	HMODULE hModule = nullptr;
	
public:
	bool LoadModule(std::string _Filename);
	void DestroyModule();

public:
	template<typename FuncType, typename ArgType>
	bool SetModule(const char* _Name, ArgType* _ArgType);
	bool EngineStartModule();
	bool EngineUpdateModule();
	bool EngineReleaseModule();
	bool GameStartModule();
	bool GameUpdateModule();
	bool GameReleaseModule();
	bool EngineWindowProcModule(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool GameWindowProcModule(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class Module
{
public:
	bool Init();

public:
	void InitModules();
	void EngineStartModules();
	void EngineUpdateModules();
	void EngineReleaseModules();
	void GameStartModules();
	void GameUpdateModules();
	void GameReleaseModules();
	void EngineWindowProcModules(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void GameWindowProcModules(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DestroyModules();

private:
	std::vector<ModuleBuffer> moduleBuffers;

public:
	static Module* GetSingleton();
};