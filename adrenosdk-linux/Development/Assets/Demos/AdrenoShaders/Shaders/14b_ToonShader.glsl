//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[ToonShader14bVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

// outputs
varying vec3    g_vNormalES;
varying vec3    g_vViewVecES;

void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Transform object-space normals to eye-space
    vec3 vNormalES = g_matNormal * g_vNormalOS;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = normalize(vNormalES.xyz);
    g_vViewVecES = normalize(vec3( 0.0, 0.0, 0.0 ) - vPositionES.xyz);
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ToonShader14bFS]
#include "CommonFS.glsl"

// inputs from the vs
varying vec3     g_vViewVecES;
varying vec3     g_vNormalES;

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL  g_Material;
uniform vec3      g_vLightPos;
uniform sampler2D g_ColorRamp;
uniform float     g_EdgeBias;
/*
void main()
{
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // calculate general intensity
    float diffuse = dot( vView, vNormal );
 	float specular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );
    float intensity = saturate( (diffuse + specular) * 0.75 );

    // use the color ramp texture to create a banded look
    vec2 colorRampUV = vec2( saturate( intensity ), 0.0);

    // fake an outline by highlighting surface tangents that are near orthogonal to the view
    float edge = pow( saturate( dot( vec3( 0.0, 0.0, 1.0 ), vNormal ) ), g_EdgeBias );
    if( edge > 0.1 )
    {
        edge = 1.0;
    }

    vec4 toonColor = vec4( texture2D( g_ColorRamp, colorRampUV ).rgb * edge, 1.0 );
    gl_FragColor = toonColor;
}
*/
void main()
{
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vHalf   = normalize( g_vLightPos + g_vViewVecES );

    // Calculate general intensity
    float diffuse = dot( g_vViewVecES, vNormal );
 	float specular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );
    float intensity = saturate( ( diffuse + specular ) * 0.75 );

    // use the color ramp texture to create a banded look
    vec2 colorRampUV = vec2( saturate( intensity ), 0.0);

    // fake an outline by highlighting surface tangents that are near orthogonal to the view
    float edge = pow( saturate( dot( vec3( 0.0, 0.0, 1.0 ), vNormal ) ), g_EdgeBias );
    if( edge > 0.1 )
    {
        edge = 1.0;
    }

    vec4 toonColor = vec4( texture2D( g_ColorRamp, colorRampUV ).rgb * edge, 1.0 );
    gl_FragColor = toonColor;
}

