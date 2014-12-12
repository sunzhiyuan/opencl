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
// Name: struct PlasticShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PlasticShader
{
    PlasticShader();
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
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;
    INT32   m_ReflectionStrId;
};


//--------------------------------------------------------------------------------------
// Name: struct BlinnPhongShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlinnPhongShader21
{
    BlinnPhongShader21();
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
// Name: struct SimpleObject21
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject21
{
    SimpleObject21();
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
    FLOAT32         ModelScale;
    BOOL            Tilt;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample21
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample21 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample21( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject21* Object );
    VOID DrawObjectBlinnPhong( SimpleObject21* Object );

    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh[2];

    FRMVECTOR3         m_CameraPos;
    FRMVECTOR3         m_LightPos;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;

    BOOL               m_ShouldRotate;
    FRMVECTOR4         m_AmbientLight;
    PlasticShader      m_PlasticShader;
    BlinnPhongShader21   m_BlinnPhongShader;
    SimpleObject21       m_Object[2];
};
