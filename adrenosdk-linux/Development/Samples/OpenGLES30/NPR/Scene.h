//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

#include "FrmPlatform.h"

enum
{
	CEL_SHADED_FBO = 0,
    NORMAL_AND_DEPTH_FBO,
    OUTLINE_FBO,
    NUM_FBOS 
};


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
    
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:    
    BOOL InitShaders();

    BOOL CheckFrameBufferStatus();
	BOOL CreateDrawBuffers();
	BOOL FreeDrawBuffers();


    VOID RenderScreenAlignedQuad();
    VOID RenderSceneCelShadedDepthAndNormalsToFBO();
    VOID RenderOutlineToFBO();
    VOID RenderOutline();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh;

    BOOL               m_bShouldRotate;
    FLOAT32            m_fRotateTime;

    // FBO related
    UINT32             m_hDepthbufferId;
    UINT32             m_nFBOTextureWidth;
    UINT32             m_nFBOTextureHeight;
    UINT32             m_pBufferIds[ NUM_FBOS ];
    UINT32             m_pTextureIds[ NUM_FBOS ];

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormalMatrix;

    // Shader related data
    CFrmTexture*       m_pLogoTexture;

    UINT32             m_hCelShadingShader;
    INT32              m_hCelNormalMatrixLoc;
    INT32              m_hCelModelViewProjMatrixLoc;
    INT32              m_hCelLightPosLoc;
    INT32              m_hCelTextureLoc;
    INT32              m_hCelShadingLoc;
    INT32              m_hCelAmbientLoc;
    CFrmTexture*       m_pCelShadingTexture;
    CFrmTexture*       m_pRayGunTexture;

    UINT32             m_hNormalDepthShader;
    INT32              m_hNDNormalMatrixLoc;
    INT32              m_hNDModelViewProjMatrixLoc;

    INT32              m_hOutlineHalfStepXSizeLoc;
    INT32              m_hOutlineHalfStepYSizeLoc;
    FLOAT32            m_fOutlineHalfStepSizeX;
    FLOAT32            m_fOutlineHalfStepSizeY;

//     UINT32             m_hOutlineShader;
    FLOAT32            m_fOutlineWidth;

    UINT32             m_hBlendShader;
    INT32              m_hBlendTexture1Loc;
    INT32              m_hBlendTexture2Loc;

    FRMVECTOR3         m_vLightPosition;

    FLOAT32            m_fAmbient;
    
	GLuint			   m_hFrameBufferNameId;
};


#endif // SCENE_H
