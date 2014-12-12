//--------------------------------------------------------------------------------------
// ObjImporter.cpp
//
// Imports an .obj file to the mesh format.
//
// Author:     QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if defined (WIN32)
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
#include <vector>
#include "FrmExtendedMesh.h"
#include "Read3ds.h"
#include "ReadMsh1.h"


//#include <FrmBasicTypes.h>
//#include <FrmMath.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <memory.h>
//#include "3ds.h"
//#include "index_t.h"





//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL Read3ds( const CHAR* strFileName, CFrmExtendedMesh* pDstMesh )
{
    // Check for the .obj file extension
    CHAR strExt[_MAX_EXT];
    CHAR strName[_MAX_FNAME];
#if defined (WIN32)
    _splitpath( strFileName, NULL, NULL, strName, strExt );
#elif defined (__linux__)
    strcpy(strExt, strrchr(strFileName, '.'));
    strcpy(strName, basename((char*)strFileName));
    char *ext = strrchr(strName, '.');
    *ext = '\0';
#endif
    if( 0 != stricmp( strExt, ".3ds" ) )
        return FALSE;

    // Try to load the model as a 3DS file
	FORMAT_3DS::FRM_3DS_MODEL srcModel;
	if( FALSE == Read3dsFileFast( strFileName, &srcModel ) )
		return FALSE;

    // Populate the extended mesh
    {
        // Allocate and initialize frames
        {
            pDstMesh->m_nNumFrames      = srcModel.m_nNumMeshes+1;
            pDstMesh->m_pFrames         = new MSH1_FORMAT::FRM_MESH_FRAME[srcModel.m_nNumMeshes+1];

			for( UINT32 i=0; i<pDstMesh->m_nNumFrames; i++ )
			{
				MSH1_FORMAT::FRM_MESH_FRAME* pFrame = &pDstMesh->m_pFrames[i];
				memset( pFrame, 0, sizeof(MSH1_FORMAT::FRM_MESH_FRAME) );
				pFrame->m_matTransform = FrmMatrixIdentity();

				// First frame is the root
				if( i == 0 )
				{
					strcpy( pFrame->m_strName, strName );
					strcat( pFrame->m_strName, strExt );

					if( i+1 < srcModel.m_nNumMeshes )
						pFrame->m_pChild = &pDstMesh->m_pFrames[i+i];
				}
				else
				{
					FORMAT_3DS::FRM_3DS_MESH* pSrcMesh = &srcModel.m_pMeshes[i];

					strcpy( pFrame->m_strName, pSrcMesh->m_strName );

					if( i+1 < srcModel.m_nNumMeshes )
						pFrame->m_pNext = &pDstMesh->m_pFrames[i+i];

					pFrame->m_pMesh = new MSH1_FORMAT::FRM_MESH;
		            MSH1_FORMAT::FRM_MESH* pDstMesh  = pFrame->m_pMesh;
				    memset( pDstMesh, 0, sizeof(MSH1_FORMAT::FRM_MESH) );

					// Build vertex layout
					{
						MSH1_FORMAT::FRM_VERTEX_ELEMENT* pVertexElement = pDstMesh->m_VertexLayout;
						if( pSrcMesh->m_pPositions )
						{
							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_POSITION;
							pVertexElement->nSize       = 3;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR3);
							pVertexElement++;
						}

						if( pSrcMesh->m_pNormals )
						{
							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_NORMAL;
							pVertexElement->nSize       = 3;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR3);
							pVertexElement++;
						}
						if( pSrcMesh->m_pTexCoords )
						{
							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_TEXCOORD0;
							pVertexElement->nSize       = 2;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR2);
							pVertexElement++;
						}
						if( pSrcMesh->m_pTangentSpace )
						{
							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_BINORMAL;
							pVertexElement->nSize       = 3;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR3);
							pVertexElement++;

							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_NORMAL;
							pVertexElement->nSize       = 3;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR3);
							pVertexElement++;

							pVertexElement->nUsage      = MSH1_FORMAT::FRM_VERTEX_TANGENT;
							pVertexElement->nSize       = 3;
							pVertexElement->nType       = GL_FLOAT;
							pVertexElement->bNormalized = FALSE;
							pVertexElement->nStride     = sizeof(FRMVECTOR3);
							pVertexElement++;
						}
					}

					// Build vertex buffer
					{
						UINT32 nNumVertices = pSrcMesh->m_nNumVertices;
						UINT32 nVertexSize  = ( pSrcMesh->m_pPositions    ? 1*sizeof(FRMVECTOR3) : 0 ) +
											  ( pSrcMesh->m_pNormals      ? 1*sizeof(FRMVECTOR3) : 0 ) +
											  ( pSrcMesh->m_pTexCoords    ? 1*sizeof(FRMVECTOR2) : 0 ) +
											  ( pSrcMesh->m_pTangentSpace ? 3*sizeof(FRMVECTOR3) : 0 );

						pDstMesh->m_nVertexSize   = nVertexSize;
						pDstMesh->m_nNumVertices  = nNumVertices;
						pDstMesh->m_pVertexData   = new BYTE[nVertexSize * nNumVertices];

						FLOAT32* pVertexData = (FLOAT32*)pDstMesh->m_pVertexData;
						for( UINT32 i = 0; i < nNumVertices; ++i )
						{
							if( pSrcMesh->m_pPositions )
							{
								*pVertexData++ = pSrcMesh->m_pPositions[i].x;
								*pVertexData++ = pSrcMesh->m_pPositions[i].y;
								*pVertexData++ = pSrcMesh->m_pPositions[i].z;
							}
							if( pSrcMesh->m_pNormals )
							{
								*pVertexData++ = pSrcMesh->m_pNormals[i].x;
								*pVertexData++ = pSrcMesh->m_pNormals[i].y;
								*pVertexData++ = pSrcMesh->m_pNormals[i].z;
							}
							if( pSrcMesh->m_pTexCoords )
							{
								*pVertexData++ = pSrcMesh->m_pTexCoords[i].tu;
								*pVertexData++ = pSrcMesh->m_pTexCoords[i].tv;
							}
							if( pSrcMesh->m_pTangentSpace )
							{
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vBinormal.x;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vBinormal.y;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vBinormal.z;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vNormal.x;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vNormal.y;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vNormal.z;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vTangent.x;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vTangent.y;
								*pVertexData++ = pSrcMesh->m_pTangentSpace[i].vTangent.z;
							}
						}
					}

					// Build index buffer
					{
						INT32  nNumIndices = 0;
						UINT32 nIndexSize  = sizeof(UINT16);
						UINT32 nNumGroups  = pSrcMesh->m_nNumGroups;
						for( UINT32 i = 0; i < nNumGroups; ++i )
							nNumIndices += pSrcMesh->m_pGroups[i].m_nNumTris * 3;

						pDstMesh->m_nNumIndices = nNumIndices;
						pDstMesh->m_nIndexSize  = nIndexSize;
						pDstMesh->m_pIndexData  = new BYTE[nIndexSize * nNumIndices];
						UINT16* pIndexData = (UINT16*)pDstMesh->m_pIndexData;
						for( UINT32 i = 0; i < nNumGroups; ++i )
						{
							for( UINT32 j=0; j<pSrcMesh->m_pGroups[i].m_nNumTris; j++ )
							{
								*pIndexData++ = (UINT16)pSrcMesh->m_pGroups[i].m_tris[3*j+0];
								*pIndexData++ = (UINT16)pSrcMesh->m_pGroups[i].m_tris[3*j+1];
								*pIndexData++ = (UINT16)pSrcMesh->m_pGroups[i].m_tris[3*j+2];
							}
						}
					}

					// Allocate and build mesh subsets
					{
						UINT32 nIndexSize  = sizeof(UINT16);
						UINT32 nNumGroups  = pSrcMesh->m_nNumGroups;

						pDstMesh->m_nNumSubsets = nNumGroups;
						pDstMesh->m_pSubsets    = new MSH1_FORMAT::FRM_MESH_SUBSET[ nNumGroups ];

						UINT32 nIndexOffset = 0;

						for( UINT32 i = 0; i < nNumGroups; ++i )
						{
							MSH1_FORMAT::FRM_MESH_SUBSET* pDstSubset = &pDstMesh->m_pSubsets[i];
							memset( pDstSubset, 0, sizeof(MSH1_FORMAT::FRM_MESH_SUBSET) );
		    
							pDstSubset->m_nPrimType    = GL_TRIANGLES;
							pDstSubset->m_nNumIndices  = pSrcMesh->m_pGroups[i].m_nNumTris * 3;
							pDstSubset->m_nIndexOffset = nIndexOffset;
							nIndexOffset += pSrcMesh->m_pGroups[i].m_nNumTris*3 + nIndexSize;
		    
							pDstSubset->m_nNumTextures = 0;
							pDstSubset->m_pTextures    = NULL; // new FRM_MESH_TEXTURE[pDstSubset->m_nNumTextures];

							{
								FORMAT_3DS::FRM_3DS_MATERIAL* pSrcMaterial = &srcModel.m_pMaterials[pSrcMesh->m_pGroups[i].m_nMaterialIndex];

								UINT32 nNumTextures = 0;
								if( strlen( pSrcMaterial->m_strTexture )  > 0 )   nNumTextures++;
								if( nNumTextures )
								{
									pDstSubset->m_pTextures = new MSH1_FORMAT::FRM_MESH_TEXTURE[nNumTextures];
									memset( pDstSubset->m_pTextures, 0, sizeof(MSH1_FORMAT::FRM_MESH_TEXTURE) * nNumTextures );

									if( strlen( pSrcMaterial->m_strTexture) > 0 )
										strcpy( pDstSubset->m_pTextures[pDstSubset->m_nNumTextures++].m_strTexture, pSrcMaterial->m_strTexture);
								}


								pDstSubset->m_vDiffuseColor  = pSrcMaterial->m_vDiffuse;
								pDstSubset->m_vAmbientColor  = *(FRMVECTOR3*)&pSrcMaterial->m_vAmbient;
								pDstSubset->m_vSpecularColor = pSrcMaterial->m_vSpecular;
							}
						}
					}
				}
			}
		}
    }

	return TRUE;
}
    
    
// Wrap everything in a namespace, to prevent name conflicts
namespace FORMAT_3DS
{


static BOOL    ParseMain3ds( CHAR* pData, FRM_3DS_MODEL* pModel );
static BOOL    ParseEdit3ds( CHAR* pData, FRM_3DS_MODEL* pModel );
static VOID    ParseEditObject( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseTriangleObject( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseVertexList( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseFaceList( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseTextureCoordinates( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseLocalAxis( CHAR* pData, FRM_3DS_MESH* pMesh );
static VOID    ParseMaterialGroup( CHAR* pData, FRM_3DS_GROUP* pGroup );
static VOID    ParseMaterial( CHAR* pData, FRM_3DS_MATERIAL* pMaterial );
static VOID    ParseTexture( CHAR* pData, FRM_3DS_MATERIAL* pMaterial );
static VOID    ParseColorChunk( CHAR* pData, FLOAT32* pColor );
static INT32   ParseString( CHAR* pData );
static FLOAT32 ParsePercentageChunk( CHAR* pData );


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL Read3dsFileFast( const CHAR* strFilename, FRM_3DS_MODEL* pModel )
{
    memset( pModel, 0, sizeof(FRM_3DS_MODEL) );

    FILE* file = fopen( strFilename, "rb" );
    if( NULL == file )
        return FALSE;

    fseek( file, 0, SEEK_END );
    INT32 nFileSize = ftell( file );
    fseek( file, 0, SEEK_SET );

    CHAR* pFileData = new CHAR[nFileSize];
    if( NULL == pFileData )
    {
        fclose( file );
        return FALSE;
    }

    fread( pFileData, nFileSize, 1, file );
    if( FALSE == ParseMain3ds( pFileData, pModel ) )
    {
        delete[] pFileData;
        return FALSE;
    }
    delete[] pFileData;

    for( INT32 i = 0; i < pModel->m_nNumMeshes; i++ )
    {
        pModel->m_pMeshes[i].RemoveDegenerates();
        pModel->m_pMeshes[i].SortTriangles();
    }

    pModel->m_nNumVertices = 0;
    pModel->m_nNumTriangles = 0;
    for( INT32 i = 0; i < pModel->m_nNumMeshes; i++ )
    {
        pModel->m_nNumVertices  += pModel->m_pMeshes[i].m_nNumVertices;
        pModel->m_nNumTriangles += pModel->m_pMeshes[i].m_nNumTriangles;
    }

    pModel->Calculate3dsBoundingBox();
    pModel->Calculate3dsNormals();
    pModel->Calculate3dsTangentSpace();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL ParseMain3ds( CHAR* pData, FRM_3DS_MODEL* pModel )
{
    // Read the first chunk
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    if( pChunk->m_nID != FRM_3DS_CHUNK_ID_MAIN_CHUNK )
        return FALSE;

    // Parse the sub chunks
    INT32 nPos = 0;
    while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_3DS_VERSION:
                break;

            case FRM_3DS_CHUNK_ID_EDIT_3DS:
                if( FALSE == ParseEdit3ds( (CHAR*)pChunk, pModel ) )
                    return FALSE;
                break;

            case FRM_3DS_CHUNK_KEYFRAMER:
                break;
        }
        nPos += pChunk->m_nLength;
    }

    // Assign mesh materials from the global list
    for( INT32 i = 0; i < pModel->m_nNumMeshes; i++ )
    {
        for( INT32 j = 0; j < pModel->m_pMeshes[i].m_nNumGroups; j++ )
        {
            for( INT32 k = 0; k < pModel->m_nNumMaterials; k++ )
            {
                if( !strcmp( pModel->m_pMeshes[i].m_pGroups[j].m_strName,
                             pModel->m_pMaterials[k].m_strName ) )
                {
                    pModel->m_pMeshes[i].m_pGroups[j].m_nMaterialIndex = k;
                    break;
                }
            }
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL ParseEdit3ds( CHAR* pData, FRM_3DS_MODEL* pModel )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	if( pChunk->m_nID != FRM_3DS_CHUNK_ID_EDIT_3DS )
        return FALSE;

    // Traverse the subchunks to parse material and meshes
	std::vector<FRM_3DS_MATERIAL> vMaterialList;
	std::vector<FRM_3DS_MESH>     vMeshList;

	INT32 nPos = 0;
    while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_MESH_VERSION:
			{
                // UINT32 nMeshVersion = *(UINT32*)pChunk->m_Data;
                break;
			}
            case FRM_3DS_CHUNK_ID_EDIT_MATERIAL:
			{
				FRM_3DS_MATERIAL material = {0};
                ParseMaterial( (CHAR*)pChunk, &material );
				vMaterialList.push_back( material );
                break;
			}
            case FRM_3DS_CHUNK_ID_ONE_UNIT:
			{
                // FLOAT32 fUnit = *(FLOAT32*)pChunk->m_Data;
                break;
			}
            case FRM_3DS_CHUNK_ID_EDIT_OBJECT:
			{
				FRM_3DS_MESH mesh = {0};
                ParseEditObject( (CHAR*)pChunk, &mesh );
				vMeshList.push_back( mesh );
                break;
			}
        }
        nPos += pChunk->m_nLength;
    }

    // Allocate space for the materials
	pModel->m_nNumMaterials = vMaterialList.size();
    if( pModel->m_nNumMaterials )
    {
        pModel->m_pMaterials = new FRM_3DS_MATERIAL[ pModel->m_nNumMaterials ];
        if( NULL == pModel->m_pMaterials )
            return FALSE;
        memcpy( pModel->m_pMaterials, &vMaterialList[0], sizeof(FRM_3DS_MATERIAL) * pModel->m_nNumMaterials );
    }

    // Allocate space for the meshes
    pModel->m_nNumMeshes = vMeshList.size();
    if( pModel->m_nNumMeshes )
    {
        pModel->m_pMeshes = new FRM_3DS_MESH[ pModel->m_nNumMeshes ];
        if( NULL == pModel->m_pMeshes )
            return FALSE;
        memcpy( pModel->m_pMeshes, &vMeshList[0], sizeof(FRM_3DS_MESH) * pModel->m_nNumMeshes );
    }
	
	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseEditObject( CHAR* pData, FRM_3DS_MESH* pMesh )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	CHAR* strName = (CHAR*)pData;
    pData += strlen( strName ) + 1;
    
	INT32 nPos = 0;
    while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_TRIANGLE_OBJECT:
            {
                strcpy( pMesh->m_strName, strName );
                ParseTriangleObject( (CHAR*)pChunk, pMesh );
                break;
            }
        }
        nPos += pChunk->m_nLength;
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseTriangleObject( CHAR* pData, FRM_3DS_MESH* pModel )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	INT32 nPos = 0;
	while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_VERTEX_LIST:
                ParseVertexList( (CHAR*)pChunk, pModel );
                break;

            case FRM_3DS_CHUNK_ID_FACE_LIST:
                ParseFaceList( (CHAR*)pChunk, pModel );
                break;

            case FRM_3DS_CHUNK_ID_MAPPING_COORDS:
                ParseTextureCoordinates( (CHAR*)pChunk, pModel );
                break;

            case FRM_3DS_CHUNK_ID_LOCAL_AXIS:
                ParseLocalAxis( (CHAR*)pChunk, pModel );
                break;

            case FRM_3DS_CHUNK_ID_TEXTURE_INFO:
                //ParseTextureInfo( (CHAR*)pChunk, pModel );
                break;
        }
        nPos += pChunk->m_nLength;
    }

    /*
    for( INT32 i = 0; i < pModel->m_nNumVertices; i++ )
    {
       pModel->m_verts[i][0] += pModel->m_position[0];
       pModel->m_verts[i][1] += pModel->m_position[1];
       pModel->m_verts[i][2] += pModel->m_position[2];
    }
    */
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseVertexList( CHAR* pData, FRM_3DS_MESH* pModel )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    struct VERTEX_LIST_CHUNK
    {
        UINT16     nNumVertices;
        FRMVECTOR3 pPositions[0];
    };
	VERTEX_LIST_CHUNK* pChunkData = (VERTEX_LIST_CHUNK*)pData;

    pModel->m_nNumVertices = pChunkData->nNumVertices;
    pModel->m_pPositions   = new FRMVECTOR3[ pChunkData->nNumVertices ];
    memcpy( pModel->m_pPositions, pChunkData->pPositions, 
            sizeof(FRMVECTOR3) * pChunkData->nNumVertices );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseFaceList( CHAR* pData, FRM_3DS_MESH* pModel )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	// Read the face list vertices
    struct FACE_LIST_CHUNK
    {
        UINT16 nNumFaces;
        UINT16 pVerts[0][4];
    };
    FACE_LIST_CHUNK* pChunkData = (FACE_LIST_CHUNK*)pData;

    pModel->m_nNumTriangles = pChunkData->nNumFaces;
    pModel->m_pIndices      = new INT32[pModel->m_nNumTriangles][3];

    for( INT32 i = 0; i < pModel->m_nNumTriangles; i++ )
    {
        pModel->m_pIndices[i][0] = (INT32)pChunkData->pVerts[i][0];
        pModel->m_pIndices[i][1] = (INT32)pChunkData->pVerts[i][1];
        pModel->m_pIndices[i][2] = (INT32)pChunkData->pVerts[i][2];
    }
    pData += sizeof(UINT16); 
    pData += sizeof(UINT16) * 4 * pModel->m_nNumTriangles;

    // Parse the material groups
	std::vector<FRM_3DS_GROUP> vGroupList;

	INT32 nPos = 0;
    while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_MATERIAL_GROUP:
			{
				FRM_3DS_GROUP group;
                ParseMaterialGroup( (CHAR*)pChunk, &group );
				vGroupList.push_back( group );
                break;
			}
            case FRM_3DS_CHUNK_ID_SMOOTH_GROUP:
			{
				pModel->m_smooth = new INT32[pModel->m_nNumTriangles];
                if( FALSE == pModel->m_smooth )
                    return;
                memcpy( pModel->m_smooth, pChunk->m_Data, sizeof(INT32) * pModel->m_nNumTriangles );
                break;
			}
        }
        nPos += pChunk->m_nLength;
    }

