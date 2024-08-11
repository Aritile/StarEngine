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
#include "../STORAGE/TextureStorage.h"
#include "../WINDOW/MainWindow.h"
#include "../USERINPUT/UserInput.h"
#include "../STORAGE/MeshStorage.h"

static DX* dx = DX::GetSingleton();
static Editor* editor = Editor::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ModelSystem* modelSystem = &ModelSystemClass();
static Sky* sky = Sky::GetSingleton();
static Game* game = Game::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();
static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static Module* module = Module::GetSingleton();
static Widgets* widgets = Widgets::GetSingleton();
static PlayerPrefs* playerPrefs = PlayerPrefs::GetSingleton();
static SettingsWindow* settingsWindow = SettingsWindow::GetSingleton();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();
static MainWindow* mainWindow = MainWindow::GetSingleton();
static UserInput* userInput = UserInput::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();

Engine* Engine::GetSingleton()
{
    static Engine Engine;
    return &Engine;
}

void Engine::SetReference(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
    this->hInstance = hInstance;
    this->hPrevInstance = hPrevInstance;
    this->pCmdLine = pCmdLine;
    this->nCmdShow = nCmdShow;
}

void Engine::SetRenderTarget(Vector4 _Color)
{
    dx->dxDeviceContext->ClearRenderTargetView(dx->dxRenderTargetView, (float*)&_Color);
    dx->dxDeviceContext->ClearDepthStencilView(dx->dxDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    dx->dxDeviceContext->OMSetRenderTargets(1, &dx->dxRenderTargetView, dx->dxDepthStencilView);
}

void Engine::EngineStart()
{
    dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    SkyFile skyFile;
    skyFile.SetSphereMap("data\\hdr\\overcast_soil_puresky_4k.hdr");
    sky->SetSky(skyFile);

    playerPrefs->Load();
    game->InitTime();

    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing User Input System..");
    if (!userInput->Init())
        Star::AddLog("[Engine] -> Failed to initialize User Input System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Model System..");
    if (!modelSystem->Init())
        Star::AddLog("[Engine] -> Failed to initialize Model System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Sky System..");
    if (!sky->Init())
        Star::AddLog("[Engine] -> Failed to initialize Sky System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Editor System..");
    if (!editor->Init())
        Star::AddLog("[Engine] -> Failed to initialize Editor System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Physics System..");
    if (!physicsSystem->Init())
        Star::AddLog("[Engine] -> Failed to initialize Physics System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Entity Component System..");
    if (!ecs->Init())
        Star::AddLog("[Engine] -> Failed to initialize Entity Component System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Lua Script System..");
    if (!scriptingSystem->Init())
        Star::AddLog("[Engine] -> Failed to initialize Lua Script System!");
    /* --------------------------- */
    Star::AddLog("[Engine] -> Initializing Module System..");
    if (!module->Init())
        Star::AddLog("[Engine] -> Failed to initialize Module System!");
    /* --------------------------- */

    // strdx
    widgets->Init();
    widgets->InitBoundingBoxWidget();
    widgets->InitGridWidget();
    widgets->InitPerspectiveFrustumWidget();
    widgets->InitOrthographicFrustumWidget();

    settingsWindow->Load();

    ShowWindow(mainWindow->GetHandle(), SW_NORMAL);
    UpdateWindow(mainWindow->GetHandle());
}

void Engine::EngineUpdate()
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

void Engine::EngineProcess()
{
    GamePlayUpdate(); /* update physics, scripts when playing */
    if (game->GetGameState() == GameState::GamePlay)
    {
        GamePlayFixedUpdate();
        physicsSystem->Update(1.0f / 120.0f); // if physics is too fast or slow, edit this value
    }
    UpdateTransform(ecs->root); /* update all entity transforms */
    GamePlayLateUpdate();
#if !defined(GAME)
    RenderToMainBuffer(); /* render engine layout */
#endif
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

void Engine::EngineShutdown()
{
    Star::AddLog("[Engine] -> Shutting/Cleaning..");

    playerPrefs->Save();
    settingsWindow->Save();

    auto view = ecs->registry.view<RigidbodyComponent, PhysicsComponent>();
    for (auto entity : view)
        ecs->Cleanup(entity);

    editor->Shutdown();
    modelSystem->Shutdown();
    sky->Shutdown();
    physicsSystem->Shutdown();
    widgets->Release();
    userInput->Release();
    textureStorage->ReleaseAllTextures();
    meshStorage->ReleaseAllModels();
    meshStorage->ReleaseAllMeshes();

    //dx->ReportLiveObjects();
    dx->Release();
}

void Engine::UpdateTransform(entt::entity _Entity)
{
    if (!ecs->IsValid(_Entity))
        return;

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
        if (!ecs->IsValid(child))
            continue;

        if (ecs->HasComponent<TransformComponent>(child))
        {
            auto& chTC = ecs->GetComponent<TransformComponent>(child);
            chTC.SetParentTransform(matrix);
        }

        UpdateTransform(child);
    }
}

void Engine::GamePlayUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).lua_call_update();
    }
}
void Engine::GamePlayLateUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).lua_call_late_update();
    }
}
void Engine::GamePlayFixedUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).lua_call_fixed_update();
    }
}

void Engine::RenderToMainBuffer()
{
    SetRenderTarget(clearColor);
    editor->Render();
}

void Engine::RenderEnvironment(Matrix _ProjectionMatrix, Matrix _ViewMatrix, Vector4 _Color, ID3D11RenderTargetView* _RenderTargetView, ID3D11DepthStencilView* _DepthStencilView, D3D11_VIEWPORT _Viewport, IDXGISwapChain* _SwapChain, bool game)
{
    if (!_RenderTargetView) return;
    if (!_DepthStencilView) return;
    if (!_SwapChain) return;

    dx->dxDeviceContext->ClearRenderTargetView(_RenderTargetView, (float*)&_Color);
    dx->dxDeviceContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    dx->dxDeviceContext->OMSetRenderTargets(1, &_RenderTargetView, _DepthStencilView);
    dx->dxDeviceContext->RSSetViewports(1, &_Viewport);
    
    sky->Render(_ViewMatrix, _ProjectionMatrix);

    auto view = ecs->registry.view<GeneralComponent>();
    for (auto entity : view)
    {
        if (entity == ecs->root) continue;
        //if (!ecs->IsValid(entity)) continue;
        if (!ecs->GetComponent<GeneralComponent>(entity).IsActive()) continue;

        if (ecs->HasComponent<RigidbodyComponent>(entity))
            ecs->GetComponent<RigidbodyComponent>(entity).UpdateActor();

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
        //widgets->SetRasterizerState(); // black lines of edges, idk why, for now disable it
        widgets->RenderBoundingBoxWidget();
        widgets->RenderGridWidget();
        widgets->RenderPerspectiveFrustumWidget();
        widgets->RenderOrthographicFrustumWidget();
        //widgets->UnsetRasterizerState();
        viewportWindow->RefreshRenderState();
    }

    if (_SwapChain) _SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

bool Engine::FindGoodCamera(Matrix& _ProjectionMatrix, Matrix& _ViewMatrix)
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