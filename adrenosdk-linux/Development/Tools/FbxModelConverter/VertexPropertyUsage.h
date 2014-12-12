//-----------------------------------------------------------------------------
// 
//  VertexPropertyUsage
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VERTEX_PROPERTY_USAGE_H
#define MCE_FBX_VERTEX_PROPERTY_USAGE_H

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        namespace VertexPropertyUsage
        {
            enum VertexPropertyUsages
            {
                INVALID = -1,
                POSITION,
                NORMAL,
                BINORMAL,
                TANGENT,
                SKIN_INDEX,
                SKIN_WEIGHT,
                COLOR,
                UV,
                NUM_VERTEX_PROPERTIES
            };
        };

        namespace VertexPropertyType
        {
            enum VertexPropertyTypes
            {
                INVALID = -1,
                INT8_TYPE,
                UINT8_TYPE,
                INT16_TYPE,
                UINT16_TYPE,
                INT32_TYPE,
                UINT32_TYPE,
                FLOAT32_TYPE,
                NUM_PROPERTY_TYPES,
            };
        }
    }
}

//-----------------------------------------------------------------------------

#endif