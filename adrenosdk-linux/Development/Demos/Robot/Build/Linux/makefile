# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Snapdragon Toolkit
# 

PROJECT_NAME = Robot

#Path to source
SRC_DIR := ../../src
INC_DIR := ../../src
SYS_DIR := ../../../../Inc
FRM_DIR := ../../../../Samples/Framework
FRM_OPENGL_DIR := ./../../../../Samples/Framework/OpenGLES
FRM_LINUX_DIR := ./../../../../Samples/Framework/Linux

BUILD_DIR := ./../../../../Intermediate/Demos/$(PROJECT_NAME)/Linux

TARGET_DIR := ./../../../../../Bin

TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32 -DOS_LINUX

INCLUDES := -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR)

CPPFILES := $(SRC_DIR)/scene.cpp \
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
			$(FRM_LINUX_PATH)/FrmApplication_Platform.cpp \
			$(FRM_LINUX_PATH)/FrmFile_Platform.cpp \
			$(FRM_LINUX_PATH)/FrmStdlib_Platform.cpp \
			$(FRM_LINUX_PATH)/FrmUtils_Platform.cpp \
			$(FRM_LINUX_PATH)/FrmNoise_Platform.cpp \

CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L../../../../Lib/ubuntu/ -L../../../../../Bin
LIBRARIES := -lTextureConverter -lGLESv2 -lEGL -lrt -lX11 -ldl 

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
	
rebuild_all: clean $(TARGET)



