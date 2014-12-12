
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
[MaterialShader9995RevFS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
uniform float g_Exposure;
varying vec2 g_TexCoord;

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = vec4(Color.rgb * g_Exposure, 1.0); 
}

