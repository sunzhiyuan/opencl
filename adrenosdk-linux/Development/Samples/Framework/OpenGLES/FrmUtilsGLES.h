//--------------------------------------------------------------------------------------
// File: FrmUtilsGLES.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef _FRM_UTILS_GLES_H_
#define _FRM_UTILS_GLES_H_

#include "FrmPlatform.h"
#include "FrmUtils.h"
#include "FrmMath.h"
#include "OpenGLES/FrmResourceGLES.h"
#include "FrmStdLib.h"


//--------------------------------------------------------------------------------------
// Helper function to render a texture in screenspace
//--------------------------------------------------------------------------------------
class CFrmTexture; // Predeclaration
VOID FrmRenderTextureToScreen_GLES( FLOAT32 sx, FLOAT32 sy, FLOAT32 fScale, 
                                    CFrmTexture* pTexture, INT32 hShader, 
                                    INT32 hScreenSizeLoc );
VOID FrmRenderTextureToScreen_GLES( FLOAT32 sx, FLOAT32 sy, FLOAT32 w, FLOAT32 h,
                                    UINT32 hTexture, INT32 hShader, 
                                    INT32 hScreenSizeLoc );

//--------------------------------------------------------------------------------------
// Name: class CFrmShowMeshVectors
// Desc: Helper class to visualize mesh normals, tangents, and binormals
//--------------------------------------------------------------------------------------
class CFrmShowMeshVectors
{
public:
    CFrmShowMeshVectors()
    {
        m_hShowNormalsShader = 0;
        m_nNumVertices = 0;
        m_fExtrusions  = NULL;
        m_vPositions   = NULL;
        m_vNormals     = NULL;
        m_vTangents    = NULL;
        m_vBinormals   = NULL;
    }

    ~CFrmShowMeshVectors()
    {
        Destroy();
    }

    VOID Destroy();

    BOOL SetMesh( FRM_VERTEX_ELEMENT* pVertexLayout, UINT32 nNumVertices,
                  BYTE* pVertexData, UINT32 nVertexSize );
    VOID DrawMeshVectors( FRMMATRIX4X4 matMVP, FLOAT32 fLength );

protected:
    VOID DrawVectors( FRMVECTOR3* pVectors, FRMVECTOR3 vColor );

    UINT32      m_hShowNormalsShader;
    UINT32      m_nNumVertices;
    FLOAT32*    m_fExtrusions;
    FRMVECTOR3* m_vPositions;
    FRMVECTOR3* m_vNormals;
    FRMVECTOR3* m_vTangents;
    FRMVECTOR3* m_vBinormals;
};


#endif // _FRM_UTILS_GLES_H_
