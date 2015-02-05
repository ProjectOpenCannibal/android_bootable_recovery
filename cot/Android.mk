# Copyright (C) 2015 Project Open Cannibal
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libcot
LOCAL_MODULE_TAGS := eng

define all-cpp-files-under
$(patsubst ./%,%, \
    $(shell cd $(LOCAL_PATH) ; \
        find $(1) -name "*.cpp" -and -not -name ".*") \
)
endef

LOCAL_SRC_FILES += $(call all-cpp-files-under, .)

LOCAL_SRC_FILES += \
    iniparser/iniparser.c \
    iniparser/dictionary.c

LOCAL_CFLAGS += -DUSE_EXT4 -DMINIVOLD

LOCAL_C_INCLUDES += \
    system/extras/ext4_utils \
    system/core/fs_mgr/include \
    external/fsck_msdos \
    system/vold \
    system/code/healthd \
    system/core/include \
    system/core/libcutils \
    external/libtar \
    external/libtar/listhash \
    external/zlib \
    bionic/libc/bionic

ifeq ($(TARGET_USERIMAGES_USE_F2FS), true)
    LOCAL_CFLAGS += -DUSE_F2FS
    LOCAL_STATIC_LIBRARIES += libmake_f2fs libfsck_f2fs libfibmap_f2fs
endif

ifneq ($(BOARD_RECOVERY_BLDRMSG_OFFSET),)
    LOCAL_CFLAGS += -DBOARD_RECOVERY_BLDRMSG_OFFSET=$(BOARD_RECOVERY_BLDRMSG_OFFSET)
endif

include $(BUILD_STATIC_LIBRARY)