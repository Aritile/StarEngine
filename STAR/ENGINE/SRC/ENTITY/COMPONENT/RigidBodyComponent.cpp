#include "RigidBodyComponent.h"
#include "../../IMGUI/imgui.h"
#include <entt/entt.hpp>
#include "../../ENTITY/Entity.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../SYSTEM/PhysicsSystem.h"

static Entity* ecs = Entity::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static PhysicsSystem* physicsSystem = &PhysicsSystemClass();

void RigidBodyComponent::Render()
{
	if (ImGui::CollapsingHeader("RIGIDBODY", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("RigidBodyComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Mass");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Linear Damping");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Angular Damping");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Use Gravity");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Is Kinematic");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Freeze");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Position");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Rotation");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				float mass = GetMass();
				if (ImGui::DragFloat("##MassRigidBodyComponent", &mass, 0.1f, 0.0f, FLT_MAX))
					SetMass(mass);

				float linearDamping = GetLinearDamping();
				if (ImGui::DragFloat("##LinearDampingRigidBodyComponent", &linearDamping, 0.1f, 0.0f, FLT_MAX))
					SetLinearDamping(linearDamping);

				float angularDamping = GetAngularDamping();
				if (ImGui::DragFloat("##AngularDampingRigidBodyComponent", &angularDamping, 0.1f, 0.0f, FLT_MAX))
					SetAngularDamping(angularDamping);

				bool useGravity = HasUseGravity();
				if (ImGui::Checkbox("##UseGravityRigidBodyComponent", &useGravity))
					UseGravity(useGravity);

				bool getKinematic = GetKinematic();
				if (ImGui::Checkbox("##IsKinematicRigidBodyComponent", &getKinematic))
					SetKinematic(getKinematic);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				bool px = GetLinearLockX();
				if (ImGui::Checkbox("X##PositionRigidBodyComponent", &px))
					SetLinearLockX(px);
				ImGui::SameLine();
				bool py = GetLinearLockY();
				if (ImGui::Checkbox("Y##PositionRigidBodyComponent", &py))
					SetLinearLockY(py);
				ImGui::SameLine();
				bool pz = GetLinearLockZ();
				if (ImGui::Checkbox("Z##PositionRigidBodyComponent", &pz))
					SetLinearLockZ(pz);

				bool rx = GetAngularLockX();
				if (ImGui::Checkbox("X##RotationRigidBodyComponent", &rx))
					SetAngularLockX(rx);
				ImGui::SameLine();
				bool ry = GetAngularLockY();
				if (ImGui::Checkbox("Y##RotationRigidBodyComponent", &ry))
					SetAngularLockY(ry);
				ImGui::SameLine();
				bool rz = GetAngularLockZ();
				if (ImGui::Checkbox("Z##RotationRigidBodyComponent", &rz))
					SetAngularLockZ(rz);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void RigidBodyComponent::UpdateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	if (pxRigidBody)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			physx::PxTransform pxTransform = pxRigidBody->getGlobalPose();
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			transformComponent.SetPosition(StarHelpers::PhysicsToVector3(pxTransform.p));
			transformComponent.SetRotationQuaternion(StarHelpers::PhysicsToQuat(pxTransform.q));
		}
	}
}

void RigidBodyComponent::CreateActor()
{
	if (pxRigidBody) pxRigidBody->release();
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		physx::PxTransform pxTransform = physx::PxTransform(
			StarHelpers::Vector3ToPhysics(transformComponent.GetPosition()),
			StarHelpers::QuatToPhysics(transformComponent.GetRotationQuaternion()));
		pxRigidBody = physicsSystem->GetPhysics()->createRigidDynamic(pxTransform);
		if (!pxRigidBody) consoleWindow->AddWarningMessage("[RigidBody] -> Failed to create the RigidBody!");
		physicsSystem->GetScene()->addActor(*pxRigidBody); /*<*>*/

		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			for (size_t i = 0; i < physicsComponent.GetBoxColliders().size(); i++)
				if (physicsComponent.GetBoxColliders()[i].GetShape())
					pxRigidBody->attachShape(*physicsComponent.GetBoxColliders()[i].GetShape());
				else
					consoleWindow->AddWarningMessage("[RigidBody] -> Failed to attach shape to the RigidBody!");
		}
	}
}

