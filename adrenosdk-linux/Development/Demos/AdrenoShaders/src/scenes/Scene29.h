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
// Name: struct MaterialShader29
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader29
{
    MaterialShader29();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct DepthFogShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DepthFogShader
{
    DepthFogShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   DepthTextureId;
    INT32   NearQId;
    INT32   FogColorId;
    INT32   FogParamsId;

    // members
    FRMVECTOR3  NearFarQ;
    FRMVECTOR4  FogColors;
    FLOAT32     FogStart;
    FLOAT32     FogRange;
    FLOAT32     FogDensity;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject29
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject29
{
    SimpleObject29();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample29
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample29 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample29( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject29* Object );
    VOID DrawScene();
    VOID DrawFoggedScene();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject29        m_Object;

    MaterialShader29      m_MaterialShader;
    DepthFogShader      m_DepthFogShader;

    OffscreenRT4         m_OffscreenRT;
};
