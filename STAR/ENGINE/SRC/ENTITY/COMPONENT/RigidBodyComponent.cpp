#include "RigidbodyComponent.h"
#include "../../IMGUI/imgui.h"
#include <entt/entt.hpp>
#include "../../ENTITY/Entity.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../SYSTEM/PhysicsSystem.h"

static Entity* ecs = Entity::GetSingleton();
static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

void RigidbodyComponent::Render()
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

void RigidbodyComponent::UpdateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	if (pxRigidBody)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			physx::PxTransform pxTransform = pxRigidBody->getGlobalPose();
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			transformComponent.SetPosition(Star::PhysicsToVector3(pxTransform.p));
			transformComponent.SetRotationQuaternion(Star::PhysicsToQuat(pxTransform.q));
		}
	}
}

void RigidbodyComponent::CreateActor()
{
	if (pxRigidBody) pxRigidBody->release();
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		physx::PxTransform pxTransform = physx::PxTransform(
			Star::Vector3ToPhysics(transformComponent.GetPosition()),
			Star::QuatToPhysics(transformComponent.GetRotationQuaternion()));
		//printf("pos %f %f %f\n", transformComponent.GetPosition().x, transformComponent.GetPosition().y, transformComponent.GetPosition().z);
		pxRigidBody = physicsSystem->GetPhysics()->createRigidDynamic(pxTransform);
		if (!pxRigidBody) consoleWindow->AddWarningMessage("[RigidBody] -> Failed to create the RigidBody!");
		physicsSystem->GetScene()->addActor(*pxRigidBody); /*<*>*/

		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			for (size_t i = 0; i < physicsComponent.GetBoxColliders()->size(); i++)
			{
				BoxColliderComponent index = physicsComponent.GetBoxColliders()->at(i);
				if (index.GetShape())
					pxRigidBody->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach box shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetSphereColliders()->size(); i++)
			{
				SphereColliderComponent index = physicsComponent.GetSphereColliders()->at(i);
				if (index.GetShape())
					pxRigidBody->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach sphere shape to the Rigidbody!");
			}
			for (size_t i = 0; i < physicsComponent.GetCapsuleColliders()->size(); i++)
			{
				CapsuleColliderComponent index = physicsComponent.GetCapsuleColliders()->at(i);
				if (index.GetShape())
					pxRigidBody->attachShape(*index.GetShape());
				else
					consoleWindow->AddWarningMessage("[Rigidbody] -> Failed to attach capsule shape to the Rigidbody!");
			}
		}
	}
}

void RigidbodyComponent::SetMass(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setMass(value);
}
float RigidbodyComponent::GetMass()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getMass();
}
void RigidbodyComponent::SetLinearVelocity(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setLinearVelocity(Star::Vector3ToPhysics(value));
}
float RigidbodyComponent::GetMagnitude()
{
	if (!pxRigidBody) return 0.0f;
	return pxRigidBody->getLinearVelocity().magnitude();
}
Vector3 RigidbodyComponent::GetLinearVelocity()
{
	if (!pxRigidBody) return Vector3();

	return Star::PhysicsToVector3(pxRigidBody->getLinearVelocity());
}
void RigidbodyComponent::SetLinearDamping(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setLinearDamping(value);
}
float RigidbodyComponent::GetLinearDamping()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getLinearDamping();
}
void RigidbodyComponent::SetAngularDamping(float value)
{
	if (value < 0.0f) return;
	if (!pxRigidBody) return;

	pxRigidBody->setAngularDamping(value);
}
float RigidbodyComponent::GetAngularDamping()
{
	if (!pxRigidBody) return 0.0f;

	return pxRigidBody->getAngularDamping();
}
void RigidbodyComponent::SetAngularVelocity(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setAngularVelocity(Star::Vector3ToPhysics(value));
}
Vector3 RigidbodyComponent::GetAngularVelocity()
{
	if (!pxRigidBody) return Vector3();

	return Star::PhysicsToVector3(pxRigidBody->getAngularVelocity());
}
void RigidbodyComponent::UseGravity(bool value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}
bool RigidbodyComponent::HasUseGravity()
{
	if (!pxRigidBody) return false;

	return !pxRigidBody->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
}
void RigidbodyComponent::SetKinematic(bool value)
{
	if (!pxRigidBody) return;

	pxRigidBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}
