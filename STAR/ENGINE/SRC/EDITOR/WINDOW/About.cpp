#include "About.h"
#include "../../DX/DX.h"
#include "../../MAIN/Main.h"
#include "../../STAR/Star.h"
#include <DirectXTex.h>
#include <WICTextureLoader11.h>

AboutWindow* AboutWindow::GetSingleton()
{
	static AboutWindow aboutWindow;
	return &aboutWindow;
}

///////////////////////////////////////////////////////////////

static DX* dx = DX::GetSingleton();

void AboutWindow::Render()
{
	if (!renderAbout) return;
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec2 windowPadding = style->WindowPadding;

	ImGui::SetNextWindowSize(ImVec2(400, 290));
	ImGui::Begin("About", &renderAbout, ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui::Image((void*)_StarBanner, ImVec2(84, 84));
		ImGui::SameLine();
		ImGui::Text(
			"Star Engine\n"
			"%s\n"
			"Copyright(c) 2022-2025 Hodak.\n"
			"All rights reserved.",
			_VersionText.c_str());
		ImGui::Text("Used third party software:");
		ImGui::BeginChild("AboutChild", ImVec2(0, 128));
		{
			ImGui::Text("Dear ImGui https://github.com/ocornut/imgui");
			ImGui::Text("assimp https://github.com/assimp/assimp");
			ImGui::Text("EnTT https://github.com/skypjack/entt");
			ImGui::Text("Lua https://github.com/lua/lua");
			ImGui::Text("PhysX https://github.com/NVIDIAGameWorks/PhysX");
			ImGui::Text("sol2 https://github.com/ThePhD/sol2");
			ImGui::Text("yaml-cpp https://github.com/jbeder/yaml-cpp");
			ImGui::Text("ttf2mesh https://github.com/fetisov/ttf2mesh");
			ImGui::Text("ImGuizmo https://github.com/CedricGuillemet/ImGuizmo");
			ImGui::Text("DirectXTK https://github.com/microsoft/DirectXTK/");
			ImGui::Text("DirectXTex https://github.com/microsoft/DirectXTex");
			ImGui::Text("STRDX https://github.com/HODAKdev/STRDX");
			ImGui::Text("IconFontCppHeaders https://github.com/juliettef/IconFontCppHeaders");
			ImGui::Text("zlib https://zlib.net/");
			ImGui::Text("LuaSocket https://github.com/lunarmodules/luasocket");
			ImGui::Text("ACES Filmic https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/");
			ImGui::Text("Font Awesome https://fontawesome.com/");
			ImGui::Text("imoguizmo https://github.com/fknfilewalker/imoguizmo");
		}
		ImGui::EndChild();
		float _X = windowPadding.x + ImGui::CalcTextSize("Made in Slovakia").x;
		ImGui::SetCursorPos(ImVec2(400 - _X, ImGui::GetCursorPos().y));
		ImGui::Text("Made in Slovakia");
	}
	ImGui::End();
}

void AboutWindow::Init()
{
	DirectX::ScratchImage scratchImage;
	if (FAILED(DirectX::LoadFromWICFile(
		L"data\\image\\star.png",
		DirectX::WIC_FLAGS_NONE,
		NULL,
		scratchImage)))
		Star::AddLog("[About] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateShaderResourceView(dx->dxDevice,
		scratchImage.GetImages(),
		scratchImage.GetImageCount(),
		scratchImage.GetMetadata(),
		&_StarBanner)))
		Star::AddLog("[About] -> Failed to create shader resource view from scratch image!");

	scratchImage.Release();

	std::string _Ver = std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(PATCH);
	_VersionText = "Version " + _Ver + " (" + _Ver + ")";
}

void AboutWindow::Shutdown()
{
	if (_StarBanner) _StarBanner->Release();
}