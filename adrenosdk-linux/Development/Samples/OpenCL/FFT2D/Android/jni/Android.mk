# Copyright (c) 2013 QUALCOMM Technologies, Inc.
LOCAL_PATH := $(call my-dir)
SRC_PATH   := ../..

CL_TEST_CHECK := true
SDK_SAMPLE_NAME := FFT2D
SRC_FILES := $(SRC_PATH)/FFT2D.cpp
include $(SRC_PATH)/../Android_common.mk
