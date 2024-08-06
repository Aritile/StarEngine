#include "MainWindow.h"
#include "../DX/DX.h"
#include "../STAR/Star.h"
#include <dwmapi.h>
#include "../ENTITY/Entity.h"
#include "../EDITOR/Editor.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../ENGINE/Engine.h"

MainWindow* MainWindow::GetSingleton()
{
	static MainWindow mainWindow;
	return &mainWindow;
}

static DX* dx = DX::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static Editor* editor = Editor::GetSingleton();
static MainWindow* mainWindow = MainWindow::GetSingleton();
static Engine* engine = Engine::GetSingleton();

bool MainWindow::CreateMainWindow(std::wstring _Name, int _Width, int _Height)
{
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = NULL;
    wcex.cbWndExtra = NULL;
    wcex.hInstance = engine->hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    //wcex.hCursor = LoadCursor(NULL, NULL);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(46, 46, 46));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _Name.c_str();
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return false;

    /*---*/

    int x = (Star::GetDisplayWidth() - _Width) / 2;
    int y = (Star::GetDisplayHeight() - _Height) / 2;

    hwnd = CreateWindowEx(
        NULL,
        _Name.c_str(),
        _Name.c_str(),
        WS_OVERLAPPEDWINDOW,
        x, y,
        _Width, _Height,
        NULL,
        NULL,
        engine->hInstance,
        NULL
    );

    if (!hwnd)
        return false;

    // dark mode
    BOOL value = TRUE;
    DwmSetWindowAttribute(hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &value,
        sizeof(value));

    /*---*/

    return true;
}
LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    editor->SetProcHandler(hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    case WM_EXITSIZEMOVE:
    {
        UINT width = mainWindow->GetContextWidth();
        UINT height = mainWindow->GetContextHeight();
        dx->ResizeBuffer(width, height);
        break;
    }

    case WM_SIZE:
    {
        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
        {
            UINT width = mainWindow->GetContextWidth();
            UINT height = mainWindow->GetContextHeight();
            dx->ResizeBuffer(width, height);
        }
        break;
    }

    case WM_GETMINMAXINFO:
    {
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 256;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 256;
        break;
    }

    case WM_SETFOCUS:
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).RecompileScriptsChecksum();
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return NULL;
}
RECT MainWindow::GetClientRect()
{
    RECT mainWindow;
    ::GetClientRect(hwnd, &mainWindow);

    POINT leftTop = { mainWindow.left, mainWindow.top };
    POINT rightBottom = { mainWindow.right, mainWindow.bottom };
    ClientToScreen(hwnd, &leftTop);
    ClientToScreen(hwnd, &rightBottom);

    RECT clip;
    SetRect(&clip, leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
    return clip;
}
UINT MainWindow::GetContextWidth()
{
    RECT rc;
    ::GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    return width;
}
UINT MainWindow::GetContextHeight()
{
    RECT rc;
    ::GetClientRect(hwnd, &rc);
    UINT height = rc.bottom - rc.top;
    return height;
}
HWND MainWindow::GetHandle()
{
    return hwnd;
}