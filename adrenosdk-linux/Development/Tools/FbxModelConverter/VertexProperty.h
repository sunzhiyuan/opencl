//-----------------------------------------------------------------------------
// 
//  VertexProperty
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VERTEX_PROPERTY_H
#define MCE_FBX_VERTEX_PROPERTY_H

#include "VertexPropertyUsage.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct VertexProperty
        {
            int  Usage;
            int  Type;
            int  ValueSize;
            int  NumValues;
            bool Normalized;
            int  Channel;

            VertexProperty()
            : Usage( VertexPropertyUsage::INVALID )
            , Type( MCE::FBX::VertexPropertyType::INVALID )
            , ValueSize( 0 )
            , NumValues( 0 )
            , Normalized( false )
            , Channel( 0 )
            {

            }

            VertexProperty( int usage, MCE::FBX::VertexPropertyType::VertexPropertyTypes type, int value_size, int num_values, bool normalized, int channel = 0 )
            : Usage( usage )
            , Type( type )
            , ValueSize( value_size )
            , NumValues( num_values )
            , Normalized( normalized )
            , Channel( channel )
            {

            }
        };

        typedef std::vector< VertexProperty > VertexPropertyArray;
    }
}

//-----------------------------------------------------------------------------

#endif