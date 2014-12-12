//--------------------------------------------------------------------------------------
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
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



varying vec2 g_TexCoord;
//uniform vec2 g_StepSize;
uniform sampler2D g_SharpTexture;
uniform float g_WarpAmount;

void main()
{
	// this is the center point we want to warp
	// if you want to mimic an huge explosion this point should be where the explosion happens in 
	// screen-space
	vec2 warpingPoint = vec2(.5, .5); 
	
	// center the texture coordinates around the warping point
	vec2 currentTexCoord = g_TexCoord - warpingPoint; 
	
	// measure the distance to the center
	float dist = length(currentTexCoord); 
   
    // how much sphere should the warp have?
	float amount = g_WarpAmount;
	
	// use a power curve to warp the texture coordinates
    // by adding the warping point, move the texture coordinate back into 
    // its original texture space
    // 0.7 is a magic value
	vec2 newTexCoord = .7 * pow(dist, amount) * currentTexCoord + warpingPoint;
  
	// fetch with new texture coordinates
	vec4 FocusPixel = texture2D( g_SharpTexture, newTexCoord);
   
    gl_FragColor = FocusPixel;
}
