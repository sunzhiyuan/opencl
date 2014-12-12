//--------------------------------------------------------------------------------------
// File: FrmExtendedMesh.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <string.h>
#include "FrmExtendedMesh.h"
#include "ReadMsh1.h"
#include "TriStripper.h"


using namespace MSH1_FORMAT;


extern BOOL ReadMesh1( const CHAR* strFileName, CFrmExtendedMesh* pExtendedMesh );
extern BOOL ReadMesh0( const CHAR* strFileName, CFrmExtendedMesh* pExtendedMesh );
extern BOOL ReadObj( const CHAR* strFileName, CFrmExtendedMesh* pExtendedMesh );


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
CFrmExtendedMesh::CFrmExtendedMesh()
{
    m_pFrames     = NULL;
    m_nNumFrames  = 0;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
CFrmExtendedMesh::~CFrmExtendedMesh()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH_FRAME* pFrame = &m_pFrames[i];

        if( pFrame->m_pMesh )
        {
            FRM_MESH* pMesh = pFrame->m_pMesh;

            for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
            {
                FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];
                delete[] pSubset->m_pTextures;
                delete[] pSubset->m_pBoneIds;
            }
        
            delete[] pMesh->m_pBones;
            delete[] pMesh->m_pSubsets;
            delete   pMesh->m_pVertexData;
            delete   pMesh->m_pIndexData;
        }

        delete pFrame->m_pMesh;
    }

    delete[] m_pFrames;
}


