README.md
# SwissalpS Time Machine Graft #
Target Audience: System administrators especially in scenarios where the files are large
enough that a random backup would disturb the workflow.

** Purpose of Time Machine Graft (TMG) **
- Tell Time Machine (TM) to run a daily backup at a predefined time.
- Eject and mount volumes as needed for more security and power-saving.
- Enable media rotation.


## Table Of Contents ##
* [Description](#swissalps-time-machine-graft)
* [Authors Info](#authors-info)
* [Version Info](#version-info)
* [Where to get from](#where-to-get-from)
* [General prerequisites for smooth operation](#general-prerequisites-for-smooth-operation)
* [How to install using AppleScript](#how-to-install-using-applescript)
* [How to install using command line interface aka Terminal.app](#how-to-install-using-command-line-interface-aka-terminal-app)
 * [Using default values](#using-default-values)
 * [Expert mode](#expert-mode)
* [Modifications after install](#modifications-after-install)
* [Uninstall](#uninstall)
* [Manual uninstall](#manual-uninstall)
* [Adding or removing Time Machine target volumes](#adding-or-removing-time-machine-target-volumes)
* [About AppleScripts in this project](#about-applescripts-in-this-project)
* [Legal](#legal)
* [Donations](#donations)


## Authors Info ##
Luke Zimmermann aka SwissalpS
SwissalpS@LukeZimmermann.com

Idea and testing by Gideon Zimmermann

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Version Info ##
20150220_230809 Tested on OS X 10.9.x

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Where to get from ##
- **disk image** Still looking for a host for image..... (https://.img)
- **git** https://github.com/SwissalpS/TMG.git
- **git** git@github.com:SwissalpS/TMG.git

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## General prerequisites for smooth operation ##
In System Preferences > Time Machine setup all the volumes you want to use to backup onto.
(You may need to unlock the lock with admin credentials to do so).

Set Time Machine's switch to 'OFF' position (left).

*Note*: if you have renamed a volume after having added it to Time Machine, please remove it from Time Machine's list and add it again to avoid confusions.

*Note*: the volumes may be partitions on local drives or locally attached drives. They may also be sparseimages mounted over the network.

**Important**: mount *all* the volumes you want to use *before* proceeding with installation!

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## How to install using AppleScript ##
0. Follow steps described under [General prerequisites for smooth operation](#general-prerequisites-for-smooth-operation)
1. Download and mount the disk image or download with git.
2. Double-click `chooseInstallTarget.app` from the 'bin' folder.
3. Enter the requested information or go with the default values.

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## How to install using command line interface aka Terminal.app ##
0. Follow steps described under [General prerequisites for smooth operation](#general-prerequisites-for-smooth-operation)
1. Download and mount disk image or download with git.
2. cd into the bin directory **as an admin user**
3. continue with [Using default values](#using-default-values) or [Expert mode](#expert-mode)

### Using default values ###
3. run: `./defaultPrepareAndInstall.sh`
  this sets up the required directories and files with correct permissions using default
  values.
4. done.


### Expert mode ###
3. run: `./prepareFiles.phps install-location log-location first-wait second-wait launch-hour launch-minute[ verbose]`

   where:

   **install-location**: is where to install to, e.g. `/usr/local/SwissalpS_TMGraft`

	**WARNING**: install into dedicated enclosing directory! If you install into e.g. `/usr/local` and then run `uninstall.sh`, entire `/usr/local` **will be deleted forever!**

    **log-location**: is where to write log files, e.g. `/var/log/SwissalpS_TMGraft`

	**WARNING**: install into dedicated enclosing directory! If you install into e.g. `/var/log` and then run `uninstall.sh`, entire `/var/log` **will be deleted forever!**

	**first-wait**: is seconds to wait before first check if Time Machine is still busy.

    **second-wait**: is seconds for successive waits to check if Time Machine is still busy.

    **launch-hour**: {0-23} at which hour of the day to run backup.

    **launch-minute**: {0-59} at which minute after the hour to run backup.

    **verbose**: optional, have preparation script be verbal about what it is doing.

4. run: `./install.sh`
  this sets up the required directories and files with correct permissions. Also UUIDs are cached.
5. done.

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Modifications after install ##
This package was written to avoid the errors that can occur when modifying system files.
But, if you really want to do so, you can modify parameters in:
- `/System/Library/LaunchDaemons/ws.SwissalpS.TMGraft.plist`
- `/etc/newsyslog.d/ws.SwissalpS.TMGraft.conf`

Rather uninstall and install again with new parameters unless you are a super-user
and comfortable with vim or emacs.

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Uninstall ##
Either
run `bin/uninstall.app` (will cause AppleScript error message as the script/app is deleted in the process. The message reads, on english systems: "File some object wasn't found.")

or

run `bin/uninstall.sh` as admin user from anywhere

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Manual uninstall ##
read `bin/uninstall.sh` and follow the steps manually
- unload from launchd
- delete .conf
- delete .plist
- delete log files
- delete main folder

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Adding or removing Time Machine target volumes ##
Whenever you add or subtract volumes from Time Machine's list of targets,
the cache needs to be updated.
To do so, mount all volumes and then **either**
 run `bin/cacheDiskUUIDs.app`
**OR**
 on the command line, as admin:
- cd to bin directory
- `sudo ./cacheDiskUUIDs.phps`

If you cannot mount all volumes at once, there are valid reasons, you need to manually
maintain the cached list of UUIDs.

Edit `/install/path/SwissalpS_TMGraft/cache/availableUUIDs`

One UUID per line, you may write comments using '#' as first character.
Save as UTF-8 encoded text file with UNIX line endings (ASCII 10).

`sudo chown root:wheel availableUUIDs`

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## About AppleScripts in this project ##
In this documentation the AppleScripts are referred to with .app suffix.
Depending on the distribution, such as git repositories, the files will end with .applescript as suffix.
This has to do with the format the scripts are saved as.
- .app: binary and resources, double-click in Finder to run.
- .applescript: text based, thus useable with revisioning systems such as git.
   double-click in Finder will open in "AppleScript Editor.app". Hit Command+R to run.

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)

## Legal ##
[Read LICENCE file](LICENCE) (in same location as this file)



## Donations ##
Please get yourself extra sweat karma by donating to following PayPal accounts:

Luke@SwissalpS.biz

[Or buy some merchandise at SwissalpS.biz](http://SwissalpS.biz)

Appart from getting yourself karma, you can encourage the authors to keep on improving
and making more useful tools for free. Thank you.

----
[Top](#swissalps-time-machine-graft) | [Table Of Contents](#table-of-contents)
