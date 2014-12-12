//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[MaterialShader46VS]
#include "CommonVS.glsl"

uniform mat4  g_MatModelViewProj;
uniform mat4  g_MatModel;
uniform mat4  g_MatNormal;
uniform vec3  g_LightPos;
uniform vec3  g_EyePos;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_NormalIn;

varying vec2 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;

void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;
    vec3 Normal    = g_NormalIn;

    gl_Position = g_MatModelViewProj * Position;
    g_Normal    = ( g_MatNormal * vec4( Normal, 1.0 ) ).xyz;
    g_TexCoord  = TexCoord;

    vec4 WorldPos = g_MatModel * Position;
    g_LightVec = g_LightPos - WorldPos.xyz;
    g_ViewVec  = g_EyePos   - WorldPos.xyz;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MaterialShader46FS]
#include "CommonFS.glsl"

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
