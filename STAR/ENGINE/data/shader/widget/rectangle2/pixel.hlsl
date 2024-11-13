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
    float4 pos : SV_POSITION;
};

float4 main(INPUT input) : SV_Target
{
    return float4(iColor, iOpacity);
}
