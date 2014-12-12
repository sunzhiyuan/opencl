
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
//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif

// Enable supported extensions
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
varying vec2 g_TexCoord;

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

	fLogLumSum = log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2( 0,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(texture2D( g_ColorTexture, g_TexCoord + vec2(-StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);

	// Average
	fLogLumSum *= (1.0 / (9.0));
	
 	gl_FragColor = vec4(fLogLumSum, fLogLumSum, fLogLumSum, fLogLumSum);
}
