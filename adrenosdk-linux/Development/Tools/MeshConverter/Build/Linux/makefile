# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# MeshConverter makefile for Linux, tested on Ubuntu
# FreeImage must be installed on your Linux system
# To install FreeImage: sudo apt-get install libfreeimage-dev

PROJECT_NAME := MeshConverter

# Path to source
SRC_DIR := ../..
INC_DIR := ../..
SYS_INC := ../../../../Inc

# BUILD object files
BUILD_DIR := ../../../../../Intermediate/Tools/$(PROJECT_NAME)/Linux

# executable goes to Bin
TARGET_DIR := ../../../../../Bin
TARGET = $(TARGET_DIR)/$(PROJECT_NAME)

CFLAGS := -w -O3 -pipe -g -fPIC -m32

INCLUDES := -I$(INC_DIR) -I$(SYS_INC)

CPPFILES := $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o)) 

# force rebuild. Comment to take out a force rebuild.
.PHONY: clean

$(TARGET): $(OBJECTS)
	mkdir -p $(dir $@)
	g++ $(CFLAGS) $(INCLUDES) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	g++ -c $(CFLAGS) $(INCLUDES)   $< -o $@ $(LIBRARIES)

clean:
	rm -f $(TARGET) $(OBJECTS)


rebuild_all: clean $(TARGET)
