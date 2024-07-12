#include "shader.h"

Shader* Shader::Create()
{
	Shader* shader = new Shader();

#if defined(RENDERER_D3D11)
	shader->d3d11_shader = D3D11Shader::Create();
#endif

	return shader;
}
bool Shader::LoadVertex(const char* _Filename, bool _Compile)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->LoadVertex(_Filename, _Compile);
#endif

	return false;
}
bool Shader::LoadPixel(const char* _Filename, bool _Compile)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->LoadPixel(_Filename, _Compile);
#endif

	return false;
}
bool Shader::CompileVertex()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CompileVertex();
#endif

	return false;
}
bool Shader::CompilePixel()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CompilePixel();
#endif

	return false;
}
bool Shader::SaveVertex(const char* _Filename)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->SaveVertex(_Filename);
#endif

	return false;
}
bool Shader::SavePixel(const char* _Filename)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->SavePixel(_Filename);
#endif

	return false;
}
bool Shader::CreateVertex()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CreateVertex();
#endif

	return false;
}
bool Shader::CreatePixel()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CreatePixel();
#endif

	return false;
}
void Shader::AddIndex(UINT _Index)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->AddIndex(_Index);
#endif
}
void Shader::AddIndex(UINT _Index0, UINT _Index1, UINT _Index2)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->AddIndex(_Index0, _Index1, _Index2);
#endif
}
bool Shader::CreateIndexBuffer(bool _CpuAccess)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CreateIndexBuffer(_CpuAccess);
#endif

	return false;
}
bool Shader::AddLayout(LPCSTR _Name, UINT _Index, UINT _Format, UINT _Slot, UINT _Offset)
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->AddLayout(_Name, _Index, _Format, _Slot, _Offset);
#endif

	return false;
}
bool Shader::CreateLayout()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return d3d11_shader->CreateLayout();
#endif

	return false;
}
void Shader::ReleaseVertex()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->ReleaseVertex();
#endif
}
void Shader::ReleasePixel()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->ReleasePixel();
#endif
}
void Shader::ReleaseLayout()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->ReleaseLayout();
#endif
}
void Shader::ReleaseVertexBlob()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->ReleaseVertexBlob();
#endif
}
void Shader::ReleasePixelBlob()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->ReleasePixelBlob();
#endif
}
void Shader::Release()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		d3d11_shader->Release();
#endif

	delete this;
}
ShaderID* Shader::Get()
{
#if defined(RENDERER_D3D11)
	if (d3d11_shader)
		return (ShaderID*)d3d11_shader;
#endif

	return NULL;
}