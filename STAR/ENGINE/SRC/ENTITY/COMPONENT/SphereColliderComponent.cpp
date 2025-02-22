#include "SphereColliderComponent.h"
#include "../../STAR/Star.h"
#include "../../ENTITY/Entity.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../ENTITY/COMPONENT/RigidDynamicComponent.h"

static Entity* ecs = Entity::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

physx::PxShape* SphereColliderComponent::GetShape() const
{
	return pxShape;
}
void SphereColliderComponent::SetCenter(Vector3 value)
{
	if (pxShape) pxShape->setLocalPose(physx::PxTransform(value.x, value.y, value.z));
}
Vector3 SphereColliderComponent::GetCenter() const
{
	physx::PxTransform pxTransform;
	if (pxShape) pxTransform = pxShape->getLocalPose();
	return Vector3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
}
void SphereColliderComponent::SetExtent(float value)
{
	lastExtent = value;
}
float SphereColliderComponent::GetExtent()
{
	return lastExtent;
}
void SphereColliderComponent::CreateShape(float _Radius)
{
	if (!ecs->HasComponent<PhysicsMaterialComponent>(entity))
	{
		ecs->AddComponent<PhysicsMaterialComponent>(entity);
		ecs->GetComponent<PhysicsMaterialComponent>(entity).CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
	}
	auto& physicsMaterialComponent = ecs->GetComponent<PhysicsMaterialComponent>(entity);

	pxShape = physicsSystem->GetPhysics()->createShape(physx::PxSphereGeometry(
		Star::FloatToPhysics(_Radius)),
		*physicsMaterialComponent.GetMaterial(),
		true);

	if (!pxShape)
		Star::AddLog("[PhysX] -> Failed to create sphere shape!");
}
void SphereColliderComponent::Update()
{
	if (pxShape)
	{
		physx::PxSphereGeometry geometry;
		geometry.radius = Star::FloatToPhysics(lastExtent + lastRadius - 1.0f);
		pxShape->setGeometry(geometry);
		//printf("update()\n");
	}
}

void SphereColliderComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "SphereCollider" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Active" << YAML::Value << activeComponent;
	out << YAML::Key << "Center"; Star::SerializeVector3(out, GetCenter());
	out << YAML::Key << "Radius" << YAML::Value << GetRadius();
	out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
	//out << YAML::Key << "StaticFriction" << YAML::Value << physicsMaterialComponent.GetStaticFriction();
	//out << YAML::Key << "DynamicFriction" << YAML::Value << physicsMaterialComponent.GetDynamicFriction();
	//out << YAML::Key << "Restitution" << YAML::Value << physicsMaterialComponent.GetRestitution();
	out << YAML::EndMap;
	out << YAML::EndMap;
	out << YAML::EndMap;
}
void SphereColliderComponent::DeserializeComponent(YAML::Node& in)
{
	activeComponent = in["Active"].as<bool>();
	auto center = in["Center"];
	SetCenter(Star::DeserializeVector3(center));
	SetRadius(in["Radius"].as<float>());
	auto material = in["Material"];
	//physicsMaterialComponent.SetStaticFriction(material["StaticFriction"].as<float>());
	//physicsMaterialComponent.SetDynamicFriction(material["DynamicFriction"].as<float>());
	//physicsMaterialComponent.SetRestitution(material["Restitution"].as<float>());
}
void SphereColliderComponent::SetRadius(float value)
{
	lastRadius = value;
}
float SphereColliderComponent::GetRadius()
{
	return lastRadius;
}
void SphereColliderComponent::Render(std::vector<SphereColliderComponent>* bcc, size_t index)
{
	ImGui::Checkbox("##SPHERECOLLIDER", &activeComponent);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("SPHERE COLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				if (ecs->registry.any_of<RigidDynamicComponent>(ecs->selected))
					ecs->registry.get<RigidDynamicComponent>(ecs->selected).GetRigidDynamic()->detachShape(*GetShape());
				Release();
				bcc->erase(bcc->begin() + index);
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
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Center");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Radius");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				Vector3 _Center = GetCenter();
				if (ImGui::DragFloat3("##CenterPhysicsComponent", (float*)&_Center, 0.1f))
					SetCenter(_Center);

				float _Radius = GetRadius();
				if (ImGui::DragFloat("##Radius", &_Radius, 0.1f, 0.0f, FLT_MAX))
				{
					SetRadius(_Radius);
					Update();
				}
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}
void SphereColliderComponent::Release()
{
	if (GetShape())
		GetShape()->release();
}