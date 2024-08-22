#include "PhysicsSystem.h"
#include "../IMGUI/imgui.h"
#include "../ENTITY/Entity.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../STAR/Star.h"
#include "../EDITOR/WINDOW/Console.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/BoxColliderComponent.h"
#include "../GAME/Game.h"

static Entity* ecs = Entity::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static DX* dx = DX::GetSingleton();
static Game* game = Game::GetSingleton();

bool PhysicsSystem::Init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);

	// for debug
#if defined(_DEBUG)
	transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, 10);
	if (!gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL))
		Star::AddLog("[PhysX] -> PhysX Visual Debugger is not connected!\n[PhysX] -> Host %s Port %i", PVD_HOST, PVD_PORT);
#endif

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

	sceneDesc = new physx::PxSceneDesc(gPhysics->getTolerancesScale());
	sceneDesc->gravity = EARTH_GRAVITY;

	if (physicsProcesor == PhysicsProcesor::xCPU)
	{
		gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	}
	else if (physicsProcesor == PhysicsProcesor::xGPU)
	{
		physx::PxCudaContextManagerDesc cudaContextManagerDesc;
		cudaContextManagerDesc.interopMode = physx::PxCudaInteropMode::D3D11_INTEROP;
		cudaContextManagerDesc.graphicsDevice = dx->dxDevice;
		gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc);
		if (gCudaContextManager) if (!gCudaContextManager->contextIsValid()) Star::AddLog("[PhysX] -> Cuda Context Manager Error!");

		gDispatcher = physx::PxDefaultCpuDispatcherCreate(4);
		sceneDesc->gpuDispatcher = gCudaContextManager->getGpuDispatcher();

		sceneDesc->flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
		sceneDesc->flags |= physx::PxSceneFlag::eENABLE_PCM;
		sceneDesc->flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
		sceneDesc->broadPhaseType = physx::PxBroadPhaseType::eGPU;
		sceneDesc->gpuMaxNumPartitions = 8;
	}
	else
	{
		Star::AddLog("[PhysX] -> Processor Error!");
	}

	sceneDesc->cpuDispatcher = gDispatcher;
	sceneDesc->filterShader = physx::PxDefaultSimulationFilterShader;

	CreateScene();
	return true;
}
void PhysicsSystem::Update(float deltaTime)
{
	//gScene->simulate(1.0f / 60.0f);
	//gScene->simulate(ImGui::GetIO().DeltaTime);
	gScene->simulate(deltaTime);
	gScene->fetchResults(true);
}
void PhysicsSystem::Shutdown()
{
	if (gScene)              gScene->release();
	if (gDispatcher)         gDispatcher->release();
	if (gPhysics)            gPhysics->release();
	if (gPvd)                gPvd->release();
	if (transport)           transport->release();
	if (gCudaContextManager) gCudaContextManager->release();
	if (gFoundation)         gFoundation->release();
	if (sceneDesc)
	{
		delete sceneDesc;
		sceneDesc = nullptr;
	}
}
physx::PxPhysics* PhysicsSystem::GetPhysics()
{
	return gPhysics;
}
physx::PxScene* PhysicsSystem::GetScene()
{
	return gScene;
}

void PhysicsComponent::AddBoxCollider()
{
	box_colliders.emplace_back();
	BoxColliderComponent* boxColliderComponent = &box_colliders.back();
	{
		entt::entity entity = entt::to_entity(ecs->registry, *this);

		// create shape
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			boxColliderComponent->CreateShape(transformComponent.GetScale());
		}

		// attach shape
		if (ecs->registry.any_of<RigidbodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidbodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*boxColliderComponent->GetShape());
		}
	}
}

void PhysicsComponent::Render()
{
	for (size_t i = 0; i < box_colliders.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			box_colliders[i].Render(GetBoxColliders(), i);
		}
		ImGui::PopID();
	}
	for (size_t i = 0; i < sphere_colliders.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			sphere_colliders[i].Render(GetSphereColliders(), i);
		}
		ImGui::PopID();
	}
	for (size_t i = 0; i < capsule_colliders.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			capsule_colliders[i].Render(GetCapsuleColliders(), i);
		}
		ImGui::PopID();
	}
}

