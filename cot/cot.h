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
#include "external.h"

#include <utils/String8.h>
using namespace android;

/*
If you have any defines that you need to access elsewhere, here
is a good place for them!
 */
#define REBOOTMENU_MAIN 0
#define REBOOTMENU_RECOVERY 1
#define REBOOTMENU_BOOTLOADER 2

namespace COT {
    class Settings {
    public:
        static void RecSettingsMenu();
    };

    class Reboot {
    public:
        static int RebootMenu(Device* device);
    };
}