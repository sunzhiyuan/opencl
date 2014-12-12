//-----------------------------------------------------------------------------
// 
//  FbxTypes
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_TYPES_H
#define MCE_FBX_TYPES_H

#include <fbxsdk.h>

//-----------------------------------------------------------------------------

typedef KFbxLayerElementArrayTemplate< int >                   FbxIndexArray;
typedef KFbxLayerElementMaterial                               FbxMaterialLayer;
typedef KFbxLayerElementNormal                                 FbxNormalLayer;
typedef KFbxLayerElementPolygonGroup                           FbxPolygonGroupLayer;
typedef KFbxLayerElementArrayTemplate< KFbxColor >             FbxColorArray;
typedef KFbxLayerElementArrayTemplate< KFbxVector2 >           FbxVector2Array;
typedef KFbxLayerElementArrayTemplate< KFbxVector4 >           FbxVector4Array;
typedef KFbxLayerElementArrayTemplate< KFbxSurfaceMaterial* >  FbxMaterialDataArray;
typedef KArrayTemplate< KFbxSurfaceMaterial* >                 FbxMaterialArray;

//-----------------------------------------------------------------------------

#endif