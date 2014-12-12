# Copyright (c) 2013 QUALCOMM Technologies, Inc.

FRM_PATH := $(SRC_PATH)/../../Framework
INC_PATH   := $(SRC_PATH)/../../../Inc
FRM_OPENCL_PATH := $(FRM_PATH)/OpenCL
FRM_OPENGL_PATH := $(FRM_PATH)/OpenGLES
FRM_ANDROID_PATH := $(FRM_PATH)/Android

include $(CLEAR_VARS)
commonSrcFiles := $(FRM_PATH)/FrmFont.cpp \
                                   $(FRM_OPENCL_PATH)/FrmKernel.cpp \
                                   $(FRM_PATH)/FrmInput.cpp \
                                   $(FRM_PATH)/FrmPackedResource.cpp \
                                   $(FRM_PATH)/FrmUserInterface.cpp \
                                   $(FRM_PATH)/FrmUtils.cpp \
                                   $(FRM_ANDROID_PATH)/FrmFile_Platform.cpp \
                                   $(FRM_ANDROID_PATH)/FrmStdlib_Platform.cpp \
                                   $(FRM_ANDROID_PATH)/FrmUtils_Platform.cpp

commonSrcFilesCL := $(commonSrcFiles) \
                                   $(SRC_PATH)/Compute.cpp \
                                   $(FRM_PATH)/FrmComputeApplication.cpp \
                                   $(FRM_ANDROID_PATH)/FrmComputeApplication_Platform.cpp

commonSrcFilesCLGLES := $(commonSrcFiles) \
                                   $(SRC_PATH)/Scene.cpp \
                                   $(FRM_OPENGL_PATH)/FrmFontGLES.cpp \
                                   $(FRM_OPENGL_PATH)/FrmMesh.cpp \
                                   $(FRM_OPENGL_PATH)/FrmPackedResourceGLES.cpp \
                                   $(FRM_OPENGL_PATH)/FrmResourceGLES.cpp \
                                   $(FRM_OPENGL_PATH)/FrmShader.cpp \
                                   $(FRM_OPENGL_PATH)/FrmUserInterfaceGLES.cpp \
                                   $(FRM_OPENGL_PATH)/FrmUtilsGLES.cpp \
                                   $(FRM_PATH)/FrmApplication.cpp \
                                   $(FRM_PATH)/FrmComputeGraphicsApplication.cpp \
                                   $(FRM_ANDROID_PATH)/FrmComputeGraphicsApplication_Platform.cpp \
                                   $(FRM_ANDROID_PATH)/FrmNoise_Platform.cpp

commonCFlags := -I$(FRM_PATH) -I$(FRM_PATH)/OpenCL -I$(FRM_PATH)/Android
commonCFlags += -Wno-psabi -Wno-conversion-null -Wno-write-strings
commonCFlagsCL := $(commonCFlags)
commonCFlagsCLGLES := $(commonCFlags) -I$(FRM_INC_PATH)/OpenGLES

commonCIncludes := $(INC_PATH) $(SRC_PATH) $(FRM_PATH) $(FRM_OPENCL_PATH) $(FRM_ANDROID_PATH)
commonCIncludesCL := $(commonCIncludes)
commonCIncludesCLGLES := $(commonCIncludes) $(FRM_OPENGL_PATH)

commonLocalLibs := -lOpenCL -llog -landroid
commonLocalLibsCL := $(commonLocalLibs)
commonLocalLibsCLGLES := $(commonLocalLibs) -lEGL -lGLESv2

LOCAL_CFLAGS += -DANDROID_CL
LOCAL_MODULE := $(SDK_SAMPLE_NAME)
LOCAL_SRC_FILES += $(SRC_FILES)

ifeq ($(CL_TEST_CHECK), true)
LOCAL_SRC_FILES += $(commonSrcFilesCL)
LOCAL_CFLAGS += $(commonCFlagsCL)
LOCAL_C_INCLUDES += $(commonCIncludesCL)
LOCAL_LDLIBS += $(commonLocalLibsCL)
else
LOCAL_SRC_FILES += $(commonSrcFilesCLGLES)
LOCAL_CFLAGS += $(commonCFlagsCLGLES)
LOCAL_C_INCLUDES += $(commonCIncludesCLGLES)
LOCAL_LDLIBS += $(commonLocalLibsCLGLES)
endif

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
