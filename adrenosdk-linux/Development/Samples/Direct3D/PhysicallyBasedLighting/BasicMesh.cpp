//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "FrmPlatform.h"
#include "FrmUtils.h"
#include "Direct3D/FrmMesh.h"
#include "BasicMesh.h"
#include "FrmFile.h"

static const int VERT_SIZE_UNCOMPRESSED = 8;

bool BasicMesh::Load( const CHAR* strFileName )
{
    // Use internal structs with UINT32 in place of pointers and
    // then remap so that it works with 64-bit as well as 32-bit pointers
    // (32-bit and 64-bit builds of apps)
    struct FRM_MESH_FRAME_INTERNAL
    {
        CHAR               m_strName[32];

        FRMMATRIX4X4       m_matTransform;
        FRMMATRIX4X4       m_matBoneOffset2;
        FRMMATRIX4X4       m_matCombined;

        UINT32             m_pMesh;
        UINT32             m_pAnimationData;

        UINT32             m_pChild;
        UINT32             m_pNext;
    };

    struct FRM_MESH_INTERNAL
    {
        FRM_VERTEX_ELEMENT m_VertexLayout[8];
        UINT32             m_Reserved;

        UINT32             m_nNumVertices;
        UINT16             m_nVertexSize;
        UINT32             m_hVertexBuffer;

        UINT32             m_nNumIndices;
        UINT16             m_nIndexSize;
        UINT32             m_hIndexBuffer;

        UINT32             m_nNumBonesPerVertex;
        UINT32             m_nNumBoneMatrices;
        UINT32             m_nNumBones;
        UINT32             m_pBones;

        UINT32             m_nNumSubsets;
        UINT32             m_pSubsets;
    };

    struct FRM_MESH_BONE_INTERNAL
    {
        UINT32            m_pFrame;             // The mesh frame associated with this bone
        FRMMATRIX4X4      m_matBoneOffset;
    };

    struct FRM_MESH_SUBSET_INTERNAL
    {
        UINT32             m_nPrimType;
        UINT32             m_nNumIndices;
        UINT32             m_nIndexOffset;

        UINT32             m_pBoneIds;

        FRMVECTOR4         m_vDiffuseColor;
        FRMVECTOR4         m_vSpecularColor;
        FRMVECTOR3         m_vAmbientColor;
        FRMVECTOR3         m_vEmissiveColor;

        UINT32             m_nNumTextures;
        UINT32             m_pTextures;
    };

    struct FRM_MESH_TEXTURE_INTERNAL
    {
        CHAR               m_strTexture[32];
        UINT32             m_pTexture;
    };

    // Open the file
    FRM_FILE* file;
    if( FALSE == FrmFile_Open( strFileName, FRM_FILE_READ, &file ) )
        return FALSE;

    // Read in the mesh header
    FRM_MESH_FILE_HEADER header;
    FrmFile_Read( file, &header, sizeof(header) );
    if( header.nMagic != FRM_MESH_MAGIC_ID )
    {
        FrmFile_Close( file );
        return FALSE;
    }
    m_nNumFrames      = header.nNumFrames;
    m_nSystemDataSize = header.nSystemDataSize;
    m_nBufferDataSize = header.nBufferDataSize;

    // Read in the data
    m_pSystemData = new BYTE[m_nSystemDataSize];
    m_pBufferData = new BYTE[m_nBufferDataSize];
    FrmFile_Read( file, m_pSystemData, m_nSystemDataSize );
    FrmFile_Read( file, m_pBufferData, m_nBufferDataSize );

    // Done with the file
    FrmFile_Close( file );

    // Fix up pointers
    FRM_MESH_FRAME_INTERNAL *pFramesInternal = (FRM_MESH_FRAME_INTERNAL*)m_pSystemData;
    m_pFrames = new FRM_MESH_FRAME [ m_nNumFrames ];

    UINT32 meshFrmSize = sizeof(FRM_MESH_FRAME);
    UINT32 meshFrmSizeInternal = sizeof(FRM_MESH_FRAME_INTERNAL);
    UINT32 meshFrmSizeDiff = ( meshFrmSize - meshFrmSizeInternal );
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH_FRAME *pFrame = &m_pFrames[i];
        FRM_MESH_FRAME_INTERNAL* pFrameInternal = &pFramesInternal[i];

        // Copy frame data
        ZeroMemory( pFrame, sizeof(FRM_MESH_FRAME) );
        memcpy( pFrame, pFrameInternal, offsetof( FRM_MESH_FRAME, m_matCombined ) );
        
        // Fix up child and next pointers
        if( pFrameInternal->m_pChild )
        {
            UINT32 numOffsetFrames = pFrameInternal->m_pChild / meshFrmSizeInternal;
            pFrame->m_pChild = (FRM_MESH_FRAME*)( &m_pFrames[numOffsetFrames] );
        }
        if( pFrameInternal->m_pNext )
        {
            UINT32 numOffsetFrames = pFrameInternal->m_pNext / meshFrmSizeInternal;
            pFrame->m_pNext = (FRM_MESH_FRAME*)( &m_pFrames[numOffsetFrames] );
        }

        if( pFrameInternal->m_pMesh )
        {
            FRM_MESH_INTERNAL *pMeshInternal = (FRM_MESH_INTERNAL*)( m_pSystemData + (UINT32)pFrameInternal->m_pMesh );

            pFrame->m_pMesh = new FRM_MESH;
            ZeroMemory( pFrame->m_pMesh, sizeof(FRM_MESH) );
            memcpy( pFrame->m_pMesh, pMeshInternal, offsetof( FRM_MESH_INTERNAL, m_nNumBones ) );  
            pFrame->m_pMesh->m_nNumSubsets = pMeshInternal->m_nNumSubsets;

            // Fix up mesh pointer
            FRM_MESH* pMesh = pFrame->m_pMesh;

            // Fix up bones pointer
            if( pMeshInternal->m_pBones )
            {
                FRM_MESH_BONE_INTERNAL *pBonesInternal = (FRM_MESH_BONE_INTERNAL*)( m_pSystemData + (UINT32)pMeshInternal->m_pBones );
                pMesh->m_pBones = new FRM_MESH_BONE[ pMesh->m_nNumBones ];

                for( UINT32 j=0; j<pMesh->m_nNumBones; j++ )
                {
                    pMesh->m_pBones[j].m_matBoneOffset = pBonesInternal->m_matBoneOffset;
                    UINT32 numOffsetFrames = pBonesInternal[j].m_pFrame / meshFrmSizeInternal;

                    pMesh->m_pBones[j].m_pFrame = (FRM_MESH_FRAME*)( &m_pFrames[numOffsetFrames] );
                }
            }

            // Fix up subsets pointer
            if( pMeshInternal->m_pSubsets )
            {
                FRM_MESH_SUBSET_INTERNAL *pSubsetsInternal = (FRM_MESH_SUBSET_INTERNAL*)( m_pSystemData + (UINT32)pMeshInternal->m_pSubsets );
                pMesh->m_pSubsets = new FRM_MESH_SUBSET[ pMesh->m_nNumSubsets ];                

                FRM_MESH_SUBSET* pSubsets = pMesh->m_pSubsets;

                for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
                {

                    pSubsets[j].m_nPrimType = pSubsetsInternal[j].m_nPrimType;
                    pSubsets[j].m_nNumIndices = pSubsetsInternal[j].m_nNumIndices;
                    pSubsets[j].m_nIndexOffset = pSubsetsInternal[j].m_nIndexOffset;
                    pSubsets[j].m_vDiffuseColor = pSubsetsInternal[j].m_vDiffuseColor;
                    pSubsets[j].m_vSpecularColor = pSubsetsInternal[j].m_vSpecularColor;
                    pSubsets[j].m_vAmbientColor = pSubsetsInternal[j].m_vAmbientColor;
                    pSubsets[j].m_vEmissiveColor = pSubsetsInternal[j].m_vEmissiveColor;
                    pSubsets[j].m_nNumTextures = pSubsetsInternal[j].m_nNumTextures;
                    pSubsets[j].m_pTextures = NULL;
                    pSubsets[j].m_pBoneIds = NULL;

                    // Fix up texture pointers
                    if( pSubsetsInternal[j].m_pTextures )
                    {
                        FRM_MESH_TEXTURE_INTERNAL* pTexturesInternal = (FRM_MESH_TEXTURE_INTERNAL*)( m_pSystemData + (UINT32)pSubsetsInternal[j].m_pTextures );                        
                        
                        pSubsets[j].m_pTextures = new FRM_MESH_TEXTURE[ pSubsets[j].m_nNumTextures ];
                        for ( UINT32 k = 0; k < pSubsets[j].m_nNumTextures; k++ )
                        {
                            memcpy( &pSubsets[j].m_pTextures[k].m_strTexture[0], &pTexturesInternal[k].m_strTexture[0], sizeof(pTexturesInternal[k].m_strTexture) );
                            pSubsets[j].m_pTextures[k].m_pTexture = NULL;
                        }                        
                    }

                    // Fix up bone subset pointers
                    if( pSubsetsInternal[j].m_pBoneIds )
                    {
                        UINT32* pBoneIdsInternal = (UINT32*)( m_pSystemData + (UINT32)pSubsetsInternal[j].m_pBoneIds );
                        pSubsets[j].m_pBoneIds = new UINT32[ pMesh->m_nNumBoneMatrices ];
                        memcpy( pSubsets[j].m_pBoneIds, pBoneIdsInternal, sizeof(UINT32) * pMesh->m_nNumBoneMatrices );
                    }
                }
            }
        }
    }

    return TRUE;
}


