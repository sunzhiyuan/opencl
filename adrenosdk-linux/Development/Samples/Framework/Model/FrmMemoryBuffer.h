//-----------------------------------------------------------------------------
// 
//  MemoryBuffer
//	
//  Copyright (c) 2011 QUALCOMM Incorporated.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef ADRENO_MEMORY_BUFFER_H
#define ADRENO_MEMORY_BUFFER_H

#include "FrmPlatform.h"

//-----------------------------------------------------------------------------

namespace Adreno
{
    class MemoryBuffer
    {
        public:
             MemoryBuffer();
            ~MemoryBuffer();

            INT32       BufferSize      () const;

            BYTE*       GetBuffer       ();
            const BYTE* GetBuffer       () const;

            INT32       ExpandBuffer    ( INT32 num_new_bytes );
            void        WriteData       ( INT32 start_position, const void* data, INT32 size );

        private:
            void    DeleteBuffer    ();

        private:
            INT32 m_size;
            BYTE* m_buffer;
    };
}

//-----------------------------------------------------------------------------

#endif