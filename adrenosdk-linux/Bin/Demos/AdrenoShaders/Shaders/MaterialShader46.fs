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



uniform vec4 g_AmbientColor;
uniform sampler2D g_DiffuseTexture;
uniform samplerCube g_Cubemap;

varying vec2 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;


vec3 CubeNormalize( vec3 InputNormal )
{
    vec4 CubeColor = textureCube( g_Cubemap, InputNormal );
    return ( CubeColor.rgb * 2.0 - 1.0 );
}


void main()
{
    vec4 Color = texture2D( g_DiffuseTexture, g_TexCoord );


    // Standard Phong lighting
    float Attenuation = saturate( 1.0 - 0.05 * dot( g_LightVec, g_LightVec ) );

#if 1
    vec3  Normal      = CubeNormalize( g_Normal );
    vec3  Light       = CubeNormalize( g_LightVec );
    vec3  View        = CubeNormalize( g_ViewVec );
    vec3  Half        = CubeNormalize( Light + View );
#else
    vec3  Normal      = normalize( g_Normal );
    vec3  Light       = normalize( g_LightVec );
    vec3  View        = normalize( g_ViewVec );
    vec3  Half        = normalize( Light + View );
#endif

    float Diffuse     = saturate( dot( Light, Normal ) );
    float Specular    = pow( saturate( dot( Half, Normal ) ), 64.0 );

    gl_FragColor = ( Diffuse * Attenuation + g_AmbientColor ) * Color;
    gl_FragColor.rgb += vec3( ( Specular * Attenuation ) );
}
