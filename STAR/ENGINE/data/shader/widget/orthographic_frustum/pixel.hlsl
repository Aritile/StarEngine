struct INPUT
{
    float4 pos : SV_POSITION;
};

float4 main(INPUT input) : SV_Target
{
    return float4(0.886275, 0.321569, 0.321569, 1.0);
}