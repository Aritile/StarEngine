#include "Viewport.h"

#include "../../ENTITY/Entity.h"
#include "../../HELPERS/Helpers.h"
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "../../ENTITY/COMPONENT/CameraComponent.h"
#include "../../ENTITY/COMPONENT/RigidBodyComponent.h"
#include "../../GAME/Game.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "Assets.h"

static DX* dx = DX::GetSingleton();
static Game* game = Game::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static AssetsWindow* assetsWindow = AssetsWindow::GetSingleton();

ViewportWindow* ViewportWindow::GetSingleton()
{
	static ViewportWindow viewportWindow;
	return &viewportWindow;
}

void ViewportWindow::Render()
{
	if (!renderViewport)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(NULL, NULL));
	ImGui::Begin("Viewport");
	{
		windowSizeAvail = ImGui::GetContentRegionAvail();
		cursorOnWindow = ImGui::IsWindowHovered();
		windowPos = ImGui::GetWindowPos();
		windowSize = ImGui::GetWindowSize();
		windowCursorPos = ImGui::GetCursorPos();

		if (!ImGui::IsWindowCollapsed())
		{
			if (IsBufferResized())
			{
				if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					SetPerspectiveProjectionMatrix(windowSizeAvail, camFov, camNearZ, camFarZ);
					CreateBuffer(windowSizeAvail);
				}
			}
		}

		ImGui::Image((void*)s_ShaderResourceView, windowSizeAvail);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
			{
				FILEs payload_n = *(FILEs*)payload->Data;
				std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
				entt::entity entity = RunRay(false);
				if (ecs->IsValid(entity))
				{
					if (payload_n.file_type == PNG || payload_n.file_type == JPEG || payload_n.file_type == DDS)
					{
						if (ecs->HasComponent<MeshComponent>(entity))
						{
							auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);
							meshComponent.LoadDiffuseTexture(buffer.c_str());
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		RenderWidget();
	}
	ImGui::End();
	ImGui::PopStyleVar(1);
}

D3D11_VIEWPORT ViewportWindow::GetViewport()
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = (float)bufferSize.x;
	viewport.Height = (float)bufferSize.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	return viewport;
}

bool ViewportWindow::CreateBuffer(ImVec2 resolution)
{
	if (resolution.x <= MIN_XY && resolution.y <= MIN_XY)
		return false;

	///////////////////////////////////////////////////////////

	ReleaseBuffer();

	///////////////////////////////////////////////////////////

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	textureDesc.Width = (UINT)resolution.x;
	textureDesc.Height = (UINT)resolution.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	///////////////////////////////////////////////////////////

	ID3D11Texture2D* tex = nullptr;
	if (FAILED(dx->dxDevice->CreateTexture2D(&textureDesc, NULL, &tex)))
		return false;

	///////////////////////////////////////////////////////////

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(dx->dxDevice->CreateRenderTargetView(tex, &renderTargetViewDesc, &s_RenderTargetView)))
		return false;


	///////////////////////////////////////////////////////////

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(dx->dxDevice->CreateShaderResourceView(tex, &shaderResourceViewDesc, &s_ShaderResourceView)))
		return false;

	///////////////////////////////////////////////////////////

	tex->Release();

	///////////////////////////////////////////////////////////

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));

	descDepth.Width = (UINT)resolution.x;
	descDepth.Height = (UINT)resolution.y;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	if (FAILED(dx->dxDevice->CreateTexture2D(&descDepth, NULL, &tex)))
		return false;


	///////////////////////////////////////////////////////////

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if (FAILED(dx->dxDevice->CreateDepthStencilView(tex, &descDSV, &s_DepthStencilView)))
		return false;

	///////////////////////////////////////////////////////////

	tex->Release();
	bufferSize = resolution;
	return true;
}

void ViewportWindow::ReleaseBuffer()
{
	if (s_RenderTargetView) s_RenderTargetView->Release();
	if (s_DepthStencilView) s_DepthStencilView->Release();
	if (s_ShaderResourceView) s_ShaderResourceView->Release();
}

bool ViewportWindow::IsBufferResized()
{
	if (bufferSize.x != windowSizeAvail.x || bufferSize.y != windowSizeAvail.y)
		return true;
	else
		return false;
}

Matrix ViewportWindow::GetPerspectiveProjectionMatrix()
{
	return projection;
}
Matrix ViewportWindow::GetPerspectiveViewMatrix()
{
	return view;
}

void ViewportWindow::SetPerspectiveProjectionMatrix(ImVec2 size, float fov, float NearZ, float FarZ)
{
	float aspectRatio = (float)size.x / (float)size.y;
	projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, NearZ, FarZ);
}

