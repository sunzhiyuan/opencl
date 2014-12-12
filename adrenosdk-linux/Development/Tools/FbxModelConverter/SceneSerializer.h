//-----------------------------------------------------------------------------
// 
//  SceneSerializer
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SCENE_SERIALIZER_H
#define MCE_FBX_SCENE_SERIALIZER_H

#include "Scene.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class SceneSerializer
        {
            public:
                 SceneSerializer();
                ~SceneSerializer();

                bool    WriteModelToFile        ( const MCE::FBX::Scene& scene, const char* filename );
                bool    WriteAnimationToFile    ( const MCE::FBX::Scene& scene, const char* filename );
        };
    }
}

//-----------------------------------------------------------------------------

#endif