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

#define MAX_LIGHTS (6)


//--------------------------------------------------------------------------------------
// Name: struct MaterialShader48
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader48
{
    MaterialShader48();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelViewProjMatrixId;
    INT32   m_DiffuseTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct PreviewShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader
{
    PreviewShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct LightShader48
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------s
struct LightShader48
{
    LightShader48();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   NormalTextureId;
    INT32   DepthTextureId;
    
    INT32  LightPosId;
    INT32  ModelViewProjInvMatrixId;
    INT32  LightColorId;
};

//--------------------------------------------------------------------------------------
// Name: struct NormalShader48
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct NormalShader48
{
    NormalShader48();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
};


//--------------------------------------------------------------------------------------
// Name: struct CombineShader48
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader48
{
    CombineShader48();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   LightTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject48
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject48
{
    SimpleObject48();
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
// Name: struct LightObject48
// Desc:
//--------------------------------------------------------------------------------------
struct LightObject48
{
    FRMVECTOR3  Position;
    FRMVECTOR3  Color;
    FLOAT32     Radius;
};


//--------------------------------------------------------------------------------------
// Name: class CSample48
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample48 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();
    CSample48( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawObject( SimpleObject48* Object );
    VOID DrawObjectNormals( SimpleObject48* Object );
    VOID DrawScene();
    VOID DrawCombinedScene();
	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight, UINT32 TextureHandle );
    VOID LightBlit( const LightObject48& Light );
    VOID LightBlitMinMax( const LightObject48& Light, FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );    
    VOID RenderSceneNormals(SimpleObject48* Object);
    VOID RenderSceneColor(SimpleObject48* Object);
    
    void DeterminePointLightClipBox(const FRMVECTOR3 position, const float range, FRMVECTOR3 &minVec, FRMVECTOR3 &maxVec);


    CFrmFontGLES            m_Font;
    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmTexture*            m_pLogoTexture;
    CFrmTexture*            m_RayGunTexture;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_LightPos;
    FLOAT32                 m_Aspect;
    FRMMATRIX4X4            m_matView;
    FRMMATRIX4X4            m_matProj;
    FRMMATRIX4X4            m_matViewProj;
    BOOL                    m_ShouldRotate;
    BOOL                    m_Preview;
    FRMVECTOR4              m_AmbientLight;
    SimpleObject48          m_Object;
    LightObject48           m_Lights[MAX_LIGHTS];
    INT32                   m_ShowLight;
    
//    INT32					m_nWidthLightRT;
//    INT32					m_nHeightLightRT;
    
    FRMMATRIX4X4            m_matModel;
    FRMMATRIX4X4            m_matModelView;
    FRMMATRIX4X4            m_matModelViewProj;
    FRMMATRIX3X3            m_matNormal;
    FRMMATRIX4X4            m_matViewProjInv; 

    MaterialShader48          m_MaterialShader;
    NormalShader48            m_NormalShader;
    CombineShader48           m_CombineShader;
    PreviewShader           m_PreviewShader;
    LightShader48             m_LightShader;

    OffscreenRT4             m_ColorRT;
	OffscreenRT4             m_NormalRT;
    OffscreenRT4             m_LightRT;
};
