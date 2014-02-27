/*
 * Copyright (C) 2012 Drew Walton & Nathan Bass
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
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
#include <getopt.h>
#include <limits.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/limits.h>
#include <dirent.h>
#include <sys/stat.h>

#include <signal.h>
#include <sys/wait.h>

#include "bootloader.h"
#include "common.h"
#include "cutils/properties.h"
#include "firmware.h"
#include "install.h"
#include "make_ext4fs.h"
#include "minui/minui.h"
#include "minzip/DirUtil.h"
#include "roots.h"
#include "recovery_ui.h"

#include "extendedcommands.h"
#include "nandroid.h"
#include "mounts.h"
#include "flashutils/flashutils.h"
#include "edify/expr.h"
#include <libgen.h>
#include "mtdutils/mtdutils.h"
#include "bmlutils/bmlutils.h"
#include "cutils/android_reboot.h"
#include "settingshandler_lang.h"

// common words and phrases
char* no;
char* yes;
char* batterylevel;
char* backmenudisabled;
char* backmenuenabled;
char* choosepackage;
char* install;
char* installing;
char* mounted;
char* unmounted;
char* installabort;
char* rebooting;
char* shutdown;
char* enabled;
char* disabled;
char* mounterror;
char* unmounterror;
char* skipformat;
char* installconfirm;
char* yesinstall;
char* deleteconfirm;
char* freespacesd;
char* yesdelete;
char* deleting;
char* confirmformat;
char* yesformat;
char* formatting;
char* formaterror;
char* done;
char* doneexc;
char* size;
char* boot;
char* recovery;
char* notfound;
char* foundold;
char* on;
char* off;
// menu headers
char* wipedataheader1;
char* wipedataheader2;
char* zipinstallheader;
char* deletebackupheader;
char* lowspaceheader1;
char* lowspaceheader2;
char* lowspaceheader3;
char* lowspaceheader4;
char* zipchooseheader;
char* nandroidrestoreheader;
char* usbmsheader1;
char* usbmsheader2;
char* usbmsheader3;
char* recommended;
char* showpartitionheader;
char* advbackupheader;
char* advrestoreheader1;
char* advrestoreheader2;
char* advrestoreheader3;
char* advrestoreheader11;
char* nandroidheader;
char* debuggingheader;
char* advoptionsheader;
// menu items for main screen
char* rebootnow;
char* wipedatafactory;
char* wipecache;
char* wipeall;
char* installzip;
char* nandroid;
char* mountsstorage;
char* advanced;
char* langpoweroff;
// menu items for extendedcommands
char* signaturecheck;
char* scriptasserts;
char* zipchoosezip;
char* zipapplyupdatezip;
char* ziptogglesig;
char* ziptoggleasserts;
char* backupproceed;
char* backupcancel;
char* zipchooseyesbackup;
char* zipchoosenobackup;
char* zipcancelinstall;
char* usbmsunmount;
char* usbmsmount;
char* nandroidbackup;
char* nandroidrestore;
char* nandroidadvbackup;
char* nandroidadvrestore;
char* nandroidrestoreboot;
char* nandroidrestoresys;
char* nandroidrestoredata;
char* nandroidrestorecache;
char* nandroidrestoresd;
char* nandroiddeleteold;
char* debugfixperm;
char* debugfixloop;
char* debugreporterror;
char* debugkeytest;
char* debugshowlog;
char* debugtoggledebugui;
char* advreboot;
char* advwipedalvik;
char* advpartitionsd;
char* advcotsettings;
char* advdebugopts;
char* lowspacecontinuebackup;
char* lowspaceviewdelete;
char* lowspacecancel;
// partition wipe prompts
char* yesdeletedata;
char* wipingdata;
char* datawipecomplete;
char* datawipeskip;
char* datawipefail;
char* confirmwipe;
char* yeswipecache;
char* wipingcache;
char* cachewipecomplete;
char* cachewipeskip;
char* cachewipefail;
char* confirmwipeall;
char* yeswipeall;
char* wipingall;
char* wipeallcomplete;
char* yeswipedalvik;
char* wipingdalvik;
char* wipedalvikcomplete;
char* wipedalvikskip;
char* partitioningsd;
char* partitionsderror;
// miscellaneous prompts
char* rebootingsystemtimed;
char* diropenfail;
char* nofilesfound;
char* ext4checking;
// ors-specific prompts
char* orssdmountwait;
char* orssdmounted;
char* orssdcontinuing;
char* orscommandis;
char* orsnovalue;
char* orsinstallingzip;
char* orszipinstallerror;
char* orsrecursivemkdir;
char* orsrebootfound;
char* orscmdnovalue;
char* orsunknowncmd;
char* orsscriptdone;
char* orsscripterror;
// nandroid prompts
char* nandroidbackupfolderset;
char* nandroidbackupcomplete;
char* nandroidsettingrestoreoptions;
char* nandroidrestorecomplete;
char* nandroidbackupdeletecomplete;
char* nandroidconfirmrestore;
char* nandroidyesrestore;
char* backup;
char* restore;
char* performbackup;
char* nandroidbackingup;
char* nandroidcantmount;
char* nandroidyaffs2error;
char* nandroidsdfreespace;
char* bootdumperror;
char* recoverydumperror;
char* nandroidandsecnotfound;
char* nandroidsdextmountfail;
char* nandroidmd5generate;
char* nandroidmd5check;
char* nandroidmd5fail;
char* nandroidtarnotfound;
char* nandroidimgnotfound;
char* restoring;
char* restoreerror;
char* nandroideraseboot;
char* nandroidrestoringboot;
char* nandroidbootflasherror;
char* nandroidnobootimg;
char* nandroidbackupbootyes;
char* nandroidbackupbootno;
char* nandroidbackuprecyes;
char* nandroidbackuprecno;
char* nandroidbackupsysyes;
char* nandroidbackupsysno;
char* nandroidbackupdatayes;
char* nandroidbackupdatano;
char* nandroidbackupcacheyes;
char* nandroidbackupcacheno;
char* nandroidbackupsdyes;
char* nandroidbackupsdno;
// secure fs prompts
char* securefsenable;
char* securefsdisable;
char* securefsinvalid;
char* securefsabortdefault;
char* securefsabort;
char* securefsupdate;
// debugging prompts
char* uidebugenable;
char* uidebugdisable;
char* fixingperm;
char* outputkeycodes;
char* kcgoback;
// zip flashing prompts
char* installcomplete;
char* yesinstallupdate;
// format prompts
char* a2sdnotfound;
char* device;
char* ext3;
char* ext2;
// bml prompts
char* bmlchecking;
char* bmlmayberfs;
// fs convert prompt
char* fsconv1;
char* fsconv2;
char* fsconv3;
char* fsconv4;
char* fsconv5;
char* fsconv6;
// failure prompt
char* failprompt;
// update.zip messages
char* installingupdate;
char* findupdatepackage;
char* openupdatepackage;
char* verifyupdatepackage;
// edify prompts
char* edifyformatting;
char* edifyformatdatadata;
char* edifywaitsdmount;
char* edifysdmounted;
char* edifysdtimeout;
char* edifysdverifymarker;
char* edifysdmarkernotfound;
char* edifycheckinternalmarker;
char* edifyinternalmarkernotfound;
char* edifyrmscripterror;
// commands
char* formatcmd;
char* deletecmd;
char* copycmd;
char* firmwriteextracting;
char* firmwriteimage;
char* writeimage;
// cot settings
char* settingsloaderror1;
char* settingsloaderror2;
char* settingsloaded;
char* settingsthemeerror;
char* settingsthemeloaded;
// cot settings headers
char* cotmainheader;
char* cotthemeheader;
char* cotorsrebootheader;
char* cotorswipepromptheader;
char* cotzippromptheader;
char* cotlangheader;
// cot settings list items
char* cotmainlisttheme;
char* cotmainlistrebootforce;
char* cotmainlistwipeprompt;
char* cotmainlistzipprompt;
char* cotmainlistlanguage;
char* cotthemehydro;
char* cotthemeblood;
char* cotthemelime;
char* cotthemecitrus;
char* cotthemedooderbutt;
char* cotlangen;
// cot settings theme engine
char* setthemedefault;
char* setthemeblood;
char* setthemelime;
char* setthemecitrus;
char* setthemedooderbutt;

typedef struct {
    char* no;
    char* yes;
    char* batterylevel;
    char* backmenudisabled;
    char* backmenuenabled;
    char* choosepackage;
    char* install;
    char* installing;
    char* mounted;
    char* unmounted;
    char* installabort;
    char* rebooting;
    char* shutdown;
    char* enabled;
    char* disabled;
    char* mounterror;
    char* unmounterror;
    char* skipformat;
    char* installconfirm;
    char* yesinstall;
    char* deleteconfirm;
    char* freespacesd;
    char* yesdelete;
    char* deleting;
    char* confirmformat;
    char* yesformat;
    char* formatting;
    char* formaterror;
    char* done;
    char* doneexc;
    char* size;
    char* boot;
    char* recovery;
    char* notfound;
    char* foundold;
    char* on;
    char* off;
    // menu headers
    char* wipedataheader1;
    char* wipedataheader2;
    char* zipinstallheader;
    char* deletebackupheader;
    char* lowspaceheader1;
    char* lowspaceheader2;
    char* lowspaceheader3;
    char* lowspaceheader4;
    char* zipchooseheader;
    char* nandroidrestoreheader;
    char* usbmsheader1;
    char* usbmsheader2;
    char* usbmsheader3;
    char* recommended;
    const char* showpartitionheader;
    const char* advbackupheader;
    const char* advrestoreheader1;
    const char* advrestoreheader2;
    const char* advrestoreheader3;
    const char* advrestoreheader11;
    const char* nandroidheader;
    const char* debuggingheader;
    const char* advoptionsheader;
    // menu items for main screen
    char* rebootnow;
    char* wipedatafactory;
    char* wipecache;
    char* wipeall;
    char* installzip;
    char* nandroid;
    char* mountsstorage;
    char* advanced;
    char* langpoweroff;
    // menu items for extendedcommands
    char* signaturecheck;
    char* scriptasserts;
    char* zipchoosezip;
    char* zipapplyupdatezip;
    char* ziptogglesig;
    char* ziptoggleasserts;
    char* backupproceed;
    char* backupcancel;
    char* zipchooseyesbackup;
    char* zipchoosenobackup;
    char* zipcancelinstall;
    char* usbmsunmount;
    char* usbmsmount;
    char* nandroidbackup;
    char* nandroidrestore;
    char* nandroidadvbackup;
    char* nandroidadvrestore;
    char* nandroidrestoreboot;
    char* nandroidrestoresys;
    char* nandroidrestoredata;
    char* nandroidrestorecache;
    char* nandroidrestoresd;
    char* nandroiddeleteold;
    char* debugfixperm;
    char* debugfixloop;
    char* debugreporterror;
    char* debugkeytest;
    char* debugshowlog;
    char* debugtoggledebugui;
    char* advreboot;
    char* advwipedalvik;
    char* advpartitionsd;
    char* advcotsettings;
    char* advdebugopts;
	char* lowspacecontinuebackup;
	char* lowspaceviewdelete;
	char* lowspacecancel;
    // partition wipe prompts
    char* yesdeletedata;
    char* wipingdata;
    char* datawipecomplete;
    char* datawipeskip;
    char* datawipefail;
    char* confirmwipe;
    char* yeswipecache;
    char* wipingcache;
    char* cachewipecomplete;
    char* cachewipeskip;
    char* cachewipefail;
    char* confirmwipeall;
    char* yeswipeall;
    char* wipingall;
    char* wipeallcomplete;
    char* yeswipedalvik;
    char* wipingdalvik;
    char* wipedalvikcomplete;
    char* wipedalvikskip;
    char* partitioningsd;
    char* partitionsderror;
    // miscellaneous prompts
    char* rebootingsystemtimed;
    char* diropenfail;
    char* nofilesfound;
    char* ext4checking;
    // ors-specific prompts
    char* orssdmountwait;
    char* orssdmounted;
    char* orssdcontinuing;
    char* orscommandis;
    char* orsnovalue;
    char* orsinstallingzip;
    char* orszipinstallerror;
    char* orsrecursivemkdir;
    char* orsrebootfound;
    char* orscmdnovalue;
    char* orsunknowncmd;
    char* orsscriptdone;
    char* orsscripterror;
    // nandroid prompts
    char* nandroidbackupfolderset;
    char* nandroidbackupcomplete;
    char* nandroidsettingrestoreoptions;
    char* nandroidrestorecomplete;
    char* nandroidbackupdeletecomplete;
    char* nandroidconfirmrestore;
    char* nandroidyesrestore;
    char* backup;
    char* restore;
    char* performbackup;
    char* nandroidbackingup;
    char* nandroidcantmount;
    char* nandroidyaffs2error;
    char* nandroidsdfreespace;
    char* bootdumperror;
    char* recoverydumperror;
    char* nandroidandsecnotfound;
    char* nandroidsdextmountfail;
    char* nandroidmd5generate;
    char* nandroidmd5check;
    char* nandroidmd5fail;
    char* nandroidtarnotfound;
    char* nandroidimgnotfound;
    char* restoring;
    char* restoreerror;
    char* nandroideraseboot;
    char* nandroidrestoringboot;
    char* nandroidbootflasherror;
    char* nandroidnobootimg;
	char* nandroidbackupbootyes;
	char* nandroidbackupbootno;
	char* nandroidbackuprecyes;
	char* nandroidbackuprecno;
	char* nandroidbackupsysyes;
	char* nandroidbackupsysno;
	char* nandroidbackupdatayes;
	char* nandroidbackupdatano;
	char* nandroidbackupcacheyes;
	char* nandroidbackupcacheno;
	char* nandroidbackupsdyes;
	char* nandroidbackupsdno;
    // secure fs prompts
    char* securefsenable;
    char* securefsdisable;
    char* securefsinvalid;
    char* securefsabortdefault;
    char* securefsabort;
    char* securefsupdate;
    // debugging prompts
    char* uidebugenable;
    char* uidebugdisable;
    char* fixingperm;
    char* outputkeycodes;
    char* kcgoback;
    // zip flashing prompts
    char* installcomplete;
    char* yesinstallupdate;
    // format prompts
    char* a2sdnotfound;
    char* device;
    char* ext3;
    char* ext2;
    // bml prompts
    char* bmlchecking;
    char* bmlmayberfs;
    // fs convert prompt
    char* fsconv1;
    char* fsconv2;
    char* fsconv3;
    char* fsconv4;
    char* fsconv5;
    char* fsconv6;
    // failure prompt
    char* failprompt;
    // update.zip messages
    char* installingupdate;
    char* findupdatepackage;
    char* openupdatepackage;
    char* verifyupdatepackage;
    // edify prompts
    char* edifyformatting;
    char* edifyformatdatadata;
    char* edifywaitsdmount;
    char* edifysdmounted;
    char* edifysdtimeout;
    char* edifysdverifymarker;
    char* edifysdmarkernotfound;
    char* edifycheckinternalmarker;
    char* edifyinternalmarkernotfound;
    char* edifyrmscripterror;
    // commands
    char* formatcmd;
    char* deletecmd;
    char* copycmd;
    char* firmwriteextracting;
    char* firmwriteimage;
    char* writeimage;
    // cot settings
    char* settingsloaderror1;
    char* settingsloaderror2;
    char* settingsloaded;
    char* settingsthemeerror;
    char* settingsthemeloaded;
    // cot settings headers
    char* cotmainheader;
    char* cotthemeheader;
    char* cotorsrebootheader;
    char* cotorswipepromptheader;
    char* cotzippromptheader;
    char* cotlangheader;
    // cot settings list items
    char* cotmainlisttheme;
    char* cotmainlistrebootforce;
    char* cotmainlistwipeprompt;
    char* cotmainlistzipprompt;
    char* cotmainlistlanguage;
    char* cotthemehydro;
    char* cotthemeblood;
    char* cotthemelime;
    char* cotthemecitrus;
    char* cotthemedooderbutt;
    char* cotlangen;
    // cot settings theme engine
    char* setthemedefault;
    char* setthemeblood;
    char* setthemelime;
    char* setthemecitrus;
    char* setthemedooderbutt;
} lang_keywords;

int language_handler(void* user, const char* section, const char* name, const char* value) {
    lang_keywords* pconfig = (lang_keywords*)user;

	#define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
	if (MATCH("language", "no")) {
		pconfig->no = strdup(value);
	} else if (MATCH("language", "yes")) {
		pconfig->yes = strdup(value);
	} else if (MATCH("language", "batterylevel")) {
		pconfig->batterylevel = strdup(value);
	} else if (MATCH("language", "backmenudisabled")) {
		pconfig->backmenudisabled = strdup(value);
	} else if (MATCH("language", "backmenuenabled")) {
		pconfig->backmenuenabled = strdup(value);
	} else if (MATCH("language", "choosepackage")) {
		pconfig->choosepackage = strdup(value);
	} else if (MATCH("language", "install")) {
		pconfig->install = strdup(value);
	} else if (MATCH("language", "installing")) {
		pconfig->installing = strdup(value);
	} else if (MATCH("language", "mounted")) {
		pconfig->mounted = strdup(value);
	} else if (MATCH("language", "unmounted")) {
		pconfig->unmounted = strdup(value);
	} else if (MATCH("language", "installabort")) {
		pconfig->installabort = strdup(value);
	} else if (MATCH("language", "rebooting")) {
		pconfig->rebooting = strdup(value);
	} else if (MATCH("language", "shutdown")) {
		pconfig->shutdown = strdup(value);
	} else if (MATCH("language", "enabled")) {
		pconfig->enabled = strdup(value);
	} else if (MATCH("language", "disabled")) {
		pconfig->disabled = strdup(value);
	} else if (MATCH("language", "mounterror")) {
		pconfig->mounterror = strdup(value);
	} else if (MATCH("language", "skipformat")) {
		pconfig->skipformat = strdup(value);
	} else if (MATCH("language", "installconfirm")) {
		pconfig->installconfirm = strdup(value);
	} else if (MATCH("language", "yesinstall")) {
		pconfig->yesinstall = strdup(value);
	} else if (MATCH("language", "deleteconfirm")) {
		pconfig->deleteconfirm = strdup(value);
	} else if (MATCH("language", "freespacesd")) {
		pconfig->freespacesd = strdup(value);
	} else if (MATCH("language", "yesdelete")) {
		pconfig->yesdelete = strdup(value);
	} else if (MATCH("language", "deleting")) {
		pconfig->deleting = strdup(value);
	} else if (MATCH("language", "confirmformat")) {
		pconfig->confirmformat = strdup(value);
	} else if (MATCH("language", "yesformat")) {
		pconfig->yesformat = strdup(value);
	} else if (MATCH("language", "formatting")) {
		pconfig->formatting = strdup(value);
	} else if (MATCH("language", "formaterror")) {
		pconfig->formaterror = strdup(value);
	} else if (MATCH("language", "done")) {
		pconfig->done = strdup(value);
	} else if (MATCH("language", "doneexc")) {
		pconfig->doneexc = strdup(value);
	} else if (MATCH("language", "size")) {
		pconfig->size = strdup(value);
	} else if (MATCH("language", "boot")) {
		pconfig->boot = strdup(value);
	} else if (MATCH("language", "recovery")) {
		pconfig->recovery = strdup(value);
	} else if (MATCH("language", "notfound")) {
		pconfig->notfound = strdup(value);
	} else if (MATCH("language", "foundold")) {
		pconfig->foundold = strdup(value);
	} else if (MATCH("language", "on")) {
		pconfig->on = strdup(value);
	} else if (MATCH("language", "off")) {
		pconfig->off = strdup(value);
	} else if (MATCH("language", "wipedataheader1")) {
		pconfig->wipedataheader1 = strdup(value);
	} else if (MATCH("language", "wipedataheader2")) {
		pconfig->wipedataheader2 = strdup(value);
	} else if (MATCH("language", "zipinstallheader")) {
		pconfig->zipinstallheader = strdup(value);
	} else if (MATCH("language", "deletebackupheader")) {
		pconfig->deletebackupheader = strdup(value);
	} else if (MATCH("language", "lowspaceheader1")) {
		pconfig->lowspaceheader1 = strdup(value);
	} else if (MATCH("language", "lowspaceheader2")) {
		pconfig->lowspaceheader2 = strdup(value);
	} else if (MATCH("language", "lowspaceheader3")) {
		pconfig->lowspaceheader3 = strdup(value);
	} else if (MATCH("language", "lowspaceheader4")) {
		pconfig->lowspaceheader4 = strdup(value);
	} else if (MATCH("language", "zipchooseheader")) {
		pconfig->zipchooseheader = strdup(value);
	} else if (MATCH("language", "nandroidrestoreheader")) {
		pconfig->nandroidrestoreheader = strdup(value);
	} else if (MATCH("language", "usbmsheader1")) {
		pconfig->usbmsheader1 = strdup(value);
	} else if (MATCH("language", "usbmsheader2")) {
		pconfig->usbmsheader2 = strdup(value);
	} else if (MATCH("language", "usbmsheader3")) {
		pconfig->usbmsheader3 = strdup(value);
	} else if (MATCH("language", "recommended")) {
		pconfig->recommended = strdup(value);
	} else if (MATCH("language", "showpartitionheader")) {
		pconfig->showpartitionheader = strdup(value);
	} else if (MATCH("language", "advbackupheader")) {
		pconfig->advbackupheader = strdup(value);
	} else if (MATCH("language", "advrestoreheader1")) {
		pconfig->advrestoreheader1 = strdup(value);
	} else if (MATCH("language", "advrestoreheader2")) {
		pconfig->advrestoreheader2 = strdup(value);
	} else if (MATCH("language", "advrestoreheader3")) {
		pconfig->advrestoreheader3 = strdup(value);
	} else if (MATCH("language", "advrestoreheader11")) {
		pconfig->advrestoreheader11 = strdup(value);
	} else if (MATCH("language", "nandroidheader")) {
		pconfig->nandroidheader = strdup(value);
	} else if (MATCH("language", "debuggingheader")) {
		pconfig->debuggingheader = strdup(value);
	} else if (MATCH("language", "advoptionsheader")) {
		pconfig->advoptionsheader = strdup(value);
	} else if (MATCH("language", "rebootnow")) {
		pconfig->rebootnow = strdup(value);
	} else if (MATCH("language", "wipedatafactory")) {
		pconfig->wipedatafactory = strdup(value);
	} else if (MATCH("language", "wipecache")) {
		pconfig->wipecache = strdup(value);
	} else if (MATCH("language", "wipeall")) {
		pconfig->wipeall = strdup(value);
	} else if (MATCH("language", "installzip")) {
		pconfig->installzip = strdup(value);
	} else if (MATCH("language", "nandroid")) {
		pconfig->nandroid = strdup(value);
	} else if (MATCH("language", "mountsstorage")) {
		pconfig->mountsstorage = strdup(value);
	} else if (MATCH("language", "advanced")) {
		pconfig->advanced = strdup(value);
	} else if (MATCH("language", "langpoweroff")) {
		pconfig->langpoweroff = strdup(value);
	} else if (MATCH("language", "signaturecheck")) {
		pconfig->signaturecheck = strdup(value);
	} else if (MATCH("language", "scriptasserts")) {
		pconfig->scriptasserts = strdup(value);
	} else if (MATCH("language", "zipchoosezip")) {
		pconfig->zipchoosezip = strdup(value);
	} else if (MATCH("language", "zipapplyupdatezip")) {
		pconfig->zipapplyupdatezip = strdup(value);
	} else if (MATCH("language", "ziptogglesig")) {
		pconfig->ziptogglesig = strdup(value);
	} else if (MATCH("language", "ziptoggleasserts")) {
		pconfig->ziptoggleasserts = strdup(value);
	} else if (MATCH("language", "backupproceed")) {
		pconfig->backupproceed = strdup(value);
	} else if (MATCH("language", "backupcancel")) {
		pconfig->backupcancel = strdup(value);
	} else if (MATCH("language", "zipchooseyesbackup")) {
		pconfig->zipchooseyesbackup = strdup(value);
	} else if (MATCH("language", "zipchoosenobackup")) {
		pconfig->zipchoosenobackup = strdup(value);
	} else if (MATCH("language", "zipcancelinstall")) {
		pconfig->zipcancelinstall = strdup(value);
	} else if (MATCH("language", "usbmsunmount")) {
		pconfig->usbmsunmount = strdup(value);
	} else if (MATCH("language", "usbmsmount")) {
		pconfig->usbmsmount = strdup(value);
	} else if (MATCH("language", "nandroidbackup")) {
		pconfig->nandroidbackup = strdup(value);
	} else if (MATCH("language", "nandroidrestore")) {
		pconfig->nandroidrestore = strdup(value);
	} else if (MATCH("language", "nandroidrestoreboot")) {
		pconfig->nandroidrestoreboot = strdup(value);
	} else if (MATCH("language", "nandroidrestoresys")) {
		pconfig->nandroidrestoresys = strdup(value);
	} else if (MATCH("language", "nandroidrestoredata")) {
		pconfig->nandroidrestoredata = strdup(value);
	} else if (MATCH("language", "nandroidrestorecache")) {
		pconfig->nandroidrestorecache = strdup(value);
	} else if (MATCH("language", "nandroidrestoresd")) {
		pconfig->nandroidrestoresd = strdup(value);
	} else if (MATCH("language", "nandroidadvbackup")) {
		pconfig->nandroidadvbackup = strdup(value);
	} else if (MATCH("language", "nandroidadvrestore")) {
		pconfig->nandroidadvrestore = strdup(value);
	} else if (MATCH("language", "nandroiddeleteold")) {
		pconfig->nandroiddeleteold = strdup(value);
	} else if (MATCH("language", "debugfixperm")) {
		pconfig->debugfixperm = strdup(value);
	} else if (MATCH("language", "debugfixloop")) {
		pconfig->debugfixloop = strdup(value);
	} else if (MATCH("language", "debugreporterror")) {
		pconfig->debugreporterror = strdup(value);
	} else if (MATCH("language", "debugkeytest")) {
		pconfig->debugkeytest = strdup(value);
	} else if (MATCH("language", "debugshowlog")) {
		pconfig->debugshowlog = strdup(value);
	} else if (MATCH("language", "debugtoggledebugui")) {
		pconfig->debugtoggledebugui = strdup(value);
	} else if (MATCH("language", "advreboot")) {
		pconfig->advreboot = strdup(value);
	} else if (MATCH("language", "advwipedalvik")) {
		pconfig->advwipedalvik = strdup(value);
	} else if (MATCH("language", "advpartitionsd")) {
		pconfig->advpartitionsd = strdup(value);
	} else if (MATCH("language", "advcotsettings")) {
		pconfig->advcotsettings = strdup(value);
	} else if (MATCH("language", "advdebugopts")) {
		pconfig->advdebugopts = strdup(value);
	} else if (MATCH("language", "lowspacecontinuebackup")) {
		pconfig->lowspacecontinuebackup = strdup(value);
	} else if (MATCH("language", "lowspaceviewdelete")) {
		pconfig->lowspaceviewdelete = strdup(value);
	} else if (MATCH("language", "lowspacecancel")) {
		pconfig->lowspacecancel = strdup(value);
	} else if (MATCH("language", "yesdeletedata")) {
		pconfig->yesdeletedata = strdup(value);
	} else if (MATCH("language", "wipingdata")) {
		pconfig->wipingdata = strdup(value);
	} else if (MATCH("language", "datawipecomplete")) {
		pconfig->datawipecomplete = strdup(value);
	} else if (MATCH("language", "datawipeskip")) {
		pconfig->datawipeskip = strdup(value);
	} else if (MATCH("language", "datawipefail")) {
		pconfig->datawipefail = strdup(value);
	} else if (MATCH("language", "confirmwipe")) {
		pconfig->confirmwipe = strdup(value);
	} else if (MATCH("language", "yeswipecache")) {
		pconfig->yeswipecache = strdup(value);
	} else if (MATCH("language", "wipingcache")) {
		pconfig->wipingcache = strdup(value);
	} else if (MATCH("language", "cachewipecomplete")) {
		pconfig->cachewipecomplete = strdup(value);
	} else if (MATCH("language", "cachewipeskip")) {
		pconfig->cachewipeskip = strdup(value);
	} else if (MATCH("language", "cachewipefail")) {
		pconfig->cachewipefail = strdup(value);
	} else if (MATCH("language", "confirmwipeall")) {
		pconfig->confirmwipeall = strdup(value);
	} else if (MATCH("language", "yeswipeall")) {
		pconfig->yeswipeall = strdup(value);
	} else if (MATCH("language", "wipingall")) {
		pconfig->wipingall = strdup(value);
	} else if (MATCH("language", "wipeallcomplete")) {
		pconfig->wipeallcomplete = strdup(value);
	} else if (MATCH("language", "yeswipedalvik")) {
		pconfig->yeswipedalvik = strdup(value);
	} else if (MATCH("language", "wipingdalvik")) {
		pconfig->wipingdalvik = strdup(value);
	} else if (MATCH("language", "wipedalvikcomplete")) {
		pconfig->wipedalvikcomplete = strdup(value);
	} else if (MATCH("language", "wipedalvikskip")) {
		pconfig->wipedalvikskip = strdup(value);
	} else if (MATCH("language", "partitioningsd")) {
		pconfig->partitioningsd = strdup(value);
	} else if (MATCH("language", "partitionsderror")) {
		pconfig->partitionsderror = strdup(value);
	} else if (MATCH("language", "rebootingsystemtimed")) {
		pconfig->rebootingsystemtimed = strdup(value);
	} else if (MATCH("language", "diropenfail")) {
		pconfig->diropenfail = strdup(value);
	} else if (MATCH("language", "nofilesfound")) {
		pconfig->nofilesfound = strdup(value);
	} else if (MATCH("language", "ext4checking")) {
		pconfig->ext4checking = strdup(value);
	} else if (MATCH("language", "orssdmountwait")) {
		pconfig->orssdmountwait = strdup(value);
	} else if (MATCH("language", "orssdmounted")) {
		pconfig->orssdmounted = strdup(value);
	} else if (MATCH("language", "orssdcontinuing")) {
		pconfig->orssdcontinuing = strdup(value);
	} else if (MATCH("language", "orscommandis")) {
		pconfig->orscommandis = strdup(value);
	} else if (MATCH("language", "orsnovalue")) {
		pconfig->orsnovalue = strdup(value);
	} else if (MATCH("language", "orsinstallingzip")) {
		pconfig->orsinstallingzip = strdup(value);
	} else if (MATCH("language", "orszipinstallerror")) {
		pconfig->orszipinstallerror = strdup(value);
	} else if (MATCH("language", "orsrecursivemkdir")) {
		pconfig->orsrecursivemkdir = strdup(value);
	} else if (MATCH("language", "orsrebootfound")) {
		pconfig->orsrebootfound = strdup(value);
	} else if (MATCH("language", "orscmdnovalue")) {
		pconfig->orscmdnovalue = strdup(value);
	} else if (MATCH("language", "orsunknowncmd")) {
		pconfig->orsunknowncmd = strdup(value);
	} else if (MATCH("language", "orsscriptdone")) {
		pconfig->orsscriptdone = strdup(value);
	} else if (MATCH("language", "orsscripterror")) {
		pconfig->orsscripterror = strdup(value);
	} else if (MATCH("language", "nandroidbackupfolderset")) {
		pconfig->nandroidbackupfolderset = strdup(value);
	} else if (MATCH("language", "nandroidbackupcomplete")) {
		pconfig->nandroidbackupcomplete = strdup(value);
	} else if (MATCH("language", "nandroidsettingrestoreoptions")) {
		pconfig->nandroidsettingrestoreoptions = strdup(value);
	} else if (MATCH("language", "nandroidrestorecomplete")) {
		pconfig->nandroidrestorecomplete = strdup(value);
	} else if (MATCH("language", "nandroidbackupdeletecomplete")) {
		pconfig->nandroidbackupdeletecomplete = strdup(value);
	} else if (MATCH("language", "nandroidconfirmrestore")) {
		pconfig->nandroidconfirmrestore = strdup(value);
	} else if (MATCH("language", "nandroidyesrestore")) {
		pconfig->nandroidyesrestore = strdup(value);
	} else if (MATCH("language", "backup")) {
		pconfig->backup = strdup(value);
	} else if (MATCH("language", "restore")) {
		pconfig->restore = strdup(value);
	} else if (MATCH("language", "performbackup")) {
		pconfig->performbackup = strdup(value);
	} else if (MATCH("language", "nandroidbackingup")) {
		pconfig->nandroidbackingup = strdup(value);
	} else if (MATCH("language", "nandroidcantmount")) {
		pconfig->nandroidcantmount = strdup(value);
	} else if (MATCH("language", "nandroidyaffs2error")) {
		pconfig->nandroidyaffs2error = strdup(value);
	} else if (MATCH("language", "nandroidsdfreespace")) {
		pconfig->nandroidsdfreespace = strdup(value);
	} else if (MATCH("language", "bootdumperror")) {
		pconfig->bootdumperror = strdup(value);
	} else if (MATCH("language", "recoverydumperror")) {
		pconfig->recoverydumperror = strdup(value);
	} else if (MATCH("language", "nandroidandsecnotfound")) {
		pconfig->nandroidandsecnotfound = strdup(value);
	} else if (MATCH("language", "nandroidsdextmountfail")) {
		pconfig->nandroidsdextmountfail = strdup(value);
	} else if (MATCH("language", "nandroidmd5generate")) {
		pconfig->nandroidmd5generate = strdup(value);
	} else if (MATCH("language", "nandroidmd5check")) {
		pconfig->nandroidmd5check = strdup(value);
	} else if (MATCH("language", "nandroidmd5fail")) {
		pconfig->nandroidmd5fail = strdup(value);
	} else if (MATCH("language", "nandroidtarnotfound")) {
		pconfig->nandroidtarnotfound = strdup(value);
	} else if (MATCH("language", "nandroidimgnotfound")) {
		pconfig->nandroidimgnotfound = strdup(value);
	} else if (MATCH("language", "restoring")) {
		pconfig->restoring = strdup(value);
	} else if (MATCH("language", "restoreerror")) {
		pconfig->restoreerror = strdup(value);
	} else if (MATCH("language", "nandroideraseboot")) {
		pconfig->nandroideraseboot = strdup(value);
	} else if (MATCH("language", "nandroidrestoringboot")) {
		pconfig->nandroidrestoringboot = strdup(value);
	} else if (MATCH("language", "nandroidbootflasherror")) {
		pconfig->nandroidbootflasherror = strdup(value);
	} else if (MATCH("language", "nandroidnobootimg")) {
		pconfig->nandroidnobootimg = strdup(value);
	} else if (MATCH("language", "nandroidbackupbootyes")) {
		pconfig->nandroidbackupbootyes = strdup(value);
	} else if (MATCH("language", "nandroidbackupbootno")) {
		pconfig->nandroidbackupbootno = strdup(value);
	} else if (MATCH("language", "nandroidbackuprecyes")) {
		pconfig->nandroidbackuprecyes = strdup(value);
	} else if (MATCH("language", "nandroidbackuprecno")) {
		pconfig->nandroidbackuprecno = strdup(value);
	} else if (MATCH("language", "nandroidbackupsysyes")) {
		pconfig->nandroidbackupsysyes = strdup(value);
	} else if (MATCH("language", "nandroidbackupsysno")) {
		pconfig->nandroidbackupsysno = strdup(value);
	} else if (MATCH("language", "nandroidbackupdatayes")) {
		pconfig->nandroidbackupdatayes = strdup(value);
	} else if (MATCH("language", "nandroidbackupdatano")) {
		pconfig->nandroidbackupdatano = strdup(value);
	} else if (MATCH("language", "nandroidbackupcacheyes")) {
		pconfig->nandroidbackupcacheyes = strdup(value);
	} else if (MATCH("language", "nandroidbackupcacheno")) {
		pconfig->nandroidbackupcacheno = strdup(value);
	} else if (MATCH("language", "nandroidbackupsdyes")) {
		pconfig->nandroidbackupsdyes = strdup(value);
	} else if (MATCH("language", "nandroidbackupsdno")) {
		pconfig->nandroidbackupsdno = strdup(value);
	} else if (MATCH("language", "securefsenable")) {
		pconfig->securefsenable = strdup(value);
	} else if (MATCH("language", "securefsdisable")) {
		pconfig->securefsdisable = strdup(value);
	} else if (MATCH("language", "securefsinvalid")) {
		pconfig->securefsinvalid = strdup(value);
	} else if (MATCH("language", "securefsabortdefault")) {
		pconfig->securefsabortdefault = strdup(value);
	} else if (MATCH("language", "securefsabort")) {
		pconfig->securefsabort = strdup(value);
	} else if (MATCH("language", "securefsupdate")) {
		pconfig->securefsupdate = strdup(value);
	} else if (MATCH("language", "uidebugenable")) {
		pconfig->uidebugenable = strdup(value);
	} else if (MATCH("language", "uidebugdisable")) {
		pconfig->uidebugdisable = strdup(value);
	} else if (MATCH("language", "fixingperm")) {
		pconfig->fixingperm = strdup(value);
	} else if (MATCH("language", "outputkeycodes")) {
		pconfig->outputkeycodes = strdup(value);
	} else if (MATCH("language", "kcgoback")) {
		pconfig->kcgoback = strdup(value);
	} else if (MATCH("language", "installcomplete")) {
		pconfig->installcomplete = strdup(value);
	} else if (MATCH("language", "yesinstallupdate")) {
		pconfig->yesinstallupdate = strdup(value);
	} else if (MATCH("language", "a2sdnotfound")) {
		pconfig->a2sdnotfound = strdup(value);
	} else if (MATCH("language", "device")) {
		pconfig->device = strdup(value);
	} else if (MATCH("language", "unmounterror")) {
		pconfig->unmounterror = strdup(value);
	} else if (MATCH("language", "ext3")) {
		pconfig->ext3 = strdup(value);
	} else if (MATCH("language", "ext2")) {
		pconfig->ext2 = strdup(value);
	} else if (MATCH("language", "bmlchecking")) {
		pconfig->bmlchecking = strdup(value);
	} else if (MATCH("language", "bmlmayberfs")) {
		pconfig->bmlmayberfs = strdup(value);
	} else if (MATCH("language", "fsconv1")) {
		pconfig->fsconv1 = strdup(value);
	} else if (MATCH("language", "fsconv2")) {
		pconfig->fsconv2 = strdup(value);
	} else if (MATCH("language", "fsconv3")) {
		pconfig->fsconv3 = strdup(value);
	} else if (MATCH("language", "fsconv4")) {
		pconfig->fsconv4 = strdup(value);
	} else if (MATCH("language", "fsconv5")) {
		pconfig->fsconv5 = strdup(value);
	} else if (MATCH("language", "fsconv6")) {
		pconfig->fsconv6 = strdup(value);
	} else if (MATCH("language", "failprompt")) {
		pconfig->failprompt = strdup(value);
	} else if (MATCH("language", "installingupdate")) {
		pconfig->installingupdate = strdup(value);
	} else if (MATCH("language", "findupdatepackage")) {
		pconfig->findupdatepackage = strdup(value);
	} else if (MATCH("language", "openupdatepackage")) {
		pconfig->openupdatepackage = strdup(value);
	} else if (MATCH("language", "verifyupdatepackage")) {
		pconfig->verifyupdatepackage = strdup(value);
	} else if (MATCH("language", "edifyformatting")) {
		pconfig->edifyformatting = strdup(value);
	} else if (MATCH("language", "edifyformatdatadata")) {
		pconfig->edifyformatdatadata = strdup(value);
	} else if (MATCH("language", "edifywaitsdmount")) {
		pconfig->edifywaitsdmount = strdup(value);
	} else if (MATCH("language", "edifysdmounted")) {
		pconfig->edifysdmounted = strdup(value);
	} else if (MATCH("language", "edifysdtimeout")) {
		pconfig->edifysdtimeout = strdup(value);
	} else if (MATCH("language", "edifysdverifymarker")) {
		pconfig->edifysdverifymarker = strdup(value);
	} else if (MATCH("language", "edifysdmarkernotfound")) {
		pconfig->edifysdmarkernotfound = strdup(value);
	} else if (MATCH("language", "edifycheckinternalmarker")) {
		pconfig->edifycheckinternalmarker = strdup(value);
	} else if (MATCH("language", "edifyinternalmarkernotfound")) {
		pconfig->edifyinternalmarkernotfound = strdup(value);
	} else if (MATCH("language", "edifyrmscripterror")) {
		pconfig->edifyrmscripterror = strdup(value);
	} else if (MATCH("language", "formatcmd")) {
		pconfig->formatcmd = strdup(value);
	} else if (MATCH("language", "deletecmd")) {
		pconfig->deletecmd = strdup(value);
	} else if (MATCH("language", "copycmd")) {
		pconfig->copycmd = strdup(value);
	} else if (MATCH("language", "firmwriteextracting")) {
		pconfig->firmwriteextracting = strdup(value);
	} else if (MATCH("language", "firmwriteimage")) {
		pconfig->firmwriteimage = strdup(value);
	} else if (MATCH("language", "writeimage")) {
		pconfig->writeimage = strdup(value);
	} else if (MATCH("language", "settingsloaderror1")) {
		pconfig->settingsloaderror1 = strdup(value);
	} else if (MATCH("language", "settingsloaderror2")) {
		pconfig->settingsloaderror2 = strdup(value);
	} else if (MATCH("language", "settingsloaded")) {
		pconfig->settingsloaded = strdup(value);
	} else if (MATCH("language", "settingsthemeerror")) {
		pconfig->settingsthemeerror = strdup(value);
	} else if (MATCH("language", "settingsthemeloaded")) {
		pconfig->settingsthemeloaded = strdup(value);
	} else if (MATCH("language", "cotmainheader")) {
		pconfig->cotmainheader = strdup(value);
	} else if (MATCH("language", "cotthemeheader")) {
		pconfig->cotthemeheader = strdup(value);
	} else if (MATCH("language", "cotorsrebootheader")) {
		pconfig->cotorsrebootheader = strdup(value);
	} else if (MATCH("language", "cotorswipepromptheader")) {
		pconfig->cotorswipepromptheader = strdup(value);
	} else if (MATCH("language", "cotzippromptheader")) {
		pconfig->cotzippromptheader = strdup(value);
	} else if (MATCH("language", "cotlangheader")) {
		pconfig->cotlangheader = strdup(value);
	} else if (MATCH("language", "cotmainlisttheme")) {
		pconfig->cotmainlisttheme = strdup(value);
	} else if (MATCH("language", "cotmainlistrebootforce")) {
		pconfig->cotmainlistrebootforce = strdup(value);
	} else if (MATCH("language", "cotmainlistwipeprompt")) {
		pconfig->cotmainlistwipeprompt = strdup(value);
	} else if (MATCH("language", "cotmainlistzipprompt")) {
		pconfig->cotmainlistzipprompt = strdup(value);
	} else if (MATCH("language", "cotmainlistlanguage")) {
		pconfig->cotmainlistlanguage = strdup(value);
	} else if (MATCH("language", "cotthemehydro")) {
		pconfig->cotthemehydro = strdup(value);
	} else if (MATCH("language", "cotthemeblood")) {
		pconfig->cotthemeblood = strdup(value);
	} else if (MATCH("language", "cotthemelime")) {
		pconfig->cotthemelime = strdup(value);
	} else if (MATCH("language", "cotthemecitrus")) {
		pconfig->cotthemecitrus = strdup(value);
	} else if (MATCH("language", "cotthemedooderbutt")) {
		pconfig->cotthemedooderbutt = strdup(value);
	} else if (MATCH("language", "cotlangen")) {
		pconfig->cotlangen = strdup(value);
	} else if (MATCH("language", "setthemedefault")) {
		pconfig->setthemedefault = strdup(value);
	} else if (MATCH("language", "setthemeblood")) {
		pconfig->setthemeblood = strdup(value);
	} else if (MATCH("language", "setthemelime")) {
		pconfig->setthemelime = strdup(value);
	} else if (MATCH("language", "setthemecitrus")) {
		pconfig->setthemecitrus = strdup(value);
	} else if (MATCH("language", "setthemedooderbutt")) {
		pconfig->setthemedooderbutt = strdup(value);
	} else {
		return 0;
	}
	return 1;
}

void parse_language() {
    ensure_path_mounted("/sdcard");
	lang_keywords config;

	LOGI("Language code: %s\n", language);
	if (strcmp(language, "en") == 0) {
		ini_parse("/res/lang/lang_en.ini", language_handler, &config);
	} else {
		char full_lang_file[1000];
		char * lang_base;
		char * lang_end;

		lang_base = "/sdcard/0/cotrecovery/lang/lang_";
		lang_end = ".ini";
		strcpy(full_lang_file, lang_base);
		strcat(full_lang_file, language);
		strcat(full_lang_file, lang_end);

		if (ini_parse(full_lang_file, language_handler, &config) < 0) {
			LOGI("Can't load language code: %s\n", language);
		} else {
			LOGI("Loaded language code: %s\n", language);
		}
	}
	no = config.no;
	yes = config.yes;
	batterylevel = config.batterylevel;
	backmenudisabled = config.backmenudisabled;
	backmenuenabled = config.backmenuenabled;
	choosepackage = config.choosepackage;
	install = config.install;
	installing = config.installing;
	mounted = config.mounted;
	unmounted = config.unmounted;
	installabort = config.installabort;
	rebooting = config.rebooting;
	shutdown = config.shutdown;
	enabled = config.enabled;
	disabled = config.disabled;
	mounterror = config.mounterror;
	unmounterror = config.unmounterror;
	skipformat = config.skipformat;
	installconfirm = config.installconfirm;
	yesinstall = config.yesinstall;
	deleteconfirm = config.deleteconfirm;
	freespacesd = config.freespacesd;
	yesdelete = config.yesdelete;
	deleting = config.deleting;
	confirmformat = config.confirmformat;
	yesformat = config.yesformat;
	formatting = config.formatting;
	formaterror = config.formaterror;
	done = config.done;
	doneexc = config.doneexc;
	size = config.size;
	boot = config.boot;
	recovery = config.recovery;
	notfound = config.notfound;
	foundold = config.foundold;
	on = config.on;
	off = config.off;
	wipedataheader1 = config.wipedataheader1;
	wipedataheader2 = config.wipedataheader2;
	zipinstallheader = config.zipinstallheader;
	deletebackupheader = config.deletebackupheader;
	lowspaceheader1 = config.lowspaceheader1;
	lowspaceheader2 = config.lowspaceheader2;
	lowspaceheader3 = config.lowspaceheader3;
	lowspaceheader4 = config.lowspaceheader4;
	zipchooseheader = config.zipchooseheader;
	nandroidrestoreheader = config.nandroidrestoreheader;
	usbmsheader1 = config.usbmsheader1;
	usbmsheader2 = config.usbmsheader2;
	usbmsheader3 = config.usbmsheader3;
	recommended = config.recommended;
	showpartitionheader = config.showpartitionheader;
	advbackupheader = config.advbackupheader;
	advrestoreheader1 = config.advrestoreheader1;
	advrestoreheader2 = config.advrestoreheader2;
	advrestoreheader3 = config.advrestoreheader3;
	advrestoreheader11 = config.advrestoreheader11;
	nandroidheader = config.nandroidheader;
	debuggingheader = config.debuggingheader;
	advoptionsheader = config.advoptionsheader;
	rebootnow = config.rebootnow;
	wipedatafactory = config.wipedatafactory;
	wipecache = config.wipecache;
	wipeall = config.wipeall;
	installzip = config.installzip;
	nandroid = config.nandroid;
	mountsstorage = config.mountsstorage;
	advanced = config.advanced;
	langpoweroff = config.langpoweroff;
	signaturecheck = config.signaturecheck;
	scriptasserts = config.scriptasserts;
	zipchoosezip = config.zipchoosezip;
	zipapplyupdatezip = config.zipapplyupdatezip;
	ziptogglesig = config.ziptogglesig;
	ziptoggleasserts = config.ziptoggleasserts;
	backupproceed = config.backupproceed;
	backupcancel = config.backupcancel;
	zipchooseyesbackup = config.zipchooseyesbackup;
	zipchoosenobackup = config.zipchoosenobackup;
	zipcancelinstall = config.zipcancelinstall;
	usbmsunmount = config.usbmsunmount;
	usbmsmount = config.usbmsmount;
	nandroidbackup = config.nandroidbackup;
	nandroidrestore = config.nandroidrestore;
	nandroidrestoreboot = config.nandroidrestoreboot;
	nandroidrestoresys = config.nandroidrestoresys;
	nandroidrestoredata = config.nandroidrestoredata;
	nandroidrestorecache = config.nandroidrestorecache;
	nandroidrestoresd = config.nandroidrestoresd;
	nandroidadvbackup = config.nandroidadvbackup;
	nandroidadvrestore = config.nandroidadvrestore;
	nandroiddeleteold = config.nandroiddeleteold;
	debugfixperm = config.debugfixperm;
	debugfixloop = config.debugfixloop;
	debugreporterror = config.debugreporterror;
	debugkeytest = config.debugkeytest;
	debugshowlog = config.debugshowlog;
	debugtoggledebugui = config.debugtoggledebugui;
	advreboot = config.advreboot;
	advwipedalvik = config.advwipedalvik;
	advpartitionsd = config.advpartitionsd;
	advcotsettings = config.advcotsettings;
	advdebugopts = config.advdebugopts;
	lowspacecontinuebackup = config.lowspacecontinuebackup;
	lowspaceviewdelete = config.lowspaceviewdelete;
	lowspacecancel = config.lowspacecancel;
	yesdeletedata = config.yesdeletedata;
	wipingdata = config.wipingdata;
	datawipecomplete = config.datawipecomplete;
	datawipeskip = config.datawipeskip;
	datawipefail = config.datawipefail;
	confirmwipe = config.confirmwipe;
	yeswipecache = config.yeswipecache;
	wipingcache = config.wipingcache;
	cachewipecomplete = config.cachewipecomplete;
	cachewipeskip = config.cachewipeskip;
	cachewipefail = config.cachewipefail;
	confirmwipeall = config.confirmwipeall;
	yeswipeall = config.yeswipeall;
	wipingall = config.wipingall;
	wipeallcomplete = config.wipeallcomplete;
	yeswipedalvik = config.yeswipedalvik;
	wipingdalvik = config.wipingdalvik;
	wipedalvikcomplete = config.wipedalvikcomplete;
	wipedalvikskip = config.wipedalvikskip;
	partitioningsd = config.partitioningsd;
	partitionsderror = config.partitionsderror;
	rebootingsystemtimed = config.rebootingsystemtimed;
	diropenfail = config.diropenfail;
	nofilesfound = config.nofilesfound;
	ext4checking = config.ext4checking;
	orssdmountwait = config.orssdmountwait;
	orssdmounted = config.orssdmounted;
	orssdcontinuing = config.orssdcontinuing;
	orscommandis = config.orscommandis;
	orsnovalue = config.orsnovalue;
	orsinstallingzip = config.orsinstallingzip;
	orszipinstallerror = config.orszipinstallerror;
	orsrecursivemkdir = config.orsrecursivemkdir;
	orsrebootfound = config.orsrebootfound;
	orscmdnovalue = config.orscmdnovalue;
	orsunknowncmd = config.orsunknowncmd;
	orsscriptdone = config.orsscriptdone;
	orsscripterror = config.orsscripterror;
	nandroidbackupfolderset = config.nandroidbackupfolderset;
	nandroidbackupcomplete = config.nandroidbackupcomplete;
	nandroidsettingrestoreoptions = config.nandroidsettingrestoreoptions;
	nandroidrestorecomplete = config.nandroidrestorecomplete;
	nandroidbackupdeletecomplete = config.nandroidbackupdeletecomplete;
	nandroidconfirmrestore = config.nandroidconfirmrestore;
	nandroidyesrestore = config.nandroidyesrestore;
	backup = config.backup;
	restore = config.restore;
	performbackup = config.performbackup;
	nandroidbackingup = config.nandroidbackingup;
	nandroidcantmount = config.nandroidcantmount;
	nandroidyaffs2error = config.nandroidyaffs2error;
	nandroidsdfreespace = config.nandroidsdfreespace;
	bootdumperror = config.bootdumperror;
	recoverydumperror = config.recoverydumperror;
	nandroidandsecnotfound = config.nandroidandsecnotfound;
	nandroidsdextmountfail = config.nandroidsdextmountfail;
	nandroidmd5generate = config.nandroidmd5generate;
	nandroidmd5check = config.nandroidmd5check;
	nandroidmd5fail = config.nandroidmd5fail;
	nandroidtarnotfound = config.nandroidtarnotfound;
	nandroidimgnotfound = config.nandroidimgnotfound;
	restoring = config.restoring;
	restoreerror = config.restoreerror;
	nandroideraseboot = config.nandroideraseboot;
	nandroidrestoringboot = config.nandroidrestoringboot;
	nandroidbootflasherror = config.nandroidbootflasherror;
	nandroidnobootimg = config.nandroidnobootimg;
	nandroidbackupbootyes = config.nandroidbackupbootyes;
	nandroidbackupbootno = config.nandroidbackupbootno;
	nandroidbackuprecyes = config.nandroidbackuprecyes;
	nandroidbackuprecno = config.nandroidbackuprecno;
	nandroidbackupsysyes = config.nandroidbackupsysyes;
	nandroidbackupsysno = config.nandroidbackupsysno;
	nandroidbackupdatayes = config.nandroidbackupdatayes;
	nandroidbackupdatano = config.nandroidbackupdatano;
	nandroidbackupcacheyes = config.nandroidbackupcacheyes;
	nandroidbackupcacheno = config.nandroidbackupcacheno;
	nandroidbackupsdyes = config.nandroidbackupsdyes;
	nandroidbackupsdno = config.nandroidbackupsdno;
	securefsenable = config.securefsenable;
	securefsdisable = config.securefsdisable;
	securefsinvalid = config.securefsinvalid;
	securefsabortdefault = config.securefsabortdefault;
	securefsabort = config.securefsabort;
	securefsupdate = config.securefsupdate;
	uidebugenable = config.uidebugenable;
	uidebugdisable = config.uidebugdisable;
	fixingperm = config.fixingperm;
	outputkeycodes = config.outputkeycodes;
	kcgoback = config.kcgoback;
	installcomplete = config.installcomplete;
	yesinstallupdate = config.yesinstallupdate;
	a2sdnotfound = config.a2sdnotfound;
	device = config.device;
	ext3 = config.ext3;
	ext2 = config.ext2;
	bmlchecking = config.bmlchecking;
	bmlmayberfs = config.bmlmayberfs;
	fsconv1 = config.fsconv1;
	fsconv2 = config.fsconv2;
	fsconv3 = config.fsconv3;
	fsconv4 = config.fsconv4;
	fsconv5 = config.fsconv5;
	fsconv6 = config.fsconv6;
	failprompt = config.failprompt;
	installingupdate = config.installingupdate;
	findupdatepackage = config.findupdatepackage;
	openupdatepackage = config.openupdatepackage;
	verifyupdatepackage = config.verifyupdatepackage;
	edifyformatting = config.edifyformatting;
	edifyformatdatadata = config.edifyformatdatadata;
	edifywaitsdmount = config.edifywaitsdmount;
	edifysdmounted = config.edifysdmounted;
	edifysdtimeout = config.edifysdtimeout;
	edifysdverifymarker = config.edifysdverifymarker;
	edifysdmarkernotfound = config.edifysdmarkernotfound;
	edifycheckinternalmarker = config.edifycheckinternalmarker;
	edifyinternalmarkernotfound = config.edifyinternalmarkernotfound;
	edifyrmscripterror = config.edifyrmscripterror;
	formatcmd = config.formatcmd;
	deletecmd = config.deletecmd;
	copycmd = config.copycmd;
	firmwriteextracting = config.firmwriteextracting;
	firmwriteimage = config.firmwriteimage;
	writeimage = config.writeimage;
	settingsloaderror1 = config.settingsloaderror1;
	settingsloaderror2 = config.settingsloaderror2;
	settingsloaded = config.settingsloaded;
	settingsthemeerror = config.settingsthemeerror;
	settingsthemeloaded = config.settingsthemeloaded;
	cotmainheader = config.cotmainheader;
	cotthemeheader = config.cotthemeheader;
	cotorsrebootheader = config.cotorsrebootheader;
	cotorswipepromptheader = config.cotorswipepromptheader;
	cotzippromptheader = config.cotzippromptheader;
	cotlangheader = config.cotlangheader;
	cotmainlisttheme = config.cotmainlisttheme;
	cotmainlistrebootforce = config.cotmainlistrebootforce;
	cotmainlistwipeprompt = config.cotmainlistwipeprompt;
	cotmainlistzipprompt = config.cotmainlistzipprompt;
	cotmainlistlanguage = config.cotmainlistlanguage;
	cotthemehydro = config.cotthemehydro;
	cotthemeblood = config.cotthemeblood;
	cotthemelime = config.cotthemelime;
	cotthemecitrus = config.cotthemecitrus;
	cotthemedooderbutt = config.cotthemedooderbutt;
	cotlangen = config.cotlangen;
	setthemedefault = config.setthemedefault;
	setthemeblood = config.setthemeblood;
	setthemelime = config.setthemelime;
	setthemecitrus = config.setthemecitrus;
	setthemedooderbutt = config.setthemedooderbutt;
}
