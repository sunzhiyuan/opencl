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
uniform highp vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

uniform sampler2D BaseTexture;
uniform sampler2D LightingTexture;

varying vec4  Out_Diffuse;
varying vec4  Out_Specular;
varying vec2  Out_Tex0;

varying	vec3  Out_vNormal;
//varying	vec3  Out_vHalf;


void main()
{
    vec4 vDiffuse;
    vec4 vSpecular;

    vec3 vNormal = normalize( Out_vNormal );
    vec2 vLightingTexCoord;
    vLightingTexCoord.x = -0.5 * vNormal.x + 0.5;
    vLightingTexCoord.y = +0.5 * vNormal.y + 0.5;

    if( true )
    {
        vec3 vNormal = normalize( Out_vNormal );
        vec3 vLight  = g_vLightDir;
//		vec3 vHalf   = normalize( Out_vHalf );
        vec3 vHalf   = vec3( -0.30151,0.30151,0.904534 );

        // Compute the lighting in eye-space
        float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
        float fSpecular = fDiffuse > 0.0 ? pow( max( 0.0, dot( vNormal, vHalf ) ), 5.0 * g_vMaterialSpecular.a ) : 0.0;

        // Combine lighting with the material properties
        vDiffuse.rgb  = g_vMaterialAmbient.rgb + g_vMaterialDiffuse.rgb * fDiffuse;
        vDiffuse.a    = g_vMaterialDiffuse.a;

        vSpecular.rgb = g_vMaterialSpecular.rgb * fSpecular;
        vSpecular.a   = 0.0;//fSpecular;
    }
    else
    {
        vDiffuse  = Out_Diffuse;
        vSpecular = Out_Specular;
    }

//	gl_FragColor.rgba  = vDiffuse.rgba * texture2D( BaseTexture, Out_Tex0 ).rgba + vSpecular.rgba;

    vSpecular = texture2D( LightingTexture, vLightingTexCoord );
    gl_FragColor.rgb = vDiffuse.rgb * texture2D( BaseTexture, Out_Tex0 ).rgb + (vSpecular.rgb * g_vMaterialSpecular.a);
    gl_FragColor.a   = vDiffuse.a;

//	gl_FragColor.r  = pow( texture2D( LightingTexture, vLightingTexCoord ).r, 0.810 );
//	gl_FragColor.g  = pow( texture2D( LightingTexture, vLightingTexCoord ).g, 0.810 );
//	gl_FragColor.b  = pow( texture2D( LightingTexture, vLightingTexCoord ).b, 0.810 );
//	gl_FragColor.a  = g_vMaterialDiffuse.a;
}
