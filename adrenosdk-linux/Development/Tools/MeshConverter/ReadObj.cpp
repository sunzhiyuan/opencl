//--------------------------------------------------------------------------------------
// ObjImporter.cpp
//
// Imports an .obj file to the mesh format.
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if defined (_WIN32)
#include <windows.h>
#elif defined (__linux__)
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#define _MAX_EXT NAME_MAX
#define _MAX_FNAME NAME_MAX
#define stricmp strcasecmp
#endif

#include <cstdio>
#include <string>
#include <cstdlib>
#include <string>
#include "FrmExtendedMesh.h"
#include "ReadObj.h"
#include "ReadMsh1.h"


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL ReadObj( const CHAR* strFileName, CFrmExtendedMesh* pDstMesh )
{
    // Check for the .obj file extension
    CHAR strExt[_MAX_EXT];
    CHAR strName[_MAX_FNAME];
#if defined (_WIN32)
    _splitpath( strFileName, NULL, NULL, strName, strExt );
#elif defined (__linux__)
    strcpy(strExt, strrchr(strFileName, '.'));
    strcpy(strName, basename((char*)strFileName));
    char *ext = strrchr(strName, '.');
    *ext = '\0';
#endif
    if( 0 != stricmp( strExt, ".obj" ) )
        return FALSE;

    // Try to load the mesh as an OBJ file
    OBJ_FORMAT::CMesh srcMesh;
    if( FALSE == srcMesh.Load( strFileName ) )
        return FALSE;

    // Populate the extended mesh
    {
        // Allocate and initialize frames
        {
            pDstMesh->m_nNumFrames      = 1;
            pDstMesh->m_pFrames         = new MSH1_FORMAT::FRM_MESH_FRAME[1];

            MSH1_FORMAT::FRM_MESH_FRAME* pFrame = &pDstMesh->m_pFrames[0];
            memset( pFrame, 0, sizeof(MSH1_FORMAT::FRM_MESH_FRAME) );
            strcpy( pFrame->m_strName, strName );
            strcat( pFrame->m_strName, strExt );
            pFrame->m_matTransform = FrmMatrixIdentity();
            pFrame->m_pMesh        = new MSH1_FORMAT::FRM_MESH;
            pFrame->m_pChild       = NULL;
            pFrame->m_pNext        = NULL;
        }

        // Allocate and initialize meshes
        {
            MSH1_FORMAT::FRM_MESH* pMesh  = pDstMesh->m_pFrames[0].m_pMesh;
            memset( pMesh, 0, sizeof(MSH1_FORMAT::FRM_MESH) );

            // Build vertex layout
            {
                MSH1_FORMAT::FRM_VERTEX_ELEMENT* pVertexElement = pMesh->m_VertexLayout;
                if( srcMesh.m_Positions.size() )
                {
                    pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_POSITION;
                    pVertexElement->nSize       = 3;
                    pVertexElement->nType       = GL_FLOAT;
                    pVertexElement->bNormalized = FALSE;
                    pVertexElement->nStride     = sizeof(FRMVECTOR3);
                    pVertexElement++;
                }

                if( srcMesh.m_Normals.size() )
                {
                    pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_NORMAL;
                    pVertexElement->nSize       = 3;
                    pVertexElement->nType       = GL_FLOAT;
                    pVertexElement->bNormalized = FALSE;
                    pVertexElement->nStride     = sizeof(FRMVECTOR3);
                    pVertexElement++;
                }
                if( srcMesh.m_Texcoords.size() )
                {
                    pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_TEXCOORD0;
                    pVertexElement->nSize       = 2;
                    pVertexElement->nType       = GL_FLOAT;
                    pVertexElement->bNormalized = FALSE;
                    pVertexElement->nStride     = sizeof(FRMVECTOR2);
                    pVertexElement++;
                }
            }

            // Build vertex buffer
            {
                UINT32 nNumVertices = srcMesh.m_Vertices.size();
                UINT32 nVertexSize  = ( srcMesh.m_Positions.size() ? sizeof(FRMVECTOR3) : 0 ) +
                                      ( srcMesh.m_Normals.size()   ? sizeof(FRMVECTOR3) : 0 ) +
                                      ( srcMesh.m_Texcoords.size() ? sizeof(FRMVECTOR2) : 0 );

                pMesh->m_nVertexSize   = nVertexSize;
                pMesh->m_nNumVertices  = nNumVertices;
                pMesh->m_pVertexData   = new BYTE[nVertexSize * nNumVertices];

                FLOAT32* pVertexData = (FLOAT32*)pMesh->m_pVertexData;
                for( UINT32 i = 0; i < nNumVertices; ++i )
                {
                    if( srcMesh.m_Positions.size() )
                    {
                        *pVertexData++ = srcMesh.m_Vertices[i].px;
                        *pVertexData++ = srcMesh.m_Vertices[i].py;
                        *pVertexData++ = srcMesh.m_Vertices[i].pz;
                    }
                    if( srcMesh.m_Normals.size() )
                    {
                        *pVertexData++ = srcMesh.m_Vertices[i].nx;
                        *pVertexData++ = srcMesh.m_Vertices[i].ny;
                        *pVertexData++ = srcMesh.m_Vertices[i].nz;
                    }
                    if( srcMesh.m_Texcoords.size() )
                    {
                        *pVertexData++ = srcMesh.m_Vertices[i].tu;
                        *pVertexData++ = srcMesh.m_Vertices[i].tv;
                    }
                }
            }

            // Build index buffer
            {
                INT32  nNumIndices = 0;
                UINT32 nIndexSize  = sizeof(UINT16);
                UINT32 nNumGroups  = srcMesh.m_Groups.size();
                for( UINT32 i = 0; i < nNumGroups; ++i )
                    nNumIndices += srcMesh.m_Groups[i].Indices.size();

                pMesh->m_nNumIndices = nNumIndices;
                pMesh->m_nIndexSize  = nIndexSize;
                pMesh->m_pIndexData  = new BYTE[nIndexSize * nNumIndices];
                BYTE* pIndexData = (BYTE*)pMesh->m_pIndexData;
                for( UINT32 i = 0; i < nNumGroups; ++i )
                {
                    memcpy( pIndexData, &srcMesh.m_Groups[i].Indices[0], srcMesh.m_Groups[i].Indices.size() * nIndexSize );
                    pIndexData += srcMesh.m_Groups[i].Indices.size() * nIndexSize;
                }
            }

            // Allocate and build mesh subsets
            {
                UINT32 nIndexSize  = sizeof(UINT16);
                UINT32 nNumGroups  = srcMesh.m_Groups.size();

                pMesh->m_nNumSubsets = nNumGroups;
                pMesh->m_pSubsets    = new MSH1_FORMAT::FRM_MESH_SUBSET[ nNumGroups ];

                UINT32 nIndexOffset = 0;

                for( UINT32 i = 0; i < nNumGroups; ++i )
                {
                    MSH1_FORMAT::FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[i];
                    memset( pSubset, 0, sizeof(MSH1_FORMAT::FRM_MESH_SUBSET) );
    
                    pSubset->m_nPrimType    = GL_TRIANGLES;
                    pSubset->m_nNumIndices  = srcMesh.m_Groups[i].Indices.size();
                    pSubset->m_nIndexOffset = nIndexOffset;
                    nIndexOffset += srcMesh.m_Groups[i].Indices.size() + nIndexSize;
    
                    pSubset->m_nNumTextures = 0;
                    pSubset->m_pTextures    = NULL; // new FRM_MESH_TEXTURE[pSubset->m_nNumTextures];

                    if( srcMesh.m_Materials.find( srcMesh.m_Groups[i].strMaterialName ) != srcMesh.m_Materials.end() )
                    {
                        OBJ_FORMAT::OBJ_MATERIAL Material = srcMesh.m_Materials[srcMesh.m_Groups[i].strMaterialName];

                        UINT32 nNumTextures = 0;
                        if( strlen( Material.strDiffuseTexture )  > 0 )   nNumTextures++;
                        if( strlen( Material.strAmbientTexture )  > 0 )   nNumTextures++;
                        if( strlen( Material.strSpecularTexture ) > 0 )   nNumTextures++;
                        if( strlen( Material.strAlphaTexture )    > 0 )   nNumTextures++;

                        pSubset->m_pTextures = new MSH1_FORMAT::FRM_MESH_TEXTURE[nNumTextures];
                        memset( pSubset->m_pTextures, 0, sizeof(MSH1_FORMAT::FRM_MESH_TEXTURE) * nNumTextures );

                        if( strlen( Material.strDiffuseTexture) > 0 )
                            strcpy( pSubset->m_pTextures[pSubset->m_nNumTextures++].m_strTexture, Material.strDiffuseTexture);

                        if( strlen( Material.strAmbientTexture) > 0 )
                            strcpy( pSubset->m_pTextures[pSubset->m_nNumTextures++].m_strTexture, Material.strAmbientTexture);

                        if( strlen( Material.strSpecularTexture) > 0 )
                            strcpy( pSubset->m_pTextures[pSubset->m_nNumTextures++].m_strTexture, Material.strSpecularTexture);

                        if( strlen( Material.strAlphaTexture) > 0 )
                            strcpy( pSubset->m_pTextures[pSubset->m_nNumTextures++].m_strTexture, Material.strAlphaTexture);

                        pSubset->m_vDiffuseColor  = Material.vDiffuseColor;
                        pSubset->m_vAmbientColor  = Material.vAmbientColor;
                        pSubset->m_vSpecularColor = Material.vSpecularColor;
                    }
                }
            }
        }
    }

    return TRUE;
}
    
    
// Wrap everything in a namespace, to prevent name conflicts
namespace OBJ_FORMAT
{


//--------------------------------------------------------------------------------------
// Name: Load()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMesh::Load( const CHAR* strFileName )
{
    FILE* pFile = fopen( strFileName, "r" );
    if( NULL == pFile )
        return FALSE;

    memset( m_strName, 0, sizeof(m_strName) );
    strcpy( m_strName, strFileName );

    m_nCurrentGroup = 0;
    m_nNumTextures  = 0;

    CHAR strLine[255];
    while( fgets( strLine, 255, pFile ) )
    {
        if( strLine[0] == 'v' )
        {
            switch( strLine[1] )
            {
                case ' ':
                {
                    FRMVECTOR3 vPosition;
                    sscanf( strLine, "v %f %f %f", &vPosition.x, &vPosition.y, &vPosition.z );
                    m_Positions.push_back( vPosition );
                    break;
                }

                case 'n':
                {
                    FRMVECTOR3 vNormal;
                    sscanf( strLine, "vn %f %f %f", &vNormal.x, &vNormal.y, &vNormal.z );
                    m_Normals.push_back( vNormal );
                    break;
                }

                case 't':
                {
                    FRMVECTOR2 vTexcoord;
                    sscanf( strLine, "vt %f %f", &vTexcoord.tu, &vTexcoord.tv );
                    m_Texcoords.push_back( vTexcoord );
                    break;
                }
            }
        }
        else if( strLine[0] == 'f' )
        {
            if( 0 == m_Groups.size() )
            {
                // if we're seeing faces before seeing any materials, then assume there's
                // only one group
                OBJ_GROUP group;
                group.strMaterialName = "";
                m_Groups.push_back( group );
                m_nCurrentGroup = 0;
            }

            OBJ_FACE face;
            // only one of the following calls will succeed, based on the string format
            sscanf( strLine, "f %d %d %d",                   &face.VertexIndices[0].nPositionIndex, &face.VertexIndices[1].nPositionIndex, &face.VertexIndices[2].nPositionIndex );
            sscanf( strLine, "f %d/%d %d/%d %d/%d",          &face.VertexIndices[0].nPositionIndex, &face.VertexIndices[0].nTexcoordIndex, &face.VertexIndices[1].nPositionIndex,
                                                             &face.VertexIndices[1].nTexcoordIndex, &face.VertexIndices[2].nPositionIndex, &face.VertexIndices[2].nTexcoordIndex );
            sscanf( strLine, "f %d//%d %d//%d %d//%d",       &face.VertexIndices[0].nPositionIndex, &face.VertexIndices[0].nNormalIndex,   &face.VertexIndices[1].nPositionIndex,
                                                             &face.VertexIndices[1].nNormalIndex,   &face.VertexIndices[2].nPositionIndex, &face.VertexIndices[2].nNormalIndex );
            sscanf( strLine, "f %d/%d/%d %d/%d/%d %d/%d/%d", &face.VertexIndices[0].nPositionIndex, &face.VertexIndices[0].nTexcoordIndex, &face.VertexIndices[0].nNormalIndex,
                                                             &face.VertexIndices[1].nPositionIndex, &face.VertexIndices[1].nTexcoordIndex, &face.VertexIndices[1].nNormalIndex,
                                                             &face.VertexIndices[2].nPositionIndex, &face.VertexIndices[2].nTexcoordIndex, &face.VertexIndices[2].nNormalIndex );
            // Obj indices are 1-based, so need to decrement them
            for( UINT32 i = 0; i < 3; ++i )
            {
                face.VertexIndices[i].nPositionIndex--;
                face.VertexIndices[i].nNormalIndex--;
                face.VertexIndices[i].nTexcoordIndex--;
            }
            m_Groups[m_nCurrentGroup].Faces.push_back( face );
        }
        else if( !strncmp( strLine, "usemtl", 6 ) )
        {
            CHAR strMaterialName[255];
            sscanf( strLine, "usemtl %s", strMaterialName );
            OBJ_GROUP group;
            group.strMaterialName = strMaterialName;
            m_nCurrentGroup = m_Groups.size();
            m_Groups.push_back( group );
        }
        else if( !strncmp( strLine, "mtllib", 6 ) )
        {
            CHAR strMtlFileName[64];
            sscanf( strLine, "mtllib %s", strMtlFileName );
            ReadMtlFile( strMtlFileName );
        }
    }

    fclose( pFile );

    if( FALSE == BuildUniqueVertices() )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: BuildUniqueVertices()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMesh::BuildUniqueVertices()
{
    if( 0 == m_Positions.size() )
        return FALSE;

    for( UINT32 i = 0; i < m_Groups.size(); ++i )
    {
        for( UINT32 j = 0; j < m_Groups[i].Faces.size(); ++j )
        {
            for( UINT32 k = 0; k < 3; ++k )
            {
                UINT16 nIndex = InsertVertex( m_Groups[i].Faces[j].VertexIndices[k] );
                m_Groups[i].Indices.push_back( nIndex );
            }
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InsertVertex()
// Desc: 
//--------------------------------------------------------------------------------------
UINT16 CMesh::InsertVertex( const OBJ_IVERTEX& VertexIndices )
{
    OBJ_VERTEX Vertex;
    memset( &Vertex, 0, sizeof(OBJ_VERTEX) );

    if( m_Positions.size() > 0 )
    {
        Vertex.px = m_Positions[VertexIndices.nPositionIndex].x;
        Vertex.py = m_Positions[VertexIndices.nPositionIndex].y;
        Vertex.pz = m_Positions[VertexIndices.nPositionIndex].z;
    }
    
    if( m_Normals.size() > 0 )
    {
        Vertex.nx = m_Normals[VertexIndices.nNormalIndex].x;
        Vertex.ny = m_Normals[VertexIndices.nNormalIndex].y;
        Vertex.nz = m_Normals[VertexIndices.nNormalIndex].z;
    }

    if( m_Texcoords.size() > 0 )
    {
        Vertex.tu = m_Texcoords[VertexIndices.nTexcoordIndex].tu;
        Vertex.tv = m_Texcoords[VertexIndices.nTexcoordIndex].tv;
    }

    UINT16 nIndex;
    if( FALSE == FindVertex( Vertex, nIndex ) )
    {
        nIndex = m_Vertices.size();
        m_Vertices.push_back( Vertex );
    }

    return nIndex;
}


//--------------------------------------------------------------------------------------
// Name: FindVertex()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMesh::FindVertex( const OBJ_VERTEX& vVertex, UINT16& nIndex )
{
    for( UINT32 i = 0; i < m_Vertices.size(); ++i )
    {
        if( vVertex.px == m_Vertices[i].px &&
            vVertex.py == m_Vertices[i].py &&
            vVertex.pz == m_Vertices[i].pz &&
            vVertex.nx == m_Vertices[i].nx &&
            vVertex.ny == m_Vertices[i].ny &&
            vVertex.nz == m_Vertices[i].nz &&
            vVertex.tu == m_Vertices[i].tu &&
            vVertex.tv == m_Vertices[i].tv )
        {
            nIndex = i;
            return TRUE;
        }
    }

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: ReadMtlFile()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMesh::ReadMtlFile( const CHAR* strFileName )
{
    FILE* pFile = fopen( strFileName, "r" );
    if( NULL == pFile )
        return FALSE;

    m_nNumTextures = 0;

    CHAR strCurrentMaterial[64] = "";

    CHAR strLine[255];
    while( fgets( strLine, 255, pFile ) )
    {
        if( !strncmp( strLine, "newmtl", 6 ) )
        {
            OBJ_MATERIAL Material;
            memset( &Material, 0, sizeof(OBJ_MATERIAL) );
            Material.vAmbientColor  = FRMVECTOR3( 0.2f, 0.2f, 0.2f );
            Material.vDiffuseColor  = FRMVECTOR4( 0.8f, 0.8f, 0.8f, 1.0f );
            Material.vSpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 0.0f );

            sscanf( strLine, "newmtl %s", strCurrentMaterial );

            m_Materials[strCurrentMaterial] = Material;

        }
        else if( strCurrentMaterial )
        {
            OBJ_MATERIAL* pMaterial = &m_Materials[strCurrentMaterial];

            if( !strncmp( strLine, "Ka", 2 ) )
            {
                FRMVECTOR3 vColor;
                sscanf( strLine, "Ka %f %f %f", &vColor.r, &vColor.g, &vColor.b );
                pMaterial->vAmbientColor = vColor;
            }
            else if( !strncmp( strLine, "Kd", 2 ) )
            {
                FRMVECTOR4 vColor;
                sscanf( strLine, "Kd %f %f %f", &vColor.r, &vColor.g, &vColor.b );
                pMaterial->vDiffuseColor = vColor;
            }
            else if( !strncmp( strLine, "Ks", 2 ) )
            {
                FRMVECTOR4 vColor;
                sscanf( strLine, "Ks %f %f %f", &vColor.r, &vColor.g, &vColor.b );
                pMaterial->vSpecularColor = vColor;
            }
            else if( !strncmp( strLine, "Tr", 2 ) )
            {
                FLOAT32 fAlpha;
                sscanf( strLine, "Tr %f", &fAlpha );
                pMaterial->vDiffuseColor.a  = fAlpha;
            }
            else if( strLine[0] == 'd' )
            {
                FLOAT32 fAlpha;
                sscanf( strLine, "d %f", &fAlpha );
                pMaterial->vDiffuseColor.a  = fAlpha;
            }
            else if( !strncmp( strLine, "Ns", 2 ) )
            {
                FLOAT32 fShininess;
                sscanf( strLine, "Ns %f", &fShininess );
                pMaterial->vSpecularColor.a = fShininess;
            }
            else if( strncmp(strLine, "map_Kd", 6) == 0 )
            {
                sscanf( strLine, "map_Kd %s", pMaterial->strDiffuseTexture );
                m_nNumTextures++;
            }
            else if( strncmp(strLine, "map_Ka", 6) == 0 )
            {
                sscanf( strLine, "map_Ka %s", pMaterial->strAmbientTexture );
                m_nNumTextures++;
            }
            else if( strncmp(strLine, "map_Ks", 6) == 0 )
            {
                sscanf( strLine, "map_Ks %s", pMaterial->strSpecularTexture );
                m_nNumTextures++;
            }
            else if( strncmp(strLine, "map_d", 5) == 0 )
            {
                sscanf( strLine, "map_d %s", pMaterial->strAlphaTexture );
                m_nNumTextures++;
            }
        }
    }

    fclose( pFile );

    return TRUE;
}


// End of namespace
}