    pModel->m_nNumGroups = vGroupList.size();
    if( pModel->m_nNumGroups )
    {
        pModel->m_pGroups = new FRM_3DS_GROUP[ pModel->m_nNumGroups ];
        if( NULL == pModel->m_pGroups )
            return;
        memcpy( pModel->m_pGroups, &vGroupList[0], sizeof(FRM_3DS_GROUP) * pModel->m_nNumGroups );
	}
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseTextureCoordinates( char* pData, FRM_3DS_MESH* pModel )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    struct TEXCOORD_LIST_CHUNK
    {
        UINT16     nNumTexCoords;
        FRMVECTOR2 pTexCoords[0];
    };
    TEXCOORD_LIST_CHUNK* pChunkData = (TEXCOORD_LIST_CHUNK*)pData;

    pModel->m_nNumTexCoords = pChunkData->nNumTexCoords;
    pModel->m_pTexCoords    = new  FRMVECTOR2[ pChunkData->nNumTexCoords ];
    memcpy( pModel->m_pTexCoords, pChunkData->pTexCoords, 
            sizeof(FRMVECTOR2) * pChunkData->nNumTexCoords );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseLocalAxis( CHAR* pData, FRM_3DS_MESH* pMesh )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    struct LOCAL_AXIS_CHUNK
    {
        FRMVECTOR3 vAxes[3];
        FRMVECTOR3 vPosition;
    };
    LOCAL_AXIS_CHUNK* pChunkData = (LOCAL_AXIS_CHUNK*)pData;

