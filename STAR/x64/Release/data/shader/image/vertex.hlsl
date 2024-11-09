#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    float iTime;
    float2 iResolution;
    matrix iWorld;
    matrix iView;
    matrix iProjection;
}

struct INPUT
{
    float3 pos : POSITION;  // Input vertex position
    float2 tex : TEXCOORD;  // Input texture coordinates
};

struct OUTPUT
{
    float4 pos : SV_POSITION; // Output position
    float2 tex : TEXCOORD;    // Output texture coordinates
};

OUTPUT main(INPUT input)
{
    OUTPUT output;
    
    // Apply World-View-Projection transformation
    matrix wvp = mul(iWorld, mul(iView, iProjection));
    output.pos = mul(float4(input.pos, 1.0), wvp);
    
    // Pass through texture coordinates
    output.tex = input.tex;
    
    return output;
}
