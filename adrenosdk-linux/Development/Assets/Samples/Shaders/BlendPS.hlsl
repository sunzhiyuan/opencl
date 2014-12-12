//--------------------------------------------------------------------------------------
// File: BlendPS.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture Blending
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------

Texture2D tImageTexture1 : register(t0);
SamplerState sImageTexture1 : register(s0);

Texture2D tImageTexture2 : register(t1);
SamplerState sImageTexture2 : register(s1);


struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoords  : TEXCOORD0;
    float2 vTexCoords0 : TEXCOORD1;
    float2 vTexCoords1 : TEXCOORD2;
    float2 vTexCoords2 : TEXCOORD3;
    float2 vTexCoords3 : TEXCOORD4;    
};

half4 main( PixelShaderInput input ) : SV_TARGET
{
    half4 vColor1 = tImageTexture1.Sample( sImageTexture1, input.vTexCoords );
    
    half4 s0 = tImageTexture2.Sample( sImageTexture2, input.vTexCoords0 );
    half4 s1 = tImageTexture2.Sample( sImageTexture2, input.vTexCoords1 );
    half4 s2 = tImageTexture2.Sample( sImageTexture2, input.vTexCoords2 );
    half4 s3 = tImageTexture2.Sample( sImageTexture2, input.vTexCoords3 );

    half4 vSobelX = 4.0 * ((s0 + s2) - (s1 + s3));
    half4 vSobelY = 4.0 * ((s0 + s1) - (s2 + s3));

    // Compute the vector length
    half4 vSobelSqr = vSobelX * vSobelX + vSobelY * vSobelY;
    half fSobel = length( vSobelSqr );
    if( fSobel > 0.2 )
    {
        fSobel = 1.0;
    }

    half4 outColor = vColor1 * clamp( half4( 1.0, 1.0, 1.0, 1.0 ) - half4( fSobel, fSobel, fSobel, 1.0 - fSobel ), float4( 0.0, 0.0, 0.0, 0.0 ), float4( 1.0, 1.0, 1.0, 1.0 ) );
    return outColor;
}

