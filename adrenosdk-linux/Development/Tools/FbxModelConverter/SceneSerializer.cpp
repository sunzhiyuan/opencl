//-----------------------------------------------------------------------------
// 
//  SceneSerializer
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "SceneSerializer.h"
#include "Model/FrmAnimation.h"
#include "Model/FrmModel.h"
#include "Model/FrmModelIO.h"
#include <algorithm>

//-----------------------------------------------------------------------------

namespace
{
    //-----------------------------------------------------------------------------

    void ConvertVector3( const MCE::FBX::Vector3& source, FRMVECTOR3& dest )
    {
        dest.x = source.X;
        dest.y = source.Y;
        dest.z = source.Z;
    }

    //-----------------------------------------------------------------------------

    void ConvertQuaternion( const MCE::FBX::Quaternion& source, FRMVECTOR4& dest )
    {
        dest.x = source.X;
        dest.y = source.Y;
        dest.z = source.Z;
        dest.w = source.W;
    }

    //-----------------------------------------------------------------------------

    void ConvertTransform( const MCE::FBX::Transform& source, Adreno::Transform& dest )
    {
        ConvertVector3( source.Position, dest.Position );
        ConvertQuaternion( source.Rotation, dest.Rotation );
        ConvertVector3( source.Scale, dest.Scale );
    }

    //-----------------------------------------------------------------------------

