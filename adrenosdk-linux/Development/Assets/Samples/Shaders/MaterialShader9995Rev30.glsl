
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
[MaterialShader9995Rev30FS]

#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_Texture;
uniform float g_Exposure;
in vec2 g_TexCoord;

out vec4 g_FragColor;

void main()
{
    vec4 Color = texture( g_Texture, g_TexCoord );
    g_FragColor = vec4(Color.rgb * g_Exposure, 1.0); 
}
