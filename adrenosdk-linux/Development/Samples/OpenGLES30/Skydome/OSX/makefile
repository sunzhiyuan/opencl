# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_PROJECT_NAME := Skydome
_SAMPLE_DIR := OpenGLES30
FRM_GLES_DIR := ../../../Framework/OpenGLES
SRC_DIR := ../../../$(_SAMPLE_DIR)/$(_PROJECT_NAME)


SAMPLE_ADDED_DEFINES:="-D_OGLES3 \
-DHALTI"

_PREBUILD_SCRIPT:=$(SRC_DIR)/Linux/prebuild.sh
_ADDED_SRC_DIRS:= "$(SRC_DIR)/scene \
$(SRC_DIR)/scatter"
SAMPLE_ADDED_SRCS:="$(FRM_GLES_DIR)/FrmGLES3.cpp \
$(SRC_DIR)/scatter/HoffmanScatter.cpp \
		$(SRC_DIR)/scene/HeightField.cpp \
		$(SRC_DIR)/scene/SkyDome.cpp \
		$(SRC_DIR)/scene/StaticTerrain.cpp "

PARAMS:= PROJECT_NAME=$(_PROJECT_NAME) \
	SAMPLE_DIR=$(_SAMPLE_DIR) \
	 ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) \
	 ADDITIONAL_SRC=$(SAMPLE_ADDED_SRCS) \
	 ADDED_DEFS=$(SAMPLE_ADDED_DEFINES) \
	 PREBUILD_SCRIPT=$(_PREBUILD_SCRIPT)

.PHONY: clean rebuild_all

all:
	make -C ../../../Build/Unix/$(ARCH) $(PARAMS)
	
clean:
	make -C ../../../Build/Unix/$(ARCH) $(PARAMS) clean
	
rebuild_all: clean all