void RigidBodyComponent::SetMass(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setMass(value);
}
float RigidBodyComponent::GetMass()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getMass();
}
void RigidBodyComponent::SetLinearVelocity(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setLinearVelocity(StarHelpers::Vector3ToPhysics(value));
}
Vector3 RigidBodyComponent::GetLinearVelocity()
{
	if (!pxRigidBody) return Vector3();

	return StarHelpers::PhysicsToVector3(pxRigidBody->getLinearVelocity());
}
void RigidBodyComponent::SetLinearDamping(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setLinearDamping(value);
}
float RigidBodyComponent::GetLinearDamping()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getLinearDamping();
}
void RigidBodyComponent::SetAngularDamping(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setAngularDamping(value);
}
float RigidBodyComponent::GetAngularDamping()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getAngularDamping();
}
void RigidBodyComponent::SetAngularVelocity(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setAngularVelocity(StarHelpers::Vector3ToPhysics(value));
}
Vector3 RigidBodyComponent::GetAngularVelocity()
{
	if (!pxRigidBody) return Vector3();

	return StarHelpers::PhysicsToVector3(pxRigidBody->getAngularVelocity());
}
void RigidBodyComponent::UseGravity(bool value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}
bool RigidBodyComponent::HasUseGravity()
{
	if (!pxRigidBody) return false;

	return !pxRigidBody->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
}
void RigidBodyComponent::SetKinematic(bool value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}
bool RigidBodyComponent::GetKinematic()
{
	if (!pxRigidBody) return false;

	return pxRigidBody->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
}
void RigidBodyComponent::AddForce(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->addForce(StarHelpers::Vector3ToPhysics(value));
}
void RigidBodyComponent::AddTorque(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->addTorque(StarHelpers::Vector3ToPhysics(value));
}
void RigidBodyComponent::ClearForce()
{
	if (!pxRigidBody) return;

	pxRigidBody->clearForce();
}
void RigidBodyComponent::ClearTorque()
{
	if (!pxRigidBody) return;

	pxRigidBody->clearTorque();
}
void RigidBodyComponent::SetLinearLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, value);
}
bool RigidBodyComponent::GetLinearLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X);
}
void RigidBodyComponent::SetLinearLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, value);
}
bool RigidBodyComponent::GetLinearLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y);
}
void RigidBodyComponent::SetLinearLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, value);
}
bool RigidBodyComponent::GetLinearLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z);
}
void RigidBodyComponent::SetAngularLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, value);
}
bool RigidBodyComponent::GetAngularLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
}
void RigidBodyComponent::SetAngularLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, value);
}
bool RigidBodyComponent::GetAngularLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
}
void RigidBodyComponent::SetAngularLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, value);
}
bool RigidBodyComponent::GetAngularLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
}
void RigidBodyComponent::SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
{
	// missing code
	return;
}
bool RigidBodyComponent::GetLock(physx::PxRigidDynamicLockFlag::Enum flag)
{
	// missing code
	return false;
}
void RigidBodyComponent::SetTransform(physx::PxTransform value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setGlobalPose(value);
}
physx::PxTransform RigidBodyComponent::GetTransform()
{
	if (!pxRigidBody) return physx::PxTransform();

	return pxRigidBody->getGlobalPose();
}
void RigidBodyComponent::ReleaseActor()
{
	if (!pxRigidBody) return;

	if (pxRigidBody) pxRigidBody->release();
}
physx::PxRigidBody* RigidBodyComponent::GetRigidBody()
{
	if (!pxRigidBody) return NULL;

	return pxRigidBody;
}

void RigidBodyComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "RigidBodyComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "Mass" << YAML::Value << GetMass();
		out << YAML::Key << "LinearDamping" << YAML::Value << GetLinearDamping();
		out << YAML::Key << "AngularDamping" << YAML::Value << GetAngularDamping();
		out << YAML::Key << "UseGravity" << YAML::Value << HasUseGravity();
		out << YAML::Key << "IsKinematic" << YAML::Value << GetKinematic();
		out << YAML::Key << "Freeze";
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Position";
			out << YAML::Flow;
			out << YAML::BeginSeq << GetLinearLockX() << GetLinearLockY() << GetLinearLockZ() << YAML::EndSeq;
			out << YAML::Key << "Rotation";
			out << YAML::Flow;
			out << YAML::BeginSeq << GetAngularLockX() << GetAngularLockY() << GetAngularLockZ() << YAML::EndSeq;
		}
		out << YAML::EndMap;
	}
	out << YAML::EndMap;
}
void RigidBodyComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node rigidBodyComponent = in["RigidBodyComponent"];
	if (rigidBodyComponent)
	{
		SetMass(rigidBodyComponent["Mass"].as<float>());
		SetLinearDamping(rigidBodyComponent["LinearDamping"].as<float>());
		SetAngularDamping(rigidBodyComponent["AngularDamping"].as<float>());
		UseGravity(rigidBodyComponent["UseGravity"].as<bool>());
		SetKinematic(rigidBodyComponent["IsKinematic"].as<bool>());
		YAML::Node freeze = rigidBodyComponent["Freeze"];
		if (freeze)
		{
			YAML::Node position = freeze["Position"];
			if (position)
			{
				if (position.IsSequence())
				{
					SetLinearLockX(position[0].as<bool>());
					SetLinearLockY(position[1].as<bool>());
					SetLinearLockZ(position[2].as<bool>());
				}
			}
			YAML::Node rotation = freeze["Rotation"];
			if (rotation)
			{
				if (rotation.IsSequence())
				{
					SetAngularLockX(rotation[0].as<bool>());
					SetAngularLockY(rotation[1].as<bool>());
					SetAngularLockZ(rotation[2].as<bool>());
				}
			}
		}
	}
}