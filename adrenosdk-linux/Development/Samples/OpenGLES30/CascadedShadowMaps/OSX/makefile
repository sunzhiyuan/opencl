# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_PROJECT_NAME := CascadedShadowMaps
_SAMPLE_DIR := OpenGLES30
FRM_GLES_DIR := ../../../Framework/OpenGLES
SRC_DIR := ../../../$(_SAMPLE_DIR)/$(_PROJECT_NAME)

SAMPLE_ADDED_DEFINES:="-D_OGLES3 \
-DHALTI"

_ADDED_SRC_DIRS:= $(SRC_DIR)/CSM
SAMPLE_ADDED_SRCS:="$(FRM_GLES_DIR)/FrmGLES3.cpp \
				$(SRC_DIR)/BasicMesh.cpp \
                $(SRC_DIR)/CSM/Bounds.cpp \
                $(SRC_DIR)/CSM/Settings.cpp \
                $(SRC_DIR)/CSM/Shadows.cpp"

.PHONY: clean rebuild_all

all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) ADDITIONAL_SRC=$(SAMPLE_ADDED_SRCS) ADDED_DEFS=$(SAMPLE_ADDED_DEFINES)
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) clean
	
rebuild_all: clean all
