#include "Viewport.h"

#include "../../ENTITY/Entity.h"
#include "../../STAR/Star.h"
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "../../ENTITY/COMPONENT/CameraComponent.h"
#include "../../ENTITY/COMPONENT/RigidBodyComponent.h"
#include "../../GAME/Game.h"
#include "../../SYSTEM/PhysicsSystem.h"
#include "Assets.h"
#include "../../WINDOW/MainWindow.h"
#include "../../USERINPUT/UserInput.h"
#include "../../STORAGE/TextureStorage.h"
#include "../../STRDX/Widgets.h"
#include "../../ENGINE/Engine.h"
#include <imoguizmo/imoguizmo.hpp>

static DX*             dx = DX::GetSingleton();
static Game*           game = Game::GetSingleton();
static Entity*         ecs = Entity::GetSingleton();
static AssetsWindow*   assetsWindow = AssetsWindow::GetSingleton();
static MainWindow*     mainWindow = MainWindow::GetSingleton();
static UserInput*      userInput = UserInput::GetSingleton();
static TextureStorage* textureStorage = TextureStorage::GetSingleton();
static Widgets*        widgets = Widgets::GetSingleton();
static Engine*         engine = Engine::GetSingleton();

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
					if (!engine->close) // this is good, because if windows is closed, then ResizeRenderTarget() is still called
						widgets->ResizeRenderTarget(Vector2(windowSizeAvail.x, windowSizeAvail.y), engine->multisamplingCount);
				}
			}
		}

		ImGui::Image((void*)s_ShaderResourceView, windowSizeAvail);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
			{
				FILEs* payload_n = (FILEs*)payload->Data;
				std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n->file_name;
				std::string exe = Star::GetParent(Star::GetExecutablePath());
				std::string x = Star::GetRelativePath(buffer, exe);
				entt::entity entity = RunRay(false);
				if (ecs->IsValid(entity))
				{
					if (Star::ImageFormatCheck(payload_n->file_type.c_str()))
					{
						if (ecs->HasComponent<MeshComponent>(entity))
						{
							auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);
							if (meshComponent.meshStorageBuffer)
							{
								textureStorage->LoadTexture(x.c_str(), &meshComponent.meshStorageBuffer->material.diffuseTexture);
								meshComponent.meshStorageBuffer->material.diffuse = x;

								if (ecs->HasComponent<GeneralComponent>(entity))
								{
									auto& generalComponent = ecs->GetComponent<GeneralComponent>(entity);
									if (meshComponent.meshStorageBuffer->material.name.empty())
										meshComponent.meshStorageBuffer->material.name = generalComponent.GetName();
								}
							}
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
	textureDesc.Format = dx->dxFormat;
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
	if (s_RenderTargetView)
	{
		s_RenderTargetView->Release();
		s_RenderTargetView = nullptr;
	}
	if (s_DepthStencilView)
	{
		s_DepthStencilView->Release();
		s_DepthStencilView = nullptr;
	}
	if (s_ShaderResourceView)
	{
		s_ShaderResourceView->Release();
		s_ShaderResourceView = nullptr;
	}
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
		if (!ImGuizmo::IsUsing() && !ImOGuizmo::CheckInside())
			RunRay(true);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		RECT clip = mainWindow->GetClientRect();

		clip.left = (long)clip.left + (long)(windowPos.x + windowSize.x / 2);
		clip.top = (long)clip.top + (long)(windowPos.y + windowSize.y / 2);
		clip.right = (long)clip.left + (long)(windowSize.x - windowSize.x / 2) - (long)(windowSize.x / 2);
		clip.bottom = (long)clip.top + (long)(windowSize.y - windowSize.y / 2) - (long)(windowSize.y / 2);

		ImGuizmo::Enable(false);
		GetCursorPos(&cursorPosition);
		Star::ShowCursor(false);
		ClipCursor(&clip);
		userInput->GetMouse(); // idk?
		off = true;
	}
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		off = false;
		ClipCursor(nullptr);
		Star::ShowCursor(true);
		SetCursorPos(cursorPosition.x, cursorPosition.y);
		ImGuizmo::Enable(true);
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && cursorOnWindow)
	{
		float delta = ImGui::GetIO().DeltaTime;
		float speed = (userInput->GetKey(DIK_LSHIFT) ? camBoostSpeed : camSpeed) * delta;

		if (userInput->GetKey(DIK_S))
		{
			Vector3 forward = Vector3::Transform(Vector3::Forward, rot);
			forward.Normalize();
			sPos += forward * speed;
		}
		if (userInput->GetKey(DIK_W))
		{
			Vector3 backward = Vector3::Transform(Vector3::Backward, rot);
			backward.Normalize();
			sPos += backward * speed;
		}
		if (userInput->GetKey(DIK_A))
		{
			Vector3 left = Vector3::Transform(Vector3::Left, rot);
			left.y = 0.0f;
			left.Normalize();
			sPos += left * speed;
		}
		if (userInput->GetKey(DIK_D))
		{
			Vector3 right = Vector3::Transform(Vector3::Right, rot);
			right.y = 0.0f;
			right.Normalize();
			sPos += right * speed;
		}

		Vector2 current = userInput->GetMouse();
		float yaw = current.x * camSensitivity;
		float pitch = current.y * camSensitivity;
		Quaternion deltaRotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
		sRot = Quaternion::Concatenate(sRot, deltaRotation);
	}

	{
		pos = Vector3::Lerp(pos, sPos, posLerp * ImGui::GetIO().DeltaTime);
		rot = Quaternion::Lerp(rot, sRot, rotLerp * ImGui::GetIO().DeltaTime);
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

	static const float space = 16.0f;
	static const float offset = 0.0f;

	ImOGuizmo::SetDrawList();
	ImOGuizmo::SetRect(windowPos.x + windowSize.x - 64.0f - space, (windowPos.y + windowCursorPos.y - 40.0f + space) + 64.0f, 40.0f);
	ImOGuizmo::BeginFrame();
	Matrix backup = view; // call before DrawGizmo()
	int coord = ImOGuizmo::DrawGizmo((float*)&view, (float*)&projection, 1.0f);
	GuizmoSystem(coord, offset, backup);
}

void ViewportWindow::Init()
{
	// config
	ImOGuizmo::config.lineThicknessScale = 0.1f;
	ImOGuizmo::config.positiveRadiusScale = 0.15f;
	ImOGuizmo::config.negativeRadiusScale = 0.15f;
	ImOGuizmo::config.hoverCircleRadiusScale = 2.1f;

	// front
	ImOGuizmo::config.xCircleFrontColor = IM_COL32(0xE2, 0x52, 0x52, 0xFF);
	ImOGuizmo::config.yCircleFrontColor = IM_COL32(0x52, 0xE2, 0x52, 0xFF);
	ImOGuizmo::config.zCircleFrontColor = IM_COL32(0x52, 0x52, 0xE2, 0xFF);

	// back
	ImOGuizmo::config.xCircleBackColor = IM_COL32(0xE2, 0x52, 0x52, 0xFF);
	ImOGuizmo::config.yCircleBackColor = IM_COL32(0x52, 0xE2, 0x52, 0xFF);
	ImOGuizmo::config.zCircleBackColor = IM_COL32(0x52, 0x52, 0xE2, 0xFF);
}

void ViewportWindow::GuizmoSystem(int _Value, float _Offset, Matrix _Backup)
{
	if (_Value == -1)
		return;

	if (ecs->selected != entt::null)
	{
		if (ecs->HasComponent<TransformComponent>(ecs->selected))
		{
			auto& transformComponent = ecs->GetComponent<TransformComponent>(ecs->selected);

			// Get the bounding box
			DirectX::BoundingBox boundingBox = transformComponent.GetBoundingBox();

			// Calculate bounding box radius
			Vector3 extents = boundingBox.Extents;
			float radius = std::sqrt(extents.x * extents.x + extents.y * extents.y + extents.z * extents.z);

			// Extract transform components
			Vector3 targetPos;
			Quaternion rot;
			Vector3 scale;
			transformComponent.GetTransform().Decompose(scale, rot, targetPos);

			// Calculate the camera position
			float distanceFactor = 2.5f; // Adjust this to fit the object comfortably
			Vector3 cameraPos;

			if (_Value == 0) // X
				cameraPos = targetPos + Vector3(radius * distanceFactor + _Offset, 0.0f, 0.0f);
			else if (_Value == 1) // Y
				cameraPos = targetPos + Vector3(0.0f, radius * distanceFactor + _Offset, 0.0f);
			else if (_Value == 2) // Z
				cameraPos = targetPos + Vector3(0.0f, 0.0f, radius * distanceFactor + _Offset);
			else if (_Value == 3) // -X
				cameraPos = targetPos + Vector3(-radius * distanceFactor + _Offset, 0.0f, 0.0f);
			else if (_Value == 4) // -Y
				cameraPos = targetPos + Vector3(0.0f, -radius * distanceFactor + _Offset, 0.0f);
			else if (_Value == 5) // -Z
				cameraPos = targetPos + Vector3(0.0f, 0.0f, -radius * distanceFactor + _Offset);

			// Set the adjusted camera position
			sPos = cameraPos;

			// Calculate the look-at direction
			Vector3 lookAtDirection = (targetPos - cameraPos);
			lookAtDirection.Normalize();

			// Calculate the camera right and up vectors
			Vector3 upDirection = Vector3::Up;
			Vector3 rightDirection = upDirection.Cross(lookAtDirection);
			rightDirection.Normalize();
			upDirection.Cross(lookAtDirection, rightDirection);
			upDirection.Normalize();

			// Create rotation quaternion
			Matrix lookAtMatrix = Matrix::CreateLookAt(cameraPos, targetPos, upDirection);
			Quaternion lookAtQuat = Quaternion::CreateFromRotationMatrix(lookAtMatrix);

			if (_Value == 2 || _Value == 5)
			{
				Quaternion reverseYRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, DirectX::XM_PI);
				lookAtQuat = reverseYRotation * lookAtQuat;
			}

			// Set the camera rotation
			sRot = lookAtQuat;
		}
	}
	else
	{
		view = _Backup;
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
	ScreenToClient(mainWindow->GetHandle(), &mousePos);

	float mousex = mousePos.x - windowPos.x;
	float mousey = mousePos.y - (windowPos.y + windowCursorPos.y);

	float tempDist;
	float closestDist = FLT_MAX;

	XMVECTOR prwsPos, prwsDir;
	RayVector(mousex, mousey, prwsPos, prwsDir);

	auto view = ecs->registry.view<MeshComponent>();
	for (auto entity : view)
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		auto& meshComponent = ecs->GetComponent<MeshComponent>(entity);
		tempDist = PickMesh(prwsPos, prwsDir, meshComponent.meshStorageBuffer->vertices, meshComponent.meshStorageBuffer->indices, transformComponent.GetTransform());
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

		entt::entity entity = entt::to_entity(ecs->registry, transformComponent);
		if (ecs->IsValid(entity))
		{
			if (ecs->HasComponent<RigidbodyComponent>(entity))
			{
				auto& rigidBodyComponent = ecs->GetComponent<RigidbodyComponent>(entity);
				rigidBodyComponent.SetTransform(transformComponent.GetTransform());
			}
		}
	}
}

