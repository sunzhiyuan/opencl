# 
#   QUALCOMM, Advanced Content Group - Snapdragon SDK
#       Copyright (c) 2014 QUALCOMM Technologies Inc. 
#       All Rights Reserved.
#       QUALCOMM Proprietary/GTDR
#

# Used to give the path of the current file. This MUST be 
# defined at the start of Android.mk
LOCAL_PATH := $(call my-dir)

# Paths for source and includes files. 
SRC_PATH := ../../../src
INC_PATH := ../../../inc
TEX_INC  := ../../../../inc

# Points to a script that clears all the Module-description 
# variables (LOCAL_XXX)
include $(CLEAR_VARS)

# The name of the module being created. This is prepended with lib
# automatically and appended with either .so or .a as specified below.
LOCAL_MODULE := ASTCLibrary

# List of source files. 
LOCAL_SRC_FILES := 	$(SRC_PATH)/astc_averages_and_directions.cpp \
					$(SRC_PATH)/astc_block_sizes2.cpp \
					$(SRC_PATH)/astc_color_quantize.cpp \
					$(SRC_PATH)/astc_color_unquantize.cpp \
					$(SRC_PATH)/astc_compress_symbolic.cpp \
					$(SRC_PATH)/astc_compute_variance.cpp \
					$(SRC_PATH)/astc_decompress_symbolic.cpp \
					$(SRC_PATH)/astc_encoding_choice_error.cpp \
					$(SRC_PATH)/astc_find_best_partitioning.cpp \
					$(SRC_PATH)/astc_ideal_endpoints_and_weights.cpp \
					$(SRC_PATH)/astc_image_load_store.cpp \
					$(SRC_PATH)/astc_integer_sequence.cpp \
					$(SRC_PATH)/astc_kmeans_partitioning.cpp \
					$(SRC_PATH)/astc_ktx_dds.cpp \
					$(SRC_PATH)/astc_main.cpp \
					$(SRC_PATH)/astc_partition_tables.cpp \
					$(SRC_PATH)/astc_percentile_tables.cpp \
					$(SRC_PATH)/astc_pick_best_endpoint_format.cpp \
					$(SRC_PATH)/astc_quantization.cpp \
					$(SRC_PATH)/astc_stb_tga.cpp \
					$(SRC_PATH)/astc_symbolic_physical.cpp \
					$(SRC_PATH)/astc_toplevel.cpp \
					$(SRC_PATH)/astc_weight_align.cpp \
					$(SRC_PATH)/astc_weight_quant_xfer_tables.cpp \
					$(SRC_PATH)/mathlib.cpp \
					$(SRC_PATH)/softfloat.cpp

# Defining the include paths for compiling. We don't need to 
# include specific headers because that is dont automatically. 
LOCAL_C_INCLUDES := $(INC_PATH) $(TEX_INC) 

LOCAL_EXPORT_C_INCLUDES := $(INC_PATH)

# This forces compilation into 32-bit mode instead of (16 bit) 
# Thumb Mode
LOCAL_ARM_MODE := arm

# These flags apply ONLY to CPP files. 
LOCAL_CPPFLAGS += -O3 -Wall -Wextra -Wreorder 

# Telling the NDK to compile as a static lib.
include $(BUILD_STATIC_LIBRARY)

