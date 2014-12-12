//-----------------------------------------------------------------------------
// 
//  FilmboxMaterialInfo
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_MATERIAL_INFO_H
#define MCE_FILMBOX_MATERIAL_INFO_H

#include "ShaderParameters.h"

#include <fbxsdk.h>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct FilmboxMaterialInfo
        {
            KFbxSurfaceMaterial*            Material;
            std::string                     Name;
            std::string                     ShaderFilename;
            MCE::FBX::ShaderParameterArray  ShaderParameters;
            

            FilmboxMaterialInfo() : Material( NULL ), Name(), ShaderFilename(), ShaderParameters()
            {

            }
        };

        typedef std::vector< FilmboxMaterialInfo > FilmboxMaterialInfoArray;
    }
}

//-----------------------------------------------------------------------------

#endif