void ViewportWindow::UpdateMovement()
{
	using namespace DirectX;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && cursorOnWindow)
		if (!ImGuizmo::IsUsing())
			RunRay(true);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		RECT clip = StarHelpers::GetClientRect();

		clip.left = (long)clip.left + (long)(windowPos.x + windowSize.x / 2);
		clip.top = (long)clip.top + (long)(windowPos.y + windowSize.y / 2);
		clip.right = (long)clip.left + (long)(windowSize.x - windowSize.x / 2) - (long)(windowSize.x / 2);
		clip.bottom = (long)clip.top + (long)(windowSize.y - windowSize.y / 2) - (long)(windowSize.y / 2);

		ImGuizmo::Enable(false);
		GetCursorPos(&cursorPosition);
		ShowCursor(false);
		ClipCursor(&clip);
		StarHelpers::InputGetMouse();
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		ClipCursor(nullptr);
		ShowCursor(true);
		SetCursorPos(cursorPosition.x, cursorPosition.y);
		ImGuizmo::Enable(true);
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		float delta = ImGui::GetIO().DeltaTime;
		float speed = (StarHelpers::InputGetKey(DIK_LSHIFT) ? camBoostSpeed : camSpeed) * delta;

		if (StarHelpers::InputGetKey(DIK_S))
		{
			Vector3 forward = Vector3::Transform(Vector3::Forward, rot);
			forward.Normalize();
			sPos += forward * speed;
		}
		if (StarHelpers::InputGetKey(DIK_W))
		{
			Vector3 backward = Vector3::Transform(Vector3::Backward, rot);
			backward.Normalize();
			sPos += backward * speed;
		}
		if (StarHelpers::InputGetKey(DIK_A))
		{
			Vector3 left = Vector3::Transform(Vector3::Left, rot);
			left.y = 0.0f;
			left.Normalize();
			sPos += left * speed;
		}
		if (StarHelpers::InputGetKey(DIK_D))
		{
			Vector3 right = Vector3::Transform(Vector3::Right, rot);
			right.y = 0.0f;
			right.Normalize();
			sPos += right * speed;
		}

		Vector2 current = StarHelpers::InputGetMouse();
		float yaw = current.x * camSensitivity;
		float pitch = current.y * camSensitivity;
		Quaternion deltaRotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
		sRot = Quaternion::Concatenate(sRot, deltaRotation);
	}

	{
		pos = Vector3::Lerp(pos, sPos, 8.0f * ImGui::GetIO().DeltaTime);
		rot = Quaternion::Lerp(rot, sRot, 16.0f * ImGui::GetIO().DeltaTime);
	}

	Matrix matrix = Matrix::Identity;
	matrix = Matrix::CreateTranslation(pos) * matrix;
	matrix = Matrix::CreateFromQuaternion(rot) * matrix;
	view = XMMatrixLookAtLH(matrix.Translation(), matrix.Translation() + matrix.Backward(), Vector3::Up);
}

void ViewportWindow::RenderWidget()
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(windowPos.x, (windowPos.y + windowCursorPos.y), windowSize.x, (windowSize.y - windowCursorPos.y));

	if (ecs->selected != entt::null)
	{
		if (ecs->HasComponent<GeneralComponent>(ecs->selected))
		{
			auto& generalComponent = ecs->GetComponent<GeneralComponent>(ecs->selected);
			if (generalComponent.IsActive())
			{
				if (ecs->HasComponent<TransformComponent>(ecs->selected))
				{
					auto& transformComponent = ecs->GetComponent<TransformComponent>(ecs->selected);
					RenderManipulateWidget(transformComponent);
				}
			}
		}
	}
}

void ViewportWindow::SetOperation(ImGuizmo::OPERATION arg)
{
	operation = arg;
}
void ViewportWindow::SetMode(ImGuizmo::MODE arg)
{
	mode = arg;
}
ImGuizmo::OPERATION ViewportWindow::GetOperation()
{
	return operation;
}
ImGuizmo::MODE ViewportWindow::GetMode()
{
	return mode;
}
ImVec2 ViewportWindow::GetBufferSize()
{
	return windowSizeAvail;
}

