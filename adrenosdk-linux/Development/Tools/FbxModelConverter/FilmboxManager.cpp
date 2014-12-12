//-----------------------------------------------------------------------------
// 
//  FilmboxManager
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FilmboxManager.h"

#include "Base.h"
#include "Bone.h"
#include "Camera.h"
#include "FbxTypes.h"
#include "FbxTypeConverters.h"
#include "FbxUtils.h"
#include "FilmboxNode.h"
#include "Scene.h"
#include "SceneNode.h"

#include "tootlelib.h"

//-----------------------------------------------------------------------------

namespace
{
    typedef std::vector< int >              VertexIndexArray;
    typedef std::vector< VertexIndexArray > MeshIndexArray;

    //-----------------------------------------------------------------------------

    MCE::FBX::FilmboxManager* s_filmbox_manager = NULL;

    //-----------------------------------------------------------------------------

    void BuildMeshIndexArray( const KFbxMesh* mesh, MeshIndexArray& fbx_to_mce_indices )
    {
        int num_fbx_indices = mesh->GetPolygonVertexCount();

        fbx_to_mce_indices = MeshIndexArray();
        fbx_to_mce_indices.resize( num_fbx_indices );

        int num_polys = mesh->GetPolygonCount();

        for( int p = 0; p < num_polys; ++p )
        {
            for( int v = 0; v < 3; ++v )
            {
                int fbx_vertex_index = mesh->GetPolygonVertex( p, v );
                int mce_vertex_index = ( p * 3 ) + v;

                VertexIndexArray& mce_indices = fbx_to_mce_indices[ fbx_vertex_index ];
                mce_indices.push_back( mce_vertex_index );
            }
        }
    }

    //-----------------------------------------------------------------------------

    MCE::FBX::FilmboxBone ExtractBoneInfo( KFbxCluster& skin_cluster )
    {
        KFbxNode*   bone_node = skin_cluster.GetLink();
        const char* bone_name = bone_node->GetName(); // To make debugging easier

        KFbxCluster::ELinkMode cluster_mode = skin_cluster.GetLinkMode();
        FBX_ASSERT( cluster_mode == KFbxCluster::eNORMALIZE, "Only normalized skin weights are supported!" );

        KFbxXMatrix link_transform;
        skin_cluster.GetTransformLinkMatrix( link_transform );

        KFbxXMatrix transform;
        skin_cluster.GetTransformMatrix( transform );

        KFbxXMatrix inv_link_transform = link_transform.Inverse();
        KFbxXMatrix inv_bind_pose      = inv_link_transform * transform;

        MCE::FBX::FilmboxBone fbx_bone;
        fbx_bone.BoneNode        = bone_node;
        fbx_bone.InverseBindPose = inv_bind_pose;

        return fbx_bone;
    }

    //-----------------------------------------------------------------------------

    void ExtractMeshSkinningInfo( KFbxMesh* mesh, MCE::FBX::FilmboxSkinInfo& skin_info )
    {
        FBX_ASSERT( mesh, "Null mesh!" );

        int num_skins = mesh->GetDeformerCount( KFbxDeformer::eSKIN );
        FBX_ASSERT( num_skins > 1, "More than one skinning set is not supported!" );
        if( num_skins <=0 ) { return; }

        KFbxSkin* skin = (KFbxSkin*)( mesh->GetDeformer( 0, KFbxDeformer::eSKIN ) );
        FBX_ASSERT( skin, "Null skin" );
        if( !skin ) { return; }

        skin_info.IsSkinned = true;

        int num_bones = skin->GetClusterCount();
        int num_polys = mesh->GetPolygonCount();
        int num_verts = num_polys * 3;

        MCE::FBX::FilmboxBoneArray&       mesh_bones   = skin_info.Bones;
        MCE::FBX::FilmboxMeshWeightArray& mesh_weights = skin_info.MeshWeights;

        mesh_weights.resize( num_verts );

        MeshIndexArray fbx_to_mce_indices;
        BuildMeshIndexArray( mesh, fbx_to_mce_indices );

        for( int b = 0; b < num_bones; ++b )
        {
            KFbxCluster* cluster = skin->GetCluster( b );
            FBX_ASSERT( cluster, "Null skin cluster!" );

            MCE::FBX::FilmboxBone fbx_bone = ExtractBoneInfo( *cluster );
            mesh_bones.push_back( fbx_bone );

            int     num_influenced_verts = cluster->GetControlPointIndicesCount();
            int*    cluster_indices      = cluster->GetControlPointIndices();
            double* cluster_weights      = cluster->GetControlPointWeights();

            for( int i = 0; i < num_influenced_verts; ++i )
            {
                int fbx_vertex_index = cluster_indices[ i ];
                float weight = (float)( cluster_weights[ i ] );

                const VertexIndexArray& mce_vertex_indices = fbx_to_mce_indices[ fbx_vertex_index ];

                int num_mce_verts = (int)( mce_vertex_indices.size() );

                for( int j = 0; j < num_mce_verts; ++j )
                {
                    int mce_vertex_index = mce_vertex_indices[ j ];
                    MCE::FBX::FilmboxVertexWeightArray& vertex_weights = mesh_weights[ mce_vertex_index ];

                    MCE::FBX::FilmboxSkinWeight vertex_weight( fbx_bone.BoneNode, weight );
                    vertex_weights.push_back( vertex_weight );
                }
            }
        }

        skin_info.NumWeightsPerVertex = 0;

        for( int v = 0; v < num_verts; ++v )
        {
            MCE::FBX::FilmboxVertexWeightArray& vertex_weights = mesh_weights[ v ];
            int num_weights = (int)( vertex_weights.size() );

            if( num_weights > skin_info.NumWeightsPerVertex )
            {
                skin_info.NumWeightsPerVertex = num_weights;
            }
        }
    }

    //-----------------------------------------------------------------------------

    void ExtractSkinningInfo( KFbxNode* fbx_node, MCE::FBX::FilmboxSkinInfo& skin_info )
    {
        skin_info = MCE::FBX::FilmboxSkinInfo();
        skin_info.Node = fbx_node;

        MCE::FBX::FilmboxNode mce_node( fbx_node );
        if( mce_node.IsMesh() )
        {
            KFbxMesh* mesh = mce_node.GetMesh();
            skin_info.Mesh = mesh;

            ExtractMeshSkinningInfo( mesh, skin_info );
        }
    }

    //-----------------------------------------------------------------------------

    template< class FbxToMceConverterType >
    int InitializeVertexProperties( MCE::FBX::VertexArray& vertex_buffer, const KFbxMesh* mesh, int num_verts, const typename FbxToMceConverterType::LayerType* fbx_layer, int mce_channel = 0 )
    {
        int num_channels_added = 0;

        if( fbx_layer )
        {
            const FbxToMceConverterType::DataArrayType& fbx_data_array = fbx_layer->GetDirectArray();
            const FbxIndexArray&                        fbx_indices    = fbx_layer->GetIndexArray();

            KFbxLayerElement::EReferenceMode fbx_layer_reference_mode = fbx_layer->GetReferenceMode();
            KFbxLayerElement::EMappingMode   fbx_layer_mapping_mode   = fbx_layer->GetMappingMode();

            switch( fbx_layer_reference_mode )
            {
                case KFbxLayerElement::eDIRECT:
                {
                    int data_array_length = fbx_data_array.GetCount();

                    switch( fbx_layer_mapping_mode )
                    {
                        case KFbxLayerElement::eBY_CONTROL_POINT:
                        {
                            int num_polys = mesh->GetPolygonCount();

                            for( int p = 0; p < num_polys; ++p )
                            {
                                for( int v = 0; v < 3; ++v )
                                {
                                    int fbx_index = mesh->GetPolygonVertex( p, v );
                                    int mce_index = ( p * 3 ) + v;

                                    FbxToMceConverterType converter;
                                    converter.ConvertValue( fbx_data_array, fbx_index, vertex_buffer, mce_index, mce_channel );
                                }
                            }
                            
                            break;
                        }

                        case KFbxLayerElement::eBY_POLYGON_VERTEX:
                        {
                            FBX_ASSERT( data_array_length == num_verts, "Invalid number of vertex properties" );

                            for( int i = 0; i < num_verts; ++i )
                            {
                                FbxToMceConverterType converter;
                                converter.ConvertValue( fbx_data_array, i, vertex_buffer, i, mce_channel );
                            }
                            
                            break;
                        }
                        
                        default:
                        {
                            FBX_ASSERT_FALSE( "Unknown vertex property mapping mode" );
                            break;
                        }
                    }

                    break;
                }

                case KFbxLayerElement::eINDEX_TO_DIRECT:
                {
                    int num_indices = fbx_indices.GetCount();
                    FBX_ASSERT( num_indices == num_verts, "Invalid number of vertex properties" );

                    for( int i = 0; i < num_indices; ++i )
                    {
                        int fbx_index = fbx_indices[ i ];

                        FbxToMceConverterType converter;
                        converter.ConvertValue( fbx_data_array, fbx_index, vertex_buffer, i, mce_channel );
                    }

                    break;
                }

                default:
                {
                    FBX_ASSERT_FALSE( "Unknown layer reference mode for vertex data" );
                }

            }

            ++num_channels_added;
        }

        return num_channels_added;
    }

