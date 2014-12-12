# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# InterleavedVBOOGLES30 OpenGL ES Tutorial makefile for OSX, tested on OSX

ARCH=x86
_PROJECT_NAME:=InterleavedVBOOGLES30
_SAMPLE_DIR:=18_InterleavedVBOOGLES30


.PHONY: clean


all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) 
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) clean
	
rebuild_all: clean all
