#! /bin/sh
##
## isTimeMachineRunning.sh
##
## exits with code:
##    1 - if TimeMachine is NOT running
##    0 - if TimeMachine IS running
##  254 - if state can not be detected
##
## Script by SwissalpS@LukeZimmermann.com
## Rev 20150210_013415
## "tested" on OS X 10.9.1

## grep output of tmutil status for line containing "Running"
sGrepTMstatus=`tmutil status | grep "Running"`;

## is not running, running or some error
if [ "    Running = 0;" = "$sGrepTMstatus" ]; then
  
  #echo "Not Running";
  exit 1;

elif [ "    Running = 1;" = "$sGrepTMstatus" ]; then

  #echo "Running";
  exit 0;

else

  #echo "Unknown State";
  exit 254;

fi;

exit 255;

