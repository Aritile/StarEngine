#include "RigidDynamicComponent.h"
#include "../../IMGUI/imgui.h"
#include <entt/entt.hpp>
#include "../../ENTITY/Entity.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../SYSTEM/PhysicsSystem.h"

// component
#include "../../ENTITY/COMPONENT/ConvexColliderComponent.h"
#include "../../ENTITY/COMPONENT/TriangleColliderComponent.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

static Entity* ecs = Entity::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

void RigidDynamicComponent::Render()
{
	if (ImGui::CollapsingHeader("RIGID DYNAMIC", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("RigidDynamicComponentTable", 2, ImGuiTableFlags_Resizable))
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
				if (ImGui::DragFloat("##MassRigidDynamicComponent", &mass, 0.1f, 0.0f, FLT_MAX))
					SetMass(mass);

				float linearDamping = GetLinearDamping();
				if (ImGui::DragFloat("##LinearDampingRigidDynamicComponent", &linearDamping, 0.1f, 0.0f, FLT_MAX))
					SetLinearDamping(linearDamping);

				float angularDamping = GetAngularDamping();
				if (ImGui::DragFloat("##AngularDampingRigidDynamicComponent", &angularDamping, 0.1f, 0.0f, FLT_MAX))
					SetAngularDamping(angularDamping);

				bool useGravity = HasUseGravity();
				if (ImGui::Checkbox("##UseGravityRigidDynamicComponent", &useGravity))
					UseGravity(useGravity);

				bool getKinematic = GetKinematic();
				if (ImGui::Checkbox("##IsKinematicRigidDynamicComponent", &getKinematic))
					SetKinematic(getKinematic);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				bool px = GetLinearLockX();
				if (ImGui::Checkbox("X##PositionRigidDynamicComponent", &px))
					SetLinearLockX(px);
				ImGui::SameLine();
				bool py = GetLinearLockY();
				if (ImGui::Checkbox("Y##PositionRigidDynamicComponent", &py))
					SetLinearLockY(py);
				ImGui::SameLine();
				bool pz = GetLinearLockZ();
				if (ImGui::Checkbox("Z##PositionRigidDynamicComponent", &pz))
					SetLinearLockZ(pz);

				bool rx = GetAngularLockX();
				if (ImGui::Checkbox("X##RotationRigidDynamicComponent", &rx))
					SetAngularLockX(rx);
				ImGui::SameLine();
				bool ry = GetAngularLockY();
				if (ImGui::Checkbox("Y##RotationRigidDynamicComponent", &ry))
					SetAngularLockY(ry);
				ImGui::SameLine();
				bool rz = GetAngularLockZ();
				if (ImGui::Checkbox("Z##RotationRigidDynamicComponent", &rz))
					SetAngularLockZ(rz);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void RigidDynamicComponent::UpdateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	if (rigidDynamic)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			physx::PxTransform pxTransform = rigidDynamic->getGlobalPose();
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			transformComponent.SetPosition(Star::PhysicsToVector3(pxTransform.p));
			transformComponent.SetRotationQuaternion(Star::PhysicsToQuat(pxTransform.q));
		}
	}
}

void RigidDynamicComponent::CreateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		physx::PxTransform pxTransform = physx::PxTransform(
			Star::Vector3ToPhysics(transformComponent.GetPosition()),
			Star::QuatToPhysics(transformComponent.GetRotationQuaternion()));
		//printf("pos %f %f %f\n", transformComponent.GetPosition().x, transformComponent.GetPosition().y, transformComponent.GetPosition().z);
		rigidDynamic = physicsSystem->GetPhysics()->createRigidDynamic(pxTransform);
		if (!rigidDynamic) consoleWindow->AddWarningMessage("[RigidBody] -> Failed to create the RigidBody!");
		physicsSystem->GetScene()->addActor(*rigidDynamic); /*<*>*/

		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			for (size_t i = 0; i < physicsComponent.GetBoxColliders()->size(); i++)
			{
				BoxColliderComponent index = physicsComponent.GetBoxColliders()->at(i);
				if (index.GetShape())
					rigidDynamic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach box shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetSphereColliders()->size(); i++)
			{
				SphereColliderComponent index = physicsComponent.GetSphereColliders()->at(i);
				if (index.GetShape())
					rigidDynamic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach sphere shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetCapsuleColliders()->size(); i++)
			{
				CapsuleColliderComponent index = physicsComponent.GetCapsuleColliders()->at(i);
				if (index.GetShape())
					rigidDynamic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach capsule shape to the Rigidbody!");
			}

			if (ecs->HasComponent<ConvexColliderComponent>(entity))
			{
				auto& convexColliderComponent = ecs->GetComponent<ConvexColliderComponent>(entity);
				if (convexColliderComponent.pxShape)
					rigidDynamic->attachShape(*convexColliderComponent.pxShape);
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach convex shape to the Rigidbody!");
			}

			if (ecs->HasComponent<TriangleColliderComponent>(entity))
			{
				auto& triangleColliderComponent = ecs->GetComponent<TriangleColliderComponent>(entity);
				if (triangleColliderComponent.pxShape)
					rigidDynamic->attachShape(*triangleColliderComponent.pxShape);
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach triangle shape to the Rigidbody!");
			}
		}
	}
}

