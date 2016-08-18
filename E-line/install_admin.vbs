'---------------------------------------------------------------------------------------------------------------------------'
'															    '
' 			  Install script for Admin Mod  v2.50.56							    '
'			  Copyright (C) 2000 Alfred Reynolds (alfred@mazuma.net.au) 				    '
'			  Copyright (C) 2001-2003 David Mart 				    '
'			  $Id: install_admin.vbs,v 1.4 2003/11/08 10:21:19 darope Exp $			    '
' 															    '
'----------------------------------------------------------------------------------------------Declarations-and-setup-------'
Option Explicit ' enforces explicit variable declaration, don't touch this.
'---------------------------------------------------------------------------------------------------------------------------'
'  To enable debugging set the line below to "Const DEBUGME=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const DEBUGME = 0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable filesystem changes set the line below to "Const NEUTER=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const NEUTER = 0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable automatic search and execution of the uninstaller set the line below to "NOUNINSTALL=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const NOUNINSTALL=0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable desktop shortcut to the manual creation set the line below to "NOSHORTCUT=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
Const NOSHORTCUT=0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable prompting and retention of old config files set the line below to "NOKEEPCONFIGS=1" , boolean value: 1 = On, 0 = Off.
'----------------------------------------------------------------------------------------------------------------------------'
Const NOKEEPCONFIGS=0

'---------------------------------------------------------------------------------------------------------------------------'
'  Sets the wscript version used in version dependant switches, wscript.version is normal but setting it will force a single set of methods.
'---------------------------------------------------------------------------------------------------------------------------'
dim WSCRIPT_VERSION:WSCRIPT_VERSION=cstr(wscript.version)

'==========================================================================================================================='
'                                YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE !!
'==========================================================================================================================='

'----------------------------------------------------------------------------------------Script-Declarations----------------'
dim IndentValue,InConsoleMode,InCVSMode,WSHShell,FSO,TSO,FSD,TSD,version,Strings,Path,mod_name,OldConfigsPath,files_t,files_f,folders_f,folders_t,Steam
const ForReading=1,ForWriting=2,ForAppending=8,FILE_ATTRIBUTE_READONLY=1
class Install: Public Path:public EXEname:public SteamName:end class
class InstallPaths: public DodPath:public CSPath:public HLPath:public HLDSPath:public STEAMPath: public Count: end class
class FilePaths_From: public mm_dll:public mm_ini:public am_amxdll:public am_amdll:public am_amcfg:public am_amxbase: public am_docs: public am_plgini:private sub class_initialize: if not InCVSMode then:	mm_ini="\plugins.ini":mm_dll = "dlls\metamod.dll": am_amxdll = "dlls\amx_admin.dll": am_amdll = "dlls\admin_MM.dll": am_amcfg = "config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="docs\AdminMod-Win-Manual.chm": am_plgini="config\plugin.ini": else:mm_ini="\plugins.ini":mm_dll="..\metamod\dlls\metamod.dll":am_amxdll = "dlls\amxdll\release\amx_admin.dll":am_amdll="dlls\adminmod\releaseMM\admin_MM.dll":am_amcfg="config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="":am_plgini="config\plugin.ini":end if:end sub:end class
class FilePaths_To: end class
class FolderPaths_From:public am_config:public am_scripts:public am_docs:public am_dlls:public am_samples:public am_tools:private sub class_initialize:am_config = "config":am_docs="docs":am_dlls="dlls":am_scripts="scripting\binaries":am_samples="config\samples":am_tools="tools":end sub:end class
class FolderPaths_To: public steam:public am_config:public am_dlls:public am_scripts:public am_root:public mm_dlls: public mm_root:public addons: private sub class_initialize:am_config ="\addons\adminmod\config":am_dlls="\addons\adminmod\dlls":am_scripts="\addons\adminmod\scripts":am_root="\addons\adminmod":mm_dlls="\addons\metamod\dlls":mm_root="\addons\metamod":steam ="":addons="\addons":end sub:end class
class SteamInstall: Public Install: Public BasePath: Public AppsPath: Public AccntPath: Public NumAccnts: end class
'----------------------------------------------------------------------------------------Script-Initialisation-----------
version= "2.xx.yy"
'----------------------------------------------------------------------------------------Script-Starts-here--------------
Init()
Checkfiles() 	' check to make sure all required Admin Mod files are present before beginning the installation
FindTargets()	' find halflife, dod, cs, steam installations
FindMods()		' find all mods that can be installed to
CheckUninstall()' check if Admin Mod is already present and if so uninstall it.
VerifyHost()	' verify an installation will work (exceptional fools excluded)
ModifyLiblistFile(InstallMetamod())' modify the liblist.gam if metamod was installed
InstallAdminmod()' actually install the adminmod files
ManualShortcut()
close()
'----------------------------------------------------------------------------------------End-of-Script-------------------
public sub Init
	on error resume next
	Set fso = CreateObject("Scripting.FileSystemObject")
	if err.number<>0 then message "The installer was unable to create the ActiveX componant required to"&vbcrlf&"interact with your computers filesystem."&vbcrlf&"This is a problem with your windows scripting host installtion and can be fixed"&vbcrlf&"by reinstalling/updating the Windows Scripting Host."&vbcrlf&vbcrlf&"The windows scripting host can be downloaded free from Microsoft at"&vbcrlf&"http://msdn.microsoft.com/scripting",vbokonly or vbExclamation,"Object creation error":err.clear:wscript.quit
	on error goto 0
	DebugStart() ' self explanatory really
	debug "beginning Init function": indent()
	InCVSMode    =IsLocalCVSDir() ' determines if this is a CVS install
	InConsoleMode=IsHostCscript() ' determines console or gui mode
	if InConsoleMode then debug "script is running in console mode" else debug "script is running in GUI mode"
	if InCVSMode     then debug "script is running in CVS location mode" else debug "script is running in distribution mode"
	set files_f = new FilePaths_From
	set files_t = new FilePaths_To
	set folders_f=new FolderPaths_From
	set folders_t=new FolderPaths_To
	set Steam = new SteamInstall
	Steam.Install = false
	'message GetString("BETA"),64,"Admin Mod Beta Installer"
	if InCVSMode then message GetString("CVS-MESSAGE"),64,"Admin Mod CVS Installation"
	Strings=array(GetString("WELCOME"),GetString("REGISTRY"),GetString("REGISTRYERROR"),GetString("SHELLERROR"),GetString("KEEPCONFIGS"),GetString("SHORTEND"),GetString("NOSHORTEND"),GetString("MISSINGFILE"),GetString("NOUNINSTALL"),getString("CONFIGWARNING")) 'sets up an array of long strings read from the very end of this file.
	unindent():	debug "completed Init function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub Close
	debug "beginning Close function": indent()
	debug "installation complete, objects will be destroyed and the script will exit."
	unindent():	debug "completed Close function"&vbcrlf
	Set fso=Nothing: DebugClose()
