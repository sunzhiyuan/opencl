# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK 
# SDK makefile for OSX x86, tested on OSX x86
# FreeImage must be installed on your OSX system

ifeq ($(ARCH),x86)
   $(info === x86 === )
else
   ARCH:=x64
   $(info === x64 === )
   EXTRA_ARCH_PATHING:=/x64
endif

ifeq ($(MODE),debug)
   $(info === DEBUG === )
   BIN_DIR := ../../../../Bin_d$(EXTRA_ARCH_PATHING)
else
   MODE:=release
   $(info === RELEASE === )
   BIN_DIR := ../../../../Bin$(EXTRA_ARCH_PATHING)
endif



PROJECT_NAME := AdrenoSDK

# Tools
RESOURCE_PACKER     :=../../Tools/ResourcePacker/Build/OSX/$(ARCH)
SHADER_PREPROCESSOR :=../../Tools/ShaderPreprocessor/Build/OSX

#Demos
DEMOS_ADRENOSHADERS 	:=../../Demos/AdrenoShaders/build/OSX
DEMOS_DISINTEGRATE		:=../../Demos/Disintegrate/build/OSX
DEMOS_ROBOT				:=../../Demos/Robot/build/OSX

# Tutorials
TUTORIAL1	:= ../../Tutorials/OpenGLES/01_CreateWindow/build/OSX
TUTORIAL2	:= ../../Tutorials/OpenGLES/02_ConfigureFrameBuffer/build/OSX
TUTORIAL3	:= ../../Tutorials/OpenGLES/03_DrawTriangle/build/OSX
TUTORIAL4	:= ../../Tutorials/OpenGLES/04_PortraitAndLandscape/build/OSX
TUTORIAL5	:= ../../Tutorials/OpenGLES/05_NonInterleavedVBO/build/OSX
TUTORIAL6	:= ../../Tutorials/OpenGLES/06_InterleavedVBO/build/OSX
TUTORIAL7	:= ../../Tutorials/OpenGLES/07_Transforms/build/OSX
TUTORIAL8  	:= ../../Tutorials/OpenGLES/08_SimpleTexture/build/OSX
TUTORIAL9  	:= ../../Tutorials/OpenGLES/09_CompressedTexture/build/OSX
TUTORIAL10  := ../../Tutorials/OpenGLES/10_TextureProperties/build/OSX
TUTORIAL11  := ../../Tutorials/OpenGLES/11_MultiTexture/build/OSX
TUTORIAL12  := ../../Tutorials/OpenGLES/12_CubemapTexture/build/OSX
TUTORIAL13  := ../../Tutorials/OpenGLES/13_CompressedVertices/build/OSX
TUTORIAL14  := ../../Tutorials/OpenGLES/14_StencilBuffer/build/OSX
TUTORIAL15  := ../../Tutorials/OpenGLES/15_RenderToTexture/build/OSX
TUTORIAL16  := ../../Tutorials/OpenGLES/16_RenderToTextureOGLES30/build/OSX
TUTORIAL17  := ../../Tutorials/OpenGLES/17_FBOTextureFormatsOGLES30/build/OSX
TUTORIAL18  := ../../Tutorials/OpenGLES/18_InterleavedVBOOGLES30/build/OSX
TUTORIAL19  := ../../Tutorials/OpenGLES/19_InterleavedVBOOGLES30/build/OSX
TUTORIAL20  := ../../Tutorials/OpenGLES/20_Texture2DArrayOGLES30/build/OSX
TUTORIAL21  := ../../Tutorials/OpenGLES/21_PixelBufferObjectsOGLES30/build/OSX
TUTORIAL22  := ../../Tutorials/OpenGLES/22_TransformFeedbackOGLES30/build/OSX
TUTORIAL23  := ../../Tutorials/OpenGLES/23_BlitFramebufferOGLES30/build/OSX
TUTORIAL24  := ../../Tutorials/OpenGLES/24_MultisampleFramebufferOGLES30/build/OSX
TUTORIAL25  := ../../Tutorials/OpenGLES/25_FrameBufferTextureLayerOGLES30/build/OSX

# Advanced Input Samples
ADVANCEDINPUT_ACCEL     :=../../Samples/AdvancedInput/Accel/OSX
ADVANCEDINPUT_GYRO      :=../../Samples/AdvancedInput/Gyro/OSX

