'---------------------------------------------------------------------------------------------------------------------------'
'															    '
' 			  Uninstall Script for Admin Mod v2.50.x							    '
'			  Copyright (C) 2000 Alfred Reynolds (alfred@mazuma.net.au) 				    '
'			  Copyright (C) 2001-2003 David Mart 				    '
'			  $Id: uninstall_admin.vbs,v 1.2 2003/11/08 10:21:30 darope Exp $			    '
' 															    '
'----------------------------------------------------------------------------------------------Declarations-and-setup-------'
Option Explicit ' enforces explicit variable declaration, don't touch this.
'---------------------------------------------------------------------------------------------------------------------------'
'  To enable debugging set the line below to "Const DEBUGME=1" , boolean value 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const DEBUGME = 0
'---------------------------------------------------------------------------------------------------------------------------'
'  To disable filesystem changes set the  below to "Const NEUTER=1" , boolean value 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const NEUTER = 0
'---------------------------------------------------------------------------------------------------------------------------'
Dim FSO,TSO,FSD,TSD,HLpath,HLDSpath,silent,InConsoleMode,InCVSMode,ScriptPath,OldConfigsPath,IndentValue,paths
class Modification: public ModPath: public NewType: end class
class InstallPaths: public DodPath:public CSPath:public HLPath:public HLDSPath:public STEAMPath: public Count: end class
Const ForReading = 1, ForWriting = 2, ForAppending = 8
Const AM_CONFIG="\addons\adminmod\config",AM_DLLS="\addons\adminmod\dlls",AM_SCRIPTS="\addons\adminmod\scripts",AM_ROOT="\addons\adminmod"
Const MM_DLLS="\addons\metamod\dlls",MM_ROOT="\addons\metamod"
'---------------------------------------------------------------------------------------------------------------------------'
on error resume next
Set fso = CreateObject("Scripting.FileSystemObject")
if err.number<>0 then message "The uninstaller was unable to create the ActiveX componant required to"&vbcrlf&"interact with your computers filesystem."&vbcrlf&"This is a problem with your windows scripting host installtion and can be fixed"&vbcrlf&"by reinstalling/updating the Windows Scripting Host."&vbcrlf&vbcrlf&"The windows scripting host can be downloaded free from Microsoft at"&vbcrlf&"http://msdn.microsoft.com/scripting",vbokonly,"Object creation error": err.clear: wscript.quit 1
on error goto 0
'---------------------------------------------------------------------------------------------------------------------------'
main()

