#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../DX/DX.h"
#include "../../IMGUI/imgui.h"
#include "../../ENTITY/Entity.h"
#include "../../XTK/MATH/SimpleMath.h"
#include "../../IMGUI/imgui_stdlib.h"
#include <yaml-cpp/yaml.h>
#include <wrl/client.h>
#include <sol/sol.hpp>
#include "../../VERTEX/Vertex.h"
#include "../../MATERIAL/Material.h"
#include "../../CONSTANTDATA/ConstantData.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../STORAGE/MeshStorage.h"

using namespace DirectX::SimpleMath;

struct MeshComponent
{
public:
	void Render();
	void DrawMesh(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

private:
	bool activeComponent = true;

public:
	void SetActive(bool arg);
	bool IsActive();

private:
	void CreateBoundingBox();

public:
	ModelStorageBuffer* modelStorageBuffer = nullptr;
	MeshStorageBuffer* meshStorageBuffer = nullptr;

	UINT GetNumVertices();
	UINT GetNumIndices();
	UINT GetNumFaces();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);

	std::string GetMaterialPath();

public:
	ConstantData cb;
	void ApplyModel(ModelStorageBuffer* _ModelStorageBuffer);
	void ApplyMesh(MeshStorageBuffer* _MeshStorageBuffer);

public:
	static void LuaAdd(sol::state& state);
};