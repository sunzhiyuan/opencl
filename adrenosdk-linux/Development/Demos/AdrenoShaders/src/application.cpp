//=============================================================================
// FILE: application.cpp
//
// Application flow. Abstracts platform specific portions from scene rendering.
//
// Author:        QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//                  Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                              All Rights Reserved.
//                          QUALCOMM Proprietary/GTDR
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================

#include <GLES2/gl2.h>

#include "application.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "FrmFile.h"

#include "scene.h"
#include "scenes/Scene01.h"

#include "scenes/Scene02.h"
#include "scenes/Scene03.h"
#include "scenes/Scene04.h"
#include "scenes/Scene05.h"
#include "scenes/Scene06.h"
#include "scenes/Scene07.h"
#include "scenes/Scene08.h"
#include "scenes/Scene09.h"
#include "scenes/Scene10.h"
#include "scenes/Scene11.h"
#include "scenes/Scene12.h"
#include "scenes/Scene13.h"
#include "scenes/Scene14.h"
#include "scenes/Scene14b.h"
#include "scenes/Scene15.h"
#include "scenes/Scene16.h"
#include "scenes/Scene17.h"
#include "scenes/Scene18.h"
#include "scenes/Scene19.h"
#include "scenes/Scene20.h"
#include "scenes/Scene21.h"
#include "scenes/Scene22.h"
#include "scenes/Scene23.h"
#include "scenes/Scene24.h"
#include "scenes/Scene25.h"
#include "scenes/Scene26.h"
#include "scenes/Scene27.h"
#include "scenes/Scene28.h"
#include "scenes/Scene29.h"
#include "scenes/Scene30.h"
#include "scenes/Scene31.h"
#include "scenes/Scene32.h"
#include "scenes/Scene33.h"
#include "scenes/Scene34.h"
#include "scenes/Scene35.h"
#include "scenes/Scene36.h"
#include "scenes/Scene37.h"
#include "scenes/Scene38.h"
#include "scenes/Scene39.h"
#include "scenes/Scene40.h"
#include "scenes/Scene41.h"
#include "scenes/Scene42.h"
#include "scenes/Scene43.h"
#include "scenes/Scene44.h"
#include "scenes/Scene45.h"
#include "scenes/Scene46.h"
#include "scenes/Scene47.h"
#include "scenes/Scene48.h"
#include "scenes/Scene49.h"
#include "scenes/Scene50.h"

//=============================================================================
// GLOBALS
//=============================================================================
char *gpSceneName[NUM_SCENES] = {
                                (char *)"01 - Ambient Lighting",
                                (char *)"02 - Diffuse Lighting",
                                (char *)"03 - Specular (Isotropic)",
                                (char *)"04 - Specular (Anisotropic)",
                                (char *)"05 - Blinn-Phong Lighting",
                                (char *)"06 - 2D Light Texture",
                                (char *)"07 - Diffuse Cube Map",
                                (char *)"08 - Normal and Parallax Map",
                                (char *)"09 - Cook-Torrance Lighting",
                                (char *)"10 - Oren-Nayar Lighting",
                                (char *)"11 - Ward Lighting",
                                (char *)"12 - Ashkhmin-Shirley",
                                (char *)"13 - Strauss Lighting",
                                (char *)"14 - Toon shading",
                                (char *)"14b - Cheap toon shading",
                                (char *)"15 - Hatch shading",
                                (char *)"16 - Skin shading",
                                (char *)"17 - Cloth shading",
                                (char *)"18 - Leather shading",
                                (char *)"19 - Metal",
                                (char *)"20 - Wood shading",
                                (char *)"21 - Plastic shading",
                                (char *)"22 - Eye shading",
                                (char *)"23 - Marble shading",
                                (char *)"24 - Fur shading",
                                (char *)"25 - Fish scales",
                                (char *)"26 - Elephant skin",
                                (char *)"27 - Depth of Field",
                                (char *)"28 - Motion Blur",
                                (char *)"29 - Attenuation depth fog",
                                (char *)"30 - Height fog",
                                (char *)"31 - God rays",
                                (char *)"32 - Glass Distortion",
                                (char *)"33 - Light Bloom",
                                (char *)"34 - Eye Adaptation (Press 9 for Preview)",
                                (char *)"35 - Bright Pass (Press 9 for Preview)",
                                (char *)"36 - Reinhard Tonemapping (Press 9 for Preview)",
                                (char *)"37 - Sepia",
                                (char *)"38 - Color Filters",
                                (char *)"39 - Light streaks",
                                (char *)"40 - Film Grain",
                                (char *)"41 - Frame Border",
                                (char *)"42 - Median Filter",
                                (char *)"43 - Warping",
                                (char *)"44 - Screen Space Ambient Occlusion",
                                (char *)"45 - One Bounce Effect",
                                (char *)"46 - Normalizing Cube Maps",
                                (char *)"47 - Environment shading",
                                (char *)"48 - Deferred Lighting",
                                (char *)"49 - HDR Textures",
                                (char *)"50 - Color Spaces",
                                };

