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
uniform sampler2D g_BumpTexture;

varying vec2 g_TexCoord;
varying vec4 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;

void main()
{
    vec4 vBaseColor = texture2D( g_DiffuseTexture, g_TexCoord );
        
    // Select the normal in the appropriate space
    vec3 vNormal = texture2D( g_BumpTexture, g_TexCoord).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    vec3  vHalf     = normalize( g_LightVec.xyz + g_ViewVec );
    float fDiffuse  = saturate( dot( g_LightVec.xyz, vNormal ) );
    
    //float fSpecular = saturate( dot( vHalf, vNormal ) * dot( vHalf, vNormal ) * dot( vHalf, vNormal ));
    float fSpecular = 0.0; //pow( saturate( dot( vHalf, vNormal ) ), 40.0 );

	gl_FragColor = saturate(g_AmbientColor + vec4(fDiffuse * vBaseColor.x + fSpecular, fDiffuse * vBaseColor.y + fSpecular, fDiffuse * vBaseColor.z + fSpecular, 0.0) * g_LightVec.w);
}
