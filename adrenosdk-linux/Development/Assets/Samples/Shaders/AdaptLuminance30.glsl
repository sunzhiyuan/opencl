
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
[AdaptLuminance30VS]

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
[AdaptLuminance30FS]

#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_CurrentLuminance;
uniform sampler2D g_PreviousLuminance;
in vec2 g_TexCoord;
uniform float ElapsedTime;
uniform float Lambda;

out vec4 g_FragColor;

void main()
{
	// fetch current luminance value
	float CurrentLum = texture( g_CurrentLuminance, vec2( 0.0, 0.0)).r;

	// fetch previous luminance value
	float PreviousLum = texture( g_PreviousLuminance, vec2( 0.0, 0.0)).r;
	
	float Lum = (PreviousLum + (CurrentLum - PreviousLum)  * ( pow( 0.98, Lambda * ElapsedTime) ));

    // set exposure limits
    Lum = clamp( Lum, 0.0, 5.0 );

	g_FragColor = vec4(Lum, Lum, Lum, Lum);
}
