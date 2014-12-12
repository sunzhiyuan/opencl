//-----------------------------------------------------------------------------
// 
//  SceneNode
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SCENE_NODE_H
#define MCE_FBX_SCENE_NODE_H

#include "Mesh.h"
#include "QuaternionAnimation.h"
#include "SceneObject.h"
#include "Transform.h"
#include "Vector3Animation.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Scene;
        class SceneNode;

        typedef std::vector< MCE::FBX::SceneNode* >       SceneNodeArray;
        typedef std::vector< const MCE::FBX::SceneNode* > ConstSceneNodeArray;

        class SceneNode
        {
            public:
                 SceneNode();
                 SceneNode( MCE::FBX::Scene* scene );
                ~SceneNode();

                const char*                             GetName                 () const;
                void                                    SetName                 ( const char* name );

                MCE::FBX::Scene*                        GetScene                ();
                const MCE::FBX::Scene*                  GetScene                () const;
                void                                    SetScene                ( MCE::FBX::Scene* scene );

                int                                     GetObjectId             () const;
                MCE::FBX::SceneObject*                  GetObject               ();
                const MCE::FBX::SceneObject*            GetObject               () const;
                void                                    SetObject               ( MCE::FBX::SceneObject* object );

                int                                     GetCollisionMeshId      () const;
                MCE::FBX::Mesh*                         GetCollisionMesh        ();
                const MCE::FBX::Mesh*                   GetCollisionMesh        () const;
                void                                    SetCollisionMesh        ( MCE::FBX::Mesh* collision_mesh );

                MCE::FBX::Transform&                    GetTransform            ();
                const MCE::FBX::Transform&              GetTransform            () const;
                void                                    SetTransform            ( const MCE::FBX::Transform& transform );
                
                MCE::FBX::Transform                     GetInverseBindPose      () const;

                MCE::FBX::Vector3Animation&             GetPositionAnimation    ();
                const MCE::FBX::Vector3Animation&       GetPositionAnimation    () const;

                MCE::FBX::QuaternionAnimation&          GetRotationAnimation    ();
                const MCE::FBX::QuaternionAnimation&    GetRotationAnimation    () const;

                MCE::FBX::Vector3Animation&             GetScaleAnimation       ();
                const MCE::FBX::Vector3Animation&       GetScaleAnimation       () const;

                MCE::FBX::SceneNode*                    GetParentNode           ();
                const MCE::FBX::SceneNode*              GetParentNode           () const;

                int                                     NumChildNodes           () const;
                void                                    AddChildNode            ( MCE::FBX::SceneNode* node );
                void                                    RemoveChildNode         ( MCE::FBX::SceneNode* node );
                MCE::FBX::SceneNode*                    GetChildNode            ( int index );
                const MCE::FBX::SceneNode*              GetChildNode            ( int index ) const;

            private:
                void                                    SetParentNode           ( MCE::FBX::SceneNode* parent );

            private:
                std::string                    m_name;
                MCE::FBX::Scene*               m_scene;
                int                            m_object_id;
                int                            m_collision_mesh_id;
                Transform                      m_tranform;
                MCE::FBX::Vector3Animation     m_position_anim;
                MCE::FBX::QuaternionAnimation  m_rotation_anim;
                MCE::FBX::Vector3Animation     m_scale_anim;
                SceneNode*                     m_parent;
                SceneNodeArray                 m_child_nodes;
        };
    }
}

//-----------------------------------------------------------------------------

#endif