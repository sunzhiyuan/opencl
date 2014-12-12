//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[GlassShaderVS]
#include "CommonVS.glsl"

uniform mat4  g_MatModelViewProj;
uniform mat4  g_MatModel;
uniform mat3  g_MatNormal;
uniform vec3  g_LightPos;
uniform vec3  g_EyePos;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_NormalIn;

varying vec4 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;


void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;
    vec3 Normal    = g_NormalIn;

    gl_Position = g_MatModelViewProj * Position;
    g_TexCoord = vec4( TexCoord, ( gl_Position.xy / gl_Position.w ) * 0.5 + 0.5 );

    // calculate light and view vectors.  everything in worldspace.
    vec4 WorldPos = g_MatModel * Position;
    g_LightVec    = g_LightPos - WorldPos.xyz;
    g_ViewVec     = g_EyePos   - WorldPos.xyz;

    g_Normal = g_MatNormal * Normal;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[GlassShaderFS]
#include "CommonFS.glsl"

uniform sampler2D g_BackgroundTexture;
uniform vec4 g_GlassParams;

varying vec4 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;


void main()
{
    vec2 UV             = g_TexCoord.xy;
    vec2 ScreenUV       = g_TexCoord.zw * 1.0;
    vec3 Normal         = normalize( g_Normal );
    vec3 Light          = normalize( g_LightVec );
    vec3 View           = normalize( g_ViewVec );
    vec3 Half           = normalize( Light + View );
    float NV            = max( 0.0, dot( Normal, View ) );
    vec3 BendVec        = reflect( -View, Normal );
    vec3 GlassColor     = g_GlassParams.xyz;
    float Distort       = g_GlassParams.w;

    vec4 BGColor    = texture2D( g_BackgroundTexture, ScreenUV - BendVec.xy * Distort );
    vec4 EnvColor   = texture2D( g_BackgroundTexture, ScreenUV + BendVec.xy * Distort );

    // Fresnel
    float F = saturate( pow( ( 1.0 - NV ), 2.0 ) );

    // Specular
    float Specular  = pow( saturate( dot( Half, Normal ) ), 100.0 ) * 4.0;

    // refraction + env + spec
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    gl_FragColor.rgb += BGColor.rgb * GlassColor * ( 1.0 - F );
    gl_FragColor.rgb += ( EnvColor.rgb + GlassColor * 0.5 ) * F;
    gl_FragColor.rgb += vec3( Specular * 0.7 );
}
