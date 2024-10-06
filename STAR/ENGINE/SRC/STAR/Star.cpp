#include "Star.h"
#include <string>
#include <stdio.h>
#include <filesystem>
#include "../DX/DX.h"
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_stdlib.h"
#include <shobjidl.h>
#include <comdef.h>
#include "../MAIN/Main.h"
#include "../WINDOW/MainWindow.h"
#include "../ENGINE/Engine.h"
#include "../EDITOR/WINDOW/Assets.h"

static DX* dx = DX::GetSingleton();
static MainWindow* mainWindow = MainWindow::GetSingleton();
static Engine* engine = Engine::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();

UINT Star::GetDisplayWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}
UINT Star::GetDisplayHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

void Star::AddLog(const char* text, ...)
{
	char buff[MAX_PATH];
	va_list args;
	va_start(args, text);
	vsnprintf(buff, MAX_PATH, text, args);
	va_end(args);
	OutputDebugStringA(buff);
	OutputDebugStringA("\n");
	printf("%s\n", buff);
}

HRESULT Star::CompileShaderFromFile(std::wstring srcFile, std::string entryPoint, std::string profile, ID3DBlob** blob)
{
	assert(!srcFile.empty());
	assert(!entryPoint.empty());
	assert(!profile.empty());
	assert(blob);

	if (srcFile.empty() || entryPoint.empty() || profile.empty() || !blob)
		return E_INVALIDARG;

	///////////////////////////////////////////////////////////

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(srcFile.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(), profile.c_str(),
		flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			Star::AddLog((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
			PostQuitMessage(0);
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	///////////////////////////////////////////////////////////

	return hr;
}
HRESULT Star::CompileShaderFromSource(const char* data, std::string entryPoint, std::string profile, ID3DBlob** blob)
{
	assert(data);
	assert(!entryPoint.empty());
	assert(!profile.empty());
	assert(blob);

	if (!data || entryPoint.empty() || profile.empty() || !blob)
		return E_INVALIDARG;

	///////////////////////////////////////////////////////////

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(data, strlen(data), NULL, NULL, NULL, entryPoint.c_str(), profile.c_str(), flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			Star::AddLog((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	///////////////////////////////////////////////////////////

	return hr;
}

std::wstring Star::ConvertString(std::string buffer)
{
	return std::wstring(buffer.begin(), buffer.end());
}

Vector3 Star::ToRadians(Vector3 rotation)
{
	using namespace DirectX;
	return Vector3(
		XMConvertToRadians(rotation.x),
		XMConvertToRadians(rotation.y),
		XMConvertToRadians(rotation.z));
}
Vector3 Star::ToDegrees(Vector3 rotation)
{
	using namespace DirectX;
	return Vector3(
		XMConvertToDegrees(rotation.x),
		XMConvertToDegrees(rotation.y),
		XMConvertToDegrees(rotation.z));
}

physx::PxVec3 Star::Vector3ToPhysics(Vector3 value)
{
	return physx::PxVec3(value.x, value.y, value.z);
}
physx::PxQuat Star::QuatToPhysics(Quaternion value)
{
	return physx::PxQuat(value.x, value.y, value.z, value.w);
}
Vector3 Star::PhysicsToVector3(physx::PxVec3 value)
{
	return Vector3(value.x, value.y, value.z);
}
Quaternion Star::PhysicsToQuat(physx::PxQuat value)
{
	return Quaternion(value.x, value.y, value.z, value.w);
}
physx::PxMat44 Star::MatrixToPhysics(Matrix value)
{
	physx::PxMat44 matrix;

	matrix[0][0] = value.m[0][0];
	matrix[0][1] = value.m[0][1];
	matrix[0][2] = value.m[0][2];
	matrix[0][3] = value.m[0][3];

	matrix[1][0] = value.m[1][0];
	matrix[1][1] = value.m[1][1];
	matrix[1][2] = value.m[1][2];
	matrix[1][3] = value.m[1][3];

	matrix[2][0] = value.m[2][0];
	matrix[2][1] = value.m[2][1];
	matrix[2][2] = value.m[2][2];
	matrix[2][3] = value.m[2][3];

	matrix[3][0] = value.m[3][0];
	matrix[3][1] = value.m[3][1];
	matrix[3][2] = value.m[3][2];
	matrix[3][3] = value.m[3][3];

	return matrix;
}
Matrix Star::PhysicsToMatrix(physx::PxMat44 value)
{
	Matrix matrix;

	matrix.m[0][0] = value[0][0];
	matrix.m[0][1] = value[0][1];
	matrix.m[0][2] = value[0][2];
	matrix.m[0][3] = value[0][3];

	matrix.m[1][0] = value[1][0];
	matrix.m[1][1] = value[1][1];
	matrix.m[1][2] = value[1][2];
	matrix.m[1][3] = value[1][3];

	matrix.m[2][0] = value[2][0];
	matrix.m[2][1] = value[2][1];
	matrix.m[2][2] = value[2][2];
	matrix.m[2][3] = value[2][3];

	matrix.m[3][0] = value[3][0];
	matrix.m[3][1] = value[3][1];
	matrix.m[3][2] = value[3][2];
	matrix.m[3][3] = value[3][3];

	return matrix;
}
physx::PxTransform Star::PositionRotationToPhysics(Vector3 position, Quaternion rotation)
{
	return physx::PxTransform(
		position.x, position.y, position.z,
		physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
}

const char* Star::OpenFileDialog(LPCWSTR dir, LPCWSTR filter, LPCWSTR title)
{
	wchar_t wtext[MAX_PATH];
	ZeroMemory(&wtext, sizeof(wtext));

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = mainWindow->GetHandle();
	ofn.lpstrFile = wtext;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = title;
	GetOpenFileName(&ofn);

	_bstr_t b(wtext);
	return b;
}
const char* Star::SaveFileDialog(LPCWSTR dir, LPCWSTR filter, LPCWSTR title)
{
	wchar_t wtext[MAX_PATH];
	ZeroMemory(&wtext, sizeof(wtext));

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = mainWindow->GetHandle();
	ofn.lpstrFile = wtext;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = title;
	GetSaveFileName(&ofn);

	_bstr_t b(wtext);
	return b;
}

void Star::SerializeVector2(YAML::Emitter& out, Vector2 value)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << value.x << value.y << YAML::EndSeq;
}
void Star::SerializeVector3(YAML::Emitter& out, Vector3 value)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << value.x << value.y << value.z << YAML::EndSeq;
}
void Star::SerializeVector4(YAML::Emitter& out, Vector4 value)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << value.x << value.y << value.z << value.w << YAML::EndSeq;
}
void Star::SerializeQuaternion(YAML::Emitter& out, Quaternion value)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << value.x << value.y << value.z << value.w << YAML::EndSeq;
}
void Star::SerializeMatrix(YAML::Emitter& out, Matrix matrix)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << matrix.m[0][0] << matrix.m[0][1] << matrix.m[0][2] << matrix.m[0][3] << matrix.m[1][0] << matrix.m[1][1] << matrix.m[1][2] << matrix.m[1][3] << matrix.m[2][0] << matrix.m[2][1] << matrix.m[2][2] << matrix.m[2][3] << matrix.m[3][0] << matrix.m[3][1] << matrix.m[3][2] << matrix.m[3][3] << YAML::EndSeq;
}

Vector2 Star::DeserializeVector2(YAML::Node& in)
{
	Vector2 value;
	if (in.IsSequence())
	{
		value.x = in[0].as<float>();
		value.y = in[1].as<float>();
	}
	return value;
}
Vector3 Star::DeserializeVector3(YAML::Node& in)
{
	Vector3 value;
	if (in.IsSequence())
	{
		value.x = in[0].as<float>();
		value.y = in[1].as<float>();
		value.z = in[2].as<float>();
	}
	return value;
}
Vector4 Star::DeserializeVector4(YAML::Node& in)
{
	Vector4 value;
	if (in.IsSequence())
	{
		value.x = in[0].as<float>();
		value.y = in[1].as<float>();
		value.z = in[2].as<float>();
		value.w = in[3].as<float>();
	}
	return value;
}
Quaternion Star::DeserializeQuaternion(YAML::Node& in)
{
	Quaternion value;
	if (in.IsSequence())
	{
		value.x = in[0].as<float>();
		value.y = in[1].as<float>();
		value.z = in[2].as<float>();
		value.w = in[3].as<float>();
	}
	return value;
}
Matrix Star::DeserializeMatrix(YAML::Node& in)
{
	Matrix matrix;

	if (in.IsSequence())
	{
		matrix.m[0][0] = in[0].as<float>();
		matrix.m[0][1] = in[1].as<float>();
		matrix.m[0][2] = in[2].as<float>();
		matrix.m[0][3] = in[3].as<float>();

		matrix.m[1][0] = in[4].as<float>();
		matrix.m[1][1] = in[5].as<float>();
		matrix.m[1][2] = in[6].as<float>();
		matrix.m[1][3] = in[7].as<float>();

		matrix.m[2][0] = in[8].as<float>();
		matrix.m[2][1] = in[9].as<float>();
		matrix.m[2][2] = in[10].as<float>();
		matrix.m[2][3] = in[11].as<float>();

		matrix.m[3][0] = in[12].as<float>();
		matrix.m[3][1] = in[13].as<float>();
		matrix.m[3][2] = in[14].as<float>();
		matrix.m[3][3] = in[15].as<float>();
	}

	return matrix;
}

float Star::RadToDeg(float value)
{
	return DirectX::XMConvertToDegrees(value);
}
float Star::DegToRad(float value)
{
	return DirectX::XMConvertToRadians(value);
}

uLong Star::GetChecksum(const char* filename)
{
	std::FILE* file = std::fopen(filename, "rb");
	std::vector<unsigned char> data;
	if (file)
	{
		std::fseek(file, 0, SEEK_END);
		data.resize(std::ftell(file));
		std::rewind(file);
		std::fread(&data[0], 1, data.size(), file);
		std::fclose(file);

		return crc32(0, data.data(), (uInt)data.size());
	}

	return 0;
}

void Star::BeginFormat(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	{
		std::string dateTime = __DATE__ + std::string(" ") + std::string(__TIME__);
		out << YAML::Comment("do not edit\n" + dateTime);
		out << YAML::Key << "Star" << YAML::Value << YAML::BeginMap;
		{
			out << YAML::Key << "Version" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Major" << YAML::Value << MAJOR;
				out << YAML::Key << "Minor" << YAML::Value << MINOR;
				out << YAML::Key << "Patch" << YAML::Value << PATCH;
			}
			out << YAML::EndMap;
			out << YAML::Key << "Data" << YAML::Value << YAML::BeginMap;
			{

			}
			//out << YAML::EndMap;
		}
		//out << YAML::EndMap;
	}
	//out << YAML::EndMap;
}
void Star::EndFormat(YAML::Emitter& out)
{
	out << YAML::EndMap;
	out << YAML::EndMap;
	out << YAML::EndMap;
}

bool Star::CheckSignature(YAML::Node& in)
{
	if (in["Star"])
		return true;
	else
		return false;
}

std::string Star::GetFileNameFromPath(std::string path)
{
	return std::filesystem::path(path).stem().string();
}
std::string Star::GetFileExtensionFromPath(std::string path)
{
	return std::filesystem::path(path).extension().string();
}
std::string Star::GetExecutablePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return buffer;
}
std::string Star::GetRelativePath(std::string _Path, std::string _Base)
{
	std::filesystem::path path(_Path);
	std::filesystem::path base(_Base);
	return std::filesystem::relative(path, base).string();
}
std::string Star::GetParent(std::string path)
{
	return std::filesystem::path(path).parent_path().string();
}
std::string Star::NormalizePath(std::string path)
{
	return std::filesystem::weakly_canonical(path).string();
}
void Star::CreateFolder(std::string path)
{
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}
}

