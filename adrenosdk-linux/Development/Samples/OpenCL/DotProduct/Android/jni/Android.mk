# Copyright (c) 2013 QUALCOMM Technologies, Inc.
LOCAL_PATH := $(call my-dir)
SRC_PATH   := ../..

APP_CFLAG := -DQC_OPTS
CL_TEST_CHECK := true
SDK_SAMPLE_NAME := DotProduct
include $(SRC_PATH)/../Android_common.mk
