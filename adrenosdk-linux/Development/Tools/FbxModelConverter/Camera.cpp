//-----------------------------------------------------------------------------
// 
//  Camera
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Camera.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Camera::Camera()
        : MCE::FBX::SceneObject( MCE::FBX::SceneObjectType::CAMERA )
        , m_is_ortho( false )
        , m_fov( 0.0f )
        , m_aspect_ratio( 0.0f )
        , m_near_plane( 0.0f )
        , m_far_plane( 0.0f )
        , m_left_plane( 0.0f )
        , m_right_plane( 0.0f )
        , m_top_plane( 0.0f )
        , m_bottom_plane( 0.0f )
        {

        }

        //-----------------------------------------------------------------------------

        Camera::~Camera()
        {

        }

        //-----------------------------------------------------------------------------

        bool Camera::IsOrthographic() const
        {
            return m_is_ortho;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetOrthographic( bool is_ortho )
        {
            m_is_ortho = is_ortho;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetFOV() const
        {
            return m_fov;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetFOV( float fov )
        {
            m_fov = fov;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetAspectRatio() const
        {
            return m_aspect_ratio;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetAspectRatio( float aspect_ratio )
        {
            m_aspect_ratio = aspect_ratio;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetNearPlane() const
        {
            return m_near_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetNearPlane( float near_plane )
        {
            m_near_plane = near_plane;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetFarPlane() const
        {
            return m_far_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetFarPlane( float far_plane )
        {
            m_far_plane = far_plane;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetLeftPlane() const
        {
            return m_left_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetLeftPlane( float left_plane )
        {
            m_left_plane = left_plane;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetRightPlane() const
        {
            return m_right_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetRightPlane( float right_plane )
        {
            m_right_plane = right_plane;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetTopPlane() const
        {
            return m_top_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetTopPlane( float top_plane )
        {
            m_top_plane = top_plane;
        }

        //-----------------------------------------------------------------------------

        float Camera::GetBottomPlane() const
        {
            return m_bottom_plane;
        }

        //-----------------------------------------------------------------------------

        void Camera::SetBottomPlane( float bottom_plane )
        {
            m_bottom_plane = bottom_plane;
        }
    }
}
