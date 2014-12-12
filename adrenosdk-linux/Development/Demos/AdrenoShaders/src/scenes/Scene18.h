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
// Name: struct LeatherShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct LeatherShader
{
    LeatherShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_AmbientLightId;
    INT32   m_SpecularColorId;
    INT32   m_BumpTextureId;
    INT32   m_DiffuseTextureId;

    // shader data
    CFrmTexture*    DiffuseTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject18
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject18
{
    SimpleObject18();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      DiffuseColor;
    FRMVECTOR4      SpecularColor;
    FLOAT32         ReflectionStrength;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample18
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample18 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample18( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject18* Object );

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
    LeatherShader       m_LeatherShader;
    SimpleObject18        m_Object[2];
};