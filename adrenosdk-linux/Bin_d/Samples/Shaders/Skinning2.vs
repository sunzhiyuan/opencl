

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



const int MAX_BONES = 80;

uniform vec4 g_matWorldArray[MAX_BONES * 3];
uniform mat4 g_matView;
uniform mat4 g_matProj;
uniform vec3 g_lightPos;
uniform vec3 g_cameraPos;

attribute vec4 In_Position;
attribute float In_BoneIndex1;
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
attribute float In_BoneIndex2;
#endif
#if defined( USE_THREE_BONES )
attribute float In_BoneIndex3;
#endif
attribute vec3 In_BoneWeights;
attribute vec3 In_Normal;
attribute vec3 In_Tangent;
attribute vec3 In_Binormal;
attribute vec2 In_TexCoord;

varying vec2 Out_Tex0;
varying vec3 Out_Light;
varying vec3 Out_EyeNormal;


//--------------------------------------------------------------------------------------
// Helper functions for transforming a vec3/vec4 by a bone
//--------------------------------------------------------------------------------------
vec3 MulBone3( vec3 vInputPos, int nBone, float fBlendWeight )
{
	vec3 vResult;
	
	int nMatrix = 3 * nBone;
	
    vResult.x = dot( vInputPos, g_matWorldArray[nMatrix + 0].xyz );
    vResult.y = dot( vInputPos, g_matWorldArray[nMatrix + 1].xyz );
    vResult.z = dot( vInputPos, g_matWorldArray[nMatrix + 2].xyz );
    
    return vResult * fBlendWeight;
}

vec3 MulBone4( vec4 vInputPos, int nBone, float fBlendWeight )
{
    vec3 vResult;
    
    int nMatrix = 3 * nBone;
    
    vResult.x = dot( vInputPos, g_matWorldArray[nMatrix + 0].xyzw );
    vResult.y = dot( vInputPos, g_matWorldArray[nMatrix + 1].xyzw );
    vResult.z = dot( vInputPos, g_matWorldArray[nMatrix + 2].xyzw );
    
    return vResult * fBlendWeight;
}

//--------------------------------------------------------------------------------------
// Vertex shader entry point
//--------------------------------------------------------------------------------------
void main()
{
	// Get bone indices
    ivec3 BoneIndices;
    BoneIndices.x = int( In_BoneIndex1 );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    BoneIndices.y = int( In_BoneIndex2 );
#endif
#if defined( USE_THREE_BONES )
    BoneIndices.z = int( In_BoneIndex3 );
#endif

	// Transform position
    vec3 vPosition = MulBone4( In_Position, BoneIndices.x, In_BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vPosition += MulBone4( In_Position, BoneIndices.y, In_BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vPosition += MulBone4( In_Position, BoneIndices.z, In_BoneWeights.z );
#endif
    
    // Transform normal
    vec3 vNormal = MulBone3( In_Normal, BoneIndices.x, In_BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vNormal += MulBone3( In_Normal, BoneIndices.y, In_BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vNormal += MulBone3( In_Normal, BoneIndices.z, In_BoneWeights.z );
#endif
	vNormal = normalize( vNormal );
	
	// Transform tangent
	vec3 vTangent = MulBone3( In_Tangent, BoneIndices.x, In_BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vTangent += MulBone3( In_Tangent, BoneIndices.y, In_BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vTangent += MulBone3( In_Tangent, BoneIndices.z, In_BoneWeights.z );
#endif
	vTangent = normalize( vTangent );
	
	// Transform binormal
	vec3 vBinormal = MulBone3( In_Binormal, BoneIndices.x, In_BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vBinormal += MulBone3( In_Binormal, BoneIndices.y, In_BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vBinormal += MulBone3( In_Binormal, BoneIndices.z, In_BoneWeights.z );
#endif
	vBinormal = normalize( vBinormal );
	
	// Apply view and projection to position
    vec4 vPositionWS = vec4( vPosition.xyz, 1.0 );
    vec4 vPositionES = g_matView * vPositionWS;
    vec4 vPositionCS = g_matProj * vPositionES;
    gl_Position = vPositionCS;
    
    // Pass light and eye normal
    mat3 TBNMatrix = mat3( vTangent, vBinormal, vNormal );
    Out_Light = ( g_lightPos - vPosition.xyz ) * TBNMatrix;
    Out_EyeNormal = ( g_cameraPos - vPosition.xyz ) * TBNMatrix;

	// Pass texture coordinates
    Out_Tex0 = vec2( In_TexCoord.x, In_TexCoord.y );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
