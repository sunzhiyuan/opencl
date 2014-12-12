//-----------------------------------------------------------------------------
// 
//  ShaderParameterType
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SHADER_PARAMETER_TYPES_H
#define MCE_FBX_SHADER_PARAMETER_TYPES_H

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        namespace ShaderParameterType
        {
            enum ShaderParameterTypes
            {
                INVALID = -1,
                BOOL,
                INTEGER,
                FLOAT,
                VECTOR2,
                VECTOR3,
                VECTOR4,
                COLOR3,
                COLOR4,
                STRING,
                TEXTURE,
                NUM_TYPES
            };
        }
    }
}

//-----------------------------------------------------------------------------

#endif