//--------------------------------------------------------------------------------------
// File: ObjImporter.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <map>


// Wrap everything in a namespace, to prevent name conflicts
namespace OBJ_FORMAT
{


struct OBJ_IVERTEX
{
    INT32 nPositionIndex;
    INT32 nNormalIndex;
    INT32 nTexcoordIndex;
};


struct OBJ_FACE
{
    OBJ_IVERTEX VertexIndices[3];
};


struct OBJ_VERTEX
{
    FLOAT32 px, py, pz;
    FLOAT32 nx, ny, nz;
    FLOAT32 tu, tv;
};


struct OBJ_GROUP
{
    std::vector<OBJ_FACE> Faces;
    std::vector<INT16>    Indices;
    std::string           strMaterialName;
};


struct OBJ_MATERIAL
{
    CHAR       strDiffuseTexture[64];
    CHAR       strAmbientTexture[64];
    CHAR       strSpecularTexture[64];
    CHAR       strAlphaTexture[64];

    FRMVECTOR4 vDiffuseColor;
    FRMVECTOR4 vSpecularColor;
    FRMVECTOR3 vAmbientColor;
};


//--------------------------------------------------------------------------------------
// Name: class CMesh
// Desc: 
//--------------------------------------------------------------------------------------
class CMesh
{
public:
    std::vector<FRMVECTOR3>             m_Positions;
    std::vector<FRMVECTOR3>             m_Normals;
    std::vector<FRMVECTOR2>             m_Texcoords;
    std::vector<OBJ_GROUP>              m_Groups;
    std::vector<OBJ_VERTEX>             m_Vertices;
    std::map<std::string, OBJ_MATERIAL> m_Materials;

    CHAR               m_strName[64];
    UINT32             m_nNumTextures;
    UINT32             m_nCurrentGroup;

    BOOL   BuildUniqueVertices();
    UINT16 InsertVertex( const OBJ_IVERTEX& VertexReference );
    BOOL   FindVertex( const OBJ_VERTEX& VertexReference, UINT16& nIndex );
    BOOL   ReadMtlFile( const CHAR* strFileName );

public:
    CMesh() {}
    ~CMesh() {}

    BOOL Load( const CHAR* strFileName );
};


// End of namespace
}