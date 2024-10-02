#pragma pack_matrix(row_major)

struct INPUT
{
   float4 pos : POSITION;
};

struct OUTPUT
{
    float4 pos : SV_POSITION;
};

cbuffer ConstantBuffer : register( b0 )
{
    float iTime;
    float2 iResolution;
    matrix iWorld;
    matrix iView;
    matrix iProjection;
}

OUTPUT main(INPUT input)
{
    OUTPUT output;
    output.pos = mul( input.pos, iWorld );
    output.pos = mul( output.pos, iView );
    output.pos = mul( output.pos, iProjection );
    return output;
}