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
LOCAL_PATH			:= $(call my-dir)
SRC_PATH			:= ../../../src
SCENE_PATH			:= ../../../src/scenes
INC_PATH			:= ../../../../../Inc
FRM_PATH			:= ../../../../../Samples/Framework
FRM_OPENGL_PATH		:= $(FRM_PATH)/OpenGLES
FRM_ANDROID_PATH	:= $(FRM_PATH)/Android

include $(CLEAR_VARS)

LOCAL_MODULE    := AdrenoShaders

LOCAL_SRC_FILES := $(SRC_PATH)/scene.cpp \
				   $(SRC_PATH)/application.cpp \
				   $(SRC_PATH)/common.cpp \
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
				   $(SCENE_PATH)/scene01.cpp \
				   $(SCENE_PATH)/scene02.cpp \
				   $(SCENE_PATH)/scene03.cpp \
				   $(SCENE_PATH)/scene04.cpp \
				   $(SCENE_PATH)/scene05.cpp \
				   $(SCENE_PATH)/scene06.cpp \
				   $(SCENE_PATH)/scene07.cpp \
				   $(SCENE_PATH)/scene08.cpp \
				   $(SCENE_PATH)/scene09.cpp \
				   $(SCENE_PATH)/scene10.cpp \
				   $(SCENE_PATH)/scene11.cpp \
				   $(SCENE_PATH)/scene12.cpp \
				   $(SCENE_PATH)/scene13.cpp \
				   $(SCENE_PATH)/scene14.cpp \
				   $(SCENE_PATH)/scene14b.cpp \
				   $(SCENE_PATH)/scene15.cpp \
				   $(SCENE_PATH)/scene16.cpp \
				   $(SCENE_PATH)/scene17.cpp \
				   $(SCENE_PATH)/scene18.cpp \
				   $(SCENE_PATH)/scene19.cpp \
				   $(SCENE_PATH)/scene20.cpp \
				   $(SCENE_PATH)/scene21.cpp \
				   $(SCENE_PATH)/scene22.cpp \
				   $(SCENE_PATH)/scene23.cpp \
				   $(SCENE_PATH)/scene24.cpp \
				   $(SCENE_PATH)/scene25.cpp \
				   $(SCENE_PATH)/scene26.cpp \
				   $(SCENE_PATH)/scene27.cpp \
				   $(SCENE_PATH)/scene28.cpp \
				   $(SCENE_PATH)/scene29.cpp \
				   $(SCENE_PATH)/scene30.cpp \
				   $(SCENE_PATH)/scene31.cpp \
				   $(SCENE_PATH)/scene32.cpp \
				   $(SCENE_PATH)/scene33.cpp \
				   $(SCENE_PATH)/scene34.cpp \
				   $(SCENE_PATH)/scene35.cpp \
				   $(SCENE_PATH)/scene36.cpp \
				   $(SCENE_PATH)/scene37.cpp \
				   $(SCENE_PATH)/scene38.cpp \
				   $(SCENE_PATH)/scene39.cpp \
				   $(SCENE_PATH)/scene40.cpp \
				   $(SCENE_PATH)/scene41.cpp \
				   $(SCENE_PATH)/scene42.cpp \
				   $(SCENE_PATH)/scene43.cpp \
				   $(SCENE_PATH)/scene44.cpp \
				   $(SCENE_PATH)/scene45.cpp \
				   $(SCENE_PATH)/scene46.cpp \
				   $(SCENE_PATH)/scene47.cpp \
				   $(SCENE_PATH)/scene48.cpp \
				   $(SCENE_PATH)/scene49.cpp \
				   $(SCENE_PATH)/scene50.cpp \
				   
LOCAL_C_INCLUDES	:= $(SRC_PATH) \
					   $(SCENE_PATH) \
					   $(INC_PATH) \
					   $(FRM_PATH)	\
					   $(FRM_OPENGL_PATH) \
					   $(FRM_ANDROID_PATH)
				   
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 

LOCAL_CFLAGS	+= -Wno-write-strings -Wno-conversion-null -DOS_ANDROID

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
