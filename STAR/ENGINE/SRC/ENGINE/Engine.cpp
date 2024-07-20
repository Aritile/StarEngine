#include "Engine.h"

#include "../EDITOR/WINDOW/Viewport.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../GAME/Game.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../SYSTEM/PhysicsSystem.h"
#include "../SYSTEM/ModelSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../MODULE/Module.h"
#include "../STRDX/Widgets.h"
#include "../SYSTEM/PlayerPrefs.h"
#include "../EDITOR/WINDOW/Settings.h"

static DX* dx = DX::GetSingleton();
static Editor* editor = &EditorClass();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ModelSystem* modelSystem = &ModelSystemClass();
static Sky* sky = &SkyClass();
static Game* game = Game::GetSingleton();
static PhysicsSystem* physicsSystem = &PhysicsSystemClass();
static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static Module* module = Module::GetSingleton();
static Widgets* widgets = Widgets::GetSingleton();
static PlayerPrefs* playerPrefs = PlayerPrefs::GetSingleton();
static SettingsWindow* settingsWindow = SettingsWindow::GetSingleton();

static Vector4 clearColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);

void DX11SetReference(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
    dx->hInstance = &hInstance;
    dx->hPrevInstance = &hPrevInstance;
    dx->pCmdLine = &pCmdLine;
    dx->nCmdShow = &nCmdShow;
}

bool DX11CreateWindow(std::wstring _Name, int _Width, int _Height)
{
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DX11WindowProc;
    wcex.cbClsExtra = NULL;
    wcex.cbWndExtra = NULL;
    wcex.hInstance = *dx->hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    //wcex.hCursor = LoadCursor(NULL, NULL);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(46, 46, 46));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = _Name.c_str();
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return false;

    /*---*/

    int x = (StarHelpers::GetDisplayWidth() - _Width) / 2;
    int y = (StarHelpers::GetDisplayHeight() - _Height) / 2;

    dx->hwnd = CreateWindowEx(
        NULL,
        _Name.c_str(),
        _Name.c_str(),
        WS_OVERLAPPEDWINDOW,
        x, y,
        _Width, _Height,
        NULL,
        NULL,
        *dx->hInstance,
        NULL
    );

    if (!dx->hwnd)
        return false;

    // dark mode
    BOOL value = TRUE;
    DwmSetWindowAttribute(dx->hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &value,
        sizeof(value));

    /*---*/

    return true;
}

LRESULT CALLBACK DX11WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    editor->SetProcHandler(hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_EXITSIZEMOVE:
        DX11ResizeBuffer();
        break;

    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            DX11ResizeBuffer();
        break;

    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 256;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 256;
        break;

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

bool DX11CreateContext()
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

    sd.BufferCount = 2;
    //sd.BufferDesc.Width = 0;
    //sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = dx->dxFormat;
    //sd.BufferDesc.RefreshRate.Numerator = 60;
    //sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = dx->hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = dx->dxSwapEffect;

    UINT flags = 0;

#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    /*---*/

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, dx->dxDriverType, NULL, flags, NULL, NULL,
        D3D11_SDK_VERSION, &sd, &dx->dxSwapChain, &dx->dxDevice,
        &dx->dxFeatureLevel, &dx->dxDeviceContext)))
        return false;

    /*---*/

    if (!DX11CreateRenderTargetView())
        return false;

    /*---*/

    if (!DX11CreateDepthStencilView())
        return false;

    /*---*/

    return true;
}

bool DX11CreateRenderTargetView()
{
    ID3D11Texture2D* tex = nullptr;
    if (FAILED(dx->dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex)))
        return false;

    /*---*/

    if (FAILED(dx->dxDevice->CreateRenderTargetView(tex, NULL, &dx->dxRenderTargetView)))
        return false;

    /*---*/

    tex->Release();
    return true;
}

bool DX11CreateDepthStencilView()
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));

    descDepth.Width = StarHelpers::GetContextWidth();
    descDepth.Height = StarHelpers::GetContextHeight();
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* tex = nullptr;
    if (FAILED(dx->dxDevice->CreateTexture2D(&descDepth, NULL, &tex)))
        return false;

    /*---*/

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if (FAILED(dx->dxDevice->CreateDepthStencilView(tex, &descDSV, &dx->dxDepthStencilView)))
        return false;

    /*---*/

    tex->Release();
    return true;
}

