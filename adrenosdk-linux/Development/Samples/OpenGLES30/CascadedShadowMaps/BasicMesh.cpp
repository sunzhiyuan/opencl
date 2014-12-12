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
#include "OpenGLES/FrmMesh.h"
#include "BasicMesh.h"
#include "FrmFile.h"

static const int VERT_SIZE_UNCOMPRESSED = 8;

bool BasicMesh::Load( const CHAR* strFileName )
{
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
    m_pFrames = (FRM_MESH_FRAME*)m_pSystemData;

    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH_FRAME* pFrame = &m_pFrames[i];

        // Fix up child and next pointers
        if( pFrame->m_pChild )
            pFrame->m_pChild = (FRM_MESH_FRAME*)( m_pSystemData + (long)pFrame->m_pChild );
        if( pFrame->m_pNext )
            pFrame->m_pNext = (FRM_MESH_FRAME*)( m_pSystemData + (long)pFrame->m_pNext );

        if( pFrame->m_pMesh )
        {
            // Fix up mesh pointer
            pFrame->m_pMesh = (FRM_MESH*)( m_pSystemData + (long)pFrame->m_pMesh );
            FRM_MESH* pMesh = pFrame->m_pMesh;

            // Fix up bones pointer
            if( pMesh->m_pBones )
            {
                pMesh->m_pBones = (FRM_MESH_BONE*)( m_pSystemData + (long)pMesh->m_pBones );
                FRM_MESH_BONE* pBones = pMesh->m_pBones;

                for( UINT32 j=0; j<pMesh->m_nNumBones; j++ )
                {
                    pBones[j].m_pFrame = (FRM_MESH_FRAME*)( m_pSystemData + (long)pBones[j].m_pFrame );
                }
            }

            // Fix up subsets pointer
            if( pMesh->m_pSubsets )
            {
                pMesh->m_pSubsets = (FRM_MESH_SUBSET*)( m_pSystemData + (long)pMesh->m_pSubsets );
                FRM_MESH_SUBSET* pSubsets = pMesh->m_pSubsets;

                for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
                {
                    // Fix up texture pointers
                    if( pSubsets[j].m_pTextures )
                        pSubsets[j].m_pTextures = (FRM_MESH_TEXTURE*)( m_pSystemData + (long)pSubsets[j].m_pTextures );

                    // Fix up bone subset pointers
                    if( pSubsets[j].m_pBoneIds )
                        pSubsets[j].m_pBoneIds = (UINT32*)( m_pSystemData + (long)pSubsets[j].m_pBoneIds );
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
            if( m_pBufferData == NULL )
            {
                glDeleteBuffers( 1, &pMesh->m_hVertexBuffer );
                glDeleteBuffers( 1, &pMesh->m_hIndexBuffer );
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
            }
        }
    }

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

		FRMVECTOR3 n = FRMVECTOR3( fx, fy, fz );
		n = FrmVector3Normalize( n );
		
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
	pMesh->m_VertexLayout[0].nStride = 24;
	pMesh->m_VertexLayout[0].nType = GL_FLOAT;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].bNormalized = false;
	pMesh->m_VertexLayout[1].nSize = 3;
	pMesh->m_VertexLayout[1].nStride = 24;
	pMesh->m_VertexLayout[1].nType = GL_FLOAT;
	pMesh->m_VertexLayout[1].nUsage = FRM_VERTEX_NORMAL;

	pMesh->m_VertexLayout[2].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 24;//6;

	m_hVertexArrayObject = FrmCreateVertexArrayObject();

// Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        outVerts, &pMesh->m_hVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, &pMesh->m_hIndexBuffer ) )
        bRet = 0;


	delete[] outVerts;

	return bRet;
}
