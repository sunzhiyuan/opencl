//-----------------------------------------------------------------------------
// 
//  IndexBuffer
//	
//  Copyright (c) 2011 QUALCOMM Incorporated.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FrmIndexBuffer.h"

//-----------------------------------------------------------------------------

Adreno::IndexBuffer::IndexBuffer()
: NumIndices( 0 )
, Indices( NULL )
{

}

//-----------------------------------------------------------------------------

Adreno::IndexBuffer::~IndexBuffer()
{
    if( Indices )
    {
        delete[] Indices;
        Indices = NULL;
   }
}

//-----------------------------------------------------------------------------

void Adreno::IndexBuffer::Resize( UINT32 num_indices )
{
    if( Indices )
    {
        delete[] Indices;
        Indices = NULL;
   }

    NumIndices = num_indices;

    if( num_indices > 0 )
    {
        Indices = new UINT32[ num_indices ];
    }
}
