//-----------------------------------------------------------------------------
// 
//  FilmboxManager
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_MANAGER_H
#define MCE_FILMBOX_MANAGER_H

#include "FilmboxSceneInfo.h"
#include "Scene.h"
#include <fbxsdk.h>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class FilmboxNode;

        class FilmboxManager
        {
            public:
                 FilmboxManager();
                ~FilmboxManager();
        
                void                                SetOptimizeMeshes               ( bool optimize_meshes );
                void                                SetWeldThreshold                ( float weld_threshold );

                KFbxScene*                          ImportScene                     ( const char* filename ) const;
                MCE::FBX::Scene*                    ConvertScene                    ( KFbxScene* fbx_scene );

            private:
                KFbxScene*                          CreateScene                     () const;
                void                                PreprocessScene                 ( KFbxScene& fbx_scene ) const;

                void                                ConvertCoordinateSystem         ( KFbxScene& fbx_scene ) const;

                void                                TriangulateGeometry             ( KFbxScene& fbx_scene ) const;
                void                                TriangulateGeometry             ( KFbxNode* root_fbx_node ) const;

                void                                BakePivotOffsetsIntoGeometry    ( KFbxScene& fbx_scene ) const;
                void                                BakePivotOffsetsIntoGeometry    ( KFbxNode* root_fbx_node ) const;

                void                                ExtractSceneInfo                ( KFbxScene& fbx_scene );
                void                                ExtractMaterialsInfo            ( KFbxScene& fbx_scene );
                void                                ExtractChildNodesInfo           ( KFbxNode& fbx_root_node );

                const MCE::FBX::FilmboxBone*        GetBoneInfo                     ( const KFbxNode* fbx_bone_node ) const;
                const MCE::FBX::FilmboxSkinInfo*    GetSkinInfo                     ( const KFbxMesh* fbx_mesh ) const;
                int                                 GetBoneIndex                    ( const KFbxNode* bone_node ) const;
                int                                 GetMaterialIndex                ( const KFbxSurfaceMaterial* fbx_material ) const;

                void                                ExtractMaterials                ( KFbxScene& fbx_scene, MCE::FBX::Scene& mce_scene ) const;

                void                                ExtractNodes                    ( KFbxScene& fbx_scene, MCE::FBX::Scene& mce_scene ) const;
                void                                ExtractChildNodes               ( KFbxNode* fbx_root_node, MCE::FBX::Scene& mce_scene, MCE::FBX::SceneNode& scene_root_node ) const;

                MCE::FBX::SceneNode*                ConvertNode                     ( KFbxNode* fbx_node, MCE::FBX::Scene& mce_scene ) const;

                void                                ConvertBoneNode                 ( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const;
                void                                ConvertCameraNode               ( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const;
                void                                ConvertMeshNode                 ( MCE::FBX::SceneNode& scene_node, MCE::FBX::FilmboxNode& mce_fbx_node ) const;

                MCE::FBX::Mesh*                     ConvertMesh                     ( const MCE::FBX::FilmboxNode& mesh_node ) const;
                MCE::FBX::Mesh*                     CreateCollisionMesh             ( const MCE::FBX::Mesh& visual_mesh ) const;

                void                                InitializeVertexArray           ( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::VertexArray& vertex_buffer ) const;
                void                                InitializeVertexFormat          ( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::VertexFormat& vertex_format ) const;
                void                                InitializeTriangleArray         ( const MCE::FBX::FilmboxNode& mesh_node, MCE::FBX::TriangleArray& triangles ) const;

                void                                InitializeSkinVertexProperties  ( MCE::FBX::VertexArray& vertex_buffer, int num_verts, const KFbxMesh* mesh ) const;

            private:
                KFbxSdkManager*             m_fbx_manager;
                KFbxGeometryConverter*      m_geometry_converter;
                MCE::FBX::FilmboxSceneInfo  m_fbx_scene_info;
                bool                        m_optimize_meshes;
                float                       m_weld_threshold;
        };

        //-----------------------------------------------------------------------------

        void                InitializeFilmboxManager();
        FilmboxManager*     GetFilmboxManager();
        void                ReleaseFilmboxManager();
    }
}

//-----------------------------------------------------------------------------

#endif