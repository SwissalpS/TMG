(*
chooseInstallTarget.applescript

chooses the install location and other parameters,
runs prepareFiles.phps then install.sh and then runs cacheDiskUUIDs.sh

by SwissalpS@LukeZimmermann.com
Rev 20150225_245904

copyright: free to use at own risk
*)

(* PROMPTS BEGIN *)

property sPromptPrepareTM : "In System Preferences > Time Machine setup all the volumes you want to use to backup onto." & return & "Set Time Machine's switch to 'OFF' position (left)." & return & return & "Important: mount all the volumes you want to use before proceeding with this script!" & return & return & "Read the portion" & return & "'### General prerequisites for smooth operation ###'"

property sPromptInstallLocation : "Where to install the executables to (An enclosing folder will be created with name: SwissalpS_TMGraft)" & return & "It's safest to use paths without spaces and special characters"
property sPromptLogLocation : "Where to put the log files to (An enclosing folder will be created with name: SwissalpS_TMGraft)" & return & "It's safest to use paths without spaces and special characters"

-- minimum and maximum will be added to these strings
property sPromptFirstWait : "How many seconds to wait before checking if TM is still working?"
property sPromptSecondWait : "Interval in seconds to wait before checking if TM is still working after first check?"
property sPromptLaunchHour : "At which hour do you want the backup scheduled?"
property sPromptLaunchMinute : "At which minute after the hour?"

property sPromptTerminalOrBackground : "Would you like the installation to run in a Terminal window or in the background?"


(* PROMPTS END *)

(* DEFAULT VALUES BEGIN *)

property sDefaultInstallLocation : "/Users/Shared/" -- "/usr/local/"
property sDefaultLogLocation : "/var/log/"

property iDefaultFirstWaitSeconds : 40
property iDefaultSecondWaitSeconds : 12

property iDefaultLaunchHour : 22
property iDefaultLaunchMinute : 22

(* DEFAULT VALUES END *)

-- global variables
global theInstallLocation, theLogLocation, theFirstWait, theSecondWait, theLaunchHour, theLaunchMinute
global bUseTerminal

-- for Terminal execution only
global mainWin


-- main routine, called by double-clicking AppleScript icon
on run argv
	
	my init()
	
	-- prompt user to setup Time Machine and mount all volumes
	set bReady to my alertReadme()
	if null is bReady then return my bail()
	
	-- get launch hour
	set theLaunchHour to my chooseInteger(my sPromptLaunchHour, my iDefaultLaunchHour, 0, 23)
	if null is theLaunchHour then return my bail()
	
	-- get launch minute
	set theLaunchMinute to my chooseInteger(my sPromptLaunchMinute, my iDefaultLaunchMinute, 0, 59)
	if null is theLaunchMinute then return my bail()
	
	-- get install location
	set theInstallLocation to my choosePath(my sPromptInstallLocation, my sDefaultInstallLocation)
	if null is theInstallLocation then return my bail()
	--
	set theInstallLocation to theInstallLocation & "SwissalpS_TMGraft"
	
	-- get log location
	set theLogLocation to my choosePath(my sPromptLogLocation, my sDefaultLogLocation)
	if null is theLogLocation then return my bail()
	--
	set theLogLocation to theLogLocation & "SwissalpS_TMGraft"
	
	-- get initial wait time
	set theFirstWait to my chooseInteger(my sPromptFirstWait, my iDefaultFirstWaitSeconds, 2, 43200)
	if null is theFirstWait then return my bail()
	
	-- get concurrent wait time
	set theSecondWait to my chooseInteger(my sPromptSecondWait, my iDefaultSecondWaitSeconds, 2, 3600)
	if null is theSecondWait then return my bail()
	
	-- if current user is admin, we can offer to run in terminal
	if my isCurrentUserAdmin() then
		
		-- ask if should use terminal or background process
		set bUseTerminal to my chooseTerminalOrBackground()
		if null is bUseTerminal then return my bail()
		
	else
		
		set bUseTerminal to false
		
	end if
	
	-- review settings
	set bOKrun to my reviewSettings()
	if null is bOKrun then return my bail()
	
	if false is bUseTerminal then
		
		my runInstallInBackground()
		
	else
		
		my runInstallInTerminalWindow()
		
	end if
	
	return my bail()
	
