<?php
/* * * *
 * * SssS_TMGDiskDetector.php.inc
 * * detect which disks are used by Time Machine and cache their UUIDs
 * *
 * * part of project: ws.SwissalpS.TMGraft
 * *
 * * developed and tested on a mac w OS X 10.9.x
 * *
 * * @version 2015023_163115 (CC) Luke JZ aka SwissalpS
 * * * */

/*
example output of
tmutil destinationinfo
> ==================================================
Name          : L
Kind          : Local
ID            : 2494DF65-1950-405B-ACAC-B7CC26AD1393
====================================================
Name          : VMs
Kind          : Local
Mount Point   : /Volumes/LL
ID            : D9D00542-FFE6-40B7-94CA-3AE9D3B3433C
*/

/*
comments to entries
> ==================================================	'> ='	indicates last used volume
Name          : May not be the current drive name, also duplicates are possible
Kind          : Local or Network
URL			  : Only availabe for network drive
Mount Point   : Only available when mounted
ID            : Time Machine ID can change and is NOT UUID!

*/

require_once('SssS_TMGShellScriptRunnerSimplified.php.inc');


class SssS_TMGDestination {

	// was last used drive?
	protected $bWasLast;

	// Local or Network
	protected $sKind;

	// label of drive (not unique, may not be what Finder displays)
	protected $sName;

	// mount point if mounted
	protected $sPath;

	// current Time Machine ID
	protected $sTMid;

	// url if network drive
	protected $sURL;

	// UUID of drive
	protected $sUUID;


	function SssS_TMGDestination($aData) {

		$this->bWasLast = false;
		$this->sKind = null;
		$this->sName = null;
		$this->sPath = null;
		$this->sTMid = null;
		$this->sURL = null;
		$this->sUUID = null;

		if ( ! is_array($aData)) {

			// TODO: maybe output message?

			return $this;

		} // if not an array given

		if (isset($aData['LastDestination'])) $this->bWasLast = true;
		if (isset($aData['Kind'])) $this->sKind = $aData['Kind'];
		if (isset($aData['Name'])) $this->sName = $aData['Name'];
		if (isset($aData['MountPoint'])) $this->sPath = $aData['MountPoint'];
		if (isset($aData['ID'])) $this->sTMid = $aData['ID'];
		if (isset($aData['URL'])) $this->sURL = $aData['URL'];

		return $this;

	} // SssS_TMGDestination


	function getMountPoint() {

		return ((null === $this->sPath) ? '' : $this->sPath);

	} // getMountPoint


	function getName() {

		return ((null === $this->sName) ? '' : $this->sName);

	} // getName


	function getTMid() {

		return ((null === $this->sTMid) ? '' : $this->sTMid);

	} // getTMid


	function getUUID() {

		return ((null === $this->sUUID) ? '' : $this->sUUID);

	} // getUUID


	function isLastUsed() {

		return $this->bWasLast;

	} // isLastUsed


	function isLocalDrive() {

		return ('Local' === $this->sKind);

	} // isLocalDrive


	function isMounted() {

		return (null !== $this->sPath);

	} // isMounted


	function isNetworkDrive() {

		return ('Local' !== $this->sKind);

	} // isNetworkDrive


	function setUUID($sNewUUID = '') {

		$this->sUUID = $sNewUUID;

	} // setUUID

} // SssS_TMGDestination



class SssS_TMGDiskDetector {

	// array of destination objects
	protected $aDisks;

	// array of found UUIDs
	protected $aUUIDs;

	// array of error-messages
	protected $aErrors;

	// shell script runner object
	protected $oRunner;

	// last used drive index of $aDisks
	protected $iLastUsed;


	public function SssS_TMGDiskDetector() {

		$this->oRunner = new SssS_TMGShellScriptRunnerSimplified();

		$this->aErrors = array();

	} // SssS_TMGDiskDetector


	// add an error-message to error array
	protected function addEM($sErrorMessage = '') {

		$this->aErrors[] = $sErrorMessage;

	} // addEM


	// exit script with error-code coresponding to error-count
	protected function bail() {

		$iCountErrors = count($this->aErrors);

		//if (0 < $iCountErrors) {

			// TODO: wrap text
			forEach ($this->aErrors as $sError) echo '- ' . $sError . chr(10);

		//} // if encountered errors

		exit($iCountErrors);

	} // bail


	protected function doScript($sScript, $bMuted = false) {

		$this->oRunner->doScript($sScript, $bMuted);

		return (0 === $this->oRunner->iRes());

	} // doScript


