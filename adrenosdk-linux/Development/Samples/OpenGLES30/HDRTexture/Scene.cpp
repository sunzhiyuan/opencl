//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmFile.h>

#include <OpenGLES/FrmGLES3.h>  // OpenGL ES 3 headers here

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// Calculates log2 of number.  
float Log2( float n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2.0f );  
}
//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "HDR Texture" );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader::MaterialShader()
{
	ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_pLogoTexture = NULL;
	m_HDRTexture[0] = m_HDRTexture[1] = NULL;
	m_OldHDRTexture[0] = m_OldHDRTexture[1] = NULL;

	m_Exposure = 1.0f;

	m_nWidth = 800;
	m_nHeight = 480;

	m_9995Shader = true;

	m_OpenGLESVersion = GLES3;

}

//--------------------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Helper function to load a 32-bit TGA image file
//--------------------------------------------------------------------------------------
BYTE* LoadTGA( const CHAR* strFileName, UINT32* pWidth, UINT32* pHeight, UINT32* nFormat )
{
    struct TARGA_HEADER
    {
        BYTE   IDLength, ColormapType, ImageType;
        BYTE   ColormapSpecification[5];
        UINT16 XOrigin, YOrigin;
        UINT16 ImageWidth, ImageHeight;
        BYTE   PixelDepth;
        BYTE   ImageDescriptor;
    };
    
    static TARGA_HEADER header;
    
	// Read the TGA file
	FRM_FILE* pFile;
    if( FALSE == FrmFile_Open( strFileName, FRM_FILE_READ, &pFile ) )
        return NULL;

	FrmFile_Read( pFile, &header, sizeof(header) );
    UINT32 nPixelSize = header.PixelDepth / 8;
    (*pWidth)  = header.ImageWidth;
    (*pHeight) = header.ImageHeight;
    (*nFormat) = nPixelSize == 3 ? GL_RGB : GL_RGBA;

    BYTE* pBits = new BYTE[ nPixelSize * header.ImageWidth * header.ImageHeight ];
	FrmFile_Read( pFile, pBits, nPixelSize * header.ImageWidth * header.ImageHeight );
	FrmFile_Close( pFile );

    // Convert the image from BGRA to RGBA
    BYTE* p = pBits;
    for( UINT32 y=0; y<header.ImageHeight; y++ )
    {
        for( UINT32 x=0; x<header.ImageWidth; x++ )
        {
            BYTE temp = p[2]; p[2] = p[0]; p[0] = temp;
            p += nPixelSize;
        }
    }

    return pBits;
}
//--------------------------------------------------------------------------------------
// Name: RePack8888As5999Rev( BYTE* in, unsigned int inByteCount)
// Desc: Reads 'in' and repacks as 9_9_9_5Rev format memory on the heap and returns it
//--------------------------------------------------------------------------------------
BYTE* RePack8888As5999Rev( BYTE* in, unsigned int inByteCount)
{
	FrmAssert( inByteCount % 4 == 0);

	unsigned int* pData = new unsigned int[ inByteCount/4 ];

	unsigned int* pOut = pData;
	BYTE* pIn = in;

	for(unsigned int i=0; i<inByteCount; i+= 4)
	{
		//First uncompress the color 
		FRMVECTOR4 color;

		color.r = (float) pIn[0];
		color.g = (float) pIn[1];
		color.b = (float) pIn[2];
		color.a = (float) pIn[3];

		color.a = color.a / 255;//normalize alpha (just as is done automaticly on shader)

		float alpha = color.a;

		float midPoint = ( 104.0f / 255.0f );
		float brightness = powf( 2.0f, ( color.a - midPoint ) * 10.0f );

		color *= brightness;

		// Now re-compress color in a way OpenGL expects, see spec under "Encoding of Special Internal Formats"
		// sharedexp = ((2^N -1)/(2^N))*2^(Emax -B) 
		// where N is the number of mantissa bits per component, B is the exponent bias, and Emax is the max bits for bias

		const float pow2to9 = 512.0f;
		const float B = 7.0f;
		const float Emax = 31.0f;
		const float N = 9.0f;

		float sharedexp = 65408.000f;//(( pow2to9  - 1.0f)/ pow2to9)*powf( 2.0f, 31.0f - 15.0f);

		float cRed = max( 0.0f, min( sharedexp, color.r ));
		float cGreen = max( 0.0f, min( sharedexp, color.g ));
		float cBlue = max( 0.0f, min( sharedexp, color.b ));

		float cMax = max( cRed, max( cGreen, cBlue ));

		// expp = max(-B - 1, log2(maxc)) + 1 + B
		float expp = max( - B - 1.0f, floor(Log2( cMax ))) + 1.0f + B;

		float sMax = (float) floor(( cMax / powf( 2.0f, expp - B - N )) + 0.5f);

		float exps = expp + 1.0f;

		if( 0.0 <= sMax && sMax < pow2to9)
		{
			exps = expp;
		}

		float sRed = ( cRed / pow( 2.0f, exps - B - N)) + 0.5f;
		float sGreen = ( cGreen / pow( 2.0f, exps - B - N)) + 0.5f;
		float sBlue = ( cBlue / pow( 2.0f, exps - B - N)) + 0.5f;

		*pOut = ((short)(sRed) & 0x1FF) | (((short)(sGreen) & 0x1FF) << 9) | (((short)(sBlue) & 0x1FF) << 18) | (((short)(exps) & 0x1F) << 27);

		pOut++;
		pIn += 4;
	}

	return (BYTE*) pData;
}
//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
	FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
	{ 
		{ "g_PositionIn", FRM_VERTEX_POSITION }
	};
	const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);


	// Create shader for 9995Rev
	{
		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/HDRTextureShader30.vs", "Samples/Shaders/MaterialShader9995Rev30.fs",
			&m_MaterialShader9995.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_MaterialShader9995.TextureId = glGetUniformLocation( m_MaterialShader9995.ShaderId, "g_Texture" );
		m_MaterialShader9995.ExposureId = glGetUniformLocation( m_MaterialShader9995.ShaderId, "g_Exposure" );
	}
	// Create shader for 8 8 8 8
	{
		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/HDRTextureShader30.vs", "Samples/Shaders/HDRTextureShader30.fs",
			&m_MaterialShaderOld.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_MaterialShaderOld.TextureId = glGetUniformLocation( m_MaterialShader9995.ShaderId, "g_Texture" );
		m_MaterialShaderOld.ExposureId = glGetUniformLocation( m_MaterialShader9995.ShaderId, "g_Exposure" );
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
	// Load the packed resources
	if( FALSE == resource.LoadFromFile( "Samples/Textures/HDRTexture.pak" ) )
		return FALSE;

	m_OldHDRTexture[0] = resource.GetTexture( "HDRTexture1" );
	m_OldHDRTexture[1] = resource.GetTexture( "HDRTexture2" );

	BYTE* pTextureData[2];
	BYTE* pRepackedTextureData[2];

	UINT32 nWidth, nHeight, nFormat;
	pTextureData[0] = LoadTGA( "Samples/Textures/HDRTexture1.tga", &nWidth, &nHeight, &nFormat );
    if( NULL == pTextureData[0] )
		return FALSE;

	pRepackedTextureData[0] = RePack8888As5999Rev( pTextureData[0], 4 * nWidth * nHeight );

	delete [] pTextureData[0];

	glGenTextures   ( 1, &m_HDRTexture[0] );
	glBindTexture   ( GL_TEXTURE_2D, m_HDRTexture[0] );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D    ( GL_TEXTURE_2D, 0, GL_RGB9_E5, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, pRepackedTextureData[0] );

	delete [] pRepackedTextureData[0];

	pTextureData[1] = LoadTGA( "Samples/Textures/HDRTexture2.tga", &nWidth, &nHeight, &nFormat );
    if( NULL == pTextureData[1] )
		return FALSE;

	pRepackedTextureData[1] = RePack8888As5999Rev( pTextureData[1], 4 * nWidth * nHeight );

	delete [] pTextureData[1];

	glGenTextures   ( 1, &m_HDRTexture[1] );
	glBindTexture   ( GL_TEXTURE_2D, m_HDRTexture[1] );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D    ( GL_TEXTURE_2D, 0, GL_RGB9_E5, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, pRepackedTextureData[1] );

	delete [] pRepackedTextureData[1];

	// Create the logo texture
	m_pLogoTexture = resource.GetTexture( "Logo" );
	
	// Create the font
	if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
		return FALSE;

	// Setup the user interface
	if( FALSE == m_UserInterface.Initialize( &m_Font, "HDR" ) )
		return FALSE;
	
	m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, 
								m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
	m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
	m_UserInterface.AddFloatVariable( &m_Exposure, (char *)"Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Change Shader" );

	m_UserInterface.SetHeading("HDR Using GL_UNSIGNED_INT_5_9_9_9_REV Textures");

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

	return TRUE;

}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
	// Initialize the viewport
	glViewport( 0, 0, m_nWidth, m_nHeight );

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
	// Free shader memory
	m_MaterialShader9995.Destroy();
	m_MaterialShaderOld.Destroy();

	// Free texture memory
	if( m_pLogoTexture ) m_pLogoTexture->Release();
	if( m_OldHDRTexture[0] ) m_OldHDRTexture[0]->Release();
	if( m_OldHDRTexture[1] ) m_OldHDRTexture[1]->Release();
	
	resource.Destroy();
	
	m_Font.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
	// get the current time
	FLOAT32 ElapsedTime = m_Timer.GetFrameElapsedTime();

	// process input
	UINT32 nButtons;
	UINT32 nPressedButtons;
	FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

	// toggle user interface
	if( nPressedButtons & INPUT_KEY_0 )
		m_UserInterface.AdvanceState();


	if( nButtons & INPUT_KEY_1 )
	{
		m_Exposure -= 3.0f * ElapsedTime;
		m_Exposure = max( 0.0001f, m_Exposure );
	}

	if( nButtons & INPUT_KEY_2 )
	{
		m_Exposure += 3.0f * ElapsedTime;
		m_Exposure = min( 60.0f, m_Exposure );
	}

	if( nPressedButtons & INPUT_KEY_3 )
	{
		m_9995Shader = !m_9995Shader;

		if(m_9995Shader)
		{
			m_UserInterface.SetHeading("HDR Using GL_UNSIGNED_INT_5_9_9_9_REV Textures");
		}else
		{
			m_UserInterface.SetHeading("HDR Using 8 8 8 8 Textures");
		}
	}
}
//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight )
{
    glDisable( GL_DEPTH_TEST );

    // There is currently a bug in Qualcomm's shader compiler in that it can't
    // negate a value.  Therefore, change the shader and the order of the 
    // quad here.
    const FLOAT32 Quad[] =
    {
/*
        BottomRight.x, TopLeft.y, 1.0f, 0.0f,
        TopLeft.x, TopLeft.y, 0.0f, 0.0f,
        BottomRight.x, BottomRight.y, 1.0f, 1.0f,
        TopLeft.x, BottomRight.y, 0.0f, 1.0f,
       
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, +1.0f, 1.0f, 0.0f,  
*/

        TopLeft.x, BottomRight.y,		0.0f, 1.0f,
        BottomRight.x, BottomRight.y,	1.0f, 1.0f,
        TopLeft.x, TopLeft.y,			0.0f, 0.0f,
        BottomRight.x, TopLeft.y,		1.0f, 0.0f,
        
    };

    GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 16, Quad, GL_DYNAMIC_DRAW);

	glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, 0 );
    
    glEnableVertexAttribArray( 0 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glDisableVertexAttribArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    glEnable( GL_DEPTH_TEST );
}

