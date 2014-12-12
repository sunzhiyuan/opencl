//-----------------------------------------------------------------------------
// 
//  Scene
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Scene.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Scene::Scene()
        : m_materials()
        , m_objects()
        , m_collision_meshes()
        , m_root_node()
        {
            m_root_node.SetScene( this );
            m_root_node.SetName( "__MODEL_ROOT__" );
        }

        //-----------------------------------------------------------------------------

        Scene::~Scene()
        {
            int num_objects = NumObjects();

            for( int i = 0; i < num_objects; ++i )
            {
                MCE::FBX::SceneObject* object = m_objects[ i ];
                delete object;
                object = NULL;
            }

            int num_collision_meshes = NumCollisionMeshes();

            for( int i = 0; i < num_collision_meshes; ++i )
            {
                MCE::FBX::Mesh* collision_mesh = m_collision_meshes[ i ];
                delete collision_mesh;
                collision_mesh = NULL;
            }
        }

        //-----------------------------------------------------------------------------

        int Scene::NumMaterials() const
        {
            int num_materials = (int)( m_materials.size() );
            return num_materials;
        }

        //-----------------------------------------------------------------------------

        int Scene::AddMaterial( const MCE::FBX::Material& material )
        {
            int id = NumMaterials();
            m_materials.push_back( material );
            return id;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Material& Scene::GetMaterial( int id )
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumMaterials() ), "Index out of bounds" );
            MCE::FBX::Material& material = m_materials[ id ];
            return material;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Material& Scene::GetMaterial( int id ) const
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumMaterials() ), "Index out of bounds" );
            const MCE::FBX::Material& material = m_materials[ id ];
            return material;
        }

        //-----------------------------------------------------------------------------

        int Scene::NumObjects() const
        {
            int num_objects = (int)( m_objects.size() );
            return num_objects;
        }

        //-----------------------------------------------------------------------------

        int Scene::AddObject( MCE::FBX::SceneObject* object )
        {
            FBX_ASSERT( object, "Null object" );
            
            object->SetScene( this );

            int id = NumObjects();
            m_objects.push_back( object );
            return id;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneObject* Scene::GetSceneObject( int id )
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumObjects() ), "Index out of bounds" );
            MCE::FBX::SceneObject* object = m_objects[ id ];
            return object;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneObject* Scene::GetSceneObject( int id ) const
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumObjects() ), "Index out of bounds" );
            const MCE::FBX::SceneObject* object = m_objects[ id ];
            return object;
        }

        //-----------------------------------------------------------------------------

        int Scene::NumCollisionMeshes() const
        {
            int num_meshes = (int)( m_collision_meshes.size() );
            return num_meshes;
        }

        //-----------------------------------------------------------------------------

        int Scene::AddCollisionMesh( MCE::FBX::Mesh* mesh )
        {
            FBX_ASSERT( mesh, "Null mesh" );

            int collision_mesh_id = NumCollisionMeshes();
            m_collision_meshes.push_back( mesh );
            return collision_mesh_id;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Mesh* Scene::GetCollisionMesh( int id )
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumCollisionMeshes() ), "Index out of bounds" );
            MCE::FBX::Mesh* mesh = m_collision_meshes[ id ];
            return mesh;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Mesh* Scene::GetCollisionMesh( int id ) const
        {
            FBX_ASSERT( ( id >= 0 ) && ( id < NumCollisionMeshes() ), "Index out of bounds" );
            const MCE::FBX::Mesh* mesh = m_collision_meshes[ id ];
            return mesh;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneNode& Scene::GetRootNode()
        {
            return m_root_node;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneNode& Scene::GetRootNode() const
        {
            return m_root_node;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SceneNode* Scene::GetBoneNode( int id ) const
        {
            const MCE::FBX::SceneNode* bone_node = NULL;

            int bone_index  = -1;
            int num_objects = NumObjects();
            
            for( int i = 0; i < num_objects; ++i )
            {
                MCE::FBX::SceneObject* object = m_objects[ i ];
                
                if( object )
                {
                    MCE::FBX::SceneObjectType::SceneObjectTypes object_type = object->Type();

                    if( object_type == MCE::FBX::SceneObjectType::BONE )
                    {
                        ++bone_index;
                        
                        if( bone_index == id )
                        {
                            bone_node = object->GetNode();
                            break;
                        }
                    }
                }
            }
            
            return bone_node;
        }

        //-----------------------------------------------------------------------------

        int Scene::GetUniqueObjectId() const
        {
            int id = NumObjects();
            return id;
        }
    }
}
