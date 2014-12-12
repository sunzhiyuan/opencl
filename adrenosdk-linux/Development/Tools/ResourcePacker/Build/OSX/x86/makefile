# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# ResourcePacker makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := ResourcePacker

TEXTURE_CONVERTER_MAKE :=../../../../TextureConverter/build/osx

# Path to source
SRC_DIR := ../../..
INC_DIR := ../../..
SYS_INC := ../../../../../Inc
TINYXML_INC := ../../../../../External/TinyXML
TEXTURECONVERTER_INC_DIR := ../../../../../Tools/TextureConverter/inc
FREEIMAGE_INC:= ../../../../../External/FreeImage

# BUILD object files
BUILD_DIR := ../../../../../../Intermediate/Tools/$(PROJECT_NAME)/OSX/x86

# executable goes to Bin
TARGET_DIR := ../../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -std=gnu++98 -m32 -arch i386 -DCOMPILE_GLES3_FORMATS

INCLUDES := -I$(INC_DIR) -I$(SYS_INC) -I$(TINYXML_INC) -I$(FREEIMAGE_INC) -I$(TEXTURECONVERTER_INC_DIR)

CPPFILES := $(notdir $(wildcard $(SRC_DIR)/*.cpp))
TINYXML_FILES := $(TINYXML_INC)/tinyxml.cpp
OBJECTS := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o)) 
TINYXML_OBJ := $(BUILD_DIR)/tinyxml.o

# Libraries
LIB_PATH := -L../../../../../Lib/osx/x86 -L../../../../../External/FreeImage/3.16/osx/x86
LIBRARIES := -lstdc++ -lfreeimage -lTextureConverter -lpthread

#cp ../../../../../External/FreeImage/3.16/osx/libfreeimage.dylib ../../../../../../Bin/libfreeimage.dylib
# force rebuild. Comment to take out a force rebuild.
.PHONY: clean






$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	ln -s -f ../Development/External/FreeImage/3.16/osx/x86/libfreeimage-3.16.0.dylib ../../../../../../Bin/libfreeimage.dylib
	cp -pf ../../Linux/Run_ResourcePacker.sh ../../../../../../Bin/Run_ResourcePacker.sh
	g++ $(CFLAGS) $(INCLUDES) $(LIB_PATH) -o $@ $(OBJECTS) $(TINYXML_OBJ) $(LIBRARIES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	# TextureConverter dependency
	make -C $(TEXTURE_CONVERTER_MAKE)
	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)
	g++ -c $(CFLAGS) $(INCLUDES) $(TINYXML_FILES) -o $(TINYXML_OBJ)



clean:
	# TextureConverter dependency
	make -C $(TEXTURE_CONVERTER_MAKE) ARCH=x86 clean	
	rm -f $(TARGET) $(OBJECTS) $(TINYXML_OBJ)

rebuild_all: clean $(TARGET)
	

