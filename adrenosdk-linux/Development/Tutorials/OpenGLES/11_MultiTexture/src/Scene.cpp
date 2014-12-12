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
#include <FrmUtils.h>
#include <OpenGLES/FrmShader.h>
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
    return new CSample( "Multi Texture" );
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

    g_hTexture0Handle = 0;
    g_hTexture1Handle = 0;
    g_hShaderProgram  = 0;
    g_VertexLoc       = 0;
    g_Texcoord0Loc    = 1;
    g_Texcoord1Loc    = 2;

    g_strImageFileName0 = "Tutorials/OpenGLES/TreeBark.tga";
    g_strImageFileName1 = "Tutorials/OpenGLES/Clouds.tga";  
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
  //--------------------------------------------------------------------------------------
// Name: g_strVSProgram / g_strFSProgram
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
    g_strVSProgram = 
    "attribute vec4 g_vVertex;											 \n"
    "attribute vec2 g_vTexcoord0;	  					 		         \n"
    "attribute vec2 g_vTexcoord1;	  					 		         \n"
    "                                                                    \n"
    "varying   vec2 g_vVSTexcoord0;                                      \n"
    "varying   vec2 g_vVSTexcoord1;                                      \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_Position  = vec4( g_vVertex.x, g_vVertex.y,                  \n"
    "                         g_vVertex.z, g_vVertex.w );                \n"
    "    g_vVSTexcoord0 = g_vTexcoord0;                                  \n"
    "    g_vVSTexcoord1 = g_vTexcoord1;                                  \n"
    "}																	 \n";

    g_strFSProgram = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "uniform   sampler2D g_sImageTexture0;                               \n"
    "uniform   sampler2D g_sImageTexture1;                               \n"
    "varying   vec2      g_vVSTexcoord0;                                 \n"
    "varying   vec2      g_vVSTexcoord1;                                 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = texture2D(g_sImageTexture0, g_vVSTexcoord0.xy)   \n"
    "                 * texture2D(g_sImageTexture1, g_vVSTexcoord1.xy);  \n"
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

    // Create the first texture, loaded from an image file
    {
        UINT32 nWidth, nHeight, nFormat;
        BYTE*  pImageData = FrmUtils_LoadTGA( g_strImageFileName0, &nWidth, &nHeight, &nFormat );
        if( NULL == pImageData )
            return FALSE;

        glGenTextures( 1, &g_hTexture0Handle );
        glBindTexture( GL_TEXTURE_2D, g_hTexture0Handle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
                      0, nFormat, GL_UNSIGNED_BYTE, pImageData );
        delete[] pImageData;
    }

    // Create the second texture, loaded from an image file
    {
        UINT32 nWidth, nHeight, nFormat;
        BYTE*  pImageData = FrmUtils_LoadTGA( g_strImageFileName1, &nWidth, &nHeight, &nFormat );
        if( NULL == pImageData )
            return FALSE;

        glGenTextures( 1, &g_hTexture1Handle );
        glBindTexture( GL_TEXTURE_2D, g_hTexture1Handle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
                      0, nFormat, GL_UNSIGNED_BYTE, pImageData );
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
        glBindAttribLocation(g_hShaderProgram, g_VertexLoc,    "g_vVertex");
        glBindAttribLocation(g_hShaderProgram, g_Texcoord0Loc, "g_vTexcoord0");
        glBindAttribLocation(g_hShaderProgram, g_Texcoord1Loc, "g_vTexcoord1");

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

        g_hTexture0Slot = glGetUniformLocation( g_hShaderProgram, "g_sImageTexture0" );
        g_hTexture1Slot = glGetUniformLocation( g_hShaderProgram, "g_sImageTexture1" );

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
    glDeleteTextures( 1, &g_hTexture0Handle );
    glDeleteTextures( 1, &g_hTexture1Handle );
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
   static float x = 0;
    x += 0.01f;

    FLOAT fSize = 0.5f;
    FLOAT VertexPositions[] =
    {
        -fSize, +fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, +fSize*g_fAspectRatio, 0.0f, 1.0f,
        -fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
    };
    FLOAT VertexTexcoord0[] = {0.0f, 1.0f,    
                               1.0f, 1.0f,    
                               0.0f, 0.0f,    
                               1.0f, 0.0f,    
                             };
    FLOAT VertexTexcoord1[] = {0.0f+x, 1.0f,    
                               1.0f+x, 1.0f,    
                               0.0f+x, 0.0f,    
                               1.0f+x, 0.0f,    
                             };

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, g_hTexture0Handle );
    glUniform1i( g_hTexture0Slot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, g_hTexture1Handle );
    glUniform1i( g_hTexture1Slot, 1 );

    // Draw the textured triangle
    glVertexAttribPointer( g_VertexLoc, 4, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_VertexLoc );

    glVertexAttribPointer( g_Texcoord0Loc, 2, GL_FLOAT, 0, 0, VertexTexcoord0 );
    glEnableVertexAttribArray( g_Texcoord0Loc );

    glVertexAttribPointer( g_Texcoord1Loc, 2, GL_FLOAT, 0, 0, VertexTexcoord1 );
    glEnableVertexAttribArray( g_Texcoord1Loc );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glDisableVertexAttribArray( g_VertexLoc );
    glDisableVertexAttribArray( g_Texcoord0Loc );
    glDisableVertexAttribArray( g_Texcoord1Loc );
}