    pMesh->m_vAxes[0]  = pChunkData->vAxes[0];
    pMesh->m_vAxes[1]  = pChunkData->vAxes[1];
    pMesh->m_vAxes[2]  = pChunkData->vAxes[2];
    pMesh->m_vPosition = pChunkData->vPosition;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseMaterialGroup( CHAR* pData, FRM_3DS_GROUP* pGroup )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    CHAR* strName = pData;
    pData += strlen( strName ) + 1;

    UINT16 nNumFaces = *(UINT16*)(pData);
    pData += sizeof(UINT16);

    pGroup->m_tris = new INT32[ nNumFaces ];
    if( NULL == pGroup->m_tris )
        return;
    memset( pGroup->m_tris, 0, nNumFaces * sizeof(INT32) );

    pGroup->m_nNumTris       = nNumFaces;
    pGroup->m_nMaterialIndex = 0;
    strcpy( pGroup->m_strName, strName );

    UINT16* pFaces = (UINT16*)pData;
    for( INT32 i = 0; i < nNumFaces; i++ )
    {
        pGroup->m_tris[i] = pFaces[i];
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseMaterial( CHAR* pData, FRM_3DS_MATERIAL* pMaterial )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	INT32 nPos = 0;
	while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_MAT_NAME:
                strcpy( pMaterial->m_strName, pChunk->m_Data );
                break;
        
            case FRM_3DS_CHUNK_ID_MAT_AMBIENT:
                ParseColorChunk( (CHAR*)pChunk, (FLOAT32*)&pMaterial->m_vAmbient );
                pMaterial->m_vAmbient.a = 1.0f;
                break;

            case FRM_3DS_CHUNK_ID_MAT_DIFFUSE:
                ParseColorChunk( (CHAR*)pChunk, (FLOAT32*)&pMaterial->m_vDiffuse );
                pMaterial->m_vDiffuse.a = 1.0f;
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_SPECUAR:
                ParseColorChunk( (CHAR*)pChunk, (FLOAT32*)&pMaterial->m_vSpecular );
                pMaterial->m_vSpecular.a = 1.0f;
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_SHININESS:
                pMaterial->m_fShininess = ParsePercentageChunk( (CHAR*)pChunk ) * 140.0f;
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_SHIN2PCT:
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_SHIN3PCT:
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_TRANSPARENCY:
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_EMISSION:
                ParseColorChunk( (CHAR*)pChunk, (FLOAT32*)&pMaterial->m_vEmission );
                pMaterial->m_vEmission.a = 1.0f;
                break;
            
            case FRM_3DS_CHUNK_ID_MAT_TEXMAP:
                ParseTexture( (CHAR*)pChunk, pMaterial );
                break;
        }
        nPos += pChunk->m_nLength;
    }
}