end run


to alertReadme()
	
	set sButtonCancel to "Cancel"
	set sButtonReadme to "Open Readme"
	set sButtonOK to "Go Ahead"
	
	display dialog my sPromptPrepareTM buttons {sButtonCancel, sButtonReadme, sButtonOK} default button 2
	set the button_pressed to the button returned of the result
	
	if the button_pressed is sButtonCancel then
		
		-- cancel
		return null
		
	else if the button_pressed is sButtonReadme then
		
		-- open readme
		tell application "TextEdit"
			
			open my pathToReadme()
			activate
			
		end tell
		
		-- and quit
		return null
		
	else
		
		-- OK
		return true
		
	end if
	
end alertReadme


to bail()
	
	--say "Goodbye"
	
	return {my theInstallLocation, my theLogLocation, my theFirstWait, my theSecondWait, my bUseTerminal, my theLaunchHour, my theLaunchMinute, my posixPathToMe()}
	
end bail


to chooseInteger(thePrompt, theDefaultInteger, theMinimum, theMaximum)
	
	set thePrompt to thePrompt & " (" & theMinimum & "-" & theMaximum & ")"
	
	try
		repeat
			
			display dialog thePrompt default answer theDefaultInteger buttons {"Cancel", "OK"} default button 2
			copy the result as list to {button_pressed, text_returned}
			
			if "OK" is not button_pressed then
				
				error "User canceled" number 15
				
			end if
			
			-- simple verification. Floats will be rounded to integer
			try
				
				set theInteger to text_returned as integer
				
				-- make sure value is in range
				if theMinimum ² theInteger and theMaximum ³ theInteger then exit repeat
				
			end try
			
		end repeat
		
		return theInteger
		
	on error eMsg number eNum
		
		display dialog "Either you canceled or there was an error, bailing---" & eMsg buttons {"OK"} default button "OK"
		
		return null
		
	end try
	
end chooseInteger


to choosePath(thePrompt, theDefaultLocation)
	
	try
		
		set theLocation to (POSIX path of (choose folder with prompt thePrompt default location theDefaultLocation))
		
		return theLocation
		
	on error eMsg number eNum
		
		display dialog "Either you canceled or there was an error, bailing---" & eMsg buttons {"OK"} default button "OK"
		
		return null
		
	end try
	
end choosePath


to chooseTerminalOrBackground()
	
	set sButtonCancel to "Cancel"
	set sButtonTerminal to "Terminal"
	set sButtonBackground to "Background"
	
	display dialog my sPromptTerminalOrBackground buttons {sButtonCancel, sButtonTerminal, sButtonBackground} default button 2
	set the button_pressed to the button returned of the result
	
	if the button_pressed is sButtonCancel then
		
		-- cancel
		return null
		
	else if the button_pressed is sButtonTerminal then
		
		-- terminal window
		return true
		
	else
		
		-- background
		return false
		
	end if
	
end chooseTerminalOrBackground


to init()
	
	set theInstallLocation to "" --my sDefaultInstallLocation
	set theLogLocation to ""
	set theFirstWait to 0
	set theSecondWait to 0
	set theLaunchHour to null
	set theLaunchMinute to null
	set bUseTerminal to null
	
end init


to isCurrentUserAdmin()
	
	-- cache current delimiters
	set atids to AppleScript's text item delimiters
	-- explode with space
	set AppleScript's text item delimiters to " "
	
	try
		
		set sGroups to (do shell script "groups") --"id -G -n"
		
		-- split string by space
		set lGroups to text items of sGroups
		
		-- detect if in list
		set bIsAdmin to ("admin" is in lGroups)
		
	on error
		
		set bIsAdmin to false
		
	end try
	
	-- set delimiters back to what they were
	set AppleScript's text item delimiters to atids
	
	return bIsAdmin
	