    //-----------------------------------------------------------------------------

    bool VerticesMatch( const MCE::FBX::VertexArray& vertices, int vertex_id_a, int vertex_id_b, float match_epsilon )
    {
        const MCE::FBX::VertexFormat& vertex_format = vertices.GetVertexFormat();
        
        if( vertex_format.HasPosition() )
        {
            const MCE::FBX::Vector3& value_a = vertices.GetPosition( vertex_id_a );
            const MCE::FBX::Vector3& value_b = vertices.GetPosition( vertex_id_b );

            if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
            {
                return false;
            }
        }

        if( vertex_format.HasNormal() )
        {
            const MCE::FBX::Vector3& value_a = vertices.GetNormal( vertex_id_a );
            const MCE::FBX::Vector3& value_b = vertices.GetNormal( vertex_id_b );

            if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
            {
                return false;
            }
        }

        if( vertex_format.HasBinormal() )
        {
            const MCE::FBX::Vector3& value_a = vertices.GetBinormal( vertex_id_a );
            const MCE::FBX::Vector3& value_b = vertices.GetBinormal( vertex_id_b );

            if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
            {
                return false;
            }
        }

        if( vertex_format.HasTangent() )
        {
            const MCE::FBX::Vector3& value_a = vertices.GetTangent( vertex_id_a );
            const MCE::FBX::Vector3& value_b = vertices.GetTangent( vertex_id_b );

            if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
            {
                return false;
            }
        }

        if( vertex_format.HasSkinIndices() )
        {
            int num_values = vertex_format.NumSkinIndices();
            
            for( int i = 0; i < num_values; ++i )
            {
                const MCE::FBX::SkinWeight& value_a = vertices.GetSkinWeight( vertex_id_a, i );
                const MCE::FBX::SkinWeight& value_b = vertices.GetSkinWeight( vertex_id_b, i );

                if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
                {
                    return false;
                }
            }
        }

        if( vertex_format.HasColors() )
        {
            int num_values = vertex_format.NumColors();
            
            for( int i = 0; i < num_values; ++i )
            {
                const MCE::FBX::Color& value_a = vertices.GetColor( vertex_id_a, i );
                const MCE::FBX::Color& value_b = vertices.GetColor( vertex_id_b, i );
                
                if( value_a != value_b )
                {
                    return false;
                }
            }
        }

        if( vertex_format.HasUVs() )
        {
            int num_values = vertex_format.NumUVs();
            
            for( int i = 0; i < num_values; ++i )
            {
                const MCE::FBX::Vector2& value_a = vertices.GetUV( vertex_id_a, i );
                const MCE::FBX::Vector2& value_b = vertices.GetUV( vertex_id_b, i );

                if( !MCE::FBX::IsSimilar( value_a, value_b, match_epsilon ) )
                {
                    return false;
                }
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------

    void BuildWeldingVertexTable( const MCE::FBX::VertexArray& vertices, VertexIndexArray& welded_indices, float weld_threshold )
    {
        int num_vertices = vertices.NumVerts();
        welded_indices.resize( num_vertices );

        // We start by assuming all vertices are unique        
        for( int v = 0; v < num_vertices; ++v )
        {
            welded_indices[ v ] = v;
        }

        // Now we replace any matching vertices with the index of the first match
        for( int v1 = 0; v1 < num_vertices; ++v1 )
        {
            if( v1 == welded_indices[ v1 ] )
            {
                // We have a unique vertex, so now we'll see if any vertices later in the list are a match
                for( int v2 = v1 + 1; v2 < num_vertices; ++v2 )
                {
                    if( VerticesMatch( vertices, v1, v2, weld_threshold ) )
                    {
                        welded_indices[ v2 ] = v1;
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------------------------

    int AddIfUnique( VertexIndexArray& vertex_indices, int vertex_index )
    {
        int unique_index = -1;

        int num_indices = (int)( vertex_indices.size() );
        
        for( int i = 0; i < num_indices; ++i )
        {
            if( vertex_index == vertex_indices[ i ] )
            {
                unique_index = i;
                break;
            }
        }
        
        if( unique_index == -1 )
        {
            vertex_indices.push_back( vertex_index );
            unique_index = num_indices;
        }
        
        return unique_index;
    }
    
    //-----------------------------------------------------------------------------

    void CopyVertices( const MCE::FBX::VertexArray& source_verts, MCE::FBX::VertexArray& dest_verts, const VertexIndexArray& source_indices )
    {
        int num_dest_verts = (int)( source_indices.size() );
        const MCE::FBX::VertexFormat& vertex_format = source_verts.GetVertexFormat();

        dest_verts.Initialize( num_dest_verts, vertex_format );

        for( int v = 0; v < num_dest_verts; ++v )
        {
            int source_vertex_id = source_indices[ v ];
            
            if( vertex_format.HasPosition() )
            {
                const MCE::FBX::Vector3& source_value = source_verts.GetPosition( source_vertex_id );
                dest_verts.SetPosition( v, source_value );
            }

            if( vertex_format.HasNormal() )
            {
                const MCE::FBX::Vector3& source_value = source_verts.GetNormal( source_vertex_id );
                dest_verts.SetNormal( v, source_value );
            }

            if( vertex_format.HasBinormal() )
            {
                const MCE::FBX::Vector3& source_value = source_verts.GetBinormal( source_vertex_id );
                dest_verts.SetBinormal( v, source_value );
            }

            if( vertex_format.HasTangent() )
            {
                const MCE::FBX::Vector3& source_value = source_verts.GetTangent( source_vertex_id );
                dest_verts.SetTangent( v, source_value );
            }

            if( vertex_format.HasSkinIndices() )
            {
                int num_values = vertex_format.NumSkinIndices();
                
                for( int i = 0; i < num_values; ++i )
                {
                    const MCE::FBX::SkinWeight& source_value = source_verts.GetSkinWeight( source_vertex_id, i );
                    dest_verts.SetSkinWeight( v, i, source_value );
                }
            }

            if( vertex_format.HasColors() )
            {
                int num_values = vertex_format.NumColors();
                
                for( int i = 0; i < num_values; ++i )
                {
                    const MCE::FBX::Color& source_value = source_verts.GetColor( source_vertex_id, i );
                    dest_verts.SetColor( v, i, source_value );
                }
            }

            if( vertex_format.HasUVs() )
            {
                int num_values = vertex_format.NumUVs();
                
                for( int i = 0; i < num_values; ++i )
                {
                    const MCE::FBX::Vector2& source_value = source_verts.GetUV( source_vertex_id, i );
                    dest_verts.SetUV( v, i, source_value );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------

    void WeldVertices( MCE::FBX::Mesh& mesh, float weld_threshold )
    {
        MCE::FBX::VertexArray&   source_vertices  = mesh.GetVertices();
        MCE::FBX::TriangleArray& source_triangles = mesh.GetTriangles();

        int num_source_verts     = source_vertices.NumVerts();
        int num_source_triangles = int( source_triangles.size() );

        // Create a look-up table that holds the first matching index for each vertex
        VertexIndexArray welding_vertex_ids;
        BuildWeldingVertexTable( source_vertices, welding_vertex_ids, weld_threshold );

        // Create new welded triangles and a lookup table of all of the unique vertices used
        MCE::FBX::TriangleArray welded_triangles;
        VertexIndexArray        welded_vertex_ids;

        for( int source_triangle_index = 0; source_triangle_index < num_source_triangles; ++source_triangle_index )
        {
            const MCE::FBX::Triangle& source_triangle = source_triangles[ source_triangle_index ];
            int source_material_id = source_triangle.MaterialId;

            MCE::FBX::Triangle welded_triangle;
            welded_triangle.MaterialId = source_material_id;

            for( int v = 0; v < 3; ++v )
            {
                int source_vert_index   = source_triangle.Vertex[ v ];
                int matching_vert_index = welding_vertex_ids[ source_vert_index ];

                welded_triangle.Vertex[ v ] = AddIfUnique( welded_vertex_ids, matching_vert_index );
            }

            welded_triangles.push_back( welded_triangle );
        }

        // Create the welded vertex array
        MCE::FBX::VertexArray welded_vertices;
        CopyVertices( source_vertices, welded_vertices, welded_vertex_ids );

        // Replace the originals with the welded versions
        source_vertices  = welded_vertices;
        source_triangles = welded_triangles;
    }

    //-----------------------------------------------------------------------------

    void CopyTrianglesToIndexBuffer( const MCE::FBX::TriangleArray& triangles, unsigned int* index_buffer, int num_triangles, int base_triangle_index )
    {
        int vert_index = 0;

        for( int t = 0; t < num_triangles; ++t )
        {
            int triangle_index = base_triangle_index + t;
            const MCE::FBX::Triangle& triangle = triangles[ triangle_index ];

            for( int v = 0; v < 3; ++v )
            {
                index_buffer[ vert_index ] = triangle.Vertex[ v ];
                ++vert_index;
            }
        }
    }

    //-----------------------------------------------------------------------------

    void CopyIndexBufferToTriangles( const unsigned int* index_buffer, MCE::FBX::TriangleArray& triangles, int num_triangles, int base_triangle_index )
    {
        int vert_index = 0;

        for( int t = 0; t < num_triangles; ++t )
        {
            int triangle_index = base_triangle_index + t;
            MCE::FBX::Triangle& triangle = triangles[ triangle_index ];

            for( int v = 0; v < 3; ++v )
            {
                triangle.Vertex[ v ] = index_buffer[ vert_index ];
                ++vert_index;
            }
        }
    }

    //-----------------------------------------------------------------------------

    void OptimizeMeshSurface( const MCE::FBX::Surface& surface, MCE::FBX::TriangleArray& triangles, const MCE::FBX::Vector3* vertex_buffer, int num_verts, int vertex_stride )
    {
        // Copy triangle vertex indices to raw index buffer
        int num_triangles       = surface.NumTriangles;
        int num_indices         = num_triangles * 3;
        int base_triangle_index = surface.StartIndex / 3;

        unsigned int* index_buffer = new unsigned int[ num_indices ];
        CopyTrianglesToIndexBuffer( triangles, index_buffer, num_triangles, base_triangle_index );

        // Optimize index buffer
        TootleFaceWinding winding = TOOTLE_CW;
        unsigned int cache_size   = TOOTLE_DEFAULT_VCACHE_SIZE;
        unsigned int num_clusters = 0;

        TootleResult result = TootleFastOptimize( vertex_buffer, index_buffer, num_verts, num_triangles, vertex_stride, cache_size, winding, index_buffer, &num_clusters );
        FBX_ASSERT( result == TOOTLE_OK, "Unknown error in TootleFastOptimize" );

        // Copy optimized index buffer back to triangles
        CopyIndexBufferToTriangles( index_buffer, triangles, num_triangles, base_triangle_index );

        // Free index buffer
        if( index_buffer )
        {
            delete[] index_buffer;
            index_buffer = NULL;
        }
    }

    //-----------------------------------------------------------------------------

    void OptimizeMesh( MCE::FBX::Mesh& mesh )
    {
        MCE::FBX::VertexArray&   vertices  = mesh.GetVertices();
        MCE::FBX::TriangleArray& triangles = mesh.GetTriangles();
        
        // Copy vertex positions to raw vertex buffer
        int num_verts = vertices.NumVerts();

        MCE::FBX::Vector3* vertex_buffer = new MCE::FBX::Vector3[ num_verts ];
        unsigned int       vertex_stride = sizeof( MCE::FBX::Vector3 );
        
        for( int v = 0; v < num_verts; ++v )
        {
            vertex_buffer[ v ] = vertices.GetPosition( v );
        }

        // Optimize mesh surfaces
        const MCE::FBX::SurfaceArray& surfaces = mesh.GetSurfaces();
        int num_surfaces = (int)( surfaces.size() );

        for( int s = 0; s < num_surfaces; ++s )
        {
            const MCE::FBX::Surface& surface = surfaces[ s ];
            OptimizeMeshSurface( surface, triangles, vertex_buffer, num_verts, vertex_stride );
        }

        // Free vertex buffer
        if( vertex_buffer )
        {
            delete[] vertex_buffer;
            vertex_buffer = NULL;
        }
    }
}

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        FilmboxManager::FilmboxManager()
        : m_fbx_manager( NULL )
        , m_geometry_converter( NULL )
        , m_fbx_scene_info()
        , m_optimize_meshes( false )
        , m_weld_threshold( 0.001f )
        {
            m_fbx_manager = KFbxSdkManager::Create();
    
            if( m_fbx_manager )
            {
                KFbxIOSettings* io_settings = KFbxIOSettings::Create( m_fbx_manager, IOSROOT );
                m_fbx_manager->SetIOSettings( io_settings );

                m_geometry_converter = new KFbxGeometryConverter( m_fbx_manager );
            }
        }

        //-----------------------------------------------------------------------------

        FilmboxManager::~FilmboxManager()
        {
            if( m_geometry_converter )
            {
                delete m_geometry_converter;
                m_geometry_converter = NULL;
            }

            if( m_fbx_manager )
            {
                m_fbx_manager->Destroy();
                m_fbx_manager = NULL;
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::SetOptimizeMeshes( bool optimize_meshes )
        {
            m_optimize_meshes = optimize_meshes;
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::SetWeldThreshold( float weld_threshold )
        {
            m_weld_threshold = weld_threshold;
        }

        //-----------------------------------------------------------------------------

        KFbxScene* FilmboxManager::ImportScene( const char* filename ) const
        {
            KFbxScene* scene = CreateScene();

            if( m_fbx_manager )
            {
                KFbxIOSettings* io_settings = m_fbx_manager->GetIOSettings();
        
                KFbxImporter* importer = KFbxImporter::Create( m_fbx_manager, "" );
                bool init_importer_success = importer->Initialize( filename, -1, io_settings );
        
                if( init_importer_success )
                {
                    int sdk_version_major = 0;
                    int sdk_version_minor = 0;
                    int sdk_version_revision = 0;

                    KFbxSdkManager::GetFileFormatVersion( sdk_version_major, sdk_version_minor, sdk_version_revision );

                    int file_version_major = 0;
                    int file_version_minor = 0;
                    int file_version_revision = 0;
            
                    importer->GetFileVersion( file_version_major, file_version_minor, file_version_revision );
            
                    if( importer->IsFBX() )
                    {
                        io_settings->SetBoolProp( IMP_FBX_MATERIAL,        true );
                        io_settings->SetBoolProp( IMP_FBX_TEXTURE,         true );
                        io_settings->SetBoolProp( IMP_FBX_LINK,            true );
                        io_settings->SetBoolProp( IMP_FBX_SHAPE,           true );
                        io_settings->SetBoolProp( IMP_FBX_GOBO,            true );
                        io_settings->SetBoolProp( IMP_FBX_ANIMATION,       true );
                        io_settings->SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );

                        bool import_success = importer->Import( scene );
                
                        if( !import_success )
                        {
                            FBX_ASSERT( import_success, "Error during FBX import" );
                        }
                    }
            
                    importer->Destroy();
                }
            }
    
            return scene;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Scene* FilmboxManager::ConvertScene( KFbxScene* fbx_scene )
        {
            FBX_ASSERT_FALSE( "Test!" );
            FBX_ASSERT( fbx_scene, "Null scene" );

            MCE::FBX::Scene* mce_scene = NULL;

            m_fbx_scene_info = MCE::FBX::FilmboxSceneInfo();

            if( fbx_scene )
            {
                PreprocessScene( *fbx_scene );

                MCE::FBX::FilmboxSceneInfo fbx_scene_info;
                ExtractSceneInfo( *fbx_scene );

                mce_scene = new MCE::FBX::Scene;
                ExtractMaterials( *fbx_scene, *mce_scene );
                ExtractNodes( *fbx_scene, *mce_scene );
            }

            return mce_scene;
        }

        //-----------------------------------------------------------------------------

        KFbxScene* FilmboxManager::CreateScene() const
        {
            KFbxScene* scene = NULL;

            if( m_fbx_manager )
            {
                scene = KFbxScene::Create( m_fbx_manager, "" );
            }
    
            return scene;
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::PreprocessScene( KFbxScene& fbx_scene ) const
        {
            ConvertCoordinateSystem( fbx_scene );
            TriangulateGeometry( fbx_scene );
            BakePivotOffsetsIntoGeometry( fbx_scene );
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ConvertCoordinateSystem( KFbxScene& fbx_scene ) const
        {
            KFbxGlobalSettings& scene_settings    = fbx_scene.GetGlobalSettings();
            KFbxAxisSystem      scene_axis_system = scene_settings.GetAxisSystem();

            if( scene_axis_system != KFbxAxisSystem::OpenGL )
            {
                KFbxAxisSystem::OpenGL.ConvertScene( &fbx_scene );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::TriangulateGeometry( KFbxScene& fbx_scene ) const
        {
            KFbxNode* root_node = fbx_scene.GetRootNode();

            if( root_node )
            {
                TriangulateGeometry( root_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::TriangulateGeometry( KFbxNode* root_fbx_node ) const
        {
            FBX_ASSERT( root_fbx_node, "Null node" );

            MCE::FBX::FilmboxNode root_mce_node( root_fbx_node );

            if( root_mce_node.IsMesh() )
            {
                m_geometry_converter->TriangulateInPlace( root_fbx_node );
            }

            int num_children = root_mce_node.NumChildren();

            for( int i = 0; i < num_children; ++i )
            {
                KFbxNode* child_fbx_node = root_mce_node.GetChildNode( i );
                TriangulateGeometry( child_fbx_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::BakePivotOffsetsIntoGeometry( KFbxScene& fbx_scene ) const
        {
            KFbxNode* root_node = fbx_scene.GetRootNode();

            if( root_node )
            {
                BakePivotOffsetsIntoGeometry( root_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::BakePivotOffsetsIntoGeometry( KFbxNode* root_fbx_node ) const
        {
            FBX_ASSERT( root_fbx_node, "Null node" );

            MCE::FBX::FilmboxNode root_mce_node( root_fbx_node );

            if( root_mce_node.IsMesh() )
            {
                KFbxXMatrix pivot = root_mce_node.GetGeometricTransform();

                KFbxMesh* mesh = root_mce_node.GetMesh();
                mesh->SetPivot( pivot );
                mesh->ApplyPivot();
            }

            int num_children = root_mce_node.NumChildren();

            for( int i = 0; i < num_children; ++i )
            {
                KFbxNode* child_fbx_node = root_mce_node.GetChildNode( i );
                BakePivotOffsetsIntoGeometry( child_fbx_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractSceneInfo( KFbxScene& fbx_scene )
        {
            m_fbx_scene_info = MCE::FBX::FilmboxSceneInfo();

            ExtractMaterialsInfo( fbx_scene );

            KFbxNode* fbx_root_node = fbx_scene.GetRootNode();

            if( fbx_root_node )
            {
                ExtractChildNodesInfo( *fbx_root_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractMaterialsInfo( KFbxScene& fbx_scene )
        {
            MCE::FBX::FilmboxMaterialInfoArray& materials_infos = m_fbx_scene_info.Materials;

            int num_materials = fbx_scene.GetMaterialCount();

            for( int m = 0; m < num_materials; ++m )
            {
                KFbxSurfaceMaterial* fbx_material = fbx_scene.GetMaterial( m );

                MCE::FBX::FilmboxMaterialInfo material_info;
                material_info.Material = fbx_material;
                material_info.Name     = fbx_material->GetName();
                
                MCE::FBX::ShaderParameterArray& shader_params = material_info.ShaderParameters;

                const KFbxImplementation* material_implementation = GetImplementation( fbx_material, ImplementationHLSL );

                if( !material_implementation )
                {
                    material_implementation = GetImplementation( fbx_material, ImplementationCGFX );
                }

                if( material_implementation )
                {
                    const KFbxBindingTable* binding_table = material_implementation->GetRootTable();

                    fbxString desc_relative_url = binding_table->DescRelativeURL.Get();
                    fbxString desc_absolute_url = binding_table->DescAbsoluteURL.Get();
                    fbxString desc_tag          = binding_table->DescTAG.Get();
                    
                    material_info.ShaderFilename = desc_absolute_url;

                    KFbxProperty root_property = fbx_material->RootProperty;
                    KFbxProperty constants     = material_implementation->GetConstants();

                    int num_entries = (int)( binding_table->GetEntryCount() );

                    for( int i = 0; i < num_entries; ++i )
                    {
                        const KFbxBindingTableEntry& entry = binding_table->GetEntry( i );

                        const char* entry_type   = entry.GetEntryType( true );
                        const char* entry_source = entry.GetSource();

                        KFbxProperty fbx_property;
                        if( MCE::FBX::StringCompare( entry_type, KFbxPropertyEntryView::sEntryType ) == 0 )
                        {
                            fbx_property = fbx_material->FindPropertyHierarchical( entry_source );

                            if( !fbx_property.IsValid() )
                            {
                                fbx_property = root_property.FindHierarchical( entry_source );
                            }
                        }
                        else if( MCE::FBX::StringCompare( entry_type, KFbxConstantEntryView::sEntryType ) == 0 )
                        {
                            
                            fbx_property = constants.FindHierarchical( entry_source );
                        }

                        if( fbx_property.IsValid() )
                        {
                            KFbxDataType property_type        = fbx_property.GetPropertyDataType();
                            KString      property_name_string = fbx_property.GetName();
                            const char*  property_name        = property_name_string.Buffer();
                            const char*  property_type_name   = property_type.GetName();

                            int num_textures = fbx_property.GetSrcObjectCount( FBX_TYPE( KFbxTexture ) );

                            if( num_textures > 0 )
                            {
                                FBX_ASSERT( num_textures == 1, "Arrays of textures are not supported!" );
                                KFbxTexture* fbx_texture     = fbx_property.GetSrcObject( FBX_TYPE( KFbxTexture ), 0 );
                                const char*  fbx_param_value = fbx_texture->GetName();

                                MCE::FBX::TextureShaderParameter* shader_param = new MCE::FBX::TextureShaderParameter( property_name );
                                shader_param->Value = fbx_param_value;

                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTBool )
                            {
                                bool fbx_param_value = KFbxGet< bool >( fbx_property );

                                MCE::FBX::BoolShaderParameter* shader_param = new MCE::FBX::BoolShaderParameter( property_name );
                                shader_param->Value = fbx_param_value;
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTInteger || property_type == DTEnum )
                            {
                                int fbx_param_value = KFbxGet< int >( fbx_property );

                                MCE::FBX::IntShaderParameter* shader_param = new MCE::FBX::IntShaderParameter( property_name );
                                shader_param->Value = fbx_param_value;
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTFloat )
                            {
                                float fbx_param_value = KFbxGet< float >( fbx_property );

                                MCE::FBX::FloatShaderParameter* shader_param = new MCE::FBX::FloatShaderParameter( property_name );
                                shader_param->Value = fbx_param_value;
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTDouble )
                            {
                                double fbx_param_value = KFbxGet< double >( fbx_property );

                                MCE::FBX::FloatShaderParameter* shader_param = new MCE::FBX::FloatShaderParameter( property_name );
                                shader_param->Value = (float)fbx_param_value;
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTDouble2 )
                            {
                                fbxDouble2 fbx_param_value = KFbxGet< fbxDouble2 >( fbx_property );

                                MCE::FBX::Vector2ShaderParameter* shader_param = new MCE::FBX::Vector2ShaderParameter( property_name );
                                shader_param->Value.X = (float)fbx_param_value[ 0 ];
                                shader_param->Value.Y = (float)fbx_param_value[ 1 ];
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTDouble3 || property_type == DTVector3D )
                            {
                                fbxDouble3 fbx_param_value = KFbxGet< fbxDouble3 >( fbx_property );

                                MCE::FBX::Vector3ShaderParameter* shader_param = new MCE::FBX::Vector3ShaderParameter( property_name );
                                shader_param->Value.X = (float)fbx_param_value[ 0 ];
                                shader_param->Value.Y = (float)fbx_param_value[ 1 ];
                                shader_param->Value.Z = (float)fbx_param_value[ 2 ];
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTColor3 )
                            {
                                fbxDouble3 fbx_param_value = KFbxGet< fbxDouble3 >( fbx_property );

                                MCE::FBX::Color3ShaderParameter* shader_param = new MCE::FBX::Color3ShaderParameter( property_name );
                                shader_param->Value.X = (float)fbx_param_value[ 0 ];
                                shader_param->Value.Y = (float)fbx_param_value[ 1 ];
                                shader_param->Value.Z = (float)fbx_param_value[ 2 ];
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTDouble4 || property_type == DTVector4D )
                            {
                                fbxDouble4 fbx_param_value = KFbxGet< fbxDouble4 >( fbx_property );

                                MCE::FBX::Vector4ShaderParameter* shader_param = new MCE::FBX::Vector4ShaderParameter( property_name );
                                shader_param->Value.X = (float)fbx_param_value[ 0 ];
                                shader_param->Value.Y = (float)fbx_param_value[ 1 ];
                                shader_param->Value.Z = (float)fbx_param_value[ 2 ];
                                shader_param->Value.W = (float)fbx_param_value[ 3 ];
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTColor4 )
                            {
                                fbxDouble4 fbx_param_value = KFbxGet< fbxDouble4 >( fbx_property );

                                MCE::FBX::Color4ShaderParameter* shader_param = new MCE::FBX::Color4ShaderParameter( property_name );
                                shader_param->Value.X = (float)fbx_param_value[ 0 ];
                                shader_param->Value.Y = (float)fbx_param_value[ 1 ];
                                shader_param->Value.Z = (float)fbx_param_value[ 2 ];
                                shader_param->Value.W = (float)fbx_param_value[ 3 ];
                                
                                shader_params.push_back( shader_param );
                            }
                            else if( property_type == DTString || property_type == DTUrl || property_type == DTXRefUrl )
                            {
                                fbxString fbx_param_value = KFbxGet< fbxString >( fbx_property );

                                MCE::FBX::StringShaderParameter* shader_param = new MCE::FBX::StringShaderParameter( property_name );
                                shader_param->Value = fbx_param_value.Buffer();
                                
                                shader_params.push_back( shader_param );
                            }
                        }
                    }
                }

                materials_infos.push_back( material_info );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractChildNodesInfo( KFbxNode& fbx_root_node )
        {
            int num_children = fbx_root_node.GetChildCount();
    
            for( int i = 0; i < num_children; ++i )
            {
                KFbxNode* fbx_child_node = fbx_root_node.GetChild( i );

                if( fbx_child_node )
                {
                    MCE::FBX::FilmboxSkinInfo skin_info;
                    ExtractSkinningInfo( fbx_child_node, skin_info );

                    m_fbx_scene_info.Nodes.push_back( fbx_child_node );
                    m_fbx_scene_info.Skins.push_back( skin_info );

                    MCE::FBX::FilmboxNode mce_child_node( fbx_child_node );

                    if( mce_child_node.IsBone() )
                    {
                        m_fbx_scene_info.Bones.push_back( fbx_child_node );
                    }

                    ExtractChildNodesInfo( *fbx_child_node );
                }
            }
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::FilmboxBone* FilmboxManager::GetBoneInfo( const KFbxNode* fbx_bone_node ) const
        {
            const MCE::FBX::FilmboxSkinInfoArray& skin_infos = m_fbx_scene_info.Skins;

            int num_skin_infos = (int)( skin_infos.size() );

            for( int s = 0; s < num_skin_infos; ++s )
            {
                const MCE::FBX::FilmboxSkinInfo& skin_info = skin_infos[ s ];

                if( skin_info.IsSkinned )
                {
                    const MCE::FBX::FilmboxBoneArray& bones = skin_info.Bones;

                    int num_bones = (int)( bones.size() );

                    for( int b = 0; b < num_bones; ++b )
                    {
                        const MCE::FBX::FilmboxBone& bone = bones[ b ];

                        if( fbx_bone_node == bone.BoneNode )
                        {
                            return &bone;
                        }
                    }
                }
            }

            return NULL;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::FilmboxSkinInfo* FilmboxManager::GetSkinInfo( const KFbxMesh* fbx_mesh ) const
        {
            const MCE::FBX::FilmboxSkinInfo* skin_info = NULL;

            const MCE::FBX::FilmboxSkinInfoArray& skin_infos = m_fbx_scene_info.Skins;
            int num_skin_infos = (int)( skin_infos.size() );

            for( int s = 0; s < num_skin_infos; ++s )
            {
                const MCE::FBX::FilmboxSkinInfo& scene_skin_info = skin_infos[ s ];

                if( fbx_mesh == scene_skin_info.Mesh )
                {
                    skin_info = &scene_skin_info;
                    break;
                }
            }

            return skin_info;
        }

        //-----------------------------------------------------------------------------

        int FilmboxManager::GetBoneIndex( const KFbxNode* bone_node ) const
        {
            int bone_index = -1;

            const MCE::FBX::FilmboxNodeArray& bones = m_fbx_scene_info.Bones;

            int num_bones = (int)( bones.size() );

            for( int i = 0; i < num_bones; ++i )
            {
                const KFbxNode* scene_bone = bones[ i ];

                if( bone_node == scene_bone )
                {
                    bone_index = i;
                    break;
                }
            }

            return bone_index;
        }

        //-----------------------------------------------------------------------------

        int FilmboxManager::GetMaterialIndex( const KFbxSurfaceMaterial* fbx_material ) const
        {
            int material_index = -1;

            const MCE::FBX::FilmboxMaterialInfoArray& scene_materials = m_fbx_scene_info.Materials;

            int num_scene_materials = (int)( scene_materials.size() );

            for( int i = 0; i < num_scene_materials; ++i )
            {
                const MCE::FBX::FilmboxMaterialInfo& material_info = scene_materials[ i ];

                if( material_info.Material == fbx_material )
                {
                    material_index = i;
                    break;
                }
            }

            return material_index;
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractMaterials( KFbxScene& fbx_scene, MCE::FBX::Scene& mce_scene ) const
        {
            const MCE::FBX::FilmboxMaterialInfoArray& scene_materials = m_fbx_scene_info.Materials;

            int num_scene_materials = (int)( scene_materials.size() );

            for( int i = 0; i < num_scene_materials; ++i )
            {
                const MCE::FBX::FilmboxMaterialInfo& material_info = scene_materials[ i ];
                
                MCE::FBX::Material material;
                material.Name             = material_info.Name;
                material.ShaderFilename   = material_info.ShaderFilename;
                material.ShaderParameters = material_info.ShaderParameters;
                
                mce_scene.AddMaterial( material );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractNodes( KFbxScene& fbx_scene, MCE::FBX::Scene& mce_scene ) const
        {
            KFbxNode* fbx_root_node = fbx_scene.GetRootNode();
    
            if( fbx_root_node )
            {
                MCE::FBX::SceneNode& scene_root_node = mce_scene.GetRootNode();
                ExtractChildNodes( fbx_root_node, mce_scene, scene_root_node );
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ExtractChildNodes( KFbxNode* fbx_root_node, MCE::FBX::Scene& mce_scene, MCE::FBX::SceneNode& scene_root_node ) const
        {
            FBX_ASSERT( fbx_root_node, "Null fbx node" );

            int num_children = fbx_root_node->GetChildCount();

            for( int i = 0; i < num_children; ++i )
            {
                KFbxNode*            fbx_child_node   = fbx_root_node->GetChild( i );
                MCE::FBX::SceneNode* scene_child_node = ConvertNode( fbx_child_node, mce_scene );

                if( scene_child_node )
                {
                    scene_root_node.AddChildNode( scene_child_node );
                    ExtractChildNodes( fbx_child_node, mce_scene, *scene_child_node );
                }
            }
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::SceneNode* FilmboxManager::ConvertNode( KFbxNode* fbx_node, MCE::FBX::Scene& mce_scene ) const
        {
            MCE::FBX::SceneNode* scene_node = new MCE::FBX::SceneNode( &mce_scene );

            MCE::FBX::FilmboxNode mce_fbx_node( fbx_node );

            // Name
            const char* node_name = mce_fbx_node.GetName();
            scene_node->SetName( node_name );

            // Object
            MCE::FBX::SceneObjectType::SceneObjectTypes type = mce_fbx_node.GetType();

            switch( type )
            {
                case MCE::FBX::SceneObjectType::BONE:
                {
                    ConvertBoneNode( *scene_node, mce_fbx_node );
                    break;
                }

                case MCE::FBX::SceneObjectType::CAMERA:
                {
                    ConvertCameraNode( *scene_node, mce_fbx_node );
                    break;
                }

                case MCE::FBX::SceneObjectType::MESH:
                {
                    ConvertMeshNode( *scene_node, mce_fbx_node );
                    break;
                }

                default:
                {
                    FBX_ASSERT_FALSE( "Unknown object type!" );
                    break;
                }
            }

            // Transform
            KFbxXMatrix local_transform = mce_fbx_node.GetLocalTransform();

            MCE::FBX::Transform& node_transform = scene_node->GetTransform();
            node_transform.Position = MCE::FBX::ExtractTranslation( local_transform );
            node_transform.Rotation = MCE::FBX::ExtractRotation( local_transform );
            node_transform.Scale    = MCE::FBX::ExtractScale( local_transform );

            // Animation
            MCE::FBX::TimeRange anim_range = mce_fbx_node.GetAnimationTimeRange();

            int anim_start_frame = anim_range.StartFrame;
            int anim_end_frame   = anim_range.EndFrame;

            MCE::FBX::Vector3Animation&    position_anim = scene_node->GetPositionAnimation();
            MCE::FBX::QuaternionAnimation& rotation_anim = scene_node->GetRotationAnimation();
            MCE::FBX::Vector3Animation&    scale_anim    = scene_node->GetScaleAnimation();

            position_anim.SetTimeRange( anim_range );
            rotation_anim.SetTimeRange( anim_range );
            scale_anim.SetTimeRange( anim_range );

            for( int t = anim_start_frame; t <= anim_end_frame; ++t )
            {
                KFbxXMatrix anim_transform = mce_fbx_node.GetLocalTransform( t );

                MCE::FBX::Vector3    position = MCE::FBX::ExtractTranslation( anim_transform );
                MCE::FBX::Quaternion rotation = MCE::FBX::ExtractRotation( anim_transform );
                MCE::FBX::Vector3    scale    = MCE::FBX::ExtractScale( anim_transform );

                int key_index = t - anim_start_frame;

                position_anim.SetKeyframe( key_index, position );
                rotation_anim.SetKeyframe( key_index, rotation );
                scale_anim.SetKeyframe( key_index, scale );
            }

            return scene_node;
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ConvertBoneNode( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const
        {
            MCE::FBX::Bone* bone = new MCE::FBX::Bone;

            const KFbxNode* fbx_node  = mce_fbx_node.GetNode();
            const char*     bone_name = mce_fbx_node.GetName(); // For debugging

            const MCE::FBX::FilmboxBone* fbx_bone_info = GetBoneInfo( fbx_node );

            if( fbx_bone_info )
            {
                const KFbxXMatrix&   fbx_inv_bind_pose = fbx_bone_info->InverseBindPose;
                MCE::FBX::Transform& mce_inv_bind_pose = bone->GetInverseBindPose();

                mce_inv_bind_pose.Position = MCE::FBX::ExtractTranslation( fbx_inv_bind_pose );
                mce_inv_bind_pose.Rotation = MCE::FBX::ExtractRotation( fbx_inv_bind_pose );
                mce_inv_bind_pose.Scale    = MCE::FBX::ExtractScale( fbx_inv_bind_pose );
            }

            scene_node.SetObject( bone );
            scene_node.SetCollisionMesh( NULL );
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ConvertCameraNode( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const
        {
            MCE::FBX::Camera* camera = new MCE::FBX::Camera;

            const KFbxNode* fbx_node  = mce_fbx_node.GetNode();
            const char*     node_name = mce_fbx_node.GetName(); // For debugging

            const KFbxCamera* fbx_camera = mce_fbx_node.GetCamera();

            float near_plane = (float)( fbx_camera->GetNearPlane() );
            float far_plane  = (float)( fbx_camera->GetFarPlane() );

            double aspect_width  = fbx_camera->AspectWidth.Get();
            double aspect_height = fbx_camera->AspectHeight.Get();
            float aspect_ratio   = (float)( aspect_width / aspect_height );

            float fov = 0.0f;

            KFbxCamera::ECameraApertureMode aperture_mode = fbx_camera->GetApertureMode();

            switch( aperture_mode )
            {
                case KFbxCamera::eHORIZONTAL_AND_VERTICAL:
                case KFbxCamera::eVERTICAL:
                {
                    double fbx_fov = fbx_camera->FieldOfView.Get();
                    fov = MCE::FBX::DegreesToRadians( (float)fbx_fov );
                    break;
                }

                case KFbxCamera::eHORIZONTAL:
                {
                    double fbx_xfov = fbx_camera->FieldOfView.Get();
                    float xfov = MCE::FBX::DegreesToRadians( (float)fbx_xfov );
                    fov = 2.0f * atanf( tanf( xfov * 0.5f ) / aspect_ratio );
                    break;
                }

                case KFbxCamera::eFOCAL_LENGTH:
                {
                    double focal_length = fbx_camera->FocalLength.Get();
                    double fbx_fov = fbx_camera->ComputeFieldOfView( focal_length );
                    fov = MCE::FBX::DegreesToRadians( (float)fbx_fov );
                    break;
                }
            }

            KFbxCamera::ECameraProjectionType camera_type = fbx_camera->ProjectionType.Get();

            if( camera_type == KFbxCamera::eORTHOGONAL )
            {
                KFbxVector4 camera_position = KFbxVector4( fbx_camera->Position.Get() );
                KFbxVector4 target_position = KFbxVector4( fbx_camera->InterestPosition.Get() );
                KFbxVector4 target_vector   = target_position - camera_position;

                double focal_length = target_vector.Length();

                float half_target_width  = (float)( focal_length ) * tanf( fov * 0.5f * aspect_ratio );
                float half_target_height = (float)( focal_length ) * tanf( fov * 0.5f );

                float left  = -half_target_width;
                float right =  half_target_width;

                float top    =  half_target_height;
                float bottom = -half_target_height;

                camera->SetOrthographic( true );
                camera->SetLeftPlane( left );
                camera->SetRightPlane( right );
                camera->SetTopPlane( top );
                camera->SetBottomPlane( bottom );
                camera->SetNearPlane( near_plane );
                camera->SetFarPlane( far_plane );
            }
            else
            {
                camera->SetOrthographic( false );
                camera->SetFOV( fov );
                camera->SetAspectRatio( aspect_ratio );
                camera->SetNearPlane( near_plane );
                camera->SetFarPlane( far_plane );
            }

            scene_node.SetObject( camera );
            scene_node.SetCollisionMesh( NULL );
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::ConvertMeshNode( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const
        {
            MCE::FBX::Mesh* mesh = ConvertMesh( mce_fbx_node );
            FBX_ASSERT( mesh, "Error creating mesh" );

            MCE::FBX::Mesh* collision_mesh = CreateCollisionMesh( *mesh );
            FBX_ASSERT( collision_mesh, "Error creating collision mesh" );

            scene_node.SetObject( mesh );
            scene_node.SetCollisionMesh( collision_mesh );
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Mesh* FilmboxManager::ConvertMesh( const MCE::FBX::FilmboxNode& mesh_node ) const
        {
            MCE::FBX::Mesh* mce_mesh = NULL;

            const KFbxMesh* fbx_mesh = mesh_node.GetMesh();
            FBX_ASSERT( fbx_mesh, "Null mesh" );
            FBX_ASSERT( fbx_mesh->IsTriangleMesh(), "Invalid mesh" );

            if( fbx_mesh && fbx_mesh->IsTriangleMesh() )
            {
                mce_mesh = new MCE::FBX::Mesh();

                MCE::FBX::VertexArray& mce_mesh_verts = mce_mesh->GetVertices();
                InitializeVertexArray( mesh_node, mce_mesh_verts );

                MCE::FBX::TriangleArray& mce_mesh_triangles = mce_mesh->GetTriangles();
                InitializeTriangleArray( mesh_node, mce_mesh_triangles );
                
                if( m_optimize_meshes )
                {
                    WeldVertices( *mce_mesh, m_weld_threshold );
                }

                mce_mesh->BuildSurfaces();

                if( m_optimize_meshes )
                {
                    OptimizeMesh( *mce_mesh );
                }
            }

            return mce_mesh;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Mesh* FilmboxManager::CreateCollisionMesh( const MCE::FBX::Mesh& visual_mesh ) const
        {
            MCE::FBX::Mesh* collision_mesh = new MCE::FBX::Mesh();

            const MCE::FBX::VertexArray&   visual_mesh_verts     = visual_mesh.GetVertices();
            const MCE::FBX::TriangleArray& visual_mesh_triangles = visual_mesh.GetTriangles();

            // Vertex Format
            MCE::FBX::VertexFormat collision_vertex_format;
            collision_vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::POSITION );

            // Vertices
            int num_verts = visual_mesh_verts.NumVerts();

            MCE::FBX::VertexArray& collision_mesh_verts = collision_mesh->GetVertices();
            collision_mesh_verts.Initialize( num_verts, collision_vertex_format );

            for( int i = 0; i < num_verts; ++i )
            {
                const MCE::FBX::Vector3& position = visual_mesh_verts.GetPosition( i );
                collision_mesh_verts.SetPosition( i, position );
            }

            // Triangles
            MCE::FBX::TriangleArray& collision_mesh_triangles = collision_mesh->GetTriangles();
            collision_mesh_triangles = visual_mesh_triangles;

            return collision_mesh;
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::InitializeVertexArray( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::VertexArray& vertex_buffer ) const
        {
            const KFbxMesh* mesh = mesh_node.GetMesh();
            
            int num_layers = mesh->GetLayerCount();
            if( num_layers <= 0 ) { return; }

            // Vertex Elements
            MCE::FBX::VertexFormat vertex_format;
            InitializeVertexFormat( mesh_node, vertex_format );

            // Vertex Buffer
            int num_polys = mesh->GetPolygonCount();
            int num_verts = num_polys * 3;

            vertex_buffer.Initialize( num_verts, vertex_format );

            // Positions
            KFbxVector4* fbx_positions = mesh->GetControlPoints();

            for( int p = 0; p < num_polys; ++p )
            {
                for( int v = 0; v < 3; ++v )
                {
                    int fbx_vertex_index = mesh->GetPolygonVertex( p, v );
                    int mce_vertex_index = ( p * 3 ) + v;

                    const KFbxVector4& fbx_position = fbx_positions[ fbx_vertex_index ];
                    MCE::FBX::Vector3  mce_position( (float)fbx_position[ 0 ], (float)fbx_position[ 1 ], (float)fbx_position[ 2 ] );

                    vertex_buffer.SetPosition( mce_vertex_index, mce_position );
                }
            }

            const KFbxLayer* base_layer = mesh->GetLayer( 0 );

            // Normals
            if( vertex_format.HasNormal() )
            {
                const KFbxLayerElementNormal* fbx_normals_layer = base_layer->GetNormals();
                
                if( fbx_normals_layer )
                {
                    InitializeVertexProperties< FbxToMceNormalConverter >( vertex_buffer, mesh, num_verts, fbx_normals_layer );
                }
                else
                {
                    MCE::FBX::Vector3 mce_normal( 0.0f, 0.0f, 1.0f );
                    
                    for( int i = 0; i < num_verts; ++i )
                    {
                        vertex_buffer.SetNormal( i, mce_normal );
                    }
                }
            }

            // Tangents
            if( vertex_format.HasTangent() )
            {
                const KFbxLayerElementTangent* fbx_tangents_layer = base_layer->GetTangents();
                
                if( fbx_tangents_layer )
                {
                    InitializeVertexProperties< FbxToMceTangentConverter >( vertex_buffer, mesh, num_verts, fbx_tangents_layer );
                }
                else
                {
                    MCE::FBX::Vector3 mce_tangent( 1.0f, 0.0f, 0.0f );
                    
                    for( int i = 0; i < num_verts; ++i )
                    {
                        vertex_buffer.SetTangent( i, mce_tangent );
                    }
                }
            }

            // Binormals
            if( vertex_format.HasBinormal() )
            {
                const KFbxLayerElementBinormal* fbx_binormals_layer = base_layer->GetBinormals();
                
                if( fbx_binormals_layer )
                {
                    InitializeVertexProperties< FbxToMceBinormalConverter >( vertex_buffer, mesh, num_verts, fbx_binormals_layer );
                }
                else
                {
                    MCE::FBX::Vector3 mce_binormal( 0.0f, 1.0f, 0.0f );
                    
                    for( int i = 0; i < num_verts; ++i )
                    {
                        vertex_buffer.SetBinormal( i, mce_binormal );
                    }
                }
            }

            // Skin
            if( vertex_format.HasSkinIndices() && vertex_format.HasSkinWeights() )
            {
                InitializeSkinVertexProperties( vertex_buffer, num_verts, mesh );
            }

            // Colors
            if( vertex_format.HasColors() )
            {
                int mce_color_channel = 0;

                for( int layer_index = 0; layer_index < num_layers; ++layer_index )
                {
                    const KFbxLayer* layer = mesh->GetLayer( layer_index );
                    const KFbxLayerElementVertexColor* vertex_color_layer = layer->GetVertexColors();
                    
                    if( vertex_color_layer )
                    {
                        if( mce_color_channel < vertex_format.NumUVs() )
                        {
                            mce_color_channel += InitializeVertexProperties< FbxToMceColorConverter >( vertex_buffer, mesh, num_verts, vertex_color_layer, mce_color_channel );
                        }
                    }
                }
                
                if( mce_color_channel == 0 )
                {
                    MCE::FBX::Color mce_color( 255, 255, 255, 255 );
                    
                    for( int i = 0; i < num_verts; ++i )
                    {
                        vertex_buffer.SetColor( i, mce_color_channel, mce_color );
                    }
                }
            }

            // UVs
            if( vertex_format.HasUVs() )
            {
                int mce_uv_channel = 0;

                for( int layer_index = 0; layer_index < num_layers; ++layer_index )
                {
                    const KFbxLayer* layer = mesh->GetLayer( layer_index );

                    int num_uv_channels = layer->GetUVSetCount();
                    const KArrayTemplate< const KFbxLayerElementUV* >& layer_uv_channels = layer->GetUVSets();

                    for( int uv = 0; uv < num_uv_channels; ++uv )
                    {
                        if( mce_uv_channel < vertex_format.NumUVs() )
                        {
                            const KFbxLayerElementUV* fbx_uv_layer = layer_uv_channels[ uv ];
                            mce_uv_channel += InitializeVertexProperties< FbxToMceUVConverter >( vertex_buffer, mesh, num_verts, fbx_uv_layer, mce_uv_channel );
                        }
                    }
                }

                if( mce_uv_channel == 0 )
                {
                    MCE::FBX::Vector2 mce_uv( 0.0f, 0.0f );
                    
                    for( int i = 0; i < num_verts; ++i )
                    {
                        vertex_buffer.SetUV( i, mce_uv_channel, mce_uv );
                    }
                }
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::InitializeVertexFormat( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::VertexFormat& vertex_format ) const
        {
            const KFbxMesh* mesh = mesh_node.GetMesh();
            FBX_ASSERT( mesh, "Invalid mesh" );

            int num_layers = mesh->GetLayerCount();
            FBX_ASSERT( num_layers > 0, "Invalid mesh" );

            const KFbxLayer* base_layer = mesh->GetLayer( 0 );

            // Position
            vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::POSITION );

            // Normal
            if( base_layer->GetNormals() )
            {
                vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::NORMAL );
            }

            // Tangent
            if( base_layer->GetTangents() )
            {
                vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::TANGENT );
            }

            // Binormal
            if( base_layer->GetBinormals() )
            {
                vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::BINORMAL );
            }

            // Skin
            const MCE::FBX::FilmboxSkinInfo* skin_info = GetSkinInfo( mesh );
            FBX_ASSERT( skin_info, "Skinning data not found!" );

            if( skin_info->IsSkinned )
            {
                vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::SKIN_INDEX,  skin_info->NumWeightsPerVertex );
                vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::SKIN_WEIGHT, skin_info->NumWeightsPerVertex );
            }

            // Colors
            for( int i = 0; i < num_layers; ++i )
            {
                const KFbxLayer* layer = mesh->GetLayer( i );

                if( layer->GetVertexColors() )
                {
                    vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::COLOR );
                }
            }

            // UVs
            for( int i = 0; i < num_layers; ++i )
            {
                const KFbxLayer* layer = mesh->GetLayer( i );

                int num_uv_channels = layer->GetUVSetCount();

                for( int uv = 0; uv < num_uv_channels; ++uv )
                {
                    vertex_format.AddProperty( MCE::FBX::VertexPropertyUsage::UV );
                }
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::InitializeTriangleArray( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::TriangleArray& triangles ) const
        {
            const KFbxMesh* fbx_mesh = mesh_node.GetMesh();

            if( fbx_mesh && fbx_mesh->IsTriangleMesh() )
            {
                const KFbxLayer* base_layer = fbx_mesh->GetLayer( 0 );

                int polygon_count = fbx_mesh->GetPolygonCount();
                triangles.resize( polygon_count );

                // Set vertex indices and default material id
                for( int p = 0; p < polygon_count; ++p )
                {
                    MCE::FBX::Triangle& triangle = triangles[ p ];

                    for( int v = 0; v < 3; ++v )
                    {
                        triangle.Vertex[ v ] = ( p * 3 ) + v;
                        triangle.MaterialId = 0;
                    }
                }

                // Assign material IDs, if material(s) are assigned
                const FbxMaterialLayer* material_layer = base_layer->GetMaterials();

                if( material_layer )
                {
                    const FbxIndexArray& material_indices = material_layer->GetIndexArray();

                    KFbxLayerElement::EReferenceMode material_reference_mode = material_layer->GetReferenceMode();

                    switch( material_reference_mode )
                    {
                        case KFbxLayerElement::eDIRECT:
                        {
                            FBX_ASSERT( material_layer->mDirectArray->GetCount() == polygon_count, "Invalid number of material ids" );

                            for( int p = 0; p < polygon_count; ++p )
                            {
                                MCE::FBX::Triangle&        triangle     = triangles[ p ];
                                const KFbxSurfaceMaterial* fbx_material = material_layer->mDirectArray->GetAt( p );

                                triangle.MaterialId = GetMaterialIndex( fbx_material );
                            }

                            break;
                        }

                        case KFbxLayerElement::eINDEX_TO_DIRECT:
                        {
                            int num_indices = material_indices.GetCount();
                            FBX_ASSERT( num_indices == polygon_count, "Invalid number of material ids" );

                            for( int p = 0; p < polygon_count; ++p )
                            {
                                int fbx_index = material_indices[ p ];

                                MCE::FBX::Triangle&        triangle     = triangles[ p ];
                                const KFbxSurfaceMaterial* fbx_material = material_layer->mDirectArray->GetAt( fbx_index );

                                triangle.MaterialId = GetMaterialIndex( fbx_material );
                            }

                            break;
                        }

                        default:
                        {
                            FBX_ASSERT_FALSE( "Unknown reference mode for material ids" );
                        }
                    }
                }
            }
        }

        //-----------------------------------------------------------------------------

        void FilmboxManager::InitializeSkinVertexProperties( MCE::FBX::VertexArray& vertex_buffer, int num_verts, const KFbxMesh* mesh ) const
        {
            const MCE::FBX::FilmboxSkinInfo* skin_info = GetSkinInfo( mesh );
            FBX_ASSERT( skin_info, "Skinning data not found!" );

            const MCE::FBX::FilmboxMeshWeightArray& mesh_weights = skin_info->MeshWeights;
            int num_weights_per_vertex = skin_info->NumWeightsPerVertex;

            MCE::FBX::SkinWeight root_weight( 0, 1.0f );
            MCE::FBX::SkinWeight zero_weight( 0, 0.0f );

            for( int v = 0; v < num_verts; ++v )
            {
                // Initialize to be skinned 100% to the root bone
                vertex_buffer.SetSkinWeight( v, 0, root_weight );

                for( int w = 1; w < num_weights_per_vertex; ++w )
                {
                    vertex_buffer.SetSkinWeight( v, w, zero_weight );
                }

                // Now overwrite the initial weights with the weights the vertex has, if any
                const MCE::FBX::FilmboxVertexWeightArray& vertex_weights = mesh_weights[ v ];

                int num_vertex_weights = (int)( vertex_weights.size() );

                for( int w = 0; w < num_vertex_weights; ++w )
                {
                    const MCE::FBX::FilmboxSkinWeight& fbx_skin_weight = vertex_weights[ w ];

                    MCE::FBX::SkinWeight mce_skin_weight;
                    mce_skin_weight.BoneIndex = GetBoneIndex( fbx_skin_weight.BoneNode );
                    mce_skin_weight.Weight    = fbx_skin_weight.Weight;

                    vertex_buffer.SetSkinWeight( v, w, mce_skin_weight );
                }
            }
        }

        //-----------------------------------------------------------------------------

        void InitializeFilmboxManager()
        {
            if( !s_filmbox_manager )
            {
                s_filmbox_manager = new MCE::FBX::FilmboxManager;
            }
        }

        //-----------------------------------------------------------------------------

        FilmboxManager* GetFilmboxManager()
        {
            return s_filmbox_manager;
        }


        //-----------------------------------------------------------------------------

        void ReleaseFilmboxManager()
        {
            if( s_filmbox_manager )
            {
                delete s_filmbox_manager;
                s_filmbox_manager = NULL;
            }
        }

        //-----------------------------------------------------------------------------
    }
}
