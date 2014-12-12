# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# MultisampleFramebufferOGLES30 OpenGL ES Tutorial makefile for OSX, tested on OSX

ARCH=x86
_PROJECT_NAME:=MultisampleFramebufferOGLES30
_SAMPLE_DIR:=24_MultisampleFramebufferOGLES30


.PHONY: clean


all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) 
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) clean
	
rebuild_all: clean all
