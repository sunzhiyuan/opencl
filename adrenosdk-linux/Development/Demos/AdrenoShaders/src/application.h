//=============================================================================
// FILE: application.h
// 
// Application flow. Abstracts platform specific portions from scene rendering.
//
// AUTHOR:          QUALCOMM
// 
//                  Copyright (c) 2008 QUALCOMM Incorporated.
//                              All Rights Reserved.
//                          QUALCOMM Proprietary/GTDR
//=============================================================================

#pragma once

// Common Sample Includes
#include "common.h"

// Framework includes
#include "FrmInput.h"
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <FrmApplication.h>

//=============================================================================
// CONSTANTS
//=============================================================================
#define NUM_SCENES  51  // +1 since 14 is two toon shaders

//=============================================================================
// CLASSES
//=============================================================================
class CScene;

//! This class controls the application flow
class CApplication : public CFrmApplication
{
public:

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    //! Constructor
    //-------------------------------------------------------------------------
    CApplication();
    CApplication(const char* s);
    //-------------------------------------------------------------------------

    //! Destructor
    //-------------------------------------------------------------------------
    virtual ~CApplication();
    //-------------------------------------------------------------------------

    //=========================================================================
    // The following methods need to be provided by the platform specific 
    // implementation
    //=========================================================================

    //! Create platform specific application framework 
    //! @return                     True if successful
   // //-------------------------------------------------------------------------
   // virtual BOOL    CreateAppContainer(unsigned int width, unsigned int height) = 0;
   // //-------------------------------------------------------------------------
   //
   // //! Create platform specific EGL rendering surface
   // //! @return                     True if successful
   // //-------------------------------------------------------------------------
   // virtual BOOL    CreateSurface() = 0;
   // //-------------------------------------------------------------------------
   //
   // //! Destroy platform specific EGL rendering surface
   // //-------------------------------------------------------------------------
   // virtual void    DestroySurface() = 0;
   // //-------------------------------------------------------------------------
   //
   // //////////////////////////////////////////////////////////////////////////
   // // Set current directory
   // virtual void SetCurrentDirectory(const char *path) {}

    //=========================================================================
    // The following methods are called by the platform specific implementation 
    // to allow this class to maintain application flow control
    //=========================================================================

    //! Initialize all application resources
    //! @return                     True if successful
    //-------------------------------------------------------------------------
    BOOL            Create(unsigned int width, unsigned int height);
    //-------------------------------------------------------------------------

    //! Reads the current scene from a file
    //! @return                     The current scene
    //-------------------------------------------------------------------------
    UINT32          ReadCurrentScene();
    //-------------------------------------------------------------------------

    //! Writes the current scene to a file
    //-------------------------------------------------------------------------
    void            WriteCurrentScene();
    //-------------------------------------------------------------------------


    //! Load the font pack used by each scene
    //! @return                     True if successful
    //-------------------------------------------------------------------------
    BOOL            LoadFontPack();
    //-------------------------------------------------------------------------

    //! Load the logo textures used by each scene
    //-------------------------------------------------------------------------
    void            LoadLogoTexture();
    //-------------------------------------------------------------------------


    //! Allocate and initialize current scene
    //-------------------------------------------------------------------------
    void            CreateCurrentScene();
    //-------------------------------------------------------------------------

    //! Set the size of the window
    //! @return                     True if successful
    //-------------------------------------------------------------------------
    BOOL            SetSize(unsigned int width, unsigned int height);
    //-------------------------------------------------------------------------

    //! Destroy all application resources
    //-------------------------------------------------------------------------
   // void            Destroy();
    //-------------------------------------------------------------------------

    //! Render a frame
    //-------------------------------------------------------------------------
   // void            Render();
    //-------------------------------------------------------------------------

    //! A key was pressed
    //-------------------------------------------------------------------------
    void            KeyDown(unsigned int keyCode);
    //-------------------------------------------------------------------------

    //! A key was released
    //-------------------------------------------------------------------------
    void            KeyUp(unsigned int keyCode);
    //-------------------------------------------------------------------------

    //! The screen was touched
    //-------------------------------------------------------------------------
    void            TouchEvent(unsigned int xPos, unsigned int yPos);
    //-------------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // command line options parsing, platform independent
    void            ParseCommandLine(char **argvList, UINT32 argc);

    //////////////////////////////////////////////////////////////////////////
    // saves the current frame to TGA
    void            SaveFrameToTGA();

    //=========================================================================
    // Attributes
    //=========================================================================
protected:
    UINT32          m_ScreenWidth;    //!< Width of the screen when initialized
    UINT32          m_ScreenHeight;   //!< Height of the screen when initialized

    UINT32          m_lastFrameTime;  //!< time of last frame (for FPS display)
    UINT32          m_frameCount;     //!< Count of frames used for FPS display

    UINT32          m_lastTouchTime;  //!< time of last screen touch event


public:
    BOOL            m_bIsWorkingDirSet; /// TRUE if working directory has been set

    //UINT32          m_nWidth;
    //UINT32          m_nHeight;

    CFrmFontGLES    m_Font;
    CFrmTexture*    m_pLogoTexture;

    UINT32          m_nCurrentScene;
    CScene*         m_pScenes[NUM_SCENES];

    BOOL            m_bSaveImageResult;
    char            m_strImageResultFile[255];     // filename for the frame 0 image result from framebuffer
};
