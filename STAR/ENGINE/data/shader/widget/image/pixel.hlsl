Texture2D sTexture2D;
SamplerState sSamplerState;

cbuffer ConstantBuffer : register(b0)
{
    matrix iWorld;
    matrix iView;
    matrix iProjection;
    float opacity;
}

struct INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(INPUT input) : SV_Target
{
    float3 color = sTexture2D.Sample(sSamplerState, input.tex).xyz;
    return float4(color, opacity);
}
