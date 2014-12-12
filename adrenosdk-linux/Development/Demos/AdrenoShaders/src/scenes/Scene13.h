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
// Name: struct StraussShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct StraussShader
{
    StraussShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightDirId;
    INT32   m_CameraPosId;
    INT32   m_DiffuseColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_SmoothnessId;
    INT32   m_MetalnessId;
    INT32   m_TransparencyId;
};




//--------------------------------------------------------------------------------------
// Name: struct SimpleObject13
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject13
{
    SimpleObject13();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    FRMVECTOR4      DiffuseColor;
    FLOAT32         Metalness;
    FLOAT32         Smoothness;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample13
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample13 : public CScene
{

public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample13( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject13* Object );

    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh;

    FRMVECTOR3         m_CameraPos;
    FRMVECTOR3         m_LightPos;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;

    BOOL               m_ShouldRotate;
    FRMVECTOR4         m_AmbientLight;
    StraussShader      m_StraussShader;
    SimpleObject13       m_Object[3];
};

