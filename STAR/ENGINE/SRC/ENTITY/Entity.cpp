#include "Entity.h"
#include <vector>
#include "../HELPERS/Helpers.h"
#include "COMPONENT/MeshComponent.h"
#include "../XTK/WICTextureLoader11.h"
#include "COMPONENT/GeneralComponent.h"
#include "COMPONENT/TransformComponent.h"
#include "COMPONENT/RigidbodyComponent.h"
#include "COMPONENT/CameraComponent.h"
#include "COMPONENT/TextMeshComponent.h"
#include "../SYSTEM/ProjectSceneSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../SYSTEM/PhysicsSystem.h"
#include "../MODEL/AssimpLoader.h"
#include "../EDITOR/WINDOW/Console.h"

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

    GetComponent<GeneralComponent>(entity).SetName(name);
	AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);
    Assimp::Importer importer;
    const aiScene* scene = StarHelpers::OpenModel(&importer, CUBE_MODEL);
    if (scene)
    {
        meshComponent.SetModelPath(CUBE_MODEL);
        meshComponent.SetMeshIndex(0);
        meshComponent.LoadMesh(scene);
        meshComponent.SetupMesh();
    }

    meshComponent.SetFileName(CUBE_MODEL);
    meshComponent.SetMeshName(name);
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
    Assimp::Importer importer;
    const aiScene* scene = StarHelpers::OpenModel(&importer, SPHERE_MODEL);
    if (scene)
    {
        meshComponent.SetModelPath(SPHERE_MODEL);
        meshComponent.SetMeshIndex(0);
        meshComponent.LoadMesh(scene);
        meshComponent.SetupMesh();
    }

    meshComponent.SetFileName(SPHERE_MODEL);
    meshComponent.SetMeshName(name);
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
    Assimp::Importer importer;
    const aiScene* scene = StarHelpers::OpenModel(&importer, CAPSULE_MODEL);
    if (scene)
    {
        meshComponent.SetModelPath(CAPSULE_MODEL);
        meshComponent.SetMeshIndex(0);
        meshComponent.LoadMesh(scene);
        meshComponent.SetupMesh();
    }

    meshComponent.SetFileName(CAPSULE_MODEL);
    meshComponent.SetMeshName(name);
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
    Assimp::Importer importer;
    const aiScene* scene = StarHelpers::OpenModel(&importer, PLANE_MODEL);
    if (scene)
    {
        meshComponent.SetModelPath(PLANE_MODEL);
        meshComponent.SetMeshIndex(0);
        meshComponent.LoadMesh(scene);
        meshComponent.SetupMesh();
    }

    meshComponent.SetFileName(PLANE_MODEL);
    meshComponent.SetMeshName(name);
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