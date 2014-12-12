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
// Name: struct EnvShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct EnvShader
{
    EnvShader();
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
    INT32   m_ReflectionStrId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject47
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject47
{
    SimpleObject47();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      SpecularColor;
    FLOAT32         ReflectionStrength;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample47
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample47 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample47( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject47* Object );

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTexture*           m_pLogoTexture;

    CFrmMesh           m_Mesh;

    FRMVECTOR3         m_CameraPos;
    FRMVECTOR3         m_LightPos;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;

    BOOL               m_ShouldRotate;
    FRMVECTOR4         m_AmbientLight;
    EnvShader          m_EnvShader;
    SimpleObject47       m_Object[2];
};
