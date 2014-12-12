
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

cbuffer HDRTextureConstantBuffer
{
    float Exposure;
};

Texture2D tTexture : register(t0);
SamplerState sTexture : register(s0);

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

float4 ConvertRGBEtoFloatingPoint( float4 Value )
{
    // Alpha values get gamma corrected in openGL or by the texture packer.
    // Values of 127 turn into 104.  This only happens to the alpha channel,
    // not the other three.
    float MidPoint = ( 104.0 / 255.0 );

    float Brightness = pow( 2.0, ( Value.a - MidPoint ) * 10.0 );
    return float4( Value.rgb * Brightness, 1.0 );
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 Color = tTexture.Sample( sTexture, input.vTexCoord );
    return ConvertRGBEtoFloatingPoint( Color ) * Exposure;
}


