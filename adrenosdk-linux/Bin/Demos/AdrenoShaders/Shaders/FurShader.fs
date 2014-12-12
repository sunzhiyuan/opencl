//--------------------------------------------------------------------------------------
// File: FurShader.glsl
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



varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform float g_FurThinning;
uniform vec4 g_SpecColor;


void main()
{
    vec2 UV = g_Tex * vec2( 1.0, 1.0 );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N = vec4( 0.0, 0.0, 1.0, 1.0 );
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    float NL = dot( N.xyz, L );
    float NH = max( 0.0, dot( N.xyz, H ) );

    // Specular
    float SpecOpacity = ( 1.0 - Color.w ); // re-use Color.w to paint in spec between the strands
    float Specular = min( 1.0, pow( NH, g_SpecColor.w ) * SpecOpacity );
    float SelfShadow = saturate( 4.0 * NL );

    gl_FragColor = vec4( Color.rgb * NL, saturate( Color.w - g_FurThinning ) );
    gl_FragColor.rgb += SelfShadow * g_SpecColor.rgb * Specular;
}
