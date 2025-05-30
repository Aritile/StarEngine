#include "Assets.h"
#include "Console.h"
#include <yaml-cpp/yaml.h>
#include "../../MAIN/Main.h"
#include <fstream>
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "Settings.h"
#include "Importer.h"

AssetsWindow* AssetsWindow::GetSingleton()
{
	static AssetsWindow assetsWindow;
	return &assetsWindow;
}

///////////////////////////////////////////////////////////////

static DX* dx = DX::GetSingleton();
static Sky* sky = Sky::GetSingleton();
static Editor* editor = Editor::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static AssimpLoader* assimpLoader = AssimpLoader::GetSingleton();
static SettingsWindow* settingsWindow = SettingsWindow::GetSingleton();
static Importer* importer = Importer::GetSingleton();

#define FOLDER_ICON_PATH   L"data\\icon\\folder.dds"   /**/
#define IMAGE_ICON_PATH    L"data\\icon\\image.dds"    /**/
#define FILE_ICON_PATH     L"data\\icon\\file.dds"     /**/
#define LUA_ICON_PATH      L"data\\icon\\lua.dds"      /**/
#define MODEL_ICON_PATH    L"data\\icon\\model.dds"    /**/
#define FONT_ICON_PATH     L"data\\icon\\font.dds"     /**/
#define AUDIO_ICON_PATH    L"data\\icon\\audio.dds"    /**/
#define SKY_ICON_PATH      L"data\\icon\\sky.dds"      /**/
#define MATERIAL_ICON_PATH L"data\\icon\\material.dds" /**/

void AssetsWindow::Init()
{
	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		FOLDER_ICON_PATH,
		nullptr,
		&folderTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		IMAGE_ICON_PATH,
		nullptr,
		&imageTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		FILE_ICON_PATH,
		nullptr,
		&fileTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		LUA_ICON_PATH,
		nullptr,
		&luaTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		MODEL_ICON_PATH,
		nullptr,
		&modelTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		FONT_ICON_PATH,
		nullptr,
		&ttfTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		AUDIO_ICON_PATH,
		nullptr,
		&audioTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		SKY_ICON_PATH,
		nullptr,
		&skyTexture))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	if (FAILED(DirectX::CreateDDSTextureFromFile(
		dx->dxDevice,
		MATERIAL_ICON_PATH,
		nullptr,
		&materialTexture)))
		Star::AddLog("[Assets] -> Failed to load texture from file!");

	//////////////////////////////////////////////////////////////

	projectDirPath = GetAppDirPath() + FOLDER;
	OpenDir(projectDirPath);
}

