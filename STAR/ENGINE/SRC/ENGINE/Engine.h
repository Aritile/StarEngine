#pragma once

#include <string>
#include <Windows.h>
#include <filesystem>
#include "../DX/DX.h"
#include "../SKY/Sky.h"
#include "../EDITOR/Editor.h"
#include "../STAR/Star.h"
#include "../DEBUG/DTiming.h"

class Engine
{
public:
	static Engine* GetSingleton();

public:
	void SetReference(HINSTANCE& hInstance,
		              HINSTANCE& hPrevInstance,
		              PWSTR& pCmdLine,
		              int& nCmdShow);
	void SetRenderTarget(Vector4 _Color);
	void EngineStart();
	void EngineUpdate();
	void EngineProcess();
	void EngineShutdown();
	void UpdateTransform(entt::entity _Entity);
	void GamePlayUpdate();
	void GamePlayLateUpdate();
	void GamePlayFixedUpdate();
	void RenderToMainBuffer();
	void RenderEnvironment(Matrix _ProjectionMatrix,
		                   Matrix _ViewMatrix,
		                   Vector4 _Color,
		                   ID3D11RenderTargetView* _RenderTargetView,
		                   ID3D11DepthStencilView* _DepthStencilView,
		                   D3D11_VIEWPORT _Viewport,
		                   IDXGISwapChain* _SwapChain,
		                   bool _Game);
	bool FindGoodCamera(Matrix& _ProjectionMatrix, Matrix& _ViewMatrix);
	entt::entity FindGoodCamera();
	void CloseSafeEngine();
	Vector4 GetClearColor();
	void TraverseEntity(entt::entity entity, Matrix _ViewMatrix, Matrix _ProjectionMatrix, bool _Game);

private:
	Vector4 clearColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);

public:
	HINSTANCE hInstance = NULL;
	HINSTANCE hPrevInstance = NULL;
	PWSTR pCmdLine = NULL;
	int nCmdShow = 0;
	bool close = false;
	bool enableAntialiasing = true;
	bool enableMultisampling = true;
	unsigned int multisamplingCount = 8;

private:
	TimingBuffer* physicsTiming = nullptr;
	TimingBuffer* drawTiming = nullptr;
};