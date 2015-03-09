#! /bin/sh
##
## startTMbackup.sh
##
## exits with code:
##    0 - if successful
##
## Script by SwissalpS@LukeZimmermann.com
## Rev 20150222_212923
## "tested" on OS X 10.9.1

## start Time Machine backup, and return status code
tmutil startbackup -r;

