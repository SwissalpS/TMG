#!/usr/bin/php -q
<?php
/* * * *
 * * prepareFiles.phps
 * * populate fields in template files with values passed on cli
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */
error_reporting(E_ALL); // 0); //

require_once('../inc/SssS_TMGPrepare.php.inc');

// instantiate the main object
$oSssS_TMGPrepare = new SssS_TMGPrepare();

// run the main object
$oSssS_TMGPrepare->run();

// if not yet exited, all OK
exit(0);

/* * * *\ prepareFiles.phps (CC) Luke JZ aka SwissalpS /* * * */
?>
