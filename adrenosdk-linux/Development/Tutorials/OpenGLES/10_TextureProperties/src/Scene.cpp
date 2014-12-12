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
#include <FrmFile.h>
#include "Scene.h"

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif

// Texture coordinate that don't wrap
FLOAT VertexTexcoord[] = {0.5f, 1.0f,    
						  0.0f, 0.0f,    
                          1.0f, 0.0f};
    // Texture coordinate that  wrap go beyond 1
FLOAT VertexTexcoord2[] = {0.0f,  2.0f,    
                           -2.0f,-2.0f,    
                           2.0f, -2.0f};

const CHAR*  g_strImageFileName[NUMBER_OF_TEXTURES] = 
	{   "Texture1d.tga",
        "Texture1.tga", 
        "Texture2.tga", 
        "Texture3.tga", 
	    "Texture4.tga", 
        "Texture5.tga", 
        "Texture6.tga", 
        "Texture7.tga", 
        "Texture8.tga"} ;

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Texture Properties" );
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

    g_hShaderProgram = 0;
    g_VertexLoc = 0;
    g_TexcoordLoc = 1;
    g_strImageFilePath = "Tutorials/OpenGLES/";
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

    return true;
}


//--------------------------------------------------------------------------------------
// Name: MakeTriangle()
// Desc: Makes vertices for triangles to be shown across the screen
//--------------------------------------------------------------------------------------
void CSample::MakeTriangle(float startX, float startY, float fSize, float* destination)
{
	destination[0] = startX + (0.5f * fSize);
	destination[1] = startY;
	destination[2] = 0.0f;
	destination[3] = 1.0f;

	destination[4] = startX;
	destination[5] = startY - fSize;
	destination[6] = 0.0f;
	destination[7] = 1.0f;

	destination[8] = startX + fSize;
	destination[9] = startY - fSize;
	destination[10] = 0.0f;
	destination[11] = 1.0f;	
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

  // Load the textures
    {
        UINT32 nWidth, nHeight, nFormat;
        BYTE* pImageData;
		CHAR g_strTempFileName[512] = "";
		glGenTextures( NUMBER_OF_TEXTURES, g_hTextureHandle );

		for (INT32 i = 0; i < NUMBER_OF_TEXTURES; ++i)
		{

			strcpy(g_strTempFileName, g_strImageFilePath);
			strcat(g_strTempFileName, g_strImageFileName[i]);			
			pImageData = FrmUtils_LoadTGA( g_strTempFileName, &nWidth, &nHeight, &nFormat );
			if( NULL == pImageData )
				return FALSE;	        
			glBindTexture( GL_TEXTURE_2D, g_hTextureHandle[i] );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
						  0, nFormat, GL_UNSIGNED_BYTE, pImageData );
			delete[] pImageData;
		}
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

	// Create Triangle samples
	{
		FLOAT fStartX = 0.6f * -g_fAspectRatio;
		FLOAT fStartY = 1.0f;
		FLOAT fX = fStartX;
		FLOAT fY = fStartY;

		FLOAT fSize = 0.45f;
	    
		MakeTriangle( fX, fY, fSize, VertexPositions[0]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[1]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[2]);
		fX = fStartX; fY -= 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[3]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[4]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[5]);
		fX = fStartX; fY -= 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[6]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[7]);
		fX += 1.5f * fSize;
		MakeTriangle( fX, fY, fSize, VertexPositions[8]);
		fX = fStartX; fY -= 1.5f * fSize;
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
    glDeleteTextures( 9, g_hTextureHandle );
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
// Name: BasicRender
// Desc: The common rendering code for all the different properties
//--------------------------------------------------------------------------------------
VOID CSample::BasicRender(INT32 i, FLOAT* VertexTexcoord)
{
	glBindTexture( GL_TEXTURE_2D, g_hTextureHandle[i%NUMBER_OF_TEXTURES] );
	// Draw the textured triangle
	glVertexAttribPointer( g_VertexLoc, 4, GL_FLOAT, 0, 0, VertexPositions[i] );
	glEnableVertexAttribArray( g_VertexLoc );

	glVertexAttribPointer( g_TexcoordLoc, 2, GL_FLOAT, 0, 0, VertexTexcoord);
	glEnableVertexAttribArray( g_TexcoordLoc );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 3 );
}

//--------------------------------------------------------------------------------------
// Name: Render(____)
// Desc: These different functions render a triangle with  a different property.  
//		This is done to show how similar the actual code is for all the different 
//		accesses, its often just a state difference.
//
//		VertexTexcoord2 is in the same format as VertexTexcoord but goes beyond 
//		the range of (0..1) to demonstrate wrapping.
//
//--------------------------------------------------------------------------------------
VOID CSample::RenderNonPowerof2Texture(INT32 i)
{
	BasicRender( i, VertexTexcoord);
}
VOID CSample::Render1DTexture(INT32 i)
{
	BasicRender( i, VertexTexcoord);
}
VOID CSample::RenderWrapRepeat(INT32 i)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	BasicRender( i, VertexTexcoord2);
}
VOID CSample::RenderWrapMirror(INT32 i)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	BasicRender( i, VertexTexcoord2);
}
VOID CSample::RenderWrapClamp(INT32 i)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	BasicRender( i, VertexTexcoord2);
}
VOID CSample::RenderWrapCombo(INT32 i)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	BasicRender( i, VertexTexcoord2);
}
VOID CSample::Render2DTexture(INT32 i)
{
	BasicRender( i, VertexTexcoord2);
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.62f, 0.60f, 0.59f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );
	
	Render1DTexture( 0 );
	RenderNonPowerof2Texture( 1 );	
	RenderWrapRepeat( 2 );
	RenderWrapMirror( 3 );
	RenderWrapClamp( 4 );
	RenderWrapCombo( 5 );
	Render2DTexture( 6 );
	Render2DTexture( 7 );
	Render2DTexture( 8 );

    glDisableVertexAttribArray( g_VertexLoc );
    glDisableVertexAttribArray( g_TexcoordLoc );
}

