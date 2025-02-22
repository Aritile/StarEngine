#include "ConvexColliderComponent.h"
#include "physx/cooking/PxConvexMeshDesc.h"
#include "MeshComponent.h"
#include "../Entity.h"
#include "physx/extensions/PxDefaultStreams.h"
#include "physx/cooking/PxCooking.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../STAR/Star.h"

static Entity* ecs = Entity::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

bool ConvexColliderComponent::Create()
{
    entt::entity entity = entt::to_entity(ecs->registry, *this);
    if (!ecs->HasComponent<MeshComponent>(entity))
        return false;

    auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);

    // Describe convex mesh
    physx::PxConvexMeshDesc desc;
    desc.points.count = (UINT)meshComponent.meshStorageBuffer->vertices.size();
    desc.points.stride = sizeof(Vertex);
    desc.points.data = &meshComponent.meshStorageBuffer->vertices[0].position;
    desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
    //convexDesc.indices.data = meshComponent.meshStorageBuffer->indices.data();
    //convexDesc.indices.count = (UINT)meshComponent.meshStorageBuffer->indices.size();
    //convexDesc.indices.stride = sizeof(UINT);

    // Cook the convex mesh
    physx::PxDefaultMemoryOutputStream outBuffer;
    physx::PxConvexMeshCookingResult::Enum result;
    if (!physicsSystem->GetCooking()->cookConvexMesh(desc, outBuffer, &result))
    {
        Star::AddLog("[ConvexCollider] -> Cook Convex Mesh Failed.");
        return false;
    }
    else
    {
        Star::AddLog("[ConvexCollider] -> Cooking Convex Mesh..");
    }

    physx::PxDefaultMemoryInputData input(outBuffer.getData(), outBuffer.getSize());
    physx::PxConvexMesh* convexMesh = physicsSystem->GetPhysics()->createConvexMesh(input);
    if (!convexMesh)
    {
        Star::AddLog("[ConvexCollider] -> Create Convex Mesh Failed.");
        return false;
    }
    else
    {
        Star::AddLog("[ConvexCollider] -> Creating Convex Mesh..");
    }

    if (!ecs->HasComponent<PhysicsMaterialComponent>(entity))
    {
        ecs->AddComponent<PhysicsMaterialComponent>(entity);
        ecs->GetComponent<PhysicsMaterialComponent>(entity).CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
    }
    auto& physicsMaterialComponent = ecs->GetComponent<PhysicsMaterialComponent>(entity);

    // Create convex shape
    physx::PxConvexMeshGeometry convexGeom(convexMesh);
    pxShape = physicsSystem->GetPhysics()->createShape(convexGeom, *physicsMaterialComponent.GetMaterial());
    if (!pxShape)
    {
        Star::AddLog("[ConvexCollider] -> Create Convex Shape Failed.");
        return false;
    }

    return true;
}

void ConvexColliderComponent::Render()
{
	ImGui::Checkbox("##CONVEXCOLLIDER", &activeComponent);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("CONVEX COLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{

			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("PhysicsComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}