//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
/*#include <EGL/egl.h>
#include <GLES3/GL3.h>
#include <GLES2/gl2ext.h>*/

#include <OpenGLES/FrmGLES3.h>  // OpenGL ES 3 headers here

#include "FrmPlatform.h"
#include "FrmUtils.h"
#include "OpenGLES/FrmMesh.h"
#include "CompressedMesh.h"
#include "FrmFile.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static const int VERT_SIZE_UNCOMPRESSED = 8;
// the eigen solver
extern void tri_diag(double a[], double d[], double e[], double z[], int n,double tol);
extern int calc_eigenstructure(double d[],double e[],double z[], int n, double macheps);

bool CompressedMesh::Load( const CHAR* strFileName )
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
VOID CompressedMesh::Destroy()
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

//--------------------------------------------------------------------------------------
// Name: CompressVector3to10_10_10()
// Desc: Helper function to compress vertex data
//--------------------------------------------------------------------------------------
unsigned int CompressedMesh::CompressVector3to101010Rev( float x, float y, float z )
{
    return ( ( ( (unsigned long)(x *511.0f) ) & 0x000003ff ) << 0L ) |
           ( ( ( (unsigned long)(y *511.0f) ) & 0x000003ff ) << 10L ) |
           ( ( ( (unsigned long)(z *511.0f) ) & 0x000003ff ) << 20L);
}
//-----------------------------------------------------------------------------
// Name: QuantiseNormal
// Desc: takes 3 floats and sets 3 bytes representing the input normal
//-----------------------------------------------------------------------------
void CompressedMesh::QuantiseNormal( const FRMVECTOR3& in, unsigned char& x, unsigned char& y, unsigned char& z)
{
	// -1.0 - 1.f -> 0.f - 255.f
	x = (unsigned char) ( (in.x * 127.f) + 128.f );
	y = (unsigned char) ( (in.y * 127.f) + 128.f );
	z = (unsigned char) ( (in.z * 127.f) + 128.f );
	
}
//--------------------------------------------------------------------------------------
// Name: CompressFloatToHalfFloat()
// Desc: Helper function to compress vertex data
//--------------------------------------------------------------------------------------
FLOAT16 CompressFloatToHalfFloat( float v )
{
    UINT32 src = *(unsigned long*)(&v);
    UINT32 sign = src >> 31;

    // Extract mantissa
    UINT32 mantissa = src  &  (1 << 23) - 1;

    // extract & convert exp bits
    long exp = (long)( src >> 23  &  0xFF ) - 127;
    if( exp > 16 )
    {
        // largest possible number...
        exp = 16;
        mantissa = (1 << 23) - 1;
    }
    else if( exp <= -15 )
    {
        // handle wraparound of shifts (done mod 64 on PPC)
        // All float32 denorm/0 values map to float16 0
        if( exp <= - 14 - 24 )
            mantissa = 0;
        else
            mantissa = ( mantissa | 1 << 23 )  >>  (-exp - 14);
        exp = -15;
    }

    return (FLOAT16)(sign<<15) | (FLOAT16)((exp+15)<<10) | (FLOAT16)(mantissa>>13);
}
//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bitMakeDrawable
// Desc: Compresses mesh with 16 bit rotated, scaled and offset positions 
//       using 101010 normals
//-----------------------------------------------------------------------------
bool CompressedMesh::CompressionTransformPosition16bitMakeDrawable()
{
	bool bRet = true;
	unsigned int i;

	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	FRMMATRIX4X4 rotMat, scaleMat, offsetMat;
	FRMVECTOR3 scale, offset;

	if(!DetermineRotationMatrix(rotMat))
		return false;

	rotMat = FrmMatrixInverse(rotMat);

	// determinte scale and offset
	DetermineScaleAndOffset(rotMat, scale, offset );

	//scale/offset
	offsetMat = FrmMatrixTranslate(offset.x, offset.y, offset.z);
	scaleMat = FrmMatrixScale(scale.x, scale.y, scale.z);

	//invert
	scaleMat =  FrmMatrixInverse(scaleMat);
	offsetMat =  FrmMatrixInverse(offsetMat);

	//keep translation invers out of w area of matrix
	offsetMat = FrmMatrixTranspose(offsetMat);


	// m = r-1 * o-1 * s-1 
	FRMMATRIX4X4 combined = FrmMatrixMultiply( offsetMat, scaleMat);
	combined = FrmMatrixMultiply( rotMat, combined);

	m_ctMatrix = combined;

	BYTE* compressedVerts = new BYTE[pMesh->m_nNumVertices *10];//6*2];
	FLOAT16* outStream= (FLOAT16*)compressedVerts;

	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		// scale and offset position
		FRMVECTOR3 pos( inStream[0], inStream[1], inStream[2] );
		CompressionTransformPosition16bit(pos, outStream[0],
											   outStream[1],
											   outStream[2]);
											  // outStream[3]);
		// normalize normals 
		float fx = inStream[3];
		float fy = inStream[4];
		float fz = inStream[5];
		FRMVECTOR3 n = FRMVECTOR3(fx,fy,fz);
		n = FrmVector3Normalize(n);
		

		this->QuantiseNormal(n, ((unsigned char*)outStream)[6], 
								((unsigned char*)outStream)[7], 
								((unsigned char*)outStream)[8]);

		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;
		outStream += 5;//keep it half float aligned (9 bytes rounded to 10)
	}

	//now prepare the vertex layout accordingly 
	pMesh->m_VertexLayout[0].bNormalized = false;
	pMesh->m_VertexLayout[0].nSize = 3;
	pMesh->m_VertexLayout[0].nStride = 10;
	pMesh->m_VertexLayout[0].nType = GL_UNSIGNED_SHORT;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].bNormalized = false;
	pMesh->m_VertexLayout[1].nSize = 3;
	pMesh->m_VertexLayout[1].nStride = 10;
	pMesh->m_VertexLayout[1].nType = GL_UNSIGNED_BYTE;
	pMesh->m_VertexLayout[1].nUsage = FRM_VERTEX_NORMAL;

	pMesh->m_VertexLayout[2].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 10;//6;

	m_hVertexArrayObject = FrmCreateVertexArrayObject();

// Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        compressedVerts, &pMesh->m_hVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, &pMesh->m_hIndexBuffer ) )
        bRet = 0;


	delete[] compressedVerts;

	return bRet;
}


//-----------------------------------------------------------------------------
// DetermineScaleAndOffset() 
//-----------------------------------------------------------------------------
bool CompressedMesh::DetermineScaleAndOffset( FRMMATRIX4X4& transform,
												FRMVECTOR3& scale, 
												FRMVECTOR3& offset )
{
	unsigned int i;

	//For now just grab fram 0
	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	FRMVECTOR3 lowerRange, upperRange;

	// determine scale and offset	
	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		FRMVECTOR3 vec(inStream[0], inStream[1], inStream[2]);
		vec = FrmVector3TransformCoord(vec, transform );

		if(i==0)//first loop set all the min/max
		{
			lowerRange.v[0] = vec.v[0];
			lowerRange.v[1] = vec.v[1];
			lowerRange.v[2] = vec.v[2];
			upperRange.v[0] = vec.v[0];
			upperRange.v[1] = vec.v[1];
			upperRange.v[2] = vec.v[2];
		}

		lowerRange.v[0] = min(vec.v[0], lowerRange.v[0]);
		lowerRange.v[1] = min(vec.v[1], lowerRange.v[1]);
		lowerRange.v[2] = min(vec.v[2], lowerRange.v[2]);

		upperRange.v[0] = max(vec.v[0], upperRange.v[0]);
		upperRange.v[1] = max(vec.v[1], upperRange.v[1]);
		upperRange.v[2] = max(vec.v[2], upperRange.v[2]);

		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;//onlys support verts packed as (xyz,(Nxyz),uv) for now (8 floats)
	}

	offset = lowerRange;
	scale = upperRange - lowerRange;

	return 1;
}