//--------------------------------------------------------------------------------------
// Name: LoadInputMesh()
// Desc: Computes the min and max extents of a mesh. Note that this code currently
//       ignores the frame transformation matrix
//--------------------------------------------------------------------------------------
BOOL CFrmExtendedMesh::LoadInputMesh( const CHAR* strFileName )
{
    // Try the default (.mesh) loader first
    if( TRUE == ReadMesh1( strFileName, this ) )
        return TRUE;

    // Try the default (.mesh) loader first
    if( TRUE == ReadMesh0( strFileName, this ) )
        return TRUE;

    // Try loading the mesh as an OBJ file
    if( TRUE == ReadObj( strFileName, this ) )
        return TRUE;

    // TODO: Add other loaders here: .x, .3ds, .obj, etc

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: ComputeBounds()
// Desc: Computes the min and max extents of a mesh. Note that this code currently
//       ignores the frame transformation matrix
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::ComputeBounds( FRMVECTOR3& min, FRMVECTOR3& max )
{
    min = FRMVECTOR3( +1e6, +1e6, +1e6 );
    max = FRMVECTOR3( -1e6, -1e6, -1e6 );

    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        BYTE* pVertexData = (BYTE*)pMesh->m_pVertexData;

        for( UINT32 i=0; i<pMesh->m_nNumVertices; i++ )
        {
            FRMVECTOR3* p = (FRMVECTOR3*)pVertexData;

            if( p->x < min.x ) min.x = p->x;
            if( p->y < min.y ) min.y = p->y;
            if( p->z < min.z ) min.z = p->z;

            if( p->x > max.x ) max.x = p->x;
            if( p->y > max.y ) max.y = p->y;
            if( p->z > max.z ) max.z = p->z;

            pVertexData += pMesh->m_nVertexSize;
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: Rotate()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::Rotate( FLOAT32 fAngle, FRMVECTOR3 vAxis )
{
    FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, vAxis );

    // Reverse the Z-axis of the vertex components
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        BYTE* pVertexData = (BYTE*)pMesh->m_pVertexData;

        for( UINT32 i=0; i<pMesh->m_nNumVertices; i++ )
        {
            FRM_VERTEX_ELEMENT* pVertexLayout = pMesh->m_VertexLayout;
            while( pVertexLayout->nStride )
            {
                if( ( pVertexLayout->nUsage == FRM_VERTEX_POSITION ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_NORMAL ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_TANGENT ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_BINORMAL ) )
                {
                    FRMVECTOR3* pVector = (FRMVECTOR3*)pVertexData;
                    FRMVECTOR3 vRotatedVector = FrmVector3TransformCoord( *pVector, matRotate );
                    *pVector = vRotatedVector;
                }
                pVertexData += pVertexLayout->nStride;
                pVertexLayout++;
            }
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: OffsetAndScale()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::OffsetAndScale( FRMVECTOR3 vOffset, FLOAT32 fScale )
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        BYTE* pVertexData = (BYTE*)pMesh->m_pVertexData;

        for( UINT32 i=0; i<pMesh->m_nNumVertices; i++ )
        {
            FRMVECTOR3* p = (FRMVECTOR3*)pVertexData;
            p->x = ( p->x + vOffset.x ) * fScale;
            p->y = ( p->y + vOffset.y ) * fScale;
            p->z = ( p->z + vOffset.z ) * fScale;

            pVertexData += pMesh->m_nVertexSize;
        }
    }
}


//--------------------------------------------------------------------------------------
// Name: ReverseHandedness()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::ReverseHandedness()
{
    // Reverse the Z-axis of the vertex components
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        BYTE* pVertexData = (BYTE*)pMesh->m_pVertexData;

        for( UINT32 i=0; i<pMesh->m_nNumVertices; i++ )
        {
            FRM_VERTEX_ELEMENT* pVertexLayout = pMesh->m_VertexLayout;
            UINT32 nOffset = 0;
            while( pVertexLayout->nStride )
            {
                if( ( pVertexLayout->nUsage == FRM_VERTEX_POSITION ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_NORMAL ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_TANGENT ) ||
                    ( pVertexLayout->nUsage == FRM_VERTEX_BINORMAL ) )
                {
                    FRMVECTOR3* pVector = (FRMVECTOR3*)( pVertexData + nOffset );
                    pVector->z *= -1.0f;
                }
                nOffset += pVertexLayout->nStride;
                pVertexLayout++;
            }
            pVertexData += pMesh->m_nVertexSize;
        }
    }
}



BOOL GetIndicesForTriangle( FRM_MESH* pMesh, UINT16* pIndex16, UINT32* pIndex32,
                            UINT32 i, UINT32* i0, UINT32* i1, UINT32* i2 )
{
    if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLES )
    {
        (*i0) = 3 * i + 0;
        (*i1) = 3 * i + 1;
        (*i2) = 3 * i + 2;
    }
    else if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
    {
        (*i0) = i + 0;
        (*i1) = i + 1;
        (*i2) = i + 2;

        // Skip degenerate triangles
        if( (*i0)==(*i1) || (*i1)==(*i2) || (*i2)==(*i0) )
            return FALSE;
    }

    if( pMesh->m_nIndexSize == sizeof(UINT16) )
    {
        (*i0) = pIndex16[ (*i0) ];
        (*i1) = pIndex16[ (*i1) ];
        (*i2) = pIndex16[ (*i2) ];
    } 
    else 
    {
        (*i0) = pIndex32[ (*i0) ];
        (*i1) = pIndex32[ (*i1) ];
        (*i2) = pIndex32[ (*i2) ];
    }

    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: GenerateNormals()
// Desc: Generate normals for a mesh
//--------------------------------------------------------------------------------------
BOOL GenerateNormals( FRM_MESH* pMesh )
{
    BYTE* pSrcVertexData = (BYTE*)pMesh->m_pVertexData;
    BYTE* pSrcIndexData  = (BYTE*)pMesh->m_pIndexData;
    
    // Currently only works for indexed data
    if( NULL == pSrcIndexData )
        return FALSE;

    UINT32 nNumPrimitives;
    if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLES )
        nNumPrimitives = pMesh->m_nNumIndices / 3;
    else if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
        nNumPrimitives = pMesh->m_nNumIndices - 2;
    else
        // Unrecognized primitive type
        return FALSE;

    UINT16* pIndex16 = (UINT16*)pSrcIndexData;
    UINT32* pIndex32 = (UINT32*)pSrcIndexData;
    
    FRMVECTOR3* vDstNormals = new FRMVECTOR3[pMesh->m_nNumVertices];
    memset( vDstNormals, 0, sizeof(FRMVECTOR3) * pMesh->m_nNumVertices );

    for( UINT32 i = 0; i < nNumPrimitives; i++ )
    {
        UINT32 i0, i1, i2;
        
        if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLES )
        {
            i0 = 3 * i + 0;
            i1 = 3 * i + 1;
            i2 = 3 * i + 2;
        }
        else if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
        {
            i0 = i + 0;
            i1 = i + 1;
            i2 = i + 2;

            // Skip degenerate triangles
            if( i0==i1 || i1==i2 || i2==i0 )
                continue;
        }

        if( pMesh->m_nIndexSize == sizeof(UINT16) )
        {
            i0 = pIndex16[i0];
            i1 = pIndex16[i1];
            i2 = pIndex16[i2];
        } 
        else 
        {
            i0 = pIndex32[i0];
            i1 = pIndex32[i1];
            i2 = pIndex32[i2];
        }

        FRMVECTOR3* v0 = (FRMVECTOR3*)( pSrcVertexData + i0 * pMesh->m_nVertexSize );
        FRMVECTOR3* v1 = (FRMVECTOR3*)( pSrcVertexData + i1 * pMesh->m_nVertexSize );
        FRMVECTOR3* v2 = (FRMVECTOR3*)( pSrcVertexData + i2 * pMesh->m_nVertexSize );

	FRMVECTOR3 subv2v0 = *v2-*v0;
	FRMVECTOR3 subv1v0 = *v1-*v0;
	FRMVECTOR3 crossVecSubs = FrmVector3Cross(subv2v0, subv1v0);
        FRMVECTOR3 vNormal = FrmVector3Normalize(crossVecSubs);

        vDstNormals[i0] += vNormal;
        vDstNormals[i1] += vNormal;
        vDstNormals[i2] += vNormal;
    }

    // Build the new vertex buffer
    UINT32 nOldVertexSize  = pMesh->m_nVertexSize;
    UINT32 nNewVertexSize  = pMesh->m_nVertexSize + sizeof(FRMVECTOR3);
    BYTE*  pDstVertexData = new BYTE[ pMesh->m_nNumVertices * nNewVertexSize];
    
    for( UINT32 i = 0; i < pMesh->m_nNumVertices; i++ )
    {
        // Copy position
        ((FRMVECTOR3*)pDstVertexData)[0] = ((FRMVECTOR3*)pSrcVertexData)[0];
        
        // Copy the normal
        ((FRMVECTOR3*)pDstVertexData)[1] = FrmVector3Normalize(vDstNormals[i]);

        // Copy all remaining vertex components
        UINT32 nRemainingSize = nOldVertexSize - sizeof(FRMVECTOR3);
        if( nRemainingSize )
            memcpy( &((FRMVECTOR3*)pDstVertexData)[2], &((FRMVECTOR3*)pSrcVertexData)[1], nRemainingSize );
        
        // Advance the pointer to the next vertex
        pSrcVertexData += nOldVertexSize;
        pDstVertexData += nNewVertexSize;
    }

    // Cleanup the temporary space
    delete[] vDstNormals;

    // Make the new buffer data go live
    delete[] pMesh->m_pVertexData;
    pMesh->m_pVertexData = pDstVertexData;

    // Update the vertex layout to account for the new normal
    for( UINT32 i=8; i >= 1; i-- )
    {
        pMesh->m_VertexLayout[i] = pMesh->m_VertexLayout[i-1];
    }
    pMesh->m_VertexLayout[1].nUsage      = FRM_VERTEX_NORMAL;
    pMesh->m_VertexLayout[1].nSize       = 3;
    pMesh->m_VertexLayout[1].nType       = GL_FLOAT;
    pMesh->m_VertexLayout[1].bNormalized = FALSE;
    pMesh->m_VertexLayout[1].nStride     = sizeof(FRMVECTOR3);

    pMesh->m_nVertexSize = nNewVertexSize;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: AddNormals()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::AddNormals()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        // Check if normals are already present
        BOOL bHasNormals = FALSE;
        FRM_VERTEX_ELEMENT* pVertexLayout = pMesh->m_VertexLayout;
        while( pVertexLayout->nStride )
        {
            if( pVertexLayout->nUsage == FRM_VERTEX_NORMAL )
                bHasNormals = TRUE;
            pVertexLayout++;
        }
        if( bHasNormals )
            continue;

        GenerateNormals( pMesh );
    }
}



