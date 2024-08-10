#include "ProjectSceneSystem.h"
#include "../ENTITY/Entity.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../EDITOR/WINDOW/Viewport.h"
#include <fstream>
#include "../MAIN/Main.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/RigidBodyComponent.h"
#include "PhysicsSystem.h"
#include "ScriptingSystem.h"

ProjectSceneSystem* ProjectSceneSystem::GetSingleton()
{
	static ProjectSceneSystem projectSceneSystem;
	return &projectSceneSystem;
}

static Entity* ecs = Entity::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();
static MeshStorage* meshStorage = MeshStorage::GetSingleton();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();

void ProjectSceneSystem::ClearScene()
{
	ecs->selected = entt::null;
	ecs->DestroyChildren(ecs->root);

	physicsSystem->ClearScene();
}
void ProjectSceneSystem::NewScene()
{
	ClearScene();
	viewportWindow->SetDefaultCam();

	entt::entity cube = ecs->CreateEntity();
	ecs->CreateCubeEntity(cube);
	ecs->selected = cube;

	entt::entity camera = ecs->CreateEntity();
	ecs->CreateCameraEntity(camera);
	ecs->GetComponent<TransformComponent>(camera).SetPosition(Vector3(0.0f, 0.0f, -5.0f));
}

void ProjectSceneSystem::SaveScene()
{
	Star::AddLog("[Scene] -> Saving Scene...");

	YAML::Emitter out;

	Star::BeginFormat(out);
	{
		SerializeHierarchy(out);
	}
	Star::EndFormat(out);

	std::string path = "data.scene";
	printf("[Scene] -> Saving to %s\n", path.c_str());
	SaveFile(out, path.c_str());
}

void ProjectSceneSystem::OpenScene(const char* path)
{
	Star::AddLog("[Scene] -> Opening Scene...");

	//Star::OpenFileDialog(L"C:\\", L"*.scene\0", L"Open");

	ClearScene();

	YAML::Node in = YAML::LoadFile(path);
	if (!Star::CheckSignature(in))
		return;

	DeserializeHierarchy(in);
}

void ProjectSceneSystem::SerializeEntity(YAML::Emitter& out, entt::entity entity)
{
	auto& parent = ecs->GetComponent<GeneralComponent>(entity);

	out << YAML::BeginMap;
	{
		out << YAML::Key << "Entity" << YAML::Value << YAML::BeginMap;
		{
			if (ecs->HasComponent<GeneralComponent>(entity))
				ecs->GetComponent<GeneralComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<TransformComponent>(entity))
				ecs->GetComponent<TransformComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<MeshComponent>(entity))
				ecs->GetComponent<MeshComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<CameraComponent>(entity))
				ecs->GetComponent<CameraComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<RigidbodyComponent>(entity))
				ecs->GetComponent<RigidbodyComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<PhysicsComponent>(entity))
				ecs->GetComponent<PhysicsComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<ScriptingComponent>(entity))
				ecs->GetComponent<ScriptingComponent>(entity).SerializeComponent(out);

			std::vector<entt::entity> children = parent.GetChildren();
			if (!children.empty())
			{
				out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
				for (size_t i = 0; i < children.size(); i++)
				{
					if (!ecs->IsValid(children[i]))
						continue;
					SerializeEntity(out, children[i]);
				}
				out << YAML::EndSeq;
			}
		}
		out << YAML::EndMap;
	}
	out << YAML::EndMap;
}

void ProjectSceneSystem::SerializeHierarchy(YAML::Emitter& out)
{
	out << YAML::Key << "Hierarchy" << YAML::Value << YAML::BeginSeq;
	{
		SerializeEntity(out, ecs->root);
	}
	out << YAML::EndSeq;
}

void ProjectSceneSystem::SaveFile(YAML::Emitter& out, const char* filename)
{
	Star::AddLog("[Scene] -> Saving File...");
	if (!out.good())
		Star::AddLog("[YAML] -> %s", out.GetLastError().c_str());
	std::ofstream stream(filename);
	stream << out.c_str();
	stream.close();
}

void ProjectSceneSystem::SaveAsScene()
{
	const char* path = Star::SaveFileDialog(L"C:\\", L"*.scene\0", L"Save As");

	if (strlen(path) == 0)
		return;
}

void ProjectSceneSystem::DeserializeHierarchy(YAML::Node& in)
{
	YAML::Node hierarchy = in["Star"]["Data"]["Hierarchy"];
	for (size_t i = 0; i < hierarchy.size(); ++i)
	{
		YAML::Node entityNode = hierarchy[i]["Entity"];
		if (entityNode)
		{
			DeserializeEntity(entityNode, ecs->root);
		}
	}
	meshStorage->CloseAllOpenedModels();
}
void ProjectSceneSystem::DeserializeEntity(YAML::Node& in, entt::entity parent)
{
	if (in["Children"])
	{
		for (size_t i = 0; i < in["Children"].size(); ++i)
		{
			YAML::Node childNode = in["Children"][i]["Entity"];
			if (childNode)
			{
				entt::entity entity = ecs->CreateEntity();

				{
					if (!ecs->HasComponent<GeneralComponent>(entity))
						ecs->AddComponent<GeneralComponent>(entity);
					ecs->GetComponent<GeneralComponent>(entity).DeserializeComponent(childNode);
				}

				{
					if (!ecs->HasComponent<TransformComponent>(entity))
						ecs->AddComponent<TransformComponent>(entity);
					ecs->GetComponent<TransformComponent>(entity).DeserializeComponent(childNode);
				}

				{
					if (childNode["MeshComponent"])
					{
						ecs->AddComponent<MeshComponent>(entity);
						ecs->GetComponent<MeshComponent>(entity).DeserializeComponent(childNode);
					}
				}
				
				if (childNode["CameraComponent"])
				{
					ecs->AddComponent<CameraComponent>(entity);
					ecs->GetComponent<CameraComponent>(entity).DeserializeComponent(childNode);
				}
				
				if (childNode["RigidbodyComponent"])
				{
					ecs->AddComponent<RigidbodyComponent>(entity);
					ecs->GetComponent<RigidbodyComponent>(entity).CreateActor(); // !
					ecs->GetComponent<RigidbodyComponent>(entity).DeserializeComponent(childNode);
				}
				
				if (childNode["Colliders"])
				{
					ecs->GetComponent<PhysicsComponent>(entity).DeserializeComponent(childNode);
				}

				if (childNode["Scripts"])
				{
					ecs->GetComponent<ScriptingComponent>(entity).DeserializeComponent(childNode);
				}

				if (ecs->HasComponent<GeneralComponent>(entity))
					ecs->GetComponent<GeneralComponent>(parent).AddChild(entity);

				DeserializeEntity(childNode, entity);
			}
		}
	}
}