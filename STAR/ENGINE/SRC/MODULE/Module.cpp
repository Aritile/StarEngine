#include "Module.h"
#include "../STAR/Star.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../DX/DX.h"
#include <filesystem>
#include "../ENTITY/Entity.h"
#include "../GAME/Game.h"
#include "../ENGINE/Engine.h"

#define MODULE_PATH "module"

// classes
typedef void (*SetDX)(DX* dx);
typedef void (*SetScriptingSystem)(ScriptingSystem* scriptingSystem);
typedef void (*SetGame)(Game* game);
typedef void (*SetEntity)(Entity* entity);

// voids
typedef void (*EngineStart)();
typedef void (*EngineUpdate)();
typedef void (*EngineRelease)();
typedef void (*GameStart)();
typedef void (*GameUpdate)();
typedef void (*GameRelease)();

// callbacks
typedef void (*EngineWindowProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef void (*GameWindowProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static DX* dx = DX::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static Game* game = Game::GetSingleton();
static Engine* engine = Engine::GetSingleton();

bool ModuleBuffer::EngineStartModule()
{
	if (!hModule)
		return false;

	EngineStart engineStart = (EngineStart)GetProcAddress(hModule, "EngineStart");
	if (!engineStart)
		return false;

	engineStart();
	return true;
}
bool ModuleBuffer::EngineUpdateModule()
{
	if (!hModule)
		return false;

	EngineUpdate engineUpdate = (EngineUpdate)GetProcAddress(hModule, "EngineUpdate");
	if (!engineUpdate)
		return false;

	engineUpdate();
	return true;
}
bool ModuleBuffer::EngineReleaseModule()
{
	if (!hModule)
		return false;

	EngineRelease engineRelease = (EngineRelease)GetProcAddress(hModule, "EngineRelease");
	if (!engineRelease)
		return false;

	engineRelease();
	return true;
}
bool ModuleBuffer::GameStartModule()
{
	if (!hModule)
		return false;

	GameStart gameStart = (GameStart)GetProcAddress(hModule, "GameStart");
	if (!gameStart)
		return false;

	gameStart();
	return true;
}
bool ModuleBuffer::GameUpdateModule()
{
	if (!hModule)
		return false;

	GameUpdate gameUpdate = (GameUpdate)GetProcAddress(hModule, "GameUpdate");
	if (!gameUpdate)
		return false;

	gameUpdate();
	return true;
}
bool ModuleBuffer::GameReleaseModule()
{
	if (!hModule)
		return false;

	GameRelease gameRelease = (GameRelease)GetProcAddress(hModule, "GameRelease");
	if (!gameRelease)
		return false;

	gameRelease();
	return true;
}
bool ModuleBuffer::LoadModule(std::string _Filename)
{
	hModule = LoadLibraryA(_Filename.c_str());
	if (!hModule)
	{
		Star::AddLog("[Module] -> Failed to load Dynamic Link Library!");
		return false;
	}

	return true;
}
void ModuleBuffer::DestroyModule()
{
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = nullptr;
	}
}
bool ModuleBuffer::EngineWindowProcModule(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!hModule)
		return false;

	EngineWindowProc engineWindowProc = (EngineWindowProc)GetProcAddress(hModule, "EngineWindowProc");
	if (!engineWindowProc)
		return false;

	engineWindowProc(hwnd, uMsg, wParam, lParam);
	return true;
}
bool ModuleBuffer::GameWindowProcModule(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!hModule)
		return false;

	GameWindowProc gameWindowProc = (GameWindowProc)GetProcAddress(hModule, "GameWindowProc");
	if (!gameWindowProc)
		return false;

	gameWindowProc(hwnd, uMsg, wParam, lParam);
	return true;
}

Module* Module::GetSingleton()
{
	static Module module;
	return &module;
}
bool Module::Init()
{
	for (const auto& index : std::filesystem::directory_iterator(MODULE_PATH))
	{
		std::filesystem::path file(index.path());

		if (!index.is_directory())
		{
			if (file.extension().string().compare(".dll") == 0)
			{
				std::string filename = file.filename().string();
				std::string x = MODULE_PATH + std::string("\\") + filename;
				moduleBuffers.emplace_back();
				ModuleBuffer* moduleBuffer = &moduleBuffers.back();
				Star::AddLog("[Module] -> Loading module.. %s", filename.c_str());
				moduleBuffer->LoadModule(x);
			}
		}
	}

	InitModules();

	return true;
}
void Module::EngineStartModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].EngineStartModule();
}
void Module::EngineUpdateModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].EngineUpdateModule();
}
void Module::EngineReleaseModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].EngineReleaseModule();
}
void Module::InitModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
	{
		ModuleBuffer* moduleBuffer = &moduleBuffers.at(i);
		moduleBuffer->SetModule<SetDX, DX>("SetDX", dx);
		moduleBuffer->SetModule<SetGame, Game>("SetGame", game);
		moduleBuffer->SetModule<SetEntity, Entity>("SetEntity", ecs);
		moduleBuffer->SetModule<SetScriptingSystem, ScriptingSystem>("SetScriptingSystem", scriptingSystem);
		// you can add more classes
	}
}
void Module::GameStartModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].GameStartModule();
}
void Module::GameUpdateModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].GameUpdateModule();
}
void Module::GameReleaseModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].GameReleaseModule();
}
void Module::EngineWindowProcModules(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].EngineWindowProcModule(hwnd, uMsg, wParam, lParam);
}
void Module::GameWindowProcModules(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].GameWindowProcModule(hwnd, uMsg, wParam, lParam);
}
template<typename FuncType, typename ArgType>
bool ModuleBuffer::SetModule(const char* _Name, ArgType* _ArgType)
{
	if (!hModule)
		return false;

	FuncType m = (FuncType)GetProcAddress(hModule, _Name);
	if (!m)
		return false;

	m(_ArgType);
	return true;
}
void Module::DestroyModules()
{
	for (size_t i = 0; i < moduleBuffers.size(); i++)
		moduleBuffers[i].DestroyModule();
}