	protected function eject($sDeviceIdentifier = '') {

		if (empty($sDeviceIdentifier)) return false;

		$sScript = 'diskutil eject ' . $sDeviceIdentifier;

		return $this->doScript($sScript);

	} // eject


	// will exit!
	// main entrance
	public function ejectCurrent() {

		// get current UUID from cache file
		$sUUID = self::sLastUsedUUID();

		if (null === $sUUID) {

			$this->addEM('failed to retrieve UUID from cache -> unable to'
							. ' eject volume.');

			// TODO: instead of giving up, run TM destinationinfo again and detect which was used last....
			$this->bail();

		} // if not able to get UUID from cache

		// eject - seems to always return 1 so we can't rely on the exit code
		$this->eject($sUUID);

		// give system some time to eject
		sleep(7);

		if ($this->isMounted($sUUID)) {

			$this->addEM('error ejecting volume.');

//var_dump($this->oRunner->aResults);

		} // if error ejecting

		// quit with exit code corresponding to error count
		$this->bail();

	} // ejectCurrent


	// returns null on error, else boolean
	protected function isMounted($sDeviceIdentifier = '') {

		if (empty($sDeviceIdentifier)) return null;

		$sScript = 'diskutil info -plist ' . $sDeviceIdentifier;

		if ($this->doScript($sScript)) {

			// output is the plist content
			$sPlist = $this->oRunner->sOut();

			// load plist handler class
			require_once('SssS_Plist.inc');

			// initiate plist object with plist content
			$oP = new SssS_Plist($sPlist);

			// extract mount point
			$sMountPoint = $oP->get('MountPoint', '');

			// discard plist object
			unset($oP);

			if (empty($sMountPoint)) {

				return false;

			} else {

				return true;

			} // if has mount point or not

		} else {

			$this->addEM('error detecting if volume is mounted');

			return null;

		} // if fetching info plist worked or not

	} // isMounted


	// (run as root for this, so permissions on files are granted)
	protected function makeUUIDfiles() {

		if (0 === count($this->aUUIDs)) {

			// no UUIDs found
			$this->addEM('failed to find any UUIDs. Setup Time Machine again '
							. 'and make sure all drives are mounted. Then try caching'
							. ' again.');

			return;

		} // if UUIDs found or not

		// determine cache path
		$sPath = self::sCachePath();

		// check existance and writability
		if ( ! is_writable($sPath)) {

			$this->addEM('cache path (' . $sPath . ') either does not exist or'
							. ' is not writable or you have invoked this script as wrong'
							. ' user. Did you install correctly?');

			return;

		} // if cache dir does not exist or is not writable

		$sUUIDs = implode(chr(10), $this->aUUIDs);

		// determine last used UUID
		$oDrive = $this->aDisks[$this->iLastUsed];
		$sUUID = $oDrive->getUUID();

		if ( ! in_array($sUUID, $this->aUUIDs)) {

			$sUUID = $this->aUUIDs[0];

		} // if not in array of UUIDs ie has no UUID

		// write list of UUIDs
		$mRes = @file_put_contents(self::sUUIDsPathFile(), $sUUIDs);
		if (false === $mRes) {

			$this->addEM('failed to write list of UUIDs to ' . $sPath);

		} // if failed to write

		// write last used UUID
		if ( ! self::writeLastUsedUUID($sUUID)) {

			$this->addEM('failed to write last used UUID to ' . $sPath);

		} // if failed to write

	} // makeUUIDfiles


	// will exit!
	// main entrance
	public function mountNext() {

		// fetch cached UUIDs
		$this->aUUIDs = self::aAvailableUUIDs();

		if (null === $this->aUUIDs) {

			$this->addEM('error reading cached UUIDs.');

			$this->bail();

		} // if error getting cached UUIDs

		// fetch last used UUID
		$sUUID = self::sLastUsedUUID();

		if (null === $sUUID) {

			$this->addEM('error reading cached UUID (last used).');

			$this->bail();

		} // if error getting cached UUID

		// count UUIDs
		$iCountUUIDs = count($this->aUUIDs);

		if (0 == $iCountUUIDs) {

			$this->addEM('no UUIDs found in cache.');

			$this->bail();

		} // if no UUIDs in cache

		// search for last used UUID in UUIDs to determine next UUID to use
		$mKey = array_search($sUUID, $this->aUUIDs);

		if (false === $mKey) {

			// TODO: maybe treat this as error? this is the only output that isn't fatal
			echo 'Notice: last used UUID, ' . $sUUID . ', was not found in cached UUIDs.'
				. ' Using first in cache.';

			$mKey = -1;

		} // if UUID not found in UUIDs

		// choose next entry wrapping around
		$iKey = ($mKey + 1) % $iCountUUIDs;

		// NOTICE: recycling variable
		$sUUID = $this->aUUIDs[$iKey];

		// write to 'lastUsed' cache
		if ( ! self::writeLastUsedUUID($sUUID)) {

			$this->addEM('failed to write current UUID to cache.');

		} // if failed to write to cache

		// mount
		if ( ! $this->mountVolume($sUUID)) {

			$this->addEM('failed to mount volume with UUID (' . $sUUID . ')');

		} // if failed to mount volume

		// exit with error-count as exit-code
		$this->bail();

	} // mountNext


