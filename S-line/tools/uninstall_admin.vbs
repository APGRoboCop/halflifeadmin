'---------------------------------------------------------------------------------------------------------------------------'
'															    '
' 			  Uninstall Script for Admin Mod v2.50.x							    '
'			  Copyright (C) 2000 Alfred Reynolds (alfred@mazuma.net.au) 				    '
'			  Copyright (C) 2001-2003 David Mart 				    '
'			  $Id: uninstall_admin.vbs,v 1.5 2004/08/02 16:33:46 the_wraith Exp $			    '
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
'  Sets the wscript version used in version dependant switches, wscript.version is normal but setting it will force a single set of methods.
'---------------------------------------------------------------------------------------------------------------------------'
dim WSCRIPT_VERSION:WSCRIPT_VERSION=cstr(wscript.version)
'---------------------------------------------------------------------------------------------------------------------------'
version= "2.xx.yy"
'---------------------------------------------------------------------------------------------------------------------------'
Dim FSO,TSO,FSD,TSD,silent,InConsoleMode,InCVSMode,ScriptPath,OldConfigsPath,IndentValue,exenames,Steam,installs,choice,version,strings,folders_f,folders_t
Const ForReading = 1, ForWriting = 2, ForAppending = 8
class FilePaths_From: public mm_dll:public mm_ini:public am_amxdll:public am_amdll:public am_amcfg:public am_amxbase: public am_docs: public am_plgini:public sub class_initialize: if not InCVSMode then:	mm_ini="\plugins.ini":mm_dll = "dlls\metamod.dll": am_amxdll = "dlls\amx_admin.dll": am_amdll = "dlls\admin_MM.dll": am_amcfg = "config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="docs\AdminMod-Win-Manual.chm": am_plgini="config\plugin.ini": else:mm_ini="\plugins.ini":mm_dll="..\metamod\dlls\metamod.dll":am_amxdll = "dlls\amxdll\release\amx_admin.dll":am_amdll="dlls\adminmod\releaseMM\admin_MM.dll":am_amcfg="config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="":am_plgini="config\plugin.ini":end if:end sub:end class
class FolderPaths_From:public am_config:public am_scripts:public am_docs:public am_dlls:public am_samples:public am_tools:public sub class_initialize:am_config = "config":am_docs="docs":am_dlls="dlls":am_scripts="scripting\binaries":am_samples="config\samples":am_tools="tools":end sub:end class
class FolderPaths_To: public steam:public am_config:public am_dlls:public am_scripts:public am_root:public mm_dlls: public mm_root:public addons: public sub class_initialize:am_config ="\addons\adminmod\config":am_dlls="\addons\adminmod\dlls":am_scripts="\addons\adminmod\scripts":am_root="\addons\adminmod":mm_dlls="\addons\metamod\dlls":mm_root="\addons\metamod":steam ="":addons="\addons":end sub:end class
class SteamInstall: Public Install: Public BasePath: Public AppsPath: Public SelectedAddress: end class
class ChoiceItem: public path: public exename: public description: public issteam: public oldtype: end class
'---------------------------------------------------------------------------------------------------------------------------'
Init()
GetScriptPath()
GetCommandLineArgs()
Start()
if silent=0 then
	set installs = FindAdminInstallations()
	set choice = Selection(installs)
end if
'printchoice choice
Uninstall choice
Close()
'---------------------------------------------------------------------------------------------------------------------------'
public sub Init
	on error resume next
	Set fso = CreateObject("Scripting.FileSystemObject")
	if err.number<>0 then message "The uninstaller was unable to create the ActiveX componant required to"&vbcrlf&"interact with your computers filesystem."&vbcrlf&"This is a problem with your windows scripting host installtion and can be fixed"&vbcrlf&"by reinstalling/updating the Windows Scripting Host."&vbcrlf&vbcrlf&"The windows scripting host can be downloaded free from Microsoft at"&vbcrlf&"http://msdn.microsoft.com/scripting",vbokonly or vbExclamation,"Object creation error":err.clear:wscript.quit 1
	on error goto 0
	DebugStart() ' self explanatory really
	debugfun "Init"
	'InCVSMode    =IsLocalCVSDir() ' determines if this is a CVS install
	InConsoleMode=IsHostCscript() ' determines console or gui mode
	if InConsoleMode then debug "script is running in console mode" else debug "script is running in GUI mode"
	'if InCVSMode     then debug "script is running in CVS location mode" else debug "script is running in distribution mode"
	'set files_f = new FilePaths_From
	set folders_f=new FolderPaths_From
	set folders_t=new FolderPaths_To
	set Steam = new SteamInstall
	Steam.Install = false
	exenames = array("hl.exe","cstrike.exe","dod.exe","hlds.exe","czero.exe")
	Strings = array(Getstring("WELCOME"),GetString("UNINSTALL_COMPLETE_MM"),GetString("UNINSTALL_COMPLETE"),GetString("SPECIFY_STRING"))
	debugendf "Init"
end sub

public sub Start()
	dim answer
	if silent=0 then
		answer = Message(Strings(WELCOME),vbokcancel or vbquestion,"Start Uninstallation")
		If answer<>vbOk Then debug "user canceled uninstallation": DebugClose(): wscript.quit 0
	end if
end sub
'---------------------------------------------------------------------------------------------------------------------------'
public sub Close
	debugfun "Close"
	debug "uninstallation complete, objects will be destroyed and the script will exit."
	debugendf "Close"
	Set fso=Nothing: DebugClose(): wscript.quit 0
end sub
'---------------------------------------------------------------------------------------------------------------------------'
private function CreateBackupPath(Version)
	dim step
	dim TempPath
	debugfun "CreateBackupPath"
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
	debugendf "CreateBackupPath"
end function