/*
//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseTextureInfo( CHAR* pData )
{
    pData += sizeof(FRM_3DS_CHUNK);

    FLOAT32 fRepeatX = *((FLOAT32*)&pData[2]);
    FLOAT32 fRepeatY = *((FLOAT32*)&pData[6]);
    FLOAT32 fScale   = *((FLOAT32*)&pData[70]);
}
*/


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseTexture( CHAR* pData, FRM_3DS_MATERIAL* pMaterial )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	INT32 nPos = 0;
	while( nPos < nLength )
    {
        pChunk = (FRM_3DS_CHUNK*)( pData + nPos );

        switch( pChunk->m_nID )
        {
            case FRM_3DS_CHUNK_ID_MAT_MAP_NAME:
                strcpy( pMaterial->m_strTexture, pChunk->m_Data );
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_TILING:
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_U_SCALE:
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_V_SCALE:
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_U_OFFSET:
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_V_OFFSET:
                break;
            case FRM_3DS_CHUNK_ID_MAT_MAP_V_ANG:
                break;
        }
        nPos += pChunk->m_nLength;
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID ParseColorChunk( CHAR* pData, FLOAT32* pColor )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

    switch( pChunk->m_nID )
    {
        case FRM_3DS_CHUNK_ID_COLOR_F:
        {
            memcpy( pColor, pData, 3*sizeof(FLOAT32) );
            break;
        }
        case FRM_3DS_CHUNK_ID_COLOR_24:
        {	
            BYTE rgb[3];
            memcpy( rgb, pData, 3*sizeof(BYTE) );
            pColor[0] = (FLOAT32)rgb[0] / 255.0f;
            pColor[1] = (FLOAT32)rgb[1] / 255.0f;
            pColor[2] = (FLOAT32)rgb[2] / 255.0f;
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
INT32 ParseString( CHAR* pData )
{
    INT32 nCount = 0;
    while( pData[nCount] != '\0' )
        nCount++;
    return nCount;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 ParsePercentageChunk( CHAR* pData )
{
    FRM_3DS_CHUNK* pChunk = (FRM_3DS_CHUNK*)pData;
    INT32 nLength = pChunk->m_nLength;
	nLength -= sizeof(FRM_3DS_CHUNK);
	pData   += sizeof(FRM_3DS_CHUNK);

	FLOAT32 fPercentage = 0.0f;

	switch( pChunk->m_nID )
    {
        case FRM_3DS_CHUNK_ID_INT16_PERCENTAGE:
        {
            UINT16 nPercentage;
            memcpy( &nPercentage, pData, sizeof(UINT16) );
            fPercentage = (FLOAT32)nPercentage / 100.0f;
            break;
        }
        case FRM_3DS_CHUNK_ID_FLOAT32_PERCENTAGE:
        {
            memcpy( &fPercentage, pData, sizeof(FLOAT32) );
            fPercentage /= 100.0f;
            break;
        }
    }

    return fPercentage;
}


//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------

inline VOID V_Cross( FRMVECTOR3& a, FRMVECTOR3& b, FRMVECTOR3& c )
{
    c.x = a.y * b.z - a.z * b.y; 
    c.y = a.z * b.x - a.x * b.z; 
    c.z = a.x * b.y - a.y * b.x;
}

inline FLOAT32 V_Magnitude( FRMVECTOR3& v )
{
    return (FLOAT32)sqrtf( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline VOID V_Normalizef( FRMVECTOR3& v )
{ 
    FLOAT32 mag = 1.0f / V_Magnitude( v );
    v.x *= mag;
    v.y *= mag; 
    v.z *= mag;
}

inline VOID V_AddTo( FRMVECTOR3& a, FRMVECTOR3& b )
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;					
}

inline VOID V_DivideByScalar( FRMVECTOR3& v, FLOAT32 s )
{
    v.x /= s;
    v.y /= s; 
    v.z /= s;
}

inline FLOAT32 V_DotProduct( FRMVECTOR3& a, FRMVECTOR3& b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
#pragma pack(push)
#pragma pack(1)

struct FRM_3DS_TRIANGLE
{
   UINT32     nMaterialIndex;
   UINT32     verts[3];
   UINT32     index;
   UINT32     nSmooth;
};

#pragma pack(pop)




//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
INT32 ComparePosition( FRMVECTOR3* a, FRMVECTOR3* b )
{
    if( a->x > b->x ) return +1;
    if( a->x < b->x ) return -1;
    
    if( a->y > b->y ) return +1;
    if( a->y < b->y ) return -1;

    if( a->z > b->z ) return -1;
    if( a->z < b->z ) return -1;

    return 0;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
INT32 SortByMaterial( FRM_3DS_TRIANGLE* a, FRM_3DS_TRIANGLE* b)
{
    if( a->nMaterialIndex > b->nMaterialIndex )
        return +1;
    if (a->nMaterialIndex < b->nMaterialIndex)
        return -1;

    if( a->nSmooth > b->nSmooth )
        return +1;
    if( a->nSmooth < b->nSmooth )
        return -1;

    return 0;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::CalculateMeshBoundingBox()
{
    for( INT32 i = 0; i < m_nNumVertices; i++ )
    {
        if( i == 0 )
        {
            m_vMax = m_pPositions[i];
            m_vMin = m_pPositions[i];
        }
        else
        {
            if( m_pPositions[i].x > m_vMax.x ) m_vMax.x = m_pPositions[i].x;
            if( m_pPositions[i].x < m_vMin.x ) m_vMin.x = m_pPositions[i].x;

            if( m_pPositions[i].y > m_vMax.y ) m_vMax.y = m_pPositions[i].y;
            if( m_pPositions[i].y < m_vMin.y ) m_vMin.y = m_pPositions[i].y;

            if( m_pPositions[i].z > m_vMax.z ) m_vMax.z = m_pPositions[i].z;
            if( m_pPositions[i].z < m_vMin.z ) m_vMin.z = m_pPositions[i].z;
        }
    }

    m_vCenter.x = m_vMin.x + ( m_vMax.x - m_vMin.x ) * 0.5f;
    m_vCenter.y = m_vMin.y + ( m_vMax.y - m_vMin.y ) * 0.5f;
    m_vCenter.z = m_vMin.z + ( m_vMax.z - m_vMin.z ) * 0.5f;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MODEL::Calculate3dsBoundingBox()
{
    for( INT32 i = 0; i < m_nNumMeshes; i++ )
    {
        m_pMeshes[i].CalculateMeshBoundingBox();

        if( i == 0 )
        {
            m_vMax = m_pMeshes[i].m_vMax;
            m_vMin = m_pMeshes[i].m_vMin;
        }
        else
        {
            if( m_pMeshes[i].m_vMax.x > m_vMax.x ) m_vMax.x = m_pMeshes[i].m_vMax.x;
            if( m_pMeshes[i].m_vMin.x < m_vMin.x ) m_vMin.x = m_pMeshes[i].m_vMin.x;

            if( m_pMeshes[i].m_vMax.y > m_vMax.y ) m_vMax.y = m_pMeshes[i].m_vMax.y;
            if( m_pMeshes[i].m_vMin.y < m_vMin.y ) m_vMin.y = m_pMeshes[i].m_vMin.y;

            if( m_pMeshes[i].m_vMax.z > m_vMax.z ) m_vMax.z = m_pMeshes[i].m_vMax.z;
            if( m_pMeshes[i].m_vMin.z < m_vMin.z ) m_vMin.z = m_pMeshes[i].m_vMin.z;
        }
    }

    m_vCenter.x = m_vMin.x + ( m_vMax.x - m_vMin.x ) * 0.5f;
    m_vCenter.y = m_vMin.y + ( m_vMax.y - m_vMin.y ) * 0.5f;
    m_vCenter.z = m_vMin.z + ( m_vMax.z - m_vMin.z ) * 0.5f;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Compute normals for this mesh taking smoothing groups into account
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::CalculateMeshNormals()
{
    INT32 nResult = 0;

    // Allocate the normals
    m_pNormals = new FRMVECTOR3[ m_nNumTriangles * 3 ];
    if( NULL == m_pNormals )
        return;
    memset( m_pNormals, 0, sizeof(FRMVECTOR3) * m_nNumTriangles * 3 );

    if( m_smooth == NULL )
    {
        for( INT32 i = 0; i < m_nNumTriangles; i++ )
        {
            FRMVECTOR3 v1 = m_pPositions[m_pIndices[i][1]] - m_pPositions[m_pIndices[i][0]];
            FRMVECTOR3 v2 = m_pPositions[m_pIndices[i][2]] - m_pPositions[m_pIndices[i][0]];
            V_Cross( v1, v2, m_pNormals[i * 3] );
            V_Normalizef( m_pNormals[i * 3] );
            m_pNormals[i * 3 + 1] = m_pNormals[i * 3];
            m_pNormals[i * 3 + 2] = m_pNormals[i * 3];
        }
        return;
    }

    // Index the vertices based on position alone
    FRM_3DS_INDEX vertIndex;
    vertIndex.Initialize( (char*)m_pPositions, sizeof(FRMVECTOR3), 
                          m_nNumVertices, (FRM_3DS_INDEX_SORT_FUNC)ComparePosition );

    // Build a table that links each vertex to all triangles that use said vertex
    INT32 (*triRefs)[50];
    triRefs = (INT32(*)[50])new INT32[ 50 * vertIndex.GetCount() ];
    if( triRefs == NULL )
        return;
    memset( triRefs, 0, sizeof(INT32) * 50 * vertIndex.GetCount() );

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        for( INT32 j = 0; j < 3; j++ )
        {
            INT32 vert = vertIndex.Find( &m_pPositions[m_pIndices[i][j]], &nResult );
            if( triRefs[vert][0] < 48 )
            {
                triRefs[vert][0]++;
                triRefs[vert][triRefs[vert][0]] = i;
            }
        }
    }

    INT32 normCount;

    // Allocate a buffer for the flat normals
    FRMVECTOR3* faceNorms = new FRMVECTOR3[ m_nNumTriangles ];
	if( NULL == faceNorms )
		return;
    memset( faceNorms, 0, sizeof(FRMVECTOR3) * m_nNumTriangles );

    // Go through every triangle to find its normal
    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        FRMVECTOR3 v1 = m_pPositions[m_pIndices[i][1]] - m_pPositions[m_pIndices[i][0]];
        FRMVECTOR3 v2 = m_pPositions[m_pIndices[i][2]] - m_pPositions[m_pIndices[i][0]];
        V_Cross( v1, v2, faceNorms[i] );
        V_Normalizef( faceNorms[i] );
    }

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        for( INT32 j = 0; j < 3; j++ )
        {
            INT32 vert = vertIndex.Find( &m_pPositions[m_pIndices[i][j]], &nResult );

            normCount = 0;
            for( INT32 k = 1; k <= triRefs[vert][0]; k++ )
            {
                if( m_smooth[i] == m_smooth[triRefs[vert][k]] )
                {
                    V_AddTo( m_pNormals[i * 3 + j], faceNorms[triRefs[vert][k]] );
                    normCount++;
                }
            }

            V_DivideByScalar( m_pNormals[i * 3 + j], (float)normCount );
            V_Normalizef( m_pNormals[i * 3 + j] );
        }
    }

    vertIndex.Free();

    // Free up the local buffers that were used
    delete[] triRefs;
    delete[] faceNorms;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Compute mesh tangent space
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::SmoothMeshTangentSpace()
{
    if( m_pTangentSpace == NULL )
        return;

    FRMVECTOR3  tempBinorm;

    FRM_3DS_TANGENT_SPACE* pNewTanSpace = new FRM_3DS_TANGENT_SPACE[ m_nNumTriangles * 3 ];
    if( NULL == pNewTanSpace )
        return;

    // Build a table that links verts that share a tangent space
    INT32 (*tanRefs)[50];
    tanRefs = (int(*)[50])new INT32[ 50 * m_nNumVertices ];
    if( NULL == tanRefs )
        return;
    memset( tanRefs, 0, sizeof(int) * 50 * m_nNumVertices );

    // Go through every triangle
    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        // Go through each vertex of this triangle
        for( INT32 j = 0; j < 3; j++ )
        {
            if( tanRefs[m_pIndices[i][j]][0] < 48 )
            {
                tanRefs[m_pIndices[i][j]][0]++;
                tanRefs[m_pIndices[i][j]][tanRefs[m_pIndices[i][j]][0]] = i * 3 + j;
            }
        }
    }

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        for( INT32 j = 0; j < 3; j++ )
        {
            INT32   tanCount = 0;
            FLOAT32 dotTangent;
            FLOAT32 dotBinormal;
            FLOAT32 dotNormal;

            FRM_3DS_TANGENT_SPACE tanSpace;
            memset( &tanSpace, 0, sizeof(FRM_3DS_TANGENT_SPACE) );

            for( INT32 k = 1; k <= tanRefs[m_pIndices[i][j]][0]; k++ )
            {
                INT32 tri  = tanRefs[m_pIndices[i][j]][k] / 3;
                INT32 vert = tanRefs[m_pIndices[i][j]][k] % 3;

                dotTangent  = V_DotProduct( m_pTangentSpace[i * 3 + j].vTangent,  m_pTangentSpace[tri * 3 +vert].vTangent );
                dotBinormal = V_DotProduct( m_pTangentSpace[i * 3 + j].vBinormal, m_pTangentSpace[tri * 3 +vert].vBinormal );
                dotNormal   = V_DotProduct( m_pTangentSpace[i * 3 + j].vNormal,   m_pTangentSpace[tri * 3 +vert].vNormal );

                if( (dotTangent > 0.85) && (dotBinormal > 0.85) && (dotNormal > 0.85) )
                {
                    V_AddTo( tanSpace.vTangent,  m_pTangentSpace[tri * 3 + vert].vTangent );
                    V_AddTo( tanSpace.vBinormal, m_pTangentSpace[tri * 3 + vert].vBinormal );
                    V_AddTo( tanSpace.vNormal,   m_pTangentSpace[tri * 3 + vert].vNormal );
                    tanCount++;
                }
            }

            V_DivideByScalar( tanSpace.vTangent,  (FLOAT32)tanCount );
            V_DivideByScalar( tanSpace.vBinormal, (FLOAT32)tanCount );
            V_DivideByScalar( tanSpace.vNormal,   (FLOAT32)tanCount );
            V_Normalizef( tanSpace.vTangent );
            V_Normalizef( tanSpace.vBinormal );
            V_Normalizef( tanSpace.vNormal );

            V_Cross( tanSpace.vTangent, tanSpace.vNormal, tempBinorm );
            V_Normalizef(tempBinorm);
            dotBinormal = V_DotProduct( tanSpace.vBinormal, tempBinorm );
            if( dotBinormal < 0.0f )
                tempBinorm = -tempBinorm;

            pNewTanSpace[i * 3 + j].vTangent  = tanSpace.vTangent;
            pNewTanSpace[i * 3 + j].vBinormal = tempBinorm;
            pNewTanSpace[i * 3 + j].vNormal   = tanSpace.vNormal;
        }
    }

    delete[] m_pTangentSpace;
    m_pTangentSpace = pNewTanSpace;

    // Free up the local buffers that were used
    delete[] tanRefs;
}


INT32 CompareLong( long* a, long* b )
{
    if( *a > *b ) return +1;
    if( *a < *b ) return -1;
    return 0;  
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::RemoveDegenerates()
{
    long* found;

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        if( ( m_pIndices[i][0] >= m_nNumVertices ) ||
            ( m_pIndices[i][1] >= m_nNumVertices ) ||
            ( m_pIndices[i][2] >= m_nNumVertices ) )
        {
            i = i;
        }
    }

    for( INT32 i = 0;i < m_nNumGroups; i++ )
    {
        qsort( m_pGroups[i].m_tris, m_pGroups[i].m_nNumTris, sizeof(long),
               (FRM_3DS_INDEX_SORT_FUNC)CompareLong);
    }

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        if( ( ( m_pIndices[i][0] == m_pIndices[i][1] ) ||
              ( m_pIndices[i][0] == m_pIndices[i][2] ) ||
              ( m_pIndices[i][1] == m_pIndices[i][2] ) )
         || 
            ( ( m_pPositions[m_pIndices[i][0]].x == m_pPositions[m_pIndices[i][1]].x ) &&
              ( m_pPositions[m_pIndices[i][0]].y == m_pPositions[m_pIndices[i][1]].y ) &&
              ( m_pPositions[m_pIndices[i][0]].z == m_pPositions[m_pIndices[i][1]].z ) )
         || 
            ( ( m_pPositions[m_pIndices[i][0]].x == m_pPositions[m_pIndices[i][2]].x ) &&
              ( m_pPositions[m_pIndices[i][0]].y == m_pPositions[m_pIndices[i][2]].y ) &&
              ( m_pPositions[m_pIndices[i][0]].z == m_pPositions[m_pIndices[i][2]].z ) )
         ||
            ( ( m_pPositions[m_pIndices[i][1]].x == m_pPositions[m_pIndices[i][2]].x ) &&
              ( m_pPositions[m_pIndices[i][1]].y == m_pPositions[m_pIndices[i][2]].y ) &&
              ( m_pPositions[m_pIndices[i][1]].z == m_pPositions[m_pIndices[i][2]].z ) ) )
        {
            if( i != ( m_nNumTriangles - 1 ) )
            {
                memmove( &m_pIndices[i], &m_pIndices[i+1],
                         sizeof(long) * 3 * (m_nNumTriangles - i - 1) );
            }

            for( INT32 j = 0; j < m_nNumGroups; j++ )
            {
                found = (long*)bsearch( &i, m_pGroups[j].m_tris, 
                                        m_pGroups[j].m_nNumTris, sizeof(long), 
                                        (FRM_3DS_INDEX_SORT_FUNC)CompareLong );

                if( found != NULL )
                {
                    INT32 k = ((int)found - (int)m_pGroups[j].m_tris) / sizeof(long);

                    if( k < m_pGroups[j].m_nNumTris - 1 )
                    {
                        memmove( &m_pGroups[j].m_tris[k], &m_pGroups[j].m_tris[k+1],
                                 sizeof(long) * (m_pGroups[j].m_nNumTris - k - 1) );
                    }
                    m_pGroups[j].m_nNumTris--;

                    for( INT32 l = k; l < m_pGroups[j].m_nNumTris; l++ )
                    {
                        m_pGroups[j].m_tris[l]--;
                    }
                }
            }

            m_nNumTriangles--;
            i--;
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Compute normals taking smoothing groups into account
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MODEL::Calculate3dsNormals()
{
    for( INT32 i = 0; i < m_nNumMeshes; i++ )
    {
        m_pMeshes[i].CalculateMeshNormals();
    }
}


VOID TangentSpace( FRMVECTOR3& vert1,  FRMVECTOR3& vert2, FRMVECTOR3& vert3, 
                   FRMVECTOR2& t1,     FRMVECTOR2& t2,    FRMVECTOR2& t3, 
                   FRMVECTOR3& normal, FRM_3DS_TANGENT_SPACE& tangentSpace )
{
    float      a, b;
    float      u1, u2;
    float      v1, v2;
    FRMVECTOR3 edge1;
    FRMVECTOR3 edge2;
    float      dot;
    FRMVECTOR3 dvTmp;
    FRMVECTOR3 duTmp;


    if( (t1.tu == t2.tu) || (t1.tv == t2.tv) || 
        (t1.tu == t3.tu) || (t1.tv == t3.tv) )
    {
        tangentSpace.vTangent.x = 1.0f;
        tangentSpace.vTangent.y = 0.0f;
        tangentSpace.vTangent.z = 0.0f;

        tangentSpace.vBinormal.x = 0.0f;
        tangentSpace.vBinormal.y = 1.0f;
        tangentSpace.vBinormal.z = 0.0f;

        tangentSpace.vNormal.x = 0.0f;
        tangentSpace.vNormal.y = 0.0f;
        tangentSpace.vNormal.z = 1.0f;
        return;
    }

    //===================================//
    // compute vertex space edge vectors //
    //===================================//
    edge1.x = vert2.x - vert1.x;
    edge1.y = vert2.y - vert1.y;
    edge1.z = vert2.z - vert1.z;

    edge2.x = vert3.x - vert1.x;
    edge2.y = vert3.y - vert1.y;
    edge2.z = vert3.z - vert1.z;

    //====================================//
    // compute texture space edge vectors //
    //====================================//
    u1 = t2.tu - t1.tu;
    u2 = t3.tu - t1.tu;

    v1 = t2.tv - t1.tv;
    v2 = t3.tv - t1.tv;

    //=========================================================//
    // compute releation between how edges change as u changes //
    //=========================================================//
    a = (u1 - v1 * u2 / v2);
    if (a != 0.0f)
    {
        a = 1.0f / a;
    }
    b = (u2 - v2 * u1 / v1);
    if (b != 0.0f)
    {
        b = 1.0f / b;
    }
    duTmp.x = a * edge1.x + b * edge2.x;
    duTmp.y = a * edge1.y + b * edge2.y;
    duTmp.z = a * edge1.z + b * edge2.z;
    V_Normalizef(duTmp);

    //=========================================================//
    // compute releation between how edges change as v changes //
    //=========================================================//
    a = (v1 - u1 * v2 / u2);
    if (a != 0.0f)
    {
        a = 1.0f / a;
    }
    b = (v2 - u2 * v1 / u1);
    if (b != 0.0f)
    {
        b = 1.0f / b;
    }
    dvTmp.x = a * edge1.x + b * edge2.x;
    dvTmp.y = a * edge1.y + b * edge2.y;
    dvTmp.z = a * edge1.z + b * edge2.z;
    V_Normalizef(dvTmp);

    //=====================================//
    // normal portion of the tangent space //
    //=====================================//
    tangentSpace.vNormal.x = normal.x;
    tangentSpace.vNormal.y = normal.y;
    tangentSpace.vNormal.z = normal.z;

    dot = V_DotProduct( duTmp, tangentSpace.vNormal );

    //======================================//
    // tangent portion of the tangent space //
    //======================================//
    tangentSpace.vTangent.x = duTmp.x - (dot * tangentSpace.vNormal.x );
    tangentSpace.vTangent.y = duTmp.y - (dot * tangentSpace.vNormal.y );
    tangentSpace.vTangent.z = duTmp.z - (dot * tangentSpace.vNormal.z );
    V_Normalizef( tangentSpace.vTangent );

    dot = V_DotProduct( dvTmp, tangentSpace.vNormal );

    //=======================================//
    // binormal portion of the tangent space //
    //=======================================//
    tangentSpace.vBinormal.x = dvTmp.x - (dot * tangentSpace.vNormal.x );
    tangentSpace.vBinormal.y = dvTmp.y - (dot * tangentSpace.vNormal.y );
    tangentSpace.vBinormal.z = dvTmp.z - (dot * tangentSpace.vNormal.z );
    V_Normalizef( tangentSpace.vBinormal );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::CalculateMeshTangentSpace()
{
    if( NULL == m_pNormals )
        CalculateMeshNormals();

    if( ( m_pNormals == NULL ) || ( m_pTexCoords == NULL ) )
        return;

    if( m_pTangentSpace != NULL )
    {
        delete[] m_pTangentSpace;
        m_pTangentSpace = NULL;
    }

    // Allocate tangent space
    m_pTangentSpace = new FRM_3DS_TANGENT_SPACE[ m_nNumTriangles * 3 ];
    if( m_pTangentSpace == NULL )
        return;
    memset( m_pTangentSpace, 0, sizeof(FRM_3DS_TANGENT_SPACE) * m_nNumTriangles * 3 );

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        for( INT32 j = 0; j < 3; j++ )
        {
            TangentSpace(
                        m_pPositions[m_pIndices[i][0]],
                        m_pPositions[m_pIndices[i][1]], 
                        m_pPositions[m_pIndices[i][2]],
                            
                        m_pTexCoords[m_pIndices[i][0]],
                        m_pTexCoords[m_pIndices[i][1]], 
                        m_pTexCoords[m_pIndices[i][2]],
                            
                        m_pNormals[i * 3 + j], 
                    
                        m_pTangentSpace[i * 3 + j]
                     );
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Compute tangent space
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MODEL::Calculate3dsTangentSpace()
{
    for( INT32 i = 0; i < m_nNumMeshes; i++ )
    {
        m_pMeshes[i].CalculateMeshTangentSpace();
        m_pMeshes[i].SmoothMeshTangentSpace();
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Sorts triangles by group and then by smoothing group
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::SortTriangles()
{
    if( NULL == m_nNumGroups )
    {
        m_pGroups = new FRM_3DS_GROUP[ 1 ];
        if( m_pGroups )
        {
            m_nNumGroups = 1;
            memset( &m_pGroups[0], 0, sizeof(FRM_3DS_GROUP) );
            m_pGroups[0].m_nMaterialIndex = -1;
            m_pGroups[0].m_nStart   = 0;
            m_pGroups[0].m_nNumTris = m_nNumTriangles;
            m_pGroups[0].m_tris     = new INT32[ m_nNumTriangles ];

            for( INT32 i = 0; i < m_pGroups[0].m_nNumTris; i++ )
            {
                m_pGroups[0].m_tris[i] = i;
            }
        }
    
        if( m_smooth == NULL )
        {
            return;
        }
    }

    FRM_3DS_TRIANGLE* tris = new FRM_3DS_TRIANGLE[ m_nNumTriangles ];
    FRM_3DS_TRIANGLE* temp = new FRM_3DS_TRIANGLE[ m_nNumTriangles ];

    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        memcpy(tris[i].verts, m_pIndices[i], sizeof(long) * 3);
        tris[i].nMaterialIndex = -1;
        tris[i].index = i;

        if (m_smooth)
        {
            tris[i].nSmooth = m_smooth[i];
        }
        else
        {
            tris[i].nSmooth = 0;
        }
    }

    for( INT32 i = 0; i < m_nNumGroups; i++ )
    {
        for( INT32 j = 0; j < m_pGroups[i].m_nNumTris; j++ )
        {
            tris[m_pGroups[i].m_tris[j]].nMaterialIndex = m_pGroups[i].m_nMaterialIndex;
        }
    }

    // Now sort the triangles by material
#if defined (WIN32)
    qsort( tris, m_nNumTriangles, sizeof(FRM_3DS_TRIANGLE),
           (int (__cdecl *)(const void *,const void *))SortByMaterial );
#elif defined (__linux__)
    qsort(tris, m_nNumTriangles, sizeof(FRM_3DS_TRIANGLE), (INT32 (*)(const void*, const void*))SortByMaterial);
#endif

    /*
    INT32 i = 0;
    while( i < m_nNumTriangles )
    {
        long* oldSmooth = NULL;
        INT32 result = 0;
        INT32 next = i + 1;
        while ((next < m_nNumTriangles) && (result == 0))
        {
            result = SortByMaterial(&pIndices[i], &tris[next]);
            next++;
        }

        if (next == m_nNumTriangles)
        {
            break;
        }
        next--;

        if (result > 0)
        {
            result = 0;
            INT32 nextNext = next + 1;
            while ((nextNext < m_nNumTriangles) && (result == 0))
            {
                result = SortByMaterial(&tris[next], &tris[nextNext]);
                nextNext++;
            }
            if (result != 0)
            {
                nextNext--;
            }

            //==========================================//
            // copy the less than portion into a buffer //
            //==========================================//
            memcpy(temp, &tris[next], (nextNext - next) * sizeof(FRM_3DS_TRIANGLE));

            //=====================================//
            // move the greater than portion ahead //
            //=====================================//
            memmove(&tris[(nextNext - next) + i], &tris[i], (next - i) * sizeof(FRM_3DS_TRIANGLE));

            //====================================================//
            // copy the less than portion back in from the buffer //
            //====================================================//
            memcpy(&tris[i], temp, (nextNext - next) * sizeof(FRM_3DS_TRIANGLE));

            //===================================//
            // start at the begining of the list //
            //===================================//
            i = 0;
        }
        else if (result < 0)
        {
            i = next;
        }
        else
        {
            break;
        }
    }
    */

    INT32 nNumGroups = 1;
    m_pGroups[0].m_nMaterialIndex = tris[0].nMaterialIndex;
    m_pGroups[0].m_nStart   = 0;
    m_pGroups[0].m_nNumTris = 0;
    for( INT32 i = 0; i < m_nNumTriangles; i++ )
    {
        memcpy(m_pIndices[i], tris[i].verts, sizeof(long) * 3);
        if (m_smooth)
        {
            m_smooth[i] = tris[i].nSmooth;
        }

        if (m_pGroups[nNumGroups - 1].m_nMaterialIndex != tris[i].nMaterialIndex)
        {
            m_pGroups[nNumGroups].m_nMaterialIndex = tris[i].nMaterialIndex;
            m_pGroups[nNumGroups].m_nStart   = i;
            m_pGroups[nNumGroups].m_nNumTris = 0;
            nNumGroups++;
        }

        m_pGroups[nNumGroups-1].m_nNumTris++;
    }

    delete[] tris;
    delete[] temp;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MESH::Destroy()
{
    delete[] m_pPositions;
    delete[] m_pIndices;
    delete[] m_pNormals;
    delete[] m_pTangentSpace;
    delete[] m_pTexCoords;
    delete[] m_smooth;

    if( m_pGroups )
    {
        for( INT32 i = 0; i < m_nNumGroups; i++ )
        {
            delete[] m_pGroups[i].m_tris;
        }
        delete[] m_pGroups;
    }

    memset( this, 0, sizeof(FRM_3DS_MESH) );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_MODEL::Destroy()
{
    if( m_pMeshes )
    {
        for( INT32 i = 0; i < m_nNumMeshes; i++ )
        {
			m_pMeshes[i].Destroy();
        }
        delete[] m_pMeshes;
    }
    delete[] m_pMaterials;
    memset( this, 0, sizeof(FRM_3DS_MODEL) );
}



//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define INIT_ALLOC      1000


//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------
static INT32 BinaryTraverse( VOID*  pValue,                                // pointer to the reference element   
                             VOID*  pData,                                 // pointer to the indexed data   
                             INT32* pIndices,                              // pointer to the indices
                             INT32  nStride,                               // data stride
                             INT32  nCount,                                // number of items in the index      
                             INT32* pResult,                               // buffer for the result of the last compare
                             INT32  (*fnCompare)(const VOID*, const VOID*) // pointer to the compare function
                           );


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID FRM_3DS_INDEX::Initialize( CHAR* pArray, INT32 nSize, INT32 nCount,
				                FRM_3DS_INDEX_SORT_FUNC fnSortFunc )
{
	m_nCount      = 0;
	m_nAllocCount = 0;
	m_pIndices    = NULL;
	m_pData       = pArray;
	m_nDataSize   = nSize;
	m_fnSortFunc  = fnSortFunc;

	for( INT32 i = 0; i < nCount; i++ )
	{
		SortedInsert( i );
	}
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Appends the passed element after the specified index
//       calling this function directly could result in an unsorted vector 
//--------------------------------------------------------------------------------------
INT32 FRM_3DS_INDEX::Append( INT32 nValue, INT32 nPosition )
{
	if( (nPosition < 0) || ((nPosition >= m_nCount) && (m_nCount != 0)) )
		return INDEX_OUT_OF_RANGE;

	INT32 nResult = CheckAlloc();
	if( nResult == INDEX_MEMORY_ERROR )
		return nResult;

	if( (m_nCount != 0) && (nPosition < (m_nCount - 1)) )
	{
		// shift down by one element from the index to the end
		memmove( &m_pIndices[nPosition + 2], &m_pIndices[nPosition+ 1], 
			     (m_nCount - nPosition - 1) * sizeof(INT32) );
	}

	// copy the new element in
	m_pIndices[nPosition + 1] = nValue;

	m_nCount++;

	return INDEX_OK;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Checks if memory is still available in the vector and allocates more if 
//       necessary 
//--------------------------------------------------------------------------------------
INT32 FRM_3DS_INDEX::CheckAlloc()
{
	INT32*   buffer;
	INT32    allocSize = 0;

	if( (m_nAllocCount == 0) && (m_nCount == 0) )
	{
		m_nAllocCount = INIT_ALLOC;
		m_pIndices    = new INT32[ m_nAllocCount ];
		if( NULL == m_pIndices )
			return INDEX_MEMORY_ERROR;
		memset( m_pIndices, 0, m_nAllocCount * sizeof(INT32) );
	}

	if( m_nCount >= m_nAllocCount )
	{
		// We will allocate twice as much memory as is currently used
		allocSize = (m_nCount << 1);

		buffer = new INT32[ allocSize ];
		if( NULL == buffer )
			return INDEX_MEMORY_ERROR;
		memset( buffer, 0, allocSize * sizeof(INT32) );

		// If we had memory allocated before then copy the old data into the new buffer
		if( (m_nAllocCount != 0) && (m_pIndices != NULL) )
		{
			memcpy( buffer, m_pIndices, m_nAllocCount * sizeof(INT32) );
		}

		// Free the old data if necessary
		delete[] m_pIndices;

		// Set the data pointer to point to the new data
		m_pIndices = buffer;

		// Record the allocation size
		m_nAllocCount = allocSize;
	}

	return INDEX_OK;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Frees all resources used by the vector
//--------------------------------------------------------------------------------------
VOID FRM_3DS_INDEX::Free()
{
	delete[] m_pIndices;
	memset( this, 0, sizeof(FRM_3DS_INDEX) );
}



//--------------------------------------------------------------------------------------
// Name: 
// Desc: Inserts the passed element into the vector before the index specified
//       calling this function directly could result in an unsorted vector
//--------------------------------------------------------------------------------------
INT32 FRM_3DS_INDEX::Insert( INT32 nValue, INT32 nPosition )
{
	if( (nPosition < 0) || ((nPosition >= m_nCount) && (m_nCount != 0)) )
		return INDEX_OUT_OF_RANGE;

	INT32 nResult = CheckAlloc();
	if( nResult == INDEX_MEMORY_ERROR )
		return nResult;

	// Shift down by one element from the index to the end
	if( m_nCount != 0 )
	{
		memmove( &m_pIndices[nPosition + 1], &m_pIndices[nPosition], 
			     (m_nCount - nPosition) * sizeof(INT32) );
	}

	// Copy the new element in
	m_pIndices[nPosition] = nValue; 

	m_nCount++;

	return INDEX_OK;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Inserts the passed element into the vector such that the vector is sorted
//       INDEX MUST BE SORTED
//--------------------------------------------------------------------------------------
INT32 FRM_3DS_INDEX::SortedInsert( INT32 nValue )
{
	INT32 nPosition;
	INT32 nCompValue;
	INT32 nResult;

	// Traverse the list of sorted data to find the index where we will insert
	nPosition = BinaryTraverse( (char*)m_pData + (nValue * m_nDataSize), 
				                m_pData, m_pIndices, m_nDataSize, m_nCount, 
				                &nCompValue, m_fnSortFunc );

	if( nCompValue == 0 )
	{
		nResult = INDEX_OK;
	}
	else if( nCompValue < 0 ) // we are inserting before this index
	{
		nResult = Insert( nValue, nPosition );
	}
	else // we are appending after this index
	{
		nResult = Append( nValue, nPosition );
	}

	return nResult;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Finds the specified element in the vector and returns its index
//       INDEX MUST BE SORTED
//--------------------------------------------------------------------------------------
INT32 FRM_3DS_INDEX::Find( VOID* pElement, INT32* pResult )
{
	// Traverse the list of sorted data to find the element
	INT32 nPosition = BinaryTraverse( pElement, m_pData, m_pIndices, 
		                              m_nDataSize, m_nCount, pResult, 
								      m_fnSortFunc );
	return nPosition;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: Generic binary traversal function
//--------------------------------------------------------------------------------------
INT32 BinaryTraverse( VOID*  pValue,                                // pointer to the reference element   
                      VOID*  pData,                                 // pointer to the indexed data      
                      INT32* pIndices,                              // pointer index      
                      INT32  nStride,                               // data stride      
                      INT32  nCount,                                // number of items in the array      
                      INT32* pResult,                               // buffer for the result of the last compare
                      INT32  (*fnCompare)(const VOID*, const VOID*) // pointer to the compare function
                     )
{
	INT32 high = nCount;
	INT32 low = 0;
	INT32 mid = low + ((high - low) >> 1);
	INT32 nextMid;
	
	INT32 nResult = -1;

	while( low != high )
	{
		nResult = fnCompare( pValue, (VOID*)( (UINT32)pData + pIndices[mid] * nStride ) );
		if( nResult == 0 )
			break;
		
		if( nResult < 0 )
			high = mid;
		else
			low = mid;

		nextMid = low + ((high - low) >> 1);
		if( mid == nextMid )
			break;
		mid = nextMid;
	}

	if( pResult )
		(*pResult) = nResult;

	return mid;
}


// End of namespace
}
