//-----------------------------------------------------------------------------
// 
//  Base
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Base.h"
#include <string.h>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        float DegreesToRadians( float degrees )
        {
            return degrees * 3.14159f / 180.0f;
        }

        //-----------------------------------------------------------------------------

        int StringCompare( const char* string1, const char* string2 )
        {
            return strcmp( string1, string2 );
        }

        //-----------------------------------------------------------------------------

        int StringICompare( const char* string1, const char* string2 )
        {
#if defined (_WIN32)
            return _stricmp(string1, string2);
#elif defined (__linux__)
	    return strcasecmp(string1, string2);
#endif
        }

        //-----------------------------------------------------------------------------

        void StripFileExtension( char* dest_path, const char* source_path, uint32 max_chars )
        {
            strncpy( dest_path, source_path, max_chars );
            char* start_of_extension = strrchr( dest_path, '.' );

            if( start_of_extension )
            {
                *start_of_extension = '\0';
            }
        }
    }
}
