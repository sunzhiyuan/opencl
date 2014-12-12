# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_PROJECT_NAME:=Skinning
_SAMPLE_DIR:=OpenGLES   
.PHONY: clean

all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR)
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) clean
	
rebuild_all: clean all
