#pragma once

#include <physx/PxPhysicsAPI.h>
#include <SimpleMath.h>
#include <yaml-cpp/yaml.h>
#include <sol/sol.hpp>

using namespace DirectX::SimpleMath;

struct RigidDynamicComponent
{
public:
	void Render();

public:
	void CreateActor();
	void UpdateActor();
	void ReleaseActor();

public:
	physx::PxRigidDynamic* GetRigidDynamic();

public:
	void SetMass(float value);
	float GetMass();
	void SetLinearVelocity(Vector3 value);
	Vector3 GetLinearVelocity();
	void SetAngularVelocity(Vector3 value);
	Vector3 GetAngularVelocity();
	void SetLinearDamping(float value);
	float GetLinearDamping();
	void SetAngularDamping(float value);
	float GetAngularDamping();
	void UseGravity(bool value);
	bool HasUseGravity();
	void SetKinematic(bool value);
	bool GetKinematic();
	void AddForce(Vector3 value);
	void AddTorque(Vector3 value);
	void ClearForce();
	void ClearTorque();
	void SetLinearLockX(bool value);
	bool GetLinearLockX();
	void SetLinearLockY(bool value);
	bool GetLinearLockY();
	void SetLinearLockZ(bool value);
	bool GetLinearLockZ();
	void SetAngularLockX(bool value);
	bool GetAngularLockX();
	void SetAngularLockY(bool value);
	bool GetAngularLockY();
	void SetAngularLockZ(bool value);
	bool GetAngularLockZ();
	float GetMagnitude();
	void SetTransform(Matrix value);
	Matrix GetTransform();
	void SetPosition(Vector3 value);
	Vector3 GetPosition();
	void SetRotationYawPitchRoll(Vector3 value);
	void SetRotationQuaternion(Quaternion value);
	Quaternion GetRotationQuaternion();
	void SetNullActor();

public:
	void SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value);
	bool GetLock(physx::PxRigidDynamicLockFlag::Enum flag);

private:
	physx::PxRigidDynamic* rigidDynamic = nullptr;

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);

public:
	static void LuaAdd(sol::state& state);
};