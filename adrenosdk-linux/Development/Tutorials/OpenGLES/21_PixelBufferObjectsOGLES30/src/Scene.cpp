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
    return new CSample( "RenderToTexture" );
}

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    g_strWindowTitle = "Pixel Buffer Objects";
    g_nWindowWidth   = 765;
    g_nWindowHeight  = 480;
    g_fAspectRatio   = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;
    g_strImageFileName = "Tutorials/OpenGLES/TreeBark.tga";
    g_nImages = 1;

    g_hTextureHandle = 0;
    g_hShaderProgram = 0;
    g_VertexLoc      = 0;
    g_TexcoordLoc    = 1;
    g_hPbos[0]	 = 0;
    g_hPbos[1]    =0;

    g_imageData		= 0;	// Unused right now, will be used if adding PBO on/off functionality for testing
    IMAGE_WIDTH		= 512;
    IMAGE_HEIGHT		= 512;
    CHANNEL_COUNT		= 4;
    DATA_SIZE			= IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;
    INTERNAL_FORMAT	= GL_RGBA8;
    DATA_TYPE			= GL_UNSIGNED_BYTE;
    FORMAT				= GL_RGBA;

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
	"#version 300 es     									\n"
    "in        vec4 g_vVertex;								\n"
    "in        vec4 g_vTexcoord;							\n"
    "														\n"
    "out     vec4 g_vVSTexcoord;							\n"
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
    "uniform   sampler2D	g_sImageTexture;	                        	\n"
    "in     vec4			g_vVSTexcoord;									\n"
    "out vec4 out_color;													\n"
    "void main()															\n"
    "{																		\n"
		// When the x component of the texcoord transitions past 0.5, the
		//	second texture will be rendered to the triangle in the window.
    "    out_color   = texture(g_sImageTexture, vec2(g_vVSTexcoord.xy));	\n"
    "}																		\n";
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: UpdatePixels()
// Desc: Manipulate the pixels at dst
//--------------------------------------------------------------------------------------
void CSample::UpdatePixels(UCHAR *dst, int size)
{
    static int color = 0;

    if(!dst)
        return;

    int* ptr = (int*)dst;

    // copy 4 bytes at once
    for(int i = 0; i < IMAGE_HEIGHT; ++i)
    {
        for(int j = 0; j < IMAGE_WIDTH; ++j)
        {
            *ptr = color;
            ++ptr;
        }
        color += 257;   // add an arbitary number (no meaning)
    }
    ++color;            // scroll down
}    




//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

     // Generate the texture that will be used for rendering
	glGenTextures( 1, &g_hTextureHandle );
	glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// Use glTexImage2D to initialize the 2D Texture
	glTexImage2D( GL_TEXTURE_2D, 0, INTERNAL_FORMAT, IMAGE_WIDTH, IMAGE_HEIGHT,
				   0, FORMAT, DATA_TYPE, 0);
				   
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	// Create the Pixel Buffers
    glGenBuffers(2, g_hPbos);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_hPbos[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_hPbos[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

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

	// Bind the vertex attributes
	glGenVertexArrays(1, &g_hVao);
	glBindVertexArray(g_hVao);

	glGenBuffers(1, &g_hPositionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hPositionVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 24, VertexPositions, GL_DYNAMIC_DRAW);
	glVertexAttribPointer( g_VertexLoc, 3, GL_FLOAT, false, 16, 0 );
	glEnableVertexAttribArray( g_VertexLoc );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_hTexcoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hTexcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 12, VertexTexcoord, GL_DYNAMIC_DRAW);
	glVertexAttribPointer( g_TexcoordLoc, 3, GL_FLOAT, false, 8, 0 );
	glEnableVertexAttribArray( g_TexcoordLoc );
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: UpdateTexture()
// Desc: Update the texture with the last updated PBO and manipulate the pixels
//		 in the other PBO.
//--------------------------------------------------------------------------------------
VOID CSample::UpdateTexture()
{
	static int index = 0;
	int nextIndex = 0;
	
	// switch which PBO is getting updated vs getting its data transferred to
	//	the texture
	index = (index+1)%2;
	nextIndex = (index+1)%2;
	
	// write from PBO into texture -> bind both the texture and PBO
	glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, g_hPbos[index] );
	
	// copy the contents of the PBO into the texture
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0,
					 IMAGE_WIDTH, IMAGE_HEIGHT, FORMAT, DATA_TYPE, 0 );
					
	// write into PBO with UpdatePixels -> bind the other PBO to update its pixelvalues
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, g_hPbos[nextIndex] );
	
	glBufferData( GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW );
	glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, DATA_SIZE, GL_MAP_WRITE_BIT );
	
	// Use glGetBufferPointerv to get a pointer to the mapped PBO data
	UCHAR *ptr = NULL;
	glGetBufferPointerv( GL_PIXEL_UNPACK_BUFFER, GL_BUFFER_MAP_POINTER, (GLvoid **)&ptr );
	
	if( ptr )
	{
		// update the PBO's pixel data
		UpdatePixels( ptr, DATA_SIZE );
		glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
	}
	
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
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
	glDeleteBuffers( 2, g_hPbos );
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

	// Update the texture and also update the other PBO
	UpdateTexture();

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );
    glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );

    glBindVertexArray(g_hVao);

	glDrawArrays( GL_TRIANGLES, 0, 6 );

    glBindVertexArray(0);
}

