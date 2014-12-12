//--------------------------------------------------------------------------------------
// WriteMesh.cpp
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "FrmExtendedMesh.h"
#include "ReadMsh1.h"

using namespace MSH1_FORMAT;

BOOL WriteMsh1( const CHAR* strFileName, FRM_MESH_FRAME* m_pFrames, UINT32 m_nNumFrames );

//--------------------------------------------------------------------------------------
// Name: SaveOutputMesh()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CFrmExtendedMesh::SaveOutputMesh( const CHAR* strFileName )
{
	return WriteMsh1( strFileName, m_pFrames, m_nNumFrames );
}


//--------------------------------------------------------------------------------------
// Name: WriteMsh1()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL WriteMsh1( const CHAR* strFileName, FRM_MESH_FRAME* pFrames, UINT32 nNumFrames )
{
    FILE* file = fopen( strFileName, "wb" );
    if( NULL == file )
        return FALSE;

    // Compute the data sizes
    UINT32 nVertexDataSize = 0;
    UINT32 nIndexDataSize  = 0;
    UINT32 nNumMeshes      = 0;
	UINT32 nNumBones       = 0;
    UINT32 nNumSubsets     = 0;
    UINT32 nNumTextures    = 0; 
	UINT32 nNumBoneIDs     = 0;

    for( UINT32 i=0; i<nNumFrames; i++ )
    {
        if( pFrames[i].m_pMesh )
        {
            nNumMeshes++;

			FRM_MESH* pMesh = pFrames[i].m_pMesh;
            nVertexDataSize += pMesh->m_nNumVertices * pMesh->m_nVertexSize;
            nIndexDataSize  += pMesh->m_nNumIndices * pMesh->m_nIndexSize;

			nNumBones   += pMesh->m_nNumBones;

            nNumSubsets += pMesh->m_nNumSubsets;

            for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
			{
                nNumTextures += pMesh->m_pSubsets[j].m_nNumTextures;
                nNumBoneIDs  += pMesh->m_nNumBoneMatrices;
			}
        }
    }

	UINT32 nOffset = 0;
	UINT32 nFramesOffset   = nOffset; nOffset += sizeof(FRM_MESH_FRAME)   * nNumFrames;
	UINT32 nMeshesOffset   = nOffset; nOffset += sizeof(FRM_MESH)         * nNumMeshes;
	UINT32 nSubsetsOffset  = nOffset; nOffset += sizeof(FRM_MESH_SUBSET)  * nNumSubsets;
	UINT32 nBonesOffset    = nOffset; nOffset += sizeof(FRM_MESH_BONE)    * nNumBones;
	UINT32 nTexturesOffset = nOffset; nOffset += sizeof(FRM_MESH_TEXTURE) * nNumTextures;
    UINT32 nBoneIDsOffset  = nOffset; nOffset += sizeof(UINT32)           * nNumBoneIDs;
	UINT32 nSystemDataSize = nOffset; 

	nOffset = 0;
    UINT32 nVerticesOffset = nOffset; nOffset += nVertexDataSize;
    UINT32 nIndicesOffset  = nOffset; nOffset += nIndexDataSize;
    UINT32 nBufferDataSize = nOffset;

    // Write the file header
    FRM_MESH_FILE_HEADER header;
    header.nMagic          = FRM_MESH_MAGIC_ID;
    header.nSystemDataSize = nSystemDataSize;
    header.nBufferDataSize = nBufferDataSize;
    header.nNumFrames      = nNumFrames;
    fwrite( &header, sizeof(header), 1, file );

    // Write the mesh frames
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH_FRAME frame;
        memset( &frame, 0, sizeof(FRM_MESH_FRAME) );
		strcpy( frame.m_strName, pFrames[i].m_strName );
		frame.m_matTransform = pFrames[i].m_matTransform;

		if( pFrames[i].m_pMesh )
		{
	        frame.m_pMesh = (FRM_MESH*)nMeshesOffset;
			nMeshesOffset += sizeof(FRM_MESH);
		}

        if( pFrames[i].m_pNext )
		{
            frame.m_pNext  = (FRM_MESH_FRAME*)((UINT32)pFrames[i].m_pNext - (UINT32)pFrames ); 
		}

        if( pFrames[i].m_pChild )
		{
            frame.m_pChild = (FRM_MESH_FRAME*)((UINT32)pFrames[i].m_pChild - (UINT32)pFrames ); 
		}

        fwrite( &frame,  sizeof(FRM_MESH_FRAME), 1, file );
    }
