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
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Adreno SDK
// 
// Copyright (c) 2009 QUALCOMM Incorporated.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix


//--------------------------------------------------------------------------------------
// External constants
//--------------------------------------------------------------------------------------
uniform vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

// Matrix palette
uniform mat3 g_matNormal;
uniform mat4 g_matWorld;
uniform mat4 g_matView;
uniform mat4 g_matProj;


//--------------------------------------------------------------------------------------
// Vertex structures
//--------------------------------------------------------------------------------------
attribute vec4  In_Pos;
attribute vec3  In_Normal;
attribute vec2  In_Tex0;

varying vec4  Out_Diffuse;
varying vec4  Out_Specular;
varying vec2  Out_Tex0;

varying	vec3  Out_vNormal;
//varying	vec3  Out_vHalf;


//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------
void main()
{
    // Transform position into clip space
    vec4 vPositionOS = In_Pos.xyzw;
    vec4 vPositionWS = g_matWorld * vPositionOS;
    vec4 vPositionES = g_matView * vPositionWS;
    vec4 vPositionCS = g_matProj * vPositionES;
    gl_Position = vPositionCS;

    vec3 vNormalOS = In_Normal;
    vec3 vNormalWS = g_matNormal * vNormalOS;
//	vec3 vNormalWS = g_matWorld * vec4( In_Normal.x, In_Normal.y, In_Normal.z, 0.0 );
    vec4 vNormalES = g_matView * vec4( vNormalWS.x, vNormalWS.y, vNormalWS.z, 0.0 );
//	vec3 vNormal = normalize( vNormalWS );
    vec3 vNormal = normalize( vNormalES.xyz );

    // Compute intermediate vectors
    vec3 vLight  = g_vLightDir;
//  vec3 vView   = normalize( -vPositionES.xyz );
    vec3 vView   = vec3(0.0,0.0,1.0);//normalize( -vPositionES.xyz );
    vec3 vHalf   = normalize( vLight + vView );

    if( true )
    {
        Out_vNormal = vNormal;
//		Out_vHalf   = vHalf;
    }
    else
    {
        // Compute the lighting in eye-space
        float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
        float fSpecular = fDiffuse > 0.0 ? pow( max( 0.0, dot( vNormal, vHalf ) ), g_vMaterialSpecular.a ) : 0.0;

        // Combine lighting with the material properties
        Out_Diffuse.rgb  = g_vMaterialAmbient.rgb + g_vMaterialDiffuse.rgb * fDiffuse;
        Out_Diffuse.a    = g_vMaterialDiffuse.a;

        Out_Specular.rgb = g_vMaterialSpecular.rgb * fSpecular;
        Out_Specular.a   = 0.0;//fSpecular;
    }

    // Copy the input texture coordinate through
    Out_Tex0.x  = In_Tex0.x;
    Out_Tex0.y  = 1.0 - In_Tex0.y;
}


