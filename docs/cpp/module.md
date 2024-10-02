Create new dll template in VS
## Entry point
```cpp
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
```
## Some default pointers, for now
```cpp
// example
static ScriptingSystem* scriptingSystem = nullptr;
static DX* dx = nullptr;
static Entity* ecs = nullptr;
static Game* game = nullptr;
```
To add more pointers you can add like this
```cpp
// example
typedef void (*SetDX)(DX* dx);
typedef void (*SetScriptingSystem)(ScriptingSystem* scriptingSystem);
typedef void (*SetGame)(Game* game);
typedef void (*SetEntity)(Entity* entity);
```
```cpp
moduleBuffer->SetModule<SetDX, DX>("SetDX", dx);
moduleBuffer->SetModule<SetGame, Game>("SetGame", game);
moduleBuffer->SetModule<SetEntity, Entity>("SetEntity", ecs);
moduleBuffer->SetModule<SetScriptingSystem, ScriptingSystem>("SetScriptingSystem", scriptingSystem);
```
You can also add custom function to call, from engine to dll
```cpp
// example
typedef void (*EngineStart)();
typedef void (*EngineRelease)();
typedef void (*GameStart)();
typedef void (*GameUpdate)();
```
```cpp
// example
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
```
And do not forget to call function in engine
```cpp
EngineReleaseModule();
```
dll save to
```
module\\
```
Engine automatic find and load dlls in this folder
## Engine
Called on engine start
```cpp
IMGUI_MODULE void EngineStart() {}
```
Called on engine update
```cpp
IMGUI_MODULE void EngineUpdate() {}
```
Called on engine release
```cpp
IMGUI_MODULE void EngineRelease() {}
```
Called on engine window update
```cpp
IMGUI_MODULE void EngineWindowProc() {}
```

## Game
Called on game start
```cpp
IMGUI_MODULE void GameStart() {}
```
Called on game update
```cpp
IMGUI_MODULE void GameUpdate() {}
```
Called on game release
```cpp
IMGUI_MODULE void GameRelease() {}
```
Called on game window update
```cpp
IMGUI_MODULE void GameWindowProc() {}
```
### Here you can add more custom pointers or functions
[Module.h](https://github.com/HODAKdev/StarEngine/blob/main/STAR/ENGINE/SRC/MODULE/Module.h)
[Module.cpp](https://github.com/HODAKdev/StarEngine/blob/main/STAR/ENGINE/SRC/MODULE/Module.cpp)
### ImGui module example
[main.h](https://github.com/HODAKdev/StarEngine/blob/main/STAR/IMGUI/main.cpp)
