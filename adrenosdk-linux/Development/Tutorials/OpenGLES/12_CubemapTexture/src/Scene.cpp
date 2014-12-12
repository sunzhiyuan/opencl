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
    return new CSample( "Cubemap Texture" );
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

    g_hCubemapHandle      = 0;
    g_hShaderProgram      = 0;
    g_hModelViewMatrixLoc = 0;
    g_hProjMatrixLoc      = 0;
    g_hVertexLoc          = 0;
    g_hNormalLoc          = 1;

    g_strImageFilePath[0] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_RT.tga";
    g_strImageFilePath[1] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_LF.tga";
    g_strImageFilePath[2] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_DN.tga";
    g_strImageFilePath[3] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_UP.tga";
    g_strImageFilePath[4] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_BK.tga";
    g_strImageFilePath[5] = "Tutorials/OpenGLES/12_CubemapTexture/BedroomCubeMap_FR.tga";
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    g_strVertexShader = 
    "uniform   mat4 g_matModelView;                                      \n"
    "uniform   mat4 g_matProj;                                           \n"
    "                                                                    \n"
    "attribute vec4 g_vPosition;	 								     \n"
    "attribute vec3 g_vNormal;	  					 		             \n"
    "                                                                    \n"
    "varying   vec3 g_vVSTexcoord;                                       \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    vec4 vPositionES = g_matModelView * g_vPosition;                     \n"
    "    vec3 vNormalES   = normalize( g_matModelView * vec4(g_vNormal,0.0) ).xyz; \n"
    "    gl_Position  = g_matProj * vPositionES;                              \n"
    "    vec3 vViewVecES = -vPositionES.xyz;                                  \n"
    "    g_vVSTexcoord = reflect( -vViewVecES, vNormalES );                   \n"
    "    g_vVSTexcoord.y = -g_vVSTexcoord.y;                                  \n"
    "}																   	      \n";

    g_strFragmentShader = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "uniform   samplerCube g_sCubemap;                                   \n"
    "                                                                    \n"
    "varying   vec3        g_vVSTexcoord;                                \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = textureCube( g_sCubemap, g_vVSTexcoord );        \n"
