<?php
/* * * *
 * * SssS_TMGPrepare.php.inc
 * * prepare for install
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */

// make sure some constants are defined
if ( ! defined('NL')) define('NL', chr(10));
if ( ! defined('DIR_SEP')) define('DIR_SEP', DIRECTORY_SEPARATOR);

require_once('SssS_TMGShellScriptRunnerSimplified.php.inc');

class SssS_TMGPrepare {

	// array of template files
	var $aTemplateFiles;

	// should we output messages
	var $bVerbose;

	// launch hour
	var $iLaunchHour;

	// launch minute
	var $iLaunchMinute;

	// first wait seconds
	var $iWaitSecondsFirst;

	// second wait seconds
	var $iWaitSecondsSecond;

	// object holder for running shell scripts
	var $oRunner;

	// install location
	var $sPathInstall;

	// log location
	var $sPathLog;

	// base path to where this script is
	var $sPathOrigin;


	// constructor
	function SssS_TMGPrepare() {

		// get path of parent-folder of script location
		$this->sPathOrigin = dirname(dirname(__FILE__));

		// init shell runner object
		$this->oRunner = new SssS_TMGShellScriptRunnerSimplified();

	} // SssS_TMGPrepare


	function bailOK() {

		// output goodbye message
		if ($this->bVerbose) {

			echo 'I hope it\'s all working in your favour - have a nice day.' . NL;

		} // if verbose

		// and bail
		exit(0);

	} // bailOK


	// exit after outputing usage message
	function bailUsage() {

		// output usage message
		echo 'Usage: prepareFiles.phps <absolute path to install to>  '
			. '<absolute path for log files> <seconds to wait for TM> '
			. '<subsequent seconds to wait for TM to finish> '
			. '<hour to launch> <minutes after hour to launch> [verbose]' . NL . NL
			. 'Please read in the README about manual install.' . NL;

		// and bail
		exit(1);

	} // bailUsage


	// find the template files
	function findTemplatesInProject() {

		if (! is_dir($this->sPathOrigin)) return array();

		// if verbose, output some info
		if ($this->bVerbose) {

			echo 'Searching for template files ending in .SssStemplate' . NL;

		} // if verbose

		// make sure find gets it that we are looking at a folder....
		$sScript = 'find -L "' . $this->sPathOrigin . '" -name "*.SssStemplate";';

		$this->oRunner->doScript($sScript);

		// cleanup
		unset($sScript);

		return explode("\n", $this->oRunner->sOut());

	} // findTemplatesInProject


	function populateTemplates() {

		$aSearches = array('###SssSlogPath###',
							'###SssSinstallPath###',
							'###SssSlaunchMinute###',
							'###SssSlaunchHour###',
							'###SssSiSecondsForBackup###',
							'###SssSiSecondsToWaitInLoop###',
							'###SssSsLaunchdName###');

		$aReplacements = array($this->sPathLog,
							$this->sPathInstall,
							$this->iLaunchMinute,
							$this->iLaunchHour,
							$this->iWaitSecondsFirst,
							$this->iWaitSecondsSecond,
							'ws.SwissalpS.TMGraft');

		// init a counter for verbose output
		$iCount = 0;

		// apply replacements for place-holders, one file at a time
		forEach ($this->aTemplateFiles as $sPathFile) {

			// remove any whitespace (pre and post)
			$sPathFile = trim($sPathFile);

			// skip if empty
			if (empty($sPathFile)) continue;

			// notify which file is being read
			if ($this->bVerbose) {

				echo 'Reading ' . $sPathFile . NL;

			} // if verbose

			// read contents of template
			$sContent = file_get_contents($sPathFile);

			// generate new name
			$sName = basename($sPathFile, '.SssStemplate');

			// new path and file-name
			$sPathFile = dirname($sPathFile) . DIR_SEP . $sName;

			// replace markers in content
			$sContent = str_replace($aSearches, $aReplacements, $sContent);

			// notify which file is being written
			if ($this->bVerbose) {

				echo 'Writing ' . $sPathFile . NL;

			} // if verbose

			// write new content to new file
			$mRes = @file_put_contents($sPathFile, $sContent);

			if (false === $mRes) {

				echo 'For some reason I was not able to write file.' . NL
					. 'Is this direcotory writeable for current user?' . NL
					. dirname($sPathFile) . NL;

				$this->bailUsage();

			} // if write failed

			// increment counter
			$iCount++;

		} // loop all found files

		if ($this->bVerbose) {

			echo 'Worked on ' . $iCount . ' template file';

			if (0 == $iCount || 1 < $iCount) {

				echo 's';

			} // if need 's'

			echo NL;

		} // if verbose

		// cleanup
		unset($aSearches, $aReplacements, $iCount, $sPathFile, $sContent);

	} // populateTemplates


	function readArguments() {

		// fetch the arguments
		$a = $_SERVER['argv'];

		// drop invocation
		array_shift($a);

		// not enough arguments -> bail printing usage
		if (6 > count($a)) $this->bailUsage();

		// be verbose?
		$this->bVerbose = (7 == count($a));

		// install location
		$this->sPathInstall = rtrim(trim(array_shift($a)), "/\\");

		// log location
		$this->sPathLog = rtrim(trim(array_shift($a)), "/\\");

		// first wait seconds
		$this->iWaitSecondsFirst = abs(intval(trim(array_shift($a))));

		// second wait seconds
		$this->iWaitSecondsSecond = abs(intval(trim(array_shift($a))));

		// launch hour
		$this->iLaunchHour = abs(intval(trim(array_shift($a)))) % 24;

		// launch minute
		$this->iLaunchMinute = abs(intval(trim(array_shift($a)))) % 60;

		// check paths
		if (empty($this->sPathInstall) || empty($this->sPathLog)) $this->bailUsage();

		// cleanup
		unset($a);

	} // readArguments


	function run() {

		// parse input, bail if not enough
		$this->readArguments();

		// collect template files
		$this->aTemplateFiles = $this->findTemplatesInProject();

		// apply values passed on command line to templates
		$this->populateTemplates();

		// make sure install.sh is executable
		$this->setFlags();

		// exit
		$this->bailOK();

	} // run


	function setFlags() {

		// make sure install.sh is executable
		$sPathFile = $this->sPathOrigin . DIR_SEP . 'bin' . DIR_SEP . 'install.sh';

		$sScript = 'chmod a+x "' . $sPathFile . '";';

		$this->oRunner->doScript($sScript);

	} // setFlags

} // SssS_TMGPrepare

/* * * *\ SssS_TMGPrepare.php.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
