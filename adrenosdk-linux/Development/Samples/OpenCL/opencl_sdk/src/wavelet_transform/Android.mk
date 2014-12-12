LOCAL_PATH:=$(call my-dir)
include $(LOCAL_PATH)/../common-defs.mk

############################### Build transformMain executable
## Build for GPU
include $(CLEAR_VARS)
include $(ADRENO_STL_INCLUDE)

LOCAL_SRC_FILES := 	\
	transformMain.cpp \
	clfunctions.cpp \
	refer_transform.cpp\
 
LOCAL_C_INCLUDES += $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDKUtil/include
LOCAL_CFLAGS += $(commonCflags)
LOCAL_SHARED_LIBRARIES := $(commonSharedLibs)
LOCAL_MODULE    := cl_demo_test_waveletransform
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/graphics_tests/opencl/cl_demo_tests/
include $(BUILD_EXECUTABLE)
 
$(shell cp $(TARGET_OUT_DATA)/graphics_tests/opencl/cl_demo_tests/cl_demo_test_waveletransform ../../../bin/cl_demo_tests/wavelet_transform)


