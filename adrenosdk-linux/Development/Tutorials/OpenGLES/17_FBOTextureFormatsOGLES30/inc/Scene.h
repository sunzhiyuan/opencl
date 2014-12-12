//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


//--------------------------------------------------------------------------------------
// TESTING FRAME BUFFER FORMATS
//	Use these defines to control the framebuffer type
//	the possible values are shown to the right
//	
//	Many possibilities are automatically restricted because of supported formats
//	(such as specifying anything other than 32 bit channels for a float frame buffer
//	will result in a half float frame buffer, or choosing a 3 channel type will
//	automatically make the FB_FORMAT RGB)
//	
//	When specifying an unsupported format/channel/type combination, the frame buffer
//	will try to default to a simple format.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Name: g_internalFormat, g_format, g_type
// Desc: Specifies the format of the texture that serves as a color buffer
//--------------------------------------------------------------------------------------	
#define FB_TYPE_10_10_10_2			// FB_TYPE_FLOAT FB_TYPE_INT FB_TYPE_4_4_4_4 FB_TYPE_8_8_8 FB_TYPE_8_8_8_8 FB_TYPE_11_11_10 FB_TYPE_5_6_5 FB_TYPE_10_10_10_2
#define FB_FORMAT_RGBA		// FB_FORMAT_RGBA FB_FORMAT_RGB FB_FORMAT_SRGBA
#define FB_CHANNEL_16		// FB_CHANNEL_8 FB_CHANNEL_16 FB_CHANNEL_32
#define FB_SIGNED			// FB_SIGNED FB_UNSIGNED

#ifdef FB_FORMAT_RGB
	GLenum g_format = GL_RGB;
	#ifdef FB_TYPE_8_8_8
		GLint g_internalFormat = GL_RGB8;
		GLenum g_type = GL_UNSIGNED_BYTE;
	#elif defined FB_TYPE_11_11_10
		GLint g_internalFormat = GL_R11F_G11F_B10F;
		GLenum g_type = GL_UNSIGNED_INT_10F_11F_11F_REV;
	#else	// defined FB_TYPE_5_6_5
		GLint g_internalFormat = GL_RGB565;
		GLenum g_type = GL_UNSIGNED_SHORT_5_6_5;
	#endif
#elif defined FB_FORMAT_SRGBA
	GLenum g_format = GL_RGBA;
	GLint g_internalFormat = GL_SRGB8_ALPHA8;
	GLenum g_type = GL_UNSIGNED_BYTE;
#else	// defined FB_FORMAT_RGBA
	#ifdef FB_TYPE_FLOAT
		#ifdef FB_CHANNEL_32
			GLenum g_format = GL_RGBA;
			GLint g_internalFormat = GL_RGBA32F;
			GLenum g_type = GL_FLOAT;
		#else
			GLenum g_format = GL_RGBA;
			GLint g_internalFormat = GL_RGBA16F;
			GLenum g_type = GL_FLOAT;
		#endif
	#elif defined FB_TYPE_INT
		GLenum g_format = GL_RGBA_INTEGER;
		#ifdef FB_CHANNEL_8
			#ifdef FB_SIGNED
				GLint g_internalFormat = GL_RGBA8I;
				GLenum g_type = GL_BYTE;
			#else
				GLint g_internalFormat = GL_RGBA8UI;
				GLenum g_type = GL_UNSIGNED_BYTE;
			#endif
		#elif defined FB_CHANNEL_16
			#ifdef FB_SIGNED
				GLint g_internalFormat = GL_RGBA16I;
				GLenum g_type = GL_SHORT;
			#else
				GLint g_internalFormat = GL_RGBA16UI;
				GLenum g_type = GL_UNSIGNED_SHORT;
			#endif
		#else // defined FB_CHANNEL_32
			#ifdef FB_SIGNED
				GLint g_internalFormat = GL_RGBA32I;
				GLenum g_type = GL_INT;
			#else
				GLint g_internalFormat = GL_RGBA32UI;
				GLenum g_type = GL_UNSIGNED_INT;
			#endif
		#endif
	#elif defined FB_TYPE_4_4_4_4
		GLenum g_format = GL_RGBA;
		GLint g_internalFormat = GL_RGBA4;
		GLenum g_type = GL_UNSIGNED_BYTE;
	#elif defined FB_TYPE_11_11_10
		GLenum g_format = GL_RGB;
		GLint g_internalFormat = GL_R11F_G11F_B10F;
		GLenum g_type = GL_UNSIGNED_INT_10F_11F_11F_REV;
	#elif defined FB_TYPE_10_10_10_2
		GLenum g_format = GL_RGBA;
		GLint g_internalFormat = GL_RGB10_A2;
		GLenum g_type = GL_UNSIGNED_INT_2_10_10_10_REV;
	#else	// defined FB_TYPE_8_8_8_8
		GLenum g_format = GL_RGBA;
		GLint g_internalFormat = GL_RGBA8;
		GLenum g_type = GL_UNSIGNED_BYTE;
	#endif
#endif

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
    UINT32 m_nWidth;
    UINT32 m_nHeight;

    UINT32 m_hFrameBuffer;
    UINT32 m_hRenderBuffer;
    UINT32 m_hTexture;
};

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
    
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:    

    BOOL InitShaders();
    BOOL CreateFBO(UINT32,UINT32,UINT32,UINT32,UINT32,FrameBufferObject**);
    VOID DestroyFBO(FrameBufferObject*);
    VOID BeginFBO(FrameBufferObject*);
    VOID EndFBO(FrameBufferObject*);
    VOID RenderScene(FLOAT);

    const CHAR*  g_strWindowTitle;
    const CHAR* g_strOverlayFragmentShader;
    const CHAR* g_strOverlayVertexShader;
    const CHAR* g_strFragmentShader;
    const CHAR* g_strVertexShader;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
      
    FrameBufferObject* g_pOffscreenFBO;

    GLuint       g_hShaderProgram        ;
    GLuint       g_hOverlayShaderProgram ;
    GLuint       g_hModelViewMatrixLoc   ;
    GLuint       g_hProjMatrixLoc        ;
    GLuint       g_hVertexLoc            ;
    GLuint       g_hColorLoc             ;
	GLuint		 g_hPositionVbo;
	GLuint		 g_hColorVbo;
	GLuint		 g_hVao;


};

#endif // SCENE_H
