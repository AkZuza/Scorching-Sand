#pragma vs main

struct VertexInput
{
    float3 position: POSITION;
    float4 color: COLOR;
};

struct V2P
{
    float4 position: SV_POSITION;
    float4 color;
};

V2P main(VertexInput input)
{
    V2P output;
    output.position = float4(input.position, 1.0);
    output.color = input.color;

    return output;
}
