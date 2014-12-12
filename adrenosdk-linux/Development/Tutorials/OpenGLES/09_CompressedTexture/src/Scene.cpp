//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include <FrmFile.h>
#include <FrmUtils.h>
#include "Scene.h"

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( " Compressed Texture " );
}

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    g_strWindowTitle = strName;
    g_nWindowWidth   = 765;
    g_nWindowHeight  = 480;
    g_fAspectRatio   = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;

    g_hTextureHandle = 0;
    g_hShaderProgram = 0;
    g_VertexLoc = 0;
    g_TexcoordLoc = 1;

    g_strImageFileName = "Tutorials/OpenGLES/RainbowFilm.atc";
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    g_strVSProgram = 
    "attribute vec4 g_vVertex;											 \n"
    "attribute vec4 g_vTexcoord;							 		     \n"
    "                                                                    \n"
    "varying   vec4 g_vVSTexcoord;                                       \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_Position  = vec4( g_vVertex.x, g_vVertex.y,                  \n"
    "                         g_vVertex.z, g_vVertex.w );                \n"
    "    g_vVSTexcoord = g_vTexcoord;                                    \n"
    "}																	 \n";

    g_strFSProgram = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "uniform   sampler2D g_sImageTexture;                                \n"
    "varying   vec4      g_vVSTexcoord;                                  \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = texture2D(g_sImageTexture, g_vVSTexcoord.xy);    \n"
    "}																	 \n";
    
    return TRUE;
}




//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

  // Create a texture, loaded from an image file
    {
        UINT32 nWidth, nHeight, nFormat, nSize;
        UINT8* pImageData = FrmUtils_LoadATC( g_strImageFileName, &nWidth, 
                                     &nHeight, &nFormat, &nSize );
        if( NULL == pImageData )
            return FALSE;

        glGenTextures( 1, &g_hTextureHandle );
        glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glCompressedTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
                                0, nSize, pImageData );
        delete[] pImageData;
    }

    // Create the shader program needed to render the texture
    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strVSProgram, NULL );
        glCompileShader( hVertexShader );

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strFSProgram, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        GLint nCompileResult = 0;
        glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &nCompileResult);
        if (!nCompileResult)
        {
            CHAR Log[1024];
            GLint nLength;
            glGetShaderInfoLog(hFragmentShader, 1024, &nLength, Log);
            return FALSE;
        }

        // Attach the individual shaders to the common shader program
        g_hShaderProgram = glCreateProgram();
        glAttachShader( g_hShaderProgram, hVertexShader );
        glAttachShader( g_hShaderProgram, hFragmentShader );

        // Init attributes BEFORE linking
        glBindAttribLocation(g_hShaderProgram, g_VertexLoc,   "g_vVertex");
        glBindAttribLocation(g_hShaderProgram, g_TexcoordLoc, "g_vTexcoord");

        // Link the vertex shader and fragment shader together
        glLinkProgram( g_hShaderProgram );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv(g_hShaderProgram, GL_LINK_STATUS, &nLinkResult);
        if (!nLinkResult)
        {
            CHAR Log[1024];
            GLint nLength;
            glGetProgramInfoLog(g_hShaderProgram, 1024, &nLength, Log);
            return FALSE;
        }

        glDeleteShader( hVertexShader );
        glDeleteShader( hFragmentShader );
    }

    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    glDeleteTextures( 1, &g_hTextureHandle );
    glDeleteProgram( g_hShaderProgram );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
   return;
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    FLOAT fSize = 0.5f;
    FLOAT VertexPositions[] =
    {
         0.0f,  +fSize*g_fAspectRatio, 0.0f, 1.0f,
        -fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
    };
    FLOAT VertexTexcoord[] = {0.5f, 1.0f,    
                              0.0f, 0.0f,    
                              1.0f, 0.0f    
                             };

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );
    glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );

    // Draw the textured triangle
    glVertexAttribPointer( g_VertexLoc, 4, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_VertexLoc );

    glVertexAttribPointer( g_TexcoordLoc, 2, GL_FLOAT, 0, 0, VertexTexcoord);
    glEnableVertexAttribArray( g_TexcoordLoc );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 3 );

    glDisableVertexAttribArray( g_VertexLoc );
    glDisableVertexAttribArray( g_TexcoordLoc );
}

