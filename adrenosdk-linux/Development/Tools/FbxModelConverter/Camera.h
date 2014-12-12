//-----------------------------------------------------------------------------
// 
//  Camera
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_CAMERA_H
#define MCE_FBX_CAMERA_H

#include "SceneObject.h"
#include "Transform.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Camera : public SceneObject
        {
            public:
                         Camera();
                virtual ~Camera();

                bool    IsOrthographic      () const;
                void    SetOrthographic     ( bool is_ortho );

                float   GetFOV              () const;
                void    SetFOV              ( float fov );

                float   GetAspectRatio      () const;
                void    SetAspectRatio      ( float aspect_ratio );

                float   GetNearPlane        () const;
                void    SetNearPlane        ( float near_plane );

                float   GetFarPlane         () const;
                void    SetFarPlane         ( float far_plane );

                float   GetLeftPlane        () const;
                void    SetLeftPlane        ( float left_plane );

                float   GetRightPlane       () const;
                void    SetRightPlane       ( float right_plane );

                float   GetTopPlane         () const;
                void    SetTopPlane         ( float top_plane );

                float   GetBottomPlane      () const;
                void    SetBottomPlane      ( float bottom_plane );

            private:
                bool  m_is_ortho;

                float m_fov;
                float m_aspect_ratio;
                float m_near_plane;
                float m_far_plane;

                float m_left_plane;
                float m_right_plane;
                float m_top_plane;
                float m_bottom_plane;
        };

        typedef std::vector< Camera* > CameraArray;
    }
}

//-----------------------------------------------------------------------------

#endif