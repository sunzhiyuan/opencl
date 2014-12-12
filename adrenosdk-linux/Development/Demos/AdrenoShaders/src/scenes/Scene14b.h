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
// Name: struct SToonShader14b
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SToonShader14b
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewMatrixLoc;
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
    INT32   LightPositionLoc;
    INT32   MaterialAmbientLoc;
    INT32   MaterialDiffuseLoc;
    INT32   MaterialSpecularLoc;
    INT32   ColorRampId;
    INT32   EdgeBiasId;

    // textures
    CFrmTexture*    ColorRampTexture;
};


//--------------------------------------------------------------------------------------
// Name: class CSample14b
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample14b : public CScene
{
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample14b( const CHAR* strName );

private:
    BOOL InitShaders();

    VOID RenderSceneToonAndOutline();

    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh;

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    // Shader related data
    INT32              m_hConstantModelViewProjMatrixLoc;
    INT32              m_hConstantColorLoc;
    INT32              m_hConstantDeformSize;
    FLOAT32            m_EdgeBias;

    SToonShader14b        m_ToonShader;

    BOOL               m_bShouldRotate;
    FLOAT              m_fRotateTime;
};