//--------------------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Prepares a texture for a draw call
//--------------------------------------------------------------------------------------
VOID CSample::SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot )
{
	glActiveTexture( GL_TEXTURE0 + SamplerSlot );
	glBindTexture( GL_TEXTURE_2D, TextureHandle );
	glUniform1i( ShaderVar, SamplerSlot );
}

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawScene()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, m_nWidth, m_nHeight );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glCullFace( GL_BACK );
	glFrontFace( GL_CW );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	if( m_9995Shader )
	{
		glUseProgram( m_MaterialShader9995.ShaderId );
		glUniform1f( m_MaterialShader9995.ExposureId, m_Exposure );

		FRMVECTOR2 CardSize = FRMVECTOR2( 0.4f, 0.8f );
		FRMVECTOR2 CardPos1 = FRMVECTOR2( -0.475f, 0.0f );
		SetTexture( m_MaterialShader9995.TextureId, m_HDRTexture[0], 0 );
		RenderScreenAlignedQuad( CardPos1 - CardSize, CardPos1 + CardSize );

		FRMVECTOR2 CardPos2 = FRMVECTOR2( 0.475f, 0.0f );
		SetTexture( m_MaterialShader9995.TextureId, m_HDRTexture[1], 0 );
		RenderScreenAlignedQuad( CardPos2 - CardSize, CardPos2 + CardSize );
	}
	else
	{
		glUseProgram( m_MaterialShaderOld.ShaderId );
		glUniform1f( m_MaterialShaderOld.ExposureId, m_Exposure );

		FRMVECTOR2 CardSize = FRMVECTOR2( 0.4f, 0.8f );
		FRMVECTOR2 CardPos1 = FRMVECTOR2( -0.475f, 0.0f );
		SetTexture( m_MaterialShaderOld.TextureId, m_OldHDRTexture[0]->m_hTextureHandle, 0 );
		RenderScreenAlignedQuad( CardPos1 - CardSize, CardPos1 + CardSize );

		FRMVECTOR2 CardPos2 = FRMVECTOR2( 0.475f, 0.0f );
		SetTexture( m_MaterialShaderOld.TextureId, m_OldHDRTexture[1]->m_hTextureHandle, 0 );
		RenderScreenAlignedQuad( CardPos2 - CardSize, CardPos2 + CardSize );
	}
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
	DrawScene();

	// Update the timer
	m_Timer.MarkFrame();

	// Render the user interface
	m_UserInterface.Render( m_Timer.GetFrameRate() );

	// Put the state back
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glFrontFace( GL_CCW );
}
