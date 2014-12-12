//--------------------------------------------------------------------------------------
// File: FrmExtendedMesh.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#include <GLES2/gl2.h>
#include "FrmBasicTypes.h"
#include "FrmMath.h"

namespace MSH1_FORMAT
{
	struct FRM_MESH_FRAME;
}



//--------------------------------------------------------------------------------------
// Name: class CFrmExtendedMesh
// Desc: 
//--------------------------------------------------------------------------------------
class CFrmExtendedMesh
{
public:
    MSH1_FORMAT::FRM_MESH_FRAME* m_pFrames;
    UINT32          m_nNumFrames;

public:
    CFrmExtendedMesh();
    ~CFrmExtendedMesh();

    BOOL LoadInputMesh( const CHAR* strFileName );
    BOOL SaveOutputMesh( const CHAR* strFileName );

    VOID ComputeBounds( FRMVECTOR3& min, FRMVECTOR3& max );
    VOID Rotate( FLOAT32 fAngle, FRMVECTOR3 vAxis );
    VOID OffsetAndScale( FRMVECTOR3 vOffset, FLOAT32 fScale );
    VOID ReverseHandedness();
    VOID AddNormals();
    VOID AddTangentSpace();
    BOOL ReverseWinding();
    VOID Stripify();
};

