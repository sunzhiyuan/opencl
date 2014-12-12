# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# ResourcePacker makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

ARCH=i386
LIB_DIR=../../../../Lib/ubuntu/$(ARCH)

PROJECT_NAME := ResourcePacker

TEXTURE_CONVERTER_MAKE :=../../../TextureConverter/build/ubuntu 

# Path to source
SRC_DIR := ../..
INC_DIR := ../..
SYS_INC := ../../../../Inc
FREEIMAGE_INC := ../../../../External/FreeImage/3.16
TEXTURECONVERTER_INC_DIR := ../../../../Tools/TextureConverter/inc
TINYXML_INC := ../../../../External/TinyXML

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tools/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32 -DCOMPILE_GLES3_FORMATS

INCLUDES := -I$(INC_DIR) -I$(SYS_INC) -I$(TINYXML_INC) -I$(TEXTURECONVERTER_INC_DIR) -I$(FREEIMAGE_INC)

CPPFILES := $(notdir $(wildcard $(SRC_DIR)/*.cpp))
TINYXML_FILES := $(TINYXML_INC)/tinyxml.cpp
OBJECTS := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o)) 
TINYXML_OBJ := $(BUILD_DIR)/tinyxml.o

# Libraries
LIB_PATH := -L$(LIB_DIR) -L../../../../External/FreeImage/3.16/ubuntu/x86
LIBRARIES := -lfreeimage -lTextureConverter -lpthread


# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(TARGET): $(OBJECTS)
	#cp ../../../../External/FreeImage/3.16/ubuntu/x86/libfreeimage.so ../../../../../Bin/libfreeimage.so
	mkdir -p $(dir $@)	
	ln -s -f ../Development/External/FreeImage/3.16/ubuntu/x86/libfreeimage-3.16.0.so ../../../../../Bin/libfreeimage.so
	cp -pf ./Run_ResourcePacker.sh ../../../../../Bin/Run_ResourcePacker.sh
	g++ $(CFLAGS) $(INCLUDES) $(LIB_PATH) -o $@ $(OBJECTS) $(TINYXML_OBJ) $(LIBRARIES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	# TextureConverter dependency
	make -C $(TEXTURE_CONVERTER_MAKE)

	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES) $(LIB_PATH)  $< -o $@ $(LIBRARIES)
	g++ -c $(CFLAGS) $(INCLUDES) $(TINYXML_FILES) -o $(TINYXML_OBJ)

clean:
	# TextureConverter dependency
	make -C $(TEXTURE_CONVERTER_MAKE) clean	
	rm -f $(TARGET) $(OBJECTS) $(TINYXML_OBJ)

rebuild_all: clean $(TARGET)
	

