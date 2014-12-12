//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[HatchShaderVS]
#include "CommonVS.glsl"

// inputs
attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexCoord;

// shader variables
uniform mat4 g_matModelView;
uniform mat4 g_matModelViewProj;
uniform mat3 g_matNormal;

// outputs to the fs
varying vec3 g_vNormalES;
varying vec4 g_vOutTexCoord;

void main()
{
    // Basic transforms
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;
    vec3 vNormalES = g_matNormal * g_vNormalOS;

    // Generate hatch uv's procedurally
    vec2 HatchUV = vec2( g_vPositionOS.x * 12.1 + sin( g_vPositionOS.y * 19.34 ), g_vPositionOS.y * 9.3 + g_vPositionOS.z * 13.8 + cos( g_vPositionOS.x * 13.11 ) );

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = normalize(vNormalES.xyz);
    g_vOutTexCoord = vec4( g_vTexCoord.x, g_vTexCoord.y, HatchUV.x, HatchUV.y );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[HatchShaderFS]
#include "CommonFS.glsl"

// inputs from the vs
varying vec3 g_vNormalES;
varying vec4 g_vOutTexCoord;

// shader variables
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_HatchTexture1;
uniform sampler2D g_HatchTexture2;
uniform vec3      g_LightPos;
uniform float     g_Ambient;

void main()
{
   // vec3 vNormal = normalize( g_vNormalES );
   // vec3 vLight = normalize( g_LightPos );
    vec4 HatchTex1 = texture2D( g_HatchTexture1, g_vOutTexCoord.zw );
    vec4 HatchTex2 = texture2D( g_HatchTexture2, g_vOutTexCoord.zw );
    vec4 DiffuseTex = texture2D( g_DiffuseTexture, g_vOutTexCoord.xy );
    float Diffuse = dot( g_LightPos, g_vNormalES );

    // calculate luminance
    float sum = saturate( saturate( Diffuse ) + g_Ambient );

    // maps tones to channels in the hatch textures
    vec4 HatchWeights1 = vec4( 1.0, 0.857, 0.714, 0.571 );
    vec4 HatchWeights2 = vec4( 0.429, 0.286, 0.143, 0.0 );

    // normalize to within 0.125 from each luminance anchor
    vec4 x = vec4( 1.0 ) - saturate( abs( ( HatchWeights1 - vec4( sum ) ) * vec4( 7.0 ) ) );
    vec4 y = vec4( 1.0 ) - saturate( abs( ( HatchWeights2 - vec4( sum ) ) * vec4( 7.0 ) ) );

    // sum up all pixel weights
    sum = saturate( dot( HatchTex1, x ) + dot( HatchTex2, y ) );

    // reduce contrast + boost luminance
    sum = sum * (0.6) + 0.35;

    // color tint
    gl_FragColor = vec4( sum, sum, sum, 1.0 );
}
