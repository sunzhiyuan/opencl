//--------------------------------------------------------------------------------------
// File: FrmUserInterfaceGLES.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef _FRM_USER_INTERFACE_GLES_H_
#define _FRM_USER_INTERFACE_GLES_H_

#include "FrmUserInterface.h"


//--------------------------------------------------------------------------------------
// Name: class CFrmUserInterfaceGLES
// Desc: Represents a sample's user interface components.
//--------------------------------------------------------------------------------------
class CFrmUserInterfaceGLES : public CFrmUserInterface
{
public:
    CFrmUserInterfaceGLES();
    virtual ~CFrmUserInterfaceGLES();

    // Initialize the user interface
    virtual BOOL Initialize( CFrmFont* pFont, const CHAR* strHeading );

protected:
    // Render functions
    virtual VOID RenderFadedBackground();
    virtual VOID RenderOverlays();

    // Fade help background shader variabes
    UINT32  m_hBackgroundShader;
    UINT32  m_hBackgroundColorLoc;

    UINT32  m_hOverlayShader;
    UINT32  m_hOverlayScreenSizeLoc;

};


#endif // _FRM_USER_INTERFACE_GLES_H_

