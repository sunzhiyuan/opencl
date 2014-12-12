# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_TARGET_NAME := NPR30
_PROJECT_NAME := NPR
_SAMPLE_DIR := OpenGLES30
FRM_GLES_DIR := ../../../Framework/OpenGLES
SRC_DIR := ../../../$(_SAMPLE_DIR)/$(_PROJECT_NAME)

SAMPLE_ADDED_DEFINES:="-D_OGLES3 \
-DHALTI"

SAMPLE_ADDED_SRCS:=$(FRM_GLES_DIR)/FrmGLES3.cpp

.PHONY: clean rebuild_all

all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDITIONAL_SRC=$(SAMPLE_ADDED_SRCS) ADDED_DEFS=$(SAMPLE_ADDED_DEFINES) TARGET_NAME=$(_TARGET_NAME)
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) TARGET_NAME=$(_TARGET_NAME) clean
	
rebuild_all: clean all
