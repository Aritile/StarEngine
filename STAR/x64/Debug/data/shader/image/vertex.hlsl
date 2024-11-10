#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix iWorld;
    matrix iView;
    matrix iProjection;
    float opacity;
}

struct INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct OUTPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

OUTPUT main(INPUT input)
{
    OUTPUT output;
    matrix wvp = mul(iWorld, mul(iView, iProjection));
    output.pos = mul(float4(input.pos, 1.0), wvp);
    output.tex = input.tex;
    return output;
}
