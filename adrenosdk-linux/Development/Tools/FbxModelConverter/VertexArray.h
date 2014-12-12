//-----------------------------------------------------------------------------
// 
//  VertexArray
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VERTEX_ARRAY_H
#define MCE_FBX_VERTEX_ARRAY_H

#include "Color.h"
#include "SkinWeight.h"
#include "Vector2.h"
#include "Vector3.h"
#include "VertexFormat.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class VertexArray
        {
            public:
                 VertexArray();
                ~VertexArray();

                void                        Initialize      ( int num_verts, const VertexFormat& format );
                void                        Reset           ();

                int                         NumVerts        () const;
                const VertexFormat&         GetVertexFormat () const;

                const MCE::FBX::Vector3&    GetPosition     ( int index ) const;
                void                        SetPosition     ( int index, const MCE::FBX::Vector3& value );

                const MCE::FBX::Vector3&    GetNormal       ( int index ) const;
                void                        SetNormal       ( int index, const MCE::FBX::Vector3& value );

                const MCE::FBX::Vector3&    GetBinormal     ( int index ) const;
                void                        SetBinormal     ( int index, const MCE::FBX::Vector3& value );

                const MCE::FBX::Vector3&    GetTangent      ( int index ) const;
                void                        SetTangent      ( int index, const MCE::FBX::Vector3& value );

                const MCE::FBX::SkinWeight& GetSkinWeight   ( int index, int channel ) const;
                void                        SetSkinWeight   ( int index, int channel, const MCE::FBX::SkinWeight& value );

                const MCE::FBX::Color&      GetColor        ( int index, int channel ) const;
                void                        SetColor        ( int index, int channel, const MCE::FBX::Color& value );

                const MCE::FBX::Vector2&    GetUV           ( int index, int channel ) const;
                void                        SetUV           ( int index, int channel, const MCE::FBX::Vector2& value );

            private:
                typedef std::vector< ColorArray >       ColorChannelArray;
                typedef std::vector< SkinWeightArray >  VertexWeightsArray;
                typedef std::vector< Vector2Array >     UVChannelArray;

                int                m_num_verts;
                VertexFormat       m_format;

                Vector3Array       m_positions;
                Vector3Array       m_normals;
                Vector3Array       m_binormals;
                Vector3Array       m_tangents;
                VertexWeightsArray m_skin_weights;
                ColorChannelArray  m_color_channels;
                UVChannelArray     m_uv_channels;
        };
    }
}

//-----------------------------------------------------------------------------

#endif