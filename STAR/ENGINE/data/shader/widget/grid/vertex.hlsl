struct INPUT
{
   float4 pos : POSITION;
   float4 color : COLOR;
};

struct OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
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
    output.color = input.color;
    return output;
}