//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#pragma once

// The Parent class
#include "../scene.h"


//--------------------------------------------------------------------------------------
// Name: struct MotionBlurShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MotionBlurShader
{
    MotionBlurShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelViewMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_LastModelViewMatrixId;
    INT32   m_LastModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_DiffuseTextureId;
    INT32   m_StretchAmountId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject28
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject28
{
    SimpleObject28();
    VOID Update( FRMMATRIX4X4& matView, FRMMATRIX4X4& matProj, FLOAT ElapsedTime, BOOL Paused = FALSE );
    VOID Destroy();
    VOID GenRandomPosition();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;

    // for motion blur
    FRMMATRIX4X4    LastMatModelView;
    FRMMATRIX4X4    LastMatModelViewProj;
};


//--------------------------------------------------------------------------------------
// Name: class CSample28
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample28 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample28( const CHAR* strName );

private:    
    BOOL InitShaders();
	VOID UpdateScene( FLOAT32 ElapsedTime );
    VOID DrawObject( SimpleObject28* Object );
    VOID DrawScene();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4			m_matView;
    FRMMATRIX4X4			m_matProj;
    FRMMATRIX4X4			m_matViewProj;

    BOOL					m_ShouldRotate;
    SimpleObject28			m_Object[10];

    MotionBlurShader		m_MotionBlurShader;
    FLOAT32					m_StretchAmount;
};
