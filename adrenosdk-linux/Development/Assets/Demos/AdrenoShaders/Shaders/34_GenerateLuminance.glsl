//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[GenerateLuminance34VS]
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
[GenerateLuminance34FS]
#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
varying vec2 g_TexCoord;

void main()
{
  //  vec4 FocusPixel = texture2D( g_ColorTexture, g_TexCoord );

    // used to prevent black pixels
    const float Delta = 0.0001;  
    
	// luminance values            
	// somehow we have to raise the value range here for the log function ....
	const vec3 LUMINANCE = vec3(0.2125, 0.7154, 0.0721) *  5.5;      
    
    float fLogLumSum = 0.0;
    vec2 StepSize = g_StepSize;
    
	fLogLumSum  = log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x, 0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x, 0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x,-0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x,-0.5 * StepSize.y)).rgb, LUMINANCE) + Delta);
    
/*
	fLogLumSum = log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);
*/
	// Average
	fLogLumSum *= (1.0 / (4.0));
	
 	gl_FragColor = vec4(fLogLumSum, fLogLumSum, fLogLumSum, fLogLumSum);
}
