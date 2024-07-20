struct INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(INPUT input) : SV_Target
{
    return input.color;
}