void AssetsWindow::Render()
{
	if (!renderAssets)
		return;

	ImGui::Begin("Assets");
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
		if (outCore)
		{
			ImGui::PushFont(editor->icons);
			{
				/*******************************************************/
				{
					if (GetProjectDirPath() == GetNowDirPath())
					{
						ImGui::BeginDisabled();
						ImGui::Button(ICON_FA_ARROW_LEFT);
						ImGui::EndDisabled();
					}
					else
					{
						if (ImGui::Button(ICON_FA_ARROW_LEFT))
							GoBackDir();
					}
				}
				ImGui::SameLine();
				/*******************************************************/
				{
					ImGui::BeginDisabled();
					ImGui::Button(ICON_FA_ARROW_RIGHT);
					ImGui::EndDisabled();
				}
				ImGui::SameLine();
				/*******************************************************/
				{
					if (ImGui::Button(ICON_FA_SYNC))
						RefreshDir();
				}
				ImGui::SameLine();
				/*******************************************************/
			}
			ImGui::PopFont();
		}
		else
		{
			ImGui::BeginDisabled();
			ImGui::PushFont(editor->icons);
			{
				ImGui::Button(ICON_FA_ARROW_LEFT);
				ImGui::SameLine();
				ImGui::Button(ICON_FA_ARROW_RIGHT);
				ImGui::SameLine();
				ImGui::Button(ICON_FA_SYNC);
				ImGui::SameLine();
			}
			ImGui::PopFont();
			ImGui::EndDisabled();
		}

		static char str1[128] = "";
		ImGui::PushItemWidth(128.0f);
		ImGui::InputTextWithHint("##AssetsSearch", "Search...", str1, IM_ARRAYSIZE(str1));
		ImGui::PopItemWidth();
		ImGui::SameLine();

		size_t pos = nowDirPath.find(FOLDER);
		std::string str = nowDirPath.substr(pos + 1);

		ImGui::Text(str.c_str());
		ImGui::PopStyleVar();
		ImGui::Separator();

		///////////////////////////////////////////////////////

		ImGui::BeginChild("table111", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextRow();
				/***** LEFT  *****/
				ImGui::TableSetColumnIndex(0);
				{
					ImGui::BeginChild("AAPPOO");
					{
						RenderDirList(dirs);
					}
					ImGui::EndChild();
				}

				/***** RIGHT *****/
				ImGui::TableSetColumnIndex(1);
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

					///////////////////////////////////////////////////////

					if (!files.empty())
					{
						ImGui::BeginChild("Assets");
						{
							float space = size;
							for (size_t i = 0; i < files.size(); i++)
							{
								ImGui::BeginGroup();
								ImGui::PushID((int)i);
								{
									if (files[i].is_selected)
										ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);

									///////////////////////////////////////

									if (files[i].is_dir) /* is dir */
									{
										ImGui::ImageButton((void*)folderTexture, ImVec2(size, size));
									}
									else if (Star::ImageFormatCheck(files[i].file_type.c_str()))
									{
										if (files[i].texture) /* if active */
										{
											ImGui::ImageButton((void*)files[i].texture, ImVec2(size, size));
										}
										else
										{
											ImGui::ImageButton((void*)imageTexture, ImVec2(size, size));
										}
									}
									else if (files[i].file_type == LUA)
									{
										ImGui::ImageButton((void*)luaTexture, ImVec2(size, size));
									}
									else if (files[i].file_type == OBJ || files[i].file_type == FBX || files[i].file_type == GLTF || files[i].file_type == GLB)
									{
										ImGui::ImageButton((void*)modelTexture, ImVec2(size, size));
									}
									else if (files[i].file_type == TTF || files[i].file_type == SPRITEFONT)
									{
										ImGui::ImageButton((void*)ttfTexture, ImVec2(size, size));
									}
									else if (files[i].file_type == WAV)
									{
										ImGui::ImageButton((void*)audioTexture, ImVec2(size, size));
									}
									else if (files[i].file_type == SKY)
									{
										ImGui::ImageButton((void*)skyTexture, ImVec2(size, size));
									}
									else if (files[i].file_type == MAT)
									{
										ImGui::ImageButton((void*)materialTexture, ImVec2(size, size));
									}
									else /* any file */
									{
										ImGui::ImageButton((void*)fileTexture, ImVec2(size, size));
									}

									///////////////////////////////////////

									//// DRAG ////
									if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
									{
										ImGui::SetDragDropPayload("DND_DEMO_ASS", &files[i], sizeof(FILEs));

										///////////////////////////////////////

										if (files[i].is_dir) /* is dir */
										{
											ImGui::Image((void*)folderTexture, ImVec2(size, size));
										}
										else if (Star::ImageFormatCheck(files[i].file_type.c_str()))
										{
											if (files[i].texture) /* if active */
											{
												ImGui::Image((void*)files[i].texture, ImVec2(size, size));
											}
											else
											{
												ImGui::Image((void*)imageTexture, ImVec2(size, size));
											}
										}
										else if (files[i].file_type == LUA)
										{
											ImGui::Image((void*)luaTexture, ImVec2(size, size));
										}
										else if (files[i].file_type == OBJ || files[i].file_type == FBX || files[i].file_type == GLTF || files[i].file_type == GLB)
										{
											ImGui::Image((void*)modelTexture, ImVec2(size, size));
										}
										else if (files[i].file_type == TTF || files[i].file_type == SPRITEFONT)
										{
											ImGui::Image((void*)ttfTexture, ImVec2(size, size));
										}
										else if (files[i].file_type == WAV)
										{
											ImGui::Image((void*)audioTexture, ImVec2(size, size));
										}
										else if (files[i].file_type == SKY)
										{
											ImGui::Image((void*)skyTexture, ImVec2(size, size));
										}
										else if (files[i].file_type == MAT)
										{
											ImGui::Image((void*)materialTexture, ImVec2(size, size));
										}
										else /* any file */
										{
											ImGui::Image((void*)fileTexture, ImVec2(size, size));
										}

										///////////////////////////////////////

										ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size);
										ImGui::TextWrapped(files[i].file_name_only.c_str());
										ImGui::PopTextWrapPos();
										//ImGui::Text(files[i].file_name.c_str());
										ImGui::EndDragDropSource();
									}

									///////////////////////////////////////

									if (files[i].is_selected)
										ImGui::PopStyleColor(1);

									///////////////////////////////////////

									if (ImGui::IsItemHovered())
									{
										if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
										{
											if (files[i].is_dir)
											{
												std::string buffer = nowDirPath + "\\" + files[i].file_name;
												OpenDir(buffer);

												ImGui::PopID();
												ImGui::EndGroup();
												break;
											}

											std::string type = nowDirPath + "\\" + files[i].file_name;
											type = Star::GetFileExtensionFromPath(type);
											std::string buffer = nowDirPath + "\\" + files[i].file_name;
											std::string exe = Star::GetParent(Star::GetExecutablePath());
											std::string x = Star::GetRelativePath(buffer, exe);

											if (type.compare(OBJ) == 0 || type.compare(FBX) == 0 || type.compare(GLTF) == 0 || type.compare(GLB) == 0)
											{
												if (ecs->selected != entt::null)
												{
													// old
													//assimpLoader->LoadModel(x.c_str(), ecs->selected);
													importer->ImportModel(x.c_str(), ecs->selected);

												}
												else
												{
													// old
													//assimpLoader->LoadModel(x.c_str(), ecs->root);
													importer->ImportModel(x.c_str(), ecs->root);
												}
											}

											if (type.compare(LUA) == 0)
											{
												if (settingsWindow->GetEditorProgram() == 0)
												{
													// text editor
													std::string comm = "notepad " + buffer;
													Star::StartCommand(comm);
												}
												else if (settingsWindow->GetEditorProgram() == 1)
												{
													// visual studio
													std::string comm = "start devenv " + buffer + " /Edit";
													Star::StartCommand(comm);
												}
												else if (settingsWindow->GetEditorProgram() == 2)
												{
													// visual studio code
													std::string comm = "code " + buffer;
													Star::StartCommand(comm);
												}
											}
										}
										else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
										{
											HideSelected();
											files[i].is_selected = true;
											selectedFile = &files[i];
										}
									}

									///////////////////////////////////////////////////////
									ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size);
									ImGui::TextWrapped(files[i].file_name_only.c_str());
									ImGui::PopTextWrapPos();
								}
								ImGui::PopID();
								ImGui::EndGroup();

								space += ImGui::GetItemRectSize().x;
								if (space < ImGui::GetWindowSize().x)
									ImGui::SameLine();
								else
									space = size;
							}

							if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
								if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
									HideSelected();
						}
						ImGui::EndChild();
					}
					ImGui::PopStyleVar(4);

					if (files.empty())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
						ImGui::Text("This folder is empty.");
						ImGui::PopStyleColor(1);
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void AssetsWindow::OpenDir(std::string path)
{
	assert(!path.empty());

	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	//assert(std::filesystem::exists(path));

	if (!outCore)
		return;

	outCore = false;

	selectedFile = nullptr;         /* selected file */
	nowDirPath = path;              /* set new path */
	ReleaseTexture();               /* release all texture */
	files.clear();                  /* -//- */
	dirs.dir_name = projectDirPath; /* -//- */
	dirs.dir_child.clear();         /* -//- */
	FillDirList(&dirs);             /* -//- */

	//// dir ////
	for (const auto& index : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path file(index.path());

		if (index.is_directory())
		{
			//// vector ////
			files.push_back(FILEs(
				file.filename().string(), // file name
				file.stem().string(), // file name only
				file.extension().string(), // file type
				index.is_directory(), // is dir
				0)); // file size
		}
	}

	//// file ////
	for (const auto& index : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path file(index.path());

		if (!index.is_directory())
		{
			//// vector ////
			files.push_back(FILEs(
				file.filename().string(), // file name
				file.stem().string(), // file name only
				file.extension().string(), // file type
				index.is_directory(), // is dir
				(int)index.file_size())); // file size
		}
	}

	std::thread thread(&AssetsWindow::OutCore, this, path);
	thread.detach();
}

