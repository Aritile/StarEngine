#include "../tonemapping/aces.hlsl"

cbuffer ConstantBuffer : register(b0)
{
	matrix sProjection;
	matrix sView;
	matrix sModel;
    bool hasTexture;
    int renderState;
}

SamplerState sSamplerState;
Texture2D sTexture2D;

float4 main(float4 pos : SV_POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 lightPos = float3(2.0f, 2.0f, -2.0f);
    float3 lightColor = float3(0.6f, 0.8f, 1.0f);
    float3 objectColor = float3(0.6f, 0.6f, 0.6f);
    
    // ambient
    float ambientStrength = 0.4;
    float3 ambient = ambientStrength * lightColor;
    
    // diffuse 
    float3 norm = normalize(normal);
    float3 pos_norm = normalize(pos);
    float3 lightDir = normalize(lightPos - float3(pos_norm.x, pos_norm.y, pos_norm.z));
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * lightColor;
    float3 result = (ambient + diffuse) * objectColor;
    
    if (renderState == 3)
    {
        return float4(pos_norm, 1.0);
    }
    else if (renderState == 4)
    {
        return float4(normal, 1.0);
    }
    else
    {
        if (hasTexture)
        {
            float3 textureColor = sTexture2D.Sample(sSamplerState, texcoord).xyz;
            float3 finalColor = textureColor * result;
            return float4(To_ACES(finalColor), 1.0);
        }
        else
        {
            return float4(To_ACES(result), 1.0);
        }
    }
}