# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

GLFW_PROJECT_NAME := libGLFW

ifeq ($(MODE),debug)
   $(info == DEBUG == )
   CFLAGS := -w -O3 -pipe -g -fPIC -m32 -arch i386 -DOS_OSX
   LIBRARIES := -lTextureConverter_d -lGLESv2_d -lGLFW_d -lEGL_d -lX11 -ldl -lGL
   TARGET_DIR := ../../../../../../Bin_d
   BIN_DIR_NAME:=Bin_d
   GLFW_PROJECT_NAME := libGLFW_d
else
   MODE:=release
   $(info == RELEASE == )
   CFLAGS := -w -O3 -pipe -g -fPIC -m32 -arch i386 -DOS_OSX 
   LIBRARIES := -lTextureConverter -lGLESv2 -lGLFW -lEGL -lX11 -ldl -lGL
   TARGET_DIR := ../../../../../../Bin
   BIN_DIR_NAME:=Bin
   GLFW_PROJECT_NAME := libGLFW
endif

GLFW_TARGET := $(GLFW_PROJECT_NAME).dylib

USE_CORE_CONTEXT := false
ifeq ($(USE_CORE_CONTEXT),true)
CFLAGS := $(CFLAGS) -D CORE_GL_CONTEXT
endif

PROJECT_BIN_NAME?=$(PROJECT_NAME)

# Path to source
SRC_DIR := ../../../$(SAMPLE_DIR)/src
INC_DIR := ../../../$(SAMPLE_DIR)/inc
SYS_DIR := ../../../../../Inc
FRM_DIR := ../../../../../Samples/Framework
FRM_OPENGL_DIR := ../../../../../Samples/Framework/OpenGLES
FRM_LINUX_DIR := ../../../../../Samples/Framework/Linux
X11_DIR := /opt/X11/include
GLFW_DIR := ../../../../../External/glfw-3.0.4/include

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tutorials/$(SAMPLE_DIR)/OSX/$(ARCH)


# executable goes to Bin
TARGET := $(TARGET_DIR)/$(PROJECT_BIN_NAME)
TARGET_APP := $(TARGET).app
INCLUDES := -I$(GLFW_DIR) -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR) -I$(X11_DIR)
SRC_DIRS:= $(SRC_DIR) $(FRM_DIR) $(FRM_LINUX_DIR) $(FRM_OPENGL_DIR)


CPPFILES := $(SRC_DIR)/Scene.cpp \
		$(FRM_DIR)/FrmApplication.cpp \
		$(FRM_DIR)/FrmFont.cpp \
		$(FRM_DIR)/FrmInput.cpp \
		$(FRM_DIR)/FrmPackedResource.cpp \
		$(FRM_DIR)/FrmUserInterface.cpp \
		$(FRM_DIR)/FrmUtils.cpp \
		$(FRM_DIR)/FrmAppContainer.cpp \
		$(FRM_LINUX_DIR)/FrmFile_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmStdlib_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmUtils_Platform.cpp \

CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L./ -L../../../../../Lib/osx/$(ARCH) -L$(TARGET_DIR) -L/opt/X11/lib -L/System/Library


# Macro for creating a target that builds .o files given .cpp files from a directory
# specified by the first parameter.
# NOTE: $$ are because there are two expansions that occur, one to expand the macro when it's $(call)'d and second when
# it's $(eval)'d by make.
define CREATE_SOURCE_TARGET

$$(BUILD_DIR)/%.o: $(1)/%.cpp
	@echo "mkdir for $${BUILD_DIR}"
	mkdir -p $$(dir $$@)
	g++ -c $$(CFLAGS) $$(INCLUDES) $$(LIB_PATH) $$< -o $$@ $$(LIBRARIES)

endef

# force rebuild. Comment to take out a force rebuild.
.PHONY: clean


$(TARGET): $(OBJECTS)
ifdef PREBUILD_SCRIPT
	@echo "Running pre-build script: $(PREBUILD_SCRIPT)"
	chmod +x $(PREBUILD_SCRIPT)
	$(PREBUILD_SCRIPT)
endif
	@echo "=== Linking ==="
	
		mkdir -p $(dir $@)
	g++ $(CFLAGS) $(INCLUDES) $(LIB_PATH) -o $@ $(OBJECTS) $(LIBRARIES)
	install_name_tool -change ../../../../Lib/osx/x86/libTextureConverter.dylib ./libTextureConverter.dylib $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/libEGL.dylib ./libEGL.dylib $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/libGLESv2.dylib ./libGLESv2.dylib $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/$(GLFW_TARGET) ./$(GLFW_TARGET) $(TARGET)
ifeq ($(wildcard ../../../$(SAMPLE_DIR)/build/OSX/InstallAssets.sh),)
	@echo "===No InstallAssets.sh==="
else
	@echo "===Calling InstallAssets.sh==="
	cd ../../../$(SAMPLE_DIR)/build/OSX/; chmod 555 InstallAssets.sh; ./InstallAssets.sh $(BIN_DIR_NAME) $(PROJECT_NAME)
endif

$(foreach curTargetDir,$(SRC_DIRS),$(eval $(call CREATE_SOURCE_TARGET,$(curTargetDir))))



clean:
ifdef PREBUILD_SCRIPT
	chmod +x $(PREBUILD_SCRIPT)
	$(PREBUILD_SCRIPT) clean
endif
	
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(TARGET_APP)
	
rebuild_all: clean $(TARGET)

