

//--------------------------------------------------------------------------------------
// File: ParallaxMapping.glsl
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
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL  g_Material;
uniform sampler2D g_sBaseTexture;
uniform sampler2D g_sBumpTexture;
uniform float     g_fParallax;
uniform float     g_fScale;
uniform int       g_nMode;

varying vec2 g_vOutTexCoord;
varying vec3 g_vOutViewTS;
varying vec3 g_vOutLightTS;
varying vec3 g_vOutNormalWS;
varying vec3 g_vOutViewWS;
varying vec3 g_vOutLightWS;

#define TEXTURE_MAP           0
#define BUMP_MAP              1
#define PARALLAX_MAP          2
#define BUMP_AND_PARALLAX_MAP 3

void main()
{
    if( g_nMode == TEXTURE_MAP )
    {
        vec3 vViewWS  = normalize( g_vOutViewWS );
        vec3 vLightWS = normalize( g_vOutLightWS );
        vec3 vHalfWS  = normalize( vLightWS + vViewWS );

        float fDiffuse = clamp( dot( g_vOutNormalWS, vLightWS), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( g_vOutNormalWS, vHalfWS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        vec4 vBaseTexture = texture2D( g_sBaseTexture, g_vOutTexCoord );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
    else if( g_nMode == BUMP_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        vec4 vBaseTexture = texture2D( g_sBaseTexture, g_vOutTexCoord );

        vec3 vBumpTS = texture2D( g_sBumpTexture, g_vOutTexCoord ).xyz * 2.0 - 1.0;
        vBumpTS = normalize( vBumpTS );

        vec3 vLightTS = normalize( g_vOutLightTS );
        vec3 vHalfTS  = normalize( vLightTS + vViewTS );

        float fDiffuse = clamp( dot( vBumpTS, vLightTS ), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( vBumpTS, vHalfTS ), 0.0, 1.0 ), g_Material.vSpecular.a );
    
        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
    else if( g_nMode == PARALLAX_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        float fHeight = texture2D( g_sBumpTexture, g_vOutTexCoord ).a;
        float fOffset = g_fParallax * ( g_fScale * fHeight - 0.5 * g_fScale );
        vec2 vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        fHeight += texture2D( g_sBumpTexture, vParallaxTexCoord ).a;
        fOffset = g_fParallax * ( fHeight - 1.0 );
        vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        vec4 vBaseTexture = texture2D( g_sBaseTexture, vParallaxTexCoord );

        vec3 vViewWS  = normalize( g_vOutViewWS );
        vec3 vLightWS = normalize( g_vOutLightWS );
        vec3 vHalfWS  = normalize( vLightWS + vViewWS );

        float fDiffuse = clamp( dot( g_vOutNormalWS, vLightWS), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( g_vOutNormalWS, vHalfWS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;

    }
    else if( g_nMode == BUMP_AND_PARALLAX_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        float fHeight = texture2D( g_sBumpTexture, g_vOutTexCoord ).a;
        float fOffset = g_fParallax * ( g_fScale * fHeight - 0.5 * g_fScale );
        vec2 vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        fHeight += texture2D( g_sBumpTexture, vParallaxTexCoord ).a;
        fOffset = g_fParallax * ( fHeight - 1.0 );
        vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        vec4 vBaseTexture = texture2D( g_sBaseTexture, vParallaxTexCoord );

        vec3 vBumpTS = texture2D( g_sBumpTexture, vParallaxTexCoord ).xyz * 2.0 - 1.0;
        vBumpTS = normalize( vBumpTS );

        vec3 vLightTS = normalize( g_vOutLightTS );
        vec3 vHalfTS  = normalize( vLightTS + vViewTS );

        float fDiffuse = clamp( dot( vBumpTS, vLightTS ), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( vBumpTS, vHalfTS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
}
