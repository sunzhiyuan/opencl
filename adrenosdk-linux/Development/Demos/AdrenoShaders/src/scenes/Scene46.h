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
// Name: struct MaterialShader46
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader46
{
    MaterialShader46();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelViewProjMatrixId;
    INT32   m_ModelMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;
    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject46
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject46
{
    SimpleObject46();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      DiffuseColor;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample46
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample46 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample46( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject46* Object );
    VOID DrawScene();
    BOOL LoadCubeMap46();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmTexture*            m_pLogoTexture;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject46        m_Object;
    CFrmTexture*        m_RockTexture;
    MaterialShader46      m_MaterialShader;
};

