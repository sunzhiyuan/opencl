//-----------------------------------------------------------------------------
// 
//  FilmboxNode
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_NODE_H
#define MCE_FILMBOX_NODE_H

#include "Quaternion.h"
#include "SceneObjectType.h"
#include "TimeRange.h"
#include "Vector3.h"
#include <fbxsdk.h>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class FilmboxNode
        {
            public:
                 FilmboxNode( KFbxNode* node );
                ~FilmboxNode();
        
                KFbxNode*                   GetNode                 ();
                const KFbxNode*             GetNode                 () const;
        
                int                         NumChildren             () const;

                KFbxNode*                   GetChildNode            ( int index );
                const KFbxNode*             GetChildNode            ( int index ) const;

                const char*                 GetName                 () const;

                KFbxXMatrix                 GetParentWorldTransform () const;
                KFbxXMatrix                 GetParentWorldTransform ( int frame ) const;

                KFbxXMatrix                 GetWorldTransform       () const;
                KFbxXMatrix                 GetWorldTransform       ( int frame ) const;

                KFbxXMatrix                 GetLocalTransform       () const;
                KFbxXMatrix                 GetLocalTransform       ( int frame ) const;

                MCE::FBX::Vector3           GetLocalTranslation     () const;
                MCE::FBX::Vector3           GetLocalRotation        () const;
                MCE::FBX::Vector3           GetLocalScale           () const;

                KFbxXMatrix                 GetGeometricTransform   () const;
                MCE::FBX::Vector3           GetGeometricTranslation () const;
                MCE::FBX::Vector3           GetGeometricRotation    () const;
                MCE::FBX::Vector3           GetGeometricScale       () const;

                MCE::FBX::TimeRange         GetAnimationTimeRange   () const;

                bool                        IsBone                  () const;
                bool                        IsCamera                () const;
                bool                        IsMesh                  () const;

                KFbxCamera*                 GetCamera               ();
                const KFbxCamera*           GetCamera               () const;

                KFbxMesh*                   GetMesh                 ();
                const KFbxMesh*             GetMesh                 () const;

                MCE::FBX::SceneObjectType::SceneObjectTypes GetType () const;

            private:
                KFbxCamera*                                 GetFbxCamera    () const;
                KFbxMesh*                                   GetFbxMesh      () const;
                KFbxNodeAttribute::EAttributeType           GetFbxType      () const;
                MCE::FBX::SceneObjectType::SceneObjectTypes GetMceType      () const;

            private:
                KFbxNode* m_node;
                MCE::FBX::SceneObjectType::SceneObjectTypes m_type;
        };
    }
}

//-----------------------------------------------------------------------------

#endif