#pragma once

#include <physx/PxPhysicsAPI.h>
#include <SimpleMath.h>
#include <yaml-cpp/yaml.h>
#include <sol/sol.hpp>

using namespace DirectX::SimpleMath;

struct RigidStaticComponent
{
public:
	void Render();

public:
	void CreateActor();
	void UpdateActor();
	void ReleaseActor();

public:
	physx::PxRigidStatic* GetRigidStatic();

public:
	void SetTransform(Matrix value);
	Matrix GetTransform();
	void SetPosition(Vector3 value);
	Vector3 GetPosition();
	void SetRotationYawPitchRoll(Vector3 value);
	void SetRotationQuaternion(Quaternion value);
	Quaternion GetRotationQuaternion();
	void SetNullActor();

private:
	physx::PxRigidStatic* rigidStatic = nullptr;

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);

public:
	static void LuaAdd(sol::state& state);
};