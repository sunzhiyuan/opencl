//--------------------------------------------------------------------------------------
// ReadMsh0.h
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once


// Wrap everything in a namespace, to prevent name conflicts
namespace MSH0_FORMAT
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


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_TEXTURE
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_TEXTURE
{
    CHAR         m_strTexture[64];
    VOID*        m_pTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_SUBSET
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_SUBSET
{
    FRM_MESH_TEXTURE* m_pTextures;
    UINT32       m_nNumTextures;

    FRMVECTOR4   m_vDiffuseColor;
    FRMVECTOR4   m_vSpecularColor;
    FRMVECTOR3   m_vAmbientColor;
    FRMVECTOR3   m_vEmissiveColor;

    UINT32       m_nPrimType;
    UINT32       m_nNumIndices;
    UINT32       m_nIndexOffset;
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_BASE
{
    UINT32             m_nNumVertices;
    UINT32             m_nNumIndices;
    FRM_VERTEX_ELEMENT m_VertexLayout[8+1]; // Space for MAX_VERTEX_ATTRIBS plus NULL terminator
    UINT32             m_hVertexBuffer;
    UINT32             m_hIndexBuffer;
    UINT16             m_nVertexSize;
    UINT16             m_nIndexSize;
    UINT32             m_nNumSubsets;
};

struct FRM_MESH : public FRM_MESH_BASE
{
    FRM_MESH_SUBSET    m_pSubsets[1];
};


//--------------------------------------------------------------------------------------
// Name: struct FRM_MESH_FRAME
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_MESH_FRAME
{
    CHAR            m_strName[64];
    FRMMATRIX4X4    m_matTransform;
    FRM_MESH*       m_pMesh;
    FRM_MESH_FRAME* m_pChild;
    FRM_MESH_FRAME* m_pNext;
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

const UINT32 FRM_MESH_MAGIC_ID = (((UINT32)'M'<< 0)|(((UINT32)'S'<< 8))|(((UINT32)'H'<<16))|(0<<24));


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

