//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();
    VOID RenderScene();

	VOID RenderSensorValues();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;

    CFrmMesh           m_Mesh;
	CFrmMesh           m_Mesh2;

    CFrmTexture*       m_pLogoTexture;

    UINT32             m_hTextureShader;

	INT32              m_hTextureModelViewMatrixLoc;
    INT32              m_hTextureModelViewProjMatrixLoc;
    CFrmTexture*       m_pTexture;
	CFrmTexture*       m_pTexture2;

	BOOL			   m_bCalibrateOn;
	BOOL			   m_bShowSensorValues;

	FLOAT32			   m_X;
	FLOAT32			   m_Y;
};


#endif // SCENE_H