int x;
x = ftell( file );

    // Write the meshes
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        FRM_MESH mesh;
        memset( &mesh, 0, sizeof(FRM_MESH) );
		memcpy( mesh.m_VertexLayout, pMesh->m_VertexLayout, sizeof( mesh.m_VertexLayout) );
        mesh.m_nNumVertices  = pMesh->m_nNumVertices;
		mesh.m_nVertexSize   = pMesh->m_nVertexSize;
		mesh.m_hVertexBuffer = nVerticesOffset;
        nVerticesOffset += pMesh->m_nNumVertices * pMesh->m_nVertexSize;

        mesh.m_nNumIndices   = pMesh->m_nNumIndices;
        mesh.m_nIndexSize    = pMesh->m_nIndexSize;
		mesh.m_hIndexBuffer  = nIndicesOffset;
        nIndicesOffset += pMesh->m_nNumIndices * pMesh->m_nIndexSize;

		mesh.m_nNumBonesPerVertex = pMesh->m_nNumBonesPerVertex;
		mesh.m_nNumBoneMatrices   = pMesh->m_nNumBoneMatrices;
		mesh.m_nNumBones          = pMesh->m_nNumBones;
		mesh.m_pBones             = (FRM_MESH_BONE*)nBonesOffset;
		nBonesOffset += sizeof(FRM_MESH_BONE) * pMesh->m_nNumBones;

        mesh.m_nNumSubsets   = pMesh->m_nNumSubsets;
        mesh.m_pSubsets      = (FRM_MESH_SUBSET*)nSubsetsOffset;
		nSubsetsOffset += sizeof(FRM_MESH_SUBSET) * pMesh->m_nNumSubsets;

		fwrite( &mesh,   sizeof(FRM_MESH), 1, file );
	}
x = ftell( file );

    // Write the mesh subsets
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
        {
            FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];

            FRM_MESH_SUBSET subset;
            memset( &subset, 0, sizeof(FRM_MESH_SUBSET) );
            
            subset.m_nPrimType      = pSubset->m_nPrimType;
            subset.m_nNumIndices    = pSubset->m_nNumIndices;
            subset.m_nIndexOffset   = pSubset->m_nIndexOffset;

			subset.m_vDiffuseColor  = pSubset->m_vDiffuseColor;
            subset.m_vSpecularColor = pSubset->m_vSpecularColor;
            subset.m_vAmbientColor  = pSubset->m_vAmbientColor;
            subset.m_vEmissiveColor = pSubset->m_vEmissiveColor;

            subset.m_nNumTextures   = pSubset->m_nNumTextures;
			subset.m_pTextures      = (FRM_MESH_TEXTURE*)(pSubset->m_nNumTextures ? nTexturesOffset : 0);
			nTexturesOffset += sizeof(FRM_MESH_TEXTURE) * pSubset->m_nNumTextures;

	        subset.m_pBoneIds     = (UINT32*)nBoneIDsOffset;
			nBoneIDsOffset += sizeof(UINT32) * pFrames[i].m_pMesh->m_nNumBoneMatrices;

            fwrite( &subset, sizeof(FRM_MESH_SUBSET), 1, file );
        }
    }
x = ftell( file );

    // Write the mesh bones
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        for( UINT32 j=0; j<pMesh->m_nNumBones; j++ )
        {
            FRM_MESH_BONE* pBone = &pMesh->m_pBones[j];

            FRM_MESH_BONE bone;
            memset( &bone, 0, sizeof(FRM_MESH_BONE) );
			bone.m_pFrame = (FRM_MESH_FRAME*)((UINT32)pBone->m_pFrame - (UINT32)pFrames );
			bone.m_matBoneOffset = pBone->m_matBoneOffset;

            fwrite( &bone, sizeof(FRM_MESH_BONE), 1, file );
        }
    }
x = ftell( file );

    // Write the texture data
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
        {
            FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];

            for( UINT32 k=0; k<pSubset->m_nNumTextures; k++ )
            {
                FRM_MESH_TEXTURE texture;
                memset( &texture, 0, sizeof(FRM_MESH_TEXTURE) );

				strcpy( texture.m_strTexture, pSubset->m_pTextures[k].m_strTexture );

				fwrite( &texture, sizeof(FRM_MESH_TEXTURE), 1, file );
            }
        }
    }
x = ftell( file );

    // Write the bone IDs
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
        {
            FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];

			fwrite( pSubset->m_pBoneIds, sizeof(UINT32), pMesh->m_nNumBoneMatrices, file );
        }
    }
x = ftell( file );

    // Write the vertex data
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;
        fwrite( pMesh->m_pVertexData, pMesh->m_nNumVertices * pMesh->m_nVertexSize, 1, file );
    }
x = ftell( file );

    // Write the index data
    for( UINT32 i=0; i < nNumFrames; i++ )	
    {
        FRM_MESH* pMesh = pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;
        fwrite( pMesh->m_pIndexData, pMesh->m_nNumIndices * pMesh->m_nIndexSize, 1, file );
    }
x = ftell( file );

    fclose( file );

    return TRUE;
}