std::string AssetsWindow::GetAppDirPath()
{
	return std::filesystem::current_path().string();
}

std::string AssetsWindow::GetProjectDirPath()
{
	return projectDirPath;
}

std::string AssetsWindow::GetNowDirPath()
{
	return nowDirPath;
}

void AssetsWindow::GoBackDir()
{
	if (GetProjectDirPath() == GetNowDirPath())
		return;

	std::filesystem::path file(nowDirPath);
	OpenDir(file.parent_path().string());
}

void AssetsWindow::RefreshDir()
{
	OpenDir(nowDirPath);
}

void AssetsWindow::Shutdown()
{
	ReleaseTexture();

	if (folderTexture) folderTexture->Release();
	if (imageTexture) imageTexture->Release();
	if (fileTexture) fileTexture->Release();
	if (luaTexture) luaTexture->Release();
	if (modelTexture) modelTexture->Release();
	if (ttfTexture) ttfTexture->Release();
	if (audioTexture) audioTexture->Release();
	if (skyTexture) skyTexture->Release();
	if (materialTexture) materialTexture->Release();
}

void AssetsWindow::OutCore(std::string path)
{
	using namespace DirectX;

	for (size_t i = 0; i < files.size(); i++)
	{
		if (!thumbnail)
			break;

		if (!files[i].is_dir)
		{
			if (Star::ImageFormatCheck(files[i].file_type.c_str()))
			{
				std::string buffer = nowDirPath + "\\" + files[i].file_name;

				ScratchImage normal_image;
				ScratchImage resized_image;
				ScratchImage converted_image;

				///////////////////////////////////////////////

				if (Star::ImageFormatCheck(files[i].file_type.c_str()))
				{
					if (FAILED(LoadFromWICFile(
						Star::ConvertString(buffer).c_str(),
						WIC_FLAGS_IGNORE_SRGB,
						nullptr,
						normal_image)))
						continue;
				}

				///////////////////////////////////////////////

				if (files[i].file_type == DDS)
				{
					if (FAILED(LoadFromDDSFile(
						Star::ConvertString(buffer).c_str(),
						DDS_FLAGS_NONE,
						nullptr,
						normal_image)))
						continue;
				}

				///////////////////////////////////////////////

				if (FAILED(Resize(
					normal_image.GetImages(),
					normal_image.GetImageCount(),
					normal_image.GetMetadata(),
					(size_t)size, (size_t)size, TEX_FILTER_DEFAULT, resized_image)))
					continue;

				///////////////////////////////////////////////

				if (FAILED(Convert(
					resized_image.GetImages(),
					resized_image.GetImageCount(),
					resized_image.GetMetadata(),
					DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT,
					converted_image)))
					continue;

				///////////////////////////////////////////////

				if (FAILED(CreateShaderResourceView(dx->dxDevice,
					converted_image.GetImages(),
					converted_image.GetImageCount(),
					converted_image.GetMetadata(),
					&files[i].texture)))
					continue;

				///////////////////////////////////////////////

				normal_image.Release();
				resized_image.Release();
				converted_image.Release();
			}
		}
	}
	outCore = true;
}

