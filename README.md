<pre> Cannibal Open Touch Recovery
==================================

<ul>
<li>Originally ClockworkMod based recovery we have incorporated and updated touch controls originally written by Napstar of Team Utter Chaos.</li>
<li>Combines features of well-known recoveries like TWRP, ClockworkMod and AmonRA to allow users to easily and effortlessly manage their Android-powered devices.</li>
<li>Learn more about Cannibal Open Touch and Project Open Cannibal at http://www.projectopencannibal.net/the-project/ or come join as on our Forums at http://forums.projectopencannibal.net/.</li>
<li>Such features include but are not limited to:</li>
</ul>
	* User defined backup locations.
	* Support for Open Recovery Scripts (http://www.teamw.in/OpenRecoveryScript).
	* Persistent settings.
	* Sideloadable (sdcard) theme support.
	* ADB Sideload.

<pre>Please note the following in regards to this branch.
=========================================================

<ul>
<li>Inclosed is a hybridized gingerbread recovery with backported ext4 and 'Retouch Binaries' for 3.0 boot image compatibility on the Kindle Fire (first edition).</li>
<li>The reboot wrappers and orientation are setup specfically for the Kindle Fire (first edition).</li>
<li>Building this branch requires the system/extras repo from Jellybean, specfically 'system/extras/ext4_utils' in order to work.</li>
<li>If you are building for any device other than the Kindle Fire (first edition) please checkout either https://github.com/ProjectOpenCannibal/android_bootable_recovery/tree/gingerbread (legacy devices) or https://github.com/ProjectOpenCannibal/android_bootable_recovery/tree/jellybean (newer devices, experimental) instead.</li>
<li>If you are building for the Kindle Fire (first edition) consider checking out a specific tag, they will be the ones labeled landscape.</li>
<li>The following repos are required for all current builds: https://github.com/ProjectOpenCannibal/android_bootable_recovery_res and https://github.com/ProjectOpenCannibal/android_bootable_recovery_gui/tree/gingerbread.</li>
</ul>

<ul>
<li>You may checkout all the required resources as well as the most recent stable tag by using one of the included local_manifests here: https://github.com/ProjectOpenCannibal/cotr-local_manifests</li>
</ul>
