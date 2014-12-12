//-----------------------------------------------------------------------------
// 
//  Surface
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SURFACE_H
#define MCE_FBX_SURFACE_H

#include "Triangle.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct Surface
        {
            int StartIndex;
            int NumTriangles;
            int MaterialId;

            Surface() : StartIndex( -1 ), NumTriangles( 0 ), MaterialId( -1 )
            {

            }

            Surface( int start_index, int num_triangles, int material_id ) : StartIndex( start_index ), NumTriangles( num_triangles ), MaterialId( material_id )
            {

            }
        };

        typedef std::vector< Surface > SurfaceArray;
    }
}

//-----------------------------------------------------------------------------

#endif