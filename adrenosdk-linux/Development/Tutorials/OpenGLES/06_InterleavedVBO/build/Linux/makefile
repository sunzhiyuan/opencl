# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := InterleavedVBO

# Path to source
SRC_DIR := ./../../src
INC_DIR := ./../../inc
SYS_DIR := ../../../../../Inc
FRM_DIR := ../../../../../Samples/Framework
FRM_OPENGL_DIR := ../../../../../Samples/Framework/OpenGLES
FRM_LINUX_DIR := ../../../../../Samples/Framework/Linux

# BUILD object files
BUILD_DIR := ../../../../../../Intermediate/Samples/OpenGLES/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32

INCLUDES := -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR)

CPPFILES := 	$(SRC_DIR)/Scene.cpp \
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
LIB_PATH := -L../../../../../Lib/ubuntu/ -L../../../../../../Bin
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

$(BUILD_DIR)/%.o: $(FRM_DIR)/%.cpp
	@echo "=== Compiling FRM_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_LINUX_DIR)/%.cpp 
	@echo "=== Compiling FRM_LINUX_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

$(BUILD_DIR)/%.o: $(FRM_OPENGL_DIR)/%.cpp
	@echo "=== Compiling FRM_OPENGL_DIR ==="
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)

clean:
	@echo "=== Cleaning ==="
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	
rebuild_all: clean $(TARGET)