    void CollectSceneMeshes( const MCE::FBX::Scene& scene, MCE::FBX::ConstMeshArray& scene_meshes )
    {
        scene_meshes.clear();

        int num_objects = scene.NumObjects();
        
        for( int i = 0; i < num_objects; ++i )
        {
            const MCE::FBX::SceneObject* scene_object = scene.GetSceneObject( i );
            MCE::FBX::SceneObjectType::SceneObjectTypes object_type = scene_object->Type();
            
            if( object_type == MCE::FBX::SceneObjectType::MESH )
            {
                const MCE::FBX::Mesh* scene_mesh = dynamic_cast< const MCE::FBX::Mesh* >( scene_object );
                
                if( scene_mesh )
                {
                    scene_meshes.push_back( scene_mesh );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------

    void CollectChildNodes( const MCE::FBX::SceneNode& root_scene_node, MCE::FBX::ConstSceneNodeArray& scene_nodes )
    {
        int num_child_nodes = root_scene_node.NumChildNodes();

        for( int i = 0; i < num_child_nodes; ++i )
        {
            const MCE::FBX::SceneNode* child_node = root_scene_node.GetChildNode( i );

            if( child_node )
            {
                scene_nodes.push_back( child_node );
                CollectChildNodes( *child_node, scene_nodes );
            }
        }
    }

    //-----------------------------------------------------------------------------

    void CollectSceneNodes( const MCE::FBX::Scene& scene, MCE::FBX::ConstSceneNodeArray& scene_nodes )
    {
        scene_nodes.clear();

        const MCE::FBX::SceneNode& root_scene_node = scene.GetRootNode();
        scene_nodes.push_back( &root_scene_node );

        CollectChildNodes( root_scene_node, scene_nodes );
    }

    //-----------------------------------------------------------------------------

    int GetNodeDepth( const MCE::FBX::SceneNode* scene_node )
    {
        int depth = -1;
        
        if( scene_node )
        {
            depth = 0;
            
            const MCE::FBX::SceneNode* iter_node   = scene_node;
            const MCE::FBX::SceneNode* parent_node = scene_node->GetParentNode();
            
            while( parent_node )
            {
                ++depth;
                
                iter_node   = parent_node;
                parent_node = parent_node->GetParentNode();
            }
        }
        
        return depth;
    }

    //-----------------------------------------------------------------------------

    bool SortNodesByDepthCompareFunc( const MCE::FBX::SceneNode* a, const MCE::FBX::SceneNode* b )
    {
        int a_depth = GetNodeDepth( a );
        int b_depth = GetNodeDepth( b );

        return a_depth < b_depth;
    }

    //-----------------------------------------------------------------------------

    void SortNodesByDepth( MCE::FBX::ConstSceneNodeArray& scene_nodes )
    {
        std::sort( scene_nodes.begin(), scene_nodes.end(), SortNodesByDepthCompareFunc );
    }

    //-----------------------------------------------------------------------------

    int FindJointIndex( const MCE::FBX::ConstSceneNodeArray& nodes, const MCE::FBX::SceneNode* node )
    {
        int index = Adreno::Joint::NULL_NODE_INDEX;

        int num_nodes = (int)( nodes.size() );

        for( int i = 0; i < num_nodes; ++i )
        {
            if( node == nodes[ i ] )
            {
                index = i;
                break;
            }
        }

        return index;
    }

    //-----------------------------------------------------------------------------

    void ConvertVertexFormat( const MCE::FBX::VertexFormat& source_vertex_format, Adreno::VertexFormat& dest_vertex_format )
    {
        int num_vertex_properties = source_vertex_format.NumProperties();
        dest_vertex_format.ResizeProperties( num_vertex_properties );

        for( int i = 0; i < num_vertex_properties; ++i )
        {
            const MCE::FBX::VertexProperty& source_property = source_vertex_format.GetProperty( i );
            Adreno::VertexProperty dest_property;

            switch( source_property.Usage )
            {
                case MCE::FBX::VertexPropertyUsage::POSITION:
                {
                    dest_property.Usage = "position";
                    dest_property.Type  = Adreno::FLOAT3_TYPE;
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::NORMAL:
                {
                    dest_property.Usage = "normal";
                    dest_property.Type  = Adreno::FLOAT3_TYPE;
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::TANGENT:
                {
                    dest_property.Usage = "tangent";
                    dest_property.Type  = Adreno::FLOAT3_TYPE;
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::BINORMAL:
                {
                    dest_property.Usage = "binormal";
                    dest_property.Type  = Adreno::FLOAT3_TYPE;
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::SKIN_INDEX:
                {
                    dest_property.Usage = "skinindex";

                    switch( source_property.NumValues )
                    {
                        case 1: { dest_property.Type = Adreno::UINT1_TYPE; break; }
                        case 2: { dest_property.Type = Adreno::UINT2_TYPE; break; }
                        case 3: { dest_property.Type = Adreno::UINT3_TYPE; break; }
                        case 4: { dest_property.Type = Adreno::UINT4_TYPE; break; }
                    }

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::SKIN_WEIGHT:
                {
                    dest_property.Usage = "skinweight";
                    
                    switch( source_property.NumValues )
                    {
                        case 1: { dest_property.Type = Adreno::FLOAT1_TYPE; break; }
                        case 2: { dest_property.Type = Adreno::FLOAT2_TYPE; break; }
                        case 3: { dest_property.Type = Adreno::FLOAT3_TYPE; break; }
                        case 4: { dest_property.Type = Adreno::FLOAT4_TYPE; break; }
                    }
                    
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::COLOR:
                {
                    dest_property.Usage = "color";
                    dest_property.Type  = Adreno::UBYTE4N_TYPE;
                    break;
                }

                case MCE::FBX::VertexPropertyUsage::UV:
                {
                    dest_property.Usage = "texcoord";
                    dest_property.Type  = Adreno::FLOAT2_TYPE;
                    break;
                }
            }

            dest_vertex_format.SetProperty( i, dest_property );
        }
    }

    //-----------------------------------------------------------------------------

    void ConvertVertices( const MCE::FBX::VertexArray& source_vertices, Adreno::VertexBuffer& dest_vertex_buffer, const MCE::FBX::Scene& source_scene, const MCE::FBX::ConstSceneNodeArray& source_nodes )
    {
        const MCE::FBX::VertexFormat& source_vertex_format = source_vertices.GetVertexFormat();
        Adreno::VertexFormat&         dest_vertex_format   = dest_vertex_buffer.Format;

        ConvertVertexFormat( source_vertex_format, dest_vertex_format );

        int num_verts = source_vertices.NumVerts();
        dest_vertex_buffer.IntializeBuffer( num_verts );

        if( num_verts > 0 )
        {
            const MCE::FBX::VertexFormat& source_vertex_format = source_vertices.GetVertexFormat();

            int    num_vertex_properties   = source_vertex_format.NumProperties();
            uint32 vertex_stride           = source_vertex_format.VertexStride();
            uint8* dest_vertex_buffer_data = dest_vertex_buffer.Buffer;

            for( int v = 0; v < num_verts; ++v )
            {
                uint8* dest_vertex_data = dest_vertex_buffer_data + ( v * vertex_stride );

                for( int i = 0; i < num_vertex_properties; ++i )
                {
                    const MCE::FBX::VertexProperty& prop = source_vertex_format.GetProperty( i );
        
                    switch( prop.Usage )
                    {
                        case MCE::FBX::VertexPropertyUsage::POSITION:
                        {
                            MCE::FBX::Vector3& vertex_value = *( (MCE::FBX::Vector3*)dest_vertex_data );
                            vertex_value = source_vertices.GetPosition( v );
                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::NORMAL:
                        {
                            MCE::FBX::Vector3& vertex_value = *( (MCE::FBX::Vector3*)dest_vertex_data );
                            vertex_value = source_vertices.GetNormal( v );
                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::TANGENT:
                        {
                            MCE::FBX::Vector3& vertex_value = *( (MCE::FBX::Vector3*)dest_vertex_data );
                            vertex_value = source_vertices.GetTangent( v );
                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::BINORMAL:
                        {
                            MCE::FBX::Vector3& vertex_value = *( (MCE::FBX::Vector3*)dest_vertex_data );
                            vertex_value = source_vertices.GetBinormal( v );
                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::SKIN_INDEX:
                        {
                            int     num_values    = prop.NumValues;
                            uint32* vertex_values = (uint32*)dest_vertex_data;

                            for( int i = 0; i < num_values; ++i )
                            {
                                const MCE::FBX::SkinWeight& skin_weight = source_vertices.GetSkinWeight( v, i );

                                const MCE::FBX::SceneNode* bone_node = source_scene.GetBoneNode( skin_weight.BoneIndex );
                                int joint_index = FindJointIndex( source_nodes, bone_node );
                                
                                vertex_values[ i ] = joint_index;
                            }

                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::SKIN_WEIGHT:
                        {
                            int    num_values    = prop.NumValues;
                            float* vertex_values = (float*)dest_vertex_data;

                            for( int i = 0; i < num_values; ++i )
                            {
                                const MCE::FBX::SkinWeight& skin_weight = source_vertices.GetSkinWeight( v, i );
                                vertex_values[ i ] = skin_weight.Weight;
                            }

                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::COLOR:
                        {
                            MCE::FBX::Color& vertex_value = *( (MCE::FBX::Color*)dest_vertex_data );
                            vertex_value = source_vertices.GetColor( v, prop.Channel );
                            break;
                        }

                        case MCE::FBX::VertexPropertyUsage::UV:
                        {
                            MCE::FBX::Vector2& vertex_value = *( (MCE::FBX::Vector2*)dest_vertex_data );
                            vertex_value = source_vertices.GetUV( v, prop.Channel );
                            break;
                        }
                    }

                    dest_vertex_data += prop.NumValues * prop.ValueSize;
                }
            }
        }
    }

    //-----------------------------------------------------------------------------

    void ConvertIndices( const MCE::FBX::TriangleArray& source_triangles, Adreno::IndexBuffer& dest_index_buffer )
    {
        uint32 num_triangles = (uint32)( source_triangles.size() );
        uint32 num_indices   = num_triangles * 3;

        if( num_indices > 0 )
        {
            dest_index_buffer.Resize( num_indices );

            for( uint32 t = 0; t < num_triangles; ++t )
            {
                const MCE::FBX::Triangle& source_triangle = source_triangles[ t ];
                uint32* triangle_indices = &( dest_index_buffer.Indices[ t * 3 ] );
                
                triangle_indices[ 0 ] = source_triangle.Vertex[ 0 ];
                triangle_indices[ 1 ] = source_triangle.Vertex[ 1 ];
                triangle_indices[ 2 ] = source_triangle.Vertex[ 2 ];
            }
        }
    }

    //-----------------------------------------------------------------------------

    void ConvertSurfaces( const MCE::FBX::SurfaceArray& source_surfaces, Adreno::MeshSurfaceArray& dest_surfaces )
    {
        uint32 num_surfaces = (uint32)( source_surfaces.size() );
        dest_surfaces.Resize( num_surfaces );

        for( uint32 i = 0; i < num_surfaces; ++i )
        {
            const MCE::FBX::Surface& source_surface = source_surfaces[ i ];
            Adreno::MeshSurface&     dest_surface   = dest_surfaces.Surfaces[ i ];

            dest_surface.StartIndex   = source_surface.StartIndex;
            dest_surface.NumTriangles = source_surface.NumTriangles;
            dest_surface.MaterialId   = source_surface.MaterialId;
        }
    }

    //-----------------------------------------------------------------------------

    void ConvertMesh( const MCE::FBX::Mesh& source_mesh, Adreno::Mesh& dest_mesh, const MCE::FBX::Scene& source_scene, const MCE::FBX::ConstSceneNodeArray& source_nodes )
    {
        const MCE::FBX::VertexArray&   source_vertices  = source_mesh.GetVertices();
        const MCE::FBX::TriangleArray& source_triangles = source_mesh.GetTriangles();
        const MCE::FBX::SurfaceArray&  source_surfaces  = source_mesh.GetSurfaces();
        
        Adreno::VertexBuffer&     dest_vertex_buffer = dest_mesh.Vertices;
        Adreno::IndexBuffer&      dest_index_buffer  = dest_mesh.Indices;
        Adreno::MeshSurfaceArray& dest_surfaces      = dest_mesh.Surfaces;

        ConvertVertices( source_vertices, dest_vertex_buffer, source_scene, source_nodes );
        ConvertIndices( source_triangles, dest_index_buffer );
        ConvertSurfaces( source_surfaces, dest_surfaces );

        const MCE::FBX::SceneNode* source_node = source_mesh.GetNode();
        
        if( source_node )
        {
            dest_mesh.JointIndex = FindJointIndex( source_nodes, source_node );
        }
    }

    //-----------------------------------------------------------------------------

    void ConvertJoint( const MCE::FBX::ConstSceneNodeArray& source_nodes, const MCE::FBX::SceneNode& source_node, Adreno::Joint& dest_joint )
    {
        const char* name = source_node.GetName();
        dest_joint.SetName( name );

        const MCE::FBX::SceneNode* parent_node = source_node.GetParentNode();

        if( parent_node )
        {
            dest_joint.ParentIndex = FindJointIndex( source_nodes, parent_node );
        }
        else
        {
            dest_joint.ParentIndex = Adreno::Joint::NULL_NODE_INDEX;
        }

        const MCE::FBX::Transform& source_transform = source_node.GetTransform();
        Adreno::Transform&         dest_transform   = dest_joint.Transform;
        
        ConvertTransform( source_transform, dest_transform );

        const MCE::FBX::Transform& source_inv_bind_pose = source_node.GetInverseBindPose();
        Adreno::Transform&         dest_inv_bind_pose   = dest_joint.InverseBindPose;

        ConvertTransform( source_inv_bind_pose, dest_inv_bind_pose );
    }

    //-----------------------------------------------------------------------------

    void ConvertMaterial( const MCE::FBX::Material& source_material, Adreno::Material& dest_material )
    {
        const char* material_name = source_material.Name.c_str();
        dest_material.SetName( material_name );
    }

    //-----------------------------------------------------------------------------

    Adreno::Model* ConvertModel( const MCE::FBX::Scene& source_scene )
    {
        Adreno::Model* dest_model = new Adreno::Model;

        MCE::FBX::ConstSceneNodeArray source_nodes;
        CollectSceneNodes( source_scene, source_nodes );
        SortNodesByDepth( source_nodes );

        // Materials
        int num_materials = source_scene.NumMaterials();
        dest_model->ResizeMaterials( num_materials );

        for( int i = 0; i < num_materials; ++i )
        {
            const MCE::FBX::Material& source_material = source_scene.GetMaterial( i );
            Adreno::Material&         dest_material   = dest_model->Materials[ i ];
            
            ConvertMaterial( source_material, dest_material );
        }

        // Joints
        int num_joints = (int)( source_nodes.size() );
        dest_model->ResizeJoints( num_joints );

        for( int i = 0; i < num_joints; ++i )
        {
            const MCE::FBX::SceneNode* source_node = source_nodes[ i ];
            Adreno::Joint&             dest_joint  = dest_model->Joints[ i ];

            if( source_node )
            {
                ConvertJoint( source_nodes, *source_node, dest_joint );
            }
        }

        // Meshes
        MCE::FBX::ConstMeshArray source_meshes;
        CollectSceneMeshes( source_scene, source_meshes );

        int num_meshes = (int)( source_meshes.size() );
        dest_model->ResizeMeshes( num_meshes );
        
        for( int i = 0; i < num_meshes; ++i )
        {
            const MCE::FBX::Mesh* source_mesh = source_meshes[ i ];
            Adreno::Mesh&         dest_mesh   = dest_model->Meshes[ i ];

            if( source_mesh )
            {
                ConvertMesh( *source_mesh, dest_mesh, source_scene, source_nodes );
            }
        }

        return dest_model;
    }

    //-----------------------------------------------------------------------------

    void ConvertAnimationTrack( const MCE::FBX::SceneNode& source_node, Adreno::AnimationTrack& dest_anim_track )
    {
        const char* track_name = source_node.GetName();
        dest_anim_track.SetName( track_name );

        const MCE::FBX::Vector3Animation&    position_anim = source_node.GetPositionAnimation();
        const MCE::FBX::QuaternionAnimation& rotation_anim = source_node.GetRotationAnimation();
        const MCE::FBX::Vector3Animation&    scale_anim    = source_node.GetScaleAnimation();

        int num_keyframes = position_anim.NumKeyFrames(); // All channels should have the same number of frames
        dest_anim_track.ResizeKeyframes( num_keyframes );
        
        for( int k = 0; k < num_keyframes; ++k )
        {
            const MCE::FBX::Vector3&    position = position_anim.GetKeyframe( k );
            const MCE::FBX::Quaternion& rotation = rotation_anim.GetKeyframe( k );
            const MCE::FBX::Vector3&    scale    = scale_anim.GetKeyframe( k );
            
            Adreno::Transform& keyframe = dest_anim_track.Keyframes[ k ];

            ConvertVector3( position, keyframe.Position );
            ConvertQuaternion( rotation, keyframe.Rotation );
            ConvertVector3( scale, keyframe.Scale );
        }
    }

    //-----------------------------------------------------------------------------

    Adreno::Animation* ConvertAnimation( const MCE::FBX::Scene& source_scene )
    {
        Adreno::Animation* dest_anim = new Adreno::Animation;

        MCE::FBX::ConstSceneNodeArray source_nodes;
        CollectSceneNodes( source_scene, source_nodes );
        SortNodesByDepth( source_nodes );

        int num_joints = (int)( source_nodes.size() );
        dest_anim->ResizeTracks( num_joints );
        
        for( int i = 0; i < num_joints; ++i )
        {
            const MCE::FBX::SceneNode* source_node     = source_nodes[ i ];
            Adreno::AnimationTrack&    dest_anim_track = dest_anim->Tracks[ i ];

            if( source_node )
            {
                ConvertAnimationTrack( *source_node, dest_anim_track );
                
                if( dest_anim_track.NumKeyframes > dest_anim->NumFrames )
                {
                    dest_anim->NumFrames = dest_anim_track.NumKeyframes;
                }
            }
        }

        return dest_anim;
    }
}

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        SceneSerializer::SceneSerializer()
        {
        
        }

        //-----------------------------------------------------------------------------

        SceneSerializer::~SceneSerializer()
        {

        }

        //-----------------------------------------------------------------------------

        bool SceneSerializer::WriteModelToFile( const MCE::FBX::Scene& scene, const char* filename )
        {
            bool success = false;

            Adreno::Model* model = ConvertModel( scene );

            if( model )
            {
                if( Adreno::FrmWriteModelToFile( *model, filename ) )
                {
/*
                    Adreno::Model* test_model = Adreno::FrmLoadModelFromFile( filename );
                    
                    if( test_model )
                    {
                        Adreno::FrmDestroyLoadedModel( test_model );
                    }
*/
                    delete model;
                    model = NULL;

                    success = true;
                }
            }

            return success;
        }

        //-----------------------------------------------------------------------------

        bool SceneSerializer::WriteAnimationToFile( const MCE::FBX::Scene& scene, const char* filename )
        {
            bool success = false;

            Adreno::Animation* animation = ConvertAnimation( scene );

            if( animation )
            {
                if( Adreno::FrmWriteAnimationToFile( *animation, filename ) )
                {
/*
                    Adreno::Animation* test_animation = Adreno::FrmLoadAnimationFromFile( filename );
                    
                    if( test_animation )
                    {
                        Adreno::FrmDestroyLoadedAnimation( test_animation );
                    }
*/
                    delete animation;
                    animation = NULL;

                    success = true;
                }
            }

            return success;
        }
    }
}
