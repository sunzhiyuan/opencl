# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

EGL_PROJECT_NAME := libEGL
GLES_PROJECT_NAME := libGLESv2
GLFW_PROJECT_NAME := libGLFW
TEXCONV_PROJECT_NAME := libTextureConverter
USE_CORE_CONTEXT := false

ifeq ($(MODE),debug)
   $(info == DEBUG == )
   CFLAGS := -pipe -g -fPIC -m32 -arch i386 $(ADDED_DEFS)
   LIBRARIES := -lGLESv2_d -lEGL_d -lGLFW_d -lX11 -ldl -lGL
   TARGET_DIR := ../../../../../Bin_d
   BIN_DIR_NAME:=Bin_d
   EGL_PROJECT_NAME := libEGL_d
   GLES_PROJECT_NAME := libGLESv2_d
   GLFW_PROJECT_NAME := libGLFW_d
   TEXCONV_PROJECT_NAME := libTextureConverter_d
else
   MODE:=release
   $(info == RELEASE == )
   CFLAGS := -w -O3 -pipe -g -fPIC -m32 -arch i386 $(ADDED_DEFS)
   LIBRARIES := -lGLESv2 -lGLESv2 -lGLFW -lEGL -lX11 -ldl -lGL 
   TARGET_DIR := ../../../../../Bin
   BIN_DIR_NAME:=Bin
   EGL_PROJECT_NAME := libEGL
   GLES_PROJECT_NAME := libGLESv2
   GLFW_PROJECT_NAME := libGLFW
   TEXCONV_PROJECT_NAME := libTextureConverter
endif

ifeq ($(USE_CORE_CONTEXT),true)
CFLAGS := $(CFLAGS) -D CORE_GL_CONTEXT
endif

EGL_TARGET := $(EGL_PROJECT_NAME).dylib
GLES_TARGET := $(GLES_PROJECT_NAME).dylib
GLFW_TARGET := $(GLFW_PROJECT_NAME).dylib
TEXCONV_TARGET := $(TEXCONV_PROJECT_NAME).dylib

ifeq ($(TARGET_NAME),)
TARGET_APP_NAME := $(PROJECT_NAME)
else
TARGET_APP_NAME := $(TARGET_NAME)
endif

PROJECT_BIN_NAME := $(TARGET_APP_NAME)

# Path to source
SRC_DIR := ../../../$(SAMPLE_DIR)/$(PROJECT_NAME)
INC_DIR := ../../../$(SAMPLE_DIR)/$(PROJECT_NAME)
SYS_DIR := ../../../../Inc
FRM_DIR := ../../../Framework
FRM_OPENGL_DIR := ../../../Framework/OpenGLES
FRM_LINUX_DIR := ../../../Framework/Linux
X11_DIR := /opt/X11/include
GLFW_DIR := ../../../../External/glfw-3.0.4/include

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Samples/$(SAMPLE_DIR)/$(PROJECT_NAME)/OSX/$(ARCH)


# executable goes to Bin
TARGET := $(TARGET_DIR)/$(PROJECT_BIN_NAME)
TARGET_APP := $(TARGET).app
INCLUDES := -I$(GLFW_DIR) -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR) -I$(X11_DIR) -I$(CROSSPLATFORM_DIR)
SRC_DIRS:= $(SRC_DIR) $(FRM_DIR) $(FRM_LINUX_DIR) $(FRM_OPENGL_DIR) $(ADDED_SRC_DIRS)


CPPFILES := $(SRC_DIR)/Scene.cpp \
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
		$(FRM_DIR)/FrmAppContainer.cpp \
		$(FRM_LINUX_DIR)/FrmFile_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmStdlib_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmUtils_Platform.cpp \
		$(ADDITIONAL_SRC)

CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L./ -L$(TARGET_DIR) -L/opt/X11/lib -L/System/Library/Frameworks/OpenGL.framework/Versions/Current/Libraries/



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
	install_name_tool -change ../../../../Lib/osx/x86/$(EGL_TARGET) ./$(EGL_TARGET) $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/$(GLES_TARGET) ./$(GLES_TARGET) $(TARGET)
	#install_name_tool -change ../../../../Lib/osx/x86/$(TEXCONV_TARGET) ./$(TEXCONV_TARGET) $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/$(GLFW_TARGET) ./$(GLFW_TARGET) $(TARGET)
ifeq ($(wildcard ../../../$(SAMPLE_DIR)/$(PROJECT_NAME)/OSX/InstallAssets.sh),)
	@echo "===No InstallAssets.sh==="
else
	cd ../../../$(SAMPLE_DIR)/$(PROJECT_NAME)/OSX/; chmod 555 ./InstallAssets.sh;./InstallAssets.sh $(BIN_DIR_NAME) $(PROJECT_NAME)
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

