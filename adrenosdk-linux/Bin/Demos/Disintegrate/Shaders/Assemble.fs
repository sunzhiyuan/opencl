//--------------------------------------------------------------------------------------
// File: Assemble.glsl
// Desc: Shaders for the Disintegrate sample
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

// OpenGL ES requires that precision is defined for a fragment shader
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



varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform sampler2D g_NoiseTexture;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform float g_fAssembleValue;


void main()
{
    vec4 C = texture2D( g_DiffuseTexture, g_Tex);
    vec4 N = texture2D( g_BumpTexture, g_Tex ) * 2.0 - 1.0;
    vec4 NoiseVal = texture2D( g_NoiseTexture, g_Tex );
    if(NoiseVal.x > g_fAssembleValue)
        discard;
    
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    
    float Diffuse   = saturate( dot( L, N.xyz ) );

    vec3 vNormal    = normalize(N.xyz);
    vec3 vHalf      = normalize( L + V );
    
    float ambient   = 0.2;
    float diffuse   = saturate( dot( L, vNormal ) );
    float specular  = pow( saturate( dot( vNormal, H ) ), g_SpecularColor.w );

    vec3 AmbientColor = C.rgb * g_AmbientLight.rgb;
    gl_FragColor    = (ambient + diffuse) * C + specular * 0.7;

    // Fix-up the alpha value
    gl_FragColor.w = 1.0;
}
