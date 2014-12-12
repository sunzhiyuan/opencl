//=============================================================================
// FILE: scene.h
//
// Create and render a scene
//
// AUTHOR:          QUALCOMM
//
//                 Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                              All Rights Reserved.
//                          QUALCOMM Proprietary/GTDR
//=============================================================================

#pragma once

// Common Sample Includes

#include <FrmPlatform.h>

// Framework includes
#include "FrmInput.h"
#include "FrmApplication.h"
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>

//=============================================================================
// FORWARD DECLARATIONS
//=============================================================================

//=============================================================================
// CONSTANTS
//=============================================================================
#define NUM_OBJECTS     2

//=============================================================================
// HELPER MACROS
//=============================================================================

//=============================================================================
// TYPEDEFS
//=============================================================================
//--------------------------------------------------------------------------------------
// Name: struct DisintegrateShader
//--------------------------------------------------------------------------------------
struct DisintegrateShader
{
    DisintegrateShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;
    INT32   m_DecayValueId;

    INT32   m_AmbientLightId;
    INT32   m_SpecularColorId;
    INT32   m_BumpTextureId;
    INT32   m_DiffuseTextureId;
    INT32   m_NoiseTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct AssembleShader
//--------------------------------------------------------------------------------------
struct AssembleShader
{
    AssembleShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_NormalMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;
    INT32   m_AssembleValueId;

    INT32   m_AmbientLightId;
    INT32   m_SpecularColorId;
    INT32   m_BumpTextureId;
    INT32   m_DiffuseTextureId;
    INT32   m_NoiseTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject
{
    SimpleObject();
    VOID Update( FLOAT ElapsedTime );
    VOID Destroy();

    BOOL            ShouldRender;
    float           ModelScale;
    FLOAT32         RotateTime;
    FRMVECTOR3      Position;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    CFrmTexture*    NoiseTexture;

    FRMVECTOR4      DiffuseColor;
    FRMVECTOR4      SpecularColor;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
    FRMMATRIX4X4    MatModelViewProj;

    float           XRotation;
    float           YRotation;
    float           ZRotation;
};


struct ParticleInfo
{
    float                   DecayLevel;
    float                   LaunchTime;
    float                   PathTime;
    FRMVECTOR4              vBasePos;
    FRMVECTOR4              vStartPos;
    FRMVECTOR4              vEndPos;    // Is this used?
    FRMVECTOR4              vVelocity;  // Is this used?
};

//=============================================================================
// CScene Class
//=============================================================================
class CScene: public CFrmApplication
{
public:
    CScene(const CHAR* strName);
    virtual ~CScene();
    
	virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    void    TouchEvent(float xPos, float yPos);

    void    BuildInfoString(char *pBuffer, int iSize, float fFrameRate);

private:
    BOOL    InitShaders();
    VOID    InitParticles();
    void    SetParticleDecayValues(const char *pDecayTexture);
    VOID    UpdateParticles(FLOAT32 fElapsedTime);

    VOID    DrawDisintegrateObject( SimpleObject* Object );
    VOID    DrawAssembleObject( SimpleObject* Object );

    VOID    SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );
    void    BuildCameraTransforms(UINT32 nWidth, UINT32 nHeight);

    void    ResetSimulation();

	void	KeyDown(unsigned int keyCode);
	void	KeyUp(unsigned int keyCode);
	void	TouchEvent(unsigned int xPos, unsigned int yPos);

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    CFrmTexture*            m_DiffuseTexture;
    CFrmTexture*            m_BumpTexture;
    CFrmTexture*            m_NoiseTexture;
    FLOAT                   m_DecayValue;
    FLOAT                   m_AssembleValue;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4            m_matView;
    FRMMATRIX4X4            m_matProj;

    FRMVECTOR4              m_AmbientLight;
    DisintegrateShader      m_DisintegrateShader;
    AssembleShader          m_AssembleShader;
    SimpleObject            m_Objects[NUM_OBJECTS];

    // Particle Shader related data
    UINT32                  m_NumParticles;
    UINT32                  m_ParticleFactor;
    GLuint                  m_hParticleShader;
    GLint                   m_slotModelViewProjMatrix;
    GLint                   m_slotColorTexture;
    GLint                   m_slotNoiseTexture;
    GLint                   m_slotNoiseValue;
    FRMVECTOR4              *m_vParticlePos;
    FRMVECTOR2              *m_vParticleUV;
    float                   *m_pSpriteSize;

    // Particle information
    ParticleInfo            *m_ParticleInfo;

    BOOL                m_bShouldRotate;
    FLOAT               m_fRotateTime;

    UINT32              m_FrameCount;
    FLOAT               m_LastFPSDisplayTime;
    FLOAT               m_LastTouchTime;

public:
	CFrmFontGLES* m_pFont;                 // Font for rendering text
	CFrmTexture* m_pLogoTexture;

    const CHAR* m_strName;
    // INT32       m_nWidth;
    // INT32       m_nHeight;
    //
    //FRM_INPUT   m_Input;   

    // Declare shader variables
    UINT32             m_hConstantShader;
    INT32              m_hConstantMVPLoc;
    INT32              m_hConstantColorLoc;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraView;
    FRMMATRIX4X4       m_matMeshModel;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMMATRIX4X4       m_matLightMeshModelViewProj;

    // Declare user controlled variables
};

