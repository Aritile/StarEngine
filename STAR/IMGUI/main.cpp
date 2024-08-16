// ImGui Module

// os
#include <d3d11.h>
#include <Windows.h>

// include
#include <sol/sol.hpp>

// core
#include "DX/DX.h"
#include "GAME/Game.h"
#include "ENTITY/Entity.h"
#include "SYSTEM/ScriptingSystem.h"
#include "ENTITY/COMPONENT/ScriptComponent.h"
#include "ENTITY/COMPONENT/GeneralComponent.h"

// imgui
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_dx11.h"
#include "IMGUI/imgui_impl_win32.h"

// define
#define IMGUI_MODULE extern "C" __declspec(dllexport)
#define DEFAULT_FONT "data\\font\\roboto.ttf"

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
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ScriptingSystem* scriptingSystem = nullptr;
static DX* dx = nullptr;
static Entity* ecs = nullptr;
static Game* game = nullptr;

void SetFont()
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF(DEFAULT_FONT, 16.0f);
}
void SetStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	// my color, red
	ImVec4 myColor = ImVec4(ImGui::ColorConvertU32ToFloat4(IM_COL32(0xE2, 0x52, 0x52, 0xFF)));

	unsigned char offset = 0x10;

	ImVec4 text = ImVec4(1.000f, 1.000f, 1.000f, 1.000f); /* OK */
	ImVec4 textDisabled = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x80 - offset, 0x80 - offset, 0x80 - offset, 0xFF));
	ImVec4 windowBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x2E - offset, 0x2E - offset, 0x2E - offset, 0xFF));
	ImVec4 childBg = ImVec4(0.280f, 0.280f, 0.280f, 0.000f); /* OK */
	ImVec4 popupBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x50 - offset, 0x50 - offset, 0x50 - offset, 0xFF));
	ImVec4 border = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x44 - offset, 0x44 - offset, 0x44 - offset, 0xFF));
	ImVec4 borderShadow = ImVec4(0.000f, 0.000f, 0.000f, 0.000f); /* OK */
	ImVec4 frameBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x29 - offset, 0x29 - offset, 0x29 - offset, 0xFF));
	ImVec4 frameBgHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x33 - offset, 0x33 - offset, 0x33 - offset, 0xFF));
	ImVec4 titleBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x26 - offset, 0x26 - offset, 0x26 - offset, 0xFF));
	ImVec4 menuBarBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x32 - offset, 0x32 - offset, 0x32 - offset, 0xFF));
	ImVec4 scrollbarGrab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x47 - offset, 0x47 - offset, 0x47 - offset, 0xFF));
	ImVec4 scrollbarGrabHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x4D - offset, 0x4D - offset, 0x4D - offset, 0xFF));
	ImVec4 sliderGrab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x64 - offset, 0x64 - offset, 0x64 - offset, 0xFF));
	ImVec4 headerHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x78 - offset, 0x78 - offset, 0x78 - offset, 0xFF));
	ImVec4 tab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x19 - offset, 0x19 - offset, 0x19 - offset, 0xFF));
	ImVec4 tabHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x5A - offset, 0x5A - offset, 0x5A - offset, 0xFF));
	ImVec4 plotHistogram = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x95 - offset, 0x95 - offset, 0x95 - offset, 0xFF));

	colors[ImGuiCol_Text] = text;
	colors[ImGuiCol_TextDisabled] = textDisabled;
	colors[ImGuiCol_WindowBg] = windowBg;
	colors[ImGuiCol_ChildBg] = childBg;
	colors[ImGuiCol_PopupBg] = popupBg;
	colors[ImGuiCol_Border] = border;
	colors[ImGuiCol_BorderShadow] = borderShadow;
	colors[ImGuiCol_FrameBg] = frameBg;
	colors[ImGuiCol_FrameBgHovered] = frameBgHovered;
	colors[ImGuiCol_FrameBgActive] = ImVec4(colors[ImGuiCol_ChildBg].x, colors[ImGuiCol_ChildBg].y, colors[ImGuiCol_ChildBg].z, 1.000f);
	colors[ImGuiCol_TitleBg] = titleBg;
	colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_MenuBarBg] = menuBarBg;
	colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_ScrollbarGrab] = scrollbarGrab;
	colors[ImGuiCol_ScrollbarGrabHovered] = scrollbarGrabHovered;
	colors[ImGuiCol_ScrollbarGrabActive] = myColor;
	colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];
	colors[ImGuiCol_SliderGrab] = sliderGrab;
	colors[ImGuiCol_SliderGrabActive] = myColor;
	colors[ImGuiCol_Button] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.391f);
	colors[ImGuiCol_Header] = colors[ImGuiCol_PopupBg];
	colors[ImGuiCol_HeaderHovered] = headerHovered;
	colors[ImGuiCol_HeaderActive] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_SliderGrab];
	colors[ImGuiCol_SeparatorActive] = myColor;
	colors[ImGuiCol_ResizeGrip] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = myColor;
	colors[ImGuiCol_Tab] = tab;
	colors[ImGuiCol_TabHovered] = tabHovered;
	colors[ImGuiCol_TabActive] = colors[ImGuiCol_MenuBarBg];
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_MenuBarBg];
	colors[ImGuiCol_DockingPreview] = ImVec4(myColor.x, myColor.y, myColor.z, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_PlotLines] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_PlotLinesHovered] = myColor;
	colors[ImGuiCol_PlotHistogram] = plotHistogram;
	colors[ImGuiCol_PlotHistogramHovered] = myColor;
	colors[ImGuiCol_TextSelectedBg] = colors[ImGuiCol_ButtonHovered];
	colors[ImGuiCol_DragDropTarget] = myColor;
	colors[ImGuiCol_NavHighlight] = myColor;
	colors[ImGuiCol_NavWindowingHighlight] = myColor;
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(colors[ImGuiCol_BorderShadow].x, colors[ImGuiCol_BorderShadow].y, colors[ImGuiCol_BorderShadow].z, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = colors[ImGuiCol_NavWindowingDimBg];

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
}
void ImGuiNewFrame()
{
	sol::state& state = scriptingSystem->GetState();

	auto view = ecs->registry.view<ScriptingComponent>();
	for (auto entity : view)
	{
		auto& scriptingComponent = ecs->GetComponent<ScriptingComponent>(entity);
		//scriptingComponent.LuaCallFunction("ImGuiNewFrame"); // not working, idk why, use manual way

		std::vector<ScriptComponent>* scriptComponents = scriptingComponent.GetScripts();
		if (scriptComponents)
		{
			if (!scriptComponents->empty())
			{
				for (size_t i = 0; i < scriptComponents->size(); i++)
				{
					ScriptComponent* scriptComponent = &scriptComponents->at(i);
					if (!scriptComponent->activeComponent) continue;
					sol::optional<sol::function> maybe_function = state[scriptComponent->fileName.c_str()]["ImGuiNewFrame"];
					if (maybe_function)
					{
						sol::function function = maybe_function.value();
						if (function)
							sol::protected_function_result result = function();
					}
				}
			}
		}
	}
}

