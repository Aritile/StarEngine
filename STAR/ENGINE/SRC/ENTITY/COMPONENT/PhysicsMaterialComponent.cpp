#include "PhysicsMaterialComponent.h"
#include "../../STAR/Star.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "../../IMGUI/imgui.h"

static PhysicsSystem* physicsSystem = PhysicsSystem::GetSingleton();

void PhysicsMaterialComponent::CreateMaterial(float StaticFriction, float DynamicFriction, float Restitution)
{
	ReleaseMaterial();

	pxMaterial = physicsSystem->GetPhysics()->createMaterial(
		StaticFriction,
		DynamicFriction,
		Restitution);

	if (!pxMaterial)
		Star::AddLog("[PhysX] -> Failed to create material!");
}
void PhysicsMaterialComponent::ReleaseMaterial()
{
	if (pxMaterial)
		pxMaterial->release();
}
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
void PhysicsMaterialComponent::SetStaticFriction(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setStaticFriction(value);
}
float PhysicsMaterialComponent::GetStaticFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getStaticFriction();
}
void PhysicsMaterialComponent::SetDynamicFriction(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setDynamicFriction(value);
}
float PhysicsMaterialComponent::GetDynamicFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getDynamicFriction();
}
void PhysicsMaterialComponent::SetRestitution(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setRestitution(value);
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