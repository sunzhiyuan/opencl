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
uniform vec2 g_StepSize;
uniform sampler2D g_SharpTexture;

// the shader compiler should be able to vectorize this ... but it seems like it doesn't allow me 
// vectorize this
vec3 FindMedian(vec3 RGB, vec3 RGB2, vec3 RGB3) 
{ 
	vec3 res;
	res.x = (RGB.x < RGB2.x) ? ((RGB2.x < RGB3.x) ? RGB2.x : max(RGB.x, RGB3.x) ) : ((RGB.x < RGB3.x) ? RGB.x : max(RGB2.x,RGB3.x)); 
	res.y = (RGB.y < RGB2.y) ? ((RGB2.y < RGB3.y) ? RGB2.y : max(RGB.y, RGB3.y) ) : ((RGB.y < RGB3.y) ? RGB.y : max(RGB2.y,RGB3.y)); 
	res.z = (RGB.z < RGB2.z) ? ((RGB2.z < RGB3.z) ? RGB2.z : max(RGB.z, RGB3.z) ) : ((RGB.z < RGB3.z) ? RGB.z : max(RGB2.z,RGB3.z)); 
	return res; 
}

void main()
{
/*
    vec3 FocusPixel0 = texture2D( g_SharpTexture, g_TexCoord + vec2( g_StepSize.x, g_StepSize.y)).xyz;
    vec3 FocusPixel1 = texture2D( g_SharpTexture, g_TexCoord + vec2(0.0, g_StepSize.y)).xyz;
    vec3 FocusPixel2 = texture2D( g_SharpTexture, g_TexCoord + vec2(-g_StepSize.x, g_StepSize.y)).xyz;
    vec3 Median0 = FindMedian(FocusPixel0, FocusPixel1, FocusPixel2); 
    vec3 FocusPixel3 = texture2D( g_SharpTexture, g_TexCoord + vec2( g_StepSize.x, 0.0)).xyz;
    vec3 FocusPixel4 = texture2D( g_SharpTexture, g_TexCoord + vec2(0.0, 0.0) ).xyz;
    vec3 FocusPixel5 = texture2D( g_SharpTexture, g_TexCoord + vec2(-g_StepSize.x, 0.0)).xyz;
    vec3 Median1 = FindMedian(FocusPixel3, FocusPixel4, FocusPixel5); 
    vec3 FocusPixel6 = texture2D( g_SharpTexture, g_TexCoord + vec2( g_StepSize.x, -g_StepSize.y)).xyz;
    vec3 FocusPixel7 = texture2D( g_SharpTexture, g_TexCoord + vec2(0.0, -g_StepSize.y) ).xyz;
    vec3 FocusPixel8 = texture2D( g_SharpTexture, g_TexCoord + vec2(-g_StepSize.x, -g_StepSize.y) ).xyz;
    vec3 Median2 = FindMedian(FocusPixel6, FocusPixel7, FocusPixel8); 
    
    vec3 Median3 = FindMedian(Median0, Median1, Median2); 

    gl_FragColor = vec4(Median3.x, Median3.y, Median3.z, 1.0);
*/

    vec3 FocusPixel0 = texture2D( g_SharpTexture, g_TexCoord + vec2( g_StepSize.x, g_StepSize.y)).xyz;
    vec3 FocusPixel1 = texture2D( g_SharpTexture, g_TexCoord + vec2(0.0, 0.0)).xyz;
    vec3 FocusPixel2 = texture2D( g_SharpTexture, g_TexCoord + vec2(-g_StepSize.x, -g_StepSize.y) ).xyz;
    vec3 Median0 = FindMedian(FocusPixel0, FocusPixel1, FocusPixel2); 
    vec3 FocusPixel6 = texture2D( g_SharpTexture, g_TexCoord + vec2( g_StepSize.x, -g_StepSize.y)).xyz;
    vec3 FocusPixel7 = texture2D( g_SharpTexture, g_TexCoord + vec2(0.0, 0.0) ).xyz;
    vec3 FocusPixel8 = texture2D( g_SharpTexture, g_TexCoord + vec2(-g_StepSize.x, g_StepSize.y) ).xyz;
    vec3 Median2 = FindMedian(FocusPixel6, FocusPixel7, FocusPixel8); 
    
	vec3 Median1 = vec3(0.5, 0.5,0.5);

    vec3 Median3 = FindMedian(Median0, Median1, Median2); 

    gl_FragColor = vec4(Median3.x, Median3.y, Median3.z, 1.0);

}
