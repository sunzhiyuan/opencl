//-----------------------------------------------------------------------------
// 
//  FilmboxSceneInfo
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_SCENE_INFO_H
#define MCE_FILMBOX_SCENE_INFO_H

#include "FilmboxMaterialInfo.h"
#include "FilmboxSkinInfo.h"
#include <fbxsdk.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        typedef std::vector< KFbxNode* > FilmboxNodeArray;

        struct FilmboxSceneInfo
        {
            MCE::FBX::FilmboxNodeArray          Nodes;
            MCE::FBX::FilmboxNodeArray          Bones;
            MCE::FBX::FilmboxSkinInfoArray      Skins;
            MCE::FBX::FilmboxMaterialInfoArray  Materials;

            FilmboxSceneInfo() : Nodes(), Skins(), Materials()
            {

            }
        };
    }
}

//-----------------------------------------------------------------------------

#endif