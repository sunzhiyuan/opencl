# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_PROJECT_NAME:=ProceduralTexture
_SAMPLE_DIR:=OpenGLES 
FRM_LINUX_DIR := ../../../Framework/Linux

.PHONY: clean

all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDITIONAL_SRC=$(FRM_LINUX_DIR)/FrmNoise_Platform.cpp
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) clean
	
rebuild_all: clean all
