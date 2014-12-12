

//--------------------------------------------------------------------------------------
// File: PerVertexLighting.glsl
// Desc: Shaders for per-vertex lighting
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
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



struct MATERIAL
{
    vec3 vAmbient;
    vec4 vDiffuse;
    vec4 vSpecular;
    float fShininess;
};

uniform sampler2D g_DiffuseSampler;
uniform sampler2D g_NormalSampler;
uniform MATERIAL g_Material;

varying vec2 Out_Tex0;
varying vec3 Out_Light;
varying vec3 Out_EyeNormal;


//--------------------------------------------------------------------------------------
// Fragment shader entry point
//--------------------------------------------------------------------------------------
void main()
{
	// Get texture color and normal map
	vec4 vDiffuse = texture2D( g_DiffuseSampler, Out_Tex0 );
	vec3 vNormalMap = texture2D( g_NormalSampler, Out_Tex0 ).rgb * 2.0 - 1.0;
	
	// Apply ambient color
    gl_FragColor.rgb = g_Material.vAmbient;
    gl_FragColor.a = 0.0;
    
    // Get light and eye normal
    vec3 vLight = normalize( Out_Light );
    vec3 vEyeNormal = normalize( Out_EyeNormal );
	
	// Apply diffuse color
    float fDiffuse = max( 0.0, dot( vNormalMap, vLight ) );
    gl_FragColor.rgba += vDiffuse * g_Material.vDiffuse * fDiffuse;
    
    // Apply specular color
    vec3 vHalfVector = normalize( vLight + vEyeNormal );
    float fSpecular = pow( max( 0.0, dot( vNormalMap, vHalfVector ) ), g_Material.fShininess );
    gl_FragColor.rgba += g_Material.vSpecular * fSpecular;
}

