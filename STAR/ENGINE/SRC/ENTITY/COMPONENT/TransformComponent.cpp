#include "TransformComponent.h"
#include "GeneralComponent.h"
#include "RigidbodyComponent.h"

static Entity* ecs = Entity::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();

/* ------------------------------------------------------------ */

void TransformComponent::Render()
{
	if (ImGui::CollapsingHeader("TRANSFORM", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		/////////////////////////////////////////////////////////////

		if (ImGui::BeginTable("TransformComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Position");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Rotation");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Scale");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				Vector3 position = localTransform.position;
				if (ImGui::DragFloat3("##PositionTransformComponent", (float*)&position, viewportWindow->useSnap ? viewportWindow->snap : 0.1f))
				{
					SetPosition(position);

					entt::entity entity = entt::to_entity(ecs->registry, *this);
					if (ecs->IsValid(entity))
					{
						if (ecs->HasComponent<RigidBodyComponent>(entity))
						{
							auto& rigidBodyComponent = ecs->GetComponent<RigidBodyComponent>(entity);
							physx::PxTransform trans(StarHelpers::Vector3ToPhysics(GetPosition()));
							rigidBodyComponent.SetTransform(trans);
						}
					}
				}

				Vector3 rotation = StarHelpers::ToDegrees(localTransform.rotation.ToEuler());
				if (ImGui::DragFloat3("##RotationTransformComponent", (float*)&rotation, viewportWindow->useSnap ? viewportWindow->snap : 0.1f))
				{
					SetRotationYawPitchRoll(StarHelpers::ToRadians(rotation));

					entt::entity entity = entt::to_entity(ecs->registry, *this);
					if (ecs->IsValid(entity))
					{
						if (ecs->HasComponent<RigidBodyComponent>(entity))
						{
							auto& rigidBodyComponent = ecs->GetComponent<RigidBodyComponent>(entity);
							physx::PxTransform trans(StarHelpers::QuatToPhysics(GetRotationQuaternion()));
							trans.p = StarHelpers::Vector3ToPhysics(GetPosition());
							rigidBodyComponent.SetTransform(trans);
						}
					}
				}

				Vector3 scale = localTransform.scale;
				if (ImGui::DragFloat3("##ScaleTransformComponent", (float*)&scale, viewportWindow->useSnap ? viewportWindow->snap : 0.1f, 0.0f, FLT_MAX))
				{
					SetScale(scale);

					entt::entity entity = entt::to_entity(ecs->registry, *this);
					if (ecs->IsValid(entity))
					{
						// missing code for physics scale
					}
				}
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

/* ------------------------------------------------------------ */

void TransformComponent::SetBoundingBox(DirectX::BoundingBox boundingBox)
{
	localTransform.boundingBox = boundingBox;
}
DirectX::BoundingBox TransformComponent::GetBoundingBox()
{
	localTransform.boundingBox.Transform(globalTransform.boundingBox, GetTransform());
	return globalTransform.boundingBox;
}

/* ------------------------------------------------------------ */

void TransformComponent::SetPosition(Vector3 position)
{
	localTransform.position = position;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}
void TransformComponent::SetRotationYawPitchRoll(Vector3 rotation)
{
	// problem: https://bentleysystems.service-now.com/community?id=kb_article&sysparm_article=KB0054630
	if (StarHelpers::RadToDeg(rotation.x) > 90.0f) rotation.x = StarHelpers::DegToRad(90.0f);
	if (StarHelpers::RadToDeg(rotation.x) < -90.0f) rotation.x = StarHelpers::DegToRad(-90.0f);

	localTransform.rotation = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}
void TransformComponent::SetRotationQuaternion(Quaternion rotation)
{
	localTransform.rotation = rotation;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}
void TransformComponent::SetScale(Vector3 scale)
{
	localTransform.scale = scale;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}
void TransformComponent::SetTransform(Matrix transform)
{
	localTransform.transform = transform;
	UpdatePositionRotationScaleFromTransform(transform);
	isDirty = true;
}

/* ------------------------------------------------------------ */

Vector3 TransformComponent::GetPosition()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.position;
}
Vector3 TransformComponent::GetRotationYawPitchRoll()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.rotation.ToEuler();
}
Quaternion TransformComponent::GetRotationQuaternion()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.rotation;
}
Vector3 TransformComponent::GetScale()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.scale;
}
Matrix TransformComponent::GetTransform()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.transform;
}

/* ------------------------------------------------------------ */

void TransformComponent::UpdateTransformFromPositionRotationScale()
{
	localTransform.transform = Matrix::Identity;
	localTransform.transform = Matrix::CreateTranslation(localTransform.position) * localTransform.transform;
	localTransform.transform = Matrix::CreateFromQuaternion(localTransform.rotation) * localTransform.transform;
	localTransform.transform = Matrix::CreateScale(localTransform.scale) * localTransform.transform;
}
void TransformComponent::UpdatePositionRotationScaleFromTransform(Matrix transform)
{
	transform.Decompose(localTransform.scale, localTransform.rotation, localTransform.position);
}
void TransformComponent::UpdateTransform()
{
	globalTransform.transform = localTransform.transform * parent;
	globalTransform.transform.Decompose(globalTransform.scale, globalTransform.rotation, globalTransform.position);
}

