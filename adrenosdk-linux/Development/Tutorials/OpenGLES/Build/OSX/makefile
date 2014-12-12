

TUTORIAL1  	:= ../../01_CreateWindow/build/OSX
TUTORIAL2  	:= ../../02_ConfigureFrameBuffer/build/OSX
TUTORIAL3	:= ../../03_DrawTriangle/build/OSX
TUTORIAL4  	:= ../../04_PortraitAndLandscape/build/OSX
TUTORIAL5  	:= ../../05_NonInterleavedVBO/build/OSX
TUTORIAL6  	:= ../../06_InterleavedVBO/build/OSX
TUTORIAL7  	:= ../../07_Transforms/build/OSX
TUTORIAL8  	:= ../../08_SimpleTexture/build/OSX
TUTORIAL9  	:= ../../09_CompressedTexture/build/OSX
TUTORIAL10  := ../../10_TextureProperties/build/OSX
TUTORIAL11  := ../../11_MultiTexture/build/OSX
TUTORIAL12  := ../../12_CubemapTexture/build/OSX
TUTORIAL13  := ../../13_CompressedVertices/build/OSX
TUTORIAL14  := ../../14_StencilBuffer/build/OSX
TUTORIAL15  := ../../15_RenderToTexture/build/OSX
TUTORIAL16  := ../../16_RenderToTextureOGLES30/build/OSX
TUTORIAL17  := ../../17_FBOTextureFormatsOGLES30/build/OSX
TUTORIAL18  := ../../18_InterleavedVBOOGLES30/build/OSX
TUTORIAL19  := ../../19_InterleavedVBOOGLES30/build/OSX
TUTORIAL20  := ../../20_Texture2DArrayOGLES30/build/OSX
TUTORIAL21  := ../../21_PixelBufferObjectsOGLES30/build/OSX
TUTORIAL22  := ../../22_TransformFeedbackOGLES30/build/OSX
TUTORIAL23  := ../../23_BlitFramebufferOGLES30/build/OSX
TUTORIAL24  := ../../24_MultisampleFramebufferOGLES30/build/OSX
TUTORIAL25  := ../../25_FrameBufferTextureLayerOGLES30/build/OSX

all:
	make -C $(TUTORIAL1)
	make -C $(TUTORIAL2)
	make -C $(TUTORIAL3)
	make -C $(TUTORIAL4)
	make -C $(TUTORIAL5)
	make -C $(TUTORIAL6)
	make -C $(TUTORIAL7)
	make -C $(TUTORIAL8)
	make -C $(TUTORIAL9)
	make -C $(TUTORIAL10)
	make -C $(TUTORIAL11)
	make -C $(TUTORIAL12)
	make -C $(TUTORIAL13)
	make -C $(TUTORIAL14)
	make -C $(TUTORIAL15)
	make -C $(TUTORIAL17) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL18) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL19) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL21) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL22) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL23) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL24) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL25) USE_CORE_CONTEXT=true
	
clean:
	make -C $(TUTORIAL1) clean
	make -C $(TUTORIAL2) clean
	make -C $(TUTORIAL3) clean
	make -C $(TUTORIAL4) clean
	make -C $(TUTORIAL5) clean
	make -C $(TUTORIAL6) clean
	make -C $(TUTORIAL7) clean
	make -C $(TUTORIAL8) clean
	make -C $(TUTORIAL9) clean
	make -C $(TUTORIAL10) clean
	make -C $(TUTORIAL11) clean
	make -C $(TUTORIAL12) clean
	make -C $(TUTORIAL13) clean
	make -C $(TUTORIAL14) clean
	make -C $(TUTORIAL15) clean
	make -C $(TUTORIAL16) clean
	make -C $(TUTORIAL17) clean
	make -C $(TUTORIAL18) clean
	make -C $(TUTORIAL19) clean
	make -C $(TUTORIAL20) clean
	make -C $(TUTORIAL21) clean
	make -C $(TUTORIAL22) clean
	make -C $(TUTORIAL23) clean
	make -C $(TUTORIAL24) clean
	make -C $(TUTORIAL25) clean
	
rebuild_all:
	clean all

