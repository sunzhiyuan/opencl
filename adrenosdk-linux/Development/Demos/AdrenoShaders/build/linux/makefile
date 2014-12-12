# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := AdrenoShaders

# Path to source
SRC_DIR := ../../src
SRC_SCENE_DIR := ../../src/scenes
INC_DIR := ./..
SYS_DIR := ../../../../Inc
FRM_DIR := ../../../../Samples/Framework
FRM_OPENGL_DIR := ../../../../Samples/Framework/OpenGLES
FRM_LINUX_DIR := ../../../../Samples/Framework/Linux

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Demos/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32 -DOS_LINUX

INCLUDES := -I$(SRC_DIR) -I$(SRC_SCENE_DIR) -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR)

CPPFILES := 	$(SRC_DIR)/scene.cpp \
				$(SRC_DIR)/application.cpp \
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
		$(SRC_SCENE_DIR)/Scene50.cpp \
		$(FRM_OPENGL_DIR)/FrmFontGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmMesh.cpp \
		$(FRM_OPENGL_DIR)/FrmPackedResourceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmResourceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmShader.cpp \
		$(FRM_OPENGL_DIR)/FrmUserInterfaceGLES.cpp \
		$(FRM_OPENGL_DIR)/FrmUtilsGLES.cpp \
		$(FRM_DIR)/FrmFont.cpp \
		$(FRM_DIR)/FrmInput.cpp \
		$(FRM_DIR)/FrmPackedResource.cpp \
		$(FRM_DIR)/FrmUserInterface.cpp \
		$(FRM_DIR)/FrmUtils.cpp \
		$(FRM_DIR)/FrmApplication.cpp \
		$(FRM_LINUX_DIR)/FrmApplication_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmFile_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmStdlib_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmNoise_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmUtils_Platform.cpp

		
CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L../../../../Lib/ubuntu/i386/ -L../../../../../Bin
LIBRARIES := -lTextureConverter -lGLESv2 -lEGL -lrt -lX11 -ldl -lGL


# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(TARGET): $(OBJECTS)
	@echo "=== Linking ==="
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(INCLUDES) $(LIB_PATH) -o $@ $(OBJECTS) $(LIBRARIES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "=== Compiling SRC_DIR ==="
	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(SRC_SCENE_DIR)/%.cpp
	@echo "=== Compiling SRC_SCENE_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)
	
$(BUILD_DIR)/%.o: $(FRM_DIR)/%.cpp
	@echo "=== Compiling FRM_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_LINUX_DIR)/%.cpp 
	@echo "=== Compiling FRM_LINUX_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

#$(BUILD_DIR)/%.o: $(FRM_LINUX_DIR)/%.inl 
#	@echo "=== Compiling FRM_LINUX_DIR ==="
#	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)
	
$(BUILD_DIR)/%.o: $(FRM_OPENGL_DIR)/%.cpp
	@echo "=== Compiling FRM_OPENGL_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

clean:
	@echo "=== Cleaning ==="
	rm -f $(OBJECTS)
	
rebuild_all: clean $(TARGET)