	protected function mountVolume($sDeviceIdentifier = '') {

		if (empty($sDeviceIdentifier)) return false;

		$sScript = 'diskutil mount ' . $sDeviceIdentifier;

		return $this->doScript($sScript);

	} // mountVolume


	// run as root for this, so permissions on files are granted
	// will exit!
	// main entrance
	public function refreshCacheOfUUIDs() {

		// get list of drives setup with Time Machine
		if ( ! $this->doScript('tmutil destinationinfo -X')) {

			$this->addEM('failed to run "tmutil destinationinfo"');

			$this->bail();

		} // if error

		// output is the plist content
		$sPlist = $this->oRunner->sOut();

		// load plist handler class
		require_once('SssS_Plist.inc');

		// initiate plist object with plist content
		$oP = new SssS_Plist($sPlist);

		// 'convert' to array
		$aDestinations = $oP->get('Destinations', array());

		// discard plist object
		unset($oP);

		// reset array of disk objects
		$this->aDisks = array();

		// reset array of found UUIDs
		$this->aUUIDs = array();

		// reset last used index
		$this->iLastUsed = null;

		// reset index count
		$iCount = 0;

		forEach ($aDestinations as $aDisk) {

			// instantiate a destination object from data portion
			$oDisk = new SssS_TMGDestination($aDisk);

			// add to array of destinations
			$this->aDisks[] = $oDisk;

			// memorize as last used, if so
			if ($oDisk->isLastUsed()) $this->iLastUsed = $iCount;

			// increment counter
			$iCount++;

			// try to find UUID by drive's name, not until no other way worked
			$bTryByName = false;

			if ($oDisk->isMounted()) {

				// try to figure out UUID of drive with mount point
				$sPath = $oDisk->getMountPoint();

				$sUUIDscript = 'diskutil info "' . $sPath . '" | grep "UUID"';

				if ($this->doScript($sUUIDscript)) {

					$sUUID = self::sOutToUUID($this->oRunner->sOut());

					$oDisk->setUUID($sUUID);

					$this->aUUIDs[] = $sUUID;

				} else {

					// error
					$this->addEM('failed to determine UUID by mount point: '
									 . $sPath);

					// try by name
					$bTryByName = true;

				} // if got uuid or not

			} else {

				// not mounted
				$this->addEM('drive not mounted. { Name: ' . $oDisk->getName()
								 . ', TM-ID: ' . $oDisk->getTMid()
								 . '}, please mount the drive and retry.');

				// try by name
				$bTryByName = true;

			} // if currently mounted or not

			if ($bTryByName) {

				// let's attempt with name
				$sName = $oDisk->getName();

				// try to figure out UUID of drive by name
				$sUUIDscript = 'diskutil info ' . $sName . ' | grep "UUID"';

				if ( ! $this->oRunner->doScript($sUUIDscript)) {

					$this->addEM('failed to determine UUID by drive name. { Name: '
									 . $sName . ', TM-ID: ' . $oDisk->getTMid()
									 . '}, please mount the drive and retry.');

				} else {

					$sUUID = self::sOutToUUID($this->oRunner->sOut());

					$oDisk->setUUID($sUUID);

					$this->aUUIDs[] = $sUUID;

				} // if got response or not

			} // if try by name

		} // loop all disks

		// cache gathered info
		$this->makeUUIDfiles();

		// exit with error-count as exit-code
		$this->bail();

	} // refreshCacheOfUUIDs


	protected function unmount($sDeviceIdentifier = '') {

		if (empty($sDeviceIdentifier)) return false;

		$sScript = 'diskutil unmount ' . $sDeviceIdentifier;

		return $this->doScript($sScript);

	} // unmount


