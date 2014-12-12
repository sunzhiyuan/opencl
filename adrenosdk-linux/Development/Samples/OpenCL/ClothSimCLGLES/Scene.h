//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

#include "ClothSimCL.h"

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeGraphicsApplication
{
public:
    CSample( const CHAR* strName );

    // CFrmApplication
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();

    BOOL                  m_bUseOpenCL;
    CClothSimCL           m_ClothSim;
    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    CFrmArcBall        m_ArcBall;
    FRMVECTOR4         m_qMeshRotation;
    FRMVECTOR4         m_qRotationDelta;

    FRMVECTOR3         m_vLightPosition;

    GLuint             m_hTextureShader;
    GLuint             m_hClothShader;
    GLint              m_hDiffuseMapLoc;
    GLint              m_hNormalMapLoc;
    GLint              m_hModelMatrixLoc;
    GLint              m_hModelViewMatrixLoc;
    GLint              m_hModelViewProjMatrixLoc;
    GLint              m_hNormalMatrixLoc;
    GLint              m_hLightPositionLoc;
    GLint              m_hMaterialAmbientLoc;
    GLint              m_hMaterialDiffuseLoc;
    GLint              m_hMaterialSpecularLoc;

    CFrmTexture*       m_pLogoTexture;
    CFrmTexture*       m_pDiffuseTexture;
    CFrmTexture*       m_pNormalTexture;

};


#endif // SCENE_H
