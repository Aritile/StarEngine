#include "BoxColliderComponent.h"
#include "../../STAR/Star.h"
#include "../../ENTITY/Entity.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../ENTITY/COMPONENT/RigidDynamicComponent.h"

static Entity* ecs = Entity::GetSingleton();
static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

physx::PxShape* BoxColliderComponent::GetShape() const
{
	return pxShape;
}
void BoxColliderComponent::SetCenter(Vector3 value)
{
	if (pxShape) pxShape->setLocalPose(physx::PxTransform(value.x, value.y, value.z));
}
Vector3 BoxColliderComponent::GetCenter() const
{
	physx::PxTransform pxTransform;
	if (pxShape) pxTransform = pxShape->getLocalPose();
	return Vector3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
}
void BoxColliderComponent::SetExtents(Vector3 value)
{
	lastExtents = value;
}
Vector3 BoxColliderComponent::GetExtents()
{
	return lastExtents;
}
void BoxColliderComponent::CreateShape(Vector3 _Size)
{
	if (!ecs->HasComponent<PhysicsMaterialComponent>(entity))
	{
		ecs->AddComponent<PhysicsMaterialComponent>(entity);
		ecs->GetComponent<PhysicsMaterialComponent>(entity).CreateMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
	}
	auto& physicsMaterialComponent = ecs->GetComponent<PhysicsMaterialComponent>(entity);

	pxShape = physicsSystem->GetPhysics()->createShape(physx::PxBoxGeometry(
		Star::Vector3ToPhysics(_Size)),
		*physicsMaterialComponent.GetMaterial(),
		true);

	if (!pxShape)
		Star::AddLog("[PhysX] -> Failed to create box shape!");
}
void BoxColliderComponent::Update()
{
	if (pxShape)
	{
		physx::PxBoxGeometry geometry;
		geometry.halfExtents = Star::Vector3ToPhysics(lastExtents + lastSize - Vector3(1, 1, 1));
		pxShape->setGeometry(geometry);
		//printf("update()\n");
	}
}

void BoxColliderComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "BoxCollider" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Active" << YAML::Value << activeComponent;
	out << YAML::Key << "Center"; Star::SerializeVector3(out, GetCenter());
	out << YAML::Key << "Size"; Star::SerializeVector3(out, GetSize());
	out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
	//out << YAML::Key << "StaticFriction" << YAML::Value << physicsMaterialComponent.GetStaticFriction();
	//out << YAML::Key << "DynamicFriction" << YAML::Value << physicsMaterialComponent.GetDynamicFriction();
	//out << YAML::Key << "Restitution" << YAML::Value << physicsMaterialComponent.GetRestitution();
	out << YAML::EndMap;
	out << YAML::EndMap;
	out << YAML::EndMap;
}
void BoxColliderComponent::DeserializeComponent(YAML::Node& in)
{
	activeComponent = in["Active"].as<bool>();
	auto center = in["Center"];
	SetCenter(Star::DeserializeVector3(center));
	auto size = in["Size"];
	SetSize(Star::DeserializeVector3(size));
	auto material = in["Material"];
	//physicsMaterialComponent.SetStaticFriction(material["StaticFriction"].as<float>());
	//physicsMaterialComponent.SetDynamicFriction(material["DynamicFriction"].as<float>());
	//physicsMaterialComponent.SetRestitution(material["Restitution"].as<float>());
}
void BoxColliderComponent::SetSize(Vector3 value)
{
	lastSize = value;
}
Vector3 BoxColliderComponent::GetSize()
{
	return lastSize;
}
void BoxColliderComponent::Render(std::vector<BoxColliderComponent>* bcc, size_t index)
{
	ImGui::Checkbox("##BOXCOLLIDER", &activeComponent);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("BOX COLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
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
				ImGui::Text("Size");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				Vector3 _Center = GetCenter();
				if (ImGui::DragFloat3("##CenterPhysicsComponent", (float*)&_Center, 0.1f))
					SetCenter(_Center);

				Vector3 _Size = GetSize();
				if (ImGui::DragFloat3("##Size", (float*)&_Size, 0.1f, 0.0f, FLT_MAX))
				{
					SetSize(_Size);
					Update();
				}
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}
void BoxColliderComponent::Release()
{
	if (GetShape())
		GetShape()->release();
}