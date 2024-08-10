#pragma once

#include "d3d/d3d11_shader.h"
#include "constant_buffer.h"
#include "strdx.h"
#include "sampler_state.h"

class Shader
{
public:
	Shader() : d3d11_shader(NULL) {}

public:
	static Shader* Create();
	bool LoadVertex(const char* _Filename, bool _Compile);
	bool LoadPixel(const char* _Filename, bool _Compile);
	bool CompileVertex();
	bool CompilePixel();
	bool SaveVertex(const char* _Filename);
	bool SavePixel(const char* _Filename);
	bool CreateVertex();
	bool CreatePixel();
	template <typename T>
	bool CreateVertexBuffer(std::vector<T>& _Vertices, bool _CpuAccess = false)
	{
#if defined(RENDERER_D3D11)
		if (d3d11_shader)
			return d3d11_shader->CreateVertexBuffer<T>(_Vertices, _CpuAccess);
#endif

		return false;
	}
	void AddIndex(UINT _Index);
	void AddIndex(UINT _Index0, UINT _Index1, UINT _Index2);
	bool CreateIndexBuffer(bool _CpuAccess = false);
	bool AddLayout(LPCSTR _Name, UINT _Index, UINT _Format, UINT _Slot, UINT _Offset);
	bool CreateLayout();
	void ReleaseVertex();
	void ReleasePixel();
	void ReleaseLayout();
	void ReleaseVertexBlob();
	void ReleasePixelBlob();
	void ReleaseVertexBuffer();
	void Release();
	ShaderID* Get();

private:
	D3D11Shader* d3d11_shader;
};