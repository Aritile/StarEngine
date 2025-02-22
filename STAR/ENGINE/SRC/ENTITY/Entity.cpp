#include "Entity.h"
#include <vector>
#include "../STAR/Star.h"
#include <WICTextureLoader11.h>

// component
#include "COMPONENT/MeshComponent.h"
#include "COMPONENT/GeneralComponent.h"
#include "COMPONENT/TransformComponent.h"
#include "COMPONENT/RigidDynamicComponent.h"
#include "COMPONENT/CameraComponent.h"
#include "COMPONENT/TextMeshComponent.h"
#include "COMPONENT/RectangleComponent.h"
#include "COMPONENT/ImageComponent.h"
#include "COMPONENT/TextComponent.h"

// system
#include "../SYSTEM/ProjectSceneSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../SYSTEM/PhysicsSystem.h"

#include "../MODEL/AssimpLoader.h"
#include "../EDITOR/WINDOW/Console.h"
#include "../STORAGE/MeshStorage.h"

// model
#define CUBE_MODEL "data\\model\\cube.obj"
#define SPHERE_MODEL "data\\model\\sphere.obj"
#define CAPSULE_MODEL "data\\model\\capsule.obj"
#define PLANE_MODEL "data\\model\\plane.obj"

Entity* Entity::GetSingleton()
{
    static Entity entity;
    return &entity;
}

static ProjectSceneSystem* projectSceneSystem = ProjectSceneSystem::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();

bool Entity::Init()
{
    root = CreateEntity();
	GetComponent<GeneralComponent>(root).SetName("Scene");
    projectSceneSystem->NewScene();
    return true;
}

