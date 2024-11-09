Texture2D sTexture2D;
SamplerState sSamplerState;

struct INPUT
{
    float4 pos : SV_POSITION;  // Pixel position
    float2 tex : TEXCOORD;     // Texture coordinates
};

float4 main(INPUT input) : SV_Target
{
    float3 color = sTexture2D.Sample(sSamplerState, input.tex).xyz;
    return float4(color, 1.0);
}
