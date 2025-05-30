#pragma once

#include <entt/entt.hpp>
#include <physx/PxPhysicsAPI.h>
#include <SimpleMath.h>
#include <yaml-cpp/yaml.h>
#include "../ENTITY/COMPONENT/BoxColliderComponent.h"
#include "../ENTITY/COMPONENT/SphereColliderComponent.h"
#include "../ENTITY/COMPONENT/CapsuleColliderComponent.h"

#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

using namespace DirectX::SimpleMath;

/* GRAVITY TYPES */
#define EARTH_GRAVITY physx::PxVec3(0.0f, -9.81f, 0.0f)
#define MOON_GRAVITY physx::PxVec3(0.0f, -1.62f, 0.0f)
#define NONE_GRAVITY physx::PxVec3(0.0f, 0.0f, 0.0f)

/* DEFAULT MATERIAL */
#define STATIC_FRICTION 0.5f
#define DYNAMIC_FRICTION 0.5f
#define RESTITUTION 0.1f

enum PhysicsProcesor
{
	xCPU = 0,
	xGPU = 1
};

class PhysicsSystem
{
public:
	bool Init();
	void Update(float deltaTime);
	void CreateScene();
	void ClearScene();
	void Shutdown();
	void SetProcesor(PhysicsProcesor _Procesor);
	PhysicsProcesor GetProcesor();

private:
	physx::PxDefaultAllocator gAllocator;
	physx::PxDefaultErrorCallback gErrorCallback;
	physx::PxFoundation* gFoundation = nullptr;
	physx::PxPhysics* gPhysics = nullptr;
	physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;
	physx::PxScene* gScene = nullptr;
	physx::PxPvd* gPvd = nullptr;
	physx::PxPvdTransport* transport = nullptr;
	physx::PxCudaContextManager* gCudaContextManager = nullptr;
	physx::PxCooking* gCooking = nullptr;

	physx::PxSceneDesc* sceneDesc;

private:
	PhysicsProcesor physicsProcesor = PhysicsProcesor::xGPU;

public:
	physx::PxPhysics* GetPhysics();
	physx::PxScene* GetScene();
	physx::PxCooking* GetCooking();

public:
	static PhysicsSystem* GetSingleton();
};

struct PhysicsComponent
{
public:
	void Render();

	void AddBoxCollider();
	void AddSphereCollider();
	void AddCapsuleCollider();

	void ReleaseAllBoxColliders();
	void ReleaseAllSphereColliders();
	void ReleaseAllCapsuleColliders();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);

	void ClearVector();

public:
	std::vector<BoxColliderComponent>* GetBoxColliders();
	std::vector<SphereColliderComponent>* GetSphereColliders();
	std::vector<CapsuleColliderComponent>* GetCapsuleColliders();

private:
	std::vector<BoxColliderComponent> box_colliders;
	std::vector<SphereColliderComponent> sphere_colliders;
	std::vector<CapsuleColliderComponent> capsule_colliders;
};