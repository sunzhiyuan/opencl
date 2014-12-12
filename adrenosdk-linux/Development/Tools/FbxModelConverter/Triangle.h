//-----------------------------------------------------------------------------
// 
//  Triangle
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_TRIANGLE_H
#define MCE_FBX_TRIANGLE_H

#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Triangle
        {
            public:
                int Vertex[ 3 ];
                int MaterialId;

                Triangle()
                {
                    Vertex[ 0 ] = -1;
                    Vertex[ 1 ] = -1;
                    Vertex[ 2 ] = -1;
                    MaterialId  = -1;
                }

                Triangle( int v0, int v1, int v2, int material_id )
                {
                    Vertex[ 0 ] = v0;
                    Vertex[ 1 ] = v1;
                    Vertex[ 2 ] = v2;
                    MaterialId  = material_id;
                }
        };

        typedef std::vector< MCE::FBX::Triangle > TriangleArray;
    }
}

//-----------------------------------------------------------------------------

#endif