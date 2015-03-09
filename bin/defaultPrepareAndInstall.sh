#! /bin/sh
##
## defaultPrepareAndInstall.sh
## prepares and installs with default values
##
## requires admin account to run!
##
## exits with code:
##    1 - if not in directory with prepareFiles.phps, or not executable.
##    2 - install.sh is not in this directory or not executable.
##    3 - if prepareFiles.phps did not exit with code 0
##    4 - if install.sh did not exit with code 0
##
## Script by SwissalpS@LukeZimmermann.com
## Rev 20150210_013415
## "tested" on OS X 10.9.1

if [ -x "prepareFiles.phps" ]; then

	echo "OK, let's prepare files";

else

	echo "cd to bin directory containing this script to run.";

	exit 1;

fi;

## run preparation with default values
./prepareFiles.phps /usr/local/SwissalpS_TMGraft /var/log/SwissalpS_TMGraft 40 12 3 33 verbose;

## catch the exit code
iRes=$?;

if [ "0" = "$iRes" ]; then

	echo "OK, files prepared.";

else

	echo "There was an error preparing files, bailing.";

	exit 3;

fi;

## double check to see if install.sh exists and is executable
if [ -x "install.sh" ]; then

	echo "OK, let's install";

else

	echo "could not execute install.sh";

	exit 2;

fi;

## install
./install.sh

## catch the exit code
iRes=$?;

if [ "0" = "$iRes" ]; then

	echo "OK, installed, enjoy life :D";

else

	echo "There was an error installing files, bailing.";

	exit 4;

fi;

## exit OK
exit 0;
