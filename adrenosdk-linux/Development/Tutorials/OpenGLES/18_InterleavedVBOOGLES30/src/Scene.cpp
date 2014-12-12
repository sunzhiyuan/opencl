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

FLOAT VertexColors[] = {1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 1.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f,
                        0.5f, 0.5f, 0.5f, 1.0f
                       };
UINT16 VertexIndices[] = { 0, 2, 1, 3 };

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "InterleavedVBOOGLES30" );
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
    fSize = 0.15f;

    pVbuff = NULL;

    g_hShaderProgram = 0;
    g_VertexLoc      = 0;
    g_ColorLoc       = 1;

    VertexPositions[0] = -fSize;
    VertexPositions[1] = +fSize*g_fAspectRatio;
    VertexPositions[2] =  0.0f;
    VertexPositions[3] =  1.0f;
    VertexPositions[4] = -fSize;
    VertexPositions[5] = -fSize*g_fAspectRatio;
    VertexPositions[6] = 0.0f;
    VertexPositions[7] = 1.0f;
    VertexPositions[8] =  +fSize;
    VertexPositions[9] = +fSize*g_fAspectRatio;
    VertexPositions[10]= 0.0f;
    VertexPositions[11]= 1.0f;
    VertexPositions[12]=  +fSize;
    VertexPositions[13]= -fSize*g_fAspectRatio;
    VertexPositions[14]= 0.0f;
    VertexPositions[15]= 1.0f;
    
    nNumVertices  = (sizeof(VertexPositions) / sizeof(FLOAT)) / 4; // Vertices have 4 components
    nNumIndices   = sizeof(VertexIndices) / sizeof(UINT16);
    nVsize        = sizeof(VertexPositions);
    nCsize        = sizeof(VertexColors);
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
	"#version 300 es														        \n"
    "in vec4 g_vVertex;											                    \n"
    "in vec4 g_vColor;											                    \n"
    "out   vec4 g_vVSColor;                                                         \n"
    "																	            \n"
    "void main()														            \n"
    "{   																            \n"
    "    float InstanceIDf = float(gl_InstanceID);                                  \n"
    // play around with the instance value to place the render target previews 
    "	 gl_Position.x = (g_vVertex.x + 0.15) - InstanceIDf * 0.3;                  \n"
	"    if(gl_InstanceID > 1)							                            \n"
	"    {												                            \n"
	"		gl_Position.x = (g_vVertex.x - 0.15) + (InstanceIDf - 2.0) * 0.3;       \n"
	"		gl_Position.y = g_vVertex.y - 0.5;                                      \n"
	"	 }												                            \n"
	"    else				                                                        \n"
	"		gl_Position.y = g_vVertex.y;					                        \n"
	"                                                                               \n"
	"    gl_Position.z = g_vVertex.z;                                               \n"
	"    gl_Position.w = g_vVertex.w;                                               \n"
    "                                                                               \n"        
    "   // gl_Position  = vec4( g_vVertex.x, g_vVertex.y,                           \n"
    "   //                      g_vVertex.z, g_vVertex.w );                         \n"
    "    g_vVSColor = g_vColor;                                                     \n"
    "}																	            \n";

    g_strFSProgram = 
	"#version 300 es													 \n"
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "in        vec4 g_vVSColor;                                          \n"
    "out vec4 out_color; 												 \n"
    "void main()														 \n"
    "{																	 \n"
    "    out_color    = g_vVSColor;                                      \n"
    "}																	 \n";

    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: InitVertexAttributesData()
// Desc: Initialize the vertex attributes data - store all attributes in a single array,
//       one after another. ie: pos1, color1, pos2, color2, ... etc.
//--------------------------------------------------------------------------------------
VOID CSample::InitVertexAttributesData()
{
    // allocate memory for 1 vertex including all its attributes.
    pVbuff = new VERTEX_ATTRIBUTES_DATA;


    pVbuff->nTotalSizeInBytes = nVsize + nCsize;
    pVbuff->pVertices = (POS *)new CHAR[pVbuff->nTotalSizeInBytes]; // this holds all the attributes

    // compute offsets
    pVbuff->pPosOffset = NULL; // Null bec' this is the first attribute in the array
    pVbuff->pColorOffset = (UINT8*)((UINT8*)pVbuff->pPosOffset + sizeof(POS));

    // stride = # of bytes before the first attribute repeats again
    pVbuff->nStride = sizeof(POS) + sizeof(COLOR);

    // populate our array with vertex data
    INT32 i = 0, vj = 0, cj = 0;
    FLOAT* pBuff = (FLOAT *)pVbuff->pVertices;

    for (i=0; i<nNumVertices; i++)
    {
        // vertices 
        *pBuff++ = VertexPositions[i+vj]; vj++; // x
        *pBuff++ = VertexPositions[i+vj]; vj++; // y
        *pBuff++ = VertexPositions[i+vj]; vj++; // z
        *pBuff++ = VertexPositions[i+vj];       // w

        // colors
        *pBuff++ = VertexColors[i+cj]; cj++; // r
        *pBuff++ = VertexColors[i+cj]; cj++; // g
        *pBuff++ = VertexColors[i+cj]; cj++; // b
        *pBuff++ = VertexColors[i+cj];       // a
    }
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

    // Initialize the vertex attributes data
    InitVertexAttributesData();

	// initialize vao (required for OSX)
	{
		GLuint TempId;
		glGenVertexArrays(1, &TempId);
		nVaoId = TempId;
	}

    // now initialize the vbo
    {
        GLuint TempId[2];
        glGenBuffers(2, TempId); // need 2 ids, one for the indices, and one for the vertex data
        nPosVBOId = TempId[0]; nIndexVBOId = TempId[1];

        // 1 vbo for all vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, nPosVBOId);
        glBufferData(GL_ARRAY_BUFFER, pVbuff->nTotalSizeInBytes, pVbuff->pVertices, GL_STATIC_DRAW);

        // 1 vbo for vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nIndexVBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VertexIndices), VertexIndices, GL_STATIC_DRAW);
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
        glBindAttribLocation(g_hShaderProgram, g_VertexLoc, "g_vVertex");
        glBindAttribLocation(g_hShaderProgram, g_ColorLoc, "g_vColor");

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
    glDeleteProgram( g_hShaderProgram );
    delete [] pVbuff->pVertices;
    delete pVbuff;
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
    // Clear the backbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );

	glBindVertexArray(nVaoId);

    // Draw the colored quad
    glBindBuffer( GL_ARRAY_BUFFER, nPosVBOId );

    glVertexAttribPointer( g_VertexLoc, 4, GL_FLOAT, 0, pVbuff->nStride, 0 );
    glEnableVertexAttribArray( g_VertexLoc );

    glVertexAttribPointer( g_ColorLoc, 4, GL_FLOAT, 0, pVbuff->nStride, (void*)16);
	glEnableVertexAttribArray( g_ColorLoc );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, nIndexVBOId);
    
    // divide by 1, 2 or 4 and see what is happening
    glVertexAttribDivisor(g_ColorLoc, 1);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, nNumIndices, GL_UNSIGNED_SHORT, NULL, 4);

    glDisableVertexAttribArray( g_VertexLoc );
    glDisableVertexAttribArray( g_ColorLoc );
	glBindBuffer( GL_ARRAY_BUFFER, 0);	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

