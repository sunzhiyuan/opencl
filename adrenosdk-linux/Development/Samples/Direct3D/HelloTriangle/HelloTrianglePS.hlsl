Texture2D simpleTexture : register(t0);
SamplerState simpleSampler: register(s0);


cbuffer AppConstantBuffer
{
    float4x4 perspective;
    float4x4 view;
}

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 texelColor = simpleTexture.Sample(simpleSampler, input.tex);

    // Return the color set in the app times the texture color
    return texelColor;
}