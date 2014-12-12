//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

class Liquid;

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
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

private:
    BOOL InitShaders();


    VOID SetSWEShader();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    FLOAT32                m_fElapsedTime;
    CFrmUserInterfaceGLES  m_UserInterface;

    GLuint             m_hSWEShader;
    GLint              m_nSWEBaseTextureSlot;
    GLint              m_nSWEEnvTextureSlot;
    GLint              m_nSWENormalTextureSlot;
 
	CFrmTexture*       m_pBaseTexture;
    CFrmTexture*       m_pEnvTexture;
    CFrmTexture*       m_pNormalTexture;
 
    FLOAT32            m_fStepSize;
    FLOAT32            m_fHalfStepSize;

    CFrmTexture*       m_pLogoTexture;

	Liquid*				m_pLiquid;

};


#endif // SCENE_H
