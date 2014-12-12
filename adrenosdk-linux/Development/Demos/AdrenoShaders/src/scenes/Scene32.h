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
// Name: struct MaterialShader32
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader32
{
    MaterialShader32();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_SpecularColorId;
    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;
    INT32   m_ReflectionStrId;
};


//--------------------------------------------------------------------------------------
// Name: struct GlassShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct GlassShader
{
    GlassShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_BackgroundTextureId;
    INT32   m_GlassParamsId;

    // members
    FRMVECTOR2 m_StepSize;
};


//--------------------------------------------------------------------------------------
// Name: struct BackdropShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BackdropShader
{
    BackdropShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   TextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject32
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject32
{
    SimpleObject32();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FLOAT32         ModelScale;
    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    FRMVECTOR3      GlassColor;
    FLOAT32         RotateAmount;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample32
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample32 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample32( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawLevelObject( SimpleObject32* Object );
    VOID DrawGlassObject( SimpleObject32* Object, UINT32 BackgroundTexture );
    VOID DrawOpaques();
    VOID DrawGlass();
    VOID GaussBlur( OffscreenRT4& TargetRT );
    VOID CreateBlurredImage();
    VOID DrawCombinedScene();
    VOID SetupGaussWeights();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_LevelMesh;
    CFrmMesh                m_GlassMesh[2];
    CFrmTexture*            m_DiffuseTexture;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject32        m_Object[3];

    MaterialShader32      m_MaterialShader;
    GlassShader         m_GlassShader;
    BackdropShader      m_BackdropShader;

    OffscreenRT4         m_SharpRT;
    OffscreenRT4         m_BlurredRT;
    OffscreenRT4         m_ScratchRT;
};

