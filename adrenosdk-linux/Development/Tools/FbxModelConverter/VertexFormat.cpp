//-----------------------------------------------------------------------------
// 
//  VertexFormat
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "VertexFormat.h"
#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"

#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        bool ignore_skin_properties = false;

        //-----------------------------------------------------------------------------

        VertexFormat::VertexFormat()
        : m_properties()
        , m_has_position( false )
        , m_has_normal( false )
        , m_has_binormal( false )
        , m_has_tangent( false )
        , m_has_skin_indices( false )
        , m_has_skin_weights( false )
        , m_has_colors( false )
        , m_has_uvs( false )
        , m_num_skin_indices( 0 )
        , m_num_skin_weights( 0 )
        , m_num_colors( 0 )
        , m_num_uvs( 0 )
        {

        }

        //-----------------------------------------------------------------------------

        VertexFormat::~VertexFormat()
        {

        }

        //-----------------------------------------------------------------------------

        void VertexFormat::AddProperty( int usage, int num_values )
        {
            static MCE::FBX::VertexProperty color_property      ( MCE::FBX::VertexPropertyUsage::INVALID, MCE::FBX::VertexPropertyType::UINT8_TYPE,   sizeof( uint8  ), 4, true,  0 );
            static MCE::FBX::VertexProperty skin_index_property ( MCE::FBX::VertexPropertyUsage::INVALID, MCE::FBX::VertexPropertyType::INT8_TYPE,    sizeof( uint32 ), 4, false, 0 );
            static MCE::FBX::VertexProperty skin_weight_property( MCE::FBX::VertexPropertyUsage::INVALID, MCE::FBX::VertexPropertyType::FLOAT32_TYPE, sizeof( float  ), 4, false, 0 );
            static MCE::FBX::VertexProperty vec2_property       ( MCE::FBX::VertexPropertyUsage::INVALID, MCE::FBX::VertexPropertyType::FLOAT32_TYPE, sizeof( float  ), 2, false, 0 );
            static MCE::FBX::VertexProperty vec3_property       ( MCE::FBX::VertexPropertyUsage::INVALID, MCE::FBX::VertexPropertyType::FLOAT32_TYPE, sizeof( float  ), 3, false, 0 );

            MCE::FBX::VertexProperty vertex_property;

            switch( usage )
            {
                case MCE::FBX::VertexPropertyUsage::POSITION:
                {
                    FBX_ASSERT( !m_has_position, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_position = true;

                    vertex_property = vec3_property;
                    vertex_property.Usage = usage;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::NORMAL:
                {
                    FBX_ASSERT( !m_has_normal, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_normal = true;

                    vertex_property = vec3_property;
                    vertex_property.Usage = usage;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::BINORMAL:
                {
                    FBX_ASSERT( !m_has_binormal, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_binormal = true;

                    vertex_property = vec3_property;
                    vertex_property.Usage = usage;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::TANGENT:
                {
                    FBX_ASSERT( !m_has_tangent, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_tangent = true;

                    vertex_property = vec3_property;
                    vertex_property.Usage = usage;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::SKIN_INDEX:
                {
                    FBX_ASSERT( !m_has_skin_indices, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_skin_indices = true;

                    vertex_property = skin_index_property;
                    vertex_property.Usage = usage;
                    vertex_property.NumValues = num_values;

                    m_num_skin_indices = num_values;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::SKIN_WEIGHT:
                {
                    FBX_ASSERT( !m_has_skin_weights, "VertexFormat::AddProperty: More than one channel not allowed" );
                    m_has_skin_weights = true;

                    vertex_property = skin_weight_property;
                    vertex_property.Usage = usage;
                    vertex_property.NumValues = num_values;

                    m_num_skin_weights = num_values;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::COLOR:
                {
                    m_has_colors = true;

                    vertex_property = color_property;
                    vertex_property.Usage = usage;
                    vertex_property.Channel = m_num_colors;
                    ++m_num_colors;

                    break;
                }

                case MCE::FBX::VertexPropertyUsage::UV:
                {
                    m_has_uvs = true;

                    vertex_property = vec2_property;
                    vertex_property.Usage = usage;
                    vertex_property.Channel = m_num_uvs;
                    ++m_num_uvs;

                    break;
                }

                default:
                {
                    FBX_ASSERT_FALSE( "VertexFormat::AddProperty:  Unknown vertex property usage!" );
                }
            }

            m_properties.push_back( vertex_property );
        }

        //-----------------------------------------------------------------------------

        int VertexFormat::NumProperties() const
        {
            int num_properties = (int)( m_properties.size() );

            if( ignore_skin_properties )
            {
                if( m_has_skin_indices ) { --num_properties; }
                if( m_has_skin_weights ) { --num_properties; }
            }

            return num_properties;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::VertexProperty& VertexFormat::GetProperty( int index ) const
        {
            if( ignore_skin_properties )
            {
                while( m_properties[ index ].Usage == MCE::FBX::VertexPropertyUsage::SKIN_INDEX || m_properties[ index ].Usage == MCE::FBX::VertexPropertyUsage::SKIN_WEIGHT )
                {
                    ++index;
                }
            }

            const MCE::FBX::VertexProperty& vertex_property = m_properties[ index ];
            return vertex_property;
        }

        //-----------------------------------------------------------------------------

        uint32 VertexFormat::VertexStride() const
        {
            uint32 stride = 0;

            int num_properties = NumProperties();

            for( int i = 0; i < num_properties; ++i )
            {
                const MCE::FBX::VertexProperty& vertex_property = GetProperty( i );
                stride += vertex_property.NumValues * vertex_property.ValueSize;
            }

            return stride;
        }
    }
}