end sub
'------------------------------------------------------------------------------------------------------------------------	
public sub FindTargets()
	debug "beginning FindTargets function": indent()
	dim Paths, answer
	answer = Message(Strings(0),33,"Starting Installation")
	If answer<> vbOk Then debug "user canceled installation": DebugClose()
	'answer = Message(Strings(1),vbOKCancel,"Allow Admin Mod Installer to read a registry key?")
	set Paths = new InstallPaths
	'if answer = vbOk then if not GetHLKeys(Paths) then Search(Paths)
	if not GetHLKeys(Paths) then Search(Paths)
	if Paths.count>0 then Path = Selection(Paths) &"\" else Path = specify() & "\"
    if Steam.Install then Path = GetSteamAccFolder(Paths) &"\"' Get the account info for a Steam installation to find the right folder
	unindent():	debug "completed FindTargets function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub FindMods()
	debug "beginning FindMods function": indent()
	dim mods,folders
	mods = GetMods()  'find the mods and list them for the user to choose
	debug "prompting user for mod choice"
	mod_name = Input("Enter the directory name (e.g. cstrike) of the mod you wish to install Admin Mod to."&vbcrlf&"Valid folders are listed below."&vbCrlf&vbCrlf&mods&vbCrlf&"Click OK when the folder has been entered.","Modification choice")
	If mod_name="" then
		debug "blank input is interpreted as cancel: quitting": DebugClose()
	elseif folderexists(path & mod_name)=false Then
		debug "user input """&mod_name&""" which is not a valid mod folder, second and final prompt"
		mod_name = Input("Enter the directory name of the Mod you wish To install Admin Mod To."&vbcrlf&"Valid folders are listed below."&vbCrlf&vbCrlf&mods&vbCrlf&"Click OK when the folder has been entered.","Modification choice")
		If mod_name="" then
			debug "blank input is interpreted as cancel: quitting": DebugClose()
		elseif folderexists(path & mod_name)=false Then
			debug "second erroneous folder: quiting":message "Either you cant type or you dont know where your halflife/server is installed to. Please try again when you know."&vbcrlf&vbcrlf&"This script will now exit",vbokonly,"Exiting": DebugClose()
		else
			debug "valid mod folder supplied """&mod_name&""" installation can proceed"
		end if
	else
		debug "valid mod folder supplied """&mod_name&""" installation can proceed"
	end if
	unindent(): debug "completed FindMods function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
private sub VerifyHost()
	debug "beginning VerifyHost function": indent()
	debug "Locating host mod essentials"
	If fileexists(path&mod_name&"\liblist.gam")<>True Then
		if mod_name="" then
			DebugClose()
		Else
			'If EXEname="CStrike" Then
				Message "The installer could not find the liblist.gam file for your mod."&vbcrlf&"This means you may not have specifed a valid modification folder."&vbCrlf&vbCrlf&"Retail Counter-Strike sometimes cannot be properly identified by this installer,"&vbcrlf&"if you find that you cannot get theis script to work then please refer to"&vbcrlf&"the manual installation instruction in the manual."&vbCrlf&vbCrlf&"This installer will now Exit.",vbOkonly,"Exiting"
			'Else
			'	Message "The installer could not find the liblist.gam file for your mod."&vbcrlf&"This probably means you have not specifed a valid modification folder."&vbCrlf&"This installer will now exit.",vbOkonly,"Exiting"
			'End If
			DebugClose()
		end if
	end if

	If folderexists(path&mod_name&"\dlls")<>true then
		Message "The installer was unable to find the dlls directory of the mod you specified."&vbcrlf&"This installer will now exit.",vbokonly,"Exiting"
		debug "Unable to locate mod dlls folder": DebugClose()
	end if
	unindent(): debug "completed VerifyHost function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub InstallAdminmod()
	debug "beginning InstallAdminmod function": indent()
	debug "starting installation and modification of files"
	' create the new folders if they are not present
	if not folderexists(path & mod_name & folders_t.addons)     then CreateFolder(path & mod_name & folders_t.addons)
	if not folderexists(path & mod_name & folders_t.am_root)    then CreateFolder(path & mod_name & folders_t.am_root)
	if not folderexists(path & mod_name & folders_t.am_config)  then CreateFolder(path & mod_name & folders_t.am_config)
	if not folderexists(path & mod_name & folders_t.am_scripts) then CreateFolder(path & mod_name & folders_t.am_scripts)
	if not folderexists(path & mod_name & folders_t.am_dlls)    then CreateFolder(path & mod_name & folders_t.am_dlls)

	' now install the essential Admin files, if base files are not found then create then, do not overwrite though
	copyfile files_f.am_plgini ,         path & mod_name & folders_t.am_config  & "\", true 'overwrite any present ini file
	copyfile files_f.am_amdll,           path & mod_name & folders_t.am_dlls    & "\", true 'always overwrite the dlls
	copyfile files_f.am_amxdll,          path & mod_name & folders_t.am_dlls    & "\", true 'always overwrite the dlls
	copyfile folders_f.am_scripts&"\*.*",path & mod_name & folders_t.am_scripts & "\", true 'always overwrite amx binaries

	if OldConfigsPath="" then
		copyfile "config\adminmod.cfg",   path & mod_name & folders_t.am_config  & "\", true 'always overwrite the config file
		if fileexists(path & mod_name & folders_t.am_config & "\maps.ini")    = false then CreateTextFile path & mod_name & folders_t.am_config & "\maps.ini",true    ': debug "created maps.ini"
		if fileexists(path & mod_name & folders_t.am_config & "\ips.ini")     = false then CreateTextFile path & mod_name & folders_t.am_config & "\ips.ini",true     ': debug "created ips.ini"
		if fileexists(path & mod_name & folders_t.am_config & "\models.ini")  = false then CreateTextFile path & mod_name & folders_t.am_config & "\models.ini", true ': debug "created models.ini"
		if fileexists(path & mod_name & folders_t.am_config & "\users.ini")   = false then CreateTextFile path & mod_name & folders_t.am_config & "\users.ini",true   ': debug "created users.ini"
		if fileexists(path & mod_name & folders_t.am_config & "\wordlist.txt")= false then CreateTextFile path & mod_name & folders_t.am_config & "\wordlist.txt",true': debug "created wordlist.txt"
	else
		debug "restoring old config files where possible, new files will otherwise be created"
		if fileexists(OldConfigsPath&"\adminmod.cfg") then copyfile OldConfigsPath&"\adminmod.cfg", path & mod_name & folders_t.am_config & "\",true else copyfile "config\adminmod.cfg",   path & mod_name & folders_t.am_config  & "\", true
		if fileexists(OldConfigsPath&"\maps.ini")     then copyfile OldConfigsPath&"\maps.ini",     path & mod_name & folders_t.am_config & "\",true else CreateTextFile path & mod_name & folders_t.am_config & "\maps.ini",    true ': debug "created maps.ini"
		if fileexists(OldConfigsPath&"\ips.ini")      then copyfile OldConfigsPath&"\ips.ini",      path & mod_name & folders_t.am_config & "\",true else CreateTextFile path & mod_name & folders_t.am_config & "\ips.ini",     true ': debug "created ips.ini"
		if fileexists(OldConfigsPath&"\models.ini")   then copyfile OldConfigsPath&"\models.ini",   path & mod_name & folders_t.am_config & "\",true else CreateTextFile path & mod_name & folders_t.am_config & "\models.ini",  true ': debug "created models.ini"
		if fileexists(OldConfigsPath&"\users.ini")    then copyfile OldConfigsPath&"\users.ini",    path & mod_name & folders_t.am_config & "\",true else CreateTextFile path & mod_name & folders_t.am_config & "\users.ini",   true ': debug "created users.ini"
		if fileexists(OldConfigsPath&"\wordlist.txt") then copyfile OldConfigsPath&"\wordlist.txt", path & mod_name & folders_t.am_config & "\",true else CreateTextFile path & mod_name & folders_t.am_config & "\wordlist.txt",true ': debug "created wordlist.txt"
		debug "files restored, """&OldConfigsPath&""" will be deleted"
		deletefolder OldConfigsPath,true
	end if
	EditMMPluginsFile path & mod_name & folders_t.mm_root &"\plugins.ini", right(replace(folders_t.am_dlls,"\","/"), len(folders_t.am_dlls)-1) & "/admin_MM.dll"
	EditConfigFiles()
	unindent(): debug "completed InstallAdminmod function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub EditConfigFiles()
	debug "beginning EditConfigFiles sub": indent()
	dim Files,FilesCount
	' now edit the server.cfg and listenserver.cfg files to exec the adminmod.cfg file, take account of read only status
	Files=array("listenserver.cfg","server.cfg")
	dim EditedFile
	EditedFile=false
	for FilesCount = 0 to ubound(Files)
		EditedFile = EditConfigFile(Files(filesCount))
	next
	if EditedFile=false then
		debug "neither statndard config files could be located to add the adminmod.cfg entry to, the user will be notified"
		message "The installer was unable to locate either listenserver.cfg or server.cfg. These files would normally be appended with an exec entry which causes the adminmod.cfg to be executed when the server runs, this will not be the case on this server. You will need to manually add the exec entry (which can be found in the manual section on this insatller) to a config file which you know will be eecuted each time the server or a map loads.",vbokonly,"No exec line created"
	end if
	unindent(): debug "completed EditConfigFiles function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub ManualShortcut()
	debug "beginning ManualShortcut sub": indent()
	' inform of the sucessful install and then do the cleanup procedures
	if CreateShortcut()=true then
		debug "displaying end dialogue"&vbcrlf
		Message Strings(5),vbOkonly,"Installation Completed" 'shortcut created string
	else
		debug "displaying end dialogue"&vbCrlf
		Message Strings(6),vbOkonly,"Installation Completed" 'no shortcut created string
	end if
	unindent(): debug "completed ManualShortcut function"&vbcrlf
end sub
'------------------------------------------------------------------------------------------------------------------------
private function Selection(byref Paths)
	debug "beginning Selection function": indent()
	dim answer,invalid,promptstring,promptnumber,selpath
	Steam.Install = false
	debug "selecting on number of located installations: """ & Paths.count &""""
	select case Paths.count
	case 0
		debug "entered 0 case, prompting user to specify path or quit"
		answer=Message("No Halflife game or dedicated server installations found." & vbcrlf & vbcrlf & "Advanced users who wish to use a full path for the installation click OK."&vbcrlf&"Click Cancel To exit.",vbokcancel,"No Half-Life dedicated server found")
		if answer=vbCancel then debug "chose to quit": DebugClose()
		if answer=vbOk     then debug "No HL.exe, HLDS.exe, DOD.exe or Cstrike.exe found: chose to specify a path"&vbCrlf: Selection = Specify()
	case 1
		debug "entered 1 case, prompting user to accept, specify path or quit"
		dim temppath
		promptstring="Single installation detected." & vbcrlf & vbcrlf
		if Paths.HLPath    <>"" then 
			promptstring=promptstring&"Halflife Game          ("&Paths.HLPath    &")"&vbcrlf: temppath=Paths.HLPath
		elseif Paths.HLDSPath  <>"" then
			promptstring=promptstring&"Dedicated Server   ("&Paths.HLDSPath  &")"    &vbcrlf: temppath=Paths.HLDSPath
		elseif Paths.CSPath    <>"" then
			promptstring=promptstring&"CS Retail            ("&Paths.CSPath    &")"  &vbcrlf: temppath=Paths.CSPath
		elseif Paths.DODPath   <>"" then
			promptstring=promptstring&"DOD Retail             ("&Paths.DODPath   &")"&vbcrlf: temppath=Paths.DODPath
		elseif Paths.STEAMPath <>"" then
			promptstring=promptstring&"STEAM Retail          ("&Paths.STEAMPath   &")" &vbcrlf: temppath=Paths.STEAMPath
			Steam.Install = true
		end if
		promptstring = promptstring&vbcrlf&"If you wish to install to this path Click Yes"&vbcrlf&"Advanced users who wish to specify a Path Click No"&vbcrlf&"To exit this installer Click Cancel"
		answer = Message(promptstring,vbyesnocancel,"Choose Installation")
		select case answer
			case vbYes
				debug "user chose to install to """&temppath&""""
				Selection = temppath
			case vbNo
				debug "user chose to specify"
				Selection = Specify()
			case vbCancel
				debug "user chose to cancel installation"
				DebugClose()
		end select
	
	case 2,3,4,5
		debug "entered 2,3,4,5 case, prompting user to choose or quit"
		promptnumber=1
		promptstring="Multiple Halflife game or dedicated server installations detected." & vbcrlf & vbcrlf
		if Paths.HLPath    <>"" then promptstring=promptstring&promptnumber  &" ) Halflife Game          ("&Paths.HLPath    &")"&vbcrlf: promptnumber=promptnumber+1
		if Paths.HLDSPath  <>"" then promptstring=promptstring&promptnumber  &" ) Dedicated Server   ("&Paths.HLDSPath  &")"    &vbcrlf: promptnumber=promptnumber+1
		if Paths.CSPath    <>"" then promptstring=promptstring&promptnumber  &" ) CS Retail            ("&Paths.CSPath    &")"  &vbcrlf: promptnumber=promptnumber+1
		if Paths.DODPath   <>"" then promptstring=promptstring&promptnumber  &" ) DOD Retail             ("&Paths.DODPath   &")"&vbcrlf: promptnumber=promptnumber+1
		if Paths.STEAMPath <>"" then promptstring=promptstring&promptnumber  &" ) STEAM           ("&Paths.STEAMPath   &")" &vbcrlf: promptnumber=promptnumber+1
		promptstring=promptstring&vbcrlf&"Please enter the NUMBER corresponding to the path you wish to install to and then click OK"&vbcrlf&"click Cancel or enter nothing to quit"
		invalid=false
		answer=input(promptstring,"Choose Installation")
		if answer="" then debug "user entered nothing or canceled": DebugClose()
		'msgbox "answer = " & answer & vbcrlf & "promptnumber = "& promptnumber & vbcrlf & "answer>promptnumber = " & (cint(answer)>cint(promptnumber))
		if not isnumeric(answer) then invalid=true: debug "user entered non numeric data """ &answer& """"	
		if not invalid then 
			answer=cint(answer)
			if cint(answer)<1 or cint(answer)>cint(promptnumber) then invalid=true : debug "user entered out of range integer """ & answer &""""
		end if
		
		if invalid then
			debug "prompting user a second time"
			invalid=false
			promptstring="ERROR: Bad input, please try again."&vbcrlf&vbcrlf&promptstring
			answer=input(promptstring,"Choose Installation")
			debug answer
			if answer="" then DebugClose()
			if not isnumeric(answer) then invalid=true: debug "user entered non numeric data """ &answer& """"	
			if not invalid then 
				answer=cint(answer)
				if cint(answer)<1 or cint(answer)>cint(promptnumber) then
					invalid=true
					debug "user entered out of range integer """ & answer &""""
					debug "user entered invalid data, the script will now exit": DebugClose()
				end if				
			end if
		end if
		
		promptnumber=1
		if Paths.HLPath<>"" then
			if promptnumber=answer then Selpath = Paths.HLPath
			promptnumber=promptnumber+1
		end if
		if Paths.HLDSPath<>"" then
			if promptnumber=answer then Selpath = Paths.HLDSPath
			promptnumber=promptnumber+1
		end if
		if Paths.CSPath<>"" then
			if promptnumber=answer then Selpath = Paths.CSPath
			promptnumber=promptnumber+1
		end if		
		if Paths.DODPath<>"" then
			if promptnumber=answer then Selpath = Paths.DODPath
			promptnumber=promptnumber+1
		end if
		if Paths.STEAMPath<>"" then
			if promptnumber=answer then 
				Selpath = Paths.STEAMPath
				Steam.Install = true
			end if
			promptnumber=promptnumber+1
		end if

		debug "user entered number " & answer & " which translates to path " & selpath
		Selection = Selpath

	case else
		debug "entered case else, something somewhere has gone horribly wrong": DebugClose()
	end select
	unindent():	debug "completed Selection function"&vbcrlf
end function
'------------------------------------------------------------------------------------------------------------------------
private function InstallMetamod
	dim ModifyLiblist,DeferMoveMetamodIni,LiblistCorrect,Liblist_stream,invar
	debug "beginning InstallMetamod function": indent()
	if not InCVSMode then
		debug "removing any old style metamod installation"
		ModifyLiblist =true
	
		' remove any oldstyle metamod install found
		if fileexists( path & mod_name & "\dlls\metamod.dll")               then deletefile path & mod_name & "\dlls\metamod.dll",true
		if fileexists( path & mod_name & "\metamod.ini") then DeferMoveMetamodIni=true else DeferMoveMetamodIni=false
		if fileexists( path & mod_name & "\AdminMod backup of liblist.gam") then ' if the old backup of liblist.gamis present then restore it.
			deletefile path & mod_name & "\liblist.gam",true
			copyfile   path & mod_name & "\AdminMod backup of liblist.gam",path & mod_name & "\liblist.gam",true
			deletefile path & mod_name & "\AdminMod backup of liblist.gam",true
		end if
		debug "completed old style metamod file removal"
	
		set Liblist_stream = fso.opentextfile(path&mod_name&"\liblist.gam",forreading,false)
		debug "scanning liblist for metamod.dll"
		do while Liblist_stream.atendofstream<>true
			invar=lcase(trim(Liblist_stream.readline))
			if len(invar) >2 then
				if instr(1,invar,"metamod.dll") >0 and instr(1,invar,"dlls\metamod.dll") =0 and left(invar,2) <>"//" and instr(1,invar,"amedll")>0 then
					LiblistCorrect=false
				end if
			end if
		loop
		Liblist_stream.close: set Liblist_stream=nothing
	
		'check for a new style metamod installtion.
		debug "checking for new style metamod installation"
		if folderexists(path&mod_name&"\addons") and folderexists(path&mod_name&"\addons\metamod") and folderexists(path&mod_name&"\addons\metamod\dlls") and fileexists(path&mod_name&"\addons\metamod\dlls\metamod.dll") then
			if liblistCorrect=false then
				ModifyLiblist=true
				debug "complete new style metamod installtion found with erroneous liblist, this will be fixed"
			 else
				debug "complete new style metamod installtion found"
				ModifyLiblist=false
			end if
		else
			debug "incomplete or no new style metamod installtion found, installing"
			if not folderexists(path & mod_name & folders_t.addons)  then createfolder path & mod_name & folders_t.addons
			if not folderexists(path & mod_name & folders_t.mm_root)    then createfolder path & mod_name & folders_t.mm_root
			if not folderexists(path & mod_name & folders_t.mm_dlls)    then createfolder path & mod_name & folders_t.mm_dlls
			copyfile files_f.mm_dll,   path & mod_name & folders_t.mm_dlls & "\",          true
			if DeferMoveMetamodIni then ' at dead_ones insitance, pointless though it may seem.
				copyfile   path & mod_name & "\metamod.ini",path &mod_name & folders_t.mm_root & "\plugins.ini",true
				deletefile path & mod_name & "\metamod.ini",true
			end if
			EditMMPluginsFile path & mod_name & folders_t.mm_root &"\plugins.ini", ""
			ModifyLiblist =true
			debug "completed metamod installation"
		end if
		InstallMetamod=ModifyLiblist
	else
		InstallMetamod=false
	end if

	
	unindent(): debug "completed InstallMetamod function"&vbcrlf
end function
'----------------------------------------------------------------------------------------ModifyLiblistFile-Sub-----------
private sub ModifyLiblistFile(ModifyLiblist)
	dim out_lib,in_lib,line,GamedllFound,RO,file
	debug "beginning ModifyLiblistFile sub": indent()
	GamedllFound=false
	if ModifyLiblist=true then
		set file=fso.getfile(path&mod_name&"\liblist.gam")
		
		If (file.attributes And FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY Then
			debug "liblist.gam Is a read-only file, changing"
			file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
			RO=true
		End If

		set file=nothing

		Set in_lib = fso.OpenTextFile(path&mod_name&"\liblist.gam",ForReading,False)

		debug "creating temporary file to replace liblist.gam"
		Set out_lib = fso.OpenTextFile("admin.tmp",ForWriting,True)

		do while in_lib.AtEndOfStream<>true
			line = in_lib.readline
			If instr(line,"gamedll")>0 And instr(line,"gamedll_linux")<1 Then
				debug "found gamedll line, changing"
				line ="gamedll ""addons\metamod\dlls\metamod.dll"""
				If line<>"" And GamedllFound =false Then out_lib.writeline(line)
				if GamedllFound=false then GamedllFound=true
			else 
				out_lib.writeline(line)
			end if
		loop

		in_lib.close: set in_lib=nothing
		out_lib.close: set out_lib=nothing
		debug "checking for previous backup of liblist.gam"

		If fileexists(path&mod_name&"\AdminMod backup of liblist.gam")=false then
			debug "no previous backup found"
			copyfile path&mod_name&"\liblist.gam", path&mod_name&"\AdminMod backup of liblist.gam",true
		Else
			debug "previous backup found"
			deletefile path&mod_name&"\liblist.gam",true
		end if
		copyFile "admin.tmp",path&mod_name&"\liblist.gam",true

		If RO=true Then
			set file=fso.getfile(path&mod_name&"\liblist.gam")
			file.attributes=(file.attributes Or FILE_ATTRIBUTE_READONLY)
			set file=nothing: RO=false
			debug "liblist.gam read-only status restored"
		End If

		deletefile "admin.tmp",true
	else
		debug "no liblist.gam modifications required"
	end if
	unindent(): debug "completed ModifyLiblistFile sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------EditMMPluginsFile-Fun-----------
