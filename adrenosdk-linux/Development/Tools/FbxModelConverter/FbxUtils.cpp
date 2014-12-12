//-----------------------------------------------------------------------------
// 
//  FbxUtils
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FbxUtils.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 MCE::FBX::ConvertVector3( KFbxVector4 vec )
        {
            return MCE::FBX::Vector3( vec[ 0 ], vec[ 1 ], vec[ 2 ] );
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Quaternion MCE::FBX::ConvertQuaternion( KFbxQuaternion quat )
        {
            return MCE::FBX::Quaternion( quat[ 0 ], quat[ 1 ], quat[ 2 ], -quat[ 3 ] );
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 MCE::FBX::ExtractTranslation( KFbxXMatrix transform )
        {
            KFbxVector4       fbx_translation = transform.GetT();
            MCE::FBX::Vector3 translation     = ConvertVector3( fbx_translation );

            return translation;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Quaternion MCE::FBX::ExtractRotation( KFbxXMatrix transform )
        {
            KFbxQuaternion       fbx_rotation = transform.GetQ();
            MCE::FBX::Quaternion rotation     = ConvertQuaternion( fbx_rotation );

            return rotation;
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3 MCE::FBX::ExtractScale( KFbxXMatrix transform )
        {
            KFbxVector4       fbx_scale = transform.GetS();
            MCE::FBX::Vector3 scale     = ConvertVector3( fbx_scale );

            return scale;
        }
    }
}
