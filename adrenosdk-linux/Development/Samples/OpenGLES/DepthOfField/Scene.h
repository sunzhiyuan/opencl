//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


//--------------------------------------------------------------------------------------
// We are going to blend between the sharp and out of focus FBOs based upon
// the distance an object is from the viewer.
//--------------------------------------------------------------------------------------
enum { SHARP_FBO=0, OUT_OF_FOCUS_FBO, NUM_FBOS };

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();

    BOOL CheckFrameBufferStatus();
    BOOL CreateFBO( UINT32 nFBOId, UINT32 nWidth, UINT32 nHeight );
    VOID FreeFBO( UINT32 nFBOId );
    VOID GetFBO( UINT32 nFBOId, UINT32 nWidth, UINT32 nHeight );
    VOID RenderScreenAlignedQuad();

    VOID RenderSharpSceneAndBlurinessToFBO();
    VOID RenderBlurredSceneToFBO();
    VOID RenderBlendedScene();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    UINT32             m_hDepthbufferId[ NUM_FBOS ];
    UINT32             m_pBufferId[ NUM_FBOS ];
    UINT32             m_pTextureId[ NUM_FBOS ];

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;

    CFrmMesh           m_Mesh;

    CFrmTexture*       m_pLogoTexture;

    UINT32             m_hTextureShader;
    INT32              m_hTextureModelViewMatrixLoc;
    INT32              m_hTextureModelViewProjMatrixLoc;
    CFrmTexture*       m_pTexture;

    UINT32             m_hGaussianShader;
    INT32              m_hGaussianStepSizeLoc;

    UINT32             m_hDepthOfFieldShader;
    INT32              m_hDepthOfFieldSharpImageLoc;
    INT32              m_hDepthOfFieldBlurredImageLoc;
    INT32              m_hDepthOfFieldRangeLoc;
    INT32              m_hDepthOfFieldFocusLoc;

    INT32              m_nOutOfFocusFBOWidth;
    INT32              m_nOutOfFocusFBOHeight;

    FLOAT32            m_fRange;
    FLOAT32            m_fFocus;
};


#endif // SCENE_H
