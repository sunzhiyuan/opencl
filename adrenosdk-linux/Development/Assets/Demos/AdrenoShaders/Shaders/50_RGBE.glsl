//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[RGBtoRGBEFS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
varying vec2 g_TexCoord;

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = Color;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[RGBEtoRGBFS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
varying vec2 g_TexCoord;

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = Color;
}
