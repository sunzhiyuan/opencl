
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
[HDRTextureShader30VS]
#version 300 es

#include "CommonVS.glsl"

in vec4 g_PositionIn;
out vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_PositionIn.x, g_PositionIn.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_PositionIn.z, g_PositionIn.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[HDRTextureShader30FS]
#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_Texture;
uniform float g_Exposure;
in vec2 g_TexCoord;
out vec4 g_FragColor;

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
	vec4 Color = texture( g_Texture, g_TexCoord);	
    g_FragColor = ConvertRGBEtoFloatingPoint( Color ) * g_Exposure;
}
