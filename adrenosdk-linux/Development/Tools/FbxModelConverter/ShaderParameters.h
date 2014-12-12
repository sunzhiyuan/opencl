//-----------------------------------------------------------------------------
// 
//  ShaderParameters
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SHADER_PARAMETERS_H
#define MCE_FBX_SHADER_PARAMETERS_H

#include "ShaderParameter.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <string>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class BoolShaderParameter : public ShaderParameter
        {
            public:
                bool Value;

                BoolShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::BOOL ), Value( false ) { }
                BoolShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::BOOL ), Value( false ) { }
        };

        class IntShaderParameter : public ShaderParameter
        {
            public:
                int Value;

                IntShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::INTEGER ), Value( 0 ) { }
                IntShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::INTEGER ), Value( 0 ) { }
        };

        class FloatShaderParameter : public ShaderParameter
        {
            public:
                float Value;

                FloatShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::FLOAT ), Value( 0.0f ) { }
                FloatShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::FLOAT ), Value( 0.0f ) { }
        };

        class Vector2ShaderParameter : public ShaderParameter
        {
            public:
                MCE::FBX::Vector2 Value;

                Vector2ShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::VECTOR2 ), Value( 0.0f, 0.0f ) { }
                Vector2ShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::VECTOR2 ), Value( 0.0f, 0.0f ) { }
        };

        class Vector3ShaderParameter : public ShaderParameter
        {
            public:
                MCE::FBX::Vector3 Value;

                Vector3ShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::VECTOR3 ), Value( 0.0f, 0.0f, 0.0f ) { }
                Vector3ShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::VECTOR3 ), Value( 0.0f, 0.0f, 0.0f ) { }
        };

        class Vector4ShaderParameter : public ShaderParameter
        {
            public:
                MCE::FBX::Vector4 Value;

                Vector4ShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::VECTOR4 ), Value( 0.0f, 0.0f, 0.0f, 0.0f ) { }
                Vector4ShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::VECTOR4 ), Value( 0.0f, 0.0f, 0.0f, 0.0f ) { }
        };

        class Color3ShaderParameter : public ShaderParameter
        {
            public:
                MCE::FBX::Vector3 Value;

                Color3ShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::COLOR3 ), Value( 0.0f, 0.0f, 0.0f ) { }
                Color3ShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::COLOR3 ), Value( 0.0f, 0.0f, 0.0f ) { }
        };

        class Color4ShaderParameter : public ShaderParameter
        {
            public:
                MCE::FBX::Vector4 Value;

                Color4ShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::COLOR4 ), Value( 0.0f, 0.0f, 0.0f, 0.0f ) { }
                Color4ShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::COLOR4 ), Value( 0.0f, 0.0f, 0.0f, 0.0f ) { }
        };

        class StringShaderParameter : public ShaderParameter
        {
            public:
                std::string Value;

                StringShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::STRING ), Value( "" ) { }
                StringShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::STRING ), Value( "" ) { }
        };

        class TextureShaderParameter : public ShaderParameter
        {
            public:
                std::string Value;

                TextureShaderParameter()                   : ShaderParameter( "",   MCE::FBX::ShaderParameterType::TEXTURE ), Value( "" ) { }
                TextureShaderParameter( const char* name ) : ShaderParameter( name, MCE::FBX::ShaderParameterType::TEXTURE ), Value( "" ) { }
        };
    }
}

//-----------------------------------------------------------------------------

#endif