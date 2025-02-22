#include "TriangleColliderComponent.h"
#include "physx/cooking/PxConvexMeshDesc.h"
#include "MeshComponent.h"
#include "../Entity.h"
#include "physx/extensions/PxDefaultStreams.h"
#include "physx/cooking/PxCooking.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../STAR/Star.h"

static Entity* ecs = Entity::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

bool TriangleColliderComponent::Create()
{
    entt::entity entity = entt::to_entity(ecs->registry, *this);
    if (!ecs->HasComponent<MeshComponent>(entity))
        return false;

    auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);

    // Describe the triangle mesh
    physx::PxTriangleMeshDesc desc;
    desc.points.count = (UINT)meshComponent.meshStorageBuffer->vertices.size();
    desc.points.stride = sizeof(Vertex);
    desc.points.data = &meshComponent.meshStorageBuffer->vertices[0].position;
    desc.triangles.count = (UINT)(meshComponent.meshStorageBuffer->indices.size() / 3);
    desc.triangles.stride = 3 * sizeof(UINT);
    desc.triangles.data = meshComponent.meshStorageBuffer->indices.data();

    // Cook the triangle mesh
    physx::PxDefaultMemoryOutputStream outBuffer;
    physx::PxTriangleMeshCookingResult::Enum result;
    if (!physicsSystem->GetCooking()->cookTriangleMesh(desc, outBuffer, &result))
    {
        Star::AddLog("[TriangleCollider] -> Cook Triangle Mesh Failed.");
        return false;
    }
    else
    {
        Star::AddLog("[TriangleCollider] -> Cooking Triangle Mesh..");
    }

    physx::PxDefaultMemoryInputData input(outBuffer.getData(), outBuffer.getSize());
    physx::PxTriangleMesh* triangleMesh = physicsSystem->GetPhysics()->createTriangleMesh(input);
    if (!triangleMesh)
    {
        Star::AddLog("[TriangleCollider] -> Create Triangle Mesh Failed.");
        return false;
    }
    else
    {
        Star::AddLog("[TriangleCollider] -> Creating Triangle Mesh..");
    }

    if (!ecs->HasComponent<PhysicsMaterialComponent>(entity))
    {
        ecs->AddComponent<PhysicsMaterialComponent>(entity);
        ecs->GetComponent<PhysicsMaterialComponent>(entity).CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
    }
    auto& physicsMaterialComponent = ecs->GetComponent<PhysicsMaterialComponent>(entity);

    // Create triangle mesh shape
    physx::PxTriangleMeshGeometry triangleGeometry(triangleMesh);
    pxShape = physicsSystem->GetPhysics()->createShape(triangleGeometry, *physicsMaterialComponent.GetMaterial());
    if (!pxShape)
    {
        Star::AddLog("[TriangleCollider] -> Create Triangle Shape Failed.");
        return false;
    }

    return true;
}

void TriangleColliderComponent::Render()
{
    ImGui::Checkbox("##CONVEXCOLLIDER", &activeComponent);
    ImGui::SameLine();
    if (ImGui::CollapsingHeader("TRIANGLE COLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
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