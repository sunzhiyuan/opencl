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
#include <GLES3/gl3.h>
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
    return new CSample( "Texture2DArrayOGLES30" );
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



    g_strImageFileNames[0] = "Tutorials/OpenGLES/TreeBark.tga";
    g_strImageFileNames[1] = "Tutorials/OpenGLES/Clouds.tga";
    g_hTextureHandle = 0;
    g_hShaderProgram = 0;
    g_VertexLoc      = 0;
    g_TexcoordLoc    = 1;
    g_nImages = 2; 
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    g_strVSProgram = 
	"#version 300 es										\n"
    "in vec4 g_vVertex;								\n"
    "in vec4 g_vTexcoord;							\n"
    "														\n"
    "out   vec4 g_vVSTexcoord;							\n"
    "														\n"
    "void main()											\n"
    "{														\n"
    "    gl_Position  = vec4( g_vVertex.x, g_vVertex.y,		\n"
    "                         g_vVertex.z, g_vVertex.w );	\n"
    "    g_vVSTexcoord = g_vTexcoord;						\n"
    "}														\n";

    g_strFSProgram = 
	"#version 300 es														\n"
    "#ifdef GL_FRAGMENT_PRECISION_HIGH										\n"
    "   precision highp float;												\n"
    "#else																	\n"
    "   precision mediump float;											\n"
    "#endif																	\n"
    "																		\n"
    "uniform sampler2DArray	g_sImageTexture;                            	\n"
    "in vec4 g_vVSTexcoord;						                		    \n"
    "																		\n"
    "out vec4 out_color;                                                    \n"
    "void main()															\n"
    "{																		\n"
		// When the x component of the texcoord transitions past 0.5, the
		//	second texture will be rendered to the triangle in the window.
    "    out_color = texture(g_sImageTexture, vec3(g_vVSTexcoord.xyx));	    \n"
    "}																		\n";
    
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
        UINT32 nWidth[2], nHeight[2], nFormat[2];
        BYTE*  pImageData[2];
        
        for(int i = 0; i<g_nImages; i++)
        {

			pImageData[i] = FrmUtils_LoadTGA( g_strImageFileNames[i], &nWidth[i], &nHeight[i], &nFormat[i] );
			if( NULL == pImageData[i] )
				return FALSE;
		}
		
		GLint internalFormat = (nFormat[0]==GL_RGB)? GL_RGB8 : GL_RGBA8;
	
		glGenTextures( 1, &g_hTextureHandle );
        glBindTexture( GL_TEXTURE_2D_ARRAY, g_hTextureHandle );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        
        // Use glTexImage3D to initialize the 2D Texture Array
        glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, internalFormat, nWidth[0], nHeight[0],
                       g_nImages, 0, nFormat[0], GL_UNSIGNED_BYTE, NULL );
                       
        // Use glTexSubImage3D to load each individual texture into the array
        for( int i = 0; i<g_nImages; i++)
        {
			//	the 5th argument specifies the array slice, and depth should be set to 1
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, nWidth[i], nHeight[i],
							1, nFormat[i], GL_UNSIGNED_BYTE, pImageData[i]);
		}
                      
        for( int i = 0; i<g_nImages; i++)
			delete[] pImageData[i];
    }

    // Create the shader program needed to render the texture
    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strVSProgram, NULL );
        glCompileShader( hVertexShader );

        // Check for compile success
        GLint nCompileResult = 0;
	    glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &nCompileResult);
	    if (!nCompileResult)
	    {
		    CHAR Log[1024];
		    GLint nLength;
		    glGetShaderInfoLog(hVertexShader, 1024, &nLength, Log);
		    return FALSE;
	    }

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strFSProgram, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        nCompileResult = 0;
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
        -fSize,  +fSize*g_fAspectRatio, 0.0f, 1.0f,
        -fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
        
        -fSize,  +fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, +fSize*g_fAspectRatio, 0.0f, 1.0f,
        +fSize, -fSize*g_fAspectRatio, 0.0f, 1.0f,
    };
	FLOAT VertexTexcoord[] = {0.0f, 1.0f,
                              0.0f, 0.0f,
                              1.0f, 0.0f,
                              
                              0.0f, 1.0f,
                              1.0f, 1.0f,
                              1.0f, 0.0f
                             };

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );
    glBindTexture( GL_TEXTURE_2D_ARRAY, g_hTextureHandle );

    // Draw the textured triangle
    glVertexAttribPointer( g_VertexLoc, 4, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_VertexLoc );

	glVertexAttribPointer( g_TexcoordLoc, 2, GL_FLOAT, 0, 0, VertexTexcoord);
	glEnableVertexAttribArray( g_TexcoordLoc );

    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glDisableVertexAttribArray( g_VertexLoc );
    glDisableVertexAttribArray( g_TexcoordLoc );
}

