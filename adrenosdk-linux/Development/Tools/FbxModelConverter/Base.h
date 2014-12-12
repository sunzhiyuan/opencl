//-----------------------------------------------------------------------------
// 
//  Base
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_BASE_H
#define MCE_FBX_BASE_H

#include <assert.h>

//-----------------------------------------------------------------------------

typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned int   uint32;

//-----------------------------------------------------------------------------

#ifdef DEBUG
    #define FBX_ASSERT( cond, desc ) { assert( cond ); ((void)(desc)); }
    #define FBX_ASSERT_FALSE( desc ) { assert( false ); ((void)(desc)); }
#else
    #define FBX_ASSERT( cond, desc ) { ((void)(cond)); ((void)(desc)); }
    #define FBX_ASSERT_FALSE( desc ) { ((void)(desc)); }
#endif

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        float   DegreesToRadians    ( float degrees );
        int     StringCompare       ( const char* string1, const char* string2 );
        int     StringICompare      ( const char* string1, const char* string2 );
        void    StripFileExtension  ( char* dest_path, const char* source_path, uint32 max_chars );
    }
}

//-----------------------------------------------------------------------------

#endif