void TransformComponent::SetParentTransform(Matrix matrix)
{
	parent = matrix;
	isDirty = true;
}
Matrix TransformComponent::GetParentTransform()
{
	return parent;
}

/* ------------------------------------------------------------ */

Vector3 TransformComponent::GetLocalPosition()
{
	return localTransform.position;
}
Vector3 TransformComponent::GetLocalRotationYawPitchRoll()
{
	return localTransform.rotation.ToEuler();
}
Quaternion TransformComponent::GetLocalRotationQuaternion()
{
	return localTransform.rotation;
}
Vector3 TransformComponent::GetLocalScale()
{
	return localTransform.scale;
}
Matrix TransformComponent::GetLocalTransform()
{
	return localTransform.transform;
}

void TransformComponent::AddPosition(Vector3 position)
{
	SetPosition(Vector3(
		GetLocalPosition().x + position.x,
		GetLocalPosition().y + position.y,
		GetLocalPosition().z + position.z));
}
void TransformComponent::AddRotationYawPitchRoll(Vector3 rotation)
{
	SetRotationYawPitchRoll(Vector3(
		GetLocalRotationYawPitchRoll().x + rotation.x,
		GetLocalRotationYawPitchRoll().y + rotation.y,
		GetLocalRotationYawPitchRoll().z + rotation.z));
}
void TransformComponent::AddRotationQuaternion(Quaternion rotation)
{
	SetRotationQuaternion(Quaternion::Concatenate(rotation, GetLocalRotationQuaternion()));
}
void TransformComponent::AddScale(Vector3 scale)
{
	SetScale(Vector3(
		GetLocalScale().x + scale.x,
		GetLocalScale().y + scale.y, 
		GetLocalScale().z + scale.z));
}
void TransformComponent::AddTransform(Matrix transform)
{
	SetTransform(GetLocalTransform() * transform);
}

void TransformComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "TransformComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "Position"; StarHelpers::SerializeVector3(out, GetLocalPosition());
		out << YAML::Key << "Rotation"; StarHelpers::SerializeQuaternion(out, GetLocalRotationQuaternion());
		out << YAML::Key << "Scale"; StarHelpers::SerializeVector3(out, GetLocalScale());
		out << YAML::Key << "Matrix"; StarHelpers::SerializeMatrix(out, GetLocalTransform());
	}
	out << YAML::EndMap;
}
void TransformComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node transformComponent = in["TransformComponent"];
	if (transformComponent)
	{
		auto position = transformComponent["Position"];
		SetPosition(StarHelpers::DeserializeVector3(position));
		auto rotation = transformComponent["Rotation"];
		SetRotationQuaternion(StarHelpers::DeserializeQuaternion(rotation));
		auto scale = transformComponent["Scale"];
		SetScale(StarHelpers::DeserializeVector3(scale));
		auto matrix = transformComponent["Matrix"];
		SetTransform(StarHelpers::DeserializeMatrix(matrix));
	}
}

void TransformComponent::LookAt(Matrix matrix)
{
	Matrix lookAt = Matrix::CreateLookAt(GetTransform().Translation(), matrix.Translation(), matrix.Up());
	SetTransform(lookAt);
}

void TransformComponent::LuaAdd(sol::state& state)
{
	sol::usertype<TransformComponent> component = state.new_usertype<TransformComponent>(
		"TransformComponent");
	component["SetBoundingBox"] = &TransformComponent::SetBoundingBox;
	component["GetBoundingBox"] = &TransformComponent::GetBoundingBox;
	component["SetPosition"] = &TransformComponent::SetPosition;
	component["SetRotationYawPitchRoll"] = &TransformComponent::SetRotationYawPitchRoll;
	component["SetRotationQuaternion"] = &TransformComponent::SetRotationQuaternion;
	component["SetScale"] = &TransformComponent::SetScale;
	component["SetTransform"] = &TransformComponent::SetTransform;
	component["AddPosition"] = &TransformComponent::AddPosition;
	component["AddRotationYawPitchRoll"] = &TransformComponent::AddRotationYawPitchRoll;
	component["AddRotationQuaternion"] = &TransformComponent::AddRotationQuaternion;
	component["AddScale"] = &TransformComponent::AddScale;
	component["AddTransform"] = &TransformComponent::AddTransform;
	component["GetPosition"] = &TransformComponent::GetPosition;
	component["GetRotationYawPitchRoll"] = &TransformComponent::GetRotationYawPitchRoll;
	component["GetRotationQuaternion"] = &TransformComponent::GetRotationQuaternion;
	component["GetScale"] = &TransformComponent::GetScale;
	component["GetTransform"] = &TransformComponent::GetTransform;
	component["GetLocalPosition"] = &TransformComponent::GetLocalPosition;
	component["GetLocalRotationYawPitchRoll"] = &TransformComponent::GetLocalRotationYawPitchRoll;
	component["GetLocalRotationQuaternion"] = &TransformComponent::GetLocalRotationQuaternion;
	component["GetLocalScale"] = &TransformComponent::GetLocalScale;
	component["GetLocalTransform"] = &TransformComponent::GetLocalTransform;
	component["LookAt"] = &TransformComponent::LookAt;
}