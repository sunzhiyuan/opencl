

//--------------------------------------------------------------------------------------
// File: PerVertexLighting.glsl
// Desc: Shaders for per-vertex lighting
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
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



struct MATERIAL
{
    vec3  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

const int MAX_BONES = 26;

uniform   vec4     g_matWorldArray[3*MAX_BONES];
uniform   mat4     g_matView;
uniform   mat4     g_matProj;

uniform   MATERIAL g_Material;

attribute vec4  In_Position;
attribute vec4  In_BoneIndices;
attribute float In_BoneWeights;
attribute vec3  In_Normal;
attribute vec2  In_TexCoord;

varying   vec4  Out_Diffuse;
varying   vec2  Out_Tex0;


//--------------------------------------------------------------------------------------
// Helper functions for transforming a vec3/vec4 by a bone
//--------------------------------------------------------------------------------------
vec3 MulBone3( vec3 vInputPos, int nMatrix, float fBlendWeight )
{
    vec3 vResult;
    vResult.x = dot( vInputPos, g_matWorldArray[3*nMatrix+0].xyz );
    vResult.y = dot( vInputPos, g_matWorldArray[3*nMatrix+1].xyz );
    vResult.z = dot( vInputPos, g_matWorldArray[3*nMatrix+2].xyz );
    return vResult * fBlendWeight;
}

vec3 MulBone4( vec4 vInputPos, int nMatrix, float fBlendWeight )
{
    vec3 vResult;
    vResult.x = dot( vInputPos, g_matWorldArray[(3*nMatrix)+0].xyzw );
    vResult.y = dot( vInputPos, g_matWorldArray[(3*nMatrix)+1].xyzw );
    vResult.z = dot( vInputPos, g_matWorldArray[(3*nMatrix)+2].xyzw );
    return vResult * fBlendWeight;
}


//--------------------------------------------------------------------------------------
// Vertex shader entry point
//--------------------------------------------------------------------------------------
void main()
{
    vec2 BoneWeights;
    BoneWeights.x = In_BoneWeights;
    BoneWeights.y = 1.0 - In_BoneWeights;

    ivec4 BoneIndices;
    BoneIndices.x = int(In_BoneIndices.x);
    BoneIndices.y = int(In_BoneIndices.y);

    vec3 vPosition;
#ifdef USE_TWO_BONES
    vPosition  = MulBone4( In_Position, BoneIndices.x, BoneWeights.x );
    vPosition += MulBone4( In_Position, BoneIndices.y, BoneWeights.y );
#else
    vPosition  = MulBone4( In_Position, BoneIndices.x, 1.0 );
#endif

    vec3 vNormal;
#ifdef USE_TWO_BONES
    vNormal  = MulBone3( In_Normal, BoneIndices.x, BoneWeights.x );
    vNormal += MulBone3( In_Normal, BoneIndices.y, BoneWeights.y ); 
#else
    vNormal  = MulBone3( In_Normal, BoneIndices.x, 1.0 );
#endif
    vNormal = normalize( vNormal );

    vec4 vPositionWS = vec4( vPosition.xyz, 1.0 );
    vec4 vPositionES = g_matView * vPositionWS;
    vec4 vPositionCS = g_matProj * vPositionES;

    // Output clip-space position
    gl_Position = vPositionCS;

    // Compute the lighting
    vec3  vLight   = normalize( vec3( 0.0, 0.5, 1.0 ) );
    float fDiffuse = max( 0.0, dot( vNormal, vLight ) );

    // Combine lighting with the material properties
    vec3 vAmbient = g_Material.vAmbient;
    vec4 vDiffuse = g_Material.vDiffuse.a != 0.0 ? g_Material.vDiffuse.rgba : vec4(1.0);
    Out_Diffuse.rgba  = vec4( vAmbient.rgb, 0.0 );
    Out_Diffuse.rgba += vDiffuse.rgba * fDiffuse;

    Out_Tex0 = vec2( In_TexCoord.x, In_TexCoord.y );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
