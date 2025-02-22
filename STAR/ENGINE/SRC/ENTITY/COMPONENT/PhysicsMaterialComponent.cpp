#include "PhysicsMaterialComponent.h"
#include "../../STAR/Star.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../IMGUI/imgui.h"

static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

void PhysicsMaterialComponent::CreateMaterial(float _StaticFriction, float _DynamicFriction, float _Restitution)
{
	pxMaterial = physicsSystem->GetPhysics()->createMaterial(
		_StaticFriction,
		_DynamicFriction,
		_Restitution);

	if (!pxMaterial)
		Star::AddLog("[PhysX] -> Failed to create material!");
}
void PhysicsMaterialComponent::Release()
{
	if (pxMaterial)
		pxMaterial->release();
}
/*
void PhysicsMaterialComponent::RenderLeft(bool begin)
{
	if (!pxMaterial)
		return;

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + (begin ? 0 : 4));
	ImGui::Text("Static Friction");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
	ImGui::Text("Dynamic Friction");
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
	ImGui::Text("Restitution");
}
*/
/*
void PhysicsMaterialComponent::RenderRight()
{
	if (!pxMaterial)
		return;

	float _StaticFriction = GetStaticFriction();
	if (ImGui::DragFloat("##StaticFrictionPhysicsComponent", &_StaticFriction, 0.1f, 0.0f, FLT_MAX))
		SetStaticFriction(_StaticFriction);

	float _DynamicFriction = GetDynamicFriction();
	if (ImGui::DragFloat("##DynamicFrictionPhysicsComponent", &_DynamicFriction, 0.1f, 0.0f, FLT_MAX))
		SetDynamicFriction(_DynamicFriction);


	float _Restitution = GetRestitution();
	if (ImGui::DragFloat("##RestitutionPhysicsComponent", &_Restitution, 0.1f, 0.0f, FLT_MAX))
		SetRestitution(_Restitution);
}
*/
void PhysicsMaterialComponent::SetStaticFriction(float _Value)
{
	if (_Value < 0.0f) return;
	if (pxMaterial) pxMaterial->setStaticFriction(_Value);
}
float PhysicsMaterialComponent::GetStaticFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getStaticFriction();
}
void PhysicsMaterialComponent::SetDynamicFriction(float _Value)
{
	if (_Value < 0.0f) return;
	if (pxMaterial) pxMaterial->setDynamicFriction(_Value);
}
float PhysicsMaterialComponent::GetDynamicFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getDynamicFriction();
}
void PhysicsMaterialComponent::SetRestitution(float _Value)
{
	if (_Value < 0.0f) return;
	if (pxMaterial) pxMaterial->setRestitution(_Value);
}
float PhysicsMaterialComponent::GetRestitution()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getRestitution();
}
physx::PxMaterial* PhysicsMaterialComponent::GetMaterial()
{
	return pxMaterial;
}
void PhysicsMaterialComponent::Render()
{
	if (ImGui::CollapsingHeader("PHYSICS MATERIAL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				pxMaterial->release();
				pxMaterial = nullptr;
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("PhysicsComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Static Friction");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Dynamic Friction");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Restitution");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				float _StaticFriction = GetStaticFriction();
				if (ImGui::DragFloat("##StaticFrictionPhysicsComponent", &_StaticFriction, 0.1f, 0.0f, FLT_MAX))
					SetStaticFriction(_StaticFriction);

				float _DynamicFriction = GetDynamicFriction();
				if (ImGui::DragFloat("##DynamicFrictionPhysicsComponent", &_DynamicFriction, 0.1f, 0.0f, FLT_MAX))
					SetDynamicFriction(_DynamicFriction);


				float _Restitution = GetRestitution();
				if (ImGui::DragFloat("##RestitutionPhysicsComponent", &_Restitution, 0.1f, 0.0f, FLT_MAX))
					SetRestitution(_Restitution);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}