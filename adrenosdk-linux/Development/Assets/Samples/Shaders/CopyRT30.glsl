
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[Copy30VS]
#version 300 es

#include "CommonVS.glsl"

in vec4 g_Vertex;

out vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[Copy30FS]
#version 300 es
#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
in vec2 g_TexCoord;

out vec4 g_FragColor;
void main()
{
	float onePixel = texture( g_ColorTexture, vec2( 0.0, 0.0)).r;
 	g_FragColor = vec4(onePixel, onePixel, onePixel, onePixel);
}
