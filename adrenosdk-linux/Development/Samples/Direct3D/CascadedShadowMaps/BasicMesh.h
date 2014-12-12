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
#include <Direct3D/FrmMesh.h>
#include <vector>

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
        m_pVertexBuffer = NULL;
        m_pIndexBuffer = NULL;
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
    VOID Render(CFrmShaderProgramD3D* shader)
    {
		FRM_MESH* pMesh = m_pFrames[0].m_pMesh;//working on frame 0 only for now
		FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[0];//and first subset

		unsigned int offset;
		//determine offset for normals
		switch(pMesh->m_VertexLayout[0].nType)
		{
        case 0x1406: //GL_FLOAT:
			offset = 12;
			break;
		default:
			offset = 8;
		}
		
        //vert
        m_pVertexBuffer->Bind(0);
        m_pIndexBuffer->Bind(0);
		
		shader->SetVertexLayout( pMesh->m_VertexLayout, pMesh->m_nVertexSize );
        shader->Bind();

		
        FrmDrawIndexedVertices( FrmGLPrimTypeToD3D(pSubset->m_nPrimType), pMesh->m_nNumIndices, 
                                pMesh->m_nIndexSize, pSubset->m_nIndexOffset );

               
    }
protected:

    CFrmVertexBuffer* m_pVertexBuffer;
    CFrmIndexBuffer*  m_pIndexBuffer;

    UINT32          m_nSystemDataSize;
    UINT32          m_nBufferDataSize;
    BYTE*           m_pSystemData;
    BYTE*           m_pBufferData;

    FRM_MESH_FRAME* m_pFrames;
    UINT32          m_nNumFrames;
};

#endif