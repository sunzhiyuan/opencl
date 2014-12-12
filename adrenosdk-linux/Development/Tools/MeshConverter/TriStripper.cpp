//--------------------------------------------------------------------------------------
// Name: TriStripper.cpp
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "FrmBasicTypes.h"
#include "TriStripper.h"
#include <algorithm>
#include <string.h>


//--------------------------------------------------------------------------------------
// Name: struct VertexCache
// Desc: Helper for simulating a simple MRU vertex cache
//--------------------------------------------------------------------------------------
struct VertexCache
{
    static const UINT32 MAX_CACHE_SIZE = 16;
    UINT16 m_IndicesMRU[MAX_CACHE_SIZE];
    UINT32 m_nCacheSlot;
    UINT32 m_nCacheSize;
    UINT32 m_nNumCacheHits;
    UINT32 m_nNumCacheMisses;

    VertexCache( UINT32 nCacheSize )
    {
        m_nCacheSize = nCacheSize;
        for( UINT32 i=0; i<m_nCacheSize; i++ )
            m_IndicesMRU[i] = 0xffff;
        m_nCacheSlot      = 0;
        m_nNumCacheHits   = 0;
        m_nNumCacheMisses = 0;
    }

#if defined (_WIN32)
    BOOL VertexCache::AddIndex( UINT32 nIndex )
#elif defined (__linux__)
    BOOL AddIndex(UINT32 nIndex)
#endif
    {
        for( UINT32 i = 0; i < m_nCacheSize; i++ )
        {
            // Cache hit: return TRUE
            if( nIndex == m_IndicesMRU[i] )
            {
                m_nNumCacheHits++;
                return TRUE;
            }
        }

        // Cache miss: Add the vertex to the simulated cache
        m_nNumCacheMisses++;
        m_IndicesMRU[m_nCacheSlot] = nIndex;
        m_nCacheSlot = ( m_nCacheSlot + 1 ) % m_nCacheSize;

        return FALSE;
    }
};


//--------------------------------------------------------------------------------------
// Name: EstimateNumCacheMisses()
// Desc: Estimates the number of vertex cahce misses for a given set of indices.
//--------------------------------------------------------------------------------------
UINT32 EstimateNumCacheMisses( UINT32  nCacheSize,
                               UINT16* pIndices16, UINT32 nNumIndices,
                               BOOL    bIsTriStrip )
{
    // The vertex cache
    VertexCache cache( nCacheSize );

    // Loop through the indices for each face
    UINT32 nNumFaces = ( bIsTriStrip ) ? nNumIndices-2 : nNumIndices/3;
    for( UINT32 i = 0; i < nNumFaces; i++ )
    {
        UINT16 i0, i1, i2;
        if( bIsTriStrip )
        {
            i0 = pIndices16[i + 0];
            i1 = pIndices16[i + 1];
            i2 = pIndices16[i + 2];
        }
        else
        {
            i0 = pIndices16[3*i + 0];
            i1 = pIndices16[3*i + 1];
            i2 = pIndices16[3*i + 2];
        }

        // Add each index to the simulated vertex cache
        cache.AddIndex( i0 );
        cache.AddIndex( i1 );
        cache.AddIndex( i2 );
    }

    return cache.m_nNumCacheMisses;
}


//--------------------------------------------------------------------------------------
// Name: StripifyMesh()
// Desc: Performs a time-consuming algorithm to generate triangle strips.
//--------------------------------------------------------------------------------------
BOOL StripifyMesh( UINT32 nCacheSize,
                   UINT16* pIndices16, UINT32 nNumIndices,
                   UINT16** ppStripIndices16, UINT32* pnNumIndices )
{
    // This is yet to be implemented
    return FALSE;
}



//--------------------------------------------------------------------------------------
// Name: OptimizeVertexOrder()
// Desc: Reorders vertices (and their indices) to minimize memory page misses.
//--------------------------------------------------------------------------------------
VOID OptimizeVertexOrder( UINT32   nVertexSize,
                          VOID*    pVertices, UINT32 nNumVertices,
                          UINT16*  pIndices,  UINT32 nNumIndices )
{
    // Sort vertices by how they are first used
    UINT32* pSortedVertices = new UINT32[nNumVertices];
    {
        struct VertexUsage
        {
            UINT16 nIndex16;
            UINT16 nUsed;
        };

        // Each vertex is sequentially numbered
        for( UINT32 i = 0; i < nNumVertices; i++ )
        {
            ((VertexUsage*)&pSortedVertices[i])->nIndex16 = i;
            ((VertexUsage*)&pSortedVertices[i])->nUsed  = 0xffff;
        }

        // Each vertex is then marked with the first index that references it
        for( UINT32 i = 0; i < nNumIndices; i++ )
        {
            if( ((VertexUsage*)&pSortedVertices[pIndices[i]])->nUsed == 0xffff )
                ((VertexUsage*)&pSortedVertices[pIndices[i]])->nUsed = (UINT16)i;
        }

        // Sort
        std::sort( (UINT32*)&pSortedVertices[0], (UINT32*)&pSortedVertices[nNumVertices] );

        // Clear the used field, so we can later reference the sorted list at UINT32 indices
        for( UINT32 i = 0; i < nNumVertices; i++ )
        {
            ((VertexUsage*)&pSortedVertices[i])->nUsed = 0;
        }
    }

    // Remap the vertices
    {
        BYTE* pOldVertices = (BYTE*)pVertices;
        BYTE* pNewVertices = new BYTE[ nNumVertices * nVertexSize ];
        for( UINT32 i=0; i<nNumVertices; i++ )
        {
            BYTE* pSrc = pOldVertices + nVertexSize * i;
            BYTE* pDst = pNewVertices + nVertexSize * pSortedVertices[i];

            memcpy( pDst, pSrc, nVertexSize );
        }

        memcpy( pOldVertices, pNewVertices, nNumVertices * nVertexSize );
        delete[] pNewVertices;
    }

    // Remap the indices
    {
        UINT16* pNewIndices = new UINT16[nNumIndices];
        for( UINT32 i = 0; i < nNumIndices; i++ )
        {
            pNewIndices[i] = pSortedVertices[ pIndices[i] ];
        }

        memcpy( pIndices, pNewIndices, nNumIndices * sizeof(UINT16) );
        delete[] pNewIndices;
    }

    delete[] pSortedVertices;
}

