(*
uninstall.applescript

runs uninstall.sh

by SwissalpS@LukeZimmermann.com
Rev 20150225_245904

copyright: free to use at own risk
*)

(* PROMPTS BEGIN *)

property sPromptUninstall : "Really want to uninstall? This script will run uninstall.sh"

property sPromptTerminalOrBackground : "Would you like the action to run in a Terminal window or in the background?"

(* PROMPTS END *)

-- global variables
global bUseTerminal

-- for Terminal execution only
global mainWin


-- main routine, called by double-clicking AppleScript icon
on run argv
	
	my init()
	
	-- prompt user, if really wants to uninstall
	set bReady to my alertUninstall()
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


to alertUninstall()
	
	set sButtonCancel to "Cancel"
	set sButtonOK to "Go Ahead"
	
	display dialog my sPromptUninstall buttons {sButtonCancel, sButtonOK} default button 1
	set the button_pressed to the button returned of the result
	
	if the button_pressed is sButtonCancel then
		
		-- cancel
		return null
		
	else
		
		-- OK
		return true
		
	end if
	
end alertUninstall


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


to posixPathToMe()
	
	set theScript to ("dirname " & (POSIX path of (path to me)) as Unicode text)
	
	set sPathBase to do shell script theScript
	
	return sPathBase
	
end posixPathToMe


to runInstallInBackground()
	
	try
		
		-- caching script
		do shell script (my shellScriptU()) with administrator privileges
		
	on error eMsg number eNum
		
		return false
		
	end try
	
	return true
	
end runInstallInBackground


to runInstallInTerminalWindow()
	
	try
		
		tell application "Terminal"
			
			-- get a reference to Terminal tab/window (mainWin) for reuse
			set mainWin to do script "clear;echo 'uninstall starting up...';"
			
			try
				-- this script requires admin credentials
				set theScript to (my shellScriptU())
				
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


to shellScriptU()
	
	-- uninstalling, needs admin authorisation
	
	set theScript to quoted form of (my posixPathToMe() & "/uninstall.sh")
	
	return theScript
	
end shellScriptU