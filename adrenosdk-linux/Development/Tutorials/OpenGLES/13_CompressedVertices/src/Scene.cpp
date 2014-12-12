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
    return new CSample( "Compressed Vertices" );
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

    g_hTextureHandle      = 0;
    g_hShaderProgram      = 0;
    g_hModelViewMatrixLoc = 0;
    g_hProjMatrixLoc      = 0;
    
    g_strImageFileName = "Tutorials/OpenGLES/TreeBark.tga";
    
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
    "attribute vec3 g_vNormal;	 								         \n"
    "attribute vec4 g_vColor; 			   		 		                 \n"
    "attribute vec2 g_vTexcoord;		   		 		                 \n"
    "                                                                    \n"
    "varying   vec4 g_vVSColor;                                          \n"
    "varying   vec2 g_vVSTexcoord;                                       \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    vec4 vPositionES = g_matModelView * g_vPosition;                          \n"
    "    vec3 vNormalES   = normalize( g_matModelView * vec4(g_vNormal,0.0) ).xyz; \n"
    "    gl_Position  = g_matProj * vPositionES;                                   \n"
    "    float fDiffuse = dot( vNormalES, normalize( vec3(0.5,1.0,1.0) ) );        \n"
    "    g_vVSColor = g_vColor * fDiffuse;                                         \n"
    "    g_vVSTexcoord = g_vTexcoord;                                              \n"
    "}															             	   \n";

    g_strFragmentShader = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "uniform   sampler2D g_sImageTexture;                                \n"
    "varying   vec4      g_vVSColor;                                     \n"
    "varying   vec2      g_vVSTexcoord;                                  \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = g_vVSColor * texture2D(g_sImageTexture, g_vVSTexcoord);    \n"
    "}																	 \n";
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: CompressFloatToHalfFloat()
// Desc: Helper function to compress vertex data
//--------------------------------------------------------------------------------------
FLOAT16 CSample::CompressFloatToHalfFloat( float v )
{
    UINT32 src = *(unsigned long*)(&v);
    UINT32 sign = src >> 31;

    // Extract mantissa
    UINT32 mantissa = src  &  (1 << 23) - 1;

    // extract & convert exp bits
    long exp = (long)( src >> 23  &  0xFF ) - 127;
    if( exp > 16 )
    {
        // largest possible number...
        exp = 16;
        mantissa = (1 << 23) - 1;
    }
    else if( exp <= -15 )
    {
        // handle wraparound of shifts (done mod 64 on PPC)
        // All float32 denorm/0 values map to float16 0
        if( exp <= - 14 - 24 )
            mantissa = 0;
        else
            mantissa = ( mantissa | 1 << 23 )  >>  (-exp - 14);
        exp = -15;
    }

    return (FLOAT16)(sign<<15) | (FLOAT16)((exp+15)<<10) | (FLOAT16)(mantissa>>13);
}

//--------------------------------------------------------------------------------------
// Name: CompressVector3to10_10_10()
// Desc: Helper function to compress vertex data
//--------------------------------------------------------------------------------------
INT_10_10_10 CSample::CompressVector3to10_10_10( float* v )
{
    return ( ( ( (unsigned long)(v[0]*511.0f) ) & 0x000003ff ) <<  0L ) |
           ( ( ( (unsigned long)(v[1]*511.0f) ) & 0x000003ff ) << 10L ) |
           ( ( ( (unsigned long)(v[2]*511.0f) ) & 0x000003ff ) << 20L );
}


