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
// Name: class CSample19
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample19 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample19( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID BuildMatrices( const FRMVECTOR3& Position );


    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmMesh               m_Mesh;
    FRMVECTOR3             m_CameraPos;
    FRMVECTOR3             m_LightPos;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX4X4       m_matNormal;

    // Shader related data
    UINT32             m_hPerPixelLightingShader;
    INT32              m_ModelMatrixId;
    INT32              m_ModelViewProjMatrixId;
    INT32              m_NormalMatrixId;
    INT32              m_LightPositionId;
    INT32              m_EyePositionId;
    INT32              m_hCookTorranceLookup;
    INT32              m_RoughnessId;
    INT32              m_ReflectionId;
    INT32              m_DiffuseId;
    INT32              m_SpecularId;
    INT32              m_DiffuseTextureId;
    INT32              m_BumpTextureId;
    INT32              m_UVPullId;

    BOOL               m_bShouldRotate;
    FLOAT              m_fRotateTime;
    FLOAT              m_Roughness;
    FLOAT              m_FresnelReflection;
    FLOAT              m_DiffuseIntensity;
    FLOAT              m_SpecularIntensity;
    CFrmTexture*       m_DiffuseTexture[2];
    CFrmTexture*       m_BumpTexture[2];

};
