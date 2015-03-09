#!/usr/bin/php -q
<?php
/* * * *
 * * cacheDiskUUIDs.phps
 * * cache UUIDs of disks used by Time Machine
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */
error_reporting(E_ALL); // 0); //

// make sure that the correct file is included, especially during install
// php may prefer the originating directory which results in wrong cache
// path and failure
$sSssSpathTMGDDinc = dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR
				 . 'inc' . DIRECTORY_SEPARATOR . 'SssS_TMGDiskDetector.php.inc';
require_once($sSssSpathTMGDDinc);

// instantiate the main object
$oSssSDiskDetector = new SssS_TMGDiskDetector();

// run the main object
$oSssSDiskDetector->refreshCacheOfUUIDs();

// if not yet exited, all OK
exit(0);

/* * * *\ cacheDiskUUIDs.phps (CC) Luke JZ aka SwissalpS /* * * */
?>
