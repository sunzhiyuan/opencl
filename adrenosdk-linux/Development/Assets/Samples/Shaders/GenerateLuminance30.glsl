
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
[GenerateLuminance30VS]

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
[GenerateLuminance30FS]

#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
in vec2 g_TexCoord;

out vec4 g_FragColor;

void main()
{
  //  vec4 FocusPixel = texture2D( g_ColorTexture, g_TexCoord );

    // used to prevent black pixels
    const float Delta = 0.0001;  
    
	// luminance values            
	// amplify luminance in this scene
	const vec3 LUMINANCE = vec3(0.2125, 0.7154, 0.0721)  * 20.0;      
    
    float fLogLumSum = 0.0;
    vec2 StepSize = g_StepSize;
/*
	fLogLumSum  = log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x, 0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x, 0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x,-0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x,-0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
*/    

	fLogLumSum = log(dot(texture( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2(StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2( 0, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2( 0,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2( 0,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2(-StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);

	// Average
	fLogLumSum *= (1.0 / (9.0));
	
 	g_FragColor = vec4(fLogLumSum, fLogLumSum, fLogLumSum, fLogLumSum);
}