# OpenGL ES 2.0 Samples
ES2_BLOOM               :=../../Samples/OpenGLES/Bloom/OSX
ES2_CUBEMAP             :=../../Samples/OpenGLES/CubemapReflection/OSX
ES2_DOF                 :=../../Samples/OpenGLES/DepthOfField/OSX
ES2_IMAGEEFFECTS        :=../../Samples/OpenGLES/ImageEffects/OSX
ES2_LIGHTING            :=../../Samples/OpenGLES/Lighting/OSX
ES2_MOTIONBLUR          :=../../Samples/OpenGLES/MotionBlur/OSX
ES2_NORMALMAP           :=../../Samples/OpenGLES/NormalMapping/OSX
ES2_NPR                 :=../../Samples/OpenGLES/NPR/OSX
ES2_PARALLAX            :=../../Samples/OpenGLES/ParallaxMapping/OSX
ES2_POINTSPRITES        :=../../Samples/OpenGLES/PointSprites/OSX
ES2_PROCEDURALTEXTURE   :=../../Samples/OpenGLES/ProceduralTexture/OSX
ES2_PROJECTIVESHADOWS   :=../../Samples/OpenGLES/ProjectiveShadows/OSX
ES2_SHADOWMAP           :=../../Samples/OpenGLES/ShadowMap/OSX
ES2_SHADOWVOLUME        :=../../Samples/OpenGLES/ShadowVolume/OSX
ES2_SKINNING            :=../../Samples/OpenGLES/Skinning/OSX
ES2_SKINNING2           :=../../Samples/OpenGLES/Skinning2/OSX
ES2_SWE                 :=../../Samples/OpenGLES/SWE/OSX

# OpenGL ES 3.0 Samples
ES3_CASCADEDSHADOWS     :=../../Samples/OpenGLES30/CascadedShadowMaps/OSX
ES3_COMPRESSEDGEOMETRY  :=../../Samples/OpenGLES30/CompressedGeometry/OSX
ES3_DEPTHOFFIELD        :=../../Samples/OpenGLES30/DepthOfField/OSX
ES3_HDRTEXTURE          :=../../Samples/OpenGLES30/HDRTexture/OSX
ES3_NPR                 :=../../Samples/OpenGLES30/NPR/OSX
ES3_PARTICLESYSTEM      :=../../Samples/OpenGLES30/ParticleSystem/OSX
ES3_PHYSICALLIGHTING    :=../../Samples/OpenGLES30/PhysicallyBasedLighting/OSX
ES3_SHADOWMAP           :=../../Samples/OpenGLES30/ShadowMap/OSX
ES3_SKYDOME             :=../../Samples/OpenGLES30/Skydome/OSX
ES3_TEXTURESYNTHESIS    :=../../Samples/OpenGLES30/TextureSynthesis/OSX
ES3_TONEMAPPING         :=../../Samples/OpenGLES30/ToneMapping/OSX


# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

