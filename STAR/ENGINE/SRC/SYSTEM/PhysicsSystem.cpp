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
	BoxColliderComponent boxColliderBuffer;
	{
		//boxColliderBuffer.CreateMaterial();
		entt::entity entity = entt::to_entity(ecs->registry, *this);
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			boxColliderBuffer.CreateShape(transformComponent.GetScale());
		}

		if (ecs->registry.any_of<RigidbodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidbodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*boxColliderBuffer.GetShape());
			//printf("Attaching shape\n");
		}
	}
	box_colliders.push_back(boxColliderBuffer);
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
	if (box_colliders.empty() && sphere_colliders.empty())
		return;

	out << YAML::Key << "Colliders" << YAML::Value << YAML::BeginSeq;
	for (size_t i = 0; i < box_colliders.size(); i++)
	{
		box_colliders[i].SerializeComponent(out);
	}
	for (size_t i = 0; i < sphere_colliders.size(); i++)
	{
		sphere_colliders[i].SerializeComponent(out);
	}
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
	SphereColliderComponent sphereColliderBuffer;
	{
		//boxColliderBuffer.CreateMaterial();
		entt::entity entity = entt::to_entity(ecs->registry, *this);
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			float extent = std::max(std::max(transformComponent.GetScale().x, transformComponent.GetScale().y), transformComponent.GetScale().z);
			sphereColliderBuffer.CreateShape(extent); // fixed! static? what about this? (1.0f, 1.0f, 1.0f) / 3
		}

		if (ecs->registry.any_of<RigidbodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidbodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*sphereColliderBuffer.GetShape());
			//printf("Attaching shape\n");
		}
	}
	sphere_colliders.push_back(sphereColliderBuffer);
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