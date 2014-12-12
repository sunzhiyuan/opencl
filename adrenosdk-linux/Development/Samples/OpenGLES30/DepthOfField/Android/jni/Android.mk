#
#             QUALCOMM, Advanced Content Group - Snapdragon SDK
#
#                Copyright (c) 2013 QUALCOMM Technologies, Inc. 
#                            All Rights Reserved. 
#                         QUALCOMM Proprietary/GTDR
#

LOCAL_PATH := $(call my-dir)
SRC_PATH			:= ../..
FRM_PATH			:= $(SRC_PATH)/../../Framework
FRM_OPENGL_PATH		:= $(FRM_PATH)/OpenGLES
FRM_ANDROID_PATH	:= $(FRM_PATH)/Android
INC_PATH	        := ../../../../../Inc

include $(CLEAR_VARS)

# Use this to enable/disable NEON (disabling switches to VFP mode)
ENABLE_NEON		:= true

ifeq ($(ENABLE_NEON),true)
	# NEON mode
	# Enable or disable NEON. Don't forget to apply, or not apply, -mfpu=neon and -mfloat-abi=softfp
	# flags in addition, e.g., if this is true both of those need to be included in LOCAL_CFLAGS
	# to avoid the possibility that ndk-build will "forget" to add them on some files
	LOCAL_ARM_NEON := true
	TARGET_CFLAGS := $(filter-out -ffpu=vfp,$(TARGET_CFLAGS))
	LOCAL_CFLAGS := -g $(LOCAL_C_INCLUDES:%=-I%) -DHALTI -D_OGLES3 -DUSE_GLES3_WRAPPER 
	LOCAL_CFLAGS += -DUSE_PTHREADS -mfpu=neon -mfloat-abi=softfp -pthread -DSCE_PFX_USE_SIMD_VECTORMATH 
	LOCAL_CFLAGS += -DOS_ANDROID  -ffast-math -funsafe-math-optimizations -fno-strict-aliasing
	LOCAL_CFLAGS += -Wno-conversion-null -Wno-write-strings
else
	# VFP mode
	LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%) -DUSE_PTHREADS -mfpu=vfp -mfloat-abi=softfp -pthread -DOS_ANDROID
	LOCAL_CFLAGS += -Wno-conversion-null -Wno-write-strings 
endif

LOCAL_CFLAGS += -DCORE_GL_CONTEXT

LOCAL_MODULE := DepthOfFieldOGLES30

LOCAL_SRC_FILES := $(SRC_PATH)/Scene.cpp \
				   $(FRM_OPENGL_PATH)/FrmGLES3.cpp \
                   $(FRM_OPENGL_PATH)/FrmFontGLES.cpp \
                   $(FRM_OPENGL_PATH)/FrmMesh.cpp \
                   $(FRM_OPENGL_PATH)/FrmPackedResourceGLES.cpp \
                   $(FRM_OPENGL_PATH)/FrmResourceGLES.cpp \
                   $(FRM_OPENGL_PATH)/FrmShader.cpp \
                   $(FRM_OPENGL_PATH)/FrmUserInterfaceGLES.cpp \
                   $(FRM_OPENGL_PATH)/FrmUtilsGLES.cpp \
                   $(FRM_PATH)/FrmApplication.cpp \
                   $(FRM_PATH)/FrmFont.cpp \
                   $(FRM_PATH)/FrmInput.cpp \
                   $(FRM_PATH)/FrmPackedResource.cpp \
                   $(FRM_PATH)/FrmUserInterface.cpp \
                   $(FRM_PATH)/FrmUtils.cpp \
                   $(FRM_ANDROID_PATH)/FrmApplication_Platform.cpp \
                   $(FRM_ANDROID_PATH)/FrmFile_Platform.cpp \
                   $(FRM_ANDROID_PATH)/FrmStdlib_Platform.cpp \
                   $(FRM_ANDROID_PATH)/FrmUtils_Platform.cpp


LOCAL_C_INCLUDES := $(SRC_PATH) \
					$(INC_PATH) \
                    $(FRM_PATH)	\
                    $(FRM_OPENGL_PATH) \
                    $(FRM_ANDROID_PATH)
				   
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3

LOCAL_CFLAGS += -Wno-psabi -Wno-conversion-null 

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
