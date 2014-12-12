//--------------------------------------------------------------------------------------
// File: Read3ds.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once

// Wrap everything in a namespace, to prevent name conflicts
namespace FORMAT_3DS
{


const UINT16 FRM_3DS_CHUNK_ID_MAIN_CHUNK    = 0x4d4d;

// Chunks under FRM_3DS_CHUNK_ID_MAIN_CHUNK
const UINT16 FRM_3DS_CHUNK_ID_3DS_VERSION   = 0x0002;
const UINT16 FRM_3DS_CHUNK_ID_EDIT_3DS      = 0x3d3d;
const UINT16 FRM_3DS_CHUNK_KEYFRAMER        = 0xb000;

// Chunks under FRM_3DS_CHUNK_ID_EDIT_OBJECT
const UINT16 FRM_3DS_CHUNK_ID_MESH_VERSION  = 0x3d3e;
const UINT16 FRM_3DS_CHUNK_ID_EDIT_MATERIAL = 0xafff;
const UINT16 FRM_3DS_CHUNK_ID_ONE_UNIT      = 0x0100;
const UINT16 FRM_3DS_CHUNK_ID_EDIT_OBJECT   = 0x4000;

const UINT16 FRM_3DS_CHUNK_ID_TRIANGLE_OBJECT = 0x4100;
const UINT16 FRM_3DS_CHUNK_ID_VERTEX_LIST = 0x4110;
const UINT16 FRM_3DS_CHUNK_ID_FACE_LIST = 0x4120;
const UINT16 FRM_3DS_CHUNK_ID_MAPPING_COORDS = 0x4140;
const UINT16 FRM_3DS_CHUNK_ID_LOCAL_AXIS = 0x4160;
const UINT16 FRM_3DS_CHUNK_ID_TEXTURE_INFO = 0x4170;
const UINT16 FRM_3DS_CHUNK_ID_MATERIAL_GROUP = 0x4130;
const UINT16 FRM_3DS_CHUNK_ID_SMOOTH_GROUP = 0x4150;
const UINT16 FRM_3DS_CHUNK_ID_MAT_NAME = 0xa000;
const UINT16 FRM_3DS_CHUNK_ID_MAT_AMBIENT = 0xa010;
const UINT16 FRM_3DS_CHUNK_ID_MAT_DIFFUSE = 0xa020;
const UINT16 FRM_3DS_CHUNK_ID_MAT_SPECUAR = 0xa030;
const UINT16 FRM_3DS_CHUNK_ID_MAT_SHININESS = 0xa040;
const UINT16 FRM_3DS_CHUNK_ID_MAT_SHIN2PCT = 0xa041;
const UINT16 FRM_3DS_CHUNK_ID_MAT_SHIN3PCT = 0xa042;
const UINT16 FRM_3DS_CHUNK_ID_MAT_TRANSPARENCY = 0xa050;
const UINT16 FRM_3DS_CHUNK_ID_MAT_EMISSION = 0xa080;
const UINT16 FRM_3DS_CHUNK_ID_MAT_TEXMAP = 0xa200;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_NAME = 0xa300;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_TILING = 0xa351;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_U_SCALE = 0xa354;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_V_SCALE = 0xa356;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_U_OFFSET = 0xa358;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_V_OFFSET = 0xa35a;
const UINT16 FRM_3DS_CHUNK_ID_MAT_MAP_V_ANG = 0xa35c;
const UINT16 FRM_3DS_CHUNK_ID_COLOR_F = 0x0010;
const UINT16 FRM_3DS_CHUNK_ID_COLOR_24 = 0x0011;
const UINT16 FRM_3DS_CHUNK_ID_INT16_PERCENTAGE   = 0x0030;
const UINT16 FRM_3DS_CHUNK_ID_FLOAT32_PERCENTAGE = 0x0031;


#pragma pack(push)
#pragma pack(1)

#pragma warning( disable : 4200 )

struct FRM_3DS_CHUNK
{
    UINT16 m_nID;
    UINT32 m_nLength;
    CHAR   m_Data[0];
};


struct FRM_3DS_MATERIAL
{
    CHAR       m_strName[32];
    FRMVECTOR4 m_vAmbient;
    FRMVECTOR4 m_vDiffuse;
    FRMVECTOR4 m_vSpecular;
    FRMVECTOR4 m_vEmission;
    FLOAT32    m_fShininess;
    CHAR       m_strTexture[32];
};


struct FRM_3DS_GROUP
{
    CHAR     m_strName[32];
    INT32    m_nMaterialIndex;
    INT32    m_nStart;
    INT32    m_nNumTris;
    INT32*   m_tris;   // pTriListIndices?
};

struct FRM_3DS_TANGENT_SPACE
{
    FRMVECTOR3 vTangent;
    FRMVECTOR3 vBinormal;
    FRMVECTOR3 vNormal;
};

struct FRM_3DS_MESH
{
    CHAR           m_strName[32];
    INT32          m_nNumVertices;
    INT32          m_nNumTexCoords;
    INT32          m_nNumTriangles;
    INT32          m_nNumGroups;
    FRMVECTOR3*    m_pPositions;
    FRMVECTOR3*    m_pNormals;
    FRMVECTOR2*    m_pTexCoords;
    FRM_3DS_TANGENT_SPACE* m_pTangentSpace;
    INT32          (*m_pIndices)[3];
    FRM_3DS_GROUP* m_pGroups;
    INT32*         m_smooth;
    FRMVECTOR3     m_vAxes[3];
    FRMVECTOR3     m_vPosition;
    FRMVECTOR3     m_vMin;
    FRMVECTOR3     m_vMax;
    FRMVECTOR3     m_vCenter;

