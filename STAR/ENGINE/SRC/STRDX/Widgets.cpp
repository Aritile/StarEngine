#include "Widgets.h"

#include "../EDITOR/WINDOW/Viewport.h"
#include "../ENTITY/Entity.h"

// components
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"

// strdx
#include "context.h"

static Context* context = Context::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ViewportWindow* viewportWindow = ViewportWindow::GetSingleton();

Widgets* Widgets::GetSingleton()
{
	static Widgets widgets;
	return &widgets;
}

void Widgets::InitBoundingBoxWidget()
{
	boundingBoxShader = Shader::Create();
	if (boundingBoxShader)
	{
		boundingBoxShader->LoadVertex("data\\shader\\widget\\bounding_box\\vertex.hlsl", true);
		boundingBoxShader->LoadPixel("data\\shader\\widget\\bounding_box\\pixel.hlsl", true);

		boundingBoxShader->CompileVertex();
		boundingBoxShader->CompilePixel();

		boundingBoxShader->CreateVertex();
		boundingBoxShader->CreatePixel();

		boundingBoxShader->AddLayout("POSITION", 0, 3, 0, 0);
		boundingBoxShader->CreateLayout();

		std::vector<POS> vertices;

		vertices.push_back(POS(1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(1.0f, 1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(1.0f, 1.0f, 1.0f));
		vertices.push_back(POS(1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(1.0f, -1.0f, -1.0f));
		vertices.push_back(POS(1.0f, -1.0f, 1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(-1.0f, 1.0f, 1.0f));
		vertices.push_back(POS(1.0f, 1.0f, -1.0f));
		vertices.push_back(POS(1.0f, 1.0f, 1.0f));

		boundingBoxShader->CreateVertexBuffer<POS>(vertices, true);

		boundingBoxShader->ReleaseVertexBlob();
		boundingBoxShader->ReleasePixelBlob();
	}
}
void Widgets::InitGridWidget()
{
	gridShader = Shader::Create();
	if (gridShader)
	{
		gridShader->LoadVertex("data\\shader\\widget\\grid\\vertex.hlsl", true);
		gridShader->LoadPixel("data\\shader\\widget\\grid\\pixel.hlsl", true);

		gridShader->CompileVertex();
		gridShader->CompilePixel();

		gridShader->CreateVertex();
		gridShader->CreatePixel();

		gridShader->AddLayout("POSITION", 0, 3, 0, 0);
		gridShader->AddLayout("COLOR", 0, 4, 0, 12);
		gridShader->CreateLayout();

		std::vector<POSCOL> vertices = BuildGrid();
		gridShader->CreateVertexBuffer<POSCOL>(vertices, true);

		gridShader->ReleaseVertexBlob();
		gridShader->ReleasePixelBlob();
	}
}
void Widgets::RenderGridWidget()
{
	if (gridShader && renderGrid)
	{
		context->Set(gridShader);
		context->SetVertexConstantBuffer(constantBuffer, 0);

		Matrix view = viewportWindow->GetPerspectiveViewMatrix();
		Matrix proj = viewportWindow->GetPerspectiveProjectionMatrix();
		Matrix world = Matrix::Identity;
		world = Matrix::CreateTranslation(gridPos) * world;

		cb.SetProjection(DirectX::XMMatrixTranspose(proj));
		cb.SetView(DirectX::XMMatrixTranspose(view));
		cb.SetWorld(DirectX::XMMatrixTranspose(world));
		constantBuffer->Update(&cb);

		context->Draw(gridShader);
	}
}
void Widgets::RenderBoundingBoxWidget()
{
	if (!IsGood())
		return;

	auto& transformComponent = ecs->GetComponent<TransformComponent>(ecs->selected);
	Matrix matrix = Matrix::Identity;
	Vector3 min = Vector3(transformComponent.GetBoundingBox().Center) - transformComponent.GetBoundingBox().Extents;
	Vector3 max = Vector3(transformComponent.GetBoundingBox().Center) + transformComponent.GetBoundingBox().Extents;
	if (ecs->HasComponent<MeshComponent>(ecs->selected))
	{
		auto& meshComponent = ecs->GetComponent<MeshComponent>(ecs->selected);
		if (meshComponent.IsActive())
		{
			if (boundingBoxShader)
			{
				context->Set(boundingBoxShader);
				context->SetVertexConstantBuffer(constantBuffer, 0);

				DirectX::XMMATRIX view = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveViewMatrix());
				DirectX::XMMATRIX proj = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveProjectionMatrix());

				cb.SetProjection(proj);
				cb.SetView(view);
				cb.SetWorld(matrix);
				constantBuffer->Update(&cb);

				std::vector<POS> vertices;
				vertices.push_back(POS(min.x, min.y, min.z));
				vertices.push_back(POS(max.x, min.y, min.z));
				vertices.push_back(POS(min.x, min.y, min.z));
				vertices.push_back(POS(min.x, max.y, min.z));
				vertices.push_back(POS(min.x, min.y, min.z));
				vertices.push_back(POS(min.x, min.y, max.z));
				vertices.push_back(POS(min.x, max.y, min.z));
				vertices.push_back(POS(max.x, max.y, min.z));
				vertices.push_back(POS(max.x, max.y, min.z));
				vertices.push_back(POS(max.x, min.y, min.z));
				vertices.push_back(POS(max.x, min.y, min.z));
				vertices.push_back(POS(max.x, min.y, max.z));
				vertices.push_back(POS(max.x, max.y, min.z));
				vertices.push_back(POS(max.x, max.y, max.z));
				vertices.push_back(POS(min.x, max.y, min.z));
				vertices.push_back(POS(min.x, max.y, max.z));
				vertices.push_back(POS(min.x, max.y, max.z));
				vertices.push_back(POS(max.x, max.y, max.z));
				vertices.push_back(POS(min.x, max.y, max.z));
				vertices.push_back(POS(min.x, min.y, max.z));
				vertices.push_back(POS(min.x, min.y, max.z));
				vertices.push_back(POS(max.x, min.y, max.z));
				vertices.push_back(POS(max.x, max.y, max.z));
				vertices.push_back(POS(max.x, min.y, max.z));

				context->UpdateVertexBuffer<POS>(boundingBoxShader, vertices);
				context->Draw(boundingBoxShader);
			}
		}
	}
}
void Widgets::Release()
{
	if (boundingBoxShader) boundingBoxShader->Release();
	if (gridShader) gridShader->Release();
	if (constantBuffer) constantBuffer->Release();
	if (rasterizerState) rasterizerState->Release();
	if (perspectiveFrustum) perspectiveFrustum->Release();
	if (orthographicFrustum) orthographicFrustum->Release();
}
void Widgets::Init()
{
	rasterizerState = RasterizerState::Create();
	constantBuffer = ConstantBuffer::Create(sizeof(CB));
}
void Widgets::InitPerspectiveFrustumWidget()
{
	perspectiveFrustum = Shader::Create();
	if (perspectiveFrustum)
	{
		perspectiveFrustum->LoadVertex("data\\shader\\widget\\perspective_frustum\\vertex.hlsl", true);
		perspectiveFrustum->LoadPixel("data\\shader\\widget\\perspective_frustum\\pixel.hlsl", true);

		perspectiveFrustum->CompileVertex();
		perspectiveFrustum->CompilePixel();

		perspectiveFrustum->CreateVertex();
		perspectiveFrustum->CreatePixel();

		perspectiveFrustum->AddLayout("POSITION", 0, 3, 0, 0);
		perspectiveFrustum->CreateLayout();

		std::vector<POS> vertices;
		for (int i = 0; i < 31; i++)
			vertices.push_back(POS(0.0f, 0.0f, 0.0f));
		perspectiveFrustum->CreateVertexBuffer<POS>(vertices, true);

		perspectiveFrustum->ReleaseVertexBlob();
		perspectiveFrustum->ReleasePixelBlob();
	}
}
void Widgets::InitOrthographicFrustumWidget()
{
	orthographicFrustum = Shader::Create();
	if (orthographicFrustum)
	{
		orthographicFrustum->LoadVertex("data\\shader\\widget\\orthographic_frustum\\vertex.hlsl", true);
		orthographicFrustum->LoadPixel("data\\shader\\widget\\orthographic_frustum\\pixel.hlsl", true);

		orthographicFrustum->CompileVertex();
		orthographicFrustum->CompilePixel();

		orthographicFrustum->CreateVertex();
		orthographicFrustum->CreatePixel();

		orthographicFrustum->AddLayout("POSITION", 0, 3, 0, 0);
		orthographicFrustum->CreateLayout();

		std::vector<POS> vertices;
		for (int i = 0; i < 31; i++)
			vertices.push_back(POS(0.0f, 0.0f, 0.0f));
		orthographicFrustum->CreateVertexBuffer<POS>(vertices, true);

		orthographicFrustum->ReleaseVertexBlob();
		orthographicFrustum->ReleasePixelBlob();
	}
}
void Widgets::RenderPerspectiveFrustumWidget()
{
	if (!IsGood())
		return;

	if (ecs->HasComponent<CameraComponent>(ecs->selected))
	{
		auto& cameraComponent = ecs->GetComponent<CameraComponent>(ecs->selected);
		if (cameraComponent.IsActive())
		{
			if (cameraComponent.GetCameraType() == Perspective)
			{
				if (perspectiveFrustum)
				{
					context->Set(perspectiveFrustum);
					context->SetVertexConstantBuffer(constantBuffer, 0);

					DirectX::XMMATRIX view = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveViewMatrix());
					DirectX::XMMATRIX proj = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveProjectionMatrix());
					Matrix matrix = CreateNoScaleMatrix(ecs->GetComponent<TransformComponent>(ecs->selected));
					matrix = DirectX::XMMatrixTranspose(matrix);

					cb.SetProjection(proj);
					cb.SetView(view);
					cb.SetWorld(matrix);
					constantBuffer->Update(&cb);

					float ZPI = DirectX::XM_PI;
					float RAD2DEG = (180.f / ZPI);
					float DEG2RAD = (ZPI / 180.f);

					float _Fov = cameraComponent.GetFov();
					float _Near = cameraComponent.GetNear();
					float _Far = cameraComponent.GetFar();
					float _Width = cameraComponent.GetFixedProjectionValue(CameraType::Perspective).x;
					float _Height = cameraComponent.GetFixedProjectionValue(CameraType::Perspective).y;
					float Hnear = 2 * (float)tan((_Fov * DEG2RAD) / 2) * _Near;
					float Wnear = Hnear * _Width / _Height;
					float Hfar = 2 * (float)tan((_Fov * DEG2RAD) / 2) * _Far;
					float Wfar = Hfar * _Width / _Height;

					std::vector<POS> vertices;
					vertices.push_back(POS(-Wnear, Hnear, _Near));
					vertices.push_back(POS(Wnear, Hnear, _Near));
					vertices.push_back(POS(Wnear, Hnear, _Near));
					vertices.push_back(POS(Wnear, -Hnear, _Near));
					vertices.push_back(POS(Wnear, -Hnear, _Near));
					vertices.push_back(POS(-Wnear, -Hnear, _Near));
					vertices.push_back(POS(-Wnear, -Hnear, _Near));
					vertices.push_back(POS(-Wnear, Hnear, _Near));
					vertices.push_back(POS(-Wnear, Hnear, _Near));
					vertices.push_back(POS(-Wfar, Hfar, _Far));
					vertices.push_back(POS(Wnear, Hnear, _Near));
					vertices.push_back(POS(Wfar, Hfar, _Far));
					vertices.push_back(POS(Wnear, -Hnear, _Near));
					vertices.push_back(POS(Wfar, -Hfar, _Far));
					vertices.push_back(POS(-Wnear, -Hnear, _Near));
					vertices.push_back(POS(-Wfar, -Hfar, _Far));
					vertices.push_back(POS(-Wfar, Hfar, _Far));
					vertices.push_back(POS(Wfar, Hfar, _Far));
					vertices.push_back(POS(Wfar, Hfar, _Far));
					vertices.push_back(POS(Wfar, -Hfar, _Far));
					vertices.push_back(POS(Wfar, -Hfar, _Far));
					vertices.push_back(POS(-Wfar, -Hfar, _Far));
					vertices.push_back(POS(-Wfar, -Hfar, _Far));
					vertices.push_back(POS(-Wfar, Hfar, _Far));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(-Wnear, Hnear, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(Wnear, Hnear, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(Wnear, -Hnear, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(-Wnear, -Hnear, _Near));

					context->UpdateVertexBuffer<POS>(perspectiveFrustum, vertices);
					context->Draw(perspectiveFrustum);
				}
			}
		}
	}
}
void Widgets::RenderOrthographicFrustumWidget()
{
	if (!IsGood())
		return;

	if (ecs->HasComponent<CameraComponent>(ecs->selected))
	{
		auto& cameraComponent = ecs->GetComponent<CameraComponent>(ecs->selected);
		if (cameraComponent.IsActive())
		{
			if (cameraComponent.GetCameraType() == Orthographic)
			{
				if (orthographicFrustum)
				{
					context->Set(orthographicFrustum);
					context->SetVertexConstantBuffer(constantBuffer, 0);

					DirectX::XMMATRIX view = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveViewMatrix());
					DirectX::XMMATRIX proj = DirectX::XMMatrixTranspose(viewportWindow->GetPerspectiveProjectionMatrix());
					Matrix matrix = CreateNoScaleMatrix(ecs->GetComponent<TransformComponent>(ecs->selected));
					matrix = DirectX::XMMatrixTranspose(matrix);

					float _Near = cameraComponent.GetNear();
					float _Far = cameraComponent.GetFar();
					float _Width = cameraComponent.GetFixedProjectionValue(CameraType::Orthographic).x;
					float _Height = cameraComponent.GetFixedProjectionValue(CameraType::Orthographic).y;

					cb.SetProjection(proj);
					cb.SetView(view);
					cb.SetWorld(matrix);
					constantBuffer->Update(&cb);

					std::vector<POS> vertices;
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(_Width, _Height, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(_Width, -_Height, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(-_Width, -_Height, _Near));
					vertices.push_back(POS(0.0f, 0.0f, 0.0f));
					vertices.push_back(POS(-_Width, _Height, _Near));
					vertices.push_back(POS(-_Width, _Height, _Near));
					vertices.push_back(POS(_Width, _Height, _Near));
					vertices.push_back(POS(_Width, _Height, _Near));
					vertices.push_back(POS(_Width, -_Height, _Near));
					vertices.push_back(POS(_Width, -_Height, _Near));
					vertices.push_back(POS(-_Width, -_Height, _Near));
					vertices.push_back(POS(-_Width, -_Height, _Near));
					vertices.push_back(POS(-_Width, _Height, _Near));
					vertices.push_back(POS(-_Width, _Height, _Far));
					vertices.push_back(POS(_Width, _Height, _Far));
					vertices.push_back(POS(_Width, _Height, _Far));
					vertices.push_back(POS(_Width, -_Height, _Far));
					vertices.push_back(POS(_Width, -_Height, _Far));
					vertices.push_back(POS(-_Width, -_Height, _Far));
					vertices.push_back(POS(-_Width, -_Height, _Far));
					vertices.push_back(POS(-_Width, _Height, _Far));
					vertices.push_back(POS(-_Width, _Height, _Near));
					vertices.push_back(POS(-_Width, _Height, _Far));
					vertices.push_back(POS(_Width, _Height, _Near));
					vertices.push_back(POS(_Width, _Height, _Far));
					vertices.push_back(POS(_Width, -_Height, _Near));
					vertices.push_back(POS(_Width, -_Height, _Far));
					vertices.push_back(POS(-_Width, -_Height, _Near));
					vertices.push_back(POS(-_Width, -_Height, _Far));

					context->UpdateVertexBuffer<POS>(orthographicFrustum, vertices);
					context->Draw(orthographicFrustum);
				}
			}
		}
	}
}
bool Widgets::IsGood()
{
	if (ecs->selected != entt::null)
	{
		if (ecs->HasComponent<GeneralComponent>(ecs->selected))
		{
			auto& generalComponent = ecs->GetComponent<GeneralComponent>(ecs->selected);
			if (generalComponent.IsActive())
			{
				if (ecs->HasComponent<TransformComponent>(ecs->selected))
				{
					return true;
				}
			}
		}
	}

	return false;
}
Matrix Widgets::CreateNoScaleMatrix(TransformComponent& transformComponent)
{
	Matrix matrix = Matrix::Identity;
	matrix = Matrix::CreateTranslation(transformComponent.GetPosition()) * matrix;
	matrix = Matrix::CreateFromQuaternion(transformComponent.GetRotationQuaternion()) * matrix;
	matrix = Matrix::CreateScale(Vector3(1.0f, 1.0f, 1.0f)) * matrix;
	return matrix;
}
void Widgets::SetRasterizerState()
{
	if (rasterizerState)
		rasterizerState->Set();
}
void Widgets::UnsetRasterizerState()
{
	if (rasterizerState)
		rasterizerState->Unset();
}

void Widgets::SetRenderGrid(bool _Render)
{
	renderGrid = _Render;
}
void Widgets::SetGridSize(int _Size)
{
	if (!gridShader)
		return;

	gridShader->ReleaseVertexBuffer();
	gridSize = _Size;

	std::vector<POSCOL> vertices = BuildGrid();
	gridShader->CreateVertexBuffer<POSCOL>(vertices, true);
}
void Widgets::SetGridPos(Vector3 _Pos)
{
	gridPos = _Pos;
}
bool Widgets::GetRenderGrid()
{
	return renderGrid;
}
int Widgets::GetGridSize()
{
	return gridSize;
}
Vector3 Widgets::GetGridPos()
{
	return gridPos;
}
std::vector<POSCOL> Widgets::BuildGrid()
{
	std::vector<POSCOL> vertices;
	float lenght = (float)(gridSize * 2);
	lenght -= 1.0;
	float pos = 1.0f;

	for (int i = 0; i < gridSize; i++)
	{
		vertices.push_back(POSCOL(-lenght, -1.0f, pos, 1.0f, 1.0f, 1.0f, 1.0f));
		vertices.push_back(POSCOL(lenght, -1.0f, pos, 1.0f, 1.0f, 1.0f, 1.0f));
		pos += 2.0f;
	}

	pos = -1.0f;

	for (int i = 0; i < gridSize; i++)
	{
		vertices.push_back(POSCOL(-lenght, -1.0f, pos, 1.0f, 1.0f, 1.0f, 1.0f));
		vertices.push_back(POSCOL(lenght, -1.0f, pos, 1.0f, 1.0f, 1.0f, 1.0f));
		pos -= 2.0f;
	}

	pos = 1.0f;

	for (int i = 0; i < gridSize; i++)
	{
		vertices.push_back(POSCOL(pos, -1.0f, -lenght, 1.0f, 1.0f, 1.0f, 1.0f));
		vertices.push_back(POSCOL(pos, -1.0f, lenght, 1.0f, 1.0f, 1.0f, 1.0f));
		pos += 2.0f;
	}

	pos = -1.0f;

	for (int i = 0; i < gridSize; i++)
	{
		vertices.push_back(POSCOL(pos, -1.0f, -lenght, 1.0f, 1.0f, 1.0f, 1.0f));
		vertices.push_back(POSCOL(pos, -1.0f, lenght, 1.0f, 1.0f, 1.0f, 1.0f));
		pos -= 2.0f;
	}

	vertices.push_back(POSCOL(lenght, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(POSCOL(-lenght, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(POSCOL(0.0f, -1.0f, lenght, 0.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(POSCOL(0.0f, -1.0f, -lenght, 0.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(POSCOL(0.0f, lenght, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	vertices.push_back(POSCOL(0.0f, -lenght, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));

	return vertices;
}