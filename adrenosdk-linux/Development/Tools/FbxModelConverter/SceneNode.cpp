//-----------------------------------------------------------------------------
// 
//  SceneNode
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "SceneNode.h"
#include "Scene.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        SceneNode::SceneNode()
        : m_scene( NULL )
        , m_name()
        , m_object_id( -1 )
        , m_collision_mesh_id( -1 )
        , m_tranform()
        , m_position_anim()
        , m_rotation_anim()
        , m_scale_anim()
        , m_parent( NULL )
        , m_child_nodes()
        {

        }

        //-----------------------------------------------------------------------------

        SceneNode::SceneNode( MCE::FBX::Scene* scene )
        : m_scene( scene )
        , m_name()
        , m_object_id( -1 )
        , m_collision_mesh_id( -1 )
        , m_tranform()
        , m_position_anim()
        , m_rotation_anim()
        , m_scale_anim()
        , m_parent( NULL )
        , m_child_nodes()
        {

        }

        //-----------------------------------------------------------------------------

        SceneNode::~SceneNode()
        {
            if( m_parent )
            {
                m_parent->RemoveChildNode( this );
            }

            int num_child_nodes = NumChildNodes();

            for( int i = 0; i < num_child_nodes; ++i )
            {
                MCE::FBX::SceneNode* child_node = m_child_nodes[ i ];
                child_node->SetParentNode( NULL );

                delete child_node;
                child_node = NULL;
            }
        }

        //-----------------------------------------------------------------------------

        const char* SceneNode::GetName() const
        {
            const char* name = m_name.c_str();
            return name;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetName( const char* name )
        {
            m_name = name;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Scene* SceneNode::GetScene()
        {
            return m_scene;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Scene* SceneNode::GetScene() const
        {
            return m_scene;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetScene( MCE::FBX::Scene* scene )
        {
            m_scene = scene;
        }

        //-----------------------------------------------------------------------------

        int SceneNode::GetObjectId() const
        {
            return m_object_id;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneObject* SceneNode::GetObject()
        {
            FBX_ASSERT( m_scene, "Null scene" );

            MCE::FBX::SceneObject* scene_object = NULL;

            if( m_object_id >= 0 )
            {
                scene_object = m_scene->GetSceneObject( m_object_id );
            }

            return scene_object;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneObject* SceneNode::GetObject() const
        {
            FBX_ASSERT( m_scene, "Null scene" );

            const MCE::FBX::SceneObject* scene_object = NULL;

            if( m_object_id >= 0 )
            {
                scene_object = m_scene->GetSceneObject( m_object_id );
            }

            return scene_object;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetObject( MCE::FBX::SceneObject* object )
        {
            FBX_ASSERT( m_scene, "Null scene" );

            if( object )
            {
                int id = m_scene->AddObject( object );
                object->SetNode( this );
                m_object_id = id;
            }
            else
            {
                m_object_id = -1;
            }
        }

        //-----------------------------------------------------------------------------

        int SceneNode::GetCollisionMeshId() const
        {
            return m_collision_mesh_id;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Mesh* SceneNode::GetCollisionMesh()
        {
            FBX_ASSERT( m_scene, "Null scene" );

            MCE::FBX::Mesh* collision_mesh = NULL;

            if( m_collision_mesh_id >= 0 )
            {
                collision_mesh = m_scene->GetCollisionMesh( m_collision_mesh_id );
            }

            return collision_mesh;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Mesh* SceneNode::GetCollisionMesh() const
        {
            FBX_ASSERT( m_scene, "Null scene" );

            const MCE::FBX::Mesh* collision_mesh = NULL;

            if( m_collision_mesh_id >= 0 )
            {
                collision_mesh = m_scene->GetCollisionMesh( m_collision_mesh_id );
            }

            return collision_mesh;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetCollisionMesh( MCE::FBX::Mesh* collision_mesh )
        {
            FBX_ASSERT( m_scene, "Null scene" );

            if( collision_mesh )
            {
                int id = m_scene->AddCollisionMesh( collision_mesh );
                m_collision_mesh_id = id;
            }
            else
            {
                m_collision_mesh_id = -1;
            }
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Transform& SceneNode::GetTransform()
        {
            return m_tranform;
        }

        //-----------------------------------------------------------------------------

        constMCE::FBX::Transform& SceneNode::GetTransform() const
        {
            return m_tranform;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetTransform( constMCE::FBX::Transform& transform )
        {
            m_tranform = transform;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Transform SceneNode::GetInverseBindPose() const
        {
            MCE::FBX::Transform inverse_bind_pose;
            
            if( m_object_id >= 0 )
            {
                const MCE::FBX::Bone*  bone   = NULL;
                MCE::FBX::SceneObject* object = m_scene->GetSceneObject( m_object_id );
                
                if( object )
                {
                    MCE::FBX::SceneObjectType::SceneObjectTypes object_type = object->Type();
                    
                    if( object_type == MCE::FBX::SceneObjectType::BONE )
                    {
                        bone = dynamic_cast< const MCE::FBX::Bone* >( object );
                    }
                }

                if( bone )
                {
                    inverse_bind_pose = bone->GetInverseBindPose();
                }
                else
                {
                    inverse_bind_pose.Position = -m_tranform.Position;
                    inverse_bind_pose.Rotation = m_tranform.Rotation.Inverse();
                    inverse_bind_pose.Scale    = 1.0f / m_tranform.Scale;
                }
            }

            return inverse_bind_pose;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3Animation& SceneNode::GetPositionAnimation()
        {
            return m_position_anim;
        }

        //-----------------------------------------------------------------------------

        constMCE::FBX::Vector3Animation& SceneNode::GetPositionAnimation() const
        {
            return m_position_anim;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::QuaternionAnimation& SceneNode::GetRotationAnimation()
        {
            return m_rotation_anim;
        }

        //-----------------------------------------------------------------------------

        constMCE::FBX::QuaternionAnimation& SceneNode::GetRotationAnimation() const
        {
            return m_rotation_anim;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3Animation& SceneNode::GetScaleAnimation()
        {
            return m_scale_anim;
        }

        //-----------------------------------------------------------------------------

        constMCE::FBX::Vector3Animation& SceneNode::GetScaleAnimation() const
        {
            return m_scale_anim;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneNode* SceneNode::GetParentNode()
        {
            return m_parent;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneNode* SceneNode::GetParentNode() const
        {
            return m_parent;
        }

        //-----------------------------------------------------------------------------

        int SceneNode::NumChildNodes() const
        {
            int num_child_nodes = (int)( m_child_nodes.size() );
            return num_child_nodes;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::AddChildNode( MCE::FBX::SceneNode* node )
        {
            FBX_ASSERT( node, "Null node" );
            node->SetParentNode( this );
            m_child_nodes.push_back( node );
        }

        //-----------------------------------------------------------------------------

        void SceneNode::RemoveChildNode( MCE::FBX::SceneNode* node )
        {
            FBX_ASSERT( node, "Null node" );

            int num_child_nodes = NumChildNodes();

            for( int i = 0; i < num_child_nodes; )
            {
                MCE::FBX::SceneNode* child_node = m_child_nodes[ i ];

                if( child_node == node )
                {
                    m_child_nodes.erase( m_child_nodes.begin() + i );
                    --num_child_nodes;
                }
                else
                {
                    ++i;
                }
            }
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneNode* SceneNode::GetChildNode( int index )
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumChildNodes() ), "Index out of bounds" );
            MCE::FBX::SceneNode* node = m_child_nodes[ index ];
            return node;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneNode* SceneNode::GetChildNode( int index ) const
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumChildNodes() ), "Index out of bounds" );
            const MCE::FBX::SceneNode* node = m_child_nodes[ index ];
            return node;
        }

        //-----------------------------------------------------------------------------

        void SceneNode::SetParentNode( MCE::FBX::SceneNode* parent )
        {
            m_parent = parent;
        }
    }
}