bool RigidbodyComponent::GetKinematic()
{
	if (!pxRigidBody) return false;

	return pxRigidBody->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
}
void RigidbodyComponent::AddForce(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->addForce(Star::Vector3ToPhysics(value));
}
void RigidbodyComponent::AddTorque(Vector3 value)
{
	if (!pxRigidBody) return;

	pxRigidBody->addTorque(Star::Vector3ToPhysics(value));
}
void RigidbodyComponent::ClearForce()
{
	if (!pxRigidBody) return;

	pxRigidBody->clearForce();
}
void RigidbodyComponent::ClearTorque()
{
	if (!pxRigidBody) return;

	pxRigidBody->clearTorque();
}
void RigidbodyComponent::SetLinearLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, value);
}
bool RigidbodyComponent::GetLinearLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X);
}
void RigidbodyComponent::SetLinearLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, value);
}
bool RigidbodyComponent::GetLinearLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y);
}
void RigidbodyComponent::SetLinearLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, value);
}
bool RigidbodyComponent::GetLinearLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z);
}
void RigidbodyComponent::SetAngularLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, value);
}
bool RigidbodyComponent::GetAngularLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
}
void RigidbodyComponent::SetAngularLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, value);
}
bool RigidbodyComponent::GetAngularLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
}
void RigidbodyComponent::SetAngularLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, value);
}
bool RigidbodyComponent::GetAngularLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
}
void RigidbodyComponent::SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
{
	// missing code
	return;
}
bool RigidbodyComponent::GetLock(physx::PxRigidDynamicLockFlag::Enum flag)
{
	// missing code
	return false;
}
void RigidbodyComponent::SetTransform(Matrix value)
{
	if (!pxRigidBody)
		return;

	physx::PxTransform trans(Star::MatrixToPhysics(value));
	pxRigidBody->setGlobalPose(trans);
}
Matrix RigidbodyComponent::GetTransform()
{
	Matrix matrix;

	if (!pxRigidBody)
		return matrix;

	physx::PxTransform trans(pxRigidBody->getGlobalPose());
	matrix = Star::PhysicsToMatrix(trans);
	return matrix;
}
void RigidbodyComponent::ReleaseActor()
{
	if (!pxRigidBody) return;

	if (pxRigidBody) pxRigidBody->release();
}
physx::PxRigidBody* RigidbodyComponent::GetRigidBody()
{
	if (!pxRigidBody) return NULL;

	return pxRigidBody;
}

void RigidbodyComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "RigidbodyComponent";
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
void RigidbodyComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node rigidBodyComponent = in["RigidbodyComponent"];
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

void RigidbodyComponent::LuaAdd(sol::state& state)
{
	sol::usertype<RigidbodyComponent> component = state.new_usertype<RigidbodyComponent>(
		"RigidbodyComponent");
	component["SetMass"] = &RigidbodyComponent::SetMass;
	component["GetMass"] = &RigidbodyComponent::GetMass;
	component["SetLinearVelocity"] = &RigidbodyComponent::SetLinearVelocity;
	component["GetLinearVelocity"] = &RigidbodyComponent::GetLinearVelocity;
	component["SetAngularVelocity"] = &RigidbodyComponent::SetAngularVelocity;
	component["GetAngularVelocity"] = &RigidbodyComponent::GetAngularVelocity;
	component["SetLinearDamping"] = &RigidbodyComponent::SetLinearDamping;
	component["GetLinearDamping"] = &RigidbodyComponent::GetLinearDamping;
	component["SetAngularDamping"] = &RigidbodyComponent::SetAngularDamping;
	component["GetAngularDamping"] = &RigidbodyComponent::GetAngularDamping;
	component["SetGravity"] = &RigidbodyComponent::UseGravity;
	component["GetGravity"] = &RigidbodyComponent::HasUseGravity;
	component["SetKinematic"] = &RigidbodyComponent::SetKinematic;
	component["GetKinematic"] = &RigidbodyComponent::GetKinematic;
	component["AddForce"] = &RigidbodyComponent::AddForce;
	component["AddTorque"] = &RigidbodyComponent::AddTorque;
	component["ClearForce"] = &RigidbodyComponent::ClearForce;
	component["ClearTorque"] = &RigidbodyComponent::ClearTorque;
	component["GetMagnitude"] = &RigidbodyComponent::GetMagnitude;
	component["SetPosition"] = &RigidbodyComponent::SetPosition;
	component["GetPosition"] = &RigidbodyComponent::GetPosition;
	component["SetRotationYawPitchRoll"] = &RigidbodyComponent::SetRotationYawPitchRoll;
	component["SetRotationQuaternion"] = &RigidbodyComponent::SetRotationQuaternion;
	component["GetRotationQuaternion"] = &RigidbodyComponent::GetRotationQuaternion;
}
void RigidbodyComponent::SetPosition(Vector3 value)
{
	if (!pxRigidBody) return;
	physx::PxTransform trans(pxRigidBody->getGlobalPose()); // for rotation
	trans.p = Star::Vector3ToPhysics(value);
	pxRigidBody->setGlobalPose(trans);
}
Vector3 RigidbodyComponent::GetPosition()
{
	Vector3 vector;
	if (!pxRigidBody) return vector;
	physx::PxTransform trans(pxRigidBody->getGlobalPose()); // for rotation
	vector = Star::PhysicsToVector3(trans.p);
	return vector;
}
void RigidbodyComponent::SetRotationYawPitchRoll(Vector3 value)
{
	if (!pxRigidBody) return;
	physx::PxTransform trans(pxRigidBody->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(Quaternion::CreateFromYawPitchRoll(value));
	pxRigidBody->setGlobalPose(trans);
}
void RigidbodyComponent::SetRotationQuaternion(Quaternion value)
{
	if (!pxRigidBody) return;
	physx::PxTransform trans(pxRigidBody->getGlobalPose()); // for position
	trans.q = Star::QuatToPhysics(value);
	pxRigidBody->setGlobalPose(trans);
}
Quaternion RigidbodyComponent::GetRotationQuaternion()
{
	Quaternion quaternion;
	if (!pxRigidBody) return quaternion;
	physx::PxTransform trans(pxRigidBody->getGlobalPose()); // for position
	quaternion = Star::PhysicsToQuat(trans.q);
	return quaternion;
}
void RigidbodyComponent::SetNullActor()
{
	pxRigidBody = nullptr;
}