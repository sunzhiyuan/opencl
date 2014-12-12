# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH		:= $(call my-dir)
SRC_PATH			:= ../..
FRM_PATH			:= $(SRC_PATH)/../../Framework
INC_PATH			:= ../../../../../Inc
FRM_OPENGL_PATH		:= $(FRM_PATH)/OpenGLES
FRM_ANDROID_PATH	:= $(FRM_PATH)/Android
FRM_MODEL_PATH		:= $(FRM_PATH)/Model

include $(CLEAR_VARS)

LOCAL_MODULE    := Skinning2

LOCAL_SRC_FILES := \
				   $(SRC_PATH)/Scene.cpp \
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
				   $(FRM_ANDROID_PATH)/FrmUtils_Platform.cpp \
				   $(FRM_ANDROID_PATH)/FrmNoise_Platform.cpp \
				   $(FRM_MODEL_PATH)/FrmAnimation.cpp \
				   $(FRM_MODEL_PATH)/FrmAnimationTrack.cpp \
				   $(FRM_MODEL_PATH)/FrmIndexBuffer.cpp \
				   $(FRM_MODEL_PATH)/FrmJoint.cpp \
				   $(FRM_MODEL_PATH)/FrmMaterial.cpp \
				   $(FRM_MODEL_PATH)/FrmMemoryBuffer.cpp \
				   $(FRM_MODEL_PATH)/FrmMeshSurfaceArray.cpp \
				   $(FRM_MODEL_PATH)/FrmModel.cpp \
				   $(FRM_MODEL_PATH)/FrmModelIO.cpp \
				   $(FRM_MODEL_PATH)/FrmModelMesh.cpp \
				   $(FRM_MODEL_PATH)/FrmNamedId.cpp \
				   $(FRM_MODEL_PATH)/FrmVertexBuffer.cpp \
				   $(FRM_MODEL_PATH)/FrmVertexFormat.cpp \
				   $(FRM_MODEL_PATH)/FrmVertexProperty.cpp
				   

LOCAL_C_INCLUDES	:= \
					   $(SRC_PATH) \
					   $(FRM_PATH)	\
					   $(INC_PATH) \
					   $(FRM_OPENGL_PATH) \
					   $(FRM_ANDROID_PATH)\
					   $(FRM_MODEL_PATH)
				   
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

LOCAL_CFLAGS	:= -Wno-write-strings -Wno-conversion-null

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