struct TANGENT_SPACE
{
    FRMVECTOR3 vNormal;
    FRMVECTOR3 vTangent;
    FRMVECTOR3 vBinormal;
};

//--------------------------------------------------------------------------------------
// Name: AddTangentSpace()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::AddTangentSpace()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        // Check for normals and texcoords
        UINT32 nNormalOffset   = 0;
        UINT32 nTangentOffset  = 0;
        UINT32 nBinormalOffset = 0;
        UINT32 nTexCoordOffset = 0;
        FRM_VERTEX_ELEMENT* pVertexLayout = pMesh->m_VertexLayout;
        UINT32 nOffset = 0;
        while( pVertexLayout->nStride )
        {
            if( pVertexLayout->nUsage == FRM_VERTEX_NORMAL )
                nNormalOffset = nOffset;
            if( pVertexLayout->nUsage == FRM_VERTEX_TANGENT )
                nTangentOffset = nOffset;
            if( pVertexLayout->nUsage == FRM_VERTEX_BINORMAL )
                nBinormalOffset = nOffset;
            if( pVertexLayout->nUsage == FRM_VERTEX_TEXCOORD0 )
                nTexCoordOffset = nOffset;
            nOffset += pVertexLayout->nStride;
            pVertexLayout++;
        }

        // Check if the mesh already has a tangent space
        if( nTangentOffset && nBinormalOffset )
            continue;

        // We need texture coordinate in order to compute tangent space basis vectors
        if( 0 == nTexCoordOffset )
            continue;

        // Generate normals, if they're not already present
        if( 0 == nNormalOffset )
        {
            if( FALSE == GenerateNormals( pMesh ) )
                continue;
        }

        BYTE* pSrcVertexData = (BYTE*)pMesh->m_pVertexData;
        BYTE* pSrcIndexData  = (BYTE*)pMesh->m_pIndexData;
        
        // Currently only works for indexed data
        if( NULL == pSrcIndexData )
            continue;

        UINT32 nNumPrimitives;
        if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLES )
            nNumPrimitives = pMesh->m_nNumIndices / 3;
        else if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
            nNumPrimitives = pMesh->m_nNumIndices - 2;
        else
            // Unrecognized primitive type
            continue;

        UINT16* pIndex16 = (UINT16*)pSrcIndexData;
        UINT32* pIndex32 = (UINT32*)pSrcIndexData;
        
        // Calculate mesh tangent space
        TANGENT_SPACE* pTangentSpace;
        {
            pTangentSpace = new TANGENT_SPACE[ pMesh->m_nNumVertices ];
            memset( pTangentSpace, 0, sizeof(TANGENT_SPACE) * pMesh->m_nNumVertices );		

            for( UINT32 i = 0; i < nNumPrimitives; i++ )
            {
                UINT32 index[3];
                if( FALSE == GetIndicesForTriangle( pMesh, pIndex16, pIndex32, i, 
                                                    &index[0], &index[1], &index[2] ) )
                    continue;

                FRMVECTOR3 p[3];
                p[0] = *(FRMVECTOR3*)( pSrcVertexData + index[0] * pMesh->m_nVertexSize );
                p[1] = *(FRMVECTOR3*)( pSrcVertexData + index[1] * pMesh->m_nVertexSize );
                p[2] = *(FRMVECTOR3*)( pSrcVertexData + index[2] * pMesh->m_nVertexSize );
        
                FRMVECTOR2 t[3];
                t[0] = *(FRMVECTOR2*)( pSrcVertexData + nTexCoordOffset + index[0] * pMesh->m_nVertexSize );
                t[1] = *(FRMVECTOR2*)( pSrcVertexData + nTexCoordOffset + index[1] * pMesh->m_nVertexSize );
                t[2] = *(FRMVECTOR2*)( pSrcVertexData + nTexCoordOffset + index[2] * pMesh->m_nVertexSize );
                
                FRMVECTOR3 vEdgePos1 = p[1] - p[0];
                FRMVECTOR3 vEdgePos2 = p[2] - p[0];
                FRMVECTOR2 vEdgeUV1  = t[1] - t[0];
                FRMVECTOR2 vEdgeUV2  = t[2] - t[0];

                FLOAT32 fCross = vEdgeUV1.tv * vEdgeUV2.tu - vEdgeUV1.tu * vEdgeUV2.tv;
                if( fabs(fCross) > 1e-6f )
                {
                    FRMVECTOR3 vTangent  = ( vEdgePos1 * -vEdgeUV2.tv + vEdgePos2 * vEdgeUV1.tv ) / fCross;
                    FRMVECTOR3 vBiNormal = ( vEdgePos1 * -vEdgeUV2.tu + vEdgePos2 * vEdgeUV1.tu ) / fCross;

                    pTangentSpace[ index[0] ].vTangent += vTangent;
                    pTangentSpace[ index[1] ].vTangent += vTangent;
                    pTangentSpace[ index[2] ].vTangent += vTangent;

                    pTangentSpace[ index[0] ].vNormal  += vBiNormal;
                    pTangentSpace[ index[1] ].vNormal  += vBiNormal;
                    pTangentSpace[ index[2] ].vNormal  += vBiNormal;
                }
            }

            for( UINT32 i = 0; i < pMesh->m_nNumVertices; i++ )
            {
                FRMVECTOR3 vVertexNormal = *(FRMVECTOR3*)( pSrcVertexData + nNormalOffset + i * pMesh->m_nVertexSize );
                FRMVECTOR3 vNormal   = FrmVector3Normalize( vVertexNormal );
                FRMVECTOR3 vTangent  = FrmVector3Normalize( pTangentSpace[i].vTangent );
                FRMVECTOR3 vBinormal = FrmVector3Normalize( pTangentSpace[i].vBinormal );

                // Gram-Schmidt orthogonalization
                vTangent  = vTangent - vNormal * FrmVector3Dot( vTangent, vNormal );
                vTangent  = FrmVector3Normalize( vTangent );
                vBinormal = FrmVector3Cross( vTangent, vNormal );

                if( FrmVector3Dot( vBinormal, vVertexNormal ) >= 0.0f )
                    vBinormal = -vBinormal;

                pTangentSpace[i].vNormal   = vNormal;
                pTangentSpace[i].vTangent  = vTangent;
                pTangentSpace[i].vBinormal = vBinormal;
            }
        }

        // Build the new vertex buffer
        UINT32 nOldVertexSize  = pMesh->m_nVertexSize;
        UINT32 nNewVertexSize  = pMesh->m_nVertexSize + sizeof(FRMVECTOR3) + sizeof(FRMVECTOR3);
        BYTE*  pNewVertexData = new BYTE[ pMesh->m_nNumVertices * nNewVertexSize];
        BYTE*  pDstVertexData = pNewVertexData;

        nTangentOffset  = nOldVertexSize; 
        nBinormalOffset = nTangentOffset + sizeof(FRMVECTOR3); 

        for( UINT32 i = 0; i < pMesh->m_nNumVertices; i++ )
        {
            FRMVECTOR3 vNormal   = pTangentSpace[i].vNormal;
            FRMVECTOR3 vTangent  = pTangentSpace[i].vTangent;
            FRMVECTOR3 vBinormal = pTangentSpace[i].vBinormal;
            
            // Copy source vertex components
            memcpy( pDstVertexData, pSrcVertexData, nOldVertexSize );

            // Copy the new normal, tangent, and binormal
            memcpy( pDstVertexData + nNormalOffset,   &vNormal,   sizeof(FRMVECTOR3) );
            memcpy( pDstVertexData + nTangentOffset,  &vTangent,  sizeof(FRMVECTOR3) );
            memcpy( pDstVertexData + nBinormalOffset, &vBinormal, sizeof(FRMVECTOR3) );

            // Advance to the next vertex
            pSrcVertexData += nOldVertexSize;
            pDstVertexData += nNewVertexSize;
        }

        // Cleanup the temporary space
        delete[] pTangentSpace;

        // Make the new buffer data go live
        delete[] pMesh->m_pVertexData;
        pMesh->m_pVertexData = pNewVertexData;
        pMesh->m_nVertexSize = nNewVertexSize;

        // Update the vertex layout to account for the new elements
        FRM_VERTEX_ELEMENT* pElement = pMesh->m_VertexLayout;
        while( pElement->nSize )
            pElement++;
        pElement->nUsage      = FRM_VERTEX_TANGENT;
        pElement->nSize       = 3;
        pElement->nType       = GL_FLOAT;
        pElement->bNormalized = FALSE;
        pElement->nStride     = sizeof(FRMVECTOR3);
        pElement++;
        pElement->nUsage      = FRM_VERTEX_BINORMAL;
        pElement->nSize       = 3;
        pElement->nType       = GL_FLOAT;
        pElement->bNormalized = FALSE;
        pElement->nStride     = sizeof(FRMVECTOR3);
        pElement++;
    }
}


