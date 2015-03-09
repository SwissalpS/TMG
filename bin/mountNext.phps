#!/usr/bin/php -q
<?php
/* * * *
 * * mountNext.phps
 * * mount the next volume in cached list of UUIDs
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */
error_reporting(E_ALL); // 0); //

require_once('../inc/SssS_TMGDiskDetector.php.inc');

// instantiate the main object
$oSssSDiskDetector = new SssS_TMGDiskDetector();

// run the main object
$oSssSDiskDetector->mountNext();

// if not yet exited, all OK
exit(0);

/* * * *\ cacheDiskUUIDs.phps (CC) Luke JZ aka SwissalpS /* * * */
?>
