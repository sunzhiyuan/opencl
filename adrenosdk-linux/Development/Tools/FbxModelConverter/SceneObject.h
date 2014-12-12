//-----------------------------------------------------------------------------
// 
//  SceneObject
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SCENE_OBJECT_H
#define MCE_FBX_SCENE_OBJECT_H

#include "SceneObjectType.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Scene;
        class SceneNode;

        class SceneObject
        {
            public:
                         SceneObject();
                         SceneObject( MCE::FBX::SceneObjectType::SceneObjectTypes type );
                virtual ~SceneObject();

                SceneObjectType::SceneObjectTypes   Type        () const;

                MCE::FBX::Scene*                    GetScene    ();
                const MCE::FBX::Scene*              GetScene    () const;
                void                                SetScene    ( MCE::FBX::Scene* scene );

                MCE::FBX::SceneNode*                GetNode     ();
                const MCE::FBX::SceneNode*          GetNode     () const;
                void                                SetNode     ( MCE::FBX::SceneNode* scene_node );

            protected:
                SceneObjectType::SceneObjectTypes m_type;
                MCE::FBX::Scene*                  m_scene;
                MCE::FBX::SceneNode*              m_node;
        };

        typedef std::vector< SceneObject* > SceneObjectArray;
    }
}

//-----------------------------------------------------------------------------

#endif