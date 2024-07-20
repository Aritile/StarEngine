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

bool Entity::Init()
{
    root = CreateEntity();
	GetComponent<GeneralComponent>(root).SetName("Scene");
    projectSceneSystem->NewScene();
    return true;
}

void Entity::CreateEmptyEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Empty");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCubeEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Cube");
	AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);
    Assimp::Importer importer;
    const aiScene* scene = StarHelpers::OpenModel(&importer, CUBE_MODEL);
    if (scene)
    {
        // bugtest
        //printf("bugtest: %s\n", CUBE_MODEL);
        meshComponent.SetModelPath(CUBE_MODEL);
        meshComponent.SetMeshIndex(0);
        meshComponent.LoadMesh(scene);
        meshComponent.SetupMesh();
    }

    meshComponent.SetFileName(CUBE_MODEL);
    meshComponent.SetMeshName("Cube");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateSphereEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Sphere");
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
    meshComponent.SetMeshName("Sphere");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCapsuleEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Capsule");
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
    meshComponent.SetMeshName("Capsule");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreatePlaneEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Plane");
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
    meshComponent.SetMeshName("Plane");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCameraEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Camera");
    AddComponent<CameraComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateTextMeshEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("TextMesh");
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