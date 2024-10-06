Texture2DMS<float4> iTexture;

cbuffer ConstantBuffer : register(b0)
{
    float2 iResolution;
    int sampleCount;
    matrix padding;
}

struct INPUT
{
    float4 pos : SV_POSITION;
};

float4 main(INPUT input) : SV_Target
{
    float2 pixelCoord = input.pos.xy / iResolution;
    int2 texCoord = int2(pixelCoord * iResolution);
    float4 color = float4(0, 0, 0, 0);
    for (int i = 0; i < sampleCount; ++i)
    {
        color += iTexture.Load(texCoord, i); // access the sample
    }
    color /= sampleCount; // average samples to get final color
    //return color;

    // vignette effect
    if (true) // just set to "false" to off
    {
        float2 fragCoord = input.pos.xy;
        float2 uv = fragCoord.xy / iResolution.xy;
        uv *= 1.0 - uv.yx;
        float vig = uv.x * uv.y * 15.0;
        vig = pow(vig, 0.1);
        return float4(color.x * vig, color.y * vig, color.z * vig, 1.0);   
    }

    return color;
}