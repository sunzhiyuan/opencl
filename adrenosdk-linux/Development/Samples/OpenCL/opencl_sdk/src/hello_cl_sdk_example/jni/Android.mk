LOCAL_PATH:= $(call my-dir)
JNI_DIR := $(call my-dir)

SRC_PATH:= ..
MODULE_SRC_FILES += $(SRC_PATH)/src/hello_cl_android_native.cpp
MODULE_SRC_FILES += $(SRC_PATH)/../hello_cl/hello_cl.cpp
MODULE_INC_FILES := inc
MODULE_INC_FILES += $(LOCAL_PATH)/../../hello_cl

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DUSE_GPU -DOS_ANDROID_NATIVE
LOCAL_CFLAGS += -DANDROID_CL 
LOCAL_SRC_FILES:= $(MODULE_SRC_FILES)

LOCAL_C_INCLUDES := $(MODULE_INC_FILES)
LOCAL_MODULE:= hello_cl_gpu
LOCAL_MODULE_TAGS := none

LOCAL_LDLIBS := \
-lOpenCL \
-lEGL \
-lGLESv2 \
-llog \
-landroid

LOCAL_STATIC_LIBRARIES := android_native_app_glue
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