private sub main()
	dim Mods(),UninstallIndex,BackPath,answer,RemovedMetamod: redim Mods(0)
	
	DebugStart()
	ScriptPath    = left(wscript.scriptfullname,instrrev(wscript.scriptfullname,"\")-1)
	InConsoleMode = IsHostCscript()
	GetCommandLineArgs()
	
	if silent = 0 then 
		answer=message("Do you wish to let the uninstaller use your registry to locate your admin mod installation?"&vbcrlf&"If not then you will be required to provide a full path to the halflife installation."&vbcrlf&vbcrlf&"Click Yes to automatically locate Admin."&vbcrlf&"Click No to enter a path."&vbcrlf&"Click Cancel to exit the Uninstaller",vbyesnocancel,"Allow Admin Mod Uninstaller to read a registry key?")
		if answer=vbyes then
			set Paths = new InstallPaths
			GetHLKeys(paths)
			if paths.HLPath   <>"" then GetAdminMods paths.HLPath,   Mods 
			if paths.HLDSPath <>"" then GetAdminMods paths.HLDSPath, Mods 
			if paths.dodpath  <>"" then GetAdminMods paths.dodpath,  Mods
			if paths.cspath   <>"" then GetAdminMods paths.cspath,   Mods
			if paths.steampath<>"" then GetAdminMods paths.steampath,Mods
		elseif answer=vbno then
			specify()
			DetermineAdminPresence HLpath,Mods
		else
			DebugClose(): wscript.quit
		end if
		
	else
		if DetermineAdminPresence(HLpath,Mods )=false then
			debug "Admin Mod was not located, this probably means that metamod is already installed"
			debugClose(): wscript.quit
		end if
	end if
	
	if silent=0 then UninstallIndex = PromptUser(mods) else UninstallIndex=0
	
	if mods(UninstallIndex).NewType = true then
		BackPath = CreateBackupPath("new")
		New_UninstallAdmin   mods(UninstallIndex).ModPath,BackPath
		if not New_OtherMMDllsPresent(mods(UninstallIndex).ModPath) then
			New_UninstallMetamod mods(UninstallIndex).ModPath,BackPath
			RestoreBackUPLiblist mods(UninstallIndex).ModPath,BackPath
			RemoveAddons(mods(UninstallIndex).ModPath&"\addons")
			RemovedMetamod=true
		else
			RemovedMetamod=false
			RemovePluginsEntry mods(UninstallIndex).ModPath&MM_ROOT&"\plugins.ini"
			RenameAMBackupLiblist(mods(UninstallIndex).ModPath &"\AdminMod backup of liblist.gam")
		end if
	else
		BackPath = CreateBackupPath("old")
		Old_UninstallAdmin   mods(UninstallIndex).ModPath,BackPath
		Old_UninstallMetamod mods(UninstallIndex).ModPath,BackPath
		RestoreBackUPLiblist mods(UninstallIndex).ModPath,BackPath
		RemovedMetamod=true
	end if 
	
	if silent = 0 then
		debug "displaying end dialogue"
		if RemovedMetamod=true then
			message "Admin Mod has been uninstalled."&vbcrlf&vbcrlf&"The files have been backed up to a folder in the same diretory as the"&vbcrlf&"uninstallation script. if you wish to keep any settings you will need those"&vbcrlf&"files. Otherwise please delete the folder."&vbcrlf&vbcrlf&"Click Ok to finish",vbokonly,"Uninstallation Completed"
		else
			message "Admin Mod has been uninstalled."&vbcrlf&vbcrlf&"The files have been backed up to a folder in the same directory as the"&vbcrlf&"uninstallation script. if you wish to keep any settings you will need those"&vbcrlf&"files. Otherwise please delete the folder."&vbcrlf&vbcrlf&"Metamod has not been uninstalled because other dlls were located which may"&vbcrlf&"be using it. If it is no longer required you will have to remove it manually"&vbcrlf&vbcrlf&"Click Ok to finish",vbokonly,"Uninstallation Completed"
		end if
	end if

	debug vbcrlf&"script has finished uninstallation, it will close destroy it's objects and return success"
	DebugClose(): wscript.quit
end sub
'----------------------------------------------------------------------------------------------------------------------------
private function CreateBackupPath(Version)
	dim step
	dim TempPath
	debug "beginning CreateBackupPath function"
	tempPath= Scriptpath &"\Admin Mod 2.50 uninstall "&datepart("yyyy",now) &datepart("m",now)&datepart("d",now)&"-"&datepart("h",now)&datepart("n",now)&datepart("s",now)
	if not folderexists (tempPath) then
		createfolder    (tempPath)
		CreateBackupPath=tempPath
	else
		do while folderexists(tempPath&" "&Step)<>false
			step=step+1
		loop
		createfolder(tempPath&" "&Step)
		CreateBackupPath=tempPath&" "&Step
	end if
	debug "completed CreateBackupPath function"&vbcrlf
end function
'----------------------------------------------------------------------------------------------------------------------------
private function PromptUser(byref InArr)
	dim OutString,answer,x,UninstallLocation
	debug "beginning PromptUser sub"
	if typename(inarr(ubound(InArr)))<>"Empty" then
		if ubound(InArr) = 0 then
			debug vbtab&"single Admin mod installation located, prompting"
			outstring="Admin Mod 2.50.50 Installation found at:"&vbcrlf&vbcrlf&InArr(0).ModPath&vbcrlf&vbcrlf&"Do you wish to uninstall this version of Admin Mod?"
			answer=message(outstring,vbyesno,"Uninstall")
			if answer=vbyes then 
				debug vbtab&"user chose to uninstall """&InArr(0).ModPath&""""
				UninstallLocation=0
			else
				debug vbtab&"user chose not to uninstall, quitting"
				DebugClose(): wscript.quit
			end if
		else
			debug vbtab&"multiple ("&ubound(InArr)+1&") Admin Mod installations located, prompting"
			outstring="Multiple Admin Mod 2.5x Installations found."&vbcrlf&vbcrlf&"Give the NUMBER of the version you wish to uninstall (e.g. 1 )"&vbcrlf&vbcrlf
			for x =0 to ubound(InArr)
				if typename(InArr(x))<>"Empty" then
					OutString=OutString&vbcrlf&x+1&" )  "&InArr(x).ModPath
				end if		
			next
			outstring=outstring&vbcrlf&vbcrlf&"Press Ok when the number has been entered."
			answer =input(outstring,"Uninstall")
			if answer="" then debug vbcrlf&"user chose to cancel uninstallation": DebugClose(): wscript.quit
			debug vbtab&"user entered """&answer&""""
			if isnumeric(answer) then
				if cint(answer)-1 <=ubound(InArr) and cint(answer)-1>=lbound(InArr) then
					UninstallLocation=cint(answer)-1
				else
					debug vbtab&"user entered an invalid selection"
					message "invalid selection"&vbcrlf&vbcrlf&"This script will now exit.",0,"Idiot": DebugClose(): wscript.quit
				end if
			else
				debug vbtab&"user entered an invalid selection"
				message "invalid selection"&vbcrlf&vbcrlf&"This script will now exit.",0,"Idiot": DebugClose(): wscript.quit
			end if
		end if	
	else
		debug "no Admin Mod found, closing"
		message "No Admin Mod installations were found."&vbcrlf&vbcrlf&"If you currenly have Admin mod installed then you should remove it manually before attempting to install Admin Mod again."&vbcrlf&vbcrlf&"This script will now exit.",0,"No Admin Mod found"
		debug "completed PromptUser function"&vbcrlf
		DebugClose(): wscript.quit 1
	end if
	PromptUser=UninstallLocation
	debug vbtab&"user chose to uninstall """&InArr(UninstallLocation).ModPath&""""
	debug "completed PromptUser function"&vbcrlf
end function
'----------------------------------------------------------------------------------------------------------------------------
private sub GetAdminMods(byval Path,byref OutArr)
	debug "beginning GetAdminMods sub"
	dim Folder,subfolder,ltso,invar
	if folderexists(Path) then
		set folder=fso.getfolder(path)
		for each subfolder in folder.subfolders
			if fileexists(path&"\"&subfolder.name&"\liblist.gam") then
				debug vbtab&"opening liblist.gam and scanning for ""metamod.dll"""
				set ltso=fso.opentextfile(path&"\"&subfolder.name&"\liblist.gam",forreading)
				invar=trim(ltso.readall)
				ltso.close: set ltso=nothing
				if instr(1,invar,"metamod.dll")>0 then ' metamod is installed
					debug vbtab&"found ""metamod.dll"", metamod is installed for this mod"
					indent() 
					DetermineAdminPresence path&"\"&subfolder.name,OutArr
					unindent()
				else
					debug vbtab&"no ""metamod.dll"" found, no metamod installed for this mod"
				end if
			else
			end if
		next	
	end if
	debug "completed GetAdminMods sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private function DetermineAdminPresence(byval ModPath,byref OutArr)
	debug vbcrlf&"beginning DetermineAdminPresence sub"
	dim LocalVersion,FoundAdmin
	FoundAdmin=false
	
	if folderexists(ModPath &"\addons\adminmod") then
		FoundAdmin=true
		localversion=true
		debug vbtab&"new style Admin mod installation found"
	else
		if fileexists(ModPath&"\dlls\admin_mm.dll") then 
			FoundAdmin=true
			localversion=false
			debug vbtab&"old style Admin mod installation found"
		end if
	end if
	
	if FoundAdmin =true then
		if typename(outarr(ubound(outarr))) <> "Empty" then redim preserve outarr(ubound(outarr)+1)
		set outarr(ubound(outarr))=new modification
		outarr(ubound(outarr)).ModPath =ModPath
		outarr(ubound(outarr)).NewType =localversion
		DetermineAdminPresence=true
	else
		DetermineAdminPresence=false
	end if
	debug "completed DetermineAdminPresence sub"&vbcrlf
end function
'----------------------------------------------------------------------------------------------Full-Path-Secification-Sub----
Private Sub Specify()
	dim path
	debug "beginning Specify sub" & vbcrlf & vbtab & "prompting use to supply a valid fully qualified path"
	Path=Input("Specify the full path to the HL, Cstrike or HLDS executable file for which you" & vbcrlf & "wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
	If Path="" Then debug vbtab & "user chose to cancel or entered a null string, quitting": DebugClose(): wscript.quit
	If fileexists(path)<>True Or (lcase(right(path,8))<>"hlds.exe" And lcase(right(path,6))<>"hl.exe"  And lcase(right(path,11))<>"cstrike.exe") Then
		debug vbtab&"incorrect but not null path entered, second prompting"
		Path=Input("Incorrect Path." & vbCrlf & vbCrlf & "Specify the full path to the HL, Cstrike or HLDS executable file for which you wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
		if fileexists(path)<>true and lcase(right(path,8))<>"hlds.exe" or lcase(right(path,6))<>"hl.exe" then debug vbtab&"second incorrect path, the user should find the correct path manually or learn to type, quitting": DebugClose(): wscript.quit
	End If
	debug vbTab & "Input path = " & Path
	if len(Path)>8 then
		If lcase(right(Path,8))="hlds.exe" Then HLDSpath =left(Path,len(Path)-9)
	end if
	if len(Path)>6 then
		If lcase(right(Path,6))="hl.exe" Then HLpath =left(Path,len(Path)-7)
	end if
	if len(Path)>11 then
		If lcase(right(Path,11))="cstrike.exe" Then HLpath =left(Path,len(Path)-12)
	end if
	debug vbTab&"resultant path = " &Path&vbcrlf&"completed Secify sub"&vbCrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private function GetHLKeys(byref Paths)
	Dim ValveHLKey,SolHlKey,SolCSKey,SolHLDSKey,SoftHLKey,SoftCSKey,SoftHLDSKey,SoftDODKey,SolDODKey,SoftStreamKey,WSHShell
	debug "beginning getHLkeys sub": indent()
	on error resume next ' handle errors in _this procedure only_
	debug "Creating Wscript.shell object"
	set WSHShell = CreateObject("WScript.Shell") 
	if err.number<>0 then
		err.clear
		on error goto 0
		debug "Could not create Wscript.Shell object. Wscript version is "&wscript.version&"."
		debug "Prompting user to continue or fail installation."
		dim MsgRet
		MsgRet = message("The installer was unable to create the ActiveX component required to"&vbcrlf&"'interact with your registry."&vbcrlf&vbcrlf&"This is a problem with your Windows Scripting Host installation and can be fixed"&vbcrlf&"by reinstalling/updating the Windows Scripting Host."&vbcrlf&"Your current Windows Scripting Host is version %wscriptversion%"&vbcrlf&"The Windows Scripting Host can be downloaded free from Microsoft at"&vbcrlf&"http://msdn.microsoft.com/scripting",vbokcancel,"Object creation error")
		if MsgRet =vbcancel then 
			debug "completed GetHLKys"&vbcrlf&"user canceled installation"
			DebugClose()
		else
			debug "chose to continue"
			unident()
			debug "completed GetHLkeys sub (unsucessfully)"&vbcrlf
		end if
		GetHLKeys=false
	else
		' get the keys, soooo many keys, blame valve
		debug "registry keys are"
		
		' HL Keys
		ValveHLKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\VALVE\HALF-LIFE\InstallPath")
		if ValveHLKey<>"" then if right(ValveHLKey,1)="\" then ValveHLKey=left(ValveHLKey,len(ValveHLKey)-1)
		debug vbtab&"ValveHLKey  = "&ValveHLKey
		
		SolHlKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Sierra OnLine\Setup\HALFLIFE\directory")
		if SolHlKey<>"" then if right(SolHlKey,1)="\" then SolHlKey=left(SolHlKey,len(SolHlKey)-1)
		debug vbtab&"SolHlKey    = "&SolHlKey
	
		SoftHLKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\Half-Life\InstallPath")
		if SoftHLKey<>"" then if right(SoftHLKey,1)="\" then SoftHLKey=left(SoftHLKey,len(SoftHLKey)-1)
		debug vbtab&"SoftHLKey   = "&SoftHLKey
		
		' CS Keys
		SolCSKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Sierra OnLine\Setup\CSTRIKE\directory")
		if SolCSKey<>"" then if right(SolCSKey,1)="\" then SolCSKey=left(SolCSKey,len(SolCSKey)-1)
		debug vbtab&"SolCSKey    = "&SolCSKey
		
		SoftCSKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\CounterStrike\InstallPath")
		if SoftCSKey<>"" then if right(SoftCSKey,1)="\" then SoftCSKey=left(SoftCSKey,len(SoftCSKey)-1)
		debug vbtab&"SoftCSKey   = "&SoftCSKey
		
		' DOD Keys
		SolDODKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\Software\Valve\DOD\InstallPath")
		if SolDODKey<>"" then if right(SolDODKey,1)="\" then SolDODKey=left(SolDODKey,len(SolDODKey)-1)
		debug vbtab&"SolDODKey   = "&SolDODKey
		
		SoftDODKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\DOD\InstallPath")
		if SoftDODKey<>"" then if right(SoftDODKey,1)="\" then SoftDODKey=left(SoftDODKey,len(SoftDODKey)-1)
		debug vbtab&"SoftDODKey  = "&SoftDODKey
		
		' HLDS Keys
		SolHLDSKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Sierra OnLine\Setup\HLSERVER\directory")
		if SolHLDSKey<>"" then if right(SolHLDSKey,1)="\" then SolHLDSKey=left(SolHLDSKey,len(SolHLDSKey)-1)
		debug vbtab&"SolHLDSKey  = "&SolHLDSKey
		
		SoftHLDSKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\HLServer\InstallPath")
		if SoftHLDSKey<>"" then if right(SoftHLDSKey,1)="\" then SoftHLDSKey=left(SoftHLDSKey,len(SoftHLDSKey)-1)
		debug vbtab&"SoftHLDSKey = "&SoftHLDSKey
		
		' STEAM keys 
		SoftStreamKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Valve\Steam\InstallPath")
		if SoftStreamKey<>"" then if right(SoftStreamKey,1)="\" then SoftStreamKey=left(SoftStreamKey,len(SoftStreamKey)-1)
		debug vbtab&"SoftStreamKey = "&SoftStreamKey
		
		Set WSHShell=Nothing: debug "keys read and WSH shell object destroyed"
		on error goto 0
		
		Paths.count=0

		if SoftHlKey<>"" then ' now determine which HL key to use, of three
			if fileexists(SoftHlKey&"\hl.exe") then	Paths.HLPath=SoftHlKey: Paths.count=Paths.count+1
		else
			if SolHLKey<>"" then 
				if fileexists(SolHLKey&"\hl.exe") then Paths.HLPath=SolHLKey: Paths.count=Paths.count+1
			else
				if ValveHLKey<>"" then
					if fileexists(ValveHLKey&"\hl.exe") then Paths.HLPath=ValveHLkey: Paths.count=Paths.count+1
				end if
			end if
		end if

		' search the Cstrike retail keys.
		if SoftHLKey<>"" then
			if fileexists(SoftHLKey&"\cstrike.exe") then Paths.CSpath=SoftHLKey: Paths.count=Paths.count+1
		else
			if SolCSKey<>"" then
				if fileexists(SolCSKey&"\cstrike.exe") then Paths.CSPath=SolCSKey: Paths.count=Paths.count+1
			end if
		end if
		'if Paths.HLpath<>"" then Paths.EXEName="CS"
		
		' search the DOD retail keys.
		if SoftDODKey<>"" then
			if fileexists(SoftDODKey&"\dod.exe") then Paths.DODpath=SoftDODKey: Paths.count=Paths.count+1
		else
			if SolDODKey<>"" then
				if fileexists(SolDODKey&"\dod.exe") then Paths.DODPath=SolDODKey: Paths.count=Paths.count+1
			end if
		end if
		'if Paths.HLpath<>"" then Paths.EXEName="DOD"

		if SoftHLDSKey<>"" then
			if fileexists(SoftHLDSKey&"\hlds.exe") then Paths.HLDSpath=SoftHLDSKey: Paths.count=Paths.count+1
		else
			if SolHLDSKey<>"" then
				if fileexists(SolHLDSKey&"\hlds.exe") then Paths.HLDSpath=SolHLDSKey: Paths.count=Paths.count+1
			end if
		end if

		debug "Paths are"
		debug vbtab & "Paths.HLPath   = " & Paths.HLPath 
		debug vbtab & "Paths.HLDSPath = " & Paths.HLDSPath 
		debug vbtab & "Paths.CSPath   = " & Paths.CSPath 
		debug vbtab & "Paths.DODPath  = " & Paths.DODPath
		debug vbtab & "Paths.STEAMPath= " & Paths.STEAMPath
		unindent():	debug "completed GetHLkeys sub"&vbCrlf
		GetHLKeys=true
	end if
end function
'----------------------------------------------------------------------------------------------------------------------------
private sub RenameAMBackupLiblist(byval FilePath)
	debug "beginning RenameAMBackupLIblist sub"
	dim File
	if fileexists(filepath) then
		debug vbtab&"attempting to rename the file"
		set file=fso.getfile(FilePath)
		if fileexists(left(filepath, instrrev(filepath,"\")) & "metamod backup of liblist.gam") then deletefile left(filepath, instrrev(filepath,"\")) & "metamod backup of liblist.gam",true
		file.name="metamod backup of liblist.gam"
		set file=nothing
		debug vbtab&"file renamed sucessfully"
	end if
	debug "completed RenameAMBackupLiblist sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
Private Sub GetCommandLineArgs
	dim Command
	debug "starting GetCommand sub"
	Set Command = WScript.Arguments
	if Command.count>0 then
		if Command(0)<>"" then
			debug vbtab&"first argument passed was """&Command(0)&""", checking for existance of the passed location"
			if fso.folderexists(Command(0))=true then
				HLpath=Command(0): silent=1: debug vbtab&"silent mode enabled. No prompts will be allowed"
			end if
		end if
		if Command.count>1 then
			if Command(1)<>"" then
				debug vbtab&"second argument passed was """&Command(1)&""", checking for existance of the passed location"
				if fso.folderexists(Command(1))=true then
					debug vbtab&"second argument will be used as the temporary config file backup area, """&Command(1)&""""
					OldConfigsPath=Command(1) 
				end if
			end if
		end if
	else
		debug vbtab&"no arguments passed"
	end if
	debug "completed GetCommand sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private sub Old_UninstallAdmin(Path,BackPath)
	debug "starting Old_UninstallAdmin sub"
	debug vbtab&"starting creating folders for backed up files"
	createfolder(BackPath & "\adminmod")
	createfolder(BackPath & "\adminmod\dlls")
	createfolder(BackPath & "\adminmod\scripts")
	createfolder(BackPath & "\adminmod\config")

	if OldConfigsPath<>"" then
		if fileexists(Path & "\adminmod.cfg") then copyfile Path & "\adminmod.cfg", OldConfigsPath & "\adminmod.cfg" 
		if fileexists(Path & "\users.ini") then copyfile Path & "\users.ini", OldConfigsPath & "\users.ini"
		if fileexists(Path & "\ips.ini") then copyfile Path & "\ips.ini", OldConfigsPath & "\ips.ini"
		if fileexists(Path & "\models.ini") then copyfile Path & "\models.ini", OldConfigsPath & "\models.ini"
		if fileexists(Path & "\maps.ini") then copyfile Path & "\maps.ini", OldConfigsPath & "\maps.ini"
		if fileexists(Path & "\wordlist.txt") then copyfile Path & "\wordlist.txt", OldConfigsPath & "\wordlist.txt"
	end if

	debug vbtab&"finished creating folders for backed up files"&vbcrlf&vbtab&"moving all Admin mod specific files to backup folders"
	movefile Path & "\dlls\*.amx", BackPath & "\adminmod\scripts\"
	if fileexists( Path & "\dlls\amx_admin.dll")= true then movefile Path & "\dlls\amx_admin.dll", BackPath & "\adminmod\dlls\"
	if fileexists( Path & "\dlls\admin_mm.dll") = true then movefile Path & "\dlls\admin_mm.dll",  BackPath & "\adminmod\dlls\"
	if fileexists( Path & "\server.cfg")        = true then 
		copyfile   Path & "\server.cfg",        BackPath & "\adminmod\config\", false
		RemoveExec Path & "\server.cfg"
	end if
	if fileexists( Path & "\listenserver.cfg")  = true then 
		copyfile   Path & "\listenserver.cfg",  BackPath & "\adminmod\config\", false
		RemoveExec Path & "\listenserver.cfg"
	end if
	if fileexists( Path & "\admin_help.cfg")    = true then movefile Path & "\admin_help.cfg",     BackPath & "\adminmod\config\"
	if fileexists( Path & "\adminmod.cfg")      = true then movefile Path & "\adminmod.cfg",       BackPath & "\adminmod\config\"
	if fileexists( Path & "\plugin.ini")        = true then movefile Path & "\plugin.ini",         BackPath & "\adminmod\config\"
	if fileexists( Path & "\users.ini")         = true then movefile Path & "\users.ini",          BackPath & "\adminmod\config\"
	if fileexists( Path & "\ips.ini")           = true then movefile Path & "\ips.ini",            BackPath & "\adminmod\config\"
	if fileexists( Path & "\models.ini")        = true then movefile Path & "\models.ini",         BackPath & "\adminmod\config\"
	if fileexists( Path & "\maps.ini")          = true then movefile Path & "\maps.ini",           BackPath & "\adminmod\config\"
	if fileexists( Path & "\wordlist.txt")      = true then movefile Path & "\wordlist.txt",       BackPath & "\adminmod\config\"
	if fileexists( Path & "\liblist.gam")       = true then movefile Path & "\liblist.gam",        BackPath & "\adminmod\config\"
	if fileexists( Path & "\AdminMod backup of liblist.gam")=true then movefile Path&"\AdminMod backup of liblist.gam" ,Path&"\liblist.gam"
	debug vbtab&"finished moving Admin mod specific files to backup folders"
	debug "finished Old_UninstallAdmin sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private sub New_UninstallAdmin(Path,BackPath)
	debug "starting New_UninstallAdmin sub"

	debug vbtab&"starting creating folders for backed up files"
	createfolder(BackPath&"\adminmod")
	createfolder(BackPath&"\adminmod\dlls")
	createfolder(BackPath&"\adminmod\scripts")
	createfolder(BackPath&"\adminmod\config")
	debug vbtab&"finished creating folders for backed up files"

	debug vbtab&"moving all Admin mod specific files to backup folders"
	copyfile Path & AM_SCRIPTS & "\*.amx", BackPath & "\adminmod\scripts\",true
	if fileexists( Path & AM_DLLS   & "\amx_admin.dll")    = true then copyfile Path & AM_DLLS & "\amx_admin.dll", BackPath & "\adminmod\dlls\" ,false
	if fileexists( Path & AM_DLLS   & "\admin_mm.dll")     = true then copyfile Path & AM_DLLS & "\admin_mm.dll",  BackPath & "\adminmod\dlls\",false
	if fileexists( Path & "\server.cfg")       = true then
		copyfile   Path & "\server.cfg",       BackPath & "\adminmod\config\", false
		RemoveExec Path & "\server.cfg"
	end if
	if fileexists( Path & "\listenserver.cfg") = true then
		copyfile   Path & "\listenserver.cfg", BackPath & "\adminmod\config\", false
		RemoveExec Path & "\listenserver.cfg"
	end if
	if fileexists( Path & AM_CONFIG & "\adinmod.cfg")       then copyfile Path & AM_CONFIG & "\adminmod.cfg",BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\plugin.ini")        then copyfile Path & AM_CONFIG & "\plugin.ini",  BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\users.ini")         then copyfile Path & AM_CONFIG & "\users.ini",   BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\ips.ini")           then copyfile Path & AM_CONFIG & "\ips.ini",     BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\models.ini")        then copyfile Path & AM_CONFIG & "\models.ini",  BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\maps.ini")          then copyfile Path & AM_CONFIG & "\maps.ini",    BackPath & "\adminmod\config\",false
	if fileexists( Path & AM_CONFIG & "\wordlist.txt")      then copyfile Path & AM_CONFIG & "\wordlist.txt",BackPath & "\adminmod\config\",false
	debug vbtab&"finished moving Admin mod specific files to backup folders"

	if OldConfigsPath<>"" then
		if fileexists( Path & AM_CONFIG & "\adminmod.cfg") then copyfile Path & AM_CONFIG & "\adminmod.cfg", OldConfigsPath & "\",true 
		if fileexists( Path & AM_CONFIG & "\plugin.ini")   then copyfile Path & AM_CONFIG & "\users.ini", OldConfigsPath & "\",true
		if fileexists( Path & AM_CONFIG & "\users.ini")    then copyfile Path & AM_CONFIG & "\ips.ini", OldConfigsPath & "\",true
		if fileexists( Path & AM_CONFIG & "\ips.ini")      then copyfile Path & AM_CONFIG & "\models.ini", OldConfigsPath & "\",true
		if fileexists( Path & AM_CONFIG & "\maps.ini")     then copyfile Path & AM_CONFIG & "\maps.ini", OldConfigsPath & "\",true
		if fileexists( Path & AM_CONFIG & "\wordlist.txt") then copyfile Path & AM_CONFIG & "\wordlist.txt", OldConfigsPath & "\",true
	end if

	debug vbtab&"deleting """&AM_ROOT&""" folder and subfolders"
	deletefolder Path & AM_DLLS,   true
	deletefolder Path & AM_CONFIG, true
	deletefolder Path & AM_SCRIPTS,true
	deletefolder Path & AM_ROOT   ,true

	debug "finished New_UninstallAdmin sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private sub Old_UninstallMetamod(Path,BackPath)
	debug "starting Old_UninstallMetamod sub"
	debug vbtab&"starting creating folders for backed up files"
	createfolder(BackPath & "\metamod")
	createfolder(BackPath & "\metamod\dlls")
	debug vbtab&"finished creating folders for backed up files"&vbcrlf&vbtab&"moving all metamod specific files to backup folders"

	if fileexists( Path & "\dlls\metamod.dll") = true then
		copyfile   Path & "\dlls\metamod.dll", BackPath & "\metamod\dlls\",true
		deletefile Path & "\dlls\metamod.dll",true
	end if	
	if fileexists( Path & "\metaexec.cfg") = true then
		copyfile   Path & "\metaexec.cfg",BackPath & "\metamod\", true
		deletefile Path & "\metaexec.cfg",true
	end if
	if fileexists( Path & "\metamod.ini") = true then
		copyfile   Path & "\metamod.ini",BackPath & "\metamod\", true
		deletefile Path & "\metamod.ini",true
	end if

	debug vbtab&"finished moving metamod specific files to backup folders"
	debug "finished Old_UninstallMetamod sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private sub New_UninstallMetamod(Path,BackPath)
	debug "starting New_UninstallMetamod sub"

	debug vbtab&"starting creating folders for backed up files"
	createfolder(BackPath & "\metamod")
	createfolder(BackPath & "\metamod\dlls")

	debug vbtab&"finished creating folders for backed up files"
	debug vbtab&"moving all metamod specific files to backup folders"
	if fileexists(Path & MM_DLLS & "\metamod.dll")  = true then movefile Path & MM_DLLS & "\metamod.dll", BackPath & "\metamod\dlls\"
	if fileexists(Path & MM_ROOT & "\plugins.ini")  = true then copyfile Path & MM_ROOT & "\plugins.ini", BackPath & "\metamod\", false
	debug vbtab&"finished moving metamod specific files to backup folders"

	debug vbtab&"deleting """ & MM_ROOT & """ folder"
	deletefolder Path & MM_ROOT, true

	debug "finished New_UninstallMetamod sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private sub RestoreBackUpLiblist(Path,BackPath)
	debug "beginning RestoreBackUPLiblist sub"
	dim LiblistPath

	if fileexists(Path & "\AdminMod backup of liblist.gam") then
		LiblistPath = Path & "\AdminMod backup of liblist.gam"
	end if

	if LiblistPath<>"" then
		copyfile LiblistPath, BackPath & "\adminmod\", true
		deletefile path & "\liblist.gam", true
		movefile LiblistPath, Path & "\liblist.gam"
		debug vbtab&"backup liblist.gam found, copied and restored"
	else
		debug vbtab&"backup liblist.gam file could not be located and thus could not be restored."
	end if

	debug "completed RestoreBackUPLiblist sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------------------------------------------
private function New_OtherMMDllsPresent(CheckPath)
	New_OtherMMDllsPresent=false
	debug "beginning New_OtherMMDllsPresent function"
	debug vbtab&"root path is """&CheckPath&""""
	dim localFolder,LocalFile,localsubfolder,subsubfolder, subsublocalfolder,subfile
	if folderexists(checkpath) and folderexists(checkpath&"\addons") then set localfolder= fso.getfolder(checkpath&"\addons")
	for each localsubfolder in localfolder.subfolders
		debug vbtab&"checking subfolder """&localsubfolder.name&""""
		for each subsubfolder in localsubfolder.subfolders
			if subsubfolder.name ="dlls" then
				set subsublocalfolder= fso.getfolder(subsubfolder.path)
				for each subfile in subsublocalfolder.files
					if len(subfile.name)>4 then
						if lcase(right(subfile.name,4))=".dll" and lcase(subfile.name)<>"metamod.dll" and lcase(subfile.name)<>"admin_amx.dll" and lcase(subfile.name)<>"admin_mm.dll" then
							debug vbtab&"found """&subfile.path&""" dll file"
							New_OtherMMDllsPresent=true
							' this should't be here really, but there is no multilevel break and no goto's.
							debug vbtab&"other addon dll located, metamod will not be removed"
							debug "completed New_OtherMMDllsPresent function"&vbcrlf
							exit function
						end if
					end if
				next
			end if
		next
	next
	debug vbtab&"no other addons dll located. metamod will be uninstalled"
	debug "completed New_OtherMMDllsPresent function"&vbcrlf
end function
'----------------------------------------------------------------------------------------------------------------------------
private sub RemoveAddons(Path)
	debug "beginning RemoveAddons sub"
	dim folder, subfolder, subfile, count
	count=0
	if folderexists(Path) then set Folder =fso.getfolder(path)
	debug vbtab&"enumerating subfolders"
	for each subfolder in folder.subfolders
		count=count+1
	next

	debug vbtab&"no subfolders found, enumerating files"
	if count=0 then
		for each subfile in folder.files
			count=count+1
		next
	else 
		debug vbtab&"file found, \addons will not be deleted"
	end if

	if count=0 then
		debug vbtab&"no files or folder were found, deleting """&path&""""
		deletefolder path,true
	end if
	debug "completed RemoveAddons sub"
end sub	
'------------------------------------------------------------------------------------------------------------------------
private sub RemoveExec(CfgPath)
	dim file,invar,RO,filetext,infile
	debug vbcrlf&vbtab&"starting RemoveExec sub"&vbcrlf&vbtab&vbtab&"path passed: """ & CfgPath &""""
	set file=fso.getfile(CfgPath)
	If (file.attributes And 1)=1 Then
		debug vbtab&vbtab&"found file set as readonly, changed for editing"
		file.attributes= (file.attributes Xor 1)
		RO=True
	End If

	set infile=fso.opentextfile(CfgPath,forreading,false)

	debug vbtab&vbtab&"starting replacements of Admin mod specific strings"

	do while infile.atendofstream<>true
		invar=infile.readline
		if instr(1,"// Execute the Admin Mod configuration file",invar) >0 then	invar=""
		if len(invar)<>6 then
			if left(invar,2)<>"//" then
				if (instr(1,invar,"addons/adminmod")>0 or instr(1,invar,"adminmod.cfg") > 0) then
					invar=""
				end if
			end if
		end if
		filetext=filetext & vbcrlf &  invar
	loop
	infile.close
	
	if NEUTER=0 then
		set infile=fso.opentextfile(CfgPath,forwriting,true)
		infile.write(trim(filetext))
		infile.close: set infile=nothing
	end if

	debug vbtab&vbtab&"file rewritten"
	If RO=True Then
		debug vbtab&vbtab&"file being set to readonly status again"
		file.attributes=(file.attributes Or 1)
		Set File=Nothing
		RO=False
	End If
	debug vbtab&"completed RemoveExec sub"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
private sub RemovePluginsEntry(pluginsFilePath)
	dim file,invar,RO,filetext,infile
	debug vbcrlf&"starting RemovePluginsEntry sub"&vbcrlf&vbtab&"path passed: """ & pluginsFilePath &""""
	set file=fso.getfile(pluginsFilePath)
	If (file.attributes And 1)=1 Then
		debug vbtab&"found file set as readonly, changed for editing"
		file.attributes= (file.attributes Xor 1)
		RO=True
	End If

	set infile=fso.opentextfile(pluginsFilePath,forreading,false)

	debug vbtab&"starting replacements of Admin mod specific strings"

	do while infile.atendofstream<>true
		invar=infile.readline
		if (instr(1,lcase(invar),"admin_mm.dll")>0 ) then
			debug vbtab&"[DEBUG] found admin_mm.dll"
			invar=""
		end if
		filetext=filetext & vbcrlf &  invar
	loop
	infile.close
	
	if NEUTER=0 then
		set infile=fso.opentextfile(pluginsFilePath,forwriting,true)
		infile.write(trim(filetext))
		infile.close: set infile=nothing
	end if

	debug vbtab&"file rewritten"
	If RO=True Then
		debug vbtab&"file being set to readonly status again"
		file.attributes=(file.attributes Or 1)
		Set File=Nothing
		RO=False
	End If
	debug "completed RemovePluginsEntry sub"&vbcrlf
end sub
















































'----------------------------------------------------------------------------------------------InputBox-Abstraction-----
private function Input(Prompt,Title)
	if InConsoleMode=true then
		dim PromptArr, x
		
		Prompt=replace(Prompt,"click OK","Enter o")
		Prompt=replace(Prompt,"click Cancel","Enter c")
		Prompt=replace(Prompt,"click Yes","Enter y")
		Prompt=replace(Prompt,"click No","Enter n")
		Prompt=replace(Prompt,"click OK","Enter o")
		Prompt=replace(Prompt,"click Cancel","Enter c")
		Prompt=replace(Prompt,"click Yes","Enter y")
		Prompt=replace(Prompt,"click No","Enter n")
		Prompt=replace(Prompt,"click OK","Press Enter to continue")
		Prompt=replace(Prompt,"click OK","Press Enter to continue")
		Prompt=replace(Prompt,"choose OK","Enter c to cancel, Enter o")
		Prompt=replace(Prompt,"choose OK","Enter c to cancel, Enter o")
		Prompt=replace(Prompt,vbcrlf&vbcrlf,vbcrlf&" "&vbcrlf)
		
		promptarr=split(prompt,vbcrlf)
		wscript.echo "--------------------------------------------------------------------------------"&vbcrlf
		for x=0 to ubound(promptarr)
			wscript.echo promptarr(x)
		next
		wscript.echo vbcrlf&"--------------------------------------------------------------------------------"
		input=wscript.stdin.readline
	else
		input=Inputbox(prompt,title)
	end if
end function
'----------------------------------------------------------------------------------------------Msgbox-Abstraction--------
private function Message(MessageText,Buttons,Title)
	if InConsoleMode=true then
		dim MessageArr,x,ConsoleInput
		
		MessageText=replace(MessageText,"click OK","Enter o")
		MessageText=replace(MessageText,"click Cancel","Enter c")
		MessageText=replace(MessageText,"click Yes","Enter y")
		MessageText=replace(MessageText,"click No","Enter n")
		MessageText=replace(MessageText,"Click OK","Enter o")
		MessageText=replace(MessageText,"Click Cancel","Enter c")
		MessageText=replace(MessageText,"Click Yes","Enter y")
		MessageText=replace(MessageText,"Click No","Enter n")
		MessageText=replace(MessageText,"click OK","Press Enter to continue")
		MessageText=replace(MessageText,"click OK","Press Enter to continue")
		MessageText=replace(MessageText,"choose OK","Enter c to cancel, Enter o")
		MessageText=replace(MessageText,"choose OK","Enter c to cancel, Enter o")
		MessageText=replace(MessageText,"Choose OK","Enter c to cancel, Enter o")
		MessageText=replace(MessageText,"Choose OK","Enter c to cancel, Enter o")
		
		wscript.echo "--------------------------------------------------------------------------------"
		wscript.echo vbcrlf&messagetext&vbcrlf
		if err.number<>0 then wscript.echo(vbcrlf&"press enter to quit"&vbcrlf)
		wscript.echo "--------------------------------------------------------------------------------"
		
		ConsoleInput=wscript.stdin.readline
			
		select case buttons
			case vbokonly
				Message=vbok
			case vbokcancel
				select case ucase(left(consoleinput,1))
					case "O","": message=vbok
					case "C": message=vbcancel
					case else: message=vbcancel
				end select
			case vbyesnocancel
				select case  ucase(left(consoleinput,1))
					case "C": message=vbcancel
					case "Y": message=vbyes
					case "N": message=vbno
					case else: message=vbcancel
				end select
			case else
				select case ucase(left(consoleinput,1))
					case "C": message=vbcancel
					case "Y": message=vbyes
					case "N": message=vbno
					case "O": message=vbok
					case else: message=vbok
				end select
		end select
	else
		message=msgbox(MessageText,Buttons,Title)
	end if
end function
'----------------------------------------------------------------------------------------------Wscript/Cscript-determination
private function IsHostCscript()
    on error resume next
    dim FullName,Command,i,j    
    IsHostCscript = false: FullName = WScript.FullName    
    i = InStr(1, FullName, ".exe", 1)
    if i <> 0 then
    	j = InStrRev(FullName, "\", i, 1)
    	if j <> 0 then
        	Command = Mid(FullName, j+1, i-j-1)
        	if LCase(Command) = "cscript" then IsHostCscript = true
    	end if
	end if
    if Err <> 0 then err.clear
    on error goto 0
end function
'------------------------------------------------------------------------------------------------------------------------
private function GetString(StringName)
' N.B: Translators please note that a messagebox can only handle up to 1024 characters, any more than that will not be
'displayed. When translating the longs strings make sure that they will all be printed. Changing DEBUGME to 2 or greater
'will make this function log it's retrieved lengths to the log file, this is specifically for your use. I hope it helps
	if DEBUGME>1 then debug "[DEBUG-2] retrieving string """&StringName&""" from script file."
	dim lTSO,InVar,RetVar,ReadOn
	set lTSO=fso.opentextfile(WScript.ScriptFullName,1,false)
	do while lTSO.atendofstream<>true
		invar=lTSO.readline
		if ReadOn=false then
			if instr(1,invar,chr(60)&"STRING-MARKER-"&StringName&chr(62)) >0 then ReadOn=true
		else
			if instr(1,invar,chr(60)&"/STRING-MARKER-"&StringName&chr(62)) >0 then 
				ReadOn=false: exit do
			else
				if len(Invar)>0 then Retvar=RetVar&vbcrlf&right(Invar,len(Invar)-1)
			end if
		end if
	loop
	
	if lTSO.atendofstream=true and ReadOn=true then
		if DEBUGME>1 then debug "[DEBUG-2] string """&StringName&""" was not closed"
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",16,"Internal script error"
		DebugClose(): wscript.quit
	end if
	lTSO.close:	set lTSO=nothing
	
	Retvar =trim(replace(RetVar,"%version%",Version))
	Retvar =replace(RetVar,"%wscriptversion%",wscript.version)
	
	if len(RetVar) >0 then
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" retrieved, "&len(RetVar)&" chrarcters long"&vbcrlf
		GetString = RetVar
	else
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" could not be retrieved, the script will exit"&vbcrlf
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",16,"Internal script error"
		DebugClose(): wscript.quit
	end if
end function

'------------------------------------------------------------------------------------------------------------------------
public sub indent
	IndentValue=IndentValue+1
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub unindent
	if IndentValue >= 1 then IndentValue=IndentValue-1
end sub
'------------------------------------------------------------------------------------------------------------------------


'------------------------------------------------------------------------------------------------------------------------
'
'  	FilesystemObject abstractions, these allow prevention of function, used in debugging.
'
'------------------------------------------------------------------------------------------------------------------------
'----------------------------------------------------------------------------------------FSO.Fileexists-Abstraction------
private function FileExists(FilePath)
	if DEBUGME>0 Then
		debug "checking file   """ &FilePath&""", #"
		if fso.fileexists(FilePath) =true Then
			debug "found file."
			FileExists=True
			exit function
		else
			debug "file missing."
			FileExists=False
			exit Function
		end If
	else
		FileExists = fso.fileexists(FilePath)
	end If
end function
'----------------------------------------------------------------------------------------FSO.FolderExists-Abstraction----
private function FolderExists(FolderPath)
	If DEBUGME>0 Then
		debug "checking folder """  & FolderPath & """ , #"
		if fso.Folderexists(FolderPath) =true Then
			debug "found folder."
			FolderExists=true
			exit function
		else
			debug "folder missing."
			FolderExists=false
			exit function
		end If
	else
		FolderExists= fso.folderexists(FolderPath)
	end if
end function
'----------------------------------------------------------------------------------------FSO.Createftextile-Abstraction--
private function CreateTextFile(lPath,OverWrite)
	debug "creating file   """ & lPath & """, forcing overwrite=" & OverWrite
	if NEUTER=0 then
		on error resume next 
		set CreateTextFile = fso.createtextfile(lpath,overwrite)
		if err.number<>0 then AbstractonError("CreateTextFile() function")
		on error goto 0
	end if
end function
'----------------------------------------------------------------------------------------FSO.OpenTextfile-Abstraction----
'private function OpenTextFile(filename,IOMode,OverWrite)
'	debug vbtab & "opening file    """ & filename & """, mode="&IOMode&", forcing overwrite=" & OverWrite
'	if NEUTER=0 then
'		on error resume next 
'		set sheep = fso.opentextfile(filename,IOMode,OverWrite)
'		if err.number<>0 then AbstractonError("OpenTextFile() function")
'		on error goto 0
'	end if
'end function
'----------------------------------------------------------------------------------------FSO.Createfolder-Abstraction----

' neither of these functions can be used as functions in VBS the use of the set keyword precludes the use of a function
' call as the rvalue. This prevents the returning of the textstream objects created and forces the use of inline
' filesystem access. This is undesireable but it'll take a better programmer than me to work out a solution to this.
' createtextfile is called with no use of the return value and thus is made available

private sub CreateFolder(lPath)
	debug "creating folder """ & lPath & """"
	if NEUTER=0 then
		on error resume next
		fso.createfolder(lPath)
		if err.number<>0 then AbstractonError("CreateFolder() subroutine")
		on error goto 0
	end if
end sub
'----------------------------------------------------------------------------------------FSO.Movefolder-Abstraction------
private sub MoveFolder(Source,Destination)
	debug "moving   folder """ & Source & """ to """ & Destination & """"
	if NEUTER=0 then
		on error resume next
		fso.movefolder Source,Destination
		if err.number<>0 then AbstractonError("MoveFolder() subroutine")
		on error goto 0
	end if
end sub
'----------------------------------------------------------------------------------------FSO.MoveFile-Abstraction--------
private sub MoveFile(Source,Destination)
	debug "moving   file   """ & Source & """ to """ & Destination & """"
	if NEUTER=0 then
		on error resume next
		fso.movefile Source,Destination
		if err.number<>0 then AbstractonError("MoveFile() subroutine")
		on error goto 0
	end if
end sub
'----------------------------------------------------------------------------------------FSO.Copyfile-Abstraction--------
private sub Copyfile(FromPath,ToPath,OverWrite)
	debug "copying  file   """ & FromPath & """ to: """ & ToPath & """, forcing overwrite=" & OverWrite
	if NEUTER=0 then
		on error resume next 
		fso.copyfile FromPath,ToPath,OverWrite
		if err.number<>0 then AbstractonError("Copyfile() subroutine")
		on error goto 0
	end if
	if InCVSMode then
		dim file
		set file=fso.getfile(ToPath)
		if (file.attributes and FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY then
			debug ToPath&" Is a read-only file, changing"
			file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
		end if
		set file=nothing
	end if
end sub
'----------------------------------------------------------------------------------------FSO.CopyFolder-Abstraction------
private sub CopyFolder(FromPath,ToPath,OverWrite)
	debug "copying  folder """ & FromPath & """ to: """ & ToPath & """, forcing overwrite=" & OverWrite
	if NEUTER=0 then
		on error resume next  
		fso.copyfolder FromPath,ToPath,OverWrite
		if err.number<>0 then AbstractonError("CopyFolder() subroutine")
		on error goto 0
	end if
end Sub
'----------------------------------------------------------------------------------------FSO.Deletefile-Abstraction------
private sub DeleteFile(FilePath,Force)
	debug "deleting file   """ & FilePath & """, forcing delete=" & Force
	if NEUTER=0 Then
		on error resume next  
		fso.deletefile FilePath,Force
		if err.number<>0 then AbstractonError("DeleteFile() subroutine")
		on error goto 0
	end if
end sub
'----------------------------------------------------------------------------------------FSO.Deletefile-Abstraction------
private sub DeleteFolder(FilePath,Force)
	debug "deleting folder """ & FilePath & """, forcing delete=" & Force
	If NEUTER=0 Then
		on error resume next 
		fso.deletefolder FilePath,Force
		if err.number<>0 then AbstractonError("DeleteFolder() subroutine")
		on error goto 0
	end if
end sub
'------------------------------------------------------------------------------------------------------------------------
private sub AbstractionError(Error_Proc)
	dim ErrorString
	ErrorString=GetString("ABSTRACTIONERROR")
	ErrorString=replace(ErrorString,"%ERROR_PROC%",Error_Proc)
	ErrorString=replace(ErrorString,"%ERROR_DETAILS%","error number: "&cstr(err.number)&vbcrlf&"error description: "&err.description)
	debug vbcrlf&"error in "&ErrorProc&vbcrlf&"error number: "&cstr(err.number)&vbcrlf&"error description: "&err.description
	err.clear
	message Errorstring
	debugClose()
	wscript.quit 1
end sub
'------------------------------------------------------------------------------------------------------------------------
'
'  	Debugging procedures used to init,close and write to the debugging log.
'
'------------------------------------------------------------------------------------------------------------------------
'----------------------------------------------------------------------------------------debug-initialisation------------
private sub DebugStart()
	if DEBUGME>0 Then
		set FSD = CreateObject("Scripting.FileSystemObject")
		on error resume next
		set TSD = FSD.createtextfile("uninstall_admin.log",True)
		on error goto 0
		if err.number<>0 then
			Message "The uninstaller has debug mode enabled. During initialisation the uinstaller was unable to create the ""uninstall_admin.log"" file in the uninstaller directory. This problem prevents the installer from running."&vbcrlf&"Please run the uninstaller again when it is possible to write to this file."&vbcrlf&vbcrlf&"This uninstaller will now exit",vbokonly or  vbExclamation,"Fatal error creating debug log"
			wscript.quit
		end if
		debug "debug file ""uninstall_admin.log"" created sucessfully"&vbCrlf&"debug Filesystem object created sucessfully"&vbcrlf
		if DEBUGME>1 then debug "options list":debug "DEBUGME = " & DEBUGME:debug "NEUTER = "&NEUTER:debug "NOUNINSTALL = " & NOUNINSTALL:debug "NOSHORTCUT = "&NOSHORTCUT:debug "end of options"&vbcrlf
	end if
end sub
'----------------------------------------------------------------------------------------debug-closure-------------------
private sub DebugClose()
	if DEBUGME>0 Then
		debug vbcrlf&"debug filesystem objects will be destroyed and the script will end."
		TSD.close
		set FSD=Nothing
		set TSD=Nothing
	end if
	'wscript.quit
end sub
'----------------------------------------------------------------------------------------debug-writing-------------------
private sub Debug(DebugString)
	if DEBUGME>0 then 
		dim x
		
		if IndentValue>0 then
			for x = 1 to IndentValue
				DebugString=vbtab&DebugString
			next
			if left(DebugString,3) = vbtab&vbcrlf then DebugString=vbcrlf&vbtab&right(debugstring,len(debugstring)-3)
		end if
		
		if right(DebugString,1)="#" then 
			debugstring=left(DebugString,len(DebugString)-1)
			TSD.write DebugString
		elseif right(DebugString,1)=">" then
			debugstring=left(DebugString,len(DebugString)-1)
			TSD.write vbtab&DebugString
		else
			TSD.write DebugString&vbCrlf
		end if
	end If
end sub
