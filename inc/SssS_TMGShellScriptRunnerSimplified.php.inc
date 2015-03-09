<?php
/* * * *
 * * SssS_TMGShellScriptRunnerSimplified.php.inc
 * * run shell scripts
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */

class SssS_TMGShellScriptRunnerSimplified {

	// var chosen for all 3 to enable maximum flexibility
	// the current script or the one just executed
	var $sScript;
	// hash set and returned after execution with results
	var $aResults;

	function SssS_TMGShellScriptRunnerSimplified() {

		//$this->fReset();

	} // SssS_TMGShellScriptRunnerSimplified


	public function fReset() {

		$this->sScript = '';
		$this->aResults = array(
				'scriptExitCode'	=> null,
				'fullOutput'		=> null,
				'lastLine'			=> null);

	} // fReset


	public function doScript($sScript = '', $bMuted = false) {

		$this->fReset();

		$this->sScript = $sScript;

		try {

			ob_start();

			$sLastLine = ($bMuted)
					? @system($this->sScript, $iRes)
					: system($this->sScript, $iRes);

			$sOut = ob_get_clean();

		} catch (Exception $e) {

			if (!isset($sLastLine)) $sLastLine = '1';

			if (!isset($sOut)) $sOut =
					' --- something bad happened ---' . "\n" .
					'   with shell script --->' . $sScript . '<-- ';

		} // try catch errors

		// fill up our results array
		$this->aResults = array(
				'scriptExitCode'	=> $iRes,
				'fullOutput'		=> $sOut,
				'lastLine'			=> $sLastLine);

		return $this->aResults;
	} // doScript

// 20100501_200518 SwissalpS added some getters
	public function iRes() { return $this->scriptExitCode(); } // iRes
	public function exitCode() { return $this->scriptExitCode(); } // exitCode
	public function scriptExitCode() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['scriptExitCode'])) return null;
		return $this->aResults['scriptExitCode'];
	} // scriptExitCode
	public function sOut() { return $this->fullOutput(); } // sOut
	public function fullOutput() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['fullOutput'])) return null;
		return $this->aResults['fullOutput'];
	} // fullOutput
	public function sLline() { return $this->sLastLine(); } // sLline
	public function sLastLine() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['lastLine'])) return null;
		return $this->aResults['lastLine'];
	} // sLastLine

    function __toString() { return print_r($this, true); } // __toString

} // SssS_TMGShellScriptRunnerSimplified

/* * * *\ SssS_TMGShellScriptRunnerSimplified.php.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
