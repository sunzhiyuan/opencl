# Copyright (c) 2013 Qualcomm Technologies, Inc.
# Adreno SDK
# ASTC Compression Library makefile for Linux, tested on Ubuntu

ARCH = i386
ARCH_FLAGS = -m32
ifeq ($(ARCH),x64)
ARCH_FLAGS = -m64 -D _Linux_x64
endif

PROJECT_NAME := ASTCLibrary
LIB_NAME := lib$(PROJECT_NAME).a

INC_DIR := ../../inc
SRC_DIR := ../../src
BUILD_DIR := ../../../../../../Intermediate/Tools/TextureConverter/ubuntu/$(ARCH)
TARGET_DIR := ../../../../../Lib/ubuntu/$(ARCH)

INC_PATH := -I$(INC_DIR) -I$(SRC_DIR) -I../../../inc

SOURCES := \
	$(SRC_DIR)/astc_averages_and_directions.cpp \
	$(SRC_DIR)/astc_block_sizes2.cpp \
	$(SRC_DIR)/astc_color_quantize.cpp \
	$(SRC_DIR)/astc_color_unquantize.cpp \
	$(SRC_DIR)/astc_compress_symbolic.cpp \
	$(SRC_DIR)/astc_decompress_symbolic.cpp \
	$(SRC_DIR)/astc_encoding_choice_error.cpp \
	$(SRC_DIR)/astc_find_best_partitioning.cpp \
	$(SRC_DIR)/astc_ideal_endpoints_and_weights.cpp \
	$(SRC_DIR)/astc_image_load_store.cpp \
	$(SRC_DIR)/astc_integer_sequence.cpp \
	$(SRC_DIR)/astc_ktx_dds.cpp \
	$(SRC_DIR)/astc_kmeans_partitioning.cpp \
	$(SRC_DIR)/astc_partition_tables.cpp \
	$(SRC_DIR)/astc_percentile_tables.cpp \
	$(SRC_DIR)/astc_pick_best_endpoint_format.cpp \
	$(SRC_DIR)/astc_quantization.cpp \
	$(SRC_DIR)/astc_stb_tga.cpp \
	$(SRC_DIR)/astc_symbolic_physical.cpp \
	$(SRC_DIR)/astc_weight_align.cpp \
	$(SRC_DIR)/astc_weight_quant_xfer_tables.cpp \
	$(SRC_DIR)/astc_compute_variance.cpp \
	$(SRC_DIR)/astc_toplevel.cpp \
	$(SRC_DIR)/astc_main.cpp \
	$(SRC_DIR)/mathlib.cpp \
	$(SRC_DIR)/softfloat.cpp

HEADERS := \
	$(INC_DIR)/astc_codec_internals.h \
	$(INC_DIR)/astc_stdint.h \
	$(INC_DIR)/mathlib.h \
	$(INC_DIR)/softfloat.h \
	$(INC_DIR)/stb_image.c \
	$(INC_DIR)/TextureConverter.h

# This gets ONLY the names of the .cpp files. 
CPP_FILES := $(notdir $(SOURCES))

# Creates the names for the objects we are building.  
OBJECTS += $(addprefix $(BUILD_DIR)/, $(CPP_FILES:.cpp=.o))

# Flags copied from the makefile that came with the ASTC library. 
# The .m32 is temporary until we add 64 bit support. 
CPPFLAGS = -O3 -W -fPIC -Wextra -msse2 -mfpmath=sse $(ARCH_FLAGS)

# Primary goal of the makefile. Builds a static library for ASTC. 
$(LIB_NAME) : $(BUILD_DIR)/stb_image.o $(OBJECTS) 
	mkdir -p $(TARGET_DIR)
	ar rs $(TARGET_DIR)/$(LIB_NAME) $(OBJECTS) $(BUILD_DIR)/stb_image.o

# Compile all the object files in the BUILD_DIR using their 
#  corresponsing cpp files in the source directory. 
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c $(CPPFLAGS) $(INC_PATH) $< -o $@ 

# Let's compile this serparate to keep the rule clean
#  since stb_image.c is in the inc folder. 
$(BUILD_DIR)/stb_image.o: ../../inc/stb_image.c
	g++ -c -o $(CPPFLAGS) $< -o $@

# We should remove the goal lib and object files. 
clean:
	rm -f $(TARGET_DIR)/$(LIB_NAME) 
	rm -f $(BUILD_DIR)/stb_image.o
	rm -f $(OBJECTS)	
