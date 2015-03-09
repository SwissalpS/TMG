(*
cacheDiskUUIDs.applescript

runs cacheDiskUUIDs.sh to refresh cached UUIDs of volumes used by Time Machine
you need to have all volumes mounted

by SwissalpS@LukeZimmermann.com
Rev 20150225_245904

copyright: free to use at own risk
*)

(* PROMPTS BEGIN *)

property sPromptPrepareTM : "In System Preferences > Time Machine setup all the volumes you want to use to backup onto." & return & "Set Time Machine's switch to 'OFF' position (left)." & return & return & "Important: mount all the volumes you want to use before proceeding with this script!" & return & return & "Read the portion" & return & "'### General prerequisites for smooth operation ###'"

property sPromptTerminalOrBackground : "Would you like the action to run in a Terminal window or in the background?"

(* PROMPTS END *)

-- global variables
global bUseTerminal

-- for Terminal execution only
global mainWin


-- main routine, called by double-clicking AppleScript icon
on run argv
	
	my init()
	
	-- prompt user to setup Time Machine and mount all volumes
	set bReady to my alertReadme()
	if null is bReady then return my bail()
	
	-- if current user is admin, we can offer to run in terminal
	if my isCurrentUserAdmin() then
		
		-- ask if should use terminal or background process
		set bUseTerminal to my chooseTerminalOrBackground()
		if null is bUseTerminal then return my bail()
		
	else
		
		set bUseTerminal to false
		
	end if
	
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
	
	return {my bUseTerminal, my posixPathToMe()}
	
end bail


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
	
	set theScript to ("dirname " & (POSIX path of (path to me)) as Unicode text)
	
	set sPathBase to do shell script theScript
	
	return sPathBase
	
end posixPathToMe


to runInstallInBackground()
	
	try
		
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
			set mainWin to do script "clear;echo 'cacheDiskUUIDs starting up...';"
			
			try
				-- this script requires admin credentials
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


to shellScriptC()
	
	-- caching, needs admin authorisation
	
	set theScript to "cd " & quoted form of (my posixPathToMe()) & "; sudo ./cacheDiskUUIDs.phps;"
	
	return theScript
	
end shellScriptC