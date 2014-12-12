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
// Name: struct WoodShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct WoodShader
{
    WoodShader();
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
    INT32   m_SinTanTextureId;
    INT32   m_DiffuseColorId;
    INT32   m_SpecularColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;

    // shader data
    CFrmTexture*    SinTanTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct BlinnPhongShader20
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlinnPhongShader20
{
    BlinnPhongShader20();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_NormalMatrixId;
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;
    INT32   m_AmbientLightId;
    INT32   m_DiffuseColorId;
    INT32   m_SpecularColorId;
    INT32   m_DiffuseTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject20
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject20
{
    SimpleObject20();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      DiffuseColor;
    FRMVECTOR4      SpecularColor;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample20
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample20 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample20( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject20* Object );
    VOID DrawObjectBlinnPhong( SimpleObject20* Object );

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
    WoodShader         m_WoodShader;
    BlinnPhongShader20   m_BlinnPhongShader;
    SimpleObject20       m_Object1;
    SimpleObject20       m_Object2;
    SimpleObject20       m_Object3;
};

