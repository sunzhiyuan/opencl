//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#if !defined(COMPRESSED_MESH_H_021201_DC)
#define COMPRESSED_MESH_H_021201_DC

#include "FrmMath.h"
class CFrmMesh;
typedef unsigned short FLOAT16;

class CompressedMesh
{
	public:
	CompressedMesh()
    {
        m_pBufferData = NULL;
        m_pSystemData = NULL;
        m_pFrames     = NULL;
        m_nNumFrames  = 0;
    }

    ~CompressedMesh()
    {
        Destroy();
    }
	bool CompressionTransformPosition8bitMakeDrawable();
	bool CompressionTransformPosition16bitMakeDrawable();
	bool SlidingCompressionTransformPosition24bitbitMakeDrawable();
	bool CompressionTransformPosition101012MakeDrawable();


    bool Load( const CHAR* strFileName );
    void Destroy();

	const FRMMATRIX4X4& GetCTMatrix(){ return m_ctMatrix;}
 
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
		default:
			offset = 8;
		}

		FrmSetVertexArrayObect(m_hVertexArrayObject);

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


		FrmSetVertexArrayObect(NULL);
		glDisableVertexAttribArray(pMesh->m_VertexLayout[0].nUsage);
		glDisableVertexAttribArray(pMesh->m_VertexLayout[1].nUsage);
		FrmSetVertexBuffer(NULL);
		FrmSetIndexBuffer(NULL);		

		FrmSetVertexArrayObect(m_hVertexArrayObject);	

        FrmDrawIndexedVertices( pSubset->m_nPrimType, 
			pMesh->m_nNumIndices,//pSubset->m_nNumIndices, 
                                pMesh->m_nIndexSize, pSubset->m_nIndexOffset );

		
		
       
    }
protected:

	enum SW_AXIS
	{
		SWA_X,
		SWA_Y,
		SWA_Z
	};

	bool DetermineScaleAndOffset(	FRMMATRIX4X4& transform, 
												FRMVECTOR3& scale, 
												FRMVECTOR3& offset );
	bool DetermineRotationMatrix( FRMMATRIX4X4& matrix);

	void QuantiseNormal( const FRMVECTOR3& in, unsigned char& x, unsigned char& y, unsigned char& z);	
	void CompressionTransformPosition8bit( const FRMVECTOR3& in, unsigned char& x, unsigned char& y, unsigned char& z);

	void CompressionTransformPosition16bit( const FRMVECTOR3& in,
													    FLOAT16& x, FLOAT16& y, FLOAT16&z);//, FLOAT16& w);
	void SlidingCompressionTransformPosition24bit(	const FRMVECTOR3& pos,
																	const FRMVECTOR3& norm,
																	unsigned int* outStream);

	void CreateSwapMatrix( FRMMATRIX4X4& in, SW_AXIS xAxis, SW_AXIS yAxis, SW_AXIS zAxis );
	unsigned int CompressionTransformPosition101012( const FRMVECTOR3& in );
	unsigned int CompressionTransformPosition101010( const FRMVECTOR3& in );
	unsigned int CompressVector3to101010Rev( float x, float y, float z );

    UINT32          m_nSystemDataSize;
    UINT32          m_nBufferDataSize;
    BYTE*           m_pSystemData;
    BYTE*           m_pBufferData;

	FRMMATRIX4X4				m_ctMatrix;		// compression transform matrix

    FRM_MESH_FRAME* m_pFrames;
    UINT32          m_nNumFrames;

	// core 3.2 req
	UINT32			   m_hVertexArrayObject;
};

#endif