private sub EditMMPluginsFile(pluginsFilePath,entry)
	dim out_lib
	debug "beginning EditMMPluginsFile sub": indent()
	if not InCVSMode then
		if entry <>"" then debug "adding ""win32 "&entry&""" to """& pluginsFilePath & """ file" else debug "creating file   """&pluginsFilePath&""""
		if fileexists(pluginsFilePath) = false then
			if NEUTER=0 then set out_lib = fso.CreateTextFile(pluginsFilePath)
		else
			set out_lib = fso.OpenTextFile(pluginsFilePath,ForAppending,false)
		end if
		if entry<>"" then
			if NEUTER=0 then out_lib.writeline(vbcrlf&"Win32 " & entry)
			debug "line written, closing file"
		end if
		if NEUTER=0 then out_lib.close	
		debug "completed file changes"
	else
		debug "no changes to metamod files are performed in CVS install, you must make them manually"
	end if
	unindent(): debug "completed EditMMPluginsFile sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------EditConfigFile-Fun--------------
private function EditConfigFile(FileName)
	dim execString,in_lib,out_lib,RO,invar,file
	debug "beginning EditConfigFile sub": indent()
	debug "starting "&FileName&" file changes"
	If fileexists(path&mod_name&"\"&FileName)=true Then
		debug "server.cfg file located at """&path&mod_name&"\"&FileName&""""
		Set file =fso.getfile(path&mod_name&"\"&FileName)

		If (file.attributes And FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY Then
			debug FileName&" Is a read-only file, changing"
			file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
			RO=True
		End If

		if file.size=0 then 
			debug FileName&" file size is 0 bytes"
			invar=""
		else
			Set out_lib = fso.opentextfile(path&mod_name&"\"&FileName,ForReading,false)
			invar=out_lib.readall
			out_lib.close
			Set out_lib=nothing
			debug "scanning for ""adminmod.cfg"""
		end if

		If instr(1,invar,"adminmod.cfg")=0 Then
			debug "no reference to adminmod.cfg found, appending"
			if invar<>"" then
				Set out_lib = fso.opentextfile(path & mod_name & "\" & FileName,forappending,false)
			else
				Set out_lib = fso.opentextfile(path & mod_name & "\" & FileName,forwriting,false)
			end if

			If NEUTER=0 Then
				out_lib.WriteBlankLines(2)
				out_lib.write "// Execute the Admin Mod configuration file"&vbCrlf
				execstring=replace(folders_t.am_config & "/adminmod.cfg"&vbCrlf,"\","/")
				execstring=right(execstring, len(execstring)-1)
				execstring="exec "&execstring
				out_lib.write execstring
			End If
			out_lib.close
			Set out_lib = nothing
			debug "file appended and closed"
		else
			debug "found reference to ""adminmod.cfg"" no changes are required"
		end If

		invar=""

		If RO=true Then
			file.attributes=(file.attributes Or FILE_ATTRIBUTE_READONLY)
			Set File=nothing
			debug FileName&" read-only status of restored"
			RO=false
		End If
		EditConfigFile=true
	Else
		debug "server.cfg not found, no changes can be made"
		EditConfigFile=false
	end if
	debug "completed "&FileName&" file changes"
	unindent(): debug "completed EditConfigFile sub"&vbcrlf
end function
'----------------------------------------------------------------------------------------GetHLKeys-Sub-------------------
private function GetHLKeys(byref Paths)
	Dim ValveHLKey,SolHlKey,SolCSKey,SolHLDSKey,SoftHLKey,SoftCSKey,SoftHLDSKey,SoftDODKey,SolDODKey,SoftSteamKey
	Dim SteamAppsInstPath
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
		MsgRet = message(Strings(2),vbokcancel,"Object creation error")
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
		SolHLDSKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Sierra OnLine\Setup\directory")
		if SolHLDSKey<>"" then if right(SolHLDSKey,1)="\" then SolHLDSKey=left(SolHLDSKey,len(SolHLDSKey)-1)
		debug vbtab&"SolHLDSKey  = "&SolHLDSKey
		
		SoftHLDSKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\HLServer\InstallPath")
		if SoftHLDSKey<>"" then if right(SoftHLDSKey,1)="\" then SoftHLDSKey=left(SoftHLDSKey,len(SoftHLDSKey)-1)
		debug vbtab&"SoftHLDSKey = "&SoftHLDSKey
		
		' STEAM keys 
		SoftSteamKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Valve\Steam\InstallPath")
		if SoftSteamKey<>"" then if right(SoftSteamKey,1)="\" then SoftSteamKey=left(SoftSteamKey,len(SoftSteamKey)-1)
		debug vbtab&"SoftSteamKey = "&SoftSteamKey
		SteamAppsInstPath =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\Steam\ModInstallPath")
		debug vbtab&"SteamAppsInstPath = "&SteamAppsInstPath
		
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

		if SoftSteamKey<>"" then
			if SteamAppsInstPath<>"" then
				' try to figure out the apps path from the registry key
				SteamAppsInstPath = left(SteamAppsInstPath,instrrev(SteamAppsInstPath ,"\")-1) 
				SteamAppsInstPath = left(SteamAppsInstPath,instrrev(SteamAppsInstPath ,"\")-1) 
			else
				SteamAppsInstPath = SoftSteamKey & "\SteamApps"
			end if
			if FolderExists(SteamAppsInstPath) then 
				Paths.STEAMpath=SoftSteamKey: Paths.count=Paths.count+1
				Steam.BasePath=SoftSteamKey
				Steam.AppsPath=SteamAppsInstPath
			end if
			debug "Steam paths are":indent()
			debug "Paths.STEAMPath= " & Paths.STEAMPath 
			debug "Steam.BasePath   = " & Steam.BasePath
			debug "Steam.AppsPath   = " & Steam.AppsPath:unindent()
		end if
		

		debug "Paths are":indent()
		debug "Paths.HLPath   = " & Paths.HLPath 
		debug "Paths.HLDSPath = " & Paths.HLDSPath 
		debug "Paths.CSPath   = " & Paths.CSPath 
		debug "Paths.DODPath  = " & Paths.DODPath
		debug "Paths.STEAMPath= " & Paths.STEAMPath:unindent()
		unindent():	debug "completed GetHLkeys sub"&vbCrlf
		GetHLKeys=true
	end if
end function
'----------------------------------------------------------------------------------------SteamAccount-Searching-Sub---------------
private function GetSteamAccFolder(byref Paths)
	dim mods,invar,folders,folder,NumFolders,accfolder,AppsPath
	debug "beginning GetSteamAccFolder sub":	indent()
	AppsPath = Steam.AppsPath
	set folders=fso.getfolder(AppsPath)
	'mods=vbTab&right(AppsPath,len(AppsPath)-instrrev(AppsPath,"\"))&vbCrlf
	mods=vbTab&vbCrlf
	NumFolders = 0
	For Each folder In folders.subfolders
		debug "checking folder: " & folder.path&"#"
		If instr(1,folder.path,"@")>0 Then
			debug "found potential account folder: " & folder.path
			if NumFolders = 0 then
				Mods=Mods &vbtab&folder.name&vbcrlf
			else
				Mods=Mods &"    or"&vbCrlf&vbtab&folder.name&vbcrlf
			end if
			NumFolders = NumFolders +1
			accfolder = folder.path
		Else 
			debug"not an account folder"
		end if
	next
	set folders=nothing
	'if mods=vbtab&right(AppsPath,len(AppsPath)-instrrev(AppsPath,"\"))&vbcrlf then message "The installer could not find any Steam account folder it can install Admin mod to."&vbcrlf&vbcrlf&"It will now exit",vbokonly,"No Mods Found": DebugClose()
	if NumFolders = 0 then message "The installer could not find any Steam account folder it can install Admin mod to."&vbcrlf&vbcrlf&"It will now exit",vbokonly,"No Mods Found": DebugClose()
	'mods=left(mods,instrrev(Mods,"|")-1)&"\"&right(Mods,len(Mods)-(instrrev(Mods,"|")+1))

	debug "Found " & NumFolders & " account folders"
	if NumFolders > 1 then
		debug "Prompting user for Steam account "
		accfolder = Steam.AppsPath & "\" &GetSteamEmail( mods )
	end if

	Steam.AccntPath = accfolder
	Paths.STEAMpath = accfolder
	debug "accFolder is " & accfolder
	debug "Steam.AccntPath is " & Steam.AccntPath

	GetSteamAccFolder=accfolder

	unindent():debug "completed GetSteamAccFolder sub"&vbCrlf
end function
'----------------------------------------------------------------------------------------SteamAccount-Request-Sub-----------------
private function GetSteamEmail(byref SteamAccList)
	debug "beginning GetSteamEmail function": indent()
	dim folder,usertext
	debug "prompting user for email address choice"
	usertext = "Enter the e-mail address for your Steam account that you want to install Admin Mod for."&vbCrlf&vbCrlf&SteamAccList
	folder = Input(usertext,"Steam e-mail address")
	If folder="" then
		debug "blank input is interpreted as cancel: quitting": DebugClose()
	elseif folderexists(Steam.AppsPath & "\" & folder)=false Then
		debug "user input """&folder&""" which is not a valid Steam folder, second and final prompt"
		folder = Input("Invalid e-mail address entered." &vbcrlf&vbcrlf&vbcrlf&usertext,"Steam account e-mail address")
		If folder="" then
			debug "blank input is interpreted as cancel: quitting": DebugClose()
		elseif folderexists(Steam.AppsPath & "\" & folder)=false Then
			debug "second erroneous folder: quiting":message "Either you cant type or you dont know what e-mail address you use for your Steam account. Please try again when you know."&vbcrlf&vbcrlf&"This script will now exit",vbokonly,"Exiting": DebugClose()
		else
			debug "valid Steam folder supplied """&folder&""" installation can proceed"
		end if
	else
		debug "valid Steam folder supplied """&folder&""" installation can proceed"
	end if
	GetSteamEmail = folder
	unindent(): debug "completed GetSteamEmail function"&vbcrlf
end function
'----------------------------------------------------------------------------------------HL/HLDS-Search-Sub--------------
private sub Search(byref Paths)
	dim AdminPath,Parent,GrandParent,folders,folder
	debug "beginning Search sub": indent()
	AdminPath   = wscript.scriptfullname
	if instr(1,AdminPath,"\")>0 then Parent = left(AdminPath,instrrev(AdminPath ,"\")-1) 
	if instr(1,Parent,"\")>0 then GrandParent = left(Parent,instrrev(Parent ,"\")-1) 
	debug "AdminPath = "&AdminPath
	debug "Parent Path = "&Parent
	debug "Grand Parent Path = "&GrandParent
	if Paths.count>1 Then debug "exe paths already located, exiting": debug "completed Search Sub"&vbCrlf: Exit Sub
	if fso.fileexists(Parent&"..\HL.exe")=true Then
		Paths.HLpath=GrandParent
		debug "Found HL.exe in parent folder"
	elseif fso.fileexists(Parent&"..\Cstrike.exe")Then
		Paths.CSpath=GrandParent
		debug "Found Cstrike.exe in parent folder"
	elseif fso.fileexists(Parent&"..\DOD.exe")Then
		Paths.DODpath=GrandParent
		debug "Found DOD.exe in parent folder"
	elseif fso.fileexists(Parent&"..\HLDS.exe")=True Then
		Paths.HLDSpath=GrandParent
		debug "Found HLDS.exe in parent folder"
	else
		debug "parent folder does not contain HL/HLDS/Cstrike/DOD, searching"
		Set Folders=fso.getfolder(GrandParent)
		debug vbTab&"Root search folder: """&folders.path&""""
		for each folder in Folders.subfolders
			if HLpath="" And fileexists(folder.path&"\Cstrike.exe")=True Then
				Paths.CSpath=left(folder.path,instrrev(folder.path,"\")-1)
				debug "found Cstrike.exe in: """&folder.path&""""
			elseif HLpath="" And fileexists(folder.path&"\HL.exe")=True Then
				Paths.HLpath=left(folder.path,instrrev(folder.path,"\")-1)
				debug "found HL.exe in: """&folder.path&""""
			Elseif HLpath="" And fileexists(folder.path&"\DOD.exe")=True Then
				Paths.DODpath=left(folder.path,instrrev(folder.path,"\")-1)
				debug "found DOD.exe in: """&folder.path&""""
			else
				debug "no Half-Life exe's found in: """&folder.path&""""
			end If
			
			if HLDSpath="" And fileexists(folder.path&"\HLDS.exe")=True Then
				Paths.HLDSpath=left(folder.path,instrrev(folder.path,"\")-1)
				debug "found HLDS.exe in :"""&folder.path&""""
			end If
			'if fso.fileexists(folder.path&"\HLDS.exe")=true and fso.fileexists(folder.path&EXEname&".exe")=false and HLDSpath="" then Paths.HLDSpath =left(folder.path,instrrev(folder.path,"\")-1)	
		next
	End If
	Set folder=Nothing:	Set folders=Nothing
	
	debug "Paths are": indent()
	debug "Paths.HLPath   = " & Paths.HLPath 
	debug "Paths.HLDSPath = " & Paths.HLDSPath 
	debug "Paths.CSPath   = " & Paths.CSPath 
	debug "Paths.DODPath  = " & Paths.DODPath: unindent()
	
	unindent(): debug "completed Search sub"&vbCrlf
end sub
'----------------------------------------------------------------------------------------Full-Path-Secification-Sub------
private function Specify()
	debug "beginning Specify sub": indent()
	dim localPath
	debug "prompting use to supply a valid fully qualified path"
	localPath=Input("Specify the full path to the HL, Cstrike or HLDS executable file for which you" & vbcrlf & "wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
	if localPath="" then debug "user chose to cancel or entered a null string, quitting": DebugClose()
	if fileexists(localPath)<>true Or ( lcase(right(localpath,8))<>"hlds.exe" and lcase(right(localpath,6))<>"hl.exe" and lcase(right(localpath,11))<>"cstrike.exe" and lcase( right(localpath,7) )<> "dod.exe" ) Then
		debug "incorrect but not null path entered, second prompting"
		localPath=Input("Incorrect Path." & vbCrlf & vbCrlf & "Specify the full path to the HL, Cstrike or HLDS executable file for which you wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
		if fileexists(localPath)<>true and (lcase(right(localPath,8))<>"hlds.exe" or lcase(right(localPath,6))<>"hl.exe" or lcase(right(localPath,11))<> "cstrike.exe" or lcase(right(localPath,7))<> "dod.exe")  then debug "second incorrect path, the user should find the correct path manually or learn to type, quitting": DebugClose()
	end If
	debug "Input path = " & localPath
	if len(localPath)>8 then
		If lcase(right(localPath,8))="hlds.exe" Then localPath =left(localPath,len(localPath)-9)
	end if
	if len(localPath)>6 then
		If lcase(right(localPath,6))="hl.exe" Then localPath =left(localPath,len(localPath)-7)
	end if
	if len(localPath)>11 then
		If lcase(right(localPath,11))="cstrike.exe" Then localPath =left(localPath,len(localPath)-12)
	end if
	if len(localPath)>8 then
		If lcase(right(localPath,8))="dod.exe" Then localPath =left(localPath,len(localPath)-8)
	end if
	debug "resultant path = " &localPath
	Specify = localPath
	Steam.Install = false
	unindent(): debug "completed Specify sub"&vbCrlf
end function
'----------------------------------------------------------------------------------------Mod-Searching-Sub---------------
private function GetMods
	dim mods,invar,folders,folder,subfolder,subfolders
	debug "beginning GetMods sub":	indent()
	debug "Path is " & Path
	set folders=fso.getfolder(Path)
	mods=vbTab&right(Path,len(Path)-instrrev(Path,"\"))&vbCrlf
	For Each folder In folders.subfolders
		debug "checking folder: " & folder.path&"#"
		If fso.fileexists(folder.path&"\liblist.gam")=True Then
			debug "found liblist.gam file in: " & folder.path
			set tso=fso.opentextfile(folder.path&"\liblist.gam",ForReading,false)
			invar=tso.readall: tso.close: set tso=nothing
			if instr(1,invar,"gamedll")>0 then Mods=Mods &vbtab&"     |--- "&folder.name&vbcrlf
			invar=""
		Else 
			' Only search subfolders for Steam
			if Steam.Install then
				set subfolders=fso.getfolder(folder.path)
				For Each subfolder In subfolders.subfolders
					debug "checking subfolder: " & subfolder.path&"#"
					If fso.fileexists(subfolder.path&"\liblist.gam")=True Then
						debug "found liblist.gam file in: " & subfolder.path
						set tso=fso.opentextfile(subfolder.path&"\liblist.gam",ForReading,false)
						invar=tso.readall: tso.close: set tso=nothing
						if instr(1,invar,"gamedll")>0 then Mods=Mods &vbtab&"     |--- "&folder.name&"\"&subfolder.name&vbcrlf
						invar=""
					Else 
						debug"not a mod folder"
					end if
				next
			else
				debug"not a mod folder"
			end if
		end if
	next
	set folders=nothing
	set subfolders=nothing
	if mods=vbtab&right(Path,len(Path)-instrrev(Path,"\"))&vbcrlf then message "The installer could not find any valid Mod installations it can install Admin mod to."&vbcrlf&vbcrlf&"It will now exit",vbokonly,"No Mods Found": DebugClose()
	mods=left(mods,instrrev(Mods,"|")-1)&"\"&right(Mods,len(Mods)-(instrrev(Mods,"|")+1))
	GetMods=mods
	unindent():debug "completed GetMods sub"&vbCrlf
end function
'----------------------------------------------------------------------------------------Create-desktop-shortcut---------
Private function CreateShortcut
	debug "beginning CreateShortcut sub":Indent()
	dim answer,DocPath,MyShortcut, DesktopPath
	if NOSHORTCUT<>0 or InCVSMode then
		debug "shortcut creation is disabled in this script"
		CreateShortcut=false
	else
		debug "beginning CreateShortcut sub"
		select case WSCRIPT_VERSION
		case "5.1"
			debug "Windows scripting host version 5.1 detected, too low to use shortcut object."
			CreateShortcut=false
		case "5.5","5.6" 
			answer=message("Do you wish to create a shortcut to the Admin mod Manual on your desktop?",vbYesNo,"Create Manual Shortcut?")
			if answer=vbyes then
				debug "user chose to create a desktop shortcut to the manual."
				debug "creating shell object"
				on error resume next ' allow soft errors
				set WSHShell = CreateObject("WScript.Shell")
				if err.number<>0 then
					debug "unable to create shell object despite correct WSH version, exiting"
					message Strings(2),vbokonly,"Shell Object Error"
			 		DebugClose()
				else
					debug "shell object created, shortcut will be created"
				end if
				on error goto 0 ' return to hard errors
				DesktopPath = WSHShell.SpecialFolders("Desktop"): debug "desktop path: "&DesktopPath
				set MyShortcut = WSHShell.CreateShortcut(DesktopPath & "\Admin Mod Manual.lnk"): debug "shortcut path: "&DesktopPath & "\Admin Mod Manual.lnk"
				DocPath = fso.GetAbsolutePathName(fso.GetParentFolderName("install_admin.vbs")) & "\docs": debug "doc path: "&DocPath
				MyShortcut.TargetPath = fso.getabsolutepathname(docpath&"\AdminMod-Win-Manual.chm"): debug "doc target: "&MyShortcut.TargetPath
				MyShortcut.WorkingDirectory = fso.getabsolutepathname(docpath)
				MyShortcut.WindowStyle = 4
				MyShortcut.Save
				set MyShortcut=nothing
				set WSHShell=Nothing
				debug "shortcut created, shell and shortcut objects destroyed"
				CreateShortcut=true
			else
				debug "user chose not to create a shortcut to the manual"
				debug "should they complain they cant find it they deserve to be slapped with a large trout"
				CreateShortcut=false
			end if
		case else
			debug "WSH version unrecognised, shell object creation was not attempted."
			CreateShortcut=false
		end select
	end if
	unindent(): debug "completed CreateShortcut sub"&vbCrlf
end function
'----------------------------------------------------------------------------------------Check-for-required-files--------
private sub CheckFiles
	debug "beginning CheckFiles Sub":indent()	

	if not FolderExists(folders_f.am_dlls)    then errorexit("folder (\"&folders_f.am_dlls&")")
	if not FolderExists(folders_f.am_config)  then errorexit("folder (\"&folders_f.am_config&")")
	if not fileexists  (files_f.am_amdll)     then errorexit("dll ("&files_f.am_amdll&")")
	if not fileexists  (files_f.am_amxdll)    then errorexit("dll ("&files_f.am_amxdll&")")
	if not fileexists  (files_f.am_amcfg)     then errorexit("file ("&files_f.am_amcfg&")")
	if not fileexists  (files_f.am_plgini)    then errorexit("file ("&files_f.am_plgini&")")
	if not FolderExists(folders_f.am_config) then
		Message replace(Strings(7),"%REPLACEME%",folders_f.am_config),vbOkonly,"Installation could not start"
		DebugClose()
	end if
	if not fileexists(files_f.am_amxbase) then
		Message replace(Strings(7),"%REPLACEME%",files_f.am_amxbase),vbOkonly,"Installation could not start"
		DebugClose()
	end If
		
	if not InCVSMode then
		if not FolderExists(folders_f.am_docs) then errorexit("folder (\"&folders_f.am_docs&")")
		if not fileexists  (files_f.am_docs) then errorexit("file (\"&files_f.am_docs&")")
		if not fileexists  (files_f.mm_dll)  then errorexit("dll ("&files_f.mm_dll&")")
	end if
	
	unindent(): debug "CheckFiles sub Completed"&vbCrlf
end sub
'----------------------------------------------------------------------------------------Generic-fileerror-exit----------
private sub ErrorExit(errorfile)
	debug "An essential file could not be found meaning that the installer cannot continue."
	debug "Please unpack the Admin Mod distribution archive again making sure to preserve the directory structure and try again."
	Message "The installer could not find an essential " & errorfile & "."& vbcrlf & vbcrlf & "This is usually caused by Admin Mod being unzipped incorrectly." & vbcrlf & "Please try again, making sure to maintain the directory structure." & vbcrlf & vbcrlf & "This installer will now exit.",vbokonly,"Installation could not start"
	DebugClose()
end Sub
'----------------------------------------------------------------------------------------find-the-uninstaller------------
private sub CheckUninstall()
	debug "beginning CheckUninstall sub": indent()

	dim OldDir,NewDir,UninstallPath,ExitLoopNow,FS2,TS2,Return,Shell,invar,answer,OldConfigsPath
	if NOUNINSTALL=1 then 
		debug "CheckUninstall has been disabled in this script"
		if fso.fileexists(path&mod_name&"\dlls\admin_mm.dll") or fso.fileexists(path&mod_name&"\dlls\amx_admin.dll") then
			message "CheckUninstall has been disabled in your script but a previous Admin Mod installation has been detected in your chosen installation location. The script will not continue."&vbcrlf&vbcrlf&"Pleaase remove the Admin Mod files either with the uninstaller or manually before attempting to install to this location.",vbokonly,"Previous install detected"
			DebugClose()
		end if
		exit sub
	end if

	debug "creating filesystem and textstream objects"
	set FS2=createobject("scripting.filesystemobject")
	set TS2=fs2.opentextfile(path&mod_name&"\liblist.gam",ForReading,false)
	ExitLoopNow=False
	do while TS2.atendofstream<>true 
		if ExitLoopNow=True Then Exit Do
		invar=trim(TS2.readline)
		if len(invar)>2 then
			if instr(1,invar,"gamedll ")>0 And instr(1,invar,"metamod.dll")>0 And left(invar,2)<>"//" then
				debug "found metamod in liblist.gam and dlls folder, searching for Admin Mod specific files"
				indent()
				if fileexists(path&mod_name&"\plugin.ini") or fileexists(path&mod_name&"\adminmod.cfg") or fileexists(path&mod_name&"\users.ini") or fileexists(path&mod_name&"\AdminMod Backup of Liblist.gam") or fileexists(path&mod_name&"\dlls\admin_mm.dll") or fileexists(path&mod_name&"\dlls\amx_admin.dll") or folderexists(path&mod_name&"\addons\adminmod") then
					if FS2.fileexists("uninstall_admin.vbs")=true then
						UninstallPath="uninstall_admin.vbs": NewDir=""
					elseif FS2.fileexists("tools\uninstall_admin.vbs")=true Then
						UninstallPath="uninstall_admin.vbs": NewDir="tools"
					else 
						UninstallPath=""
					end If
					invar=""
					ExitLoopNow=True
				else
					debug "no Admin Mod specific files found, only metamod is installed"
				end if
				unindent()
			end if
		end if
	loop
	TS2.close

	if ExitLoopNow =true then
		if NewDir="" then
			debug "uninstaller path is """&UninstallPath&"""" 
		else
			debug  "uninstaller path is """&NewDir&"\"&UninstallPath&""""
		end if
	else 
		debug  "no uninstaller path required"
	end if
	debug  "destroying filesystem and textstream objects"
	set Shell=Nothing: Set FS2=Nothing: Set TS2=Nothing

	if Uninstallpath<>"" Then
		select case WSCRIPT_VERSION
		case "5.1"
			debug "WSH version is too low to use the run method. Wscript version is " &WSCRIPT_VERSION
			debug "unable to run uninstall_admin.vbs so uninstallation cannot be performed automatically"
			Message Strings(8),vbOkonly,"WSH version too low to automatically uninstall"
			DebugClose()
		case "5.5","5.6"
			if NOKEEPCONFIGS=0 then
				debug  "prompting user to retain old config files or create anew"
				answer=message(strings(4),vbYesNo,"Retain old config files?")
				if answer = vbyes then
					debug  "user chose to retain their old files, on their own head be it"
					OldConfigsPath = path &mod_name&"\Old AdminMod Config Files"
					debug "OldConfigsPath = "&OldConfigsPath
					debug "backups folder will be created for the uninstaller to use"
					if not folderexists(OldConfigsPath) then createfolder OldConfigsPath
				else
					debug "user chose to create new files."
					OldConfigsPath=""
				end if
			else
				debug "old config file retention is disabled in this script, old config files will be removed without prompting"
			end if

			debug "creating shell object"
			on error resume next ' alow soft errors
			set Shell = createobject("Wscript.shell")
			if err.number<>0 then
				message Strings(2),vbokonly,"Shell Object Error"
				err.clear: DebugClose()
			else
				on error goto 0 ' return to hard errors
				debug "uninstaller was located at """&UninstallPath&""" it will now be run"
				debug "changing working directory for uninstallation"
				OldDir = shell.currentdirectory					
				if NewDir<>"" then shell.currentdirectory=shell.currentdirectory&"\tools"
				debug "executing "&UninstallPath&" "&chr(34)&path&mod_name&chr(34)&" "&chr(34)&OldConfigsPath&chr(34)
				Return= shell.run (UninstallPath&" "&chr(34)&path&mod_name&chr(34)&" "&chr(34)&OldConfigsPath&chr(34),,true)
				debug "uninstaller returned code "&Return
				debug "restoring working directory for installation to continue"
				shell.currentdirectory=OldDir
				debug "displaying upgrade warning about (listen)server.cfg file cvars"
				message strings(9),vbokonly or 64,"Upgrade installation advisory"
			end if

			if Return<>0 then
				Message "Admin Mod was unable to uninstall your previous version of Admin Mod completely. Please refer to the release notes before attempting to continue."&"We apologise for the inconvienience."&vbCrlf&vbCrlf&"This installer will now exit.",vbOkonly,"Exiting"
				DebugClose()
			end If
		end select
	else
		if ExitLoopNow=true then
			debug "unable to locate uninstall_admin.vbs so uninstallation cannot be performed automatically"
			Message "You must uninstall your previous version of Admin Mod before you install Admin Mod "&version&"."&vbCrlf&vbCrlf&"This installer will now exit.",vbOkonly,"Exiting"
			DebugClose()
		else
			debug "no previous installations located, continuing"
		end if
	end if
	unindent(): debug "completed CheckUninstall sub"&vbcrlf
end sub
'----------------------------------------------------------------------------------------InputBox-Abstraction------------
private function Input(Prompt,Title)
	if InConsoleMode=true then
		dim PromptArr, x
	
		Prompt=replace(Prompt,"click Cancel","Enter c")
		Prompt=replace(Prompt,"click Yes","Enter y")
		Prompt=replace(Prompt,"click No","Enter n")
		Prompt=replace(Prompt,"Click Cancel","Enter c")
		Prompt=replace(Prompt,"Click Yes","Enter y")
		Prompt=replace(Prompt,"Click No","Enter n")
		Prompt=replace(Prompt,"click OK","Press Enter to continue")
		Prompt=replace(Prompt,"Click OK","Press Enter to continue")
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
'----------------------------------------------------------------------------------------Msgbox-Abstraction--------------
private function Message(MessageText,Buttons,Title)
	if InConsoleMode=true then
		dim MessageArr, x, ConsoleInput
		
		MessageText=replace(MessageText,"click OK","Enter o")
		MessageText=replace(MessageText,"Click OK","Enter o")
		MessageText=replace(MessageText,"click Cancel","Enter c")
		MessageText=replace(MessageText,"click Yes","Enter y")
		MessageText=replace(MessageText,"click No","Enter n")
		MessageText=replace(MessageText,"Click Cancel","Enter c")
		MessageText=replace(MessageText,"Click Yes","Enter y")
		MessageText=replace(MessageText,"Click No","Enter n")
		MessageText=replace(MessageText,"click OK","Press Enter to continue")
		MessageText=replace(MessageText,"Click OK","Press Enter to continue")
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
					case "C":    message=vbcancel
					case else:   message=vbcancel
				end select
			case vbyesnocancel
				select case  ucase(left(consoleinput,1))
					case "C":  message=vbcancel
					case "Y":  message=vbyes
					case "N":  message=vbno
					case else: message=vbcancel
				end select
			case else
				select case ucase(left(consoleinput,1))
					case "C":  message=vbcancel
					case "Y":  message=vbyes
					case "N":  message=vbno
					case "O":  message=vbok
					case else: message=vbok
				end select
		end select
	else
		message=msgbox(MessageText,Buttons,Title)
	end if
end function
'----------------------------------------------------------------------------------------Wscript/Cscript-determination---
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
private function IsLocalCVSDir()
	dim retval
	retval=false
	if fso.FolderExists("CVS") then
		if fso.FileExists("CVS\root") then
			retval = true
		end if
	end if
	IsLocalCVSDir = retval
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
			if instr(1,invar,chr(60)&StringName&chr(62)) >0 then ReadOn=true
		else
			if instr(1,invar,chr(60)&"/"&StringName&chr(62)) >0 then 
				ReadOn=false: exit do
			else
				if len(Invar)>0 then Retvar=RetVar&vbcrlf&right(Invar,len(Invar)-1)
			end if
		end if
	loop
	if lTSO.atendofstream=true and ReadOn=true then
		if DEBUGME>1 then debug "[DEBUG-2] string """&StringName&""" was not closed"
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",16,"Internal script error"
		DebugClose()
	end if
	lTSO.close: set lTSO=nothing

	Retvar =trim(replace(RetVar,"%VERSION%",Version))
	Retvar =replace(RetVar,"%WSCRIPTVERSION%",WSCRIPT_VERSION)

	if len(RetVar) >0 then
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" retrieved, "&len(RetVar)&" chrarcters long"&vbcrlf
		GetString = RetVar
	else
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" could not be retrieved, the script will exit"&vbcrlf
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",16,"Internal script error"
		DebugClose()
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
 		if right(frompath,3)<>"*.*" then
			set file=fso.getfile(ToPath & right(frompath,len(frompath) - instrrev(frompath,"\")))
			if (file.attributes and FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY then
				debug ToPath&" Is a read-only file, changing"
				file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
			end if
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
		set TSD = FSD.createtextfile("install_admin.log",True)
		on error goto 0
		if err.number<>0 then
			Message "The installer has debug mode enabled. During initialisation the installer was unable to create the ""install_admin.log"" file in the installer directory. This problem prevents the installer from running."&vbcrlf&"Please run the installer again when it is possible to write to this file."&vbcrlf&vbcrlf&"This installer will now exit",vbokonly or  vbExclamation,"Fatal error creating debug log"
			wscript.quit
		end if
		debug "debug file ""install_admin.log"" created sucessfully"&vbCrlf&"debug Filesystem object created sucessfully"&vbcrlf
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
	wscript.quit
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
'--------------------------------------------------------------------------------
'<WELCOME>
'	This script will install Admin Mod %VERSION% for you.
'
'
'Admin Mod is not a trivial program. It requires the user to possess at least
'minimal knowledge pertaining to the setup and maintenance of a server for
'Half-Life or other Modification
'
'It is strongly advised that first time server operators set up their server to
'a fully working and stable state before attempting to use Admin Mod.
'If a problem is encountered, consult the supplied documentation.
'
'		Choose OK to continue.
'</WELCOME>
'<REGISTRY>
'This installer will now attempt to use your registry to find the location of
'your Half-life game and/or your dedicated server.
'
'Click OK to allow the Admin Mod installer to read the required registry keys.
'Click Cancel to disallow the Admin Mod installer from using your registry. (Advanced)
'</REGISTRY>
'<SHELLERROR>
'The installer was unable to create the Shell object required to run the
'uninstall script despite the Windows Scripting Host version being high enough
'to support this method.
'
'This is likely to be a problem with your Windows Scripting Host Installation.
'Either uninstall the previous version of Admin Mod manually or update/repair
'your Windows Scripting Host installation then run this script again.
'
'The installer cannot recover, it will now exit.
'</SHELLERROR>
'<REGISTRYERROR>
'The installer was unable to create the ActiveX component required to
'interact with your registry.
'
'This is a problem with your Windows Scripting Host installation and can be fixed
'by reinstalling/updating the Windows Scripting Host.
'
'Your current Windows Scripting Host is version %wscriptversion%
'The Windows Scripting Host can be downloaded free from Microsoft at
'http://msdn.microsoft.com/scripting
'
'You can continue by specifying the path manually if you wish to.
'Click OK to continue.
'Click Cancel to exit now
'</REGISTRYERROR>
'<SHORTEND>
'	           Admin Mod %VERSION% is now minimally installed.
'
'In order to use Admin Mod successfully you will need to create/configure your
'users.ini file and set up the client/game-play computer with passwords in order
'to access the server with administrative rights. Please note that this version
'of the installer does not add any users to the users.ini or create any password
'files. This must be done after installation.
'
'Admin Mod %VERSION% installs its configuration variables into a separate config
'file called adminmod.cfg. This keeps them separate from other server settings
'and allows easier configuration. You should edit your config file to reflect
'your configuration preferences.
'
'A shortcut to the documentation has been created on your desktop - Please take
'the time to read it carefully. If you have a problem after reading the manual
'visit the Admin Mod forums, which can be found by following the links from
'www.adminmod.org.
'</SHORTEND>
'<NOSHORTEND>
'	           Admin Mod %VERSION% is now minimally installed.
'
'In order to use Admin Mod successfully you will need to create/configure your
'users.ini file and set up the client/game-play computer with passwords in order
'to access the server with adminisrative rights. Please note that this version
'of the installer does not add any users to the users.ini or create any password
'files. This must be done after installation.
'
'Admin Mod %VERSION% installs its configuration variables into a separate config
'file called adminmod.cfg, this keeps them separate from other server settings
'and allows easier configuration. You should edit your config file to reflect
'your configuration preferences.
'
'The Admin Mod manual can be found in the Adminmod\docs folder - Please take
'the time to read it carefully. If you have a problem after reading the
'manual visit the Admin Mod forums, which can be found by following the links
'from www.adminmod.org.
'</NOSHORTEND>
'<MISSINGFILE>
'The installer could not find an essential folder (%REPLACEME%).
'
'Before you install Admin Mod you must run compile_all.bat To produce the
'compiled scripts Admin Mod uses To provide it's functionality. Compile_all.bat
'can be found in the \scripting\examples directory. Please run it and then
'execute this installation script again.
'</MISSINGFILE>
'<KEEPCONFIGS>
'In order to install Admin Mod %VERSION% the previous version will first be removed.
'
'Do you with to retain the configuration files (adminmod.cfg, wordlist.txt, 
'models.ini, etc...) from your previous installation? If so please be aware that 
'if the previous version of Admin Mod installed is a version before %VERSION% then
'the adminmod.cfg file may be missing new and possibly important cvar entries.
'It is strongly suggested that you only retain config files if you are certain that
'they are appropriate to the version of Admin Mod being installed.
'
'N.B. The uninstaller will always make backup copies of all files it removes,
'these are placed in a folder in the same directory as the uninstallation script 
'(default Adminmod\tools). This means that you can still recover your old files 
'even if you anwer no to this prompt.
'
'Click Yes to retain your current config files
'Click No to have new empty files created
'</KEEPCONFIGS>
'<NOUNINSTALL>
'Your Windows Scripting Host version (%WSCRIPTVERSION%) is too low for the
'installer to use the Wscript.shell object to run the uninstallation script
'automatically. You must uninstall your previous version of Admin Mod before
'you install Admin Mod %VERSION%.
'
'You can either locate the uninstaller (default Adminmod\tools) and use that
'script, or you can upgrade your scripting host installation or you can uninstall
'your previous version of Admin Mod manually.
'
'Once you have completed one of these options please run this installation
'script again.
'
'This installer will now exit.
'</NOUNINSTALL>
'<CONFIGWARNING>
'Your previous version of Admin Mod has been uninstalled.
'
'At present the uninstaller cannot selectively remove Admin Mod Cvars from the 
'server.cfg or listenserver.cfg files. Admin Mod %VERSION% keeps all Admin Mod
'Cvars in the adminmod.cfg file in the <mod>\addons\adminmod\config folder and
'requires only a single exec line to be present in the normal (listen)server.cfg.
'
'if you are upgrading from a version before 2.50.50 then you will need to manually
'remove all adminmod settings apart from the newly appended exec line at the end
'of the (listen)server.cfg file. 
'
'If you do not do this you may experience odd behavior because of duplicated
'and different Cvar settings. It is strongly reccomended that you do this as
'soon as possible to avoid confusion.
'</CONFIGWARNING>
'<BETA>
'	    This is a BETA installer for Admin Mod
'
'This script cannot make any filesystem changes outside the directory it is run
'from. In it's own directory it will create a log file which details the steps
'it took in finding and attempting to install Admin Mod.
'
'  IT WILL NOT INSTALL ADMIN MOD UNLESS YOU ENABLE IT TO DO SO
'
'To enable installation open the script file in notepad or other text editor and 
'change the line:
'  Const NEUTER = 1
'to:
'  Const NEUTER = 0
'save, exit, then run the script again
'
'any errors or problems should be reported to Wraith, in the Admin mod Win32 forum.
'</BETA>
'<ABSTRACTIONERROR>
'An error has occured in the %ERROR_PROC%. 
'
'If you cannot locate the cause of this problem (read only media, invalid
'or inadequate permissions etc...) then please report this error on the Admin
'Mod Win32 forum which can be found by following the links from the main Admin
'Mod website at www.adminmod.org
'
'The Error details are:
'%ERROR_DETAILS%
'
'This cannot be recovered from and the script will now close. 
'</ABSTRACTIONERROR>
'<CVS-MESSAGE>
'
'
'
'
'
'                                 Your Ad here                                                   
'
'
'
'
'
'
'</CVS-MESSAGE>
