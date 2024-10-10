#include "CapsuleColliderComponent.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../STAR/Star.h"
#include "../../IMGUI/imgui.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../ENTITY/COMPONENT/RigidBodyComponent.h"

static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();
static Entity* ecs = Entity::GetSingleton();

bool CapsuleColliderComponent::CreateShape(float _Radius, float _Height)
{
	if (pxShape) pxShape->release();
	if (physicsMaterialComponent.GetMaterial()) physicsMaterialComponent.ReleaseMaterial();
	physicsMaterialComponent.CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);

	pxShape = physicsSystem->GetPhysics()->createShape(physx::PxCapsuleGeometry(
		_Radius,
		_Height),
		*physicsMaterialComponent.GetMaterial(),
		true);

	if (!pxShape)
	{
		Star::AddLog("[PhysX] -> Failed to create capsule shape.");
		return false;
	}
	
	// capsule is reversed, reverse of 90 degress
	//Quaternion quat;
	//quat = Quaternion::CreateFromYawPitchRoll(Vector3(Star::DegToRad(90.0f), 0.0f, 0.0f));
	//pxShape->setLocalPose(physx::PxTransform(0.0f, 0.0f, 0.0f, Star::QuatToPhysics(quat)));
	return true;
}

void CapsuleColliderComponent::Render(std::vector<CapsuleColliderComponent>* ccc, size_t index)
{
	ImGui::Checkbox("##CAPSULECOLLIDER", &activeComponent);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("CAPSULECOLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				if (ecs->registry.any_of<RigidbodyComponent>(ecs->selected))
					ecs->registry.get<RigidbodyComponent>(ecs->selected).GetRigidBody()->detachShape(*GetShape());
				Release();
				ccc->erase(ccc->begin() + index);
				ImGui::EndPopup();
				return; // !
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("PhysicsComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				physicsMaterialComponent.RenderLeft(true);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Center");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Radius");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Height");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				physicsMaterialComponent.RenderRight();

				Vector3 _Center = GetCenter();
				if (ImGui::DragFloat3("##CenterCapsuleColliderComponent", (float*)&_Center, 0.1f))
					SetCenter(_Center);

				float _Radius = GetRadius();
				if (ImGui::DragFloat("##RadiusCapsuleColliderComponent", &_Radius, 0.1f, 0.0f, FLT_MAX))
				{
					SetRadius(_Radius);
					Update();
				}

				float _Height = GetHeight();
				if (ImGui::DragFloat("##HeightCapsuleColliderComponent", &_Height, 0.1f, 0.0f, FLT_MAX))
				{
					SetHeight(_Height);
					Update();
				}
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}
physx::PxShape* CapsuleColliderComponent::GetShape()
{
	return pxShape;
}
void CapsuleColliderComponent::Release()
{
	if (GetShape()) GetShape()->release();
	if (physicsMaterialComponent.GetMaterial()) physicsMaterialComponent.ReleaseMaterial();
}
void CapsuleColliderComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "CapsuleCollider" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Active" << YAML::Value << activeComponent;
	out << YAML::Key << "Center"; Star::SerializeVector3(out, GetCenter());
	out << YAML::Key << "Radius" << YAML::Value << GetRadius();
	out << YAML::Key << "Height" << YAML::Value << GetHeight();
	out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "StaticFriction" << YAML::Value << physicsMaterialComponent.GetStaticFriction();
	out << YAML::Key << "DynamicFriction" << YAML::Value << physicsMaterialComponent.GetDynamicFriction();
	out << YAML::Key << "Restitution" << YAML::Value << physicsMaterialComponent.GetRestitution();
	out << YAML::EndMap;
	out << YAML::EndMap;
	out << YAML::EndMap;
}
void CapsuleColliderComponent::DeserializeComponent(YAML::Node& in)
{
	activeComponent = in["Active"].as<bool>();
	auto center = in["Center"];
	SetCenter(Star::DeserializeVector3(center));
	SetRadius(in["Radius"].as<float>());
	SetHeight(in["Height"].as<float>());
	auto material = in["Material"];
	physicsMaterialComponent.SetStaticFriction(material["StaticFriction"].as<float>());
	physicsMaterialComponent.SetDynamicFriction(material["DynamicFriction"].as<float>());
	physicsMaterialComponent.SetRestitution(material["Restitution"].as<float>());
}

void CapsuleColliderComponent::SetCenter(Vector3 value)
{
	if (pxShape) pxShape->setLocalPose(physx::PxTransform(value.x, value.y, value.z));
}
Vector3 CapsuleColliderComponent::GetCenter() const
{
	physx::PxTransform pxTransform;
	if (pxShape) pxTransform = pxShape->getLocalPose();
	return Vector3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
}
void CapsuleColliderComponent::SetRadius(float value)
{
	lastRadius = value;
}
float CapsuleColliderComponent::GetRadius()
{
	return lastRadius;
}
void CapsuleColliderComponent::Update()
{
	if (pxShape)
	{
		physx::PxCapsuleGeometry geometry;
		geometry.radius = Star::FloatToPhysics(lastExtent + lastRadius - 1.0f);
		geometry.halfHeight = lastHeight;
		pxShape->setGeometry(geometry);
	}
}
void CapsuleColliderComponent::SetExtent(float value)
{
	lastExtent = value;
}
float CapsuleColliderComponent::GetExtent()
{
	return lastExtent;
}
void CapsuleColliderComponent::SetHeight(float value)
{
	lastHeight = value;
}
float CapsuleColliderComponent::GetHeight()
{
	return lastHeight;
}