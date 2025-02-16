#include "Engine.h"

// component
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/TextComponent.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"

// system
#include "../SYSTEM/PhysicsSystem.h"
#include "../SYSTEM/ModelSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../SYSTEM/PlayerPrefs.h"

// editor
#include "../EDITOR/WINDOW/Viewport.h"
#include "../EDITOR/WINDOW/Settings.h"

// storage
#include "../STORAGE/TextureStorage.h"
#include "../STORAGE/MeshStorage.h"
#include "../STORAGE/SpriteStorage.h"

// other
#include "../MODULE/Module.h"
#include "../STRDX/Widgets.h"
#include "../GAME/Game.h"
#include "../WINDOW/MainWindow.h"
#include "../USERINPUT/UserInput.h"
#include "../JOB/Job.h"

#define ENABLE_MODULES

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
static Timing* timing = Timing::GetSingleton();
static Job* job = Job::GetSingleton();
static SpriteStorage* spriteStorage = SpriteStorage::GetSingleton();

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
#if defined(_DEBUG)
    std::filesystem::current_path("..\\x64\\Debug");
#endif

    // topology
    dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // sky
    //sky->SetSolidColor(Vector3(0.5f, 0.5f, 0.5f));
    sky->LoadTexture("data\\hdr\\overcast_soil_puresky_4k.hdr");
    //sky->SetNone(); // default

    playerPrefs->Load();
    game->InitTime();
    spriteStorage->Init();

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
#if defined(ENABLE_MODULES)
    Star::AddLog("[Engine] -> Initializing Module System..");
    if (!module->Init())
        Star::AddLog("[Engine] -> Failed to initialize Module System!");
#endif
    /* --------------------------- */

    // strdx
    widgets->Init();
    widgets->InitBoundingBoxWidget();
    widgets->InitGridWidget();
    widgets->InitPerspectiveFrustumWidget();
    widgets->InitOrthographicFrustumWidget();
    widgets->InitImage();
    widgets->InitRectangle();
    widgets->InitRenderTarget(multisamplingCount);
    widgets->InitCanvas();

#if !defined(GAME) // this is only for engine
    physicsTiming = timing->AddTimer("Physics");
    drawTiming = timing->AddTimer("Draw");
#endif

    settingsWindow->Load();
    module->EngineStartModules();

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
    // call lua update void
    GamePlayUpdate();

    if (game->GetGameState() == GameState::GamePlay)
    {
        // call lua fixed update void
        GamePlayFixedUpdate();

        if (physicsTiming)
        {
            physicsTiming->SetTotalTime(0.0f);
            physicsTiming->StartTimer();
        }
        // if physics is too fast or slow, edit this value
        physicsSystem->Update(1.0f / 120.0f);
        if (physicsTiming)
        {
            physicsTiming->StopTimer();
            physicsTiming->SetTotalTime(physicsTiming->GetElapsedTime());
        }
    }

    // update all entity transforms
    UpdateTransform(ecs->root);

    // lua call late update
    GamePlayLateUpdate();

    // render engine layout
#if !defined(GAME)
    RenderToMainBuffer();
