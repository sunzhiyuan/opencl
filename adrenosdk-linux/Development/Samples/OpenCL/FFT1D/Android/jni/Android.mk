# Copyright (c) 2013 QUALCOMM Technologies, Inc.
LOCAL_PATH := $(call my-dir)
SRC_PATH   := ../..

CL_TEST_CHECK := true
SDK_SAMPLE_NAME := FFT1D
SRC_FILES := $(SRC_PATH)/FFT1D.cpp
include $(SRC_PATH)/../Android_common.mk
