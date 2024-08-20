struct INPUT
{
   float4 pos : POSITION;
};

struct OUTPUT
{
    float4 pos : SV_POSITION;
};

OUTPUT main(INPUT input)
{
    OUTPUT output;
    output.pos = input.pos;
    return input;
}