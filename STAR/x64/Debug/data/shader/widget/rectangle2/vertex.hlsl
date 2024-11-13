#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix iWorld;
    matrix iView;
    matrix iProjection;
    float iOpacity;
	float3 iColor;
}

struct INPUT
{
    float3 pos : POSITION;
};

struct OUTPUT
{
    float4 pos : SV_POSITION;
};

OUTPUT main(INPUT input)
{
    OUTPUT output;
    matrix wvp = mul(iWorld, mul(iView, iProjection));
    output.pos = mul(float4(input.pos, 1.0), wvp);
    return output;
}
