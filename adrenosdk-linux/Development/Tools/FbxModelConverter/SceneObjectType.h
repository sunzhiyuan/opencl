//-----------------------------------------------------------------------------
// 
//  SceneObjectType
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SCENE_OBJECT_TYPE_H
#define MCE_FBX_SCENE_OBJECT_TYPE_H

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        namespace SceneObjectType
        {
            enum SceneObjectTypes
            {
                INVALID = -1,
                BONE,
                CAMERA,
                MESH,
                NUM_TYPES
            };
        }
    }
}

//-----------------------------------------------------------------------------

#endif