std::vector<BoxColliderComponent>* PhysicsComponent::GetBoxColliders()
{
	return &box_colliders;
}
std::vector<SphereColliderComponent>* PhysicsComponent::GetSphereColliders()
{
	return &sphere_colliders;
}
void PhysicsComponent::SerializeComponent(YAML::Emitter& out)
{
	unsigned int x = 0;

	{
		// any type of colliders?
		if (!box_colliders.empty())
			x++;
		if (!sphere_colliders.empty())
			x++;
		if (!capsule_colliders.empty())
			x++;
	}

	if (x != 0)
		out << YAML::Key << "Colliders" << YAML::Value << YAML::BeginSeq;

	{
		for (size_t i = 0; i < box_colliders.size(); i++)
			box_colliders[i].SerializeComponent(out);
		for (size_t i = 0; i < sphere_colliders.size(); i++)
			sphere_colliders[i].SerializeComponent(out);
		for (size_t i = 0; i < capsule_colliders.size(); i++)
			capsule_colliders[i].SerializeComponent(out);
	}

	if (x != 0)
		out << YAML::EndSeq;
}
void PhysicsComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node colliders = in["Colliders"];
	if (colliders)
	{
		for (const auto& collider : colliders)
		{
			YAML::Node boxCollider = collider["BoxCollider"];
			if (boxCollider)
			{
				AddBoxCollider();
				box_colliders.back().DeserializeComponent(boxCollider);
			}
			YAML::Node sphereCollider = collider["SphereCollider"];
			if (sphereCollider)
			{
				AddSphereCollider();
				sphere_colliders.back().DeserializeComponent(sphereCollider);
			}
			YAML::Node capsuleCollider = collider["CapsuleCollider"];
			if (capsuleCollider)
			{
				AddCapsuleCollider();
				capsule_colliders.back().DeserializeComponent(sphereCollider);
			}
		}
	}
}

void PhysicsSystem::SetProcesor(PhysicsProcesor _Procesor)
{
	physicsProcesor = _Procesor;
}
PhysicsProcesor PhysicsSystem::GetProcesor()
{
	return physicsProcesor;
}
void PhysicsSystem::ClearScene()
{
	if (gScene)
		gScene->release();

	CreateScene();
}
void PhysicsSystem::CreateScene()
{
	gScene = gPhysics->createScene(*sceneDesc);
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}
PhysicsSystem* PhysicsSystem::GetSingleton()
{
	static PhysicsSystem physicsSystem;
	return &physicsSystem;
}
void PhysicsComponent::AddSphereCollider()
{
	sphere_colliders.emplace_back();
	SphereColliderComponent* sphereColliderComponent = &sphere_colliders.back();
	{
		entt::entity entity = entt::to_entity(ecs->registry, *this);

		// create shape
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			float extent = std::max(std::max(transformComponent.GetScale().x, transformComponent.GetScale().y), transformComponent.GetScale().z);
			sphereColliderComponent->CreateShape(extent); // fixed! static? what about this? (1.0f, 1.0f, 1.0f) / 3
		}

		// attach shape
		if (ecs->registry.any_of<RigidbodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidbodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*sphereColliderComponent->GetShape());
		}
	}
}
void PhysicsComponent::ReleaseAllBoxColliders()
{
	for (size_t i = 0; i < box_colliders.size(); i++)
		box_colliders[i].Release();
}
void PhysicsComponent::ReleaseAllSphereColliders()
{
	for (size_t i = 0; i < sphere_colliders.size(); i++)
		sphere_colliders[i].Release();
}
void PhysicsComponent::ClearVector()
{
	if (!box_colliders.empty())
		box_colliders.clear();
	if (!sphere_colliders.empty())
		sphere_colliders.clear();
	if (!capsule_colliders.empty())
		capsule_colliders.clear();
}
std::vector<CapsuleColliderComponent>* PhysicsComponent::GetCapsuleColliders()
{
	return &capsule_colliders;
}
void PhysicsComponent::ReleaseAllCapsuleColliders()
{
	for (size_t i = 0; i < capsule_colliders.size(); i++)
		capsule_colliders[i].Release();
}
void PhysicsComponent::AddCapsuleCollider()
{
	capsule_colliders.emplace_back();
	CapsuleColliderComponent* capsuleColliderComponent = &capsule_colliders.back();
	{
		entt::entity entity = entt::to_entity(ecs->registry, *this);

		// create shape
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			capsuleColliderComponent->CreateShape(1.0f, 1.0f);
		}

		// attach shape
		if (ecs->registry.any_of<RigidbodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidbodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*capsuleColliderComponent->GetShape());
		}
	}
}