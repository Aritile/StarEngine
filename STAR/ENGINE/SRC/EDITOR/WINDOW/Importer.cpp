#include "Importer.h"
#include "../../IMGUI/imgui.h"
#include "../../STAR/Star.h"
#include "../../MODEL/AssimpLoader.h"

static AssimpLoader* assimpLoader = AssimpLoader::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();

Importer* Importer::GetSingleton()
{
	static Importer importer;
	return &importer;
}

void Importer::ImportModel(const char* path, entt::entity entity)
{
	if (meshStorage->IsModelAlreadyLoaded(path))
	{
		assimpLoader->LoadModel(path, entity);
		return;
	}

	model = path;
	this->entity = entity;
}
void Importer::Render()
{
	if (!model.empty())
	{
		unsigned int flags = 0;

		ImGui::Begin("Import Model");
		{
			ImGui::Text("Flags");
			if (radio != 0)
				ImGui::BeginDisabled();
			ImGui::Checkbox("Find Instances", &findInstances);
			ImGui::SameLine(); Star::HelpMarker(findInstancesInfo.c_str());
			ImGui::Checkbox("Find Invalid Data", &findInvalidData);
			ImGui::SameLine(); Star::HelpMarker(findInvalidDataInfo.c_str());
			ImGui::Checkbox("Fix Infacing Normals", &fixInfacingNormals);
			ImGui::SameLine(); Star::HelpMarker(fixInfacingNormalsInfo.c_str());
			ImGui::Checkbox("Gen Normals", &genNormals);
			ImGui::SameLine(); Star::HelpMarker(genNormalsInfo.c_str());
			ImGui::Checkbox("Gen Smooth Normals", &genSmoothNormals);
			ImGui::SameLine(); Star::HelpMarker(genSmoothNormalsInfo.c_str());
			ImGui::Checkbox("Gen UV Coords", &genUVCoords);
			ImGui::SameLine(); Star::HelpMarker(genUVCoordsInfo.c_str());
			ImGui::Checkbox("Improve Cache Locality", &improveCacheLocality);
			ImGui::SameLine(); Star::HelpMarker(improveCacheLocalityInfo.c_str());
			ImGui::Checkbox("Optimize Graph", &optimizeGraph);
			ImGui::SameLine(); Star::HelpMarker(optimizeGraphInfo.c_str());
			ImGui::Checkbox("Optimize Meshes", &optimizeMeshes);
			ImGui::SameLine(); Star::HelpMarker(optimizeMeshesInfo.c_str());
			ImGui::Checkbox("Remove Redundant Materials", &removeRedundantMaterials);
			ImGui::SameLine(); Star::HelpMarker(removeRedundantMaterialsInfo.c_str());
			ImGui::Checkbox("Split Large Meshes", &splitLargeMeshes);
			ImGui::SameLine(); Star::HelpMarker(splitLargeMeshesInfo.c_str());

			if (radio != 0)
				ImGui::EndDisabled();

			ImGui::Separator();
			ImGui::Text("Presets");
			ImGui::RadioButton("None", &radio, 0);
			ImGui::RadioButton("Max Quality", &radio, 1);
			ImGui::RadioButton("Quality", &radio, 2);
			ImGui::RadioButton("Fast", &radio, 3);
			ImGui::Separator();
			if (ImGui::Button("Import"))
			{
				// process

				if (radio != 0)
				{
					if (radio == 1)
						flags |= aiProcessPreset_TargetRealtime_MaxQuality;
					else if (radio == 2)
						flags |= aiProcessPreset_TargetRealtime_Quality;
					else if (radio == 3)
						flags |= aiProcessPreset_TargetRealtime_Fast;
				}
				else
				{
					if (findInstances)
						flags |= aiProcess_FindInstances;
					if (findInvalidData)
						flags |= aiProcess_FindInvalidData;
					if (fixInfacingNormals)
						flags |= aiProcess_FixInfacingNormals;
					if (genNormals)
						flags |= aiProcess_GenNormals;
					if (genSmoothNormals)
						flags |= aiProcess_GenSmoothNormals;
					if (genUVCoords)
						flags |= aiProcess_GenUVCoords;
					if (improveCacheLocality)
						flags |= aiProcess_ImproveCacheLocality;
					if (optimizeGraph)
						flags |= aiProcess_OptimizeGraph;
					if (optimizeMeshes)
						flags |= aiProcess_OptimizeMeshes;
					if (removeRedundantMaterials)
						flags |= aiProcess_RemoveRedundantMaterials;
					if (splitLargeMeshes)
						flags |= aiProcess_SplitLargeMeshes;
				}

				assimpLoader->LoadModel(model.c_str(), entity, flags);
				Reset();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				Reset();
			}
		}
		ImGui::End();
	}
}

void Importer::Reset()
{
	model.clear();
	entity = entt::null;
}