LOCAL_PATH:= $(call my-dir)

# By default build both CPU and GPU, set these to 0 on command line to build only GPU or CPU
BUILD_CPU ?= 1
BUILD_GPU ?= 1

commonSrcFiles := hello_cl.cpp
commonSrcFiles += hello_main.c
commonIncludes := vendor/qcom/proprietary/gles/adreno200/include/public/
commonCFlags := -DBUILD_CONSOLE_APP

ifeq ($(BUILD_GPU), 1)
include $(CLEAR_VARS)
LOCAL_CFLAGS := -DUSE_GPU -c -g -O0  -W 
LOCAL_CFLAGS += $(commonCFlags)
LOCAL_SRC_FILES:= $(commonSrcFiles)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_MODULE:= hello-cl-gpu
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/graphics_tests/opencl/cl_examples
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES  += libcutils libOpenCL
include $(BUILD_EXECUTABLE)
endif

ifeq ($(BUILD_CPU), 1)
include $(CLEAR_VARS)
LOCAL_CFLAGS := -DUSE_CPU -c -g -O0  -W  
LOCAL_CFLAGS += $(commonCFlags)
LOCAL_SRC_FILES:= $(commonSrcFiles)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_MODULE:= hello-cl-cpu
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/graphics_tests/opencl/cl_examples
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES  += libcutils libOpenCL
include $(BUILD_EXECUTABLE)
endif