IMGUI_MODULE void SetDX(DX* _DX)
{
	dx = _DX;
}
IMGUI_MODULE void SetScriptingSystem(ScriptingSystem* _ScriptingSystem)
{
	scriptingSystem = _ScriptingSystem;
}
IMGUI_MODULE void SetGame(Game* _Game)
{
	game = _Game;
}
IMGUI_MODULE void SetEntity(Entity* _Entity)
{
	ecs = _Entity;
}

// engine
IMGUI_MODULE void EngineStart()
{
	sol::state& state = scriptingSystem->GetState();
	state["ImGui"] = sol::new_table();
	state["ImGui"]["Begin"] = [](const char* _Name) { ImGui::Begin(_Name); };
	state["ImGui"]["End"] = []() { ImGui::End(); };
	state["ImGui"]["Text"] = [](const char* _Text) { ImGui::Text(_Text); };
	state["ImGui"]["Button"] = [](const char* _Label, Vector2 _Size) { return ImGui::Button(_Label, ImVec2(_Size.x, _Size.y)); };
	state["ImGui"]["Bullet"] = []() { ImGui::Bullet(); };
	state["ImGui"]["BulletText"] = [](const char* _Text) { ImGui::BulletText(_Text); };
	state["ImGui"]["Separator"] = []() { ImGui::Separator(); };
	state["ImGui"]["SeparatorText"] = [](const char* _Text) { ImGui::SeparatorText(_Text); };
	state["ImGui"]["BeginDisabled"] = []() { ImGui::BeginDisabled(); };
	state["ImGui"]["EndDisabled"] = []() { ImGui::EndDisabled(); };
	state["ImGui"]["DragFloat"] = [](const char* _Name, float* _Value, float _Speed, float _Min, float _Max, const char* _Format) { ImGui::DragFloat(_Name, _Value, _Speed, _Min, _Max, _Format); };
	state["ImGui"]["DragFloat2"] = [](const char* _Name, Vector2* _Value, float _Speed, float _Min, float _Max, const char* _Format) { ImGui::DragFloat2(_Name, (float*)_Value, _Speed, _Min, _Max, _Format); };
	state["ImGui"]["DragFloat3"] = [](const char* _Name, Vector3* _Value, float _Speed, float _Min, float _Max, const char* _Format) { ImGui::DragFloat3(_Name, (float*)_Value, _Speed, _Min, _Max, _Format); };
	state["ImGui"]["DragFloat4"] = [](const char* _Name, Vector4* _Value, float _Speed, float _Min, float _Max, const char* _Format) { ImGui::DragFloat4(_Name, (float*)_Value, _Speed, _Min, _Max, _Format); };
	// add more stuff
}
IMGUI_MODULE void EngineUpdate()
{
	// code
}
IMGUI_MODULE void EngineRelease()
{
	// code
}
IMGUI_MODULE void EngineWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// code
}

// game
IMGUI_MODULE void GameStart()
{
	if (!game->GetHandle()) return;
	if (!dx->dxDevice) return;
	if (!dx->dxDeviceContext) return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = "module\\imgui.ini";
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(game->GetHandle());
	ImGui_ImplDX11_Init(dx->dxDevice, dx->dxDeviceContext);
	SetFont();
	SetStyle();
}
IMGUI_MODULE void GameUpdate()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		//ImGui::ShowDemoWindow();
		ImGuiNewFrame(); // call lua void
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
IMGUI_MODULE void GameRelease()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
IMGUI_MODULE void GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
}