void AssetsWindow::ReleaseTexture()
{
	for (size_t i = 0; i < files.size(); i++)
		if (files[i].texture)
			files[i].texture->Release();
}

void AssetsWindow::CreateDir(std::string path)
{
	assert(!path.empty());
	//assert(!std::filesystem::exists(path));
	std::filesystem::create_directory(path);
}

FILEs* AssetsWindow::GetSelFile()
{
	return selectedFile;
}

void AssetsWindow::HideSelected()
{
	for (size_t i = 0; i < files.size(); i++)
		files[i].is_selected = false;

	selectedFile = nullptr;
}

void AssetsWindow::RenderDirList(dir_list dir)
{
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_DefaultOpen;

	if (!dir.dir_child.size())
		tree_flags |= ImGuiTreeNodeFlags_Leaf;

	if (!dir.dir_name.compare(nowDirPath))
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	size_t pos = dir.dir_name.find(FOLDER);
	std::string str = dir.dir_name.substr(pos + 1);

	bool node_open = ImGui::TreeNodeEx((void*)NULL, tree_flags, str.c_str());

	if (ImGui::IsItemHovered())
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			OpenDir(dir.dir_name);

	if (node_open)
	{
		for (size_t i = 0; i < dir.dir_child.size(); i++)
			RenderDirList(dir.dir_child[i]);
		ImGui::TreePop();
	}
}

void AssetsWindow::FillDirList(dir_list* dir)
{
	for (const auto& index : std::filesystem::directory_iterator(dir->dir_name))
	{
		if (index.is_directory())
		{
			dir_list entry;
			entry.dir_name = index.path().string();
			dir->dir_child.push_back(entry);
		}
	}

	for (size_t i = 0; i < dir->dir_child.size(); i++)
		FillDirList(&dir->dir_child[i]);
}

void AssetsWindow::RemoveAsset(std::string path)
{
	std::string fix = path + std::string(1, '\0');
	std::wstring buff = std::wstring(fix.begin(), fix.end());
	/********/
	SHFILEOPSTRUCT sh;
	ZeroMemory(&sh, sizeof(SHFILEOPSTRUCT));
	sh.wFunc = FO_DELETE;
	sh.fFlags = FOF_ALLOWUNDO;
	sh.pFrom = buff.c_str();
	SHFileOperation(&sh);
}

bool AssetsWindow::GetOutCore()
{
	return outCore;
}

unsigned int AssetsWindow::GetSafeName(std::string path, std::string type)
{
	unsigned int i = 0;
	while (true)
	{
		std::string str = path + std::to_string(i) + type;
		if (!std::filesystem::exists(str))
			break;
		i++;
	}
	return i;
}

void AssetsWindow::GetFileNameFromProjectDir(std::string path, std::string extension, std::vector<std::pair<std::string, std::string>>& data)
{
	for (const auto& index : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path file(index.path());

		if (index.is_directory())
		{
			GetFileNameFromProjectDir(index.path().string(), extension, data);
		}
		else
		{
			if (!file.extension().string().compare(extension))
				data.push_back(std::pair(index.path().string(), file.stem().string()));
		}
	}
}