bool CompressedMesh::DetermineRotationMatrix(FRMMATRIX4X4& matrix)
{
	unsigned int i,j,k;

	//For now just grab fram 0
	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;
	short * inIBStream = (short *)(inStream + VERT_SIZE_UNCOMPRESSED*pMesh->m_nNumVertices);//HACK: jump past the verts and normals to find the indices

	double sum[3] = {0,0,0};
	unsigned int n = pMesh->m_nNumIndices/3; //in->GetNumFaces();

	// sum the position data from each triangle
	for(i=0;i < n;i++)
	{
		unsigned int i0 = inIBStream[(i * 3) + 0];
		unsigned int i1 = inIBStream[(i * 3) + 1];
		unsigned int i2 = inIBStream[(i * 3) + 2];

		sum[0] += inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 0];
		sum[1] += inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 1];
		sum[2] += inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 2];

		sum[0] += inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 0];
		sum[1] += inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 1];
		sum[2] += inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 2];

		sum[0] += inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 0];
		sum[1] += inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 1];
		sum[2] += inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 2];
	}

	// calculate u
	double u[3];
	u[0] = (1.f/(3.f * n)) * sum[0];
	u[1] = (1.f/(3.f * n)) * sum[1];
	u[2] = (1.f/(3.f * n)) * sum[2];

	double R[3][3]; // doubles will help due to numerical precision issues

	// calculate rotation matrix
	for(i=0;i < n;i++)
	{
		unsigned int i0 = inIBStream[(i * 3) + 0];
		unsigned int i1 = inIBStream[(i * 3) + 1];
		unsigned int i2 = inIBStream[(i * 3) + 2];

		double p[3],q[3],r[3];

		p[0] = inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 0] - u[0];
		p[1] = inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 1] - u[1];
		p[2] = inStream[(i0 * VERT_SIZE_UNCOMPRESSED) + 2] - u[2];

		q[0] = inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 0] - u[0];
		q[1] = inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 1] - u[1];
		q[2] = inStream[(i1 * VERT_SIZE_UNCOMPRESSED) + 2] - u[2];

		r[0] = inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 0] - u[0];
		r[1] = inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 1] - u[1];
		r[2] = inStream[(i2 * VERT_SIZE_UNCOMPRESSED) + 2] - u[2];

		for(unsigned int j = 0; j < 3; j++)
		{
			for(unsigned int k = 0; k < 3; k++)
			{
				R[j][k] =	(p[j]*p[k]) + 
							(q[j]*q[k]) +
							(r[j]*r[k]);
			}
		}
	}

	// average rotation sums
	for(j = 0; j < 3; j++)
	{
		for(k = 0; k < 3; k++)
		{
			R[j][k] = R[j][k] * (1.f/(3.f * n));
		}
	}

	// convert matrix
	double a[10],v[10];
	for(j = 0; j < 3; j++)
	{
		for(k = 0; k < 3; k++)
		{
			a[j * 3 + k+1] = R[j][k];
		}
	}

	// run it through an eigensolver
	// originally I used jacobi from Numerical Receipes, but that can't be
	// distributed, so I use this version.

	// tol and machineprecision are so high, because cube row taxes numerical precision
	// for 'proper' work at the scale of cube row, its time to dig out the numerical analsyse 
	// texts
	double ld[4], le[4];
	double tol = 1.e-30;
	tri_diag(a,ld,le,v,3,tol);

	double macheps = 1.e-30 ;//1.e-16;
	if( calc_eigenstructure(ld,le,v,3,macheps) == -1)
	{
		return 0;
	}

	// fill in D3D Matrix
	matrix = FrmMatrixIdentity();

	matrix.M(0,0) = (float)v[1]; matrix.M(1,0) = (float)v[2];	matrix.M(2,0) = (float)v[3];
	matrix.M(0,1) = (float)v[4]; matrix.M(1,1) = (float)v[5];	matrix.M(2,1) = (float)v[6];
	matrix.M(0,2) = (float)v[7]; matrix.M(1,2) = (float)v[8];	matrix.M(2,2) = (float)v[9];

	float f = 0.0f; ///sanity check to ensure each eigen vec is mag is 1
	for(i=0;i<4;i++)
	{
		f = 0.0f;
		for(j=0;j<4;j++)
		{
			f += matrix.M(i,j) * matrix.M(i,j);
		}
		f = FrmSqrt(f);
	}
	
	return 1;
}
void CompressedMesh::CompressionTransformPosition8bit( const FRMVECTOR3& in, unsigned char& x, unsigned char& y, unsigned char& z)
{
	FRMVECTOR3 fv = FRMVECTOR3(in);
	fv = FrmVector3TransformCoord(fv,m_ctMatrix);

	x = (unsigned char) (fv.v[0] * 255.f);
	y = (unsigned char) (fv.v[1] * 255.f);
	z = (unsigned char) (fv.v[2] * 255.f);
}
//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition8bitMakeDrawable
// Desc: Compresses mesh with 8 bit rotated, scaled and offset positions 
//-----------------------------------------------------------------------------
bool CompressedMesh::CompressionTransformPosition8bitMakeDrawable()
{
	bool bRet = true;
	unsigned int i;

	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	FRMMATRIX4X4 rotMat, scaleMat, offsetMat;
	FRMVECTOR3 scale, offset;

	if(!DetermineRotationMatrix(rotMat))
		return false;

	rotMat = FrmMatrixInverse(rotMat);

	// determinte scale and offset
	DetermineScaleAndOffset(rotMat, scale, offset );

	//scale/offset
	offsetMat = FrmMatrixTranslate(offset.x, offset.y, offset.z);
	scaleMat = FrmMatrixScale(scale.x, scale.y, scale.z);

	//invert
	scaleMat =  FrmMatrixInverse(scaleMat);
	offsetMat =  FrmMatrixInverse(offsetMat);

	//keep translation invers out of w area of matrix
	offsetMat = FrmMatrixTranspose(offsetMat);


	// m = r-1 * o-1 * s-1 
	FRMMATRIX4X4 combined = FrmMatrixMultiply( offsetMat, scaleMat);
	combined = FrmMatrixMultiply( rotMat, combined);

	m_ctMatrix = combined;

	unsigned char * compressedVerts = new unsigned char[pMesh->m_nNumVertices * 6];//6*2];
	unsigned char* outStream = compressedVerts;

	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		// scale and offset position
		FRMVECTOR3 pos( inStream[0], inStream[1], inStream[2] );
		CompressionTransformPosition8bit(pos, outStream[0],
											   outStream[1],
											   outStream[2]);
		// normalize normals 
		float fx = inStream[3];
		float fy = inStream[4];
		float fz = inStream[5];
		FRMVECTOR3 n = FRMVECTOR3(fx,fy,fz);
		n = FrmVector3Normalize(n);
		
		QuantiseNormal(n,outStream[3],
						 outStream[4],
						 outStream[5]);

		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;
		outStream += 6;//xyz(w)+Qnormal (no uvs for this)
	}

	//now prepare the vertex layout accordingly 
	pMesh->m_VertexLayout[0].bNormalized = false;
	pMesh->m_VertexLayout[0].nSize = 3;
	pMesh->m_VertexLayout[0].nStride = 6;
	pMesh->m_VertexLayout[0].nType = GL_UNSIGNED_BYTE;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].bNormalized = false;
	pMesh->m_VertexLayout[1].nSize = 3;
	pMesh->m_VertexLayout[1].nStride = 6;
	pMesh->m_VertexLayout[1].nType = GL_UNSIGNED_BYTE;
	pMesh->m_VertexLayout[1].nUsage = FRM_VERTEX_NORMAL;

	pMesh->m_VertexLayout[2].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 6;//6;

// Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        compressedVerts, &pMesh->m_hVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, &pMesh->m_hIndexBuffer ) )
        bRet = 0;


	delete[] compressedVerts;

	return bRet;
}
//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition16bit
// Desc: takes a position vector and returns a D3DCOLOR scale and offset compressed version
//-----------------------------------------------------------------------------
void CompressedMesh::CompressionTransformPosition16bit( const FRMVECTOR3& in,
													    FLOAT16& x, FLOAT16& y, FLOAT16&z)//, FLOAT16& w)
{
	FRMVECTOR3 fv = FRMVECTOR3(in);
	fv = FrmVector3TransformCoord(fv, m_ctMatrix);

	//ClipVec(fv);
	
	x = (FLOAT16) ( (fv.v[0]) * 65535.f);
	y = (FLOAT16) ( (fv.v[1]) * 65535.f);
	z = (FLOAT16) ( (fv.v[2]) * 65535.f);
	//w = FLOAT16 (65535.f); // we need w = 1 for 4x4 transform

}
//-----------------------------------------------------------------------------
// Name: SlidingCompressionTransformPosition24bit
// Desc: Lays out memory as such:
//			[ 8 bits ][ 8 bits ][ 8 bits ][ 8 bits ]
// OS[0] =  [ n.x    ][ lx     ][ ly     ][ lz     ]
// OS[1] =  [ n.y    ][ mx     ][ my     ][ mz     ]
// OS[2] =  [ n.z    ][ hx     ][ hy     ][ hz     ]
// Where n is the normal, and lx,mx,hx are the low med high 8 bits of x
//-----------------------------------------------------------------------------
void CompressedMesh::SlidingCompressionTransformPosition24bit( const FRMVECTOR3& pos,
															   const FRMVECTOR3& norm,
															   unsigned int* outStream)
{
	FRMVECTOR3 out = FRMVECTOR3(pos);
	out = FrmVector3TransformCoord( out, m_ctMatrix);

	unsigned int x,y,z;

	// we have to use double as float only have 23bit precision and we require 24!
	double tx,ty,tz;

	//2^23 = 8388608
	//2^22 = 4194304

	tx = double(out.x) * 8388607.0; 
	ty = double(out.y) * 8388607.0; 
	tz = double(out.z) * 8388607.0;

	// we have 24 bits of precision
	x = (unsigned int) tx;
	y = (unsigned int) ty;
	z = (unsigned int) tz;

	// split position into from a 24 bit integer into 3 8 bit integer
	unsigned int ilx = (x & 0x0000FF) >> 0;
	unsigned int imx = (x & 0x00FF00) >> 8;
	unsigned int ihx = (x & 0xFF0000) >> 16;
	unsigned int ily = (y & 0x0000FF) >> 0;
	unsigned int imy = (y & 0x00FF00) >> 8;
	unsigned int ihy = (y & 0xFF0000) >> 16;
	unsigned int ilz = (z & 0x0000FF) >> 0;
	unsigned int imz = (z & 0x00FF00) >> 8;
	unsigned int ihz = (z & 0xFF0000) >> 16;

	// normals -1.0 - 1.f -> 0.f - 255.f
	unsigned int inx = (unsigned int) ( (norm.x * 127.f) + 128.f );
	unsigned int iny = (unsigned int) ( (norm.y * 127.f) + 128.f );
	unsigned int inz = (unsigned int) ( (norm.z * 127.f) + 128.f );

	// we pre-swap components due to use of D3DCOLOR (uses intel byte-handiness)
	outStream[0] = (inx << 24) | (ilx << 16) | (ily << 8) | (ilz << 0);
	outStream[1] = (iny << 24) | (imx << 16) | (imy << 8) | (imz << 0);
	outStream[2] = (inz << 24) | (ihx << 16) | (ihy << 8) | (ihz << 0);

}


