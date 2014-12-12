//-----------------------------------------------------------------------------
// 
//  ShaderParameter
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SHADER_PARAMETER_H
#define MCE_FBX_SHADER_PARAMETER_H

#include "ShaderParameterTypes.h"
#include <string>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class ShaderParameter
        {
            public:
                         ShaderParameter();
                virtual ~ShaderParameter();

                const char*                                 GetName() const;
                void                                        SetName( const char* name );

                ShaderParameterType::ShaderParameterTypes   GetType() const;

            protected:
                ShaderParameter( MCE::FBX::ShaderParameterType::ShaderParameterTypes type );
                ShaderParameter( const char* name, MCE::FBX::ShaderParameterType::ShaderParameterTypes type );

            private:
                std::string m_name;
                ShaderParameterType::ShaderParameterTypes m_type;
        };

        typedef std::vector< ShaderParameter* > ShaderParameterArray;
    }
}

//-----------------------------------------------------------------------------

#endif