ID3D11ShaderResourceView* ViewportWindow::GetShaderResourceView()
{
	return s_ShaderResourceView;
}

void ViewportWindow::LookAt(Vector3 targetPos, DirectX::BoundingBox boundingBox, float distanceFactor, float offset)
{
	// Calculate bounding box radius
    Vector3 extents = boundingBox.Extents;
    float radius = std::sqrt(extents.x * extents.x + extents.y * extents.y + extents.z * extents.z);

    // Calculate forward vector
    Vector3 forward = (targetPos - sPos);
    forward.Normalize();

    // Calculate a suitable camera position
    float distance = radius * distanceFactor + offset;
    Vector3 cameraPos = targetPos - (forward * distance);

    // Apply the calculated position
    sPos = cameraPos;
	sPos.y = targetPos.y;

	Vector3 forward2 = (targetPos - sPos);
	forward.Normalize();

    // Orient the camera to look at the target position
    sRot = Quaternion::LookRotation(-forward2, Vector3::Up);
}
void ViewportWindow::LookAtEntity(entt::entity entity)
{
	if (!ecs->IsValid(entity)) return;
	if (!ecs->HasComponent<TransformComponent>(entity)) return;

	auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
	LookAt(transformComponent.GetPosition(), transformComponent.GetBoundingBox(), 2.5f, 0.0f);

	// because why not
	LookAt(transformComponent.GetPosition(), transformComponent.GetBoundingBox(), 2.5f, 0.0f);
}
void ViewportWindow::SetDefaultCam()
{
	pos = Vector3(0, 0, -5);
	sPos = Vector3(0, 0, -5);
	rot = Quaternion::Identity;
	sRot = Quaternion::Identity;
}

