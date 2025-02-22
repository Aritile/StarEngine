#include "RigidStaticComponent.h"
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

void RigidStaticComponent::Render()
{
	if (ImGui::CollapsingHeader("RIGID DYNAMIC", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("RigidStaticComponentTable", 2, ImGuiTableFlags_Resizable))
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

void RigidStaticComponent::UpdateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	if (rigidStatic)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			physx::PxTransform pxTransform = rigidStatic->getGlobalPose();
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			transformComponent.SetPosition(Star::PhysicsToVector3(pxTransform.p));
			transformComponent.SetRotationQuaternion(Star::PhysicsToQuat(pxTransform.q));
		}
	}
}

void RigidStaticComponent::CreateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		physx::PxTransform pxTransform = physx::PxTransform(
			Star::Vector3ToPhysics(transformComponent.GetPosition()),
			Star::QuatToPhysics(transformComponent.GetRotationQuaternion()));
		//printf("pos %f %f %f\n", transformComponent.GetPosition().x, transformComponent.GetPosition().y, transformComponent.GetPosition().z);
		rigidStatic = physicsSystem->GetPhysics()->createRigidStatic(pxTransform);
		if (!rigidStatic) consoleWindow->AddWarningMessage("[RigidBody] -> Failed to create the RigidBody!");
		physicsSystem->GetScene()->addActor(*rigidStatic); /*<*>*/

		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			for (size_t i = 0; i < physicsComponent.GetBoxColliders()->size(); i++)
			{
				BoxColliderComponent index = physicsComponent.GetBoxColliders()->at(i);
				if (index.GetShape())
					rigidStatic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach box shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetSphereColliders()->size(); i++)
			{
				SphereColliderComponent index = physicsComponent.GetSphereColliders()->at(i);
				if (index.GetShape())
					rigidStatic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach sphere shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetCapsuleColliders()->size(); i++)
			{
				CapsuleColliderComponent index = physicsComponent.GetCapsuleColliders()->at(i);
				if (index.GetShape())
					rigidStatic->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach capsule shape to the Rigidbody!");
			}

			if (ecs->HasComponent<ConvexColliderComponent>(entity))
			{
				auto& convexColliderComponent = ecs->GetComponent<ConvexColliderComponent>(entity);
				if (convexColliderComponent.pxShape)
					rigidStatic->attachShape(*convexColliderComponent.pxShape);
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach convex shape to the Rigidbody!");
			}

			if (ecs->HasComponent<TriangleColliderComponent>(entity))
			{
				auto& triangleColliderComponent = ecs->GetComponent<TriangleColliderComponent>(entity);
				if (triangleColliderComponent.pxShape)
					rigidStatic->attachShape(*triangleColliderComponent.pxShape);
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach triangle shape to the Rigidbody!");
			}
		}
	}
}
void RigidStaticComponent::SetTransform(Matrix value)
{
	if (!rigidStatic)
		return;

	physx::PxTransform trans(Star::MatrixToPhysics(value));
	rigidStatic->setGlobalPose(trans);
}
Matrix RigidStaticComponent::GetTransform()
{
	Matrix matrix;

	if (!rigidStatic)
		return matrix;

	physx::PxTransform trans(rigidStatic->getGlobalPose());
	matrix = Star::PhysicsToMatrix(trans);
	return matrix;
}
void RigidStaticComponent::ReleaseActor()
{
	if (!rigidStatic) return;
	if (rigidStatic) rigidStatic->release();
}
physx::PxRigidStatic* RigidStaticComponent::GetRigidStatic()
{
	return rigidStatic;
}

void RigidStaticComponent::SerializeComponent(YAML::Emitter& out)
{
	/*
	out << YAML::Key << "RigidStaticComponent";
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
	*/
}
void RigidStaticComponent::DeserializeComponent(YAML::Node& in)
{
	/*
	YAML::Node RigidStaticComponent = in["RigidStaticComponent"];
	if (RigidStaticComponent)
	{
		SetMass(RigidStaticComponent["Mass"].as<float>());
		SetLinearDamping(RigidStaticComponent["LinearDamping"].as<float>());
		SetAngularDamping(RigidStaticComponent["AngularDamping"].as<float>());
		UseGravity(RigidStaticComponent["UseGravity"].as<bool>());
		SetKinematic(RigidStaticComponent["IsKinematic"].as<bool>());
		YAML::Node freeze = RigidStaticComponent["Freeze"];
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
	*/
}

