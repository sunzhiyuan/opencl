//--------------------------------------------------------------------------------------
// File: MaterialShader.glsl
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



uniform vec4 g_AmbientColor;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;

varying vec2 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;

void main()
{
    vec4 vBaseColor = texture2D( g_DiffuseTexture, g_TexCoord );
    
    // Select the normal in the appropriate space
    // Note: the mip level is biased to sharpen the bumpmapping effect
    vec3 vNormal    = texture2D( g_BumpTexture, g_TexCoord, -1.0 ).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    float fAtten    = saturate( 1.0 - 0.05 * dot( g_LightVec, g_LightVec ) );
    vec3  vLight    = normalize( g_LightVec );
    vec3  vView     = normalize( g_ViewVec );
    vec3  vHalf     = normalize( vLight + vView );
    float fDiffuse  = saturate( dot( vLight, vNormal ) );
    float fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 64.0 );
    float fLightIntensity = 2.0;

    gl_FragColor = ( fDiffuse * fAtten * fLightIntensity + g_AmbientColor ) * vBaseColor;
    gl_FragColor.rgb += vec3( ( fSpecular * fAtten ) * 0.7);
}