    VOID CalculateMeshBoundingBox();
    VOID CalculateMeshNormals();
    VOID CalculateMeshTangentSpace();
    VOID SmoothMeshTangentSpace();
    VOID RemoveDegenerates();
    VOID SortTriangles();
    VOID Destroy();
};


struct FRM_3DS_MODEL
{
    INT32             m_nNumMaterials;
    INT32             m_nNumMeshes;
    INT32             m_nNumVertices;
    INT32             m_nNumTriangles;
    FRM_3DS_MATERIAL* m_pMaterials;
    FRM_3DS_MESH*     m_pMeshes;

    FRMVECTOR3        m_vMin;
    FRMVECTOR3        m_vMax;
    FRMVECTOR3        m_vCenter;

    VOID Calculate3dsBoundingBox();
    VOID Calculate3dsNormals();
    VOID Calculate3dsTangentSpace();
    VOID Destroy();
};

#pragma pack(pop)

BOOL Read3dsFileFast( const CHAR* strFilename, FRM_3DS_MODEL* pModel );


//--------------------------------------------------------------------------------------
// Return codes
//--------------------------------------------------------------------------------------
#define INDEX_OK                 0
#define INDEX_MEMORY_ERROR      -2
#define INDEX_OUT_OF_RANGE      -3
#define INDEX_NOT_FOUND         -4


//--------------------------------------------------------------------------------------
// Typedefs
//--------------------------------------------------------------------------------------
typedef int (*FRM_3DS_INDEX_SORT_FUNC)( const VOID*, const VOID* );


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
struct FRM_3DS_INDEX
{
public:
	VOID  Initialize( CHAR* pArray, INT32 nSize, INT32 nCount,
		              FRM_3DS_INDEX_SORT_FUNC fnSortFunc );
	INT32 GetCount() { return m_nCount; }
	INT32 Find( VOID* pElement, INT32* pResult );
	VOID  Free();

protected:
	INT32 Append( INT32 nValue, INT32 nPosition );
	INT32 Insert( INT32 nValue, INT32 nPosition );
	INT32 SortedInsert( INT32 nValue );
	INT32 CheckAlloc();

protected:
	INT32       m_nCount;
	INT32       m_nAllocCount;
	INT32*      m_pIndices;
	CHAR*       m_pData;
	INT32       m_nDataSize;
	FRM_3DS_INDEX_SORT_FUNC m_fnSortFunc;
};


// End of namespace
}