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
// Name: struct FishShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct FishShader
{
    FishShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_LightPosition2Id;
    INT32   m_EyePositionId;
    INT32   m_DiffuseColorId;
    INT32   m_SpecularColorId;
    INT32   m_SpecularColor2Id;
    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;
    INT32   m_TilingId;
    INT32   m_ReflectionStrId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject25
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject25
{
    SimpleObject25();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      DiffuseColor;
    FRMVECTOR4      SpecularColor;
    FRMVECTOR4      SpecularColor2;
    FRMVECTOR2      Tiling;
    FLOAT32         ReflectionStrength;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample25
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample25 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample25( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject25* Object );

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;
    FRMVECTOR3              m_LightPos2;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    FishShader          m_FishShader;
    SimpleObject25        m_Object;
};
