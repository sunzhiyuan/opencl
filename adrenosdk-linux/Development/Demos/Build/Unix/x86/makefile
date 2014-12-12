# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# Bloom OpenGL ES Sample makefile for OSX, tested on OSX
# FreeImage must be installed on your OSX system

ifeq ($(MODE),debug)
   $(info == DEBUG == )
   CFLAGS := -pipe -g -fPIC -m32 -arch i386 $(ADDED_DEFS)
   LIBRARIES := -lGLESv2_d -lEGL_d -lX11 -ldl -lGL
   TARGET_DIR := ../../../../../Bin_d
   BIN_DIR_NAME:=Bin_d
else
   MODE:=release
   $(info == RELEASE == )
   CFLAGS := -w -O3 -pipe -g -fPIC -m32 -arch i386 $(ADDED_DEFS)
   LIBRARIES := -lGLESv2 -lEGL -lX11 -ldl -lGL 
   TARGET_DIR := ../../../../../Bin
   BIN_DIR_NAME:=Bin
endif


PROJECT_BIN_NAME?=$(PROJECT_NAME)

# Path to source
SRC_DIR := ../../../$(PROJECT_NAME)/src
INC_DIR := ../../../$(PROJECT_NAME)/src
SYS_DIR := ../../../../Inc
FRM_DIR := ../../../../Samples/Framework
FRM_OPENGL_DIR := ../../../../Samples/Framework/OpenGLES
FRM_LINUX_DIR := ../../../../Samples/Framework/Linux
X11_DIR := /opt/X11/include


# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Demos/$(PROJECT_NAME)/OSX/$(ARCH)


# executable goes to Bin
TARGET := $(TARGET_DIR)/$(PROJECT_BIN_NAME)
TARGET_APP := $(TARGET).app
INCLUDES := -I$(INC_DIR) -I$(SYS_DIR) -I$(FRM_DIR) -I$(FRM_OPENGL_DIR) -I$(FRM_LINUX_DIR) -I$(X11_DIR) -I$(CROSSPLATFORM_DIR)
SRC_DIRS:= $(SRC_DIR) $(FRM_DIR) $(FRM_LINUX_DIR) $(FRM_OPENGL_DIR) $(ADDED_SRC_DIRS)


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
		$(FRM_LINUX_DIR)/FrmApplication_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmFile_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmStdlib_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmNoise_Platform.cpp \
		$(FRM_LINUX_DIR)/FrmUtils_Platform.cpp \
		$(ADDITIONAL_SRC)

CPPSTRIP := $(notdir $(CPPFILES:.cpp=.o))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(CPPFILES:.cpp=.o)))

# Libraries
LIB_PATH := -L./ -L../../../../Lib/osx/x86 -L../Development/Lib/osx/x86 -L/opt/X11/lib -L/System/Library/Frameworks/OpenGL.framework/Versions/Current/Libraries/



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
	@echo "=== Creating .app ==="
	install_name_tool -change ../../../../Lib/osx/x86/libEGL.dylib ./libEGL.dylib $(TARGET)
	install_name_tool -change ../../../../Lib/osx/x86/libGLESv2.dylib ./libGLESv2.dylib $(TARGET)
	mkdir -p $(TARGET_APP)
	mkdir -p $(TARGET_APP)/Contents
	mkdir -p $(TARGET_APP)/Contents/MacOS
	mkdir -p $(TARGET_APP)/Contents/Resources
	chmod 555 ../../OSX/ES_Icon.icns
	chmod 555 ../../OSX/Info.plist
	chmod 555 ../../OSX/Run.sh
	chmod 555 $(TARGET_DIR)/libGLESv2.dylib
	chmod 555 $(TARGET_DIR)/libEGL.dylib
	chmod 555 $(TARGET)
	chmod 555 $(TARGET_DIR)/libTextureConverter.dylib
	chmod -R 777 $(TARGET_APP)/*
	cp ../../OSX/ES_Icon.icns $(TARGET_APP)/Contents/Resources
	cp ../../OSX/Info.plist $(TARGET_APP)/Contents/
	cp ../../OSX/Run.sh $(TARGET_APP)/Contents/MacOS/
	cp $(TARGET) $(TARGET).app/Contents/MacOS/Demo
	cp $(TARGET_DIR)/libGLESv2.dylib $(TARGET).app/Contents/MacOS/
	cp $(TARGET_DIR)/libEGL.dylib $(TARGET).app/Contents/MacOS/
	cp $(TARGET_DIR)/libTextureConverter.dylib $(TARGET).app/Contents/MacOS/
	rm -f $(TARGET)
	cd ../../../$(PROJECT_NAME)/Build/OSX/; chmod 555 ./InstallAssets.sh;./InstallAssets.sh $(BIN_DIR_NAME) $(PROJECT_NAME)


$(foreach curTargetDir,$(SRC_DIRS),$(eval $(call CREATE_SOURCE_TARGET,$(curTargetDir))))



clean:
ifdef PREBUILD_SCRIPT
	chmod +x $(PREBUILD_SCRIPT)
	$(PREBUILD_SCRIPT) clean
endif
	
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(TARGET_APP)
	
rebuild_all: clean $(TARGET)

