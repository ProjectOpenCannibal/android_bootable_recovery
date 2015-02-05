/* Copyright (C) 2015 Project Open Cannibal
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../bootloader.h"
#include "../common.h"
#include "cutils/properties.h"
#include "cutils/android_reboot.h"
#include "../install.h"
#include "../minui/minui.h"
#include "../minzip/DirUtil.h"
#include "../roots.h"
#include "../ui.h"
#include "../screen_ui.h"
#include "../device.h"
#include "cutils/properties.h"
#include "../voldclient/voldclient.h"
#include "cot.h"

extern RecoveryUI* ui;
using namespace COT;

namespace COT {
    int Reboot::RebootMenu(Device* device) {
        static const char* RebootMenuHeaders[] = { "Reboot", "", NULL };
        static const char* RebootMenuItems[] = { "Reboot to Android",
            "Reboot Recovery",
            "Reboot to Bootloader",
            NULL
        };

        for (;;) {
            int RebootSelection = get_menu_selection(RebootMenuHeaders, RebootMenuItems, 0, 0, device);
            switch (RebootSelection) {
                case REBOOTMENU_MAIN:
                    return REBOOTMENU_MAIN;
                case REBOOTMENU_RECOVERY:
                    return REBOOTMENU_RECOVERY;
                case REBOOTMENU_BOOTLOADER:
                    return REBOOTMENU_BOOTLOADER;
                case Device::kGoBack:
                    return -1;
            }
        }
    }
}