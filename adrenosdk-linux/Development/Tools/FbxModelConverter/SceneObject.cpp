//-----------------------------------------------------------------------------
// 
//  SceneObject
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "SceneObject.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        SceneObject::SceneObject()
        : m_type( MCE::FBX::SceneObjectType::INVALID )
        , m_scene( NULL )
        , m_node( NULL )
        {

        }

        //-----------------------------------------------------------------------------

        SceneObject::SceneObject( MCE::FBX::SceneObjectType::SceneObjectTypes type )
        : m_type( type )
        , m_scene( NULL )
        , m_node( NULL )
        {

        }

        //-----------------------------------------------------------------------------

        SceneObject::~SceneObject()
        {

        }

        //-----------------------------------------------------------------------------

        SceneObjectType::SceneObjectTypes SceneObject::Type() const
        {
            return m_type;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Scene* SceneObject::GetScene()
        {
            return m_scene;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Scene* SceneObject::GetScene() const
        {
            return m_scene;
        }

        //-----------------------------------------------------------------------------

        void SceneObject::SetScene( MCE::FBX::Scene* scene )
        {
            m_scene = scene;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneNode* SceneObject::GetNode()
        {
            return m_node;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneNode* SceneObject::GetNode() const
        {
            return m_node;
        }

        //-----------------------------------------------------------------------------

        void SceneObject::SetNode( MCE::FBX::SceneNode* scene_node )
        {
            m_node = scene_node;
        }
    }
}