bool DX11ResizeBuffer()
{
    StarHelpers::AddLog("[Engine] -> Resizing SwapChain Buffer...");
    dx->dxDeviceContext->OMSetRenderTargets(NULL, NULL, NULL);
    if (dx->dxRenderTargetView) dx->dxRenderTargetView->Release();
    if (dx->dxDepthStencilView) dx->dxDepthStencilView->Release();

    if (FAILED(dx->dxSwapChain->ResizeBuffers(NULL, NULL, NULL, DXGI_FORMAT_UNKNOWN, NULL)))
        return false;

    /*---*/

    if (!DX11CreateRenderTargetView())
        return false;

    /*---*/

    if (!DX11CreateDepthStencilView())
        return false;

    /*---*/

    return true;
}

void SetRenderTarget(Vector4 _Color)
{
    dx->dxDeviceContext->ClearRenderTargetView(dx->dxRenderTargetView, (float*)&_Color);
    dx->dxDeviceContext->ClearDepthStencilView(dx->dxDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    dx->dxDeviceContext->OMSetRenderTargets(1, &dx->dxRenderTargetView, dx->dxDepthStencilView);
}

void EngineStart()
{
    dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    SkyFile skyFile;
    skyFile.SetSphereMap("data\\hdr\\overcast_soil_puresky_4k.hdr");
    sky->SetSky(skyFile);

    playerPrefs->Load();
    game->InitTime();

    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Input System...");
    if (!StarHelpers::InitInput())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Input System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Model System...");
    if (!modelSystem->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Model System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Sky System...");
    if (!sky->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Sky System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Editor System...");
    if (!editor->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Editor System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Physics System...");
    if (!physicsSystem->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Physics System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Entity Component System...");
    if (!ecs->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Entity Component System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Lua Script System...");
    if (!scriptingSystem->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Lua Script System!");
    /* --------------------------- */
    StarHelpers::AddLog("[Engine] -> Initializing Module System...");
    if (!module->Init())
        StarHelpers::AddLog("[Engine] -> Failed to initialize Module System!");
    /* --------------------------- */

    // strdx
    widgets->Init();
    widgets->InitBoundingBoxWidget();
    widgets->InitGridWidget();
    widgets->InitPerspectiveFrustumWidget();
    widgets->InitOrthographicFrustumWidget();

    settingsWindow->Load();

    ShowWindow(dx->hwnd, *dx->nCmdShow);
    UpdateWindow(dx->hwnd);
}

void EngineUpdate()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        viewportWindow->UpdateMovement();
        EngineProcess();
    }
}

void EngineProcess()
{
    GamePlayUpdate(); /* update physics, scripts when playing */
    RenderToMainBuffer(); /* render engine layout */
    UpdateTransform(ecs->root); /* update all entity transforms */
    RenderEnvironment( /* render sky, models for viewport window */
        viewportWindow->GetPerspectiveProjectionMatrix(),
        viewportWindow->GetPerspectiveViewMatrix(),
        clearColor,
        viewportWindow->GetRenderTargetView(),
        viewportWindow->GetDepthStencilView(),
        viewportWindow->GetViewport(),
        dx->dxSwapChain,
        false);

    if (game->GetGameState() == GameState::GamePlay)
    {
        Matrix projectionMatrix = Matrix::Identity;
        Matrix viewMatrix = Matrix::Identity;
        bool goodCamera = FindGoodCamera(projectionMatrix, viewMatrix);
        if (goodCamera)
        {
            game->BeginTime();
            {
                RenderEnvironment(
                    projectionMatrix,
                    viewMatrix,
                    clearColor,
                    game->GetRenderTargetView(),
                    game->GetDepthStencilView(),
                    game->GetViewport(),
                    game->GetSwapChain(),
                    true);
            }
            game->EndTime();
        }
    }
}

void EngineShutdown()
{
    StarHelpers::AddLog("[Engine] -> Shutting/Cleaning...");
    playerPrefs->Save();
    settingsWindow->Save();
    editor->Shutdown();
    modelSystem->Shutdown();
    sky->Shutdown();
    physicsSystem->Shutdown();
    widgets->Release();

    /*---*/

    if (dx->dxSwapChain) dx->dxSwapChain->Release();
    if (dx->dxDevice) dx->dxDevice->Release();
    if (dx->dxDeviceContext) dx->dxDeviceContext->Release();
    if (dx->dxRenderTargetView) dx->dxRenderTargetView->Release();
    if (dx->dxDepthStencilView) dx->dxDepthStencilView->Release();
}

void UpdateTransform(entt::entity _Entity)
{
    Matrix matrix = Matrix::Identity;

    auto& pGC = ecs->GetComponent<GeneralComponent>(_Entity);
    if (ecs->HasComponent<TransformComponent>(_Entity))
    {
        auto& pTC = ecs->GetComponent<TransformComponent>(_Entity);
        matrix = pTC.GetTransform();
    }

    for (size_t i = 0; i < pGC.GetChildren().size(); i++)
    {
        entt::entity child = pGC.GetChildren()[i];
        if (ecs->HasComponent<TransformComponent>(child))
        {
            auto& chTC = ecs->GetComponent<TransformComponent>(child);
            chTC.SetParentTransform(matrix);
        }

        UpdateTransform(child);
    }
}

void GamePlayUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        physicsSystem->Update();

        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).lua_call_update();
    }
}

void RenderToMainBuffer()
{
    SetRenderTarget(clearColor);
    editor->Render();
}

void RenderEnvironment(Matrix _ProjectionMatrix, Matrix _ViewMatrix, Vector4 _Color, ID3D11RenderTargetView* _RenderTargetView, ID3D11DepthStencilView* _DepthStencilView, D3D11_VIEWPORT _Viewport, IDXGISwapChain* _SwapChain, bool game)
{
    if (!_RenderTargetView && !_DepthStencilView) return;
    dx->dxDeviceContext->ClearRenderTargetView(_RenderTargetView, (float*)&_Color);
    dx->dxDeviceContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    dx->dxDeviceContext->OMSetRenderTargets(1, &_RenderTargetView, _DepthStencilView);
    dx->dxDeviceContext->RSSetViewports(1, &_Viewport);
    
    sky->Render(_ViewMatrix, _ProjectionMatrix);

    auto view = ecs->registry.view<GeneralComponent>();
    for (auto entity : view)
    {
        if (entity == ecs->root) continue;
        if (!ecs->GetComponent<GeneralComponent>(entity).IsActive()) continue;

        if (ecs->HasComponent<RigidBodyComponent>(entity))
            ecs->GetComponent<RigidBodyComponent>(entity).UpdateActor();

        if (ecs->HasComponent<MeshComponent>(entity))
        {
            auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);
            if (meshComponent.IsActive())
                meshComponent.DrawMesh(_ViewMatrix, _ProjectionMatrix);
        }
    }

    if (!game)
    {
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        //widgets->SetRasterizerState();
        widgets->RenderBoundingBoxWidget();
        widgets->RenderGridWidget();
        widgets->RenderPerspectiveFrustumWidget();
        widgets->RenderOrthographicFrustumWidget();
        //widgets->UnsetRasterizerState();
        viewportWindow->RefreshRenderState();
    }

    if (_SwapChain) _SwapChain->Present(1, 0);
}

bool FindGoodCamera(Matrix& _ProjectionMatrix, Matrix& _ViewMatrix)
{
    auto view = ecs->registry.view<CameraComponent>();
    for (auto entity : view)
    {
        if (!ecs->GetComponent<GeneralComponent>(entity).IsActive()) continue;
        if (!ecs->GetComponent<CameraComponent>(entity).IsActive()) continue;
        if (!ecs->HasComponent<TransformComponent>(entity)) continue;

        auto& cameraComponent = ecs->GetComponent<CameraComponent>(entity);
        _ProjectionMatrix = cameraComponent.GetProjectionMatrix();
        _ViewMatrix = cameraComponent.GetViewMatrix();
        return true;
    }
    return false;
}