
cbuffer AppConstantBuffer
{
    float4x4 perspective;
    float4x4 view;
}

struct VertexShaderInput
{
    float2 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vertexShaderOutput;
    vertexShaderOutput.pos = mul(perspective, mul(view, float4(input.pos, 0.5f, 1.0f)));
    vertexShaderOutput.tex = input.tex;

    return vertexShaderOutput;
}