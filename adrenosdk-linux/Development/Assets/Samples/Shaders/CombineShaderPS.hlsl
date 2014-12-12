
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

cbuffer CombineConstantBuffer
{
    float2 NearQ;
    float2 FocalDistRange;
    bool ThermalColors;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};


Texture2D tSharpTexture : register(t0);
SamplerState sSharpTexture : register(s0);

Texture2D tBlurredTexture : register(t1);
SamplerState sBlurredTexture : register(s1);

Texture2D tDepthTexture : register(t2);
SamplerState sDepthTexture : register(s2);

half4 main(PixelShaderInput input) : SV_TARGET
{
//    gl_FragColor = texture2D( g_SharpTexture, g_TexCoord );
    half4 FocusPixel = tSharpTexture.Sample( sSharpTexture, float2(input.TexCoord.x, 1.0 - input.TexCoord.y) );
    half4 BlurPixel = tBlurredTexture.Sample( sBlurredTexture, input.TexCoord );
    half4 Depth = tDepthTexture.Sample( sDepthTexture, float2(input.TexCoord.x, 1.0 - input.TexCoord.y) );

/*
    // to visualize the dof blend
    if( ThermalColors )
    {
        FocusPixel.gb = float2( 0.0 );
        BlurPixel.rg = float2( 0.0 );
    }
*/
    // Q = Zf / Zf - Zn
    // z = -Zn * Q / Zd - Q
    half Zd = Depth.r;
    half Zn = NearQ.x;
    half Q = NearQ.y;
    half WorldDepth = -( Zn * Q ) / ( Zd - Q );
    // Do 1 - WorldDepth to match output of pixel shader that writes 1-z/w
    WorldDepth = 1.0 - WorldDepth;
    half Lerpval = min( 1.0, abs( FocalDistRange.x - WorldDepth ) * FocalDistRange.y );

    return lerp( FocusPixel, BlurPixel, Lerpval );    
}
