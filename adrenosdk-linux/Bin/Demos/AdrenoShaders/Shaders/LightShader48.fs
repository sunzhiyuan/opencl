//--------------------------------------------------------------------------------------
// File: DownsizeShader.glsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
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
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



varying vec2 g_TexCoord;

uniform sampler2D g_NormalTexture;
uniform sampler2D g_DepthTexture;

uniform vec4 g_LightPos;
uniform vec3 g_LightColor;
uniform mat4 g_MatModelViewProjInv;

void main()
{
    vec4 Normal = texture2D( g_NormalTexture, g_TexCoord ) * 2.0 - 1.0;
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    // recalculate world position of this pixel
    vec4 ScreenPos = vec4( g_TexCoord.x * 2.0 - 1.0, g_TexCoord.y * 2.0 - 1.0, Depth.r, 1.0 );
    vec4 WorldPos = ScreenPos * g_MatModelViewProjInv;
    WorldPos.xyz /= WorldPos.w;

	// Lighting
/*
	vec3 lVec = (g_LightPos.xyz - WorldPos.xyz) * (1.0 / g_LightPos.a);
	vec3 lightVec = normalize(lVec);

	// Attenuation = 1/( att0 + att1 * d + att2 * d2)
	// hard-coded here
	float d = length(lVec);
	float d2 = d*d;
	float atten = 1.0 / ( 0.1 + 46.6 * d + 26.8 * d2);
*/

	vec3 lVec = (g_LightPos.xyz - WorldPos.xyz);
	vec3 lightVec = normalize(lVec);
	float d = length(lVec);
	float atten = pow(saturate((g_LightPos.a - d) / g_LightPos.a), 8.0);
	
	float NL = saturate(dot(lightVec, Normal.xyz));
	float Diffuse = NL * atten;

    gl_FragColor = vec4(Diffuse, Diffuse, Diffuse, Diffuse);
    gl_FragColor.rgb *= g_LightColor.rgb;
  //  gl_FragColor = vec4(1.0 * atten, 0.0, 0.0, 1.0);
}