//=============================================================================
// IMPLEMENTATION
//=============================================================================

//-----------------------------------------------------------------------------
CApplication::CApplication(const CHAR* strName ) : CFrmApplication( strName )
//-----------------------------------------------------------------------------
{
    m_nCurrentScene = 100001; // set to 100001 as a flag for whether we're reading from the command line or the file

    for (UINT32 iIndx = 0; iIndx < NUM_SCENES; iIndx++)
    {
        m_pScenes[iIndx] = NULL;
    }

    m_pLogoTexture = NULL;
    m_bIsWorkingDirSet = FALSE;

    m_bSaveImageResult = FALSE;
}

BOOL CApplication::Initialize()
{
    return Create(m_nWidth,m_nHeight);           
}
BOOL CApplication::Resize()
{
   

    return TRUE;
}




//-----------------------------------------------------------------------------
CApplication::~CApplication()
//-----------------------------------------------------------------------------
{
    for(UINT32 iIndx = 0; iIndx < NUM_SCENES; iIndx++)
    {
        if(m_pScenes[iIndx] != NULL)
        {
            SAFE_DELETE(m_pScenes[iIndx]);
            m_pScenes[iIndx] = NULL;
        }
    }
}

//-----------------------------------------------------------------------------
BOOL CApplication::Create(unsigned int width, unsigned int height)
//-----------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::Create()");

    m_ScreenWidth = width;
    m_ScreenHeight = height;
    m_lastFrameTime =  FrmGetTime() * 1000;
    m_frameCount = 0;

    m_lastTouchTime = 0;
  
    if(!LoadFontPack())
        return false;

    LoadLogoTexture();
    if(m_pLogoTexture == NULL)
        return false;

    // if the current scene hasn't been set, then read from the file
    if (m_nCurrentScene > 100000)
        m_nCurrentScene = ReadCurrentScene();
    CreateCurrentScene();

    return true;
}

//-------------------------------------------------------------------------
UINT32 CApplication::ReadCurrentScene()
//-------------------------------------------------------------------------
{
    UINT32 nCurrent = 0; // NUM_SCENES - 1

    FRM_FILE* pFile;
    if( FALSE == FrmFile_Open( "Demos/AdrenoShaders/CurrentScene.txt", FRM_FILE_READ, &pFile ) )
        return nCurrent;

    char Buffer[100];
    memset(Buffer, 0, sizeof(Buffer));
    FrmFile_Read( pFile, Buffer, sizeof(Buffer) );

    // Even if file doesn't exist, atoi("") is valid
    nCurrent = atoi(Buffer);
    FrmFile_Close( pFile );

    return nCurrent;
}

//-------------------------------------------------------------------------
void CApplication::WriteCurrentScene()
//-------------------------------------------------------------------------
{
    FRM_FILE* pFile;
    if( FALSE == FrmFile_Open( "Demos/AdrenoShaders/CurrentScene.txt", FRM_FILE_WRITE, &pFile ) )
        return;

    char Buffer[100];
    memset(Buffer, 0, sizeof(Buffer));
    sprintf(Buffer, "%d", m_nCurrentScene);
    FrmFile_Write( pFile, Buffer, strlen(Buffer) );
    FrmFile_Close( pFile );
}

