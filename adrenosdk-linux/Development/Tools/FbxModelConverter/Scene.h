//-----------------------------------------------------------------------------
// 
//  Scene
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SCENE_H
#define MCE_FBX_SCENE_H

#include "Bone.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneNode.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Scene
        {
            public:
                 Scene();
                ~Scene();

                int                             NumMaterials        () const;
                int                             AddMaterial         ( const MCE::FBX::Material& material );
                MCE::FBX::Material&             GetMaterial         ( int id );
                const MCE::FBX::Material&       GetMaterial         ( int id ) const;

                int                             NumObjects          () const;
                int                             AddObject           ( MCE::FBX::SceneObject* object );
                MCE::FBX::SceneObject*          GetSceneObject      ( int id );
                const MCE::FBX::SceneObject*    GetSceneObject      ( int id ) const;

                int                             NumCollisionMeshes  () const;
                int                             AddCollisionMesh    ( MCE::FBX::Mesh* mesh );
                MCE::FBX::Mesh*                 GetCollisionMesh    ( int id );
                const MCE::FBX::Mesh*           GetCollisionMesh    ( int id ) const;

                MCE::FBX::SceneNode&            GetRootNode         ();
                const MCE::FBX::SceneNode&      GetRootNode         () const;
                
                const MCE::FBX::SceneNode*      GetBoneNode         ( int id ) const;

            private:
                int                             GetUniqueObjectId   () const;

            private:
                MaterialArray    m_materials;
                SceneObjectArray m_objects;
                MeshArray        m_collision_meshes;
                SceneNode        m_root_node;
        };
    }
}

//-----------------------------------------------------------------------------

#endif