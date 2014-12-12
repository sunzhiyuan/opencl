# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
FRM_MODEL_DIR := ../../../Framework/Model

_PROJECT_NAME:=Skinning2
_SAMPLE_DIR:=OpenGLES 
_ADDED_SRC_DIRS:= $(FRM_MODEL_DIR)
_ADDITIONAL_SRC:= "$(FRM_MODEL_DIR)/FrmAnimation.cpp \
		$(FRM_MODEL_DIR)/FrmAnimationTrack.cpp \
		$(FRM_MODEL_DIR)/FrmIndexBuffer.cpp \
		$(FRM_MODEL_DIR)/FrmJoint.cpp \
		$(FRM_MODEL_DIR)/FrmMaterial.cpp \
		$(FRM_MODEL_DIR)/FrmMemoryBuffer.cpp \
		$(FRM_MODEL_DIR)/FrmMeshSurfaceArray.cpp \
		$(FRM_MODEL_DIR)/FrmModel.cpp \
		$(FRM_MODEL_DIR)/FrmModelIO.cpp \
		$(FRM_MODEL_DIR)/FrmModelMesh.cpp \
		$(FRM_MODEL_DIR)/FrmNamedId.cpp \
		$(FRM_MODEL_DIR)/FrmVertexBuffer.cpp \
		$(FRM_MODEL_DIR)/FrmVertexFormat.cpp \
		$(FRM_MODEL_DIR)/FrmVertexProperty.cpp"

.PHONY: clean

all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) ADDITIONAL_SRC=$(_ADDITIONAL_SRC)
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) ADDITIONAL_SRC=$(_ADDITIONAL_SRC) clean
	
rebuild_all: clean all