void RigidStaticComponent::LuaAdd(sol::state& state)
{
	/*
	sol::usertype<RigidStaticComponent> component = state.new_usertype<RigidStaticComponent>(
		"RigidStaticComponent");
	component["SetMass"] = &RigidStaticComponent::SetMass;
	component["GetMass"] = &RigidStaticComponent::GetMass;
	component["SetLinearVelocity"] = &RigidStaticComponent::SetLinearVelocity;
	component["GetLinearVelocity"] = &RigidStaticComponent::GetLinearVelocity;
	component["SetAngularVelocity"] = &RigidStaticComponent::SetAngularVelocity;
	component["GetAngularVelocity"] = &RigidStaticComponent::GetAngularVelocity;
	component["SetLinearDamping"] = &RigidStaticComponent::SetLinearDamping;
	component["GetLinearDamping"] = &RigidStaticComponent::GetLinearDamping;
	component["SetAngularDamping"] = &RigidStaticComponent::SetAngularDamping;
	component["GetAngularDamping"] = &RigidStaticComponent::GetAngularDamping;
	component["SetGravity"] = &RigidStaticComponent::UseGravity;
	component["GetGravity"] = &RigidStaticComponent::HasUseGravity;
	component["SetKinematic"] = &RigidStaticComponent::SetKinematic;
	component["GetKinematic"] = &RigidStaticComponent::GetKinematic;
	component["AddForce"] = &RigidStaticComponent::AddForce;
	component["AddTorque"] = &RigidStaticComponent::AddTorque;
	component["ClearForce"] = &RigidStaticComponent::ClearForce;
	component["ClearTorque"] = &RigidStaticComponent::ClearTorque;
	component["GetMagnitude"] = &RigidStaticComponent::GetMagnitude;
	component["SetPosition"] = &RigidStaticComponent::SetPosition;
	component["GetPosition"] = &RigidStaticComponent::GetPosition;
	component["SetRotationYawPitchRoll"] = &RigidStaticComponent::SetRotationYawPitchRoll;
	component["SetRotationQuaternion"] = &RigidStaticComponent::SetRotationQuaternion;
	component["GetRotationQuaternion"] = &RigidStaticComponent::GetRotationQuaternion;
	*/
}
void RigidStaticComponent::SetPosition(Vector3 value)
{
	if (!rigidStatic) return;
	physx::PxTransform trans(rigidStatic->getGlobalPose()); // for rotation
	trans.p = Star::Vector3ToPhysics(value);
	rigidStatic->setGlobalPose(trans);
}
Vector3 RigidStaticComponent::GetPosition()
{
	Vector3 vector;
	if (!rigidStatic) return vector;
	physx::PxTransform trans(rigidStatic->getGlobalPose()); // for rotation
	vector = Star::PhysicsToVector3(trans.p);
	return vector;
}
void RigidStaticComponent::SetRotationYawPitchRoll(Vector3 value)
{
	if (!rigidStatic) return;
	physx::PxTransform trans(rigidStatic->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(Quaternion::CreateFromYawPitchRoll(value));
	rigidStatic->setGlobalPose(trans);
}
void RigidStaticComponent::SetRotationQuaternion(Quaternion value)
{
	if (!rigidStatic) return;
	physx::PxTransform trans(rigidStatic->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(value);
	rigidStatic->setGlobalPose(trans);
}
Quaternion RigidStaticComponent::GetRotationQuaternion()
{
	Quaternion quaternion;
	if (!rigidStatic) return quaternion;
	physx::PxTransform trans(rigidStatic->getGlobalPose()); // for position
	quaternion = Star::PhysicsToQuat(trans.q);
	return quaternion;
}
void RigidStaticComponent::SetNullActor()
{
	rigidStatic = nullptr;
}