float ViewportWindow::GetNear()
{
	return camNearZ;
}
float ViewportWindow::GetFar()
{
	return camFarZ;
}
float ViewportWindow::GetFov()
{
	return camFov;
}
float ViewportWindow::GetSpeed()
{
	return camSpeed;
}
float ViewportWindow::GetBoostSpeed()
{
	return camBoostSpeed;
}
void ViewportWindow::SetNear(float _Near)
{
	camNearZ = _Near;
	SetPerspectiveProjectionMatrix(windowSizeAvail, camFov, camNearZ, camFarZ);
}
void ViewportWindow::SetFar(float _Far)
{
	camFarZ = _Far;
	SetPerspectiveProjectionMatrix(windowSizeAvail, camFov, camNearZ, camFarZ);
}
void ViewportWindow::SetFov(float _Fov)
{
	camFov = _Fov;
	SetPerspectiveProjectionMatrix(windowSizeAvail, camFov, camNearZ, camFarZ);
}
void ViewportWindow::SetSpeed(float _Speed)
{
	camSpeed = _Speed;
}
void ViewportWindow::SetBoostSpeed(float _BoostSpeed)
{
	camBoostSpeed = _BoostSpeed;
}
float ViewportWindow::GetPosLerp()
{
	return posLerp;
}
float ViewportWindow::GetRotLerp()
{
	return rotLerp;
}
void ViewportWindow::SetPosLerp(float _Value)
{
	posLerp = _Value;
}
void ViewportWindow::SetRotLerp(float _Value)
{
	rotLerp = _Value;
}
void ViewportWindow::SetRenderState(RenderState _RenderState)
{
	renderState = _RenderState;

	if (renderState == RenderState::Triangle)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (renderState == RenderState::Line)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if (renderState == RenderState::Point)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
	else if (renderState == RenderState::Position)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (renderState == RenderState::Normal)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}
RenderState ViewportWindow::GetRenderState()
{
	return renderState;
}
void ViewportWindow::RefreshRenderState()
{
	if (renderState == RenderState::Triangle)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (renderState == RenderState::Line)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if (renderState == RenderState::Point)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
	else if (renderState == RenderState::Position)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (renderState == RenderState::Normal)
	{
		dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}