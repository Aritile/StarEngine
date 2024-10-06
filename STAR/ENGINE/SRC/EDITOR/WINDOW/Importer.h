#pragma once

#include <string>
#include <entt/entt.hpp>

class Importer
{
public:
	static Importer* GetSingleton();

public:
	void Render();
	void ImportModel(const char* path, entt::entity entity);
	void Reset();

private:
	int radio = 2;

	// path
	std::string model;
	entt::entity entity = entt::null;

	// flags
	bool findInstances = false;
	bool findInvalidData = false;
	bool fixInfacingNormals = false;
	bool genNormals = false;
	bool genSmoothNormals = false;
	bool genUVCoords = false;
	bool improveCacheLocality = false;
	bool optimizeGraph = false;
	bool optimizeMeshes = false;
	bool removeRedundantMaterials = false;
	bool splitLargeMeshes = false;

	// info
	std::string findInstancesInfo = "This step searches for duplicate meshes and replaces them with references to the first mesh.";
	std::string findInvalidDataInfo = "This step searches all meshes for invalid data, such as zeroed normal vectors or invalid UV coords and removes / fixes them. This is intended to get rid of some common exporter errors.";
	std::string fixInfacingNormalsInfo = "This step tries to determine which meshes have normal vectors that are facing inwards and inverts them.";
	std::string genNormalsInfo = "Generates normals for all faces of all meshes.";
	std::string genSmoothNormalsInfo = "Generates smooth normals for all vertices in the mesh.";
	std::string genUVCoordsInfo = "This step converts non-UV mappings (such as spherical or cylindrical mapping) to proper texture coordinate channels.";
	std::string improveCacheLocalityInfo = "Reorders triangles for better vertex cache locality.";
	std::string optimizeGraphInfo = "A post-processing step to optimize the scene hierarchy.";
	std::string optimizeMeshesInfo = "A post-processing step to reduce the number of meshes.";
	std::string removeRedundantMaterialsInfo = "Searches for redundant/unreferenced materials and removes them.";
	std::string splitLargeMeshesInfo = "Splits large meshes into smaller sub-meshes.";
};