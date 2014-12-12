//--------------------------------------------------------------------------------------
// Desc: Shaders for the Robot demo
//
// Adreno SDK
//
// Copyright (c) 2013 QUALCOMM Technologies, Inc.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Adreno SDK
//
// Copyright (c) 2013 QUALCOMM Incorporated.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
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


//--------------------------------------------------------------------------------------
// Fragment shader
//--------------------------------------------------------------------------------------
uniform vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

uniform sampler2D BaseTexture;

varying	vec3  Out_vNormal;
varying vec4  Out_Diffuse;
varying vec2  Out_Tex0;

void main()
{
    vec4 vDiffuse  = Out_Diffuse;

    if( false )
    {
        vec3  vNormal = normalize( Out_vNormal );
        vec3  vLight  = g_vLightDir;

        // Compute the lighting in eye-space
        float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );

        // Combine lighting with the material properties
        vDiffuse.rgb  = g_vMaterialAmbient.rgb + g_vMaterialDiffuse.rgb * fDiffuse;
        vDiffuse.a    = g_vMaterialDiffuse.a;
    }

    gl_FragColor.rgba  = vDiffuse.rgba * texture2D( BaseTexture, Out_Tex0 ).rgba;
}