	// will exit!
	// main entrance
	public function unmountCurrent() {

		// get current UUID from cache file
		$sUUID = self::sLastUsedUUID();

		if (null === $sUUID) {

			$this->addEM('failed to retrieve UUID from cache -> unable to'
							. ' unmount volume.');

			// TODO: instead of giving up, run TM destinationinfo again and detect which was used last....
			$this->bail();

		} // if not able to get UUID from cache

		// eject - seems to always return 1 so we can't rely on the exit code
		$this->unmount($sUUID);

		// give system some time to eject
		sleep(7);

		if ($this->isMounted($sUUID)) {

			$this->addEM('error unmounting volume.');

//var_dump($this->oRunner->aResults);

		} // if error ejecting

		// quit with exit code corresponding to error count
		$this->bail();

	} // unmountCurrent


	// will exit!
	// main entrance
	public function unmountCurrentDisk() {

		// get current UUID from cache file
		$sUUID = self::sLastUsedUUID();

		if (null === $sUUID) {

			$this->addEM('failed to retrieve UUID from cache -> unable to'
							. ' unmount disk.');

			// TODO: instead of giving up, run TM destinationinfo again and detect which was used last....
			$this->bail();

		} // if not able to get UUID from cache

		// eject - seems to always return 1 so we can't rely on the exit code
		$this->unmountDisk($sUUID);

		// give system some time to eject
		sleep(7);

		if ($this->isMounted($sUUID)) {

			$this->addEM('error unmounting disk.');

//var_dump($this->oRunner->aResults);

		} // if error ejecting

		// quit with exit code corresponding to error count
		$this->bail();

	} // unmountCurrentDisk


	protected function unmountDisk($sDeviceIdentifier = '') {

		if (empty($sDeviceIdentifier)) return false;

		$sScript = 'diskutil unmountDisk ' . $sDeviceIdentifier;

		return $this->doScript($sScript);

	} // unmountDisk


	// returns null on error or array of cached UUIDs
	static function aAvailableUUIDs() {

		$mRes = @file_get_contents(self::sUUIDsPathFile());

		if (false === $mRes) return null;

		$aIDs = array();

		$aRes = explode(chr(10), trim($mRes));

		forEach ($aRes as $sLine) {

			// if first character is '#' skip
			if ('#' == $sLine{0}) continue;

			$aIDs[] = $sLine;

		} // loop all lines skipping comments

		// remove any duplicates
		return array_unique($aIDs);

	} // aAvailableUUIDs


	// determine base path of installation based on __FILE__ (with trailing slash)
	static function sBasePath() {

		static $sPath = null;
		if (null === $sPath) {

			$sPath = dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR;

		} // if first request

		return $sPath;

	} // sBasePath


	// cache path of installation, with trailing slash
	static function sCachePath() {

		static $sPath = null;
		if (null === $sPath) {

			$sPath = self::sBasePath() . 'cache' . DIRECTORY_SEPARATOR;

		} // if first request

		return $sPath;

	} // sCachePath


	// UUID of current/last used destination volume
	// returns null on error or whatever is in the cache file
	static function sLastUsedUUID() {

		$sPathFile = self::sLastUsedUUIDPathFile();

		$mRes = @file_get_contents($sPathFile);

		if (false === $mRes) return null;

		return trim($mRes);

	} // sLastUsedUUID


	// determine cache path of last used UUID
	static function sLastUsedUUIDPathFile() {

		static $sPath = null;
		if (null === $sPath) {

			$sPath = self::sCachePath() . 'lastUsedUUID';

		} // if first request

		return $sPath;

	} // sLastUsedUUIDPathFile


	// strip UUID from grepped diskutil info output
	static function sOutToUUID($sOut = '') {

		//$sUUID = substr(trim($sOut), 29);

		$aUUID = explode(' ', trim($sOut));

		$sUUID = empty($aUUID) ? '' : $aUUID[(count($aUUID) -1)];

		unset($aUUID);

		return $sUUID;

	} // sOutToUUID


	// determine cache path and file of UUIDs
	static function sUUIDsPathFile() {

		static $sPath = null;
		if (null === $sPath) {

			$sPath = self::sCachePath() . 'availableUUIDs';

		} // if first request

		return $sPath;

	} // sUUIDsPathFile


	// write UUID to cache as last used
	static function writeLastUsedUUID($sUUID = '') {

		$mRes = @file_put_contents(self::sLastUsedUUIDPathFile(), $sUUID);

		return (false !== $mRes);

	} // writeLastUsedUUID($sUUID)

} // SssS_TMGDiskDetector

/* * * *\ SssS_TMGDiskDetector.php.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
