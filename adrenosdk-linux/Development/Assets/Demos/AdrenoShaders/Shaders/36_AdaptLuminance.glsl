//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[AdaptLuminance36VS]
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
[AdaptLuminance36FS]
#include "CommonFS.glsl"

uniform sampler2D g_CurrentLuminance;
uniform sampler2D g_PreviousLuminance;
varying vec2 g_TexCoord;
uniform float ElapsedTime;
uniform float Lambda;


void main()
{
	// fetch current luminance value
	float CurrentLum = texture2D( g_CurrentLuminance, vec2( 0.0, 0.0)).r;

	// fetch previous luminance value
	float PreviousLum = texture2D( g_PreviousLuminance, vec2( 0.0, 0.0)).r;
	
	float Lum = (PreviousLum + (CurrentLum - PreviousLum) * ( 1.0 - pow( 0.98, Lambda * ElapsedTime) ));

    // set exposure limits
    Lum = clamp( Lum, 0.0, 0.2 );

	gl_FragColor = vec4(Lum, Lum, Lum, Lum);
}
