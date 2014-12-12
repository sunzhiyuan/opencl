//--------------------------------------------------------------------------------------
// File: PreethamScatter.glsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader for terrain
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



varying vec3    g_vVSTexCoord;
varying vec3    LightVec;
varying vec3	Inscatter;
varying vec3	Extinction;

uniform   sampler2D g_sBaseTexture; 
uniform   sampler2D g_sNormTexture;

uniform   mat4 g_matModelView;

void main()
{
	vec3 normal = 2.0 * (texture2D( g_sNormTexture, g_vVSTexCoord.xy ).xyz - 0.5);
	normal = vec4(g_matModelView * vec4( normal, 0.0 )).xyz;
	
	vec3 light  = vec3(saturate( dot( normal, LightVec ) ));
	vec3 albedo = texture2D( g_sBaseTexture, g_vVSTexCoord.xy ).xyz; 
 
    gl_FragColor.rgba  = vec4( (light * albedo * Extinction) + Inscatter, 1.);
} 

