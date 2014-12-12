//-----------------------------------------------------------------------------
// 
//  FbxUtils
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_UTILS_H
#define MCE_FBX_UTILS_H

#include "Quaternion.h"
#include "Vector3.h"
#include <fbxsdk.h>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        MCE::FBX::Vector3       ConvertVector3      ( KFbxVector4 vec );
        MCE::FBX::Quaternion    ConvertQuaternion   ( KFbxQuaternion quat );

        MCE::FBX::Vector3       ExtractTranslation  ( KFbxXMatrix transform );
        MCE::FBX::Quaternion    ExtractRotation     ( KFbxXMatrix transform );
        MCE::FBX::Vector3       ExtractScale        ( KFbxXMatrix transform );
    }
}

//-----------------------------------------------------------------------------

#endif