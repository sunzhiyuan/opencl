//--------------------------------------------------------------------------------------
// ReadMsh1.cpp
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


BOOL ReadMesh1( const CHAR* strFileName, CFrmExtendedMesh* pDstMesh )
{
    static MSH1_FORMAT::CMesh srcMesh;
    if( FALSE == srcMesh.Load( strFileName ) )
        return FALSE;

    pDstMesh->m_nNumFrames = srcMesh.m_nNumFrames;
    pDstMesh->m_pFrames    = new MSH1_FORMAT::FRM_MESH_FRAME[pDstMesh->m_nNumFrames];

    // Copy the mesh frames
    for( UINT32 i=0; i < srcMesh.m_nNumFrames; i++ )	
    {
        MSH1_FORMAT::FRM_MESH_FRAME* pOldFrame = &srcMesh.m_pFrames[i];
        MSH1_FORMAT::FRM_MESH_FRAME* pNewFrame = &pDstMesh->m_pFrames[i];

        memset( pNewFrame, 0, sizeof(*pNewFrame) );

        strcpy( pNewFrame->m_strName, pOldFrame->m_strName );

        pNewFrame->m_matTransform   = pOldFrame->m_matTransform;
		pNewFrame->m_matBoneOffset2 = pOldFrame->m_matBoneOffset2;
		pNewFrame->m_matCombined    = pOldFrame->m_matCombined;

        if( pOldFrame->m_pMesh )
            pNewFrame->m_pMesh  = new MSH1_FORMAT::FRM_MESH;

        if( pOldFrame->m_pNext )
            pNewFrame->m_pNext  = &pDstMesh->m_pFrames[pOldFrame->m_pNext - srcMesh.m_pFrames ]; 
        
        if( pOldFrame->m_pChild )
            pNewFrame->m_pChild = &pDstMesh->m_pFrames[pOldFrame->m_pChild - srcMesh.m_pFrames ]; 
    }

    BYTE* pOldBufferData = srcMesh.m_pBufferData;

    for( UINT32 i=0; i < srcMesh.m_nNumFrames; i++ )	
    {
        MSH1_FORMAT::FRM_MESH* pOldMesh = srcMesh.m_pFrames[i].m_pMesh;
        MSH1_FORMAT::FRM_MESH* pNewMesh = pDstMesh->m_pFrames[i].m_pMesh;

		if( pNewMesh )
		{
			memset( pNewMesh, 0, sizeof(*pNewMesh) );

			memcpy( pNewMesh->m_VertexLayout, pOldMesh->m_VertexLayout, sizeof(pNewMesh->m_VertexLayout) );

			pNewMesh->m_nNumVertices = pOldMesh->m_nNumVertices;
			pNewMesh->m_nVertexSize  = pOldMesh->m_nVertexSize;
			pNewMesh->m_pVertexData  = new BYTE[pNewMesh->m_nNumVertices * pNewMesh->m_nVertexSize ];
			memcpy( pNewMesh->m_pVertexData, pOldBufferData, pNewMesh->m_nNumVertices * pNewMesh->m_nVertexSize );
			pOldBufferData += pOldMesh->m_nNumVertices * pOldMesh->m_nVertexSize;

			pNewMesh->m_nNumIndices  = pOldMesh->m_nNumIndices;
			pNewMesh->m_nIndexSize   = pOldMesh->m_nIndexSize;
			pNewMesh->m_pIndexData   = new BYTE[pNewMesh->m_nNumIndices * pNewMesh->m_nIndexSize ];
			memcpy( pNewMesh->m_pIndexData, pOldBufferData, pNewMesh->m_nNumIndices * pNewMesh->m_nIndexSize );
			pOldBufferData += pOldMesh->m_nNumIndices * pOldMesh->m_nIndexSize;

			pNewMesh->m_nNumBonesPerVertex = pOldMesh->m_nNumBonesPerVertex;
			pNewMesh->m_nNumBoneMatrices   = pOldMesh->m_nNumBoneMatrices;
			pNewMesh->m_nNumBones          = pOldMesh->m_nNumBones;
			pNewMesh->m_pBones = new MSH1_FORMAT::FRM_MESH_BONE[pNewMesh->m_nNumBones];
			for( UINT32 j=0; j<pOldMesh->m_nNumBones; j++ )
			{
				memset( &pNewMesh->m_pBones[j], 0, sizeof(pNewMesh->m_pBones[j]) );
				pNewMesh->m_pBones[j].m_pFrame = &pDstMesh->m_pFrames[pOldMesh->m_pBones[j].m_pFrame - srcMesh.m_pFrames ]; 
				pNewMesh->m_pBones[j].m_matBoneOffset = pOldMesh->m_pBones[j].m_matBoneOffset;
			}

			pNewMesh->m_nNumSubsets = pOldMesh->m_nNumSubsets;
			pNewMesh->m_pSubsets    = new MSH1_FORMAT::FRM_MESH_SUBSET[pNewMesh->m_nNumSubsets];
			for( UINT32 j=0; j<pOldMesh->m_nNumSubsets; j++ )
			{
				MSH1_FORMAT::FRM_MESH_SUBSET* pOldSubset = &pOldMesh->m_pSubsets[j];
				MSH1_FORMAT::FRM_MESH_SUBSET* pNewSubset = &pNewMesh->m_pSubsets[j];

				memset( pNewSubset, 0, sizeof(*pNewSubset) );

				pNewSubset->m_nPrimType      = pOldSubset->m_nPrimType;
				pNewSubset->m_nNumIndices    = pOldSubset->m_nNumIndices;
				pNewSubset->m_nIndexOffset   = pOldSubset->m_nIndexOffset;

				pNewSubset->m_vDiffuseColor  = pOldSubset->m_vDiffuseColor;
				pNewSubset->m_vSpecularColor = pOldSubset->m_vSpecularColor;
				pNewSubset->m_vAmbientColor  = pOldSubset->m_vAmbientColor;
				pNewSubset->m_vEmissiveColor = pOldSubset->m_vEmissiveColor;

				if( pNewMesh->m_nNumBoneMatrices )
				{
					pNewSubset->m_pBoneIds = new UINT32[pNewMesh->m_nNumBoneMatrices];
					for( UINT32 k=0; k<pOldMesh->m_nNumBoneMatrices; k++ )
					{
						pNewSubset->m_pBoneIds[k] = pOldSubset->m_pBoneIds[k];
					}
				}

				pNewSubset->m_nNumTextures   = pOldSubset->m_nNumTextures;
				pNewSubset->m_pTextures      = new MSH1_FORMAT::FRM_MESH_TEXTURE[pNewSubset->m_nNumTextures];
				for( UINT32 k=0; k<pOldSubset->m_nNumTextures; k++ )
				{
					MSH1_FORMAT::FRM_MESH_TEXTURE* pOldTexture = &pOldSubset->m_pTextures[k];
					MSH1_FORMAT::FRM_MESH_TEXTURE* pNewTexture = &pNewSubset->m_pTextures[k];

					memset( pNewTexture, 0, sizeof(*pNewTexture) );
					strcpy( pNewTexture->m_strTexture, pOldTexture->m_strTexture );
				}
			}
		}
    }

    return TRUE;
}


