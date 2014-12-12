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


enum { MESH_FBO=0, BLOOM_FBO, NUM_FBOS };

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
    BOOL CreateFBO( INT32 nFBOId, INT32 nWidth, INT32 nHeight );
    VOID FreeFBO( INT32 nFBOId );
    VOID GetFBO( INT32 nFBOId, INT32 nWidth, INT32 nHeight );
    VOID RenderScreenAlignedQuad();

    VOID RenderMeshFBO();
    VOID RenderBloomFBO();
    VOID RenderBlurredBloomFBO();
    VOID RenderCompositedBloom();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh;

    // FBO related
    UINT32             m_hDepthbufferId[ NUM_FBOS ];
    UINT32             m_pBufferId[ NUM_FBOS ];
    UINT32             m_pTextureId[ NUM_FBOS ];
    INT32              m_nBloomFBOTextureWidth;
    INT32              m_nBloomFBOTextureHeight;
    UINT32             m_nBloomFBOTextureExp;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    // Shader related data
    CFrmTexture*       m_pLogoTexture;

    UINT32             m_hConstantShader;
    INT32              m_hConstantModelViewProjMatrixLoc;
    INT32              m_hConstantColorLoc;
    INT32              m_hConstantDeformSize;
    FLOAT32            m_fDeformSize;

    UINT32             m_hPerPixelLightingShader;
    INT32              m_hModelViewMatrixLoc;
    INT32              m_hModelViewProjMatrixLoc;
    INT32              m_hNormalMatrixLoc;
    INT32              m_hLightPositionLoc;
    INT32              m_hMaterialAmbientLoc;
    INT32              m_hMaterialDiffuseLoc;
    INT32              m_hMaterialSpecularLoc;

    UINT32             m_hGaussianShader;
    INT32              m_hGaussianStepSize;

    UINT32             m_hBlendShader;
    INT32              m_hBlendTexture1Loc;
    INT32              m_hBlendTexture2Loc;

    UINT32             m_hTextureShader;

    BOOL               m_bBloomOn;
    FLOAT32            m_fBlurSize;

    BOOL               m_bShouldRotate;
    FLOAT32            m_fRotateTime;
};


#endif // SCENE_H
