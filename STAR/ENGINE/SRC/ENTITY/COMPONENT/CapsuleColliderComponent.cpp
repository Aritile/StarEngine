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
		Star::AddLog("[PhysX] -> Failed to create box shape.");
		return false;
	}
	
	// capsule is reversed, you need reverse of 90 degress
	Quaternion quat;
	quat = Quaternion::CreateFromYawPitchRoll(Vector3(Star::DegToRad(90.0f), 0.0f, 0.0f));
	pxShape->setLocalPose(physx::PxTransform(0.0f, 0.0f, 0.0f, Star::QuatToPhysics(quat)));
	return true;
}

void CapsuleColliderComponent::Render(std::vector<CapsuleColliderComponent>* ccc, size_t index)
{
	ImGui::Checkbox("##CAPSULECOLLIDER", &activeComponent);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("BOXCOLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
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
				ImGui::Text("Radius");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Half Height");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				physicsMaterialComponent.RenderRight();
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

}
void CapsuleColliderComponent::DeserializeComponent(YAML::Node& in)
{

}