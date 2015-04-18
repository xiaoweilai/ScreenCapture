# Copyright (C) 2009 The Android Open Source Project
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
LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

PATH_TO_FFMPEG_SOURCE := $(LOCAL_PATH)/ffmpeg19
#LOCAL_C_INCLUDES += $(PATH_TO_FFMPEG_SOURCE)
LOCAL_C_INCLUDES += $(PATH_TO_FFMPEG_SOURCE)/include 
#LOCAL_CFLAGS += -I$(PATH_TO_FFMPEG_SOURCE) -I$(PATH_TO_FFMPEG_SOURCE)/include

LOCAL_LDLIBS := -L$(PATH_TO_FFMPEG_SOURCE)/lib -lavcodec-56 -lavdevice-56 -lavfilter-5 -lavformat-56 -lavutil-54 -lswresample-1 -lswscale-3
#LOCAL_SHARED_LIBRARIES := libffmpeg

#LOCAL_SHARED_LIBRARIES = -lffmpeg
#LOCAL_SHARED_LIBRARIES := -L$(PATH_TO_FFMPEG_SOURCE) -lffmpeg

#LOCAL_MODULE    := H264Android
#LOCAL_SRC_FILES := test.c

LOCAL_MODULE    := H264Androidffmpeg22
LOCAL_SRC_FILES := H264Androidffmpeg22.c

include $(BUILD_SHARED_LIBRARY)
