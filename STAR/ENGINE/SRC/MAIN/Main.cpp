#include "Main.h"
#include "../SPLASHSCREEN/SplashScreen.h"
#include <thread>
#include "../GAME/Game.h"
#include <iostream>
#include "../WINDOW/MainWindow.h"
#include "../SYSTEM/ProjectSceneSystem.h"

static Game* game = Game::GetSingleton();
static SplashScreen* splashScreen = SplashScreen::GetSingleton();
static DX* dx = DX::GetSingleton();
static Engine* engine = Engine::GetSingleton();
static MainWindow* mainWindow = MainWindow::GetSingleton();
static ProjectSceneSystem* projectSceneSystem = ProjectSceneSystem::GetSingleton();

int StartEngine(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	engine->SetReference(hInstance, hPrevInstance, pCmdLine, nCmdShow);

	Star::AddLog("[Engine] -> Engine version: %i.%i.%i", MAJOR, MINOR, PATCH);

	if (!splashScreen->ShowSplashScreen())
		return 0;

	if (!mainWindow->CreateMainWindow(L"StarEngine", 1280, 720))
		return 0;
	
	UINT width = mainWindow->GetContextWidth();
	UINT height = mainWindow->GetContextHeight();
	if (!dx->CreateContext(width, height))
		return 0;

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return 0;

	engine->EngineStart();

	splashScreen->HideSplashScreen();

	engine->EngineUpdate();
	engine->EngineShutdown();
	CoUninitialize();

	return 0;
}

int StartGame(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
	engine->SetReference(hInstance, hPrevInstance, pCmdLine, nCmdShow);

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return 0;

	{
		splashScreen->ShowSplashScreen();
		game->InitGame(false, NULL);
		engine->EngineStart();
		projectSceneSystem->OpenScene("data.scene");
		game->StartGame();
		splashScreen->HideSplashScreen();
		game->SetWindowState(SW_NORMAL);
		engine->EngineUpdate();
		engine->EngineShutdown();
	}

	CoUninitialize();

	return 0;
}

#if defined(_DEBUG)
int main()
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#endif
{
#if !defined(_DEBUG)
	freopen("log.txt", "w", stdout);
#endif

#if defined(_DEBUG)
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HINSTANCE hPrevInstance = NULL;
	PWSTR pCmdLine = GetCommandLineW();
	int nCmdShow = SW_NORMAL;
#endif

#if defined(GAME)
	return StartGame(hInstance, hPrevInstance, pCmdLine, nCmdShow);
#else
#if defined(_DEBUG)
#endif
	return StartEngine(hInstance, hPrevInstance, pCmdLine, nCmdShow);
#endif
	return 0;
}