end isCurrentUserAdmin


to pathToReadme()
	
	set sPathBase to my posixPathToMe()
	
	set theScript to (("dirname '" & sPathBase & "'") as Unicode text)
	
	set sPathBase to do shell script theScript
	
	return POSIX file ((sPathBase & "/README.md") as Unicode text)
	
end pathToReadme


to posixPathToMe()
	
	set theScript to ("dirname '" & (POSIX path of (path to me) & "'") as Unicode text)
	
	set sPathBase to do shell script theScript
	
	return sPathBase
	
end posixPathToMe


to reviewSettings()
	
	set thePrompt to "I will now install into:" & return & my theInstallLocation & return & "also making a folder in:" & return & theLogLocation & return & "using " & my theFirstWait & " seconds for first wait" & return & "and " & my theSecondWait & " seconds for concurrent intervals." & return & "Scheduling the backup for " & my theLaunchHour & ":" & my theLaunchMinute & return & "I'll run installation script in "
	
	if true is my bUseTerminal then
		
		set thePrompt to thePrompt & "a Terminal window."
		
	else
		
		set thePrompt to thePrompt & "the background."
		
	end if
	
	try
		
		display dialog thePrompt buttons {"Cancel", "Go Ahead"} default button 2
		
	on error
		
		return null
		
	end try
	
	if the button returned of the result is "Cancel" then
		
		return null
		
	else
		
		return true
		
	end if
	
end reviewSettings


to runInstallInBackground()
	
	try
		
		-- preparation script
		do shell script (my shellScriptA())
		
		-- install script
		do shell script (my shellScriptB()) with administrator privileges
		
		-- caching script
		do shell script (my shellScriptC()) with administrator privileges
		
	on error eMsg number eNum
		
		return false
		
	end try
	
	return true
	
end runInstallInBackground


to runInstallInTerminalWindow()
	
	try
		
		tell application "Terminal"
			
			
			-- get a reference to Terminal tab/window (mainWin) for reuse
			set mainWin to do script "echo 'Preparing installation...';"
			
			try
				
				set theScript to (my shellScriptA())
				
				-- returns tab/window (mainWin) and no error detection
				do script theScript in my mainWin
				
				-- this script requires admin credentials
				set theScript to (my shellScriptB())
				
				-- returns tab/window (mainWin) and no error detection
				do script theScript in my mainWin
				
				-- this script also requires admin credentials
				set theScript to (my shellScriptC())
				
				-- returns tab/window (mainWin) and no error detection
				do script theScript in my mainWin
				
			on error eMsg number eNum
				
				activate me
				display dialog eMsg
				
			end try
			
		end tell
		
	on error
		
		activate me
		
		display dialog "couldn't run in Terminal window"
		
		return null
		
	end try
	
	return true
	
end runInstallInTerminalWindow


to shellScriptA()
	
	--return "echo 'hahahah';"
	-- prepareation script, no need for admin authorisation
	
	set theScript to "cd " & quoted form of (my posixPathToMe()) & "; ./prepareFiles.phps " & quoted form of (my theInstallLocation) & " " & quoted form of (my theLogLocation) & " " & my theFirstWait & " " & my theSecondWait & " " & my theLaunchHour & " " & my theLaunchMinute
	
	if my bUseTerminal then set theScript to theScript & " verbose"
	
	return theScript
	
end shellScriptA


to shellScriptB()
	
	-- actual install, needs admin authorisation
	
	set theScript to "cd " & quoted form of (my posixPathToMe()) & "; ./install.sh;"
	
	return theScript
	
end shellScriptB


to shellScriptC()
	
	-- caching, needs admin authorisation
	
	set theScript to "cd " & quoted form of (my posixPathToMe()) & "; sudo ./cacheDiskUUIDs.phps;"
	
	return theScript
	
end shellScriptC