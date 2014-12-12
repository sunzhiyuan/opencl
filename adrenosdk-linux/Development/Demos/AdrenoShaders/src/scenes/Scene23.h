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
// Name: struct MarbleShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MarbleShader
{
    MarbleShader();
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
    INT32   m_DiffuseColorId;
    INT32   m_SpecularColorId;
    INT32   m_PerlinTextureId;
    INT32   m_BumpTextureId;
    INT32   m_MarbleTextureId;
    INT32   m_ReflectionStrId;

    // shader data
    CFrmTexture*    MarbleTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct PerlinNoiseShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PerlinNoiseShader
{
    PerlinNoiseShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_NoiseTextureId;
    INT32   m_PersistenceId;

    // shader data
    CFrmTexture*    NoiseTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject23
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject23
{
    SimpleObject23();
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
// Name: class CSample23
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample23 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample23( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID InitPerlinNoise();
    VOID DrawObject( SimpleObject23* Object );

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    OffscreenRT         m_PerlinNoiseRT;
    FLOAT32             m_Persistence;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    MarbleShader        m_MarbleShader;
    PerlinNoiseShader   m_PerlinShader;
    SimpleObject23        m_Object[2];
};

