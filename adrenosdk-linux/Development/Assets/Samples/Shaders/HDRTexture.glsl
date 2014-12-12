
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The vertex shader.
//--------------------------------------------------------------------------------------
[HDRTextureShaderVS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
varying vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_PositionIn.x, g_PositionIn.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_PositionIn.z, g_PositionIn.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[HDRTextureShaderFS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
uniform float g_Exposure;
varying vec2 g_TexCoord;

vec4 ConvertRGBEtoFloatingPoint( vec4 Value )
{
    // Alpha values get gamma corrected in openGL or by the texture packer.
    // Values of 127 turn into 104.  This only happens to the alpha channel,
    // not the other three.
    float MidPoint = ( 104.0 / 255.0 );

    float Brightness = pow( 2.0, ( Value.a - MidPoint ) * 10.0 );
    return vec4( Value.rgb * Brightness, 1.0 );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBEtoFloatingPoint( Color ) * g_Exposure;
}