//-----------------------------------------------------------------------------
BOOL CApplication::LoadFontPack()
//-----------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::LoadFontPack()\n");
    if( FALSE == m_Font.Create( "Demos/AdrenoShaders/Fonts/Tuffy12.pak" ) )
    {
        FrmLogMessage("Failed to load Font Pack!\n");
        return FALSE;
    }

    return TRUE;
}

//! Load the logo textures used by each scene
//-------------------------------------------------------------------------
void CApplication::LoadLogoTexture()
//-------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::LoadLogoTexture()\n");

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/Logo.pak" ) )
    {
        FrmLogMessage("Failed to load Logo Texture!\n");
        return;
    }

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
}

//! Allocate and initialize current scene
//-------------------------------------------------------------------------
void CApplication::CreateCurrentScene()
//-------------------------------------------------------------------------
{
    if(m_nCurrentScene >= NUM_SCENES)
    {
        return;
    }
    // Make sure it is not already created
    if(m_pScenes[m_nCurrentScene] != NULL)
    {
        // Already created, make sure it is correct size
        m_pScenes[m_nCurrentScene]->m_nWidth = m_ScreenWidth;
        m_pScenes[m_nCurrentScene]->m_nHeight = m_ScreenHeight;
        m_pScenes[m_nCurrentScene]->Resize();
        return;
    }

    m_pScenes[m_nCurrentScene] = NULL;
    switch(m_nCurrentScene)
    {
    
    case  0: m_pScenes[m_nCurrentScene] = new CSample01(gpSceneName[m_nCurrentScene]); break;
    case  1: m_pScenes[m_nCurrentScene] = new CSample02(gpSceneName[m_nCurrentScene]); break;
    case  2: m_pScenes[m_nCurrentScene] = new CSample03(gpSceneName[m_nCurrentScene]); break;
    case  3: m_pScenes[m_nCurrentScene] = new CSample04(gpSceneName[m_nCurrentScene]); break;
    case  4: m_pScenes[m_nCurrentScene] = new CSample05(gpSceneName[m_nCurrentScene]); break;
    case  5: m_pScenes[m_nCurrentScene] = new CSample06(gpSceneName[m_nCurrentScene]); break;
    case  6: m_pScenes[m_nCurrentScene] = new CSample07(gpSceneName[m_nCurrentScene]); break;
    case  7: m_pScenes[m_nCurrentScene] = new CSample08(gpSceneName[m_nCurrentScene]); break;
    case  8: m_pScenes[m_nCurrentScene] = new CSample09(gpSceneName[m_nCurrentScene]); break;
    case  9: m_pScenes[m_nCurrentScene] = new CSample10(gpSceneName[m_nCurrentScene]); break;
    case 10: m_pScenes[m_nCurrentScene] = new CSample11(gpSceneName[m_nCurrentScene]); break;
    case 11: m_pScenes[m_nCurrentScene] = new CSample12(gpSceneName[m_nCurrentScene]); break;
    case 12: m_pScenes[m_nCurrentScene] = new CSample13(gpSceneName[m_nCurrentScene]); break;
    case 13: m_pScenes[m_nCurrentScene] = new CSample14(gpSceneName[m_nCurrentScene]); break;
    case 14: m_pScenes[m_nCurrentScene] = new CSample14b(gpSceneName[m_nCurrentScene]); break;
    case 15: m_pScenes[m_nCurrentScene] = new CSample15(gpSceneName[m_nCurrentScene]); break;
    case 16: m_pScenes[m_nCurrentScene] = new CSample16(gpSceneName[m_nCurrentScene]); break;
    case 17: m_pScenes[m_nCurrentScene] = new CSample17(gpSceneName[m_nCurrentScene]); break;
    case 18: m_pScenes[m_nCurrentScene] = new CSample18(gpSceneName[m_nCurrentScene]); break;
    case 19: m_pScenes[m_nCurrentScene] = new CSample19(gpSceneName[m_nCurrentScene]); break;
    case 20: m_pScenes[m_nCurrentScene] = new CSample20(gpSceneName[m_nCurrentScene]); break;
    case 21: m_pScenes[m_nCurrentScene] = new CSample21(gpSceneName[m_nCurrentScene]); break;
    case 22: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
    case 23: m_pScenes[m_nCurrentScene] = new CSample23(gpSceneName[m_nCurrentScene]); break;
    case 24: m_pScenes[m_nCurrentScene] = new CSample24(gpSceneName[m_nCurrentScene]); break;
    case 25: m_pScenes[m_nCurrentScene] = new CSample25(gpSceneName[m_nCurrentScene]); break;
    case 26: m_pScenes[m_nCurrentScene] = new CSample26(gpSceneName[m_nCurrentScene]); break;
    case 27: m_pScenes[m_nCurrentScene] = new CSample27(gpSceneName[m_nCurrentScene]); break;
    case 28: m_pScenes[m_nCurrentScene] = new CSample28(gpSceneName[m_nCurrentScene]); break;
    case 29: m_pScenes[m_nCurrentScene] = new CSample29(gpSceneName[m_nCurrentScene]); break;
    case 30: m_pScenes[m_nCurrentScene] = new CSample30(gpSceneName[m_nCurrentScene]); break;
    case 31: m_pScenes[m_nCurrentScene] = new CSample31(gpSceneName[m_nCurrentScene]); break;
    case 32: m_pScenes[m_nCurrentScene] = new CSample32(gpSceneName[m_nCurrentScene]); break;
    case 33: m_pScenes[m_nCurrentScene] = new CSample33(gpSceneName[m_nCurrentScene]); break;
    case 37: m_pScenes[m_nCurrentScene] = new CSample37(gpSceneName[m_nCurrentScene]); break;
    case 38: m_pScenes[m_nCurrentScene] = new CSample38(gpSceneName[m_nCurrentScene]); break;
    case 42: m_pScenes[m_nCurrentScene] = new CSample42(gpSceneName[m_nCurrentScene]); break;
    case 43: m_pScenes[m_nCurrentScene] = new CSample43(gpSceneName[m_nCurrentScene]); break;
    case 44: m_pScenes[m_nCurrentScene] = new CSample44(gpSceneName[m_nCurrentScene]); break;
    case 45: m_pScenes[m_nCurrentScene] = new CSample45(gpSceneName[m_nCurrentScene]); break;
    case 46: m_pScenes[m_nCurrentScene] = new CSample46(gpSceneName[m_nCurrentScene]); break;
    case 47: m_pScenes[m_nCurrentScene] = new CSample47(gpSceneName[m_nCurrentScene]); break;
    case 48: m_pScenes[m_nCurrentScene] = new CSample48(gpSceneName[m_nCurrentScene]); break;
    case 49: m_pScenes[m_nCurrentScene] = new CSample49(gpSceneName[m_nCurrentScene]); break;
    case 50: m_pScenes[m_nCurrentScene] = new CSample50(gpSceneName[m_nCurrentScene]); break;
  
    case 34: m_pScenes[m_nCurrentScene] = new CSample34(gpSceneName[m_nCurrentScene]); break;
    case 35: m_pScenes[m_nCurrentScene] = new CSample35(gpSceneName[m_nCurrentScene]); break;
    case 36: m_pScenes[m_nCurrentScene] = new CSample36(gpSceneName[m_nCurrentScene]); break;
    case 39: m_pScenes[m_nCurrentScene] = new CSample39(gpSceneName[m_nCurrentScene]); break;
    case 40: m_pScenes[m_nCurrentScene] = new CSample40(gpSceneName[m_nCurrentScene]); break;
    case 41: m_pScenes[m_nCurrentScene] = new CSample41(gpSceneName[m_nCurrentScene]); break;


    // THESE TEST CRASHES THE DROID! STUB OUT WITH EYEBALL
#if defined(OS_ANDROID)
//    case 34: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
//    case 35: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
//    case 36: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
 //   case 39: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
//    case 40: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
 //   case 41: m_pScenes[m_nCurrentScene] = new CSample22(gpSceneName[m_nCurrentScene]); break;
#else 
//    case 34: m_pScenes[m_nCurrentScene] = new CSample34(gpSceneName[m_nCurrentScene]); break;
//    case 35: m_pScenes[m_nCurrentScene] = new CSample35(gpSceneName[m_nCurrentScene]); break;
//    case 36: m_pScenes[m_nCurrentScene] = new CSample36(gpSceneName[m_nCurrentScene]); break;
//    case 39: m_pScenes[m_nCurrentScene] = new CSample39(gpSceneName[m_nCurrentScene]); break;
//    case 40: m_pScenes[m_nCurrentScene] = new CSample40(gpSceneName[m_nCurrentScene]); break;
  //  case 41: m_pScenes[m_nCurrentScene] = new CSample41(gpSceneName[m_nCurrentScene]); break;
#endif // defined(OS_ANDROID

    }

    if(m_pScenes[m_nCurrentScene] == NULL)
        return;

    m_pScenes[m_nCurrentScene]->m_nWidth = m_ScreenWidth;
    m_pScenes[m_nCurrentScene]->m_nHeight = m_ScreenHeight;
    if (!m_pScenes[m_nCurrentScene]->Initialize(m_Font, m_pLogoTexture))
    {
        delete m_pScenes[m_nCurrentScene];
        m_pScenes[m_nCurrentScene] = NULL;
        return;
    }
    if (!m_pScenes[m_nCurrentScene]->Resize())
    {
        delete m_pScenes[m_nCurrentScene];
        m_pScenes[m_nCurrentScene] = NULL;
        return;
    }

    // Save the new current scene
    WriteCurrentScene();

}

