//--------------------------------------------------------------------------------------
// ReadMsh1.h
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once


// Wrap everything in a namespace, to prevent name conflicts
namespace MSH1_FORMAT
{


//--------------------------------------------------------------------------------------
// Name: enum FRM_VERTEX_USAGE
// Desc: 
//--------------------------------------------------------------------------------------
enum FRM_VERTEX_USAGE
{
    FRM_VERTEX_POSITION,          // 0
    FRM_VERTEX_BONEINDICES,       // 1
    FRM_VERTEX_BONEWEIGHTS,       // 2
    FRM_VERTEX_NORMAL,            // 3
    FRM_VERTEX_TANGENT,           // 4
    FRM_VERTEX_BINORMAL,          // 5
    FRM_VERTEX_TEXCOORD0,         // 6
    FRM_VERTEX_TEXCOORD1,         // 7
    FRM_VERTEX_TEXCOORD2,         // 8
    FRM_VERTEX_TEXCOORD3,         // 9
    FRM_VERTEX_TEXCOORD4,         // 10
    FRM_VERTEX_TEXCOORD5,         // 11
    FRM_VERTEX_COLOR0,            // 12
    FRM_VERTEX_COLOR1,            // 13
    FRM_VERTEX_COLOR2,            // 14
    FRM_VERTEX_COLOR3,            // 15
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_VERTEX_ELEMENT
// Desc: 
//--------------------------------------------------------------------------------------
#pragma pack(push,1)
struct FRM_VERTEX_ELEMENT
{
    UINT16 nUsage;        // FRM_VERTEX_USAGE
    UINT8  nSize;         // Number of components
    UINT16 nType;         // OpenGL data type
    UINT8  bNormalized;   // Whether to normalize the value
    UINT16 nStride;       // Stride of this component
};
#pragma pack(pop)


// Forward references
struct FRM_MESH_FRAME;
struct FRM_ANIMATION;



//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_TEXTURE
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_TEXTURE
{
    CHAR               m_strTexture[32];
    VOID*              m_pTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_BONE
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_BONE
{
    FRM_MESH_FRAME*   m_pFrame;             // The mesh frame associated with this bone
    FRMMATRIX4X4      m_matBoneOffset;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_SUBSET
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_SUBSET
{
    UINT32             m_nPrimType;
    UINT32             m_nNumIndices;
    UINT32             m_nIndexOffset;

    UINT32*            m_pBoneIds;

    FRMVECTOR4         m_vDiffuseColor;
    FRMVECTOR4         m_vSpecularColor;
    FRMVECTOR3         m_vAmbientColor;
    FRMVECTOR3         m_vEmissiveColor;

    UINT32             m_nNumTextures;
    FRM_MESH_TEXTURE*  m_pTextures;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH
{
    FRM_VERTEX_ELEMENT m_VertexLayout[8];
	UINT32             m_Reserved;

    UINT32             m_nNumVertices;
    UINT16             m_nVertexSize;
    union
    {
        UINT32         m_hVertexBuffer;
        VOID*          m_pVertexData;
    };

    UINT32             m_nNumIndices;
    UINT16             m_nIndexSize;
    union
    {
        UINT32         m_hIndexBuffer;
        VOID*          m_pIndexData;
    };

    UINT32             m_nNumBonesPerVertex;
    UINT32             m_nNumBoneMatrices;
    UINT32             m_nNumBones;
    FRM_MESH_BONE*     m_pBones;

	UINT32             m_nNumSubsets;
    FRM_MESH_SUBSET*   m_pSubsets;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_FRAME
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_FRAME
{
    CHAR               m_strName[32];

    FRMMATRIX4X4       m_matTransform;
    FRMMATRIX4X4       m_matBoneOffset2;
    FRMMATRIX4X4       m_matCombined;

    FRM_MESH*          m_pMesh;
    FRM_ANIMATION*     m_pAnimationData;

    FRM_MESH_FRAME*    m_pChild;
    FRM_MESH_FRAME*    m_pNext;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_FILE_HEADER
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_FILE_HEADER
{
    UINT32 nMagic;
    UINT32 nSystemDataSize;
    UINT32 nBufferDataSize;
    UINT32 nNumFrames;
};

const UINT32 FRM_MESH_MAGIC_ID = ( ('M'<<0L) | ('S'<<8L) | ('H'<<16) | (1<<24L) );


//--------------------------------------------------------------------------------------
// Name: class CMesh
// Desc: 
//--------------------------------------------------------------------------------------
class CMesh
{
public:
    CMesh()
    {
        m_pFrames = NULL;
        m_nNumFrames = 0;
    }

    ~CMesh()
    {
        Destroy();
    }

    BOOL Load( const CHAR* strFileName );
    VOID Destroy();

public:
    UINT32          m_nSystemDataSize;
    UINT32          m_nBufferDataSize;
    BYTE*           m_pSystemData;
    BYTE*           m_pBufferData;

    FRM_MESH_FRAME* m_pFrames;
    UINT32          m_nNumFrames;
};


// End of namespace
}