//--------------------------------------------------------------------------------------
// Name: CompressVector3to8_8_8_8()
// Desc: Helper function to compress vertex data
//--------------------------------------------------------------------------------------
UINT_8_8_8_8 CSample::CompressVector3to8_8_8_8( float* v )
{
    return ( ( ( (unsigned long)(v[0]*255.0f) ) & 0x000000ff ) <<  0L ) |
           ( ( ( (unsigned long)(v[1]*255.0f) ) & 0x000000ff ) <<  8L ) |
           ( ( ( (unsigned long)(v[2]*255.0f) ) & 0x000000ff ) << 16L ) |
           ( ( ( (unsigned long)(1.0f*255.0f) ) & 0x000000ff ) << 24L );
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
        BYTE*  pImageData = FrmUtils_LoadTGA( g_strImageFileName, &nWidth, &nHeight, &nFormat );
        if( NULL == pImageData )
            return FALSE;

        glGenTextures( 1, &g_hTextureHandle );
        glBindTexture( GL_TEXTURE_2D, g_hTextureHandle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
                      0, nFormat, GL_UNSIGNED_BYTE, pImageData );
        delete[] pImageData;
    }

    // Create the shader program needed to render the scene
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
        glBindAttribLocation( g_hShaderProgram, g_hColorLoc,    "g_vColor" );
        glBindAttribLocation( g_hShaderProgram, g_hTexCoordLoc, "g_vTexCoord" );

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

    // Compress the vertex data
    {
        for( int i=0; i<g_nNumVertices; i++ )
        {
            UNCOMPRESSED_VERTEX* pSrcVertex = &g_UnCompressedVertexData[i];
            COMPRESSED_VERTEX*   pDstVertex = &g_CompressedVertexData[i];

            pDstVertex->vPosition[0] = CompressFloatToHalfFloat( pSrcVertex->vPosition[0] );
            pDstVertex->vPosition[1] = CompressFloatToHalfFloat( pSrcVertex->vPosition[1] );
            pDstVertex->vPosition[2] = CompressFloatToHalfFloat( pSrcVertex->vPosition[2] );
            pDstVertex->vNormal = CompressVector3to10_10_10( pSrcVertex->vNormal );
            pDstVertex->vColor  = CompressVector3to8_8_8_8( pSrcVertex->vColor );
            pDstVertex->vTexCoord[0] = CompressFloatToHalfFloat( pSrcVertex->vTexCoord[0] );
            pDstVertex->vTexCoord[1] = CompressFloatToHalfFloat( pSrcVertex->vTexCoord[1] );
        }
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

    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set some state
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Set the shader program
    glUseProgram( g_hShaderProgram );
    glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, FALSE, matModelView );
    glUniformMatrix4fv( g_hProjMatrixLoc,      1, FALSE, matProj );

    // Bind the vertex attributes
#ifdef USE_COMPRESSED_VERTICES
    // Use compressed vertices
    glVertexAttribPointer( g_hVertexLoc,   3, GL_HALF_FLOAT_OES,     GL_FALSE, sizeof(g_CompressedVertexData[0]), &g_CompressedVertexData[0].vPosition );
    glVertexAttribPointer( g_hNormalLoc,   4, GL_INT_2_10_10_10_REV, GL_TRUE,  sizeof(g_CompressedVertexData[0]), &g_CompressedVertexData[0].vNormal );
    glVertexAttribPointer( g_hColorLoc,    4, GL_UNSIGNED_BYTE,      GL_TRUE,  sizeof(g_CompressedVertexData[0]), &g_CompressedVertexData[0].vColor );
    glVertexAttribPointer( g_hTexCoordLoc, 2, GL_HALF_FLOAT_OES,     GL_FALSE, sizeof(g_CompressedVertexData[0]), &g_CompressedVertexData[0].vTexCoord );
#else
    // Use uncompressed vertices
    glVertexAttribPointer( g_hVertexLoc,   3, GL_FLOAT, GL_FALSE, sizeof(g_UnCompressedVertexData[0]), &g_UnCompressedVertexData[0].vPosition );
    glVertexAttribPointer( g_hNormalLoc,   3, GL_FLOAT, GL_FALSE, sizeof(g_UnCompressedVertexData[0]), &g_UnCompressedVertexData[0].vNormal );
    glVertexAttribPointer( g_hColorLoc,    3, GL_FLOAT, GL_FALSE, sizeof(g_UnCompressedVertexData[0]), &g_UnCompressedVertexData[0].vColor );
    glVertexAttribPointer( g_hTexCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(g_UnCompressedVertexData[0]), &g_UnCompressedVertexData[0].vTexCoord );
#endif

    glEnableVertexAttribArray( g_hVertexLoc );
    glEnableVertexAttribArray( g_hNormalLoc );
    glEnableVertexAttribArray( g_hColorLoc );
    glEnableVertexAttribArray( g_hTexCoordLoc );

    // Draw the cubemap-reflected triangle
    glDrawArrays( GL_TRIANGLES, 0, 12 );

    // Cleanup
    glDisableVertexAttribArray( g_hVertexLoc );
    glDisableVertexAttribArray( g_hNormalLoc );
    glDisableVertexAttribArray( g_hColorLoc );
    glDisableVertexAttribArray( g_hTexCoordLoc );
}

