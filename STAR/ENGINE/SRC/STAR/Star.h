#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "../XTK/MATH/SimpleMath.h"
#include <physx/PxPhysicsAPI.h>
#include <yaml-cpp/yaml.h>
#include <zlib/zlib.h>

// assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define VS_VERSION "vs_4_0"
#define PS_VERSION "ps_4_0"
#define ENTRY_POINT "main"
#define LEFT_SPACING 32
#define RIGHT_SPACING 84
#define FIXED_SPACING 64
#define BUTTON_SPACING 26

// is this good idea?
using namespace DirectX::SimpleMath;

namespace Star
{
	UINT GetDisplayWidth();
	UINT GetDisplayHeight();

	void AddLog(const char* text, ...);

	HRESULT CompileShaderFromFile(std::wstring srcFile, std::string entryPoint, std::string profile, ID3DBlob** blob);
	HRESULT CompileShaderFromSource(const char* data, std::string entryPoint, std::string profile, ID3DBlob** blob);

	std::wstring ConvertString(std::string buffer);

	Vector3 ToRadians(Vector3 rotation);
	Vector3 ToDegrees(Vector3 rotation);
	float RadToDeg(float value);
	float DegToRad(float value);

	physx::PxVec3 Vector3ToPhysics(Vector3 value);
	physx::PxQuat QuatToPhysics(Quaternion value);
	Vector3 PhysicsToVector3(physx::PxVec3 value);
	Quaternion PhysicsToQuat(physx::PxQuat value);
	physx::PxMat44 MatrixToPhysics(Matrix value);
	Matrix PhysicsToMatrix(physx::PxMat44 value);
	physx::PxTransform PositionRotationToPhysics(Vector3 position, Quaternion rotation);
	physx::PxReal FloatToPhysics(float value);
	float PhysicsToFloat(physx::PxReal value);

	const char* OpenFileDialog(LPCWSTR dir, LPCWSTR filter, LPCWSTR title);
	const char* SaveFileDialog(LPCWSTR dir, LPCWSTR filter, LPCWSTR title);

	void SerializeVector2(YAML::Emitter& out, Vector2 value);
	void SerializeVector3(YAML::Emitter& out, Vector3 value);
	void SerializeVector4(YAML::Emitter& out, Vector4 value);
	void SerializeQuaternion(YAML::Emitter& out, Quaternion value);
	void SerializeMatrix(YAML::Emitter& out, Matrix matrix);

	Matrix DeserializeMatrix(YAML::Node& in);
	Vector2 DeserializeVector2(YAML::Node& in);
	Vector3 DeserializeVector3(YAML::Node& in);
	Vector4 DeserializeVector4(YAML::Node& in);
	Quaternion DeserializeQuaternion(YAML::Node& in);

	uLong GetChecksum(const char* filename);

	void BeginFormat(YAML::Emitter& out);
	void EndFormat(YAML::Emitter& out);

	bool CheckSignature(YAML::Node& in);

	std::string GetFileNameFromPath(std::string path);
	std::string GetFileExtensionFromPath(std::string path);
	std::string GetExecutablePath();
	std::string GetRelativePath(std::string _Path, std::string _Base);
	std::string GetParent(std::string path);
	std::string NormalizePath(std::string path);
	void CreateFolder(std::string path);
	bool FileExist(std::string path);

	const aiScene* OpenModel(Assimp::Importer* importer, const char* path);
	void StartCommand(std::string string);
	std::string SlashesFix(std::string string);

	void ShowCursor(bool _Show);
	bool IsCursorShowed();
	bool ImageFormatCheck(std::string _FileType);
}