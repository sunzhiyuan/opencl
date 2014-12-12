//-----------------------------------------------------------------------------
// 
//  VertexArray
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "VertexArray.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        VertexArray::VertexArray()
        : m_num_verts( 0 )
        , m_format()
        , m_positions()
        , m_normals()
        , m_binormals()
        , m_tangents()
        , m_skin_weights()
        , m_color_channels()
        , m_uv_channels()
        {

        }

        //-----------------------------------------------------------------------------

        VertexArray::~VertexArray()
        {

        }

        //-----------------------------------------------------------------------------

        void VertexArray::Initialize( int num_verts, const VertexFormat& format )
        {
            Reset();

            m_num_verts = num_verts;
            m_format    = format;

            if( format.HasPosition() )
            {
                m_positions.resize( m_num_verts );
            }

            if( format.HasNormal() )
            {
                m_normals.resize( m_num_verts );
            }

            if( format.HasBinormal() )
            {
                m_binormals.resize( m_num_verts );
            }

            if( format.HasTangent() )
            {
                m_tangents.resize( m_num_verts );
            }

            if( format.HasSkinWeights() )
            {
                m_skin_weights.resize( m_num_verts );

                int num_weights_per_vertex = format.NumSkinWeights();

                for( int i = 0; i < m_num_verts; ++i )
                {
                    SkinWeightArray& vertex_weights = m_skin_weights[ i ];
                    vertex_weights.resize( num_weights_per_vertex );
                }
            }

            if( format.HasColors() )
            {
                int num_channels = format.NumColors();
                m_color_channels.resize( num_channels );

                for( int i = 0; i < num_channels; ++i )
                {
                    ColorArray& channel = m_color_channels[ i ];
                    channel.resize( m_num_verts );
                }
            }

            if( format.HasUVs() )
            {
                int num_channels = format.NumUVs();
                m_uv_channels.resize( num_channels );

                for( int i = 0; i < num_channels; ++i )
                {
                    Vector2Array& channel = m_uv_channels[ i ];
                    channel.resize( m_num_verts );
                }
            }
        }

        //-----------------------------------------------------------------------------

        void VertexArray::Reset()
        {
            m_num_verts = 0;
            m_format    = VertexFormat();

            m_positions.clear();
            m_normals.clear();
            m_binormals.clear();
            m_tangents.clear();
            m_color_channels.clear();
            m_uv_channels.clear();
        }

        //-----------------------------------------------------------------------------

        int VertexArray::NumVerts() const
        {
            return m_num_verts;
        }

        //-----------------------------------------------------------------------------

        const VertexFormat& VertexArray::GetVertexFormat() const
        {
            return m_format;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector3& VertexArray::GetPosition( int index ) const
        {
            FBX_ASSERT( m_format.HasPosition(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            return m_positions[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetPosition( int index, const MCE::FBX::Vector3& value )
        {
            FBX_ASSERT( m_format.HasPosition(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            m_positions[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector3& VertexArray::GetNormal( int index ) const
        {
            FBX_ASSERT( m_format.HasNormal(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            return m_normals[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetNormal( int index, const MCE::FBX::Vector3& value )
        {
            FBX_ASSERT( m_format.HasNormal(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            m_normals[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector3& VertexArray::GetBinormal( int index ) const
        {
            FBX_ASSERT( m_format.HasBinormal(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            return m_binormals[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetBinormal( int index, const MCE::FBX::Vector3& value )
        {
            FBX_ASSERT( m_format.HasBinormal(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            m_binormals[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector3& VertexArray::GetTangent( int index ) const
        {
            FBX_ASSERT( m_format.HasTangent(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            return m_tangents[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetTangent( int index, const MCE::FBX::Vector3& value )
        {
            FBX_ASSERT( m_format.HasTangent(), "Invalid vertex property" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            m_tangents[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::SkinWeight& VertexArray::GetSkinWeight( int index, int channel ) const
        {
            FBX_ASSERT( m_format.HasSkinWeights(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_skin_weights.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            const MCE::FBX::SkinWeightArray& vertex_weights = m_skin_weights[ index ];
            return vertex_weights[ channel ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetSkinWeight( int index, int channel, const MCE::FBX::SkinWeight& value )
        {
            FBX_ASSERT( m_format.HasSkinWeights(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_skin_weights.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            MCE::FBX::SkinWeightArray& vertex_weights = m_skin_weights[ index ];
            vertex_weights[ channel ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Color& VertexArray::GetColor( int index, int channel ) const
        {
            FBX_ASSERT( m_format.HasColors(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_color_channels.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            const MCE::FBX::ColorArray& color_channel = m_color_channels[ channel ];
            return color_channel[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetColor( int index, int channel, const MCE::FBX::Color& value )
        {
            FBX_ASSERT( m_format.HasColors(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_color_channels.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            MCE::FBX::ColorArray& color_channel = m_color_channels[ channel ];
            color_channel[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector2& VertexArray::GetUV( int index, int channel ) const
        {
            FBX_ASSERT( m_format.HasUVs(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_uv_channels.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            const MCE::FBX::Vector2Array& uv_channel = m_uv_channels[ channel ];
            return uv_channel[ index ];
        }

        //-----------------------------------------------------------------------------

        void VertexArray::SetUV( int index, int channel, const MCE::FBX::Vector2& value )
        {
            FBX_ASSERT( m_format.HasUVs(), "Invalid vertex property" );
            FBX_ASSERT( ( channel >= 0 ) && ( channel < (int)m_uv_channels.size() ), "Channel index out of range" );
            FBX_ASSERT( ( index >= 0 ) && ( index < m_num_verts ), "Vertex index out of range" );

            MCE::FBX::Vector2Array& uv_channel = m_uv_channels[ channel ];
            uv_channel[ index ] = value;
        }
    }
}