//-----------------------------------------------------------------------------
// Name: SlidingCompressionTransformPosition24bitbitMakeDrawable
// Desc: 24 bits per component, see SlidingCompressionTransformPosition24bit(..)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CompressedMesh::SlidingCompressionTransformPosition24bitbitMakeDrawable()
{
	bool bRet = true;
	unsigned int i;

	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	FRMMATRIX4X4 rotMat, scaleMat, offsetMat;
	FRMVECTOR3 scale, offset;

	if(!DetermineRotationMatrix(rotMat))
		return false;

	rotMat = FrmMatrixInverse(rotMat);

	// determinte scale and offset
	DetermineScaleAndOffset(rotMat, scale, offset );

	//scale/offset
	offsetMat = FrmMatrixTranslate(offset.x, offset.y, offset.z);
	scaleMat = FrmMatrixScale(scale.x, scale.y, scale.z);

	//invert
	scaleMat =  FrmMatrixInverse(scaleMat);
	offsetMat =  FrmMatrixInverse(offsetMat);

	//keep translation invers out of w area of matrix
	offsetMat = FrmMatrixTranspose(offsetMat);


	// m = r-1 * o-1 * s-1 
	FRMMATRIX4X4 combined = FrmMatrixMultiply( offsetMat, scaleMat);
	combined = FrmMatrixMultiply( rotMat, combined);

	m_ctMatrix = combined;

	unsigned int * compressedVerts = new unsigned int[pMesh->m_nNumVertices * 3];
	unsigned int* outStream = compressedVerts;

	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		// scale and offset position
		FRMVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		// normalize normals 
		float fx = inStream[3];
		float fy = inStream[4];
		float fz = inStream[5];
		FRMVECTOR3 n = FRMVECTOR3(fx,fy,fz);
		n = FrmVector3Normalize(n);

		SlidingCompressionTransformPosition24bit( pos, n, outStream);
		
		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;
		outStream += 3;
	}

	//now prepare the vertex layout accordingly 
	pMesh->m_VertexLayout[0].bNormalized = false;
	pMesh->m_VertexLayout[0].nSize = 3;
	pMesh->m_VertexLayout[0].nStride = 0;
	pMesh->m_VertexLayout[0].nType = GL_UNSIGNED_INT;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 12;

// Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        compressedVerts, &pMesh->m_hVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, &pMesh->m_hIndexBuffer ) )
        bRet = 0;


	delete[] compressedVerts;

	return bRet;
}

//-----------------------------------------------------------------------------
// Name: CompressionTransformPosition101012
// Desc: takes a position vector and returns a 32 bit unsigned integer compressed version
//-----------------------------------------------------------------------------
unsigned int CompressedMesh::CompressionTransformPosition101012 ( const FRMVECTOR3& in )
{
	FRMVECTOR3 fv = FRMVECTOR3(in);
	fv = FrmVector3TransformCoord(fv, m_ctMatrix);

	unsigned int x = (unsigned int) (fv.v[0] * 1023.f); // 10 bits
	unsigned int y = (unsigned int) (fv.v[1] * 1023.f); // 10 bits
	unsigned int z = (unsigned int) (fv.v[2] * 4095.f); // 12 bits
	
	return (x & 0x3FF) | ((y & 0x3FF) << 10) | ((z & 0xFFF) << 20);
}

unsigned int CompressedMesh::CompressionTransformPosition101010 ( const FRMVECTOR3& in )
{
	FRMVECTOR3 fv = FRMVECTOR3(in);
	fv = FrmVector3TransformCoord(fv, m_ctMatrix);

	int x = (int) (fv.v[0] * 1023.f); // 10 bits
	int y = (int) (fv.v[1] * 1023.f); // 10 bits
	int z = (int) (fv.v[2] * 1023.f); // 10 bits
	
	return (x & 0x3FF) | ((y & 0x3FF) << 10) | ((z & 0x3FF) << 20);
}
//-----------------------------------------------------------------------------
// Name: CreateSwapMatrix
// Desc: Creates a FRMMATRIX4X4 that swaps axises
//-----------------------------------------------------------------------------
void CompressedMesh::CreateSwapMatrix( FRMMATRIX4X4& inOut, SW_AXIS xAxis, SW_AXIS yAxis, SW_AXIS zAxis )
{
	inOut = FrmMatrixIdentity();

	switch( xAxis )
	{
	case SWA_X:
		inOut.M(0,0) = 1.f;
		inOut.M(1,0) = 0.f;
		inOut.M(2,0) = 0.f;
		break;
	case SWA_Y:
		inOut.M(0,0) = 0.f;
		inOut.M(1,0) = 1.f;
		inOut.M(2,0) = 0.f;
		break;
	case SWA_Z:
		inOut.M(0,0) = 0.f;
		inOut.M(1,0) = 0.f;
		inOut.M(2,0) = 1.f;
		break;
	}

	switch( yAxis )
	{
	case SWA_X:
		inOut.M(0,1) = 1.f;
		inOut.M(1,1) = 0.f;
		inOut.M(2,1) = 0.f;
		break;
	case SWA_Y:
		inOut.M(0,1) = 0.f;
		inOut.M(1,1) = 1.f;
		inOut.M(2,1) = 0.f;
		break;
	case SWA_Z:
		inOut.M(0,1) = 0.f;
		inOut.M(1,1) = 0.f;
		inOut.M(2,1) = 1.f;
		break;
	}

	switch( zAxis )
	{
	case SWA_X:
		inOut.M(0,2) = 1.f;
		inOut.M(1,2) = 0.f;
		inOut.M(2,2) = 0.f;
		break;
	case SWA_Y:
		inOut.M(0,2) = 0.f;
		inOut.M(1,2) = 1.f;
		inOut.M(2,2) = 0.f;
		break;
	case SWA_Z:
		inOut.M(0,2) = 0.f;
		inOut.M(1,2) = 0.f;
		inOut.M(2,2) = 1.f;
		break;
	}

}