void Entity::CreateEmptyEntity(entt::entity entity)
{
    std::string name = "Empty";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCubeEntity(entt::entity entity)
{
    std::string name = "Cube";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);;
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    ModelStorageBuffer* modelStorageBuffer = nullptr;
    meshStorage->LoadModel(CUBE_MODEL, &modelStorageBuffer);
    meshComponent.ApplyModel(modelStorageBuffer);

    MeshStorageBuffer* meshStorageBuffer = nullptr;
    modelStorageBuffer->LoadMesh(0, &meshStorageBuffer);
    meshComponent.ApplyMesh(meshStorageBuffer);

    modelStorageBuffer->CloseModel();
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateSphereEntity(entt::entity entity)
{
    std::string name = "Sphere";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    ModelStorageBuffer* modelStorageBuffer = nullptr;
    meshStorage->LoadModel(SPHERE_MODEL, &modelStorageBuffer);
    meshComponent.ApplyModel(modelStorageBuffer);

    MeshStorageBuffer* meshStorageBuffer = nullptr;
    modelStorageBuffer->LoadMesh(0, &meshStorageBuffer);
    meshComponent.ApplyMesh(meshStorageBuffer);

    modelStorageBuffer->CloseModel();
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCapsuleEntity(entt::entity entity)
{
    std::string name = "Capsule";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    ModelStorageBuffer* modelStorageBuffer = nullptr;
    meshStorage->LoadModel(CAPSULE_MODEL, &modelStorageBuffer);
    meshComponent.ApplyModel(modelStorageBuffer);

    MeshStorageBuffer* meshStorageBuffer = nullptr;
    modelStorageBuffer->LoadMesh(0, &meshStorageBuffer);
    meshComponent.ApplyMesh(meshStorageBuffer);

    modelStorageBuffer->CloseModel();
    DirectX::BoundingBox boundingBox = DirectX::BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(2.0f, 1.0f, 1.0f));
    GetComponent<TransformComponent>(entity).SetBoundingBox(boundingBox);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreatePlaneEntity(entt::entity entity)
{
    std::string name = "Plane";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    ModelStorageBuffer* modelStorageBuffer = nullptr;
    meshStorage->LoadModel(PLANE_MODEL, &modelStorageBuffer);
    meshComponent.ApplyModel(modelStorageBuffer);

    MeshStorageBuffer* meshStorageBuffer = nullptr;
    modelStorageBuffer->LoadMesh(0, &meshStorageBuffer);
    meshComponent.ApplyMesh(meshStorageBuffer);

    modelStorageBuffer->CloseModel();
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCameraEntity(entt::entity entity)
{
    std::string name = "Camera";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<CameraComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateTextMeshEntity(entt::entity entity)
{
    std::string name = "TextMesh";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<TextMeshComponent>(entity);
    GetComponent<TextMeshComponent>(entity).SetText("hello");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}

void Entity::CreateRectangleEntity(entt::entity entity)
{
    std::string name = "Rectangle";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<RectangleComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}

void Entity::CreateImageEntity(entt::entity entity)
{
    std::string name = "Image";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<ImageComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}

void Entity::CreateTextEntity(entt::entity entity)
{
    std::string name = "Text";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    AddComponent<TextComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}

// last code edit
// check if this working
// 18.2

void Entity::CreateButtonEntity(entt::entity entity)
{
    std::string name = "Button";
    unsigned int index = GetFreeNameIndex(name.c_str());
    if (index != 0)
        name = name + std::to_string(index);

    GetComponent<GeneralComponent>(entity).SetName(name);
    GetComponent<GeneralComponent>(root).AddChild(entity);

    auto textEntity = CreateEntity();
    GetComponent<GeneralComponent>(textEntity).SetName("Text");
    AddComponent<TextComponent>(textEntity);
    GetComponent<GeneralComponent>(entity).AddChild(textEntity);

    auto rectangleEntity = CreateEntity();
    GetComponent<GeneralComponent>(rectangleEntity).SetName("Rectangle");
    AddComponent<RectangleComponent>(rectangleEntity);
    GetComponent<GeneralComponent>(entity).AddChild(rectangleEntity);
}

entt::entity Entity::CreateEntity()
{
    entt::entity entity = entt::null;
    entity = registry.create();
    AddComponent<GeneralComponent>(entity);
    AddComponent<TransformComponent>(entity);
    AddComponent<ScriptingComponent>(entity);
    AddComponent<PhysicsComponent>(entity);
    return entity;
}
void Entity::SetSelectedEntity(entt::entity entity)
{
    selected = entity;
}
entt::entity Entity::GetSelectedEntity()
{
    return selected;
}
void Entity::SetRootEntity(entt::entity entity)
{
    root = entity;
}
entt::entity Entity::GetRootEntity()
{
    return root;
}
entt::registry& Entity::GetRegistry()
{
    return registry;
}
unsigned int Entity::GetFreeNameIndex(const char* name)
{
    unsigned int index = 2;
    bool x = true;
    auto view = registry.view<GeneralComponent>();
    for (auto entity : view)
    {
        if (x)
        {
            // first check for only name without index
            entt::entity entity = FindByName(name, false);
            if (entity == entt::null)
                return 0;

            // entity is not null, no find good name
            x = false;
        }

        // start check by index
        std::string string = name + std::to_string(index);
        entt::entity entity = FindByName(string.c_str(), false);
        if (entity == entt::null)
            return index;
        index++;
    }

    return 0;
}
entt::entity Entity::FindByName(const char* name, bool print)
{
    auto view = registry.view<GeneralComponent>();
    for (auto entity : view)
    {
        if (entity == root) continue; // skip root entity
        auto& gc = GetComponent<GeneralComponent>(entity);

        if (!gc.GetName().compare(name))
            return entity;
    }

    if (print)
        consoleWindow->AddWarningMessage("Failed to find entity by name %s", name);
    return entt::null;
}
entt::entity Entity::FindByTag(const char* tag, bool print)
{
    auto view = registry.view<GeneralComponent>();
    for (auto entity : view)
    {
        if (entity == root) continue; // skip root entity
        auto& gc = GetComponent<GeneralComponent>(entity);

        if (!gc.GetTag().compare(tag))
            return entity;
    }

    if (print)
        consoleWindow->AddWarningMessage("Failed to find entity by tag %s", tag);
    return entt::null;
}

void Entity::Destroy(entt::entity entity)
{
    if (!IsValid(entity))
        return;

    std::vector<entt::entity> bin;
    DestroyAll(entity, bin);

    if (!bin.empty())
    {
        for (size_t i = 0; i < bin.size(); i++)
        {
            auto& generalComponent = GetComponent<GeneralComponent>(bin[i]);
            printf("Destroying.. %s\n", generalComponent.GetName().c_str());
            Cleanup(bin[i]);
            registry.destroy(bin[i]);
        }

        bin.clear();
    }
}
void Entity::DestroyChildren(entt::entity entity)
{
    if (!IsValid(entity))
        return;

    std::vector<entt::entity> bin;
    DestroyAll(entity, bin);

    if (!bin.empty())
    {
        for (size_t i = 0; i < bin.size(); i++)
        {
            // skip main entry
            if (i == 0)
            {
                // fix arrow
                GetComponent<GeneralComponent>(bin[0]).GetChildren().clear();
                continue;
            }
            auto& generalComponent = GetComponent<GeneralComponent>(bin[i]);
            printf("[Entity] -> Destroying.. %s\n", generalComponent.GetName().c_str());
            Cleanup(bin[i]);
            registry.destroy(bin[i]);
        }

        bin.clear();
    }
}
void Entity::DestroyAll(entt::entity entity, std::vector<entt::entity>& bin)
{
    if (!IsValid(entity))
        return;

    bin.push_back(entity);

    auto& generalComponent = registry.get<GeneralComponent>(entity);
    for (size_t i = 0; i < generalComponent.GetChildren().size(); i++)
        DestroyAll(generalComponent.GetChildren()[i], bin);
}
void Entity::Cleanup(entt::entity entity)
{
    if (!IsValid(entity))
        return;

    if (HasComponent<RigidDynamicComponent>(entity))
    {
        auto& rigidBodyComponent = GetComponent<RigidDynamicComponent>(entity);
        rigidBodyComponent.ReleaseActor();
    }
    if (HasComponent<PhysicsComponent>(entity))
    {
        auto& physicsComponent = GetComponent<PhysicsComponent>(entity);
        physicsComponent.ReleaseAllBoxColliders();
        physicsComponent.ReleaseAllSphereColliders();
    }
}
entt::entity Entity::CopyEntity(entt::entity _Entity)
{
    // DEMO

    entt::entity entity = CreateEntity();
    {
        if (HasComponent<GeneralComponent>(_Entity))
        {
            CopyComponent<GeneralComponent>(_Entity, entity);
            GetComponent<GeneralComponent>(entity).ClearVector();
        }

        if (HasComponent<TransformComponent>(_Entity))
        {
            CopyComponent<TransformComponent>(_Entity, entity);
        }

        if (HasComponent<ScriptingComponent>(_Entity))
        {
            CopyComponent<ScriptingComponent>(_Entity, entity);
        }

        if (HasComponent<PhysicsComponent>(_Entity))
        {
            CopyComponent<PhysicsComponent>(_Entity, entity);
            GetComponent<PhysicsComponent>(entity).ClearVector();
            for (size_t i = 0; i < GetComponent<PhysicsComponent>(_Entity).GetBoxColliders()->size(); i++)
            {
                BoxColliderComponent* _BoxColliderComponent = &GetComponent<PhysicsComponent>(_Entity).GetBoxColliders()->at(i);

                GetComponent<PhysicsComponent>(entity).AddBoxCollider();
                BoxColliderComponent* boxColliderComponent = &GetComponent<PhysicsComponent>(entity).GetBoxColliders()->back();
                boxColliderComponent->SetCenter(_BoxColliderComponent->GetCenter());
                boxColliderComponent->SetExtents(_BoxColliderComponent->GetExtents());
                boxColliderComponent->SetSize(_BoxColliderComponent->GetSize());
            }
            for (size_t i = 0; i < GetComponent<PhysicsComponent>(_Entity).GetSphereColliders()->size(); i++)
            {
                SphereColliderComponent* _SphereColliderComponent = &GetComponent<PhysicsComponent>(_Entity).GetSphereColliders()->at(i);

                GetComponent<PhysicsComponent>(entity).AddSphereCollider();
                SphereColliderComponent* sphereColliderComponent = &GetComponent<PhysicsComponent>(entity).GetSphereColliders()->back();
                sphereColliderComponent->SetCenter(_SphereColliderComponent->GetCenter());
                sphereColliderComponent->SetExtent(_SphereColliderComponent->GetExtent());
                sphereColliderComponent->SetRadius(_SphereColliderComponent->GetRadius());
            }
        }

        if (HasComponent<RigidDynamicComponent>(_Entity))
        {
            CopyComponent<RigidDynamicComponent>(_Entity, entity);
            GetComponent<RigidDynamicComponent>(entity).SetNullActor(); // set null, but do not release actor
            GetComponent<RigidDynamicComponent>(entity).CreateActor(); // and now create new actor
            auto& _RigidbodyComponent = GetComponent<RigidDynamicComponent>(_Entity);
            auto& rigidbodyComponent = GetComponent<RigidDynamicComponent>(entity);
            rigidbodyComponent.SetMass(_RigidbodyComponent.GetMass());
            rigidbodyComponent.SetLinearDamping(_RigidbodyComponent.GetLinearDamping());
            rigidbodyComponent.SetAngularDamping(_RigidbodyComponent.GetAngularDamping());
            rigidbodyComponent.UseGravity(_RigidbodyComponent.HasUseGravity());
            rigidbodyComponent.SetKinematic(_RigidbodyComponent.GetKinematic());
        }

        if (HasComponent<MeshComponent>(_Entity))
        {
            CopyComponent<MeshComponent>(_Entity, entity);
        }

        if (HasComponent<CameraComponent>(_Entity))
        {
            CopyComponent<CameraComponent>(_Entity, entity);
        }

        for (size_t i = 0; i < GetComponent<GeneralComponent>(_Entity).GetChildren().size(); i++)
        {
            entt::entity x = CopyEntity(GetComponent<GeneralComponent>(_Entity).GetChildren()[i]);
            GetComponent<GeneralComponent>(entity).AddChild(x);
        }
    }
    return entity;
}