'----------------------------------------------------------------------------------------Full-Path-Secification-Sub------
public function Specify()
	debugfun "Specify"
	dim localPath,newchoice,exename
	debug "prompting use to supply a valid fully qualified path"
	'localPath=input("Specify the full path to the HL, Cstrike or HLDS executable file for which you" & vbcrlf & "wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
	localPath=input(strings(SPECIFY_STRING),"Specify a full path")
	if localPath="" then debug "user chose to cancel or entered a null string, quitting": DebugClose(): wscript.quit 0
	if fileexists(localPath)<>true Or ( lcase(right(localpath,8))<>"hlds.exe" and lcase(right(localpath,6))<>"hl.exe" and lcase(right(localpath,11))<>"cstrike.exe" and lcase( right(localpath,7) )<> "dod.exe" ) Then
		debug "incorrect but not null path entered, second prompting"
		localPath=Input("Incorrect Path." & vbCrlf & vbCrlf & "Specify the full path to the HL, Cstrike or HLDS executable file for which you wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
		if (not fileexists(localPath)) and (lcase(right(localPath,8))<>"hlds.exe" or lcase(right(localPath,6))<>"hl.exe" or lcase(right(localPath,11))<> "cstrike.exe" or lcase(right(localPath,7))<> "dod.exe")  then debug "second incorrect path, the user should find the correct path manually or learn to type, quitting": DebugClose(): wscript.quit 0
	end If
	debug "Input path = " & localPath
	set newchoice = Nothing
	for each exename in exenames
		if len(localPath) > (len(exename)+1) then
			if (lcase(right(localpath,len(exename)))) = exename then
				localpath = left(localpath,len(localpath)-(len(exename)+1))
				set newchoice = new ChoiceItem
				newchoice.exename=exename
				newchoice.issteam=false
				newchoice.path=localpath
				newchoice.description=""
				newchoice.oldtype=OldAdminVersion(localpath)
				exit for
			end if
		end if
	next
	debug "resultant path = " &localPath
	set specify = newchoice
	Steam.Install = false
	debugendf "Specify"
end function
'---------------------------------------------------------------------------------------------------------------------------'
public function Selection(byref Paths)
	debugsub "Selection"
	dim answer,invalid,promptstring,promptnumber,selpath,x,itemarray,items
	Steam.Install = false
	debug "selecting on number of located installations: " & Paths.count 
	select case Paths.Count
	case 0
		debug "entered 0 case, prompting user to specify path or quit"
		answer=Message("No Halflife game or dedicated server installations found." & vbcrlf & vbcrlf & "Advanced users who wish to use a full path for the installation click OK."&vbcrlf&"Click Cancel To exit.",vbokcancel,"No Half-Life dedicated server found")
		if answer=vbCancel then debug "chose to quit": DebugClose(): wscript.quit 0
		if answer=vbOk     then debug "No HL.exe, HLDS.exe, DOD.exe or Cstrike.exe found: chose to specify a path"&vbCrlf: set Selection = Specify()
	case 1
		debug "entered 1 case, prompting user to accept, specify path or quit"
		dim temppath,num
		num = cstr(ubound(paths.Items))
		promptstring="Single installation detected." & vbcrlf & vbcrlf
		promptstring=promptstring & Paths.Item(num).description &"(" & Paths.Item(num).path & ")" &vbcrlf
		promptstring = promptstring&vbcrlf&"If you wish to uninstall this version Click Yes"&vbcrlf&"Advanced users who wish to specify a Path Click No"&vbcrlf&"To exit this installer Click Cancel"
		answer = Message(promptstring,vbyesnocancel or vbquestion,"Choose Installation")
		select case answer
			case vbYes
				debug "user chose to install to """&Paths.Item(num).path&""""
				set Selection = Paths.Item(num)
			case vbNo
				debug "user chose to specify"
				Selection = Specify()
			case vbCancel
				debug "user chose to cancel uninstallation"
				DebugClose(): wscript.quit 0
		end select
	case 2,3,4,5
		debug "entered 2,3,4,5 case, prompting user to choose or quit"
		itemarray=paths.Items()
		redim items(ubound(itemarray))
		for x=0 to ubound(itemarray) 
			items(x)=itemarray(x).description &"("&itemarray(x).path &")"
		next
		answer = NumericChoice( _
			"Multiple Halflife game or dedicated server installations detected.", _
			"Please enter the NUMBER corresponding to the path you wish to uninstall from and then click OK"&vbcrlf&"click Cancel or enter nothing to quit", _
			items, _
			"Choose Uninstallation", _
			"" _
			)
		debug "user entered number " & answer & " which translates to path " & Paths.Item(cstr(answer)).path
		set Selection = Paths.Item(cstr(answer))
	case else
		debug "entered case else, something somewhere has gone horribly wrong" 
		message "entered case else, something somewhere has gone horribly wrong",vbokonly or vberror,"Script error"
		DebugClose()
		wscript.quit 1
	end select
	debugendf "Selection"
end function
'----------------------------------------------------------------------------------------HL/HLDS-Search-Sub--------------
public sub Search(byref Paths)
	dim AdminPath,Parent,GrandParent,folders,folder,exename,found
	debugsub "Search"
		if Paths.Count>1 then
		debug "installtion paths already located"
		debugends "Search"
		exit sub
	end if
	AdminPath   = wscript.scriptfullname
	if instr(1,AdminPath,"\")>0 then Parent = left(AdminPath,instrrev(AdminPath ,"\")-1) 
	if instr(1,Parent,"\")>0 then GrandParent = left(Parent,instrrev(Parent ,"\")-1) 
	debug "AdminPath = "&AdminPath
	debug "Parent Path = "&Parent
	debug "Grand Parent Path = "&GrandParent

	for each exename in exenames
		if (fso.FileExists(Parent & "..\"&exename)) then
			debug "Found "&exename&" in parent folder"
			AddChoice Paths,GrandParent,"\"&exename,"",false
			found=true
		end if
	next

	if not found then
		debug "parent folder does not contain HL/HLDS/Cstrike/DOD, searching"
		Set Folders=fso.getfolder(GrandParent)
		debug "Root search folder: """&folders.path&""""
		for each folder in Folders.subfolders
			for each exename in exenames
				if (fso.FileExists(folder.path&"\"&exename)) then
					debug "Found "&exename&" in """&folder.path&""""
					AddChoice Paths,folder.path,exename,"",false
					found=true
				end if
			next
			if not found then debug "no Half-Life exe's found in: """&folder.path&""""
		next
	End If
	Set folder=Nothing:	Set folders=Nothing

	debug "Paths are":indent()
	dim itemarray,x
	itemarray=paths.Items()
	for x=0 to ubound(itemarray) 
		debug "Paths(" & x & ").path=" & itemarray(x).path &" : "& itemarray(x).description
	next
	unindent()

	debugends "Search"
end sub
'---------------------------------------------------------------------------------------------------------------------------'
public sub Uninstall(byref choice)
	debugsub "Uninstall"
	dim BackPath,RemovedMetamod
	if choice.oldtype then
		BackPath = CreateBackupPath("old")
		Old_UninstallAdmin   choice.path,BackPath
		Old_UninstallMetamod choice.path,BackPath
		RestoreBackUPLiblist choice.path,BackPath
		RemovedMetamod=true
	else
		BackPath = CreateBackupPath("new")
		New_UninstallAdmin choice.path,BackPath
		if not New_OtherMMDllsPresent(choice.path) then
			New_UninstallMetamod choice.path,BackPath
			RestoreBackUPLiblist choice.path,BackPath
			RemoveAddons(choice.path&"\addons")
			RemovedMetamod=true
		else
			RemovedMetamod=false
			RemovePluginsEntry choice.path&folders_t.mm_root&"\plugins.ini"
			RenameAMBackupLiblist(choice.path&"\AdminMod backup of liblist.gam")
		end if
	end if 
	
	if silent = 0 then
		debug "displaying end dialogue"
		if RemovedMetamod=true then
			'message "Admin Mod has been uninstalled."&vbcrlf&vbcrlf&"The files have been backed up to a folder in the same diretory as the"&vbcrlf&"uninstallation script. if you wish to keep any settings you will need those"&vbcrlf&"files. Otherwise please delete the folder."&vbcrlf&vbcrlf&"Click Ok to finish",vbokonly,"Uninstallation Completed"
			message strings(UNINSTALL_COMPLETE_MM),vbokonly or vbinformation,"Uninstallation Completed"
		else
			'message "Admin Mod has been uninstalled."&vbcrlf&vbcrlf&"The files have been backed up to a folder in the same directory as the"&vbcrlf&"uninstallation script. if you wish to keep any settings you will need those"&vbcrlf&"files. Otherwise please delete the folder."&vbcrlf&vbcrlf&"Metamod has not been uninstalled because other dlls were located which may"&vbcrlf&"be using it. If it is no longer required you will have to remove it manually"&vbcrlf&vbcrlf&"Click Ok to finish",vbokonly,"Uninstallation Completed"
			message strings(UNINSTALL_COMPLETE),vbokonly or vbinformation,"Uninstallation Completed"
		end if
	end if
	debugends "Uninstall"
end sub
'----------------------------------------------------------------------------------------------------------------------------
public function GetHLKeys(byref Paths)
	Dim ValveHLKey,SolHlKey,SolCSKey,SolHLDSKey,SoftHLKey,SoftCSKey,SoftHLDSKey,SoftDODKey,SolDODKey,SoftSteamKey,SoftCzeroKey,SolCzeroKey
	Dim SteamAppsInstPath,WSHShell
	debugfun "GetHLkeys"
	on error resume next ' handle errors in _this procedure only_
	debug "Creating Wscript.shell object"
	set WSHShell = CreateObject("WScript.Shell") 
	if err.number<>0 then
		err.clear
		on error goto 0
		debug "Could not create Wscript.Shell object. Wscript version is "&wscript.version&"."
		debug "Prompting user to continue or fail uninstallation."
		dim MsgRet
		'MsgRet = message(Strings(REGISTRYERROR),vbokcancel or vbExclamation,"Object creation error")
		if MsgRet =vbcancel then 
			debug "completed GetHLKys"&vbcrlf&"user canceled uninstallation"
			DebugClose() : wscript.quit 0
		else
			debug "chose to continue"
			unindent()
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
		
		' Condition Zero key
		SolCzeroKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\Software\Valve\CZero\InstallPath")
		if SolCzeroKey<>"" then if right(SolCzeroKey,1)="\" then SolCzeroKey=left(SolCzeroKey,len(SolCzeroKey)-1)
		debug vbtab&"SolCzeroKey = "&SolCzeroKey
		
		SoftCzeroKey =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\CZero\InstallPath")
		if SoftCzeroKey<>"" then if right(SoftCzeroKey,1)="\" then SoftCzeroKey=left(SoftCzeroKey,len(SoftCzeroKey)-1)
		debug vbtab&"SoftCzeroKey = "&SoftCzeroKey
		
		' STEAM keys 
		SoftSteamKey =WSHShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Valve\Steam\InstallPath")
		if SoftSteamKey<>"" then if right(SoftSteamKey,1)="\" then SoftSteamKey=left(SoftSteamKey,len(SoftSteamKey)-1)
		debug vbtab&"SoftSteamKey = "&SoftSteamKey
		SteamAppsInstPath =WSHShell.RegRead("HKEY_CURRENT_USER\Software\Valve\Steam\ModInstallPath")
		debug vbtab&"SteamAppsInstPath = "&SteamAppsInstPath
		
		set WSHShell=Nothing: debug "keys read and WSH shell object destroyed"
		on error goto 0
		
		SearchHLKeys Paths,SoftHLKey,   SolHLKey,   ValveHLKey,"\hl.exe",   "Halflife Game          "
		SearchHLKeys Paths,SoftHLKey,   SolHLKey,   "",        "\cs.exe",   "CS Retail            "
		SearchHLKeys Paths,SoftDODKey,  SolDODKey,  "",        "\dod.exe",  "DOD Retail             "
		SearchHLKeys Paths,SoftHLDSKey, SolHLDSKey, "",        "\hlds.exe", "Dedicated Server   "
		SearchHLKeys Paths,SoftCzeroKey,SolCzeroKey,"",        "\czero.exe","Condition Zero        "
	
		
		if SoftSteamKey<>"" then
			if SteamAppsInstPath<>"" then
				' try to figure out the apps path from the registry key
				SteamAppsInstPath = left(SteamAppsInstPath,instrrev(SteamAppsInstPath ,"\")-1) 
				SteamAppsInstPath = left(SteamAppsInstPath,instrrev(SteamAppsInstPath ,"\")-1) 
			else
				SteamAppsInstPath = SoftSteamKey & "\SteamApps"
			end if
			if FolderExists(SteamAppsInstPath) then 
				dim choice
				set choice = new ChoiceItem
				choice.description="Steam Account      "
				choice.path=SoftSteamKey
				choice.issteam=true
				Paths.Add Cstr(Paths.Count),choice
				Steam.BasePath=SoftSteamKey
				Steam.AppsPath=SteamAppsInstPath
			end if
			debug "Steam paths are":indent()
			debug "Steam.BasePath   = " & Steam.BasePath
			debug "Steam.AppsPath   = " & Steam.AppsPath:unindent()
		end if
		
		debug "Paths are":indent()
		dim itemarray,x
		itemarray=paths.Items()
		for x=0 to ubound(itemarray) 
			debug "Paths(" & x & ").path=" & itemarray(x).path &" : "& itemarray(x).description
		next
		unindent()
		debugends "GetHLkeys"
		GetHLKeys=true
	end if
end function
'----------------------------------------------------------------------------------------SearchHLKeys-Sub------------------------
private sub SearchHLKeys(byref pathcollection,firstkey,secondkey,thirdkey,exename,description)
	dim choice
	debugsub "SearchHLKeys"
	if firstkey<>"" then 
		if fileexists(firstkey&exename) then
			AddChoice pathcollection,firstkey,exename,description,false,false
		end if
	else
		if secondkey<>"" then 
			if fileexists(secondkey&exename) then
				AddChoice pathcollection,secondkey,exename,description,false,false
			end if
		else
			if thirdkey<>"" then
				if fileexists(thirdkey&exename) then
					AddChoice pathcollection,thirdkey,exename,description,false,false
				end if
			end if
		end if
	end if
	debugends "SearchHLkeys"
end sub
'----------------------------------------------------------------------------------------AddChoice--------------------------------
private sub AddChoice(byref pathcollection,path,exename,description,issteam,oldtype)
	dim choice
	set choice = new ChoiceItem
	choice.description=description
	choice.exename=exename
	choice.path=path
	choice.issteam=issteam
	choice.oldtype=oldtype
	pathcollection.Add Cstr(pathcollection.Count),choice
	debug "Adding new choice to available paths": indent(): debug "path=" & choice.path: debug "exename="&choice.exename: debug "description="&choice.description: debug "issteam="&issteam:unindent()
end sub
'----------------------------------------------------------------------------------------Mod-Searching-Sub---------------
public function GetMods(modpath)
	dim mods(),invar,folders,folder,subfolder,subfolders
	debugfun "GetMods"
	debug "Path is " & modpath
	redim mods(0)
	set folders=fso.getfolder(modpath)
	for each folder in folders.subfolders
		debug "checking folder: " & folder.path&"#"
		if fso.fileexists(folder.path&"\liblist.gam") then
			debug "found liblist.gam file in: " & folder.path
			set tso=fso.opentextfile(folder.path&"\liblist.gam",ForReading,false)
			invar=tso.readall: tso.close: set tso=nothing
			if instr(1,invar,"gamedll")>0 then
				if mods(ubound(mods))<>"" then redim preserve mods(ubound(mods)+1)
				Mods(ubound(mods))=folder.name
			end if
			invar=""
		else
		debug "not a mod folder"
		end if
	next
	set folders=nothing: set subfolders=nothing
	GetMods=mods
	debugendf "GetMods"
end function
'-----------------------------------------------------------------------------------------------------------------------
public function GetFolderList(path)
	debugfun "GetFolderList"
	dim mods(),invar,folders,folder,subfolder,subfolders
	debug "Path is " & path
	redim mods(0)
	set folders=fso.getfolder(path)
	for each folder in folders.subfolders
		if mods(ubound(mods))<>"" then redim preserve mods(ubound(mods)+1)
		Mods(ubound(mods))=folder.name
	next
	set folders=nothing: set subfolders=nothing
	GetFolderList=mods
	debugendf "GetFolderList"
end function
'----------------------------------------------------------------------------------------------------------------------------
public Sub GetCommandLineArgs
	dim Command,bool
	debugsub "GetCommand"
	Set Command = WScript.Arguments
	
	if Command.count>0 then
		if Command(0)<>"" then
			debug "first argument passed was """&Command(0)&""", checking for existance of the passed location"
			if folderexists(Command(0))=true then
				if AdminInstalled(Command(0)) then
					'HLpath=Command(0)
					debug "path """&Command(0)&""" was passed in and verified"
					silent=1
					debug "silent mode enabled. No prompts will be allowed"
				end if
			end if
		end if
		if Command.count>1 then
			if Command(1)<>"" then
				debug "second argument passed was """&Command(1)&""", checking for existance of the passed location"
				if fso.folderexists(Command(1))=true then
					debug "second argument will be used as the temporary config file backup area, """&Command(1)&""""
					OldConfigsPath=Command(1) 
				else
					debug "second argument was empty, no temporary config file backup area will be used"
				end if
			end if
		end if
		if Command.count>2 then
			if Command(2)<>"" then
				bool = CBool(Command(2))
				debug "third argument will be used as a steam boolean identifier, issteam="&bool
			end if
		end if
		set choice = new ChoiceItem
		if bool then choice.issteam = true else choice.issteam=false
		choice.path = Command(0)
		choice.oldtype=OldAdminVersion(choice.path)		
	else
		debug "no arguments passed"
	end if
	debugends "GetCommand"
end sub

public sub GetScriptPath()
	ScriptPath    = left(wscript.scriptfullname,instrrev(wscript.scriptfullname,"\")-1)
end sub
'----------------------------------------------------------------------------------------------
public function FindAdminInstallations()
	debugfun "FindAdminInstallations"
	dim Paths,items,item,accounts,account,games,game,mods,mod_
	set Paths = CreateObject("Scripting.Dictionary")
	set installs = CreateObject("Scripting.Dictionary")
	GetHlKeys(paths)
	items=paths.Items()
	for each item in items
		if item.issteam then
			accounts = GetFolderList(steam.AppsPath)
			for each account in accounts
				if account <>"" then
					debug "checking account """ & account & """"
					indent()
					games = GetFolderList(steam.AppsPath&"\"&account)
					for each game in games
						if game<>"" then
							debug "checking game """ & game & """"
							indent()
							mods = GetMods(steam.AppsPath&"\"&account&"\"&game)
							for each mod_ in mods
								if mod_<>"" then
									debug "checking mod """ & mod_ & """"
									indent()
									if AdminInstalled( steam.AppsPath &"\"& account &"\"& game &"\"& mod_) then
										AddChoice _
											 installs, _
											 steam.AppsPath &"\"& account &"\"& game &"\"& mod_, _
											 "", _
											 "", _
											 false , _
											 OldAdminVersion(steam.AppsPath &"\"& account &"\"& game &"\"& mod_)
									end if
									unindent()
								end if
							next
							unindent()
						end if
					next
					unindent
				end if 
			next			
		else
			mods = GetMods(item.path)
			for each mod_ in mods
				if mod_<>"" then
					debug "checking mod """  & item.path &"\" & mod_ & """"
					indent()
					if AdminInstalled(item.path &"\" & mod_) then
						AddChoice installs,item.path &"\" & mod_,"","",false,OldAdminVersion(item.path &"\" & mod_)
					end if
					unindent()
				end if
			next
			
		end if
		
	next
	set FindAdminInstallations=installs
	debugendf "FindAdminInstallations"
end function
'----------------------------------------------------------------------------------------------------------------------------
public function AdminInstalled(byval ModPath)
	debugfun "AdminInstalled"
	if folderexists(ModPath &"\addons\adminmod")  then
		AdminInstalled=true
		debug "new style Admin mod installation found"
	else
		if fileexists(ModPath&"\dlls\admin_mm.dll") then 
			AdminInstalled=true
			debug "old style Admin mod installation found"
		end if
	end if
	debugendf "AdminInstalled"
end function
'----------------------------------------------------------------------------------------------------------------------------
public function OldAdminVersion(ModPath)
	debugfun "OldAdminVersion"
	if fileexists(ModPath&"\dlls\admin_mm.dll") then 
		OldAdminVersion=true
		debug "old style Admin mod installation found"
	else
		OldAdminVersion=false
	end if
	debugendf "OldAdminVersion"
end function
'----------------------------------------------------------------------------------------------------------------------------
public sub New_UninstallMetamod(Path,BackPath)
	debugsub "New_UninstallMetamod"

	debug "starting creating folders for backed up files"
	createfolder(BackPath & "\metamod")
	createfolder(BackPath & "\metamod\dlls")

	debug "finished creating folders for backed up files"
	debug "moving all metamod specific files to backup folders"
	if fileexists(Path & folders_t.mm_dlls & "\metamod.dll")  = true then movefile Path & folders_t.mm_dlls & "\metamod.dll", BackPath & "\metamod\dlls\"
	if fileexists(Path & folders_t.mm_root & "\plugins.ini")  = true then copyfile Path & folders_t.mm_root & "\plugins.ini", BackPath & "\metamod\", false
	debug "finished moving metamod specific files to backup folders"

	debug "deleting """ & folders_t.mm_root & """ folder"
	deletefolder Path & folders_t.mm_root, true

	debugends "New_UninstallMetamod"
end sub
'----------------------------------------------------------------------------------------------------------------------------
public sub New_UninstallAdmin(Path,BackPath)
	debugsub "New_UninstallAdmin"

	debug "starting creating folders for backed up files"
	createfolder(BackPath&"\adminmod")
	createfolder(BackPath&"\adminmod\dlls")
	createfolder(BackPath&"\adminmod\scripts")
	createfolder(BackPath&"\adminmod\config")
	debug "finished creating folders for backed up files"

	debug "moving all Admin mod specific files to backup folders"
	copyfile Path & folders_t.am_scripts & "\*.amx", BackPath & "\adminmod\scripts\",true
	if fileexists( Path & folders_t.am_dlls   & "\amx_admin.dll") then copyfile Path & folders_t.am_dlls & "\amx_admin.dll", BackPath & "\adminmod\dlls\" ,false
	if fileexists( Path & folders_t.am_dlls   & "\admin_mm.dll")  then copyfile Path & folders_t.am_dlls & "\admin_mm.dll",  BackPath & "\adminmod\dlls\",false
	if fileexists( Path & "\server.cfg") then
		copyfile   Path & "\server.cfg",       BackPath & "\adminmod\config\", false
		RemoveExec Path & "\server.cfg"
	end if
	if fileexists( Path & "\listenserver.cfg") then
		copyfile   Path & "\listenserver.cfg", BackPath & "\adminmod\config\", false
		RemoveExec Path & "\listenserver.cfg"
	end if
	if fileexists( Path & folders_t.am_config & "\adinmod.cfg")       then copyfile Path & folders_t.am_config & "\adminmod.cfg",BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\plugin.ini")        then copyfile Path & folders_t.am_config & "\plugin.ini",  BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\users.ini")         then copyfile Path & folders_t.am_config & "\users.ini",   BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\ips.ini")           then copyfile Path & folders_t.am_config & "\ips.ini",     BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\models.ini")        then copyfile Path & folders_t.am_config & "\models.ini",  BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\maps.ini")          then copyfile Path & folders_t.am_config & "\maps.ini",    BackPath & "\adminmod\config\",false
	if fileexists( Path & folders_t.am_config & "\wordlist.txt")      then copyfile Path & folders_t.am_config & "\wordlist.txt",BackPath & "\adminmod\config\",false
	debug "finished moving Admin mod specific files to backup folders"

	if OldConfigsPath<>"" then
		if fileexists( Path & folders_t.am_config & "\adminmod.cfg") then copyfile Path & folders_t.am_config & "\adminmod.cfg", OldConfigsPath & "\",true 
		if fileexists( Path & folders_t.am_config & "\plugin.ini")   then copyfile Path & folders_t.am_config & "\users.ini", OldConfigsPath & "\",true
		if fileexists( Path & folders_t.am_config & "\users.ini")    then copyfile Path & folders_t.am_config & "\ips.ini", OldConfigsPath & "\",true
		if fileexists( Path & folders_t.am_config & "\ips.ini")      then copyfile Path & folders_t.am_config & "\models.ini", OldConfigsPath & "\",true
		if fileexists( Path & folders_t.am_config & "\maps.ini")     then copyfile Path & folders_t.am_config & "\maps.ini", OldConfigsPath & "\",true
		if fileexists( Path & folders_t.am_config & "\wordlist.txt") then copyfile Path & folders_t.am_config & "\wordlist.txt", OldConfigsPath & "\",true
	end if

	debug "deleting """&folders_t.am_root&""" folder and subfolders"
	deletefolder Path & folders_t.am_dlls,   true
	deletefolder Path & folders_t.am_config, true
	deletefolder Path & folders_t.am_scripts,true
	deletefolder Path & folders_t.am_root   ,true

	debugends "New_UninstallAdmin sub"
end sub
'----------------------------------------------------------------------------------------------------------------------------
public function New_OtherMMDllsPresent(CheckPath)
	New_OtherMMDllsPresent=false
	debugfun "New_OtherMMDllsPresent"
	debug "root path is """&CheckPath&""""
	dim localFolder,LocalFile,localsubfolder,subsubfolder, subsublocalfolder,subfile
	if folderexists(checkpath) and folderexists(checkpath&"\addons") then set localfolder= fso.getfolder(checkpath&"\addons")
	for each localsubfolder in localfolder.subfolders
		debug "checking subfolder """&localsubfolder.name&""""
		for each subsubfolder in localsubfolder.subfolders
			if subsubfolder.name ="dlls" then
				set subsublocalfolder= fso.getfolder(subsubfolder.path)
				for each subfile in subsublocalfolder.files
					if len(subfile.name)>4 then
						if lcase(right(subfile.name,4))=".dll" and lcase(subfile.name)<>"metamod.dll" and lcase(subfile.name)<>"admin_amx.dll" and lcase(subfile.name)<>"admin_mm.dll" then
							debug "found """&subfile.path&""" dll file"
							New_OtherMMDllsPresent=true
							' this should't be here really, but there is no multilevel break and no goto's.
							debug "other addon dll located, metamod will not be removed"
							debug "completed New_OtherMMDllsPresent function"
							debugendf "New_OtherMMDllsPresent"
							exit function
						end if
					end if
				next
			end if
		next
	next
	debug "no other addons dll located. metamod will be uninstalled"
	debugendf "New_OtherMMDllsPresent"
end function
'----------------------------------------------------------------------------------------------------------------------------
public sub Old_UninstallMetamod(Path,BackPath)
	debugsub "Old_UninstallMetamod"
	debug "starting creating folders for backed up files"
	createfolder(BackPath & "\metamod")
	createfolder(BackPath & "\metamod\dlls")
	debug "finished creating folders for backed up files"
	
	debug "moving all metamod specific files to backup folders"
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

	debug "finished moving metamod specific files to backup folders"
	debugends "Old_UninstallMetamod"
end sub
'----------------------------------------------------------------------------------------------------------------------------
public sub Old_UninstallAdmin(Path,BackPath)
	debugsub "Old_UninstallAdmin"
	debug "starting creating folders for backed up files"
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
	debug "finished creating folders for backed up files"
	
	debug "moving all Admin mod specific files to backup folders"
	movefile Path & "\dlls\*.amx", BackPath & "\adminmod\scripts\"
	if fileexists( Path & "\dlls\amx_admin.dll") then movefile Path & "\dlls\amx_admin.dll", BackPath & "\adminmod\dlls\"
	if fileexists( Path & "\dlls\admin_mm.dll")  then movefile Path & "\dlls\admin_mm.dll",  BackPath & "\adminmod\dlls\"
	if fileexists( Path & "\server.cfg")         then 
		copyfile   Path & "\server.cfg",        BackPath & "\adminmod\config\", false
		RemoveExec Path & "\server.cfg"
	end if
	if fileexists( Path & "\listenserver.cfg") then 
		copyfile   Path & "\listenserver.cfg",  BackPath & "\adminmod\config\", false
		RemoveExec Path & "\listenserver.cfg"
	end if
	if fileexists( Path & "\admin_help.cfg")  then movefile Path & "\admin_help.cfg",     BackPath & "\adminmod\config\"
	if fileexists( Path & "\adminmod.cfg")    then movefile Path & "\adminmod.cfg",       BackPath & "\adminmod\config\"
	if fileexists( Path & "\plugin.ini")      then movefile Path & "\plugin.ini",         BackPath & "\adminmod\config\"
	if fileexists( Path & "\users.ini")       then movefile Path & "\users.ini",          BackPath & "\adminmod\config\"
	if fileexists( Path & "\ips.ini")         then movefile Path & "\ips.ini",            BackPath & "\adminmod\config\"
	if fileexists( Path & "\models.ini")      then movefile Path & "\models.ini",         BackPath & "\adminmod\config\"
	if fileexists( Path & "\maps.ini")        then movefile Path & "\maps.ini",           BackPath & "\adminmod\config\"
	if fileexists( Path & "\wordlist.txt")    then movefile Path & "\wordlist.txt",       BackPath & "\adminmod\config\"
	if fileexists( Path & "\liblist.gam")     then movefile Path & "\liblist.gam",        BackPath & "\adminmod\config\"
	if fileexists( Path & "\AdminMod backup of liblist.gam") then movefile Path&"\AdminMod backup of liblist.gam" ,Path&"\liblist.gam"
	debug "finished moving Admin mod specific files to backup folders"
	debugends "Old_UninstallAdmin "
end sub
'----------------------------------------------------------------------------------------------------------------------------
public sub RenameAMBackupLiblist(byval FilePath)
	debugsub "RenameAMBackupLIblist"
	dim File
	if fileexists(filepath) then
		debug "attempting to rename the file"
		set file=fso.getfile(FilePath)
		if fileexists(left(filepath, instrrev(filepath,"\")) & "metamod backup of liblist.gam") then deletefile left(filepath, instrrev(filepath,"\")) & "metamod backup of liblist.gam",true
		if not fileexists(left(filepath, instrrev(filepath,"\")) & "metamod backup of liblist.gam") then file.name="metamod backup of liblist.gam" ' looks daft but NEUTER protection is required here
		set file=nothing
		debug "file renamed sucessfully"
	end if
	debugends "RenameAMBackupLiblist"
end sub
'----------------------------------------------------------------------------------------------------------------------------
public sub RestoreBackUpLiblist(Path,BackPath)
	debugsub "RestoreBackUPLiblist"
	dim LiblistPath

	if fileexists(Path & "\AdminMod backup of liblist.gam") then
		LiblistPath = Path & "\AdminMod backup of liblist.gam"
	end if

	if LiblistPath<>"" then
		copyfile LiblistPath, BackPath & "\adminmod\", true
		deletefile path & "\liblist.gam", true
		movefile LiblistPath, Path & "\liblist.gam"
		debug "backup liblist.gam found, copied and restored"
	else
		debug "backup liblist.gam file could not be located and thus could not be restored."
	end if

	debugends "RestoreBackUPLiblist"
end sub

'----------------------------------------------------------------------------------------------------------------------------
public sub RemoveAddons(Path)
	debugsub "RemoveAddons"
	dim folder, subfolder, subfile, count
	count=0
	if folderexists(Path) then set Folder =fso.getfolder(path)
	debug "enumerating subfolders"
	for each subfolder in folder.subfolders
		count=count+1
	next

	debug "no subfolders found, enumerating files"
	if count=0 then
		for each subfile in folder.files
			count=count+1
		next
	else 
		debug "file found, \addons will not be deleted"
	end if

	if count=0 then
		debug "no files or folder were found, deleting """&path&""""
		deletefolder path,true
	end if
	debugends "RemoveAddons"
end sub	
'------------------------------------------------------------------------------------------------------------------------
public sub RemoveExec(CfgPath)
	dim file,invar,RO,filetext,infile
	debugsub "RemoveExec"
	debug "path passed: """ & CfgPath &""""
	set file=fso.getfile(CfgPath)
	If (file.attributes And 1)=1 Then
		debug "found file set as readonly, changed for editing"
		file.attributes= (file.attributes Xor 1)
		RO=True
	End If

	set infile=fso.opentextfile(CfgPath,forreading,false)

	debug "starting replacements of Admin mod specific strings"

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
		filetext=filetext & vbcrlf & invar
	loop
	infile.close
	
	if NEUTER=0 then
		set infile=fso.opentextfile(CfgPath,forwriting,true)
		infile.write(trim(filetext))
		infile.close: set infile=nothing
	end if

	debug "file rewritten"
	If RO=True Then
		debug "file being set to readonly status again"
		file.attributes=(file.attributes Or 1)
		Set File=Nothing
		RO=False
	End If
	debugends "RemoveExec"
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub RemovePluginsEntry(pluginsFilePath)
	dim file,invar,RO,filetext,infile
	debugsub "RemovePluginsEntry"
	debug "path passed: """ & pluginsFilePath &""""
	set file=fso.getfile(pluginsFilePath)
	If (file.attributes And 1)=1 Then
		debug "found file set as readonly, changed for editing"
		file.attributes= (file.attributes Xor 1)
		RO=True
	End If

	set infile=fso.opentextfile(pluginsFilePath,forreading,false)

	debug "starting replacements of Admin mod specific strings"

	do while infile.atendofstream<>true
		invar=infile.readline
		if (instr(1,lcase(invar),"admin_mm.dll")>0 ) then
			debug "[DEBUG] found admin_mm.dll"
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

	debug "file rewritten"
	If RO=True Then
		debug "file being set to readonly status again"
		file.attributes=(file.attributes Or 1)
		Set File=Nothing
		RO=False
	End If
	debugends "RemovePluginsEntry "
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
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",vbOkOnly or vbCritical,"Internal script error"
		DebugClose(): wscript.quit 1
	end if
	lTSO.close: set lTSO=nothing

	Retvar =trim(replace(RetVar,"%VERSION%",Version))
	Retvar =replace(RetVar,"%WSCRIPTVERSION%",WSCRIPT_VERSION)

	if len(RetVar) >0 then
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" retrieved, "&len(RetVar)&" chrarcters long"&vbcrlf
		GetString = RetVar
	else
		if DEBUGME >1 then debug "[DEBUG-2] string """&StringName&""" could not be retrieved, the script will exit"&vbcrlf
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",vbOkOnly or vbCritical,"Internal script error"
		DebugClose() : wscript.quit 1
	end if
end function

public function NumericChoice(header,footer,options,title,aboveoptions)
	debugfun "NumericChoice"
	dim answer,invalid,promptstring,items,x
	promptstring=header&vbcrlf&vbcrlf
	if aboveoptions<>"" then promptstring=promptstring&aboveoptions&vbcrlf
	for x=0 to ubound(options)
		promptstring = promptstring&cstr(x+1)&" ) "&options(x)
		if (right(promptstring,2))<>vbcrlf then promptstring=promptstring&vbcrlf
	next
	promptstring=promptstring&vbcrlf&footer
	NumericChoice=-1
	invalid=false
	answer=input(promptstring,title)
	if answer="" then
		debug "user entered nothing or canceled"
		DebugClose(): wscript.quit 0
	end if
	if not isnumeric(answer) then
		invalid=true
		debug "user entered non numeric data """ &answer& """"
	end if
	if not invalid then 
		answer=cint(answer)
		if cint(answer)<1 or cint(answer)>cint(ubound(options)+1) then invalid=true: debug "user entered out of range integer """ & answer &""""
	end if
	
	if invalid then
		debug "prompting user a second time"
		invalid=false
		promptstring="ERROR: Bad input, please try again."&vbcrlf&vbcrlf&promptstring
		answer=input(promptstring,"Choose Installation")
		debug answer
		if answer="" then DebugClose(): wscript.quit 0
		if not isnumeric(answer) then
			invalid=true
			debug "user entered non numeric data """ &answer& """"	
		end if
		if not invalid then 
			answer=cint(answer)
			if cint(answer)<1 or cint(answer)>cint(ubound(options)+1) then
				invalid=true
				debug "user entered out of range integer """ & answer &""""
				debug "user entered invalid data, the script will now exit": DebugClose() : wscript.quit 0
			end if				
		end if
	end if
	debug "user chose "&cstr(answer)&" (of 1 to "&cstr(ubound(options)+1)&")"
	NumericChoice = answer-1
	debugendf "NumericChoice"
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
public sub DebugStart()
	if DEBUGME>0 Then
		set FSD = CreateObject("Scripting.FileSystemObject")
		on error resume next
		set TSD = FSD.createtextfile("uninstall_admin.log",True)
		on error goto 0
		if err.number<>0 then
			Message "The uninstaller has debug mode enabled. During initialisation the uninstaller was unable to create the ""uninstall_admin.log"" file in the uninstaller directory. This problem prevents the uninstaller from running."&vbcrlf&"Please run the uninstaller again when it is possible to write to this file."&vbcrlf&vbcrlf&"This uninstaller will now exit",vbokonly or  vbExclamation,"Fatal error creating debug log"
			wscript.quit 1
		end if
		debug "debug file ""uninstall_admin.log"" created sucessfully"&vbCrlf&"debug Filesystem object created sucessfully"&vbcrlf
		if DEBUGME>1 then debug "options list":debug "DEBUGME = " & DEBUGME:debug "NEUTER = "&NEUTER:debug "end of options"&vbcrlf
	end if
end sub
'----------------------------------------------------------------------------------------debug-closure-------------------
public sub DebugClose()
	if DEBUGME>0 Then
		debug vbcrlf&"debug filesystem objects will be destroyed and the script will end."
		TSD.close
		set FSD=Nothing
		set TSD=Nothing
	end if
end sub
'----------------------------------------------------------------------------------------debug-writing-------------------
public sub Debug(DebugString)
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

public sub DebugFun(name)
	debug "beginning "&name&" function":Indent()
end sub

public sub DebugEndF(name)
	unindent():debug "completed "&name&" function"&vbcrlf
end sub

public sub DebugSub(name)
	debug "beginning "&name&" function":Indent()
end sub

public sub DebugEndS(name)
	unindent():debug "completed "&name&" sub"&vbcrlf
end sub

public const WELCOME=0,UNINSTALL_COMPLETE_MM=1,UNINSTALL_COMPLETE=2,SPECIFY_STRING=3
'<WELCOME>
'      This script will uninstall Admin Mod %VERSION% and earlier for you.
'
'
'This script will also remove Metamod if it cannot locate any other metamod
'plugins in the specified installation directory. This script can uninstall any
'version of Admin Mod greater than 2.10a and less than %VERSION% inclusive.
'
'All Admin Mod core files that are removed will be backed up to a newly created 
'subdirectory in the location that the uninstaller is run from. Any files which
'are not a part of Admin Mod will be deleted by this script and you are advised
'to remove any files you consider valuable before you continue.
'
'		  Choose OK to continue.
'</WELCOME>
'<UNINSTALL_COMPLETE_MM>
'                          Admin Mod has been uninstalled.
'
'The Admin Mod core files have been backed up to a folder in the directory this
'uninstall script was run from. if you wish to keep any settings you will need
'those files. Otherwise please delete the folder.
'
'Metamod has also been uninstalled.
'
'                                       Click Ok to finish
'</UNINSTALL_COMPLETE_MM>		
'<UNINSTALL_COMPLETE>		
'                          Admin Mod has been uninstalled.
'
'The files have been backed up to a folder in the same directory as the
'uninstallation script. if you wish to keep any settings you will need those
'files. Otherwise please delete the folder.
'
'Metamod has not been uninstalled because other dlls were located which may
'be using it. If it is no longer required you will have to remove it manually
'
'                                       Click Ok to finish
'</UNINSTALL_COMPLETE>
'<SPECIFY_STRING>
'Specify the full path to the HL, Cstrike or HLDS executable file for which you
'wish to install Admin Mod in the space below.
'eg. C:\SIERRA\half-life\hl.exe
'
'Click OK when the path has been entered.
'Click Cancel or enter nothing to exit the script.
'</SPECIFY_STRING>
		
'If you wish to uninstall this version Click Yes
'Advanced users who wish to specify a Path Click No'
'To exit this installer Click Cancel"
		
		
'No Halflife game or dedicated server installations found.
'Advanced users who wish to use a full path for the installation click OK.
'Click Cancel To exit.