// Wrap everything in a namespace, to prevent name conflicts
namespace MSH1_FORMAT
{


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMesh::Load( const CHAR* strFileName )
{
    // Open the file
    FILE* file = fopen( strFileName, "rb" );
    if( NULL == file )
        return FALSE;

    // Read in the mesh header
    FRM_MESH_FILE_HEADER header;
    fread( &header, sizeof(header), 1, file );
    if( header.nMagic != FRM_MESH_MAGIC_ID )
    {
        fclose( file );
        return FALSE;
    }
    m_nNumFrames      = header.nNumFrames;
    m_nSystemDataSize = header.nSystemDataSize;
    m_nBufferDataSize = header.nBufferDataSize;

    // Read in the data
    m_pSystemData = new BYTE[m_nSystemDataSize];
    m_pBufferData = new BYTE[m_nBufferDataSize];
    fread( m_pSystemData, m_nSystemDataSize, 1, file );
    fread( m_pBufferData, m_nBufferDataSize, 1, file );

    // Done with the file
    fclose( file );

    // Fix up pointers
    m_pFrames = (FRM_MESH_FRAME*)m_pSystemData;

    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH_FRAME* pFrame = &m_pFrames[i];

        // Fix up child and next pointers
        if( pFrame->m_pChild )
            pFrame->m_pChild = (FRM_MESH_FRAME*)( m_pSystemData + (UINT32)pFrame->m_pChild );
        if( pFrame->m_pNext )
            pFrame->m_pNext = (FRM_MESH_FRAME*)( m_pSystemData + (UINT32)pFrame->m_pNext );

		if( pFrame->m_pMesh )
        {
            // Fix up mesh pointer
            pFrame->m_pMesh = (FRM_MESH*)( m_pSystemData + (UINT32)pFrame->m_pMesh );
            FRM_MESH* pMesh = pFrame->m_pMesh;

            // Fix up bones pointer
            if( pMesh->m_pBones )
            {
                pMesh->m_pBones = (FRM_MESH_BONE*)( m_pSystemData + (UINT32)pMesh->m_pBones );
                FRM_MESH_BONE* pBones = pMesh->m_pBones;

                for( UINT32 j=0; j<pMesh->m_nNumBones; j++ )
                {
                    pBones[j].m_pFrame = (FRM_MESH_FRAME*)( m_pSystemData + (UINT32)pBones[j].m_pFrame );
                }
            }

            // Fix up subsets pointer
            if( pMesh->m_pSubsets )
            {
                pMesh->m_pSubsets = (FRM_MESH_SUBSET*)( m_pSystemData + (UINT32)pMesh->m_pSubsets );
                FRM_MESH_SUBSET* pSubsets = pMesh->m_pSubsets;

                for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
                {
                    // Fix up texture pointers
                    if( pSubsets[j].m_pTextures )
                        pSubsets[j].m_pTextures = (FRM_MESH_TEXTURE*)( m_pSystemData + (UINT32)pSubsets[j].m_pTextures );

                    // Fix up bone subset pointers
                    if( pSubsets[j].m_pBoneIds )
                        pSubsets[j].m_pBoneIds = (UINT32*)( m_pSystemData + (UINT32)pSubsets[j].m_pBoneIds );
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
VOID CMesh::Destroy()
{
    if( m_pBufferData ) delete[] m_pBufferData;
    if( m_pSystemData ) delete[] m_pSystemData;

    m_pBufferData = NULL;
    m_pSystemData = NULL;
    m_pFrames     = NULL;
    m_nNumFrames  = 0;
}


// End of namespace
}

