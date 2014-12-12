//-----------------------------------------------------------------------------
// 
//  Mesh
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_MESH_H
#define MCE_FBX_MESH_H

#include "SceneObject.h"
#include "Surface.h"
#include "Triangle.h"
#include "Vector3.h"
#include "VertexArray.h"
#include <fbxsdk.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class FilmboxNode;

        class Mesh : public SceneObject
        {
            public:
                typedef std::vector< uint32 > BoneIdArray;

                         Mesh();
                virtual ~Mesh();

                VertexArray&            GetVertices     ();
                const VertexArray&      GetVertices     () const;

                TriangleArray&          GetTriangles    ();
                const TriangleArray&    GetTriangles    () const;

                void                    BuildSurfaces   ();

                SurfaceArray&           GetSurfaces     ();
                const SurfaceArray&     GetSurfaces     () const;

                bool                    IsSkinned       () const;
                void                    GetBoneIds      ( BoneIdArray& bone_ids ) const;

            private:
                void Reset();

            private:
                VertexArray    m_verts;
                TriangleArray  m_triangles;
                SurfaceArray   m_surfaces;
        };

        typedef std::vector< Mesh* >       MeshArray;
        typedef std::vector< const Mesh* > ConstMeshArray;
    }
}

//-----------------------------------------------------------------------------

#endif