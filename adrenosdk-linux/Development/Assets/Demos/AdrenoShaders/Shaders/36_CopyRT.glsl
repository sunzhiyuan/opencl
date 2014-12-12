//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CopyRT36VS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CopyRT36FS]
#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
varying vec2 g_TexCoord;

void main()
{
	float onePixel = texture2D( g_ColorTexture, vec2( 0.0, 0.0)).r;
 	gl_FragColor = vec4(onePixel, onePixel, onePixel, onePixel);
}
