# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK 
# SDK makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := AdrenoSDK

BIN_DIR=../../../Bin

# Tools
RESOURCE_PACKER     :=../../Tools/ResourcePacker/Build/Linux
SHADER_PREPROCESSOR :=../../Tools/ShaderPreprocessor/Build/Linux

# Demos
ADRENOSHADERS_DEMO	:= ../../Demos/AdrenoShaders/build/linux
DISINTEGRATE_DEMO 	:= ../../Demos/Disintegrate/Build/linux
ROBOT_DEMO 			:= ../../Demos/Robot/Build/Linux

# Tutorials
TUTORIAL1	:= ../../Tutorials/OpenGLES/01_CreateWindow/build/Linux
TUTORIAL2	:= ../../Tutorials/OpenGLES/02_ConfigureFrameBuffer/build/Linux
TUTORIAL3	:= ../../Tutorials/OpenGLES/03_DrawTriangle/build/Linux
TUTORIAL4	:= ../../Tutorials/OpenGLES/04_PortraitAndLandscape/build/Linux
TUTORIAL5	:= ../../Tutorials/OpenGLES/05_NonInterleavedVBO/build/Linux
TUTORIAL6	:= ../../Tutorials/OpenGLES/06_InterleavedVBO/build/Linux
TUTORIAL7	:= ../../Tutorials/OpenGLES/07_Transforms/build/Linux
TUTORIAL8  	:= ../../Tutorials/OpenGLES/08_SimpleTexture/build/Linux
TUTORIAL9  	:= ../../Tutorials/OpenGLES/09_CompressedTexture/build/Linux
TUTORIAL10  := ../../Tutorials/OpenGLES/10_TextureProperties/build/Linux
TUTORIAL11  := ../../Tutorials/OpenGLES/11_MultiTexture/build/Linux
TUTORIAL12  := ../../Tutorials/OpenGLES/12_CubemapTexture/build/Linux
TUTORIAL13  := ../../Tutorials/OpenGLES/13_CompressedVertices/build/Linux
TUTORIAL14  := ../../Tutorials/OpenGLES/14_StencilBuffer/build/Linux
TUTORIAL15  := ../../Tutorials/OpenGLES/15_RenderToTexture/build/Linux
TUTORIAL16  := ../../Tutorials/OpenGLES/16_RenderToTextureOGLES30/build/Linux
TUTORIAL17  := ../../Tutorials/OpenGLES/17_FBOTextureFormatsOGLES30/build/Linux
TUTORIAL18  := ../../Tutorials/OpenGLES/18_InterleavedVBOOGLES30/build/Linux
TUTORIAL19  := ../../Tutorials/OpenGLES/19_InterleavedVBOOGLES30/build/Linux
TUTORIAL20  := ../../Tutorials/OpenGLES/20_Texture2DArrayOGLES30/build/Linux
TUTORIAL21  := ../../Tutorials/OpenGLES/21_PixelBufferObjectsOGLES30/build/Linux
TUTORIAL22  := ../../Tutorials/OpenGLES/22_TransformFeedbackOGLES30/build/Linux
TUTORIAL23  := ../../Tutorials/OpenGLES/23_BlitFramebufferOGLES30/build/Linux
TUTORIAL24  := ../../Tutorials/OpenGLES/24_MultisampleFramebufferOGLES30/build/Linux
TUTORIAL25  := ../../Tutorials/OpenGLES/25_FrameBufferTextureLayerOGLES30/build/Linux

# Advanced Input Samples
ADVANCEDINPUT_ACCEL     :=../../Samples/AdvancedInput/Accel/Linux
ADVANCEDINPUT_GYRO      :=../../Samples/AdvancedInput/Gyro/Linux

# OpenGL ES 2.0 Samples
ES2_BLOOM               :=../../Samples/OpenGLES/Bloom/Linux
ES2_CUBEMAP             :=../../Samples/OpenGLES/CubemapReflection/Linux
ES2_DOF                 :=../../Samples/OpenGLES/DepthOfField/Linux
ES2_IMAGEEFFECTS        :=../../Samples/OpenGLES/ImageEffects/Linux
ES2_LIGHTING            :=../../Samples/OpenGLES/Lighting/Linux
ES2_MOTIONBLUR          :=../../Samples/OpenGLES/MotionBlur/Linux
ES2_NORMALMAP           :=../../Samples/OpenGLES/NormalMapping/Linux
ES2_NPR                 :=../../Samples/OpenGLES/NPR/Linux
ES2_PARALLAX            :=../../Samples/OpenGLES/ParallaxMapping/Linux
ES2_POINTSPRITES        :=../../Samples/OpenGLES/PointSprites/Linux
ES2_PROCEDURALTEXTURE   :=../../Samples/OpenGLES/ProceduralTexture/Linux
ES2_PROJECTIVESHADOWS   :=../../Samples/OpenGLES/ProjectiveShadows/Linux
ES2_SHADOWMAP           :=../../Samples/OpenGLES/ShadowMap/Linux
ES2_SHADOWVOLUME        :=../../Samples/OpenGLES/ShadowVolume/Linux
ES2_SKINNING            :=../../Samples/OpenGLES/Skinning/Linux
ES2_SKINNING2           :=../../Samples/OpenGLES/Skinning2/Linux
ES2_SWE                 :=../../Samples/OpenGLES/SWE/Linux

