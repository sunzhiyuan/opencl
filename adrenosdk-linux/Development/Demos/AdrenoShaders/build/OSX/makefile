# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ARCH?=x86
_PROJECT_NAME:=AdrenoShaders
_SAMPLE_DIR:=AdrenoShaders


SRC_DIR := ../../../AdrenoShaders/src
SRC_SCENE_DIR := ../../../AdrenoShaders/src/scenes

_ADDED_SRC_DIRS := "$(SRC_DIR) \
					$(SRC_SCENE_DIR)"

_ADDITIONAL_SRC :=	"$(SRC_DIR)/application.cpp \
					$(SRC_DIR)/common.cpp \
		$(SRC_SCENE_DIR)/Scene01.cpp \
		$(SRC_SCENE_DIR)/Scene02.cpp \
		$(SRC_SCENE_DIR)/Scene03.cpp \
		$(SRC_SCENE_DIR)/Scene04.cpp \
		$(SRC_SCENE_DIR)/Scene05.cpp \
		$(SRC_SCENE_DIR)/Scene06.cpp \
		$(SRC_SCENE_DIR)/Scene07.cpp \
		$(SRC_SCENE_DIR)/Scene08.cpp \
		$(SRC_SCENE_DIR)/Scene09.cpp \
		$(SRC_SCENE_DIR)/Scene10.cpp \
		$(SRC_SCENE_DIR)/Scene11.cpp \
		$(SRC_SCENE_DIR)/Scene12.cpp \
		$(SRC_SCENE_DIR)/Scene13.cpp \
		$(SRC_SCENE_DIR)/Scene14.cpp \
		$(SRC_SCENE_DIR)/Scene14b.cpp \
		$(SRC_SCENE_DIR)/Scene15.cpp \
		$(SRC_SCENE_DIR)/Scene16.cpp \
		$(SRC_SCENE_DIR)/Scene17.cpp \
		$(SRC_SCENE_DIR)/Scene18.cpp \
		$(SRC_SCENE_DIR)/Scene19.cpp \
		$(SRC_SCENE_DIR)/Scene20.cpp \
		$(SRC_SCENE_DIR)/Scene21.cpp \
		$(SRC_SCENE_DIR)/Scene22.cpp \
		$(SRC_SCENE_DIR)/Scene23.cpp \
		$(SRC_SCENE_DIR)/Scene24.cpp \
		$(SRC_SCENE_DIR)/Scene25.cpp \
		$(SRC_SCENE_DIR)/Scene26.cpp \
		$(SRC_SCENE_DIR)/Scene27.cpp \
		$(SRC_SCENE_DIR)/Scene28.cpp \
		$(SRC_SCENE_DIR)/Scene29.cpp \
		$(SRC_SCENE_DIR)/Scene30.cpp \
		$(SRC_SCENE_DIR)/Scene31.cpp \
		$(SRC_SCENE_DIR)/Scene32.cpp \
		$(SRC_SCENE_DIR)/Scene33.cpp \
		$(SRC_SCENE_DIR)/Scene34.cpp \
		$(SRC_SCENE_DIR)/Scene35.cpp \
		$(SRC_SCENE_DIR)/Scene36.cpp \
		$(SRC_SCENE_DIR)/Scene37.cpp \
		$(SRC_SCENE_DIR)/Scene38.cpp \
		$(SRC_SCENE_DIR)/Scene39.cpp \
		$(SRC_SCENE_DIR)/Scene40.cpp \
		$(SRC_SCENE_DIR)/Scene41.cpp \
		$(SRC_SCENE_DIR)/Scene42.cpp \
		$(SRC_SCENE_DIR)/Scene43.cpp \
		$(SRC_SCENE_DIR)/Scene44.cpp \
		$(SRC_SCENE_DIR)/Scene45.cpp \
		$(SRC_SCENE_DIR)/Scene46.cpp \
		$(SRC_SCENE_DIR)/Scene47.cpp \
		$(SRC_SCENE_DIR)/Scene48.cpp \
		$(SRC_SCENE_DIR)/Scene49.cpp \
		$(SRC_SCENE_DIR)/Scene50.cpp"

CFLAGS := -DOS_OSX


.PHONY: clean


all:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDED_DEFS=$(CFLAGS) ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) ADDITIONAL_SRC=$(_ADDITIONAL_SRC)
	
clean:
	make -C ../../../Build/Unix/$(ARCH) PROJECT_NAME=$(_PROJECT_NAME) SAMPLE_DIR=$(_SAMPLE_DIR) ADDED_SRC_DIRS=$(_ADDED_SRC_DIRS) ADDITIONAL_SRC=$(_ADDITIONAL_SRC) clean
	
rebuild_all: clean all