#endif

    module->EngineUpdateModules();
    job->Update();

    // render sky, models to render target
    RenderEnvironment(viewportWindow->GetPerspectiveProjectionMatrix(),
                      viewportWindow->GetPerspectiveViewMatrix(),
                      GetClearColor(),
                      (ID3D11RenderTargetView*)widgets->GetRenderTarget(),
                      (ID3D11DepthStencilView*)widgets->GetDepthStencil(),
                      viewportWindow->GetViewport(),
                      dx->dxSwapChain,
                      false);

    {
        // render rectangle with render target texture
        ID3D11RenderTargetView* renderTargetView = viewportWindow->GetRenderTargetView();
        ID3D11DepthStencilView* depthStencilView = viewportWindow->GetDepthStencilView();

        // alpha fix
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        dx->dxDeviceContext->OMSetBlendState(dx->dxBlendState, blendFactor, 0xffffffff);

        D3D11_VIEWPORT viewport = viewportWindow->GetViewport();
        dx->dxDeviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
        Vector4 clearColor = GetClearColor();
        dx->dxDeviceContext->ClearRenderTargetView(renderTargetView, (float*)&clearColor);
        //dx->dxDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        dx->dxDeviceContext->RSSetViewports(1, &viewport);
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // for debug
        widgets->RenderRectangle(enableMultisampling, multisamplingCount);
        viewportWindow->RefreshRenderState();
        dx->dxDeviceContext->OMSetRenderTargets(NULL, NULL, NULL);
    }

    if (game->GetGameState() == GameState::GamePlay)
    {
        Matrix projectionMatrix = Matrix::Identity;
        Matrix viewMatrix = Matrix::Identity;
        bool goodCamera = FindGoodCamera(projectionMatrix, viewMatrix);
        if (goodCamera)
        {
            game->BeginTime();
            {
                RenderEnvironment(projectionMatrix,
                                  viewMatrix,
                                  GetClearColor(),
                                  game->GetRenderTargetView(),
                                  game->GetDepthStencilView(),
                                  game->GetViewport(),
                                  game->GetSwapChain(),
                                  true);
            }
            game->EndTime();
        }
    }

    if (close)
        if (job->IsDone())
            PostQuitMessage(0);
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
    sky->Release();
    physicsSystem->Shutdown();
    widgets->Release();
    userInput->Release();
    textureStorage->ReleaseAllTextures();
    meshStorage->ReleaseAllModels();
    meshStorage->ReleaseAllMeshes();
    module->EngineReleaseModules();
    module->DestroyModules();
    job->Release();
    spriteStorage->Release();

    dx->dxDeviceContext->ClearState();
    dx->dxDeviceContext->Flush();

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
            ecs->GetComponent<ScriptingComponent>(entity).LuaCallUpdate();
    }
}
void Engine::GamePlayLateUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).LuaCallLateUpdate();
    }
}
void Engine::GamePlayFixedUpdate()
{
    if (game->GetGameState() == GameState::GamePlay)
    {
        auto view = ecs->registry.view<ScriptingComponent>();
        for (auto entity : view)
            ecs->GetComponent<ScriptingComponent>(entity).LuaCallFixedUpdate();
    }
}

void Engine::RenderToMainBuffer()
{
    SetRenderTarget(GetClearColor());
    editor->Render();
}

void Engine::RenderEnvironment(Matrix _ProjectionMatrix, Matrix _ViewMatrix, Vector4 _Color, ID3D11RenderTargetView* _RenderTargetView, ID3D11DepthStencilView* _DepthStencilView, D3D11_VIEWPORT _Viewport, IDXGISwapChain* _SwapChain, bool game)
{
    if (!_RenderTargetView) return;
    if (!_DepthStencilView) return;
    if (!_SwapChain) return;

    dx->dxDeviceContext->OMSetRenderTargets(1, &_RenderTargetView, _DepthStencilView);
    dx->dxDeviceContext->ClearRenderTargetView(_RenderTargetView, (float*)&_Color);
    dx->dxDeviceContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    dx->dxDeviceContext->RSSetViewports(1, &_Viewport);

    sky->Render(_ViewMatrix, _ProjectionMatrix);

    if (drawTiming) drawTiming->SetTotalTime(0.0f);

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
            {
                if (drawTiming) drawTiming->StartTimer();
                meshComponent.DrawMesh(_ViewMatrix, _ProjectionMatrix);
                if (drawTiming)
                {
                    drawTiming->StopTimer();
                    drawTiming->AddTotalTime(drawTiming->GetElapsedTime());
                }
            }
        }

        // UI
        if (game)
        {
            widgets->RenderImage(entity);
            widgets->RenderRectangle(entity);

            if (ecs->HasComponent<TextComponent>(entity))
            {
                auto& textComponent = ecs->GetComponent<TextComponent>(entity);
                textComponent.RenderText();
            }
        }
    }

    if (!game)
    {
        dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        if (enableAntialiasing)
            widgets->SetRasterizerState();
        widgets->RenderBoundingBoxWidget();
        widgets->RenderGridWidget();
        widgets->RenderPerspectiveFrustumWidget();
        widgets->RenderOrthographicFrustumWidget();
        // EXPERIMENTAL FEATURE
        //widgets->RenderCanvas();
        if (enableAntialiasing)
            widgets->UnsetRasterizerState();
        viewportWindow->RefreshRenderState();
    }
    else
    {
        module->GameUpdateModules();
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
void Engine::CloseSafeEngine()
{
    if (!job->IsDone())
        Star::AddLog("[Window] -> Please wait. Some of the jobs are still running.");
    close = true;
}
Vector4 Engine::GetClearColor()
{
    if (sky->GetType() == SkySolidColor)
    {
        Vector3 solidColor = sky->GetSolidColor();
        return Vector4(solidColor.x, solidColor.y, solidColor.z, 1.0f);
    }
    else
    {
        return clearColor;
    }
}
