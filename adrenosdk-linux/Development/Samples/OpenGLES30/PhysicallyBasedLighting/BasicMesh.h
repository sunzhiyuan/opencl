//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if !defined(BASIC_MESH_H)
#define BASIC_MESH_H

#include "FrmMath.h"
class CFrmMesh;
typedef unsigned short FLOAT16;

class BasicMesh
{
	public:
	BasicMesh()
    {
        m_pBufferData = NULL;
        m_pSystemData = NULL;
        m_pFrames     = NULL;
        m_nNumFrames  = 0;
    }

    ~BasicMesh()
    {
        Destroy();
    }
	bool MakeDrawable();

    bool Load( const CHAR* strFileName );
    void Destroy();

    FRM_MESH* GetMesh( UINT32 nFrame )
    {
        return m_pFrames[nFrame].m_pMesh;
    };
    VOID Render()
    {
		FRM_MESH* pMesh = m_pFrames[0].m_pMesh;//working on frame 0 only for now
		FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[0];//and first subset

		unsigned int offset;
		//determine offset for normals
		switch(pMesh->m_VertexLayout[0].nType)
		{
		case GL_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			offset = 4;
			break;
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
			offset = 3;
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
		case GL_HALF_FLOAT:
			offset = 6;
			break;
		case GL_FLOAT:
			offset = 12;
			break;
		default:
			offset = 8;
		}

		//vert
		FrmSetVertexBuffer( pMesh->m_hVertexBuffer );

		if( pMesh->m_VertexLayout[0].nType == GL_UNSIGNED_INT )
		{
			glVertexAttribIPointer( pMesh->m_VertexLayout[0].nUsage,   pMesh->m_VertexLayout[0].nSize, pMesh->m_VertexLayout[0].nType, pMesh->m_VertexLayout[0].nStride, 0 );
		}else
		{
			glVertexAttribPointer( pMesh->m_VertexLayout[0].nUsage,   pMesh->m_VertexLayout[0].nSize, pMesh->m_VertexLayout[0].nType, pMesh->m_VertexLayout[0].bNormalized, pMesh->m_VertexLayout[0].nStride, 0 );
		}
		glEnableVertexAttribArray(  pMesh->m_VertexLayout[0].nUsage);
		//norm
		if(pMesh->m_VertexLayout[1].nSize)//some compression packs normals into the verts, so skip if nSize==0
		{
			glVertexAttribPointer( pMesh->m_VertexLayout[1].nUsage,   pMesh->m_VertexLayout[1].nSize, pMesh->m_VertexLayout[1].nType , pMesh->m_VertexLayout[1].bNormalized,  pMesh->m_VertexLayout[1].nStride, (void *)offset );
			glEnableVertexAttribArray(  pMesh->m_VertexLayout[1].nUsage);
		}
		
		FrmSetIndexBuffer( pMesh->m_hIndexBuffer );

        FrmDrawIndexedVertices( pSubset->m_nPrimType, 
			pMesh->m_nNumIndices,//pSubset->m_nNumIndices, 
                                pMesh->m_nIndexSize, pSubset->m_nIndexOffset );

		FrmSetVertexBuffer( NULL );
		
       
    }
protected:

    UINT32          m_nSystemDataSize;
    UINT32          m_nBufferDataSize;
    BYTE*           m_pSystemData;
    BYTE*           m_pBufferData;

	FRMMATRIX4X4				m_ctMatrix;		// compression transform matrix

    FRM_MESH_FRAME* m_pFrames;
    UINT32          m_nNumFrames;
};

#endif