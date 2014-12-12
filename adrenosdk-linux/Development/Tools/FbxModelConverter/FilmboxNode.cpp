//-----------------------------------------------------------------------------
// 
//  FilmboxNode
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FilmboxNode.h"
#include "FbxUtils.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {

        //-----------------------------------------------------------------------------

        FilmboxNode::FilmboxNode( KFbxNode* node )
        : m_node( node )
        , m_type( MCE::FBX::SceneObjectType::INVALID )
        {
            m_type = GetMceType();
        }

        //-----------------------------------------------------------------------------

        FilmboxNode::~FilmboxNode()
        {

        }

        //-----------------------------------------------------------------------------

        KFbxNode* FilmboxNode::GetNode()
        {
            return m_node;
        }

        //-----------------------------------------------------------------------------

        const KFbxNode* FilmboxNode::GetNode() const
        {
            return m_node;
        }

        //-----------------------------------------------------------------------------

        int FilmboxNode::NumChildren() const
        {
            int num_children = m_node->GetChildCount();
            return num_children;
        }

        //-----------------------------------------------------------------------------

        KFbxNode* FilmboxNode::GetChildNode( int index )
        {
            KFbxNode* child_node = m_node->GetChild( index );
            return child_node;
        }

        //-----------------------------------------------------------------------------

        const KFbxNode* FilmboxNode::GetChildNode( int index ) const
        {
            const KFbxNode* child_node = m_node->GetChild( index );
            return child_node;
        }

        //-----------------------------------------------------------------------------

        const char* FilmboxNode::GetName() const
        {
            const char* value = NULL;

            if( m_node )
            {
                value = m_node->GetName();
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetWorldTransform() const
        {
            KFbxXMatrix value = GetWorldTransform( 0xFFFFFFFF );
            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetWorldTransform( int frame ) const
        {
            KFbxXMatrix value;
            value.SetIdentity();

            if( m_node )
            {
                KFbxScene*         scene     = m_node->GetScene();
                KFbxAnimEvaluator* evaluator = scene->GetEvaluator();
                
                KTime frame_time;

                if( frame == 0xFFFFFFFF )
                {
                    frame_time = KTIME_INFINITE;
                }
                else
                {
                    frame_time.SetTime( 0, 0, 0, frame );
                }

                value = evaluator->GetNodeGlobalTransform( m_node, frame_time );

                if( m_type == MCE::FBX::SceneObjectType::CAMERA )
                {
                    KFbxNode* fbx_target_node = m_node->GetTarget();

                    if( fbx_target_node )
                    {
                        // For target cameras we need to manually replace the rotation to point towards the target
                        KFbxVector4 camera_position = value.GetT();

                        MCE::FBX::FilmboxNode mce_target_node( fbx_target_node );

                        KFbxXMatrix target_transform = mce_target_node.GetWorldTransform( frame );
                        KFbxVector4 target_position  = target_transform.GetT();

                        KFbxVector4 up_vector( 0.0, 1.0, 0.0, 0.0 );

                        KFbxVector4 look_axis = -( target_position - camera_position );
                        look_axis.Normalize();

                        KFbxVector4 right_axis = up_vector.CrossProduct( look_axis );
                        KFbxVector4 up_axis    = look_axis.CrossProduct( right_axis );

                        KFbxMatrix rotation_matrix;
                        rotation_matrix.SetRow( 0, right_axis );
                        rotation_matrix.SetRow( 1, up_axis );
                        rotation_matrix.SetRow( 2, look_axis );
                        rotation_matrix.SetRow( 3, KFbxVector4( 0.0, 0.0, 0.0, 1.0 ) );

                        KFbxQuaternion camera_rotation;
                        double determinant;
                        rotation_matrix.GetElements( KFbxVector4(), camera_rotation, KFbxVector4(), KFbxVector4(), determinant );

                        KFbxVector4 camera_scale( 1.0, 1.0, 1.0, 0.0 );
                        value.SetTQS( camera_position, camera_rotation, camera_scale );
                    }
                    else
                    {
                        // Even though the SDK docs say that GetNodeGlobalTransform takes all transforms into account
                        // For cameras it appears that you have to manually apply post-rotation
                        KFbxVector4 post_rotation = m_node->GetPostRotation( KFbxNode::eSOURCE_SET );

                        KFbxXMatrix fbx_to_mce_camera;
                        fbx_to_mce_camera.SetR( post_rotation );

                        value *= fbx_to_mce_camera;
                    }
                }
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetLocalTransform() const
        {
            KFbxXMatrix world_transform  = GetWorldTransform();
            KFbxXMatrix parent_transform = GetParentWorldTransform();
            KFbxXMatrix world_to_local   = parent_transform.Inverse();
            KFbxXMatrix local_transform  = world_to_local * world_transform;

            return local_transform;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetLocalTransform( int frame ) const
        {
            KFbxXMatrix world_transform  = GetWorldTransform( frame );
            KFbxXMatrix parent_transform = GetParentWorldTransform( frame );
            KFbxXMatrix world_to_local   = parent_transform.Inverse();
            KFbxXMatrix local_transform  = world_to_local * world_transform;

            return local_transform;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetParentWorldTransform() const
        {
            KFbxXMatrix value;
            value.SetIdentity();

            if( m_node )
            {
                KFbxNode* fbx_parent_node = m_node->GetParent();

                if( fbx_parent_node )
                {
                    MCE::FBX::FilmboxNode mce_parent_node( fbx_parent_node );
                    value = mce_parent_node.GetWorldTransform();
                }
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetParentWorldTransform( int frame ) const
        {
            KFbxXMatrix value;
            value.SetIdentity();

            if( m_node )
            {
                KFbxNode* fbx_parent_node = m_node->GetParent();

                if( fbx_parent_node )
                {
                    MCE::FBX::FilmboxNode mce_parent_node( fbx_parent_node );
                    value = mce_parent_node.GetWorldTransform( frame );
                }
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetLocalTranslation() const
        {
            MCE::FBX::Vector3 value( 0.0f, 0.0f, 0.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->LclTranslation.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetLocalRotation() const
        {
            MCE::FBX::Vector3 value( 0.0f, 0.0f, 0.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->LclRotation.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetLocalScale() const
        {
            MCE::FBX::Vector3 value( 1.0f, 1.0f, 1.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->LclScaling.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxXMatrix FilmboxNode::GetGeometricTransform() const
        {
            KFbxXMatrix value;
            value.SetIdentity();

            if( m_node )
            {
                KFbxVector4 translation = m_node->GeometricTranslation.Get();
                KFbxVector4 rotation    = m_node->GeometricRotation.Get();
                KFbxVector4 scale       = m_node->GeometricScaling.Get();

                value.SetS( scale );
                value.SetR( rotation );
                value.SetT( translation );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetGeometricTranslation() const
        {
            MCE::FBX::Vector3 value( 0.0f, 0.0f, 0.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->GeometricTranslation.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetGeometricRotation() const
        {
            MCE::FBX::Vector3 value( 0.0f, 0.0f, 0.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->GeometricRotation.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 FilmboxNode::GetGeometricScale() const
        {
            MCE::FBX::Vector3 value( 1.0f, 1.0f, 1.0f );

            if( m_node )
            {
                KFbxVector4 node_value = m_node->GeometricScaling.Get();
                value = MCE::FBX::ConvertVector3( node_value );
            }

            return value;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::TimeRange FilmboxNode::GetAnimationTimeRange() const
        {
            TimeRange animation_range;

            if( m_node )
            {
                KFbxScene* fbx_scene = m_node->GetScene();
                FBX_ASSERT( fbx_scene->GetSrcObjectCount( FBX_TYPE( KFbxAnimStack ) ) == 0, "Multiple animation stacks are not supported!" );

                KFbxAnimStack* anim_stack = fbx_scene->GetSrcObject( FBX_TYPE( KFbxAnimStack ), 0 );
                
                if( anim_stack )
                {
                    const char* anim_name     = anim_stack->GetName();
                    KTimeSpan   anim_timespan = anim_stack->GetLocalTimeSpan();
                    KTime       anim_start    = anim_timespan.GetStart();
                    KTime       anim_stop     = anim_timespan.GetStop();

                    animation_range.StartFrame = (int)( anim_start.GetFrame( true ) );
                    animation_range.EndFrame   = (int)( anim_stop.GetFrame( true ) );
                }
            }

            return animation_range;
        }

        //-----------------------------------------------------------------------------

        bool FilmboxNode::IsBone() const
        {
            bool value = m_type == MCE::FBX::SceneObjectType::BONE;
            return value;
        }

        //-----------------------------------------------------------------------------

        bool FilmboxNode::IsCamera() const
        {
            bool value = m_type == MCE::FBX::SceneObjectType::CAMERA;
            return value;
        }

        //-----------------------------------------------------------------------------

        bool FilmboxNode::IsMesh() const
        {
            bool value = m_type == MCE::FBX::SceneObjectType::MESH;
            return value;
        }

        //-----------------------------------------------------------------------------

        KFbxCamera* FilmboxNode::GetCamera()
        {
            KFbxCamera* camera = GetFbxCamera();
            return camera;
        }

        //-----------------------------------------------------------------------------

        const KFbxCamera* FilmboxNode::GetCamera() const
        {
            const KFbxCamera* camera = GetFbxCamera();
            return camera;
        }

        //-----------------------------------------------------------------------------

        KFbxMesh* FilmboxNode::GetMesh()
        {
            KFbxMesh* mesh = GetFbxMesh();
            return mesh;
        }

        //-----------------------------------------------------------------------------

        const KFbxMesh* FilmboxNode::GetMesh() const
        {
            const KFbxMesh* mesh = GetFbxMesh();
            return mesh;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneObjectType::SceneObjectTypes FilmboxNode::GetType() const
        {
            return m_type;
        }

        //-----------------------------------------------------------------------------

        KFbxCamera* FilmboxNode::GetFbxCamera() const
        {
            KFbxCamera* camera = NULL;

            if( m_node && IsCamera() )
            {
                KFbxNodeAttribute* node_attribute = m_node->GetNodeAttribute();
                camera = static_cast< KFbxCamera* >( node_attribute );
            }

            return camera;
        }

        //-----------------------------------------------------------------------------

        KFbxMesh* FilmboxNode::GetFbxMesh() const
        {
            KFbxMesh* mesh = NULL;

            if( m_node && IsMesh() )
            {
                KFbxNodeAttribute* node_attribute = m_node->GetNodeAttribute();
                mesh = static_cast< KFbxMesh* >( node_attribute );
            }

            return mesh;
        }

        //-----------------------------------------------------------------------------

        KFbxNodeAttribute::EAttributeType FilmboxNode::GetFbxType() const
        {
            KFbxNodeAttribute::EAttributeType node_type = KFbxNodeAttribute::eUNIDENTIFIED;

            if( m_node )
            {
                const KFbxNodeAttribute* node_attribute = m_node->GetNodeAttribute();

                if( node_attribute )
                {
                    node_type = node_attribute->GetAttributeType();
                }
            }

            return node_type;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneObjectType::SceneObjectTypes FilmboxNode::GetMceType() const
        {
            MCE::FBX::SceneObjectType::SceneObjectTypes mce_type = MCE::FBX::SceneObjectType::INVALID;

            if( m_node )
            {
                KFbxNodeAttribute::EAttributeType fbx_type = GetFbxType();

                switch( fbx_type )
                {
                    case KFbxNodeAttribute::eNULL:
                    case KFbxNodeAttribute::eSKELETON:
                    {
                        mce_type = MCE::FBX::SceneObjectType::BONE;
                        break;
                    }

                    case KFbxNodeAttribute::eCAMERA:
                    {
                        mce_type = MCE::FBX::SceneObjectType::CAMERA;
                        break;
                    }

                    case KFbxNodeAttribute::eMESH:
                    {
                        mce_type = MCE::FBX::SceneObjectType::MESH;
                        break;
                    }

                    default:
                    {
                        mce_type = MCE::FBX::SceneObjectType::BONE;
                        break;
                    }
                }
            }

            return mce_type;
        }

        //-----------------------------------------------------------------------------

    }
}
