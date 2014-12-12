//-----------------------------------------------------------------------------
// 
//  Material
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_MATERIAL_H
#define MCE_MATERIAL_H

#include "ShaderParameters.h"

#include <string>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct Material
        {
            std::string                     Name;
            std::string                     ShaderFilename;
            MCE::FBX::ShaderParameterArray  ShaderParameters;

            Material() : Name(), ShaderFilename(), ShaderParameters()
            {

            }
        };

        typedef std::vector< Material > MaterialArray;
    }
}

//-----------------------------------------------------------------------------

#endif