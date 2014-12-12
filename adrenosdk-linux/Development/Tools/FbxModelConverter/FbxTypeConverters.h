//-----------------------------------------------------------------------------
// 
//  FbxTypeConverters
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_TYPE_CONVERTERS_H
#define MCE_FBX_TYPE_CONVERTERS_H

#include "FbxTypes.h"

//-----------------------------------------------------------------------------

template< class FbxDataArrayType >
class FbxToMceVertexPropertyConverter
{
    public:
        virtual void ConvertValue( const FbxDataArrayType& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel ) = 0;
};

//-----------------------------------------------------------------------------

class FbxToMceNormalConverter : public FbxToMceVertexPropertyConverter< FbxVector4Array >
{
    public:
        typedef KFbxLayerElementNormal LayerType;
        typedef FbxVector4Array        DataArrayType;

        virtual void ConvertValue( const FbxVector4Array& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel = 0 )
        {
            const KFbxVector4& fbx_normal = fbx_data_array[ fbx_data_index ];
            MCE::FBX::Vector3  mce_normal( fbx_normal[ 0 ], fbx_normal[ 1 ], fbx_normal[ 2 ] );

            vertex_buffer.SetNormal( mce_data_index, mce_normal );
        }
};

//-----------------------------------------------------------------------------

class FbxToMceBinormalConverter : public FbxToMceVertexPropertyConverter< FbxVector4Array >
{
    public:
        typedef KFbxLayerElementBinormal LayerType;
        typedef FbxVector4Array          DataArrayType;

        virtual void ConvertValue( const FbxVector4Array& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel = 0 )
        {
            const KFbxVector4& fbx_binormal = fbx_data_array[ fbx_data_index ];
            MCE::FBX::Vector3  mce_binormal( fbx_binormal[ 0 ], fbx_binormal[ 1 ], fbx_binormal[ 2 ] );

            vertex_buffer.SetBinormal( mce_data_index, mce_binormal );
        }
};

//-----------------------------------------------------------------------------

class FbxToMceTangentConverter : public FbxToMceVertexPropertyConverter< FbxVector4Array >
{
    public:
        typedef KFbxLayerElementTangent LayerType;
        typedef FbxVector4Array         DataArrayType;

        virtual void ConvertValue( const FbxVector4Array& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel = 0 )
        {
            const KFbxVector4& fbx_tangent = fbx_data_array[ fbx_data_index ];
            MCE::FBX::Vector3  mce_tangent( fbx_tangent[ 0 ], fbx_tangent[ 1 ], fbx_tangent[ 2 ] );

            vertex_buffer.SetTangent( mce_data_index, mce_tangent );
        }
};

//-----------------------------------------------------------------------------

class FbxToMceColorConverter : public FbxToMceVertexPropertyConverter< FbxColorArray >
{
    public:
        typedef KFbxLayerElementVertexColor LayerType;
        typedef FbxColorArray               DataArrayType;

        virtual void ConvertValue( const FbxColorArray& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel = 0 )
        {
            const KFbxColor& fbx_color = fbx_data_array[ fbx_data_index ];
            MCE::FBX::Color  mce_color( fbx_color.mRed, fbx_color.mGreen, fbx_color.mBlue, fbx_color.mAlpha );

            vertex_buffer.SetColor( mce_data_index, mce_channel, mce_color );
        }
};

//-----------------------------------------------------------------------------

class FbxToMceUVConverter : public FbxToMceVertexPropertyConverter< FbxVector2Array >
{
    public:
        typedef KFbxLayerElementUV LayerType;
        typedef FbxVector2Array    DataArrayType;

        virtual void ConvertValue( const FbxVector2Array& fbx_data_array, int fbx_data_index, MCE::FBX::VertexArray& vertex_buffer, int mce_data_index, int mce_channel = 0 )
        {
            const KFbxVector2& fbx_uv = fbx_data_array[ fbx_data_index ];
            MCE::FBX::Vector2  mce_uv( fbx_uv[ 0 ], fbx_uv[ 1 ] );

            vertex_buffer.SetUV( mce_data_index, mce_channel, mce_uv );
        }
};

//-----------------------------------------------------------------------------

#endif