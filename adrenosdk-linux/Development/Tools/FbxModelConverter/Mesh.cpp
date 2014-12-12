//-----------------------------------------------------------------------------
// 
//  Mesh
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Mesh.h"
#include "FilmboxManager.h"
#include "FilmboxNode.h"
#include <algorithm>

//-----------------------------------------------------------------------------

namespace
{
    bool TriangleByMaterialIdSorter( const MCE::FBX::Triangle& a, const MCE::FBX::Triangle& b )
    {
        return a.MaterialId < b.MaterialId;
    }
}

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Mesh::Mesh()
        : MCE::FBX::SceneObject( MCE::FBX::SceneObjectType::MESH )
        , m_verts()
        , m_triangles()
        {

        }

        //-----------------------------------------------------------------------------

        Mesh::~Mesh()
        {
            Reset();
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::VertexArray& Mesh::GetVertices()
        {
            return m_verts;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::VertexArray& Mesh::GetVertices() const
        {
            return m_verts;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::TriangleArray& Mesh::GetTriangles()
        {
            return m_triangles;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::TriangleArray& Mesh::GetTriangles() const
        {
            return m_triangles;
        }

        //-----------------------------------------------------------------------------

        void Mesh::BuildSurfaces()
        {
            m_surfaces.clear();

            std::sort( m_triangles.begin(), m_triangles.end(), TriangleByMaterialIdSorter );

            int num_triangles = (int)( m_triangles.size() );

            int current_material_id = -1;

            MCE::FBX::Surface surface;

            for( int t = 0; t < num_triangles; ++t )
            {
                const MCE::FBX::Triangle& triangle = m_triangles[ t ];

                if( current_material_id != triangle.MaterialId )
                {
                    // Add the current surface
                    if( ( surface.StartIndex >= 0 ) && ( surface.NumTriangles > 0 ) )
                    {
                        m_surfaces.push_back( surface );
                    }

                    // Start the next surface
                    current_material_id = triangle.MaterialId;

                    surface = MCE::FBX::Surface();
                    surface.MaterialId = triangle.MaterialId;
                    surface.StartIndex = t * 3;
                }

                ++surface.NumTriangles;
            }

            // Add the final surface
            if( ( surface.StartIndex >= 0 ) && ( surface.NumTriangles > 0 ) )
            {
                m_surfaces.push_back( surface );
            }
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SurfaceArray& Mesh::GetSurfaces()
        {
            return m_surfaces;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SurfaceArray& Mesh::GetSurfaces() const
        {
            return m_surfaces;
        }

        //-----------------------------------------------------------------------------

        bool Mesh::IsSkinned() const
        {
            const MCE::FBX::VertexFormat& vertex_format = m_verts.GetVertexFormat();
            bool is_skinned = vertex_format.HasSkinWeights();
            return is_skinned;
        }

        //-----------------------------------------------------------------------------

        void Mesh::GetBoneIds( BoneIdArray& bone_ids ) const
        {
            bone_ids.clear();

            if( !IsSkinned() ) { return; }

            const MCE::FBX::VertexFormat& vertex_format = m_verts.GetVertexFormat();

            int num_verts = m_verts.NumVerts();
            int num_weights_per_vertex = vertex_format.NumSkinWeights();

            for( int v = 0; v < num_verts; ++v )
            {
                for( int w = 0; w < num_weights_per_vertex; ++w )
                {
                    const MCE::FBX::SkinWeight& skin_weight = m_verts.GetSkinWeight( v, w );
                    uint32 bone_id = skin_weight.BoneIndex;

                    bool is_new_bone_id = true;

                    int num_bone_ids = (int)( bone_ids.size() );

                    for( int b = 0; b < num_bone_ids; ++b )
                    {
                        if( bone_id == bone_ids[ b ] )
                        {
                            is_new_bone_id = false;
                            break;
                        }
                    }

                    if( is_new_bone_id )
                    {
                        bone_ids.push_back( bone_id );
                    }
                }
            }
        }

        //-----------------------------------------------------------------------------

        void Mesh::Reset()
        {
            m_verts.Reset();
            m_triangles.clear();
        }
    }
}
