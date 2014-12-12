# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# DepthOfField makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := DepthOfField

# Path to source
SRC_DIR := ./..
INC_DIR := ./..
SYS_DIR := ../../../../Inc
FRM_DIR := ../../../Framework
FRM_OPENGL_DIR := ../../../Framework/OpenGLES
FRM_LINUX_DIR := ../../../Framework/Linux

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Samples/OpenGLES/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32

INCLUDES := -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR)

CPPFILES := 	$(SRC_DIR)/Scene.cpp \
		$(FRM_OPENGL_DIR)/FrmFontGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmMesh.cpp \
		$(FRM_OPENGL_DIR)/FrmPackedResourceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmResourceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmShader.cpp \
		$(FRM_OPENGL_DIR)/FrmUserInterfaceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmUtilsGLES.cpp \
		$(FRM_DIR)/FrmApplication.cpp \
		$(FRM_DIR)/FrmFont.cpp \
		$(FRM_DIR)/FrmInput.cpp \
		$(FRM_DIR)/FrmPackedResource.cpp \
		$(FRM_DIR)/FrmUserInterface.cpp \
		$(FRM_DIR)/FrmUtils.cpp \
		$(FRM_LINUX_DIR)/FrmApplication_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmFile_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmStdlib_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmUtils_Platform.cpp

CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L../../../../Lib/ubuntu/ -L../../../../../Bin
LIBRARIES := -lTextureConverter -lGLESv2 -lEGL -lrt -lX11 -ldl -lGL


# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(INCLUDES) $(LIB_PATH) -o $@ $(OBJECTS) $(LIBRARIES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_DIR)/%.cpp
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_LINUX_DIR)/%.cpp 
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_OPENGL_DIR)/%.cpp
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

clean:
	rm -f $(TARGET) $(OBJECTS)

rebuild_all: clean $(TARGET)