//-----------------------------------------------------------------------------
BOOL CApplication::SetSize(unsigned int width, unsigned int height)
//-----------------------------------------------------------------------------
{

    m_ScreenWidth = width;
    m_ScreenHeight = height;

    // Set size for current scene
    if(m_nCurrentScene < NUM_SCENES && m_pScenes[m_nCurrentScene] != NULL)
    {
        m_pScenes[m_nCurrentScene]->m_nWidth = width;
        m_pScenes[m_nCurrentScene]->m_nHeight = height;
        if (!m_pScenes[m_nCurrentScene]->Resize())
        {
            return false;
        }
    }
    else
    {
        // At this point the current scene must have failed to load.
        // Try to load the next one.
        while (m_nCurrentScene > NUM_SCENES || m_pScenes[m_nCurrentScene] == NULL)
        {
            FrmLogMessage("Scene load must have failed, loading next scene");
            m_nCurrentScene++;
            if(m_nCurrentScene >= NUM_SCENES)
                m_nCurrentScene = 0;

            // Now create the new scene
            CreateCurrentScene();
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void CApplication::Destroy()
//-----------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::Destroy()\n");

    for(UINT32 iIndx = 0; iIndx < NUM_SCENES; iIndx++)
    {
        if(m_pScenes[iIndx] != NULL)
        {
            m_pScenes[iIndx]->Destroy();
            SAFE_DELETE(m_pScenes[iIndx]);
            m_pScenes[iIndx] = NULL;
        }
    }

    m_Font.Destroy();

    // Free texture memory
    if( m_pLogoTexture )
        m_pLogoTexture->Release();

//    DestroySurface();
}

//=============================================================================
// EVENTS
//=============================================================================

VOID CApplication::Update()
{

}


//-----------------------------------------------------------------------------
void CApplication::Render()
//-----------------------------------------------------------------------------
{
    if(m_nCurrentScene >= NUM_SCENES || m_pScenes[m_nCurrentScene] == NULL)
        return;

    // If the mouse/screen has been pressed of arrows pressed, move to next/prev scene
    //if( (m_pScenes[m_nCurrentScene]->m_Input.m_nButtons & FRM_INPUT::DPAD_LEFT) ||
    //    (m_pScenes[m_nCurrentScene]->m_Input.m_nButtons & FRM_INPUT::DPAD_RIGHT) ||
    //    (m_pScenes[m_nCurrentScene]->m_Input.m_nPointerState & FRM_INPUT::POINTER_PRESSED))
    
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

	UINT32 pntState;
	FRMVECTOR2 newMousePose;
	m_Input.GetPointerState( &pntState, &newMousePose );

    //if( (m_pScenes[m_nCurrentScene]->m_Input.m_nPointerState & FRM_INPUT::POINTER_PRESSED))
    //                    [->] button                       [<-] Button
    if((nPressedButtons & 0x00100000) || 
        (nPressedButtons & 0x00080000) || 
        (pntState & FRM_INPUT::POINTER_RELEASED) 

        )
    {


        float ClickPos = 1.0f;
        //if( (m_pScenes[m_nCurrentScene]->m_Input.m_nButtons & FRM_INPUT::DPAD_LEFT))
        //    ClickPos = -1.0f;
        //else if (m_pScenes[m_nCurrentScene]->m_Input.m_nButtons & FRM_INPUT::DPAD_RIGHT)
        //    ClickPos = 1.0f;
        //else if(m_pScenes[m_nCurrentScene]->m_Input.m_nPointerState & FRM_INPUT::POINTER_PRESSED)
        ClickPos = newMousePose.x;

        if(nPressedButtons & 0x00100000)
            ClickPos = 1.0;
        if(nPressedButtons & 0x00080000)
            ClickPos = -1.0f;


        // Reset the button/pointer state
        m_pScenes[m_nCurrentScene]->m_Input.m_nButtons = 0;
        m_pScenes[m_nCurrentScene]->m_Input.m_nPointerState = 0;

        // Release the old scene
        m_pScenes[m_nCurrentScene]->Destroy();
        SAFE_DELETE(m_pScenes[m_nCurrentScene]);
        m_pScenes[m_nCurrentScene] = NULL;

        // Put this in a do loop so if a scene fails to load (usually compiling shaders fails on Droid)
        // we will come out with the next scene.
        do 
        {
            // If pressed on the right (0.0 < X < 1.0), go to next.
            // Otherwise, go to previous
            if(ClickPos >= 0.0f)
            {
                // Next scene
                m_nCurrentScene++;
                if(m_nCurrentScene >= NUM_SCENES)
                    m_nCurrentScene = 0;
            }
            else
            {
                // Previous scene
                if(m_nCurrentScene > 0)
                    m_nCurrentScene--;
                else
                    m_nCurrentScene = NUM_SCENES - 1;
            }

            // Now create the new scene
            CreateCurrentScene();
            //if(m_nCurrentScene >= NUM_SCENES || m_pScenes[m_nCurrentScene] == NULL)
            //    return;
        } while (m_pScenes[m_nCurrentScene] == NULL);
    }

    // Update the current scene
    if(m_nCurrentScene < NUM_SCENES && m_pScenes[m_nCurrentScene] != NULL)
        m_pScenes[m_nCurrentScene]->Update();

    // Render the current scene
    if(m_nCurrentScene < NUM_SCENES && m_pScenes[m_nCurrentScene] != NULL)
        m_pScenes[m_nCurrentScene]->Render();

    // for now we only do this for the first frame - should save one frame
    if (m_bSaveImageResult)
    {
        SaveFrameToTGA();
    }
}

//-----------------------------------------------------------------------------
void CApplication::KeyDown(unsigned int keyCode)
//-----------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::KeyDown()");

    if( m_nCurrentScene < NUM_SCENES && m_pScenes[m_nCurrentScene] != NULL)
    {
        m_pScenes[m_nCurrentScene]->m_Input.m_nButtons |= keyCode;
    }
}

//-----------------------------------------------------------------------------
void CApplication::KeyUp(unsigned int keyCode)
//-----------------------------------------------------------------------------
{
    FrmLogMessage("CApplication::KeyUp()\n");

    if( m_nCurrentScene < NUM_SCENES && m_pScenes[m_nCurrentScene] != NULL)
    {
        m_pScenes[m_nCurrentScene]->m_Input.m_nButtons &= ~keyCode;
    }
}

//-----------------------------------------------------------------------------
void CApplication::TouchEvent(unsigned int xPos, unsigned int yPos)
//-----------------------------------------------------------------------------
{

    // Ignore touch events if not yet set up
    if( m_nCurrentScene >= NUM_SCENES || m_pScenes[m_nCurrentScene] == NULL)
        return;

    // Just for sanity, we only respond to touch events at 2Hz
    UINT32 TimeNow = FrmGetTime() * 1000;
    if(TimeNow - m_lastTouchTime < 500)
    {
        // Ignore this touch event
        return;
    }

    // If touch is outside the screen, ignore it.
    if (xPos >= (unsigned int)m_pScenes[m_nCurrentScene]->m_nWidth ||
        yPos >= (unsigned int)m_pScenes[m_nCurrentScene]->m_nHeight)
    {
        FrmLogMessage("Ignoring touch event that is outside screen area");
        return;
    }


    // Handle the touch event
    m_lastTouchTime = TimeNow;

    FRMVECTOR2 vPointerPosition;
    vPointerPosition.x = +2.0f * (FLOAT32)xPos/(FLOAT32)(m_pScenes[m_nCurrentScene]->m_nWidth -1) - 1.0f;
    vPointerPosition.y = -2.0f * (FLOAT32)yPos/(FLOAT32)(m_pScenes[m_nCurrentScene]->m_nHeight-1) + 1.0f;


    m_pScenes[m_nCurrentScene]->m_Input.m_vPointerPosition = vPointerPosition;
    m_pScenes[m_nCurrentScene]->m_Input.m_nPointerState = FRM_INPUT::POINTER_DOWN | FRM_INPUT::POINTER_PRESSED;
}

//////////////////////////////////////////////////////////////////////////
void CApplication::ParseCommandLine(char **argvList, UINT32 argc)
{
    // we have command line arguments
    for (UINT32 idx = 0; idx < argc; ++idx)
    {
        if (!strcmp(argvList[idx], "shader"))   // shader initialization
        {
            // next argument is the shader we should initialize with
            m_nCurrentScene = atoi(argvList[idx+1]);
            idx++;
        } 
        else if (!strcmp(argvList[idx], "cd")) // current directory
        {
            // next argument is the desired current directory
            // call platform specific
            // NOTE: THIS WILL FAIL IF SPACES ARE IN THE PATH NAME WITHOUT QUOTES
            //SetCurrentDirectory(argvList[idx+1]);
            m_bIsWorkingDirSet = TRUE;
            idx++;
        }
        else if (!strcmp(argvList[idx], "savefb"))    // save the frame buffer
        {
            // next argument is the name of the result file (no extension assumed)
            strcpy(m_strImageResultFile, argvList[idx+1]);
            m_bSaveImageResult = TRUE;
            idx++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void CApplication::SaveFrameToTGA()
{
    BYTE *pImage = NULL;
    pImage = (BYTE*)malloc(m_ScreenWidth * m_ScreenHeight * sizeof(BYTE)*3);
    glReadPixels(0, 0, m_ScreenWidth, m_ScreenHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage);
   
    // Convert the image from BGR to RGB
    BYTE* p = (BYTE*)pImage;
    for( UINT32 y=0; y<m_ScreenHeight; y++ )
    {
        for( UINT32 x=0; x<m_ScreenWidth; x++ )
        {
            BYTE temp = p[2]; 
            p[2] = p[0];
            p[0] = temp;
            p += 3;
        }
    }

    FrmSaveImageAsTGA(m_strImageResultFile, m_ScreenWidth, m_ScreenHeight, (UINT32*)pImage);
    m_bSaveImageResult = FALSE;         // turn off flag so we don't go here
    free(pImage);
}