const aiScene* Star::OpenModel(Assimp::Importer* importer, const char* path)
{
	if (!path)
	{
		printf("path is null\n");
		return NULL;
	}

	const aiScene* scene = importer->ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene)
	{
		printf("scene is null\n");
		return NULL;
	}

	return scene;
}
physx::PxReal Star::FloatToPhysics(float value)
{
	return value;
}
float Star::PhysicsToFloat(physx::PxReal value)
{
	return value;
}
void Star::StartCommand(std::string string)
{
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	LPSTR lpstr = const_cast<LPSTR>(string.c_str());

	if (CreateProcessA(NULL, lpstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		printf("StartCommand() failed\n");
	}
}
std::string Star::SlashesFix(std::string string)
{
	std::string output = string;
	for (char& c : output)
		if (c == '\\')
			c = '/';
	return output;
}
bool Star::FileExist(std::string path)
{
	std::filesystem::path file(path);
	return std::filesystem::exists(file);
}
static bool isCursorVisible = true;
void Star::ShowCursor(bool _Show)
{
	if (_Show && !isCursorVisible)
	{
		::ShowCursor(TRUE);
		isCursorVisible = true;
	}
	else if (!_Show && isCursorVisible)
	{
		::ShowCursor(FALSE);
		isCursorVisible = false;
	}
}
bool Star::IsCursorShowed()
{
	return isCursorVisible;
}
bool Star::ImageFormatCheck(std::string _FileType)
{
	if (_FileType.compare(PNG) == 0 || _FileType.compare(DDS) == 0 || _FileType.compare(JPEG) == 0 || _FileType.compare(JPG) == 0 || _FileType.compare(HDR) == 0)
		return true;

	return false;
}
std::string Star::FixPath(std::string _Filename)
{
	std::string buffer = assetsWindow->GetNowDirPath() + "\\" + _Filename;
	std::string exe = Star::GetParent(Star::GetExecutablePath());
	std::string x = Star::GetRelativePath(buffer, exe);
	return x;
}
void Star::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}