all:  
	mkdir -p ../../../Bin
	make -C $(RESOURCE_PACKER)         
	make -C $(SHADER_PREPROCESSOR) ARCH=$(ARCH) MODE=$(MODE)


	if [ "$(GENERATE_BINARY_ASSETS)" != "No" ]; then \
		chmod 555 ../../Samples/Build/linux/SampleShaderCompiler.sh; \
		chmod 555 ../../Samples/Build/linux/SampleAssetCompiler.sh; \
		cd ../../Samples/Build/linux; ./SampleShaderCompiler.sh; ./SampleAssetCompiler.sh; \
		chmod 555 ../../../Demos/Build/Linux/DemoShaderCompiler.sh; \
		chmod 555 ../../../Demos/Build/Linux/DemoAssetCompiler.sh; \
		cd ../../../Demos/Build/Linux; ./DemoShaderCompiler.sh; ./DemoAssetCompiler.sh; \
	else \
		cp -r -p -f ../../BinaryAssets/ $(BIN_DIR)/BinaryAssets/; \
	fi
	
	mkdir -p ../../../Bin/Samples/Kernels
	mkdir -p ../../../Bin/Samples/Meshes	
	cp -r -p -f ../../Assets/Samples/Kernels/* ../../../Bin/Samples/Kernels			
	cp -r -p -f ../../Assets/Samples/Meshes/* ../../../Bin/Samples/Meshes
	rm -r -f ../../../Bin/Samples/Meshes/Source

ifeq ($(ARCH),x86)
	make -C $(ADVANCEDINPUT_ACCEL)
	make -C $(ADVANCEDINPUT_GYRO)

	make -C $(DEMOS_ADRENOSHADERS) ARCH=$(ARCH)
	make -C $(DEMOS_DISINTEGRATE) ARCH=$(ARCH)
	make -C $(DEMOS_ROBOT) ARCH=$(ARCH)
	
	make -C $(TUTORIAL1) ARCH=$(ARCH)
	make -C $(TUTORIAL2) ARCH=$(ARCH)
	make -C $(TUTORIAL3) ARCH=$(ARCH)
	make -C $(TUTORIAL4) ARCH=$(ARCH)
	make -C $(TUTORIAL5) ARCH=$(ARCH)
	make -C $(TUTORIAL6) ARCH=$(ARCH)
	make -C $(TUTORIAL7) ARCH=$(ARCH)
	make -C $(TUTORIAL8) ARCH=$(ARCH)
	make -C $(TUTORIAL9) ARCH=$(ARCH)
	make -C $(TUTORIAL10) ARCH=$(ARCH)
	make -C $(TUTORIAL11) ARCH=$(ARCH)
	make -C $(TUTORIAL12) ARCH=$(ARCH)
	make -C $(TUTORIAL13) ARCH=$(ARCH)
	make -C $(TUTORIAL14) ARCH=$(ARCH)
	make -C $(TUTORIAL15) ARCH=$(ARCH)
	make -C $(TUTORIAL17) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL18) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL19) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL21) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL22) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL23) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL24) ARCH=$(ARCH) USE_CORE_CONTEXT=true
	make -C $(TUTORIAL25) ARCH=$(ARCH) USE_CORE_CONTEXT=true
endif

ifeq ($(ARCH),x86)
	make -C $(ES2_BLOOM) ARCH=$(ARCH)
	make -C $(ES2_CUBEMAP) ARCH=$(ARCH)
	make -C $(ES2_DOF) ARCH=$(ARCH)
	make -C $(ES2_IMAGEEFFECTS) ARCH=$(ARCH)
	make -C $(ES2_LIGHTING) ARCH=$(ARCH)
	make -C $(ES2_MOTIONBLUR) ARCH=$(ARCH)
	make -C $(ES2_NORMALMAP) ARCH=$(ARCH)
	make -C $(ES2_NPR) ARCH=$(ARCH)
	make -C $(ES2_PARALLAX) ARCH=$(ARCH)
	make -C $(ES2_POINTSPRITES) ARCH=$(ARCH)
	make -C $(ES2_PROCEDURALTEXTURE) ARCH=$(ARCH)
	make -C $(ES2_PROJECTIVESHADOWS) ARCH=$(ARCH)
	make -C $(ES2_SHADOWMAP) ARCH=$(ARCH)
	make -C $(ES2_SHADOWVOLUME) ARCH=$(ARCH)
	make -C $(ES2_SKINNING) ARCH=$(ARCH)
	make -C $(ES2_SKINNING2) ARCH=$(ARCH)
	make -C $(ES2_SWE) ARCH=$(ARCH)
    
	make -C $(ES3_CASCADEDSHADOWS) USE_CORE_CONTEXT=true
	make -C $(ES3_COMPRESSEDGEOMETRY) USE_CORE_CONTEXT=true
	make -C $(ES3_DEPTHOFFIELD) USE_CORE_CONTEXT=true
	make -C $(ES3_HDRTEXTURE) USE_CORE_CONTEXT=true
	make -C $(ES3_NPR) USE_CORE_CONTEXT=true
	make -C $(ES3_PARTICLESYSTEM) USE_CORE_CONTEXT=true
	make -C $(ES3_SHADOWMAP) USE_CORE_CONTEXT=true
	make -C $(ES3_TEXTURESYNTHESIS) USE_CORE_CONTEXT=true
	make -C $(ES3_TONEMAPPING) USE_CORE_CONTEXT=true
endif
	mkdir -p $(BIN_DIR)
	#cp  -p -f $(RELEASE_BIN_DIR)/Run_ResourcePacker.sh $(DEBUG_BIN_DIR)
	#cp  -r -p -f $(RELEASE_BIN_DIR)/Samples $(DEBUG_BIN_DIR)
	
clean:
	make -C $(RESOURCE_PACKER) clean     
	make -C $(SHADER_PREPROCESSOR) ARCH=$(ARCH) MODE=$(MODE) clean

ifeq ($(ARCH),x86)
	make -C $(ADVANCEDINPUT_ACCEL) clean
	make -C $(ADVANCEDINPUT_GYRO) clean
	
	make -C $(DEMOS_ADRENOSHADERS) ARCH=$(ARCH) clean
	make -C $(DEMOS_DISINTEGRATE) ARCH=$(ARCH) clean
	make -C $(DEMOS_ROBOT) ARCH=$(ARCH) clean
	
	make -C $(TUTORIAL1) ARCH=$(ARCH) clean
	make -C $(TUTORIAL2) ARCH=$(ARCH) clean
	make -C $(TUTORIAL3) ARCH=$(ARCH) clean
	make -C $(TUTORIAL4) ARCH=$(ARCH) clean
	make -C $(TUTORIAL5) ARCH=$(ARCH) clean
	make -C $(TUTORIAL6) ARCH=$(ARCH) clean
	make -C $(TUTORIAL7) ARCH=$(ARCH) clean
	make -C $(TUTORIAL8) ARCH=$(ARCH) clean
	make -C $(TUTORIAL9) ARCH=$(ARCH) clean
	make -C $(TUTORIAL10) ARCH=$(ARCH) clean
	make -C $(TUTORIAL11) ARCH=$(ARCH) clean
	make -C $(TUTORIAL12) ARCH=$(ARCH) clean
	make -C $(TUTORIAL13) ARCH=$(ARCH) clean
	make -C $(TUTORIAL14) ARCH=$(ARCH) clean
	make -C $(TUTORIAL15) ARCH=$(ARCH) clean
	make -C $(TUTORIAL16) ARCH=$(ARCH) clean
	make -C $(TUTORIAL17) ARCH=$(ARCH) clean
	make -C $(TUTORIAL18) ARCH=$(ARCH) clean
	make -C $(TUTORIAL19) ARCH=$(ARCH) clean
	make -C $(TUTORIAL20) ARCH=$(ARCH) clean
	make -C $(TUTORIAL21) ARCH=$(ARCH) clean
	make -C $(TUTORIAL22) ARCH=$(ARCH) clean
	make -C $(TUTORIAL23) ARCH=$(ARCH) clean
	make -C $(TUTORIAL24) ARCH=$(ARCH) clean
	make -C $(TUTORIAL25) ARCH=$(ARCH) clean
endif

ifeq ($(ARCH),x86)
	make -C $(ES2_BLOOM) ARCH=$(ARCH) clean
	make -C $(ES2_CUBEMAP) ARCH=$(ARCH) clean
	make -C $(ES2_DOF) ARCH=$(ARCH) clean
	make -C $(ES2_IMAGEEFFECTS) ARCH=$(ARCH) clean
	make -C $(ES2_LIGHTING) ARCH=$(ARCH) clean
	make -C $(ES2_MOTIONBLUR) ARCH=$(ARCH) clean
	make -C $(ES2_NORMALMAP) ARCH=$(ARCH) clean
	make -C $(ES2_NPR) ARCH=$(ARCH) clean
	make -C $(ES2_PARALLAX) ARCH=$(ARCH) clean
	make -C $(ES2_POINTSPRITES) ARCH=$(ARCH) clean
	make -C $(ES2_PROCEDURALTEXTURE) ARCH=$(ARCH) clean
	make -C $(ES2_PROJECTIVESHADOWS) ARCH=$(ARCH) clean
	make -C $(ES2_SHADOWMAP) ARCH=$(ARCH) clean
	make -C $(ES2_SHADOWVOLUME) ARCH=$(ARCH) clean
	make -C	$(ES2_SKINNING) ARCH=$(ARCH) clean
	make -C $(ES2_SKINNING2) ARCH=$(ARCH) clean
	make -C $(ES2_SWE) ARCH=$(ARCH) clean
    
	make -C $(ES3_CASCADEDSHADOWS) clean
	make -C $(ES3_COMPRESSEDGEOMETRY) clean
	make -C $(ES3_DEPTHOFFIELD) clean
	make -C $(ES3_HDRTEXTURE) clean
	make -C $(ES3_NPR) clean
	make -C $(ES3_PARTICLESYSTEM) clean
	make -C $(ES3_PHYSICALLIGHTING) clean
	make -C	$(ES3_SHADOWMAP) clean
	make -C $(ES3_SKYDOME) clean
	make -C $(ES3_TEXTURESYNTHESIS) clean
	make -C $(ES3_TONEMAPPING) clean
endif
rebuild_all: clean all