# OpenGL ES 3.0 Samples
ES3_CASCADEDSHADOWS     :=../../Samples/OpenGLES30/CascadedShadowMaps/Linux
ES3_COMPRESSEDGEOMETRY  :=../../Samples/OpenGLES30/CompressedGeometry/Linux
ES3_DEPTHOFFIELD        :=../../Samples/OpenGLES30/DepthOfField/Linux
ES3_HDRTEXTURE          :=../../Samples/OpenGLES30/HDRTexture/Linux
ES3_NPR                 :=../../Samples/OpenGLES30/NPR/Linux
ES3_PARTICLESYSTEM      :=../../Samples/OpenGLES30/ParticleSystem/Linux
ES3_PHYSICALLIGHTING    :=../../Samples/OpenGLES30/PhysicallyBasedLighting/Linux
ES3_SHADOWMAP           :=../../Samples/OpenGLES30/ShadowMap/Linux
ES3_SKYDOME             :=../../Samples/OpenGLES30/Skydome/Linux
ES3_TEXTURESYNTHESIS    :=../../Samples/OpenGLES30/TextureSynthesis/Linux
ES3_TONEMAPPING         :=../../Samples/OpenGLES30/ToneMapping/Linux


# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

all:  
	mkdir -p ../../../Bin
	cp -p -f ./Run_Samples.sh ../../../Bin/Run_Samples.sh
	make -C $(RESOURCE_PACKER)         
	make -C $(SHADER_PREPROCESSOR)  

	chmod 777 ../../Samples/Build/linux/SampleShaderCompiler.sh
	chmod 777 ../../Samples/Build/linux/SampleAssetCompiler.sh
    
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

	make -C $(ADRENOSHADERS_DEMO)
	make -C $(DISINTEGRATE_DEMO)
	make -C $(ROBOT_DEMO)

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
	make -C $(TUTORIAL16)
	make -C $(TUTORIAL17)
	make -C $(TUTORIAL18)
	make -C $(TUTORIAL19)
	make -C $(TUTORIAL20)
	make -C $(TUTORIAL21)
	make -C $(TUTORIAL22)
	make -C $(TUTORIAL23)
	make -C $(TUTORIAL24)
	make -C $(TUTORIAL25)
	
	make -C $(ADVANCEDINPUT_ACCEL)
	make -C $(ADVANCEDINPUT_GYRO)
	
	make -C $(ES2_BLOOM)
	make -C $(ES2_CUBEMAP)
	make -C $(ES2_DOF)
	make -C $(ES2_IMAGEEFFECTS)
	make -C $(ES2_LIGHTING)
	make -C $(ES2_MOTIONBLUR)
	make -C $(ES2_NORMALMAP)
	make -C $(ES2_NPR)
	make -C $(ES2_PARALLAX)
	make -C $(ES2_POINTSPRITES)
	make -C $(ES2_PROCEDURALTEXTURE)
	make -C $(ES2_PROJECTIVESHADOWS)
	make -C $(ES2_SHADOWMAP)
	make -C $(ES2_SHADOWVOLUME)
	make -C $(ES2_SKINNING)
	make -C $(ES2_SKINNING2)
	make -C $(ES2_SWE)

	make -C $(ES3_CASCADEDSHADOWS) USE_CORE_CONTEXT=true
	make -C $(ES3_COMPRESSEDGEOMETRY) USE_CORE_CONTEXT=true
	make -C $(ES3_DEPTHOFFIELD) USE_CORE_CONTEXT=true
	make -C $(ES3_HDRTEXTURE) USE_CORE_CONTEXT=true
	make -C $(ES3_NPR) USE_CORE_CONTEXT=true
	make -C $(ES3_PARTICLESYSTEM) USE_CORE_CONTEXT=true
	make -C $(ES3_PHYSICALLIGHTING) USE_CORE_CONTEXT=true
	make -C $(ES3_SHADOWMAP) USE_CORE_CONTEXT=true
	make -C $(ES3_SKYDOME) USE_CORE_CONTEXT=true
	make -C $(ES3_TEXTURESYNTHESIS) USE_CORE_CONTEXT=true
	make -C $(ES3_TONEMAPPING) USE_CORE_CONTEXT=true

	mkdir -p ../../../Bin_d
	cp  -p -f ../../../Bin/Run_Samples.sh ../../../Bin_d
	cp  -p -f ../../../Bin/Run_QCompress.sh ../../../Bin_d
	cp  -p -f ../../../Bin/Run_ResourcePacker.sh ../../../Bin_d
	cp  -r -p -f ../../../Bin/Samples ../../../Bin_d/
	
clean:
	make -C $(RESOURCE_PACKER) clean     
	make -C $(SHADER_PREPROCESSOR) clean

	make -C $(ADVANCEDINPUT_ACCEL) clean
	make -C $(ADVANCEDINPUT_GYRO) clean
	
	make -C $(ADRENOSHADERS_DEMO) clean
	make -C $(DISINTEGRATE_DEMO) clean
	make -C $(ROBOT_DEMO) clean

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
	
	make -C $(ES2_BLOOM) clean
	make -C $(ES2_CUBEMAP) clean
	make -C $(ES2_DOF) clean
	make -C $(ES2_IMAGEEFFECTS) clean
	make -C $(ES2_LIGHTING) clean
	make -C $(ES2_MOTIONBLUR) clean
	make -C $(ES2_NORMALMAP) clean
	make -C $(ES2_NPR) clean
	make -C $(ES2_PARALLAX) clean
	make -C $(ES2_POINTSPRITES) clean
	make -C $(ES2_PROCEDURALTEXTURE) clean
	make -C $(ES2_PROJECTIVESHADOWS) clean
	make -C $(ES2_SHADOWMAP) clean
	make -C $(ES2_SHADOWVOLUME) clean
	make -C	$(ES2_SKINNING) clean
	make -C $(ES2_SKINNING2) clean
	make -C $(ES2_SWE) clean
    
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

rebuild_all: clean all

