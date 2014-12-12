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
// Name: struct MaterialShader31
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader31
{
    MaterialShader31();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
    INT32   m_BumpTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct CutoutShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CutoutShader
{
    CutoutShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   DepthTextureId;
    INT32   ScreenLightPosId;
    INT32   OccluderParamsId;
    INT32   NearQAspectId;

    // members
    FRMVECTOR3  NearFarQ;
    FLOAT32     OccluderThreshold;
    FLOAT32     OccluderFadeout;
};


//--------------------------------------------------------------------------------------
// Name: struct RayExtruderShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct RayExtruderShader
{
    RayExtruderShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   RayTextureId;
    INT32   ScreenLightPosId;
    INT32   RayParamsId;

    // members
    FLOAT32     RayLength;
};


//--------------------------------------------------------------------------------------
// Name: struct CombineShader31
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader31
{
    CombineShader31();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   RaysTextureId;
    INT32   RayOpacityId;
    INT32   RayColorId;

    // members
    FLOAT32     RayOpacity;
};


//--------------------------------------------------------------------------------------
// Name: struct DownsizeShader31
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader31
{
    DownsizeShader31();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct BlurShader31
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader31
{
    BlurShader31();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   GaussWeightId;
    INT32   GaussInvSumId;
    INT32   StepSizeId;

    // members
    FLOAT32 GaussWeight[4];
    FLOAT32 GaussInvSum;
    FRMVECTOR2 StepSize;
    FLOAT32 GaussSpread;
    FLOAT32 GaussRho;
};


//--------------------------------------------------------------------------------------
// Name: struct PreviewShader31
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader31
{
    PreviewShader31();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject31
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject31
{
    SimpleObject31();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
};


//--------------------------------------------------------------------------------------
// Name: class CSample31
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample31 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample31( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject31* Object );
    VOID SetupGaussWeights();
    VOID GaussBlur( OffscreenRT4& TargetRT );
    VOID DrawSceneToBackbuffer();
    VOID DrawGodRays();

	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmTexture*            m_GodRayTexture;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;
    FLOAT32                 m_Aspect;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject31        m_Object;

    MaterialShader31      m_MaterialShader;
    CutoutShader        m_CutoutShader;
    DownsizeShader31      m_DownsizeShader;
    RayExtruderShader   m_RayExtruderShader;
    CombineShader31       m_CombineShader;
    BlurShader31          m_BlurShader;

    OffscreenRT4         m_OffscreenRT;
    OffscreenRT4         m_CutoutRT;
    OffscreenRT4         m_QuarterRT;
    OffscreenRT4         m_ScratchRT;

    FLOAT32 zNear;
    FLOAT32 zFar;

    PreviewShader31       m_PreviewShader;
};

