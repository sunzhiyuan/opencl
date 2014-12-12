//-----------------------------------------------------------------------------
// 
//  ShaderParameter
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "ShaderParameter.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        ShaderParameter::ShaderParameter()
        : m_name()
        , m_type( MCE::FBX::ShaderParameterType::INVALID )
        {

        }

        //-----------------------------------------------------------------------------

        ShaderParameter::ShaderParameter( MCE::FBX::ShaderParameterType::ShaderParameterTypes type )
        : m_name()
        , m_type( type )
        {

        }

        //-----------------------------------------------------------------------------

        ShaderParameter::ShaderParameter( const char* name, MCE::FBX::ShaderParameterType::ShaderParameterTypes type )
        : m_name( name )
        , m_type( type )
        {

        }

        //-----------------------------------------------------------------------------

        ShaderParameter::~ShaderParameter()
        {

        }

        //-----------------------------------------------------------------------------

        ShaderParameterType::ShaderParameterTypes ShaderParameter::GetType() const
        {
            return m_type;
        }

        //-----------------------------------------------------------------------------

        const char* ShaderParameter::GetName() const
        {
            return m_name.c_str();
        }

        //-----------------------------------------------------------------------------

        void ShaderParameter::SetName( const char* name )
        {
            m_name = name;
        }
    }
}