//-----------------------------------------------------------------------------
// Name: ScaleAndOffsetPosition
// Desc: Compresses mesh into 101012 compression transform 
//       and 101010 normals
//-----------------------------------------------------------------------------
bool CompressedMesh::CompressionTransformPosition101012MakeDrawable()
{
	bool bRet = true;
	unsigned int i;

	FRM_MESH* pMesh = GetMesh(0);
	FrmAssert(pMesh != 0 && m_pBufferData != 0);

	float* inStream = (float*)m_pBufferData;// + (UINT32)pMesh->m_hVertexBuffer); 

	FRMMATRIX4X4 rotMat, scaleMat, offsetMat, swapMat;
	FRMVECTOR3 scale, offset;

	if(!DetermineRotationMatrix(rotMat))
		return false;

	rotMat = FrmMatrixInverse(rotMat);

	// determinte scale and offset
	DetermineScaleAndOffset(rotMat, scale, offset );

	//scale/offset
	offsetMat = FrmMatrixTranslate(offset.x, offset.y, offset.z);
	scaleMat = FrmMatrixScale(scale.x, scale.y, scale.z);

	SW_AXIS minorAxis = SWA_X;
	SW_AXIS otherAxis = SWA_Y;
	SW_AXIS majorAxis = SWA_Z;

	// find major and minoraxis
	if( scale.v[0] > scale.v[1] )
	{
		if( scale.v[0] > scale.v[2] )
		{
			majorAxis = SWA_X;
			if( scale.v[1] > scale.v[2] )
				minorAxis = SWA_Z;
			else
				minorAxis = SWA_Y;
		} else
		{
			majorAxis = SWA_Z;
			minorAxis = SWA_Y;
		}
	} else
	{
		if( scale.v[1] > scale.v[2] )
		{
			majorAxis = SWA_Y;
			if( scale.v[0] > scale.v[2] )
				minorAxis = SWA_Z;
			else
				minorAxis = SWA_X;
		} else
		{
			majorAxis = SWA_Z;
			minorAxis = SWA_X;
		}
	}

	switch( majorAxis )
	{
	case SWA_X:
		switch( minorAxis )
		{
		case SWA_Y:
			otherAxis = SWA_Z;
			break;
		case SWA_Z:
			otherAxis = SWA_Y;
			break;
		}
	break;
	case SWA_Y:
		switch( minorAxis )
		{
		case SWA_X:
			otherAxis = SWA_Z;
			break;
		case SWA_Z:
			otherAxis = SWA_X;
			break;
		}
	break;
	case SWA_Z:
		switch( minorAxis )
		{
		case SWA_X:
			otherAxis = SWA_Y;
			break;
		case SWA_Y:
			otherAxis = SWA_X;
			break;
		}
	break;
	}

	// create a swap matrix that put the major component into Z 
	CreateSwapMatrix( swapMat, minorAxis, otherAxis, majorAxis );

	//invert
	scaleMat =  FrmMatrixInverse(scaleMat);
	offsetMat =  FrmMatrixInverse(offsetMat);

	//keep translation invers out of w area of matrix
	offsetMat = FrmMatrixTranspose(offsetMat);

	swapMat = FrmMatrixInverse(swapMat);

	// m = r-1 * o-1 * s-1 * sw-1
	FRMMATRIX4X4 combined = FrmMatrixMultiply( scaleMat, swapMat);
	combined = FrmMatrixMultiply( offsetMat, combined);
	combined = FrmMatrixMultiply( rotMat, combined);

	m_ctMatrix = combined;

	BYTE* compressedVerts = new BYTE[pMesh->m_nNumVertices *8];//6*2];
	unsigned int * outStream = (unsigned int*)compressedVerts;

	for(i=0;i < pMesh->m_nNumVertices;i++)
	{
		// scale and offset position
		FRMVECTOR3 pos( inStream[0], inStream[1], inStream[2] );

		//pos = FrmVector3TransformCoord(pos, m_ctMatrix);

		outStream[0] = CompressionTransformPosition101012(pos);

		// normalize normals 
		float fx = inStream[3];
		float fy = inStream[4];
		float fz = inStream[5];
		FRMVECTOR3 n = FRMVECTOR3(fx,fy,fz);
		n = FrmVector3Normalize(n);
		
		outStream[1] = CompressVector3to101010Rev(n.x, n.y, n.z);

		// next vertex
		inStream += VERT_SIZE_UNCOMPRESSED;
		outStream += 2;
	}

	//now prepare the vertex layout accordingly 

	pMesh->m_VertexLayout[0].bNormalized = false;
	pMesh->m_VertexLayout[0].nSize = 4;
	pMesh->m_VertexLayout[0].nStride = 8;
	pMesh->m_VertexLayout[0].nType = GL_UNSIGNED_INT_2_10_10_10_REV;
	pMesh->m_VertexLayout[0].nUsage = FRM_VERTEX_POSITION;

	pMesh->m_VertexLayout[1].bNormalized = false;
	pMesh->m_VertexLayout[1].nSize = 4;
	pMesh->m_VertexLayout[1].nStride = 8;
	pMesh->m_VertexLayout[1].nType = GL_INT_2_10_10_10_REV;
	pMesh->m_VertexLayout[1].nUsage = FRM_VERTEX_NORMAL;

	pMesh->m_VertexLayout[2].nSize = 0;//a flag to FrmSetVertexLayout() to stop looping when setting attributes

	pMesh->m_nVertexSize = 8;//6;

	// Create the vertex buffer
    if( FALSE == FrmCreateVertexBuffer( pMesh->m_nNumVertices, 
                                        pMesh->m_nVertexSize, 
                                        compressedVerts, &pMesh->m_hVertexBuffer ) )
        bRet = 0;

    // Create the index buffer
    BYTE* pIndexData = m_pBufferData + (UINT32)pMesh->m_hIndexBuffer;

    if( FALSE == FrmCreateIndexBuffer( pMesh->m_nNumIndices, 
                                       pMesh->m_nIndexSize, 
                                       pIndexData, &pMesh->m_hIndexBuffer ) )
        bRet = 0;


	delete[] compressedVerts;

	return bRet;
}