//--------------------------------------------------------------------------------------
// Name: ReverseWinding()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CFrmExtendedMesh::ReverseWinding()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        BYTE* pSrcIndexData  = (BYTE*)pMesh->m_pIndexData;
        if( NULL == pSrcIndexData )
            continue;

        UINT32 nNumPrimitives;
        if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLES )
            nNumPrimitives = pMesh->m_nNumIndices / 3;
        else if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
            nNumPrimitives = pMesh->m_nNumIndices - 2;
        else
            continue;

        UINT16* pIndex16 = (UINT16*)pSrcIndexData;
        UINT32* pIndex32 = (UINT32*)pSrcIndexData;

        if( pMesh->m_pSubsets[0].m_nPrimType == GL_TRIANGLE_STRIP )
        {
            // Error: Reversing the winding of a tri strip is a MUCH more difficult problem!
            return FALSE;
        }

        for( UINT32 i = 0; i < nNumPrimitives; i++ )
        {
            if( pMesh->m_nIndexSize == sizeof(UINT16) )
            {
                UINT16 v1 = pIndex16[3*i+1];
                UINT16 v2 = pIndex16[3*i+2];
                pIndex16[3*i+1] = v2;
                pIndex16[3*i+2] = v1;
            } 
            else 
            {
                UINT16 v1 = pIndex32[3*i+1];
                UINT16 v2 = pIndex32[3*i+2];
                pIndex32[3*i+1] = v2;
                pIndex32[3*i+2] = v1;
            }
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Stripify()
// Desc: Turns triangle list mesh data into optimized triangle strips
//--------------------------------------------------------------------------------------
VOID CFrmExtendedMesh::Stripify()
{
    for( UINT32 i=0; i<m_nNumFrames; i++ )
    {
        FRM_MESH* pMesh  = m_pFrames[i].m_pMesh;
        if( NULL == pMesh )
            continue;

        // Allocate conservative space for new indices for the mesh
        UINT16* pNewIndexData  = new UINT16[2*pMesh->m_nNumIndices];
        UINT32  nNewNumIndices = 0;

        for( UINT32 j=0; j<pMesh->m_nNumSubsets; j++ )
        {
            FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];
            UINT16* pSubsetIndices    = ((UINT16*)pMesh->m_pIndexData) + pSubset->m_nIndexOffset;
            UINT32  nNumSubsetIndices = pSubset->m_nNumIndices;
               

            if( pSubset->m_nPrimType == GL_TRIANGLES )
            {
                const UINT32 nVertexCacheSize = 4;

                // Stripify the subset
                UINT32  nNumStripIndices = 0;
                UINT16* pStripIndices    = 0;
                BOOL bSuccess = StripifyMesh( nVertexCacheSize,
                                              pSubsetIndices, nNumSubsetIndices,
                                              &pStripIndices, &nNumStripIndices );
                if( bSuccess )
                {
                    // Update the subset to be a tri strip
                    pSubset->m_nPrimType    = GL_TRIANGLE_STRIP;
                    pSubset->m_nNumIndices  = nNumStripIndices;
                    pSubset->m_nIndexOffset = nNewNumIndices;

                    // Add the new (tri-strip) indices to a master list for the mesh
                    memcpy( pNewIndexData + nNewNumIndices, pStripIndices, sizeof(UINT16)*nNumStripIndices );
                    nNewNumIndices += nNumStripIndices;
                }
            }

            // If the above failed, or we have non-triangle primitive data...
            if( pSubset->m_nPrimType != GL_TRIANGLE_STRIP )
            {
                // Add the exising (non tri-strip) indices to a master list for the mesh
                memcpy( pNewIndexData + nNewNumIndices, pSubsetIndices, sizeof(UINT16)*nNumSubsetIndices );
                nNewNumIndices += pSubset->m_nNumIndices;
            }
        }

        OptimizeVertexOrder( pMesh->m_nVertexSize,
                             pMesh->m_pVertexData, pMesh->m_nNumVertices,
                             pNewIndexData, nNewNumIndices );
        
        // Replace the old mesh indices with the new indices created above
        delete[] pMesh->m_pIndexData;
        pMesh->m_pIndexData  = new UINT16[nNewNumIndices];
        pMesh->m_nNumIndices = nNewNumIndices;
        memcpy( pMesh->m_pIndexData, pNewIndexData, sizeof(UINT16)*nNewNumIndices );
        delete[] pNewIndexData;
    }
}