//	"    gl_FragColor = 0.5 * vec4( g_vVSTexcoord.rgb, 1.0 ) + 0.5;        \n"
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
        UINT32 nWidth, nHeight, nFormat;
        BYTE* pImageData[6];
        pImageData[0] = FrmUtils_LoadTGA( g_strImageFilePath[0], &nWidth, &nHeight, &nFormat );
        pImageData[1] = FrmUtils_LoadTGA( g_strImageFilePath[1], &nWidth, &nHeight, &nFormat );
        pImageData[2] = FrmUtils_LoadTGA( g_strImageFilePath[2], &nWidth, &nHeight, &nFormat );
        pImageData[3] = FrmUtils_LoadTGA( g_strImageFilePath[3], &nWidth, &nHeight, &nFormat );
        pImageData[4] = FrmUtils_LoadTGA( g_strImageFilePath[4], &nWidth, &nHeight, &nFormat );
        pImageData[5] = FrmUtils_LoadTGA( g_strImageFilePath[5], &nWidth, &nHeight, &nFormat );
        if( NULL == pImageData[0] || NULL == pImageData[1] || NULL == pImageData[2] )
            return FALSE;
        if( NULL == pImageData[3] || NULL == pImageData[4] || NULL == pImageData[5] )
            return FALSE;

        glGenTextures( 1, &g_hCubemapHandle );
        glBindTexture( GL_TEXTURE_CUBE_MAP, g_hCubemapHandle );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        for( UINT32 i=0; i<6; i++ )
        {
            glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, nFormat,
                          nWidth, nHeight, 0, nFormat, GL_UNSIGNED_BYTE, pImageData[i] );
        }

        delete[] pImageData[0];
        delete[] pImageData[1];
        delete[] pImageData[2];
        delete[] pImageData[3];
        delete[] pImageData[4];
        delete[] pImageData[5];
    }

    // Create the shader program needed to render the texture
    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strVertexShader, NULL );
        glCompileShader( hVertexShader );

        // Check for compile success
        GLint nCompileResult = 0;
        glGetShaderiv( hVertexShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hVertexShader, 1024, &nLength, strLog );
            return FALSE;
        }

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strFragmentShader, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        glGetShaderiv( hFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hFragmentShader, 1024, &nLength, strLog );
            return FALSE;
        }

        // Attach the individual shaders to the common shader program
        g_hShaderProgram = glCreateProgram();
        glAttachShader( g_hShaderProgram, hVertexShader );
        glAttachShader( g_hShaderProgram, hFragmentShader );

        // Init attributes BEFORE linking
        glBindAttribLocation( g_hShaderProgram, g_hVertexLoc,   "g_vPosition" );
        glBindAttribLocation( g_hShaderProgram, g_hNormalLoc,   "g_vNormal" );

        // Link the vertex shader and fragment shader together
        glLinkProgram( g_hShaderProgram );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv( g_hShaderProgram, GL_LINK_STATUS, &nLinkResult );
        if( 0 == nLinkResult )
        {
            CHAR strLog[1024];
            GLint nLength;
            glGetProgramInfoLog( g_hShaderProgram, 1024, &nLength, strLog );
            return FALSE;
        }

        // Get uniform locations
        g_hModelViewMatrixLoc = glGetUniformLocation( g_hShaderProgram, "g_matModelView" );
        g_hProjMatrixLoc      = glGetUniformLocation( g_hShaderProgram, "g_matProj" );

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
    glDeleteTextures( 1, &g_hCubemapHandle );
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
    static float fAngle = 0.0f;
    fAngle += 0.01f;

    // Rotate and translate the model view matrix
    float matModelView[16] = {0};
    matModelView[ 0] = +cosf( fAngle );
    matModelView[ 2] = +sinf( fAngle );
    matModelView[ 5] = 1.0f;
    matModelView[ 8] = -sinf( fAngle );
    matModelView[10] = +cosf( fAngle );
    matModelView[14] = -5.0f;
    matModelView[15] = 1.0f;

    // Build a perspective projection matrix
    float matProj[16] = {0};
    matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
    matProj[ 5] = matProj[0] * g_fAspectRatio;
    matProj[10] = -( 10.0f ) / ( 9.0f );
    matProj[11] = -1.0f;
    matProj[14] = -( 10.0f ) / ( 9.0f );

    // Define vertice for a 4-sided pyramid
    FLOAT VertexPositions[] =
    {
        0.0f,+1.0f, 0.0f,      -1.0f,-1.0f, 1.0f,      +1.0f,-1.0f, 1.0f,
        0.0f,+1.0f, 0.0f,      +1.0f,-1.0f, 1.0f,      +1.0f,-1.0f,-1.0f,
        0.0f,+1.0f, 0.0f,      +1.0f,-1.0f,-1.0f,      -1.0f,-1.0f,-1.0f,
        0.0f,+1.0f, 0.0f,      -1.0f,-1.0f,-1.0f,      -1.0f,-1.0f, 1.0f,
    };
    FLOAT VertexNormals[] =
    {
        0.0f, 0.5f, 1.0f,       0.0f, 0.5f, 1.0f,       0.0f, 0.5f, 1.0f,
        1.0f, 0.5f, 0.0f,       1.0f, 0.5f, 0.0f,       1.0f, 0.5f, 0.0f,
        0.0f, 0.5f,-1.0f,       0.0f, 0.5f,-1.0f,       0.0f, 0.5f,-1.0f,
       -1.0f, 0.5f, 0.0f,      -1.0f, 0.5f, 0.0f,      -1.0f, 0.5f, 0.0f,
    };

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set some state
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Set the shader program and the texture
    glUseProgram( g_hShaderProgram );
    glBindTexture( GL_TEXTURE_CUBE_MAP, g_hCubemapHandle );
    glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, FALSE, matModelView );
    glUniformMatrix4fv( g_hProjMatrixLoc,      1, FALSE, matProj );

    // Bind the vertex attributes
    glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_hVertexLoc );

    glVertexAttribPointer( g_hNormalLoc, 3, GL_FLOAT, 0, 0, VertexNormals );
    glEnableVertexAttribArray( g_hNormalLoc );

    // Draw the cubemap-reflected triangle
    glDrawArrays( GL_TRIANGLES, 0, 12 );

    // Cleanup
    glDisableVertexAttribArray( g_hVertexLoc );
    glDisableVertexAttribArray( g_hNormalLoc );
}