void ViewportWindow::RayVector(float mouseX, float mouseY, DirectX::XMVECTOR& pickRayInWorldSpacePos, DirectX::XMVECTOR& pickRayInWorldSpaceDir)
{
	using namespace DirectX;

	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;

	Matrix m_projection = projection;
	PRVecX = (((2.0f * mouseX) / windowSize.x) - 1) / m_projection(0, 0);
	PRVecY = -(((2.0f * mouseY) / (windowSize.y - windowCursorPos.y)) - 1) / m_projection(1, 1);
	PRVecZ = 1.0f;

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	XMMATRIX pickRayToWorldSpaceMatrix;
	XMVECTOR matInvDeter;

	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, view);

	pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
}
bool ViewportWindow::PointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point)
{
	using namespace DirectX;

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}
float ViewportWindow::PickMesh(DirectX::XMVECTOR pickRayInWorldSpacePos, DirectX::XMVECTOR pickRayInWorldSpaceDir, const std::vector<Vertex>& vertPosArray, const std::vector<UINT>& indexPosArray, DirectX::XMMATRIX worldSpace)
{
	using namespace DirectX;

	for (int i = 0; i < indexPosArray.size() / 3; i++)
	{
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		XMFLOAT3 tV1, tV2, tV3;

		tV1 = vertPosArray[indexPosArray[(i * 3) + 0]].position;
		tV2 = vertPosArray[indexPosArray[(i * 3) + 1]].position;
		tV3 = vertPosArray[indexPosArray[(i * 3) + 2]].position;

		tri1V1 = XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);

		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		faceNormal = XMVector3Cross(U, V);
		faceNormal = XMVector3Normalize(faceNormal);

		XMVECTOR triPoint = tri1V1;

		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * XMVectorGetX(triPoint) - tri1B * XMVectorGetY(triPoint) - tri1C * XMVectorGetZ(triPoint));

		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
		ep2 = (XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

		if (ep2 != 0.0f)
			t = -(ep1 + tri1D) / (ep2);

		if (t > 0.0f)
		{
			planeIntersectX = XMVectorGetX(pickRayInWorldSpacePos) + XMVectorGetX(pickRayInWorldSpaceDir) * t;
			planeIntersectY = XMVectorGetY(pickRayInWorldSpacePos) + XMVectorGetY(pickRayInWorldSpaceDir) * t;
			planeIntersectZ = XMVectorGetZ(pickRayInWorldSpacePos) + XMVectorGetZ(pickRayInWorldSpaceDir) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				return t / 2.0f;
			}
		}
	}

	return FLT_MAX;
}
entt::entity ViewportWindow::RunRay(bool select)
{
	using namespace DirectX;

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(dx->hwnd, &mousePos);

	float mousex = mousePos.x - windowPos.x;
	float mousey = mousePos.y - (windowPos.y + windowCursorPos.y);

	float tempDist;
	float closestDist = FLT_MAX;

	XMVECTOR prwsPos, prwsDir;
	RayVector(mousex, mousey, prwsPos, prwsDir);

	auto view = ecs->registry.view<MeshComponent>();
	for (auto entity : view)
	{
		auto& transComp = ecs->GetComponent<TransformComponent>(entity);
		auto& meshComp = ecs->GetComponent<MeshComponent>(entity);
		tempDist = PickMesh(prwsPos, prwsDir, meshComp.GetVertices(), meshComp.GetIndices(), transComp.GetTransform());
		if (tempDist < closestDist && ecs->GetComponent<GeneralComponent>(entity).IsActive() && ecs->GetComponent<MeshComponent>(entity).IsActive())
		{
			closestDist = tempDist;
			if (select)
				ecs->selected = entity;
			return entity;
			break;
		}
		else
		{
			if (select)
				ecs->selected = entt::null;
		}
	}

	return entt::null;
}

ID3D11RenderTargetView* ViewportWindow::GetRenderTargetView()
{
	return s_RenderTargetView;
}
ID3D11DepthStencilView* ViewportWindow::GetDepthStencilView()
{
	return s_DepthStencilView;
}

void ViewportWindow::RenderManipulateWidget(TransformComponent& transformComponent)
{
	Matrix matrix = transformComponent.GetTransform();
	Matrix deltaMatrix = Matrix::Identity;
	Vector3 snaps = Vector3(snap, snap, snap);
	if (ImGuizmo::Manipulate(
		(float*)&view,
		(float*)&projection,
		operation,
		mode,
		(float*)&matrix,
		(float*)&deltaMatrix,
		useSnap ? (float*)&snaps : NULL,
		NULL,
		NULL))
	{
		matrix *= transformComponent.GetParentTransform().Invert();
		transformComponent.SetTransform(matrix);
	}

	if (ImGuizmo::IsUsing())
	{
		entt::entity entity = entt::to_entity(ecs->registry, transformComponent);
		if (ecs->IsValid(entity))
		{
			if (ecs->HasComponent<RigidBodyComponent>(entity))
			{
				auto& rigidBodyComponent = ecs->GetComponent<RigidBodyComponent>(entity);
				physx::PxTransform trans(StarHelpers::MatrixToPhysics(matrix));
				rigidBodyComponent.SetTransform(trans);
			}
		}
	}
}

ID3D11ShaderResourceView* ViewportWindow::GetShaderResourceView()
{
	return s_ShaderResourceView;
}

void ViewportWindow::LookAt(Vector3 lookAt)
{
	if (pos == lookAt)
		return;

	Vector3 forward = lookAt - pos;
	sRot = Quaternion::LookRotation(-forward, Vector3::Up);
}
void ViewportWindow::LookAtEntity(entt::entity entity)
{
	if (!ecs->IsValid(entity)) return;
	if (!ecs->HasComponent<TransformComponent>(entity)) return;

	auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
	LookAt(transformComponent.GetPosition());

	/*
	DirectX::BoundingBox boundingBox = transformComponent.GetBoundingBox();
	Vector3 center = boundingBox.Center;
	Vector3 extents = boundingBox.Extents;
	*/
}
void ViewportWindow::SetDefaultCam()
{
	pos = Vector3(0, 0, -5);
	sPos = Vector3(0, 0, -5);
}