void RigidDynamicComponent::SetMass(float value)
{
	if (value < 0.0f) return;
	if (!rigidDynamic) return;

	rigidDynamic->setMass(value);
}
float RigidDynamicComponent::GetMass()
{
	if (!rigidDynamic) return 0.0f;

	return rigidDynamic->getMass();
}
void RigidDynamicComponent::SetLinearVelocity(Vector3 value)
{
	if (!rigidDynamic) return;

	rigidDynamic->setLinearVelocity(Star::Vector3ToPhysics(value));
}
float RigidDynamicComponent::GetMagnitude()
{
	if (!rigidDynamic) return 0.0f;
	return rigidDynamic->getLinearVelocity().magnitude();
}
Vector3 RigidDynamicComponent::GetLinearVelocity()
{
	if (!rigidDynamic) return Vector3();

	return Star::PhysicsToVector3(rigidDynamic->getLinearVelocity());
}
void RigidDynamicComponent::SetLinearDamping(float value)
{
	if (value < 0.0f) return;
	if (!rigidDynamic) return;

	rigidDynamic->setLinearDamping(value);
}
float RigidDynamicComponent::GetLinearDamping()
{
	if (!rigidDynamic) return 0.0f;

	return rigidDynamic->getLinearDamping();
}
void RigidDynamicComponent::SetAngularDamping(float value)
{
	if (value < 0.0f) return;
	if (!rigidDynamic) return;

	rigidDynamic->setAngularDamping(value);
}
float RigidDynamicComponent::GetAngularDamping()
{
	if (!rigidDynamic) return 0.0f;

	return rigidDynamic->getAngularDamping();
}
void RigidDynamicComponent::SetAngularVelocity(Vector3 value)
{
	if (!rigidDynamic) return;

	rigidDynamic->setAngularVelocity(Star::Vector3ToPhysics(value));
}
Vector3 RigidDynamicComponent::GetAngularVelocity()
{
	if (!rigidDynamic) return Vector3();

	return Star::PhysicsToVector3(rigidDynamic->getAngularVelocity());
}
void RigidDynamicComponent::UseGravity(bool value)
{
	if (!rigidDynamic) return;

	rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}
bool RigidDynamicComponent::HasUseGravity()
{
	if (!rigidDynamic) return false;

	return !rigidDynamic->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
}
void RigidDynamicComponent::SetKinematic(bool value)
{
	if (!rigidDynamic) return;

	rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}
bool RigidDynamicComponent::GetKinematic()
{
	if (!rigidDynamic) return false;

	return rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
}
void RigidDynamicComponent::AddForce(Vector3 value)
{
	if (!rigidDynamic) return;

	rigidDynamic->addForce(Star::Vector3ToPhysics(value));
}
void RigidDynamicComponent::AddTorque(Vector3 value)
{
	if (!rigidDynamic) return;

	rigidDynamic->addTorque(Star::Vector3ToPhysics(value));
}
void RigidDynamicComponent::ClearForce()
{
	if (!rigidDynamic) return;

	rigidDynamic->clearForce();
}
void RigidDynamicComponent::ClearTorque()
{
	if (!rigidDynamic) return;

	rigidDynamic->clearTorque();
}
void RigidDynamicComponent::SetLinearLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, value);
}
bool RigidDynamicComponent::GetLinearLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X);
}
void RigidDynamicComponent::SetLinearLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, value);
}
bool RigidDynamicComponent::GetLinearLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y);
}
void RigidDynamicComponent::SetLinearLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, value);
}
bool RigidDynamicComponent::GetLinearLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z);
}
void RigidDynamicComponent::SetAngularLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, value);
}
bool RigidDynamicComponent::GetAngularLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
}
void RigidDynamicComponent::SetAngularLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, value);
}
bool RigidDynamicComponent::GetAngularLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
}
void RigidDynamicComponent::SetAngularLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, value);
}
bool RigidDynamicComponent::GetAngularLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
}
void RigidDynamicComponent::SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
{
	// missing code
	return;
}
bool RigidDynamicComponent::GetLock(physx::PxRigidDynamicLockFlag::Enum flag)
{
	// missing code
	return false;
}
void RigidDynamicComponent::SetTransform(Matrix value)
{
	if (!rigidDynamic)
		return;

	physx::PxTransform trans(Star::MatrixToPhysics(value));
	rigidDynamic->setGlobalPose(trans);
}
Matrix RigidDynamicComponent::GetTransform()
{
	Matrix matrix;

	if (!rigidDynamic)
		return matrix;

	physx::PxTransform trans(rigidDynamic->getGlobalPose());
	matrix = Star::PhysicsToMatrix(trans);
	return matrix;
}
void RigidDynamicComponent::ReleaseActor()
{
	if (!rigidDynamic) return;
	if (rigidDynamic) rigidDynamic->release();
}
physx::PxRigidDynamic* RigidDynamicComponent::GetRigidDynamic()
{
	return rigidDynamic;
}

void RigidDynamicComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "RigidDynamicComponent";
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
void RigidDynamicComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node RigidDynamicComponent = in["RigidDynamicComponent"];
	if (RigidDynamicComponent)
	{
		SetMass(RigidDynamicComponent["Mass"].as<float>());
		SetLinearDamping(RigidDynamicComponent["LinearDamping"].as<float>());
		SetAngularDamping(RigidDynamicComponent["AngularDamping"].as<float>());
		UseGravity(RigidDynamicComponent["UseGravity"].as<bool>());
		SetKinematic(RigidDynamicComponent["IsKinematic"].as<bool>());
		YAML::Node freeze = RigidDynamicComponent["Freeze"];
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

void RigidDynamicComponent::LuaAdd(sol::state& state)
{
	sol::usertype<RigidDynamicComponent> component = state.new_usertype<RigidDynamicComponent>(
		"RigidDynamicComponent");
	component["SetMass"] = &RigidDynamicComponent::SetMass;
	component["GetMass"] = &RigidDynamicComponent::GetMass;
	component["SetLinearVelocity"] = &RigidDynamicComponent::SetLinearVelocity;
	component["GetLinearVelocity"] = &RigidDynamicComponent::GetLinearVelocity;
	component["SetAngularVelocity"] = &RigidDynamicComponent::SetAngularVelocity;
	component["GetAngularVelocity"] = &RigidDynamicComponent::GetAngularVelocity;
	component["SetLinearDamping"] = &RigidDynamicComponent::SetLinearDamping;
	component["GetLinearDamping"] = &RigidDynamicComponent::GetLinearDamping;
	component["SetAngularDamping"] = &RigidDynamicComponent::SetAngularDamping;
	component["GetAngularDamping"] = &RigidDynamicComponent::GetAngularDamping;
	component["SetGravity"] = &RigidDynamicComponent::UseGravity;
	component["GetGravity"] = &RigidDynamicComponent::HasUseGravity;
	component["SetKinematic"] = &RigidDynamicComponent::SetKinematic;
	component["GetKinematic"] = &RigidDynamicComponent::GetKinematic;
	component["AddForce"] = &RigidDynamicComponent::AddForce;
	component["AddTorque"] = &RigidDynamicComponent::AddTorque;
	component["ClearForce"] = &RigidDynamicComponent::ClearForce;
	component["ClearTorque"] = &RigidDynamicComponent::ClearTorque;
	component["GetMagnitude"] = &RigidDynamicComponent::GetMagnitude;
	component["SetPosition"] = &RigidDynamicComponent::SetPosition;
	component["GetPosition"] = &RigidDynamicComponent::GetPosition;
	component["SetRotationYawPitchRoll"] = &RigidDynamicComponent::SetRotationYawPitchRoll;
	component["SetRotationQuaternion"] = &RigidDynamicComponent::SetRotationQuaternion;
	component["GetRotationQuaternion"] = &RigidDynamicComponent::GetRotationQuaternion;
}
void RigidDynamicComponent::SetPosition(Vector3 value)
{
	if (!rigidDynamic) return;
	physx::PxTransform trans(rigidDynamic->getGlobalPose()); // for rotation
	trans.p = Star::Vector3ToPhysics(value);
	rigidDynamic->setGlobalPose(trans);
}
Vector3 RigidDynamicComponent::GetPosition()
{
	Vector3 vector;
	if (!rigidDynamic) return vector;
	physx::PxTransform trans(rigidDynamic->getGlobalPose()); // for rotation
	vector = Star::PhysicsToVector3(trans.p);
	return vector;
}
void RigidDynamicComponent::SetRotationYawPitchRoll(Vector3 value)
{
	if (!rigidDynamic) return;
	physx::PxTransform trans(rigidDynamic->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(Quaternion::CreateFromYawPitchRoll(value));
	rigidDynamic->setGlobalPose(trans);
}
void RigidDynamicComponent::SetRotationQuaternion(Quaternion value)
{
	if (!rigidDynamic) return;
	physx::PxTransform trans(rigidDynamic->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(value);
	rigidDynamic->setGlobalPose(trans);
}
Quaternion RigidDynamicComponent::GetRotationQuaternion()
{
	Quaternion quaternion;
	if (!rigidDynamic) return quaternion;
	physx::PxTransform trans(rigidDynamic->getGlobalPose()); // for position
	quaternion = Star::PhysicsToQuat(trans.q);
	return quaternion;
}
void RigidDynamicComponent::SetNullActor()
{
	rigidDynamic = nullptr;
}