VOID BasicMesh::Destroy()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh = m_pFrames[i].m_pMesh;
        if( pMesh )
        {
            // Release vertex and index buffers
            if( m_pBufferData != NULL )
            {
                if ( m_pVertexBuffer) m_pVertexBuffer->Release();
                m_pVertexBuffer = NULL;
                if ( m_pIndexBuffer ) m_pIndexBuffer->Release();
                m_pIndexBuffer = NULL;
            }
            
            // Release any textures
            for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
            {
                FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];
                for( UINT32 k=0; k<pSubset->m_nNumTextures; k++ )
                {
                    if( pSubset->m_pTextures[k].m_pTexture )
                        pSubset->m_pTextures[k].m_pTexture->Release();
                }

                if ( pSubset->m_pTextures ) delete [] pSubset->m_pTextures;
                pSubset->m_pTextures = NULL;

                if ( pSubset->m_pBoneIds )  delete [] pSubset->m_pBoneIds;
                pSubset->m_pBoneIds = NULL;
            }

            if ( pMesh->m_pBones )      delete [] pMesh->m_pBones;
            if ( pMesh->m_pSubsets )    delete [] pMesh->m_pSubsets;

            pMesh->m_pBones = NULL;
            pMesh->m_pSubsets = NULL;
        }
    }

    if( m_pFrames )     delete [] m_pFrames;
    if( m_pBufferData ) delete[] m_pBufferData;
    if( m_pSystemData ) delete[] m_pSystemData;

    m_pBufferData = NULL;
    m_pSystemData = NULL;
    m_pFrames     = NULL;
    m_nNumFrames  = 0;
}
bool BasicMesh::MakeDrawable()
{
	bool bRet = true;
	unsigned int i;

	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	float* outVerts = new float[pMesh->m_nNumVertices * 6];//6*2];
	float* outStream = outVerts;

	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		// scale and offset position
		FRMVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		outStream[0] = pos.x;
		outStream[1] = pos.y;
		outStream[2] = pos.z;

		// normalize normals 
		float fx = inStream[3];
		float fy = inStream[4];
		float fz = inStream[5];

		FRMVECTOR3 n = FrmVector3Normalize(FRMVECTOR3(fx,fy,fz));
		
		outStream[3] = n.x;
		outStream[4] = n.y;
		outStream[5] = n.z;

		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;
		outStream += 6;
	}

	//now prepare the vertex layout accordingly 
	pMesh->m_VertexLayout[0].bNormalized = false;
	pMesh->m_VertexLayout[0].nSize = 3;
	pMesh->m_VertexLayout[0].nStride = 12;
	pMesh->m_VertexLayout[0].nType = 0x1406; //GL_FLOAT;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].bNormalized = false;
	pMesh->m_VertexLayout[1].nSize = 3;
	pMesh->m_VertexLayout[1].nStride = 12;
	pMesh->m_VertexLayout[1].nType = 0x1406; //GL_FLOAT;
	pMesh->m_VertexLayout[1].nUsage = FRM_VERTEX_NORMAL;

	pMesh->m_VertexLayout[2].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 24;//6;

    // Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        outVerts, &m_pVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, 
                                       &m_pIndexBuffer ) )
        bRet = 0;


	delete[] outVerts;

    return bRet;
}
