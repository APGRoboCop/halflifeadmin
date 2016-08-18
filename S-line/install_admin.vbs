'---------------------------------------------------------------------------------------------------------------------------'
'															    '
' 			  Install script for Admin Mod  v2.50.59							    '
'			  Copyright (C) 2000 Alfred Reynolds (alfred@mazuma.net.au) 				    '
'			  Copyright (C) 2001-2004 David Mart 				    '
'			  $Id: install_admin.vbs,v 1.22 2004/08/02 16:33:23 the_wraith Exp $	
'
' 															    '
'----------------------------------------------------------------------------------------------Declarations-and-setup-------'
option explicit ' enforces explicit variable declaration, don't touch this.
'---------------------------------------------------------------------------------------------------------------------------'
'  To enable debugging set the line below to "Const DEBUGME=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
const DEBUGME = 0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable filesystem changes set the line below to "Const NEUTER=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
const NEUTER = 0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable automatic search and execution of the uninstaller set the line below to "NOUNINSTALL=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
const NOUNINSTALL=0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable desktop shortcut to the manual creation set the line below to "NOSHORTCUT=1" , boolean value: 1 = On, 0 = Off.
'---------------------------------------------------------------------------------------------------------------------------'
const NOSHORTCUT=0

'---------------------------------------------------------------------------------------------------------------------------'
'  To disable prompting and retention of old config files set the line below to "NOKEEPCONFIGS=1" , boolean value: 1 = On, 0 = Off.
'----------------------------------------------------------------------------------------------------------------------------'
const NOKEEPCONFIGS=0
'---------------------------------------------------------------------------------------------------------------------------'
'  Sets the wscript version used in version dependant switches, wscript.version is normal but setting it will force a single set of methods.
'---------------------------------------------------------------------------------------------------------------------------'
dim WSCRIPT_VERSION:WSCRIPT_VERSION=cstr(wscript.version)

'==========================================================================================================================='
'                                YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE !!
'==========================================================================================================================='

'----------------------------------------------------------------------------------------Script-Declarations----------------'
dim IndentValue,InConsoleMode,InCVSMode,WSHShell,FSO,TSO,FSD,TSD,version,Strings,choice,mod_name,OldConfigsPath,files_t,files_f,folders_f,folders_t,Steam,exenames
const ForReading=1,ForWriting=2,ForAppending=8,FILE_ATTRIBUTE_READONLY=1,s="sub",f="function"
const WELCOME=0,REGISTRY=1,REGISTRYERROR=2,SHELLERROR=3,KEEPCONFIGS=4,SHORTEND=5,NOSHORTEND=6,MISSINGFILE=7,NO_UNINSTALL=8,CONFIGWARNING=9,NOCONFIGS=10,STDFOOTER=11,NOMODFOUND=12
class Install: Public Path:public EXEname:public SteamName:end class
class FilePaths_From: public mm_dll:public mm_ini:public am_amxdll:public am_amdll:public am_amcfg:public am_amxbase: public am_docs: public am_plgini:public sub class_initialize: if not InCVSMode then:	mm_ini="\plugins.ini":mm_dll = "dlls\metamod.dll": am_amxdll = "dlls\amx_admin.dll": am_amdll = "dlls\admin_MM.dll": am_amcfg = "config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="docs\AdminMod-Win-Manual.chm": am_plgini="config\plugin.ini": else:mm_ini="\plugins.ini":mm_dll="..\metamod\dlls\metamod.dll":am_amxdll = "dlls\amxdll\release\amx_admin.dll":am_amdll="dlls\adminmod\releaseMM\admin_MM.dll":am_amcfg="config\adminmod.cfg":am_amxbase="scripting\binaries\plugin_base.amx":am_docs="":am_plgini="config\plugin.ini":end if:end sub:end class
'class FilePaths_To: end class
class FolderPaths_From:public am_config:public am_scripts:public am_docs:public am_dlls:public am_samples:public am_tools:public sub class_initialize:am_config = "config":am_docs="docs":am_dlls="dlls":am_scripts="scripting\binaries":am_samples="config\samples":am_tools="tools":end sub:end class
class FolderPaths_To: public steam:public am_config:public am_dlls:public am_scripts:public am_root:public mm_dlls: public mm_root:public addons: public sub class_initialize:am_config ="\addons\adminmod\config":am_dlls="\addons\adminmod\dlls":am_scripts="\addons\adminmod\scripts":am_root="\addons\adminmod":mm_dlls="\addons\metamod\dlls":mm_root="\addons\metamod":steam ="":addons="\addons":end sub:end class
class SteamInstall: Public Install: Public BasePath: Public AppsPath: Public SelectedAddress:: end class
class ChoiceItem: public path: public exename: public description: public issteam: end class
'----------------------------------------------------------------------------------------Script-Initialisation-----------
version= "2.xx.yy"
'----------------------------------------------------------------------------------------Script-Starts-here--------------
Init()
Checkfiles() 	' check to make sure all required Admin Mod files are present before beginning the installation
FindTargets()	' find halflife, dod, cs, steam installations
FindMods()		' find all mods that can be installed to
CheckUninstall()' check if Admin Mod is already present and if so uninstall it.
CheckHost()	    ' verify an installation will work (exceptional fools excluded)
EditLiblistFile(InstallMetamod())' modify the liblist.gam if metamod was installed
InstallAdminmod()' actually install the adminmod files
ManualShortcut()
Close()
'----------------------------------------------------------------------------------------End-of-Script-------------------
public sub Init
	on error resume next
	Set fso = CreateObject("Scripting.FileSystemObject")
	if err.number<>0 then message "The installer was unable to create the ActiveX componant required to"&vbcrlf&"interact with your computers filesystem."&vbcrlf&"This is a problem with your windows scripting host installtion and can be fixed"&vbcrlf&"by reinstalling/updating the Windows Scripting Host."&vbcrlf&vbcrlf&"The windows scripting host can be downloaded free from Microsoft at"&vbcrlf&"http://msdn.microsoft.com/scripting",vbokonly or vbExclamation,"Object creation error":err.clear:wscript.quit
	on error goto 0
	DebugStart() ' self explanatory really
	debugfun "Init"
	InCVSMode    =IsLocalCVSDir() ' determines if this is a CVS install
	InConsoleMode=IsHostCscript() ' determines console or gui mode
	if InConsoleMode then debug "script is running in console mode" else debug "script is running in GUI mode"
	if InCVSMode     then debug "script is running in CVS location mode" else debug "script is running in distribution mode"
	set files_f = new FilePaths_From
	'set files_t = new FilePaths_To
	set folders_f=new FolderPaths_From
	set folders_t=new FolderPaths_To
	set Steam = new SteamInstall
	exenames = array("hl.exe","cstrike.exe","dod.exe","hlds.exe","czero.exe")
	Steam.Install = false
	'message GetString("BETA"),vbokonly,"Admin Mod Beta Installer"
	if InCVSMode then 
		dim MsgRet
		MsgRet = Message( GetString("CVS-MESSAGE"),vbOkCancel or vbExclamation,"Admin Mod CVS Installation")
		if MsgRet = vbcancel then 
			debug "user canceled installation"
			DebugClose()
		end if
	end if
	Strings=array(GetString("WELCOME"),GetString("REGISTRY"),GetString("REGISTRYERROR"),GetString("SHELLERROR"),GetString("KEEPCONFIGS"),GetString("SHORTEND"),GetString("NOSHORTEND"),GetString("MISSINGFILE"),GetString("NOUNINSTALL"),getString("CONFIGWARNING"),getstring("NOCONFIGS"),GetString("STDFOOTER"),GetString("NOMODFOUND")) 'sets up an array of long strings read from the very end of this file.
	debugendf "Init"
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub Close
	debugfun "Close"
	debug "installation complete, objects will be destroyed and the script will exit."
	debugendf "Close"
	Set fso=Nothing: DebugClose()
end sub
'------------------------------------------------------------------------------------------------------------------------	
public sub FindTargets()
	debugfun "FindTargets"
	dim Paths, answer
	answer = Message(Strings(WELCOME),vbokcancel or vbquestion,"Starting Installation")
	If answer<> vbOk Then debug "user canceled installation": DebugClose()
	set Paths = CreateObject("Scripting.Dictionary")
	if not GetHLKeys(Paths) then Search(Paths)
	if Paths.Count>0 then set choice = Selection(Paths) else set choice = specify()
	if choice.issteam then
		choice.path = Steam.AppsPath & "\" & GetSteamAddress(choice)
		choice.path = choice.path & "\" & GetSteamGame(Steam.AppsPath &"\" & Steam.SelectedAddress)
	end if
    choice.path=choice.path&"\"
	debugendf "FindTargets"
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub FindMods()
	debugfun "FindMods"
	dim mods,modstings,folders,answer,x,pathsstring,games
	mods = GetMods(choice.path)  'find the mods and list them for the user to choose
	
	if mods(lbound(mods))= "" then ' we did not find any mod
		debug "no mods found, aborting installation"  
		message Strings(NOMODFOUND), vbOkOnly or vbCritical, "No Mod found"
		DebugClose()
	end if

	redim modstrings(ubound(mods))
	for x=0 to ubound(mods)
		modstrings(x)= "          |--- "&mods(x)
		if x=ubound(mods) then modstrings(x) = replace(modstrings(x),"|-","\")
	next
	debug "prompting user for mod choice"
	answer = NumericChoice( _
		"Enter the NUMBER of the mod you wish to install Admin Mod to."&vbcrlf&"Valid folders are listed below with their numbers on the left hand side.", _
		"Click OK when the folder number has been entered.", _
		modstrings, _
		"Modification choice", _
		"          "&right(left(choice.path,len(choice.path)-1),len(left(choice.path,len(choice.path)-1))-instrrev(left(choice.path,len(choice.path)-1),"\")) _
		)
	if folderexists(choice.path&mods(answer)) then
		mod_name = mods(answer)
	else
		message "er... crap, its all gone wrong. Whats that over there!", vbokonly, "horrible error condition #11456"
		DebugClose()
	end if
	debugendf "FindMods"
end sub
'------------------------------------------------------------------------------------------------------------------------
public sub ManualShortcut()
	debugsub "ManualShortcut"
	if CreateShortcut() then
		debug "displaying end dialogue"&vbcrlf
		message Strings(SHORTEND),vbOkonly,"Installation Completed" 'shortcut created string
	else
		debug "displaying end dialogue"&vbCrlf
		message Strings(NOSHORTEND),vbOkonly,"Installation Completed" 'no shortcut created string
	end if
	debugends "ManualShortcut"
end sub
'------------------------------------------------------------------------------------------------------------------------
public function Selection(byref Paths)
	debugsub "Selection"
	dim answer,invalid,promptstring,promptnumber,selpath,x,itemarray,items
	Steam.Install = false
	debug "selecting on number of located installations: " & Paths.count 
	select case Paths.Count
	case 0
		debug "entered 0 case, prompting user to specify path or quit"
		answer=Message("No Halflife game or dedicated server installations found." & vbcrlf & vbcrlf & "Advanced users who wish to use a full path for the installation click OK."&vbcrlf&"Click Cancel To exit.",vbokcancel or vbquestion,"No Half-Life dedicated server found")
		if answer=vbCancel then debug "chose to quit": DebugClose()
		if answer=vbOk     then debug "No HL.exe, HLDS.exe, DOD.exe or Cstrike.exe found: chose to specify a path"&vbCrlf: set Selection = Specify()
	case 1
		debug "entered 1 case, prompting user to accept, specify path or quit"
		dim temppath,num
		num = cstr(ubound(paths.Items))
		promptstring="Single installation detected." & vbcrlf & vbcrlf
		promptstring=promptstring & Paths.Item(num).description &"(" & Paths.Item(num).path & ")" &vbcrlf
		promptstring = promptstring&vbcrlf&"If you wish to install to this path Click Yes"&vbcrlf&"Advanced users who wish to specify a Path Click No"&vbcrlf&"To exit this installer Click Cancel"
		answer = Message(promptstring,vbyesnocancel or vbquestion,"Choose Installation")
		select case answer
			case vbYes
				debug "user chose to install to """&Paths.Item(num).path&""""
				set Selection = Paths.Item(num)
			case vbNo
				debug "user chose to specify"
				Selection = Specify()
			case vbCancel
				debug "user chose to cancel installation"
				DebugClose()
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
			"Please enter the NUMBER corresponding to the path you wish to install to and then click OK"&vbcrlf&"click Cancel or enter nothing to quit", _
			items, _
			"Choose Installation", _
			"" _
			)
		debug "user entered number " & answer & " which translates to path " & Paths.Item(cstr(answer)).path
		set Selection = Paths.Item(cstr(answer))
	case else
		debug "entered case else, something somewhere has gone horribly wrong" 
		message "entered case else, something somewhere has gone horribly wrong",vbokonly or vberror,"Script error"
		DebugClose()
	end select
	debugendf "Selection"
end function
'------------------------------------------------------------------------------------------------------------------------
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
		DebugClose()
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
		if answer="" then DebugClose()
		if not isnumeric(answer) then
			invalid=true
			debug "user entered non numeric data """ &answer& """"	
		end if
		if not invalid then 
			answer=cint(answer)
			if cint(answer)<1 or cint(answer)>cint(ubound(options)+1) then
				invalid=true
				debug "user entered out of range integer """ & answer &""""
				debug "user entered invalid data, the script will now exit": DebugClose()
			end if				
		end if
	end if
	debug "user chose "&cstr(answer)&" (of 1 to "&cstr(ubound(options)+1)&")"
	NumericChoice = answer-1
	debugendf "NumericChoice"
end function
'------------------------------------------------------------------------------------------------------------------------
public function InstallMetamod
	dim ModifyLiblist,DeferMoveMetamodIni,LiblistCorrect,Liblist_stream,invar
	debugfun "InstallMetamod"
	if not InCVSMode then
		debug "removing any old style metamod installation"
		ModifyLiblist =true
	
		' remove any oldstyle metamod install found
		if fileexists( choice.path & mod_name & "\dlls\metamod.dll")               then deletefile choice.path & mod_name & "\dlls\metamod.dll",true
		if fileexists( choice.path & mod_name & "\metamod.ini") then DeferMoveMetamodIni=true else DeferMoveMetamodIni=false
		if fileexists( choice.path & mod_name & "\AdminMod backup of liblist.gam") then ' if the old backup of liblist.gam is present then restore it.
			deletefile choice.path & mod_name & "\liblist.gam",true
			copyfile   choice.path & mod_name & "\AdminMod backup of liblist.gam",choice.path & mod_name & "\liblist.gam",true
			deletefile choice.path & mod_name & "\AdminMod backup of liblist.gam",true
		end if
		debug "completed old style metamod file removal"
	
		set Liblist_stream = fso.opentextfile(choice.path&mod_name&"\liblist.gam",forreading,false)
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
		if folderexists(choice.path&mod_name&"\addons") and folderexists(choice.path&mod_name&"\addons\metamod") and folderexists(choice.path&mod_name&"\addons\metamod\dlls") and fileexists(choice.path&mod_name&"\addons\metamod\dlls\metamod.dll") then
			if liblistCorrect=false then
				ModifyLiblist=true
				debug "complete new style metamod installtion found with erroneous liblist, this will be fixed"
			 else
				debug "complete new style metamod installtion found"
				ModifyLiblist=false
			end if
		else
			debug "incomplete or no new style metamod installtion found, installing"
			if not folderexists(choice.path & mod_name & folders_t.addons)     then createfolder choice.path & mod_name & folders_t.addons
			if not folderexists(choice.path & mod_name & folders_t.mm_root)    then createfolder choice.path & mod_name & folders_t.mm_root
			if not folderexists(choice.path & mod_name & folders_t.mm_dlls)    then createfolder choice.path & mod_name & folders_t.mm_dlls
			copyfile files_f.mm_dll,choice.path & mod_name & folders_t.mm_dlls & "\",          true
			if DeferMoveMetamodIni then ' at dead_one's insistance, pointless though it may seem.
				copyfile   choice.path & mod_name & "\metamod.ini",choice.path &mod_name & folders_t.mm_root & "\plugins.ini",true
				deletefile choice.path & mod_name & "\metamod.ini",true
			end if
			EditMMPluginsFile choice.path & mod_name & folders_t.mm_root &"\plugins.ini", ""
			ModifyLiblist =true
			debug "completed metamod installation"
		end if
		InstallMetamod=ModifyLiblist
	else
		InstallMetamod=false
	end if	
	debugendf "InstallMetamod"
end function
'------------------------------------------------------------------------------------------------------------------------
public sub InstallAdminmod()
	debugfun "InstallAdminmod"
	debug "starting installation and modification of files"
	' create the new folders if they are not present
	if not folderexists(choice.path & mod_name & folders_t.addons)     then CreateFolder(choice.path & mod_name & folders_t.addons)
	if not folderexists(choice.path & mod_name & folders_t.am_root)    then CreateFolder(choice.path & mod_name & folders_t.am_root)
	if not folderexists(choice.path & mod_name & folders_t.am_config)  then CreateFolder(choice.path & mod_name & folders_t.am_config)
	if not folderexists(choice.path & mod_name & folders_t.am_scripts) then CreateFolder(choice.path & mod_name & folders_t.am_scripts)
	if not folderexists(choice.path & mod_name & folders_t.am_dlls)    then CreateFolder(choice.path & mod_name & folders_t.am_dlls)

	' now install the essential Admin files, if base files are not found then create then, do not overwrite though
	copyfile files_f.am_plgini ,         choice.path & mod_name & folders_t.am_config  & "\", true 'overwrite any present ini file
	copyfile files_f.am_amdll,           choice.path & mod_name & folders_t.am_dlls    & "\", true 'always overwrite the dlls
	copyfile files_f.am_amxdll,          choice.path & mod_name & folders_t.am_dlls    & "\", true 'always overwrite the dlls
	copyfile folders_f.am_scripts&"\*.*",choice.path & mod_name & folders_t.am_scripts & "\", true 'always overwrite amx binaries

	if OldConfigsPath="" then
		copyfile "config\adminmod.cfg",   choice.path & mod_name & folders_t.am_config  & "\", true 'always overwrite the config file
		if fileexists(choice.path & mod_name & folders_t.am_config & "\maps.ini")    = false then CreateTextFile choice.path & mod_name & folders_t.am_config & "\maps.ini",true    ': debug "created maps.ini"
		if fileexists(choice.path & mod_name & folders_t.am_config & "\ips.ini")     = false then CreateTextFile choice.path & mod_name & folders_t.am_config & "\ips.ini",true     ': debug "created ips.ini"
		if fileexists(choice.path & mod_name & folders_t.am_config & "\models.ini")  = false then CreateTextFile choice.path & mod_name & folders_t.am_config & "\models.ini", true ': debug "created models.ini"
		if fileexists(choice.path & mod_name & folders_t.am_config & "\users.ini")   = false then CreateTextFile choice.path & mod_name & folders_t.am_config & "\users.ini",true   ': debug "created users.ini"
		if fileexists(choice.path & mod_name & folders_t.am_config & "\wordlist.txt")= false then CreateTextFile choice.path & mod_name & folders_t.am_config & "\wordlist.txt",true': debug "created wordlist.txt"
	else
		debug "restoring old config files where possible, new files will otherwise be created"
		if fileexists(OldConfigsPath&"\adminmod.cfg") then copyfile OldConfigsPath&"\adminmod.cfg", choice.path & mod_name & folders_t.am_config & "\",true else copyfile "config\adminmod.cfg",   choice.path & mod_name & folders_t.am_config  & "\", true
		if fileexists(OldConfigsPath&"\maps.ini")     then copyfile OldConfigsPath&"\maps.ini",     choice.path & mod_name & folders_t.am_config & "\",true else CreateTextFile choice.path & mod_name & folders_t.am_config & "\maps.ini",    true ': debug "created maps.ini"
		if fileexists(OldConfigsPath&"\ips.ini")      then copyfile OldConfigsPath&"\ips.ini",      choice.path & mod_name & folders_t.am_config & "\",true else CreateTextFile choice.path & mod_name & folders_t.am_config & "\ips.ini",     true ': debug "created ips.ini"
		if fileexists(OldConfigsPath&"\models.ini")   then copyfile OldConfigsPath&"\models.ini",   choice.path & mod_name & folders_t.am_config & "\",true else CreateTextFile choice.path & mod_name & folders_t.am_config & "\models.ini",  true ': debug "created models.ini"
		if fileexists(OldConfigsPath&"\users.ini")    then copyfile OldConfigsPath&"\users.ini",    choice.path & mod_name & folders_t.am_config & "\",true else CreateTextFile choice.path & mod_name & folders_t.am_config & "\users.ini",   true ': debug "created users.ini"
		if fileexists(OldConfigsPath&"\wordlist.txt") then copyfile OldConfigsPath&"\wordlist.txt", choice.path & mod_name & folders_t.am_config & "\",true else CreateTextFile choice.path & mod_name & folders_t.am_config & "\wordlist.txt",true ': debug "created wordlist.txt"
		debug "files restored, """&OldConfigsPath&""" will be deleted"
		deletefolder OldConfigsPath,true
	end if
	EditMMPluginsFile choice.path & mod_name & folders_t.mm_root &"\plugins.ini", right(replace(folders_t.am_dlls,"\","/"), len(folders_t.am_dlls)-1) & "/admin_MM.dll"
	EditConfigFiles()
	debugendf "InstallAdminmod"
end sub
'----------------------------------------------------------------------------------------EditLiblistFile-Sub-----------
public sub EditLiblistFile(ModifyLiblist)
	dim out_lib,in_lib,line,GamedllFound,RO,file
	debugsub "EditLiblistFile"
	GamedllFound=false
	if ModifyLiblist=true then
		set file=fso.getfile(choice.path&mod_name&"\liblist.gam")
		
		If (file.attributes And FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY Then
			debug "liblist.gam Is a read-only file, changing"
			file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
			RO=true
		End If

		set file=nothing

		set in_lib = fso.OpenTextFile(choice.path&mod_name&"\liblist.gam",ForReading,False)

		debug "creating temporary file to replace liblist.gam"
		set out_lib = fso.OpenTextFile("temp.tmp",ForWriting,True)

		do while in_lib.AtEndOfStream<>true
			line = in_lib.readline
			If instr(line,"gamedll")>0 And instr(line,"gamedll_linux")<1 then
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

		if fileexists(choice.path&mod_name&"\AdminMod backup of liblist.gam")=false then
			debug "no previous backup found"
			copyfile choice.path&mod_name&"\liblist.gam", choice.path&mod_name&"\AdminMod backup of liblist.gam",true
		else
			debug "previous backup found"
			deletefile choice.path&mod_name&"\liblist.gam",true
		end if
		copyfile "temp.tmp",choice.path&mod_name&"\liblist.gam",true

		if RO=true Then
			set file=fso.getfile(choice.path&mod_name&"\liblist.gam")
			file.attributes=(file.attributes Or FILE_ATTRIBUTE_READONLY)
			set file=nothing: RO=false
			debug "liblist.gam read-only status restored"
		end if

		deletefile "temp.tmp",true
	else
		debug "no liblist.gam modifications required"
	end if
	
	if choice.issteam then
		debug "testing Liblist.gam readonly status"
		set file=fso.getfile(choice.path&mod_name&"\liblist.gam")
		if (file.attributes and FILE_ATTRIBUTE_READONLY)<>FILE_ATTRIBUTE_READONLY then
			debug "liblist.gam is a not read-only file, changing to prevent steam reverting the file to a standard template"
			file.attributes=(file.attributes or FILE_ATTRIBUTE_READONLY)
		end if
	end if
	debugends "EditLiblistFile"
end sub
'----------------------------------------------------------------------------------------EditMMPluginsFile-Fun-----------
public sub EditMMPluginsFile(pluginsFilePath,entry)
	dim out_lib
	debugsub "EditMMPluginsFile"
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
	debugends "EditMMPluginsFile"
end sub
'----------------------------------------------------------------------------------------EditConfigFile-Fun--------------
public function EditConfigFile(FileName)
	dim execString,in_lib,out_lib,RO,invar,file
	debugsub "EditConfigFile"
	debug "starting "&FileName&" file changes"
	if FileName="listenserver.cfg" and not fileexists(choice.path&mod_name&"\"&FileName) then
		debug "listenserver.cfg was not found, will be created to prevent end user confusion"
		createtextfile choice.path&mod_name&"\"&FileName,false
	end if
	If fileexists(choice.path&mod_name&"\"&FileName)=true Then
		debug "server.cfg file located at """&choice.path&mod_name&"\"&FileName&""""
		Set file =fso.getfile(choice.path&mod_name&"\"&FileName)

		If (file.attributes And FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY Then
			debug FileName&" is a read-only file, changing"
			file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
			RO=True
		End If

		if file.size=0 then 
			debug FileName&" file size is 0 bytes"
			invar=""
		else
			Set out_lib = fso.opentextfile(choice.path&mod_name&"\"&FileName,ForReading,false)
			invar=out_lib.readall
			out_lib.close
			Set out_lib=nothing
			debug "scanning for ""adminmod.cfg"""
		end if

		if instr(1,invar,"adminmod.cfg")=0 then
			debug "no reference to adminmod.cfg found, appending"
			if invar<>"" then
				Set out_lib = fso.opentextfile(choice.path & mod_name & "\" & FileName,forappending,false)
			else
				Set out_lib = fso.opentextfile(choice.path & mod_name & "\" & FileName,forwriting,false)
			end if

			if NEUTER=0 then
				out_lib.WriteBlankLines(2)
				out_lib.write "// Execute the Admin Mod configuration file"&vbCrlf
				execstring=replace(folders_t.am_config & "/adminmod.cfg"&vbCrlf,"\","/")
				execstring=right(execstring, len(execstring)-1)
				execstring="exec "&execstring
				out_lib.write execstring
			end if
			out_lib.close
			Set out_lib = nothing
			debug "file appended and closed"
		else
			debug "found reference to ""adminmod.cfg"" no changes are required"
		end if

		invar=""

		if RO=true then
			file.attributes=(file.attributes Or FILE_ATTRIBUTE_READONLY)
			Set File=nothing
			debug FileName&" read-only status of restored"
			RO=false
		end if
		EditConfigFile=true
	else
		debug FileName&" not found, no changes can be made"
		EditConfigFile=false
	end if
	debug "completed "&FileName&" file changes"
	debugends "EditConfigFile"
end function
'------------------------------------------------------------------------------------------------------------------------
public sub EditConfigFiles()
	debugsub "EditConfigFiles"
	dim Files,FilesCount
	' now edit the server.cfg and listenserver.cfg files to exec the adminmod.cfg file, take account of read only status
	Files=array("listenserver.cfg","server.cfg")
	dim EditedFile
	EditedFile=false
	for FilesCount = 0 to ubound(Files)
		EditedFile = (EditedFile or EditConfigFile(Files(filesCount)))
	next
	if EditedFile=false then
		debug "neither statndard config files could be located to add the adminmod.cfg entry to, the user will be notified"
		message strings(NOCONFIGS),vbokonly or vbExclamation,"No exec line created"
	end if
	debugendf "EditConfigFiles"
end sub
'----------------------------------------------------------------------------------------GetHLKeys-Sub-------------------
public function GetHLKeys(byref Paths)
	Dim ValveHLKey,SolHlKey,SolCSKey,SolHLDSKey,SoftHLKey,SoftCSKey,SoftHLDSKey,SoftDODKey,SolDODKey,SoftSteamKey,SoftCzeroKey,SolCzeroKey
	Dim SteamAppsInstPath
	debugfun "GetHLkeys"
	on error resume next ' handle errors in _this procedure only_
	debug "Creating Wscript.shell object"
	set WSHShell = CreateObject("WScript.Shell") 
	if err.number<>0 then
		err.clear
		on error goto 0
		debug "Could not create Wscript.Shell object. Wscript version is "&wscript.version&"."
		debug "Prompting user to continue or fail installation."
		dim MsgRet
		MsgRet = message(Strings(REGISTRYERROR),vbokcancel or vbExclamation,"Object creation error")
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
		'solcskey="C:\sierra\counter-strike\"
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
		SearchHLKeys Paths,SoftCSKey,   SolCSKey,   "",        "\cstrike.exe",   "CS Retail                "
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
			AddChoice pathcollection,firstkey,exename,description,false
		end if
	else
		if secondkey<>"" then 
			if fileexists(secondkey&exename) then
				AddChoice pathcollection,secondkey,exename,description,false
			end if
		else
			if thirdkey<>"" then
				if fileexists(thirdkey&exename) then
					AddChoice pathcollection,thirdkey,exename,description,false
				end if
			end if
		end if
	end if
	debugends "SearchHLkeys"
end sub
'----------------------------------------------------------------------------------------AddChoice--------------------------------
private sub AddChoice(byref pathcollection,path,exename,description,issteam)
	dim choice
	set choice = new ChoiceItem
	choice.description=description
	choice.exename=exename
	choice.path=path
	choice.issteam=issteam
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
		end if
	next
	set folders=nothing: set subfolders=nothing
	GetMods=mods
	debugendf "GetMods"
end function
'----------------------------------------------------------------------------------------SteamAccount-Searching-Sub---------------
public function GetSteamAddress(byref chosen)
	dim addresses,folders,folder,accfolder,answer
	debugfun "GetSteamAddress"
	redim addresses(0)
	
	set folders=fso.getfolder(Steam.AppsPath)
	for each folder in folders.subfolders
' valve in their infinite wisdom changed things _again_, no more e-mail address as account names, just names.
'		debug "checking folder: " & folder.path&"#"
'		if instr(1,folder.path,"@")>0 then
			debug "found potential account folder: " & folder.path
			if addresses(ubound(addresses))<>"" then redim preserve addresses(ubound(addresses)+1)
			addresses(ubound(addresses))=folder.name
'		else 
'			debug "not an account folder"
'		end if
	next
	set folders=nothing
	
	select case ubound(addresses)
		case 0:
			if addresses(0)<>"" then
				answer=message("Single account folder located." &vbcrlf&vbcrlf& addresses(0) &vbcrlf&vbcrlf&"If you wish to use this address click OK."&vbcrlf&"Click Cancel To exit.",vbokcancel or vbquestion,"Single Account found")
				if answer=vbCancel then debug "chose to quit": DebugClose()
				if answer=vbOk     then accfolder=addresses(0): debug "user chose to use the account: " & accfolder
			else
				message "The installer could not find any Steam account folder it can install Admin mod to."&vbcrlf&vbcrlf&"It will now exit",vbokonly,"No Mods Found": DebugClose()
			end if
		case else
			answer = NumericChoice( _
				"Enter the NUMBER from the left hand side for the account that you wish to use Admin Mod with.", _
				strings(STDFOOTER), _
				addresses, _
				"Choose STEAM Account", _
				"" _
				)
			accfolder = addresses(answer)	
	end select

	Steam.SelectedAddress = accfolder
	debug "accFolder is " & accfolder
	debug "Steam Installation path is now" & Steam.AppsPath &"\" & Steam.SelectedAddress
	GetSteamAddress=accfolder
	debugendf "GetSteamAddress"
end function
'----------------------------------------------------------------------------------------SteamAccount-Request-Sub-----------------
public function GetSteamGame(gamepath)
	dim games,gamestrings,folders,folder,subfolder,subfolders,exename,answer,x
	debugfun "GetSteamGames"
	debug "Path is " & gamepath
	redim games(0)
	set folders=fso.getfolder(gamepath)
	for each folder in folders.subfolders
		debug "checking folder: " & folder.path&"#"
		for each exename in exenames
			if fso.fileexists(folder.path&"\"&exename) then
				debug "found "& exename &" in: " & folder.path
				if games(ubound(games))<>"" then redim preserve games(ubound(games)+1)
				games(ubound(games))=folder.name
				'should we exit here to prevent listing a directory twice when hl.exe AND hlds.exe are present?
				exit for
			end if
		next
	next
	set folders=nothing: set subfolders=nothing

	redim gamestrings(ubound(games))
	for x=0 to ubound(games)
		gamestrings(x)= "          |--- "&games(x)&vbcrlf
		if x=ubound(games) then gamestrings(x) = replace(gamestrings(x),"|-","\")
	next

	answer = NumericChoice( _
		"Enter the NUMBER of the Steam game you wish to install Admin Mod to.", _
		strings(STDFOOTER), _
		gamestrings, _
		"Choose the Steam Mod", _
		steam.SelectedAddress _
		)

	debug "user chose: " & games(answer)
	GetSteamGame = games(answer)
	debugendf "GetSteamGame"
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
'----------------------------------------------------------------------------------------Full-Path-Secification-Sub------
public function Specify()
	debugfun "Specify"
	dim localPath,newchoice,exename
	debug "prompting use to supply a valid fully qualified path"
	localPath=input("Specify the full path to the HL, Cstrike or HLDS executable file for which you" & vbcrlf & "wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
	if localPath="" then debug "user chose to cancel or entered a null string, quitting": DebugClose()
	if fileexists(localPath)<>true Or ( lcase(right(localpath,8))<>"hlds.exe" and lcase(right(localpath,6))<>"hl.exe" and lcase(right(localpath,11))<>"cstrike.exe" and lcase( right(localpath,7) )<> "dod.exe" ) Then
		debug "incorrect but not null path entered, second prompting"
		localPath=Input("Incorrect Path." & vbCrlf & vbCrlf & "Specify the full path to the HL, Cstrike or HLDS executable file for which you wish to install Admin Mod in the space below."&vbCrlf&"eg. C:\SIERRA\half-life\hl.exe"&vbCrlf&vbCrlf&"Click OK when the path has been entered."&vbCrlf&vbCrlf&"Note: a null length path will be assumed to mean you wish to exit.","Specify a full path")
		if (not fileexists(localPath)) and (lcase(right(localPath,8))<>"hlds.exe" or lcase(right(localPath,6))<>"hl.exe" or lcase(right(localPath,11))<> "cstrike.exe" or lcase(right(localPath,7))<> "dod.exe")  then debug "second incorrect path, the user should find the correct path manually or learn to type, quitting": DebugClose()
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
				exit for
			end if
		end if
	next
	debug "resultant path = " &localPath
	set specify = newchoice
	Steam.Install = false
	debugendf "Specify"
end function
'----------------------------------------------------------------------------------------Create-desktop-shortcut---------
Private function CreateShortcut
	debugfun "CreateShortcut"
	dim answer,DocPath,MyShortcut, DesktopPath
	if NOSHORTCUT<>0 or InCVSMode then
		debug "shortcut creation is disabled in this script"
		CreateShortcut=false
	else
		debugsub "CreateShortcut"
		select case WSCRIPT_VERSION
		case "5.1"
			debug "Windows scripting host version 5.1 detected, too low to use shortcut object."
			CreateShortcut=false
		case "5.5","5.6" 
			answer=message("Do you wish to create a shortcut to the Admin mod Manual on your desktop?",vbYesNo or vbquestion,"Create Manual Shortcut?")
			if answer=vbyes then
				debug "user chose to create a desktop shortcut to the manual."
				debug "creating shell object"
				on error resume next ' allow soft errors
				set WSHShell = CreateObject("WScript.Shell")
				if err.number<>0 then
					debug "unable to create shell object despite correct WSH version, exiting"
					message Strings(REGISTRYERROR),vbokonly or vbExclamation,"Shell Object Error"
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
	debugendf "CreateShortcut"
end function
'----------------------------------------------------------------------------------------Check-for-required-files--------
public sub CheckFiles
	debugsub "CheckFiles"

	if not FolderExists(folders_f.am_dlls)    then errorexit("folder (\"&folders_f.am_dlls&")")
	if not FolderExists(folders_f.am_config)  then errorexit("folder (\"&folders_f.am_config&")")
	if not fileexists  (files_f.am_amdll)     then errorexit("dll ("&files_f.am_amdll&")")
	if not fileexists  (files_f.am_amxdll)    then errorexit("dll ("&files_f.am_amxdll&")")
	if not fileexists  (files_f.am_amcfg)     then errorexit("file ("&files_f.am_amcfg&")")
	if not fileexists  (files_f.am_plgini)    then errorexit("file ("&files_f.am_plgini&")")
	if not FolderExists(folders_f.am_config) then
		Message replace(Strings(MISSINGFILE),"%REPLACEME%","folder "&folders_f.am_config),vbOkonly or vbCritical,"Installation could not start"
		DebugClose()
	end if
	if not fileexists(files_f.am_amxbase) then
		Message replace(Strings(MISSINGFILE),"%REPLACEME%","file "&files_f.am_amxbase),vbOkonly or vbCritical,"Installation could not start"
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
	Message "The installer could not find an essential " & errorfile & "."& vbcrlf & vbcrlf & "This is usually caused by Admin Mod being unzipped incorrectly." & vbcrlf & "Please try again, making sure to maintain the directory structure." & vbcrlf & vbcrlf & "This installer will now exit.",vbokonly or vbCritical,"Installation could not start"
	DebugClose()
end Sub
'------------------------------------------------------------------------------------------------------------------------
public sub CheckHost()
	debugsub "CheckHost"
	debug "Locating host mod essentials"
	If fileexists(choice.path&mod_name&"\liblist.gam")<>True Then
		if mod_name="" then
			DebugClose()
		Else
			Message "The installer could not find the liblist.gam file for your mod."&vbcrlf&"This means you may not have specified a valid modification folder."&vbCrlf&vbCrlf&"Retail Counter-Strike sometimes cannot be properly identified by this installer,"&vbcrlf&"if you find that you cannot get this script to work then please refer to"&vbcrlf&"the manual installation instruction in the manual."&vbCrlf&vbCrlf&"This installer will now Exit.",vbOkonly or vbExclamation,"Exiting"
			DebugClose()
		end if
	end if

	if not choice.issteam then
		If folderexists(choice.path&mod_name&"\dlls")<>true then
			message "The installer was unable to find the dlls directory of the mod you specified."&vbcrlf&"This installer will now exit.",vbokonly or vbExclamation,"Exiting"
			debug "Unable to locate mod dlls folder": DebugClose()
		end if
	end if
	debugends "CheckHost"
end sub
'----------------------------------------------------------------------------------------find-the-uninstaller------------
public sub CheckUninstall()
	debugsub "CheckUninstall"

	dim OldDir,NewDir,UninstallPath,ExitLoopNow,FS2,TS2,Return,Shell,invar,answer,OldConfigsPath
	if NOUNINSTALL=1 then 
		debug "CheckUninstall has been disabled in this script"
		if fso.fileexists(choice.path&mod_name&"\dlls\admin_mm.dll") or fso.fileexists(choice.path&mod_name&"\dlls\amx_admin.dll") then
			message "CheckUninstall has been disabled in your script but a previous Admin Mod installation has been detected in your chosen installation location. The script will not continue."&vbcrlf&vbcrlf&"Pleaase remove the Admin Mod files either with the uninstaller or manually before attempting to install to this location.",vbokonly or vbExclamation,"Previous install detected"
			DebugClose()
		end if
		exit sub
	end if

	debug "creating filesystem and textstream objects"
	set FS2=createobject("scripting.filesystemobject")
	set TS2=fs2.opentextfile(choice.path&mod_name&"\liblist.gam",ForReading,false)
	ExitLoopNow=False
	do while TS2.atendofstream<>true 
		if ExitLoopNow=True then Exit Do
		invar=trim(TS2.readline)
		if len(invar)>2 then
			if instr(1,invar,"gamedll ")>0 And instr(1,invar,"metamod.dll")>0 And left(invar,2)<>"//" then
				debug "found metamod in liblist.gam and dlls folder, searching for Admin Mod specific files"
				indent()
				if fileexists(choice.path&mod_name&"\plugin.ini") or fileexists(choice.path&mod_name&"\adminmod.cfg") or fileexists(choice.path&mod_name&"\users.ini") or fileexists(choice.path&mod_name&"\AdminMod Backup of Liblist.gam") or fileexists(choice.path&mod_name&"\dlls\admin_mm.dll") or fileexists(choice.path&mod_name&"\dlls\amx_admin.dll") or folderexists(choice.path&mod_name&"\addons\adminmod") then
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
	set Shell=Nothing: set FS2=Nothing: set TS2=Nothing

	if Uninstallpath<>"" then
		select case WSCRIPT_VERSION
		case "5.1"
			debug "WSH version is too low to use the run method. Wscript version is " &WSCRIPT_VERSION
			debug "unable to run uninstall_admin.vbs so uninstallation cannot be performed automatically"
			Message Strings(8),vbOkonly or vbExclamation,"WSH version too low to automatically uninstall"
			DebugClose()
		case "5.5","5.6"
			if NOKEEPCONFIGS=0 then
				debug  "prompting user to retain old config files or create anew"
				answer=message(strings(KEEPCONFIGS),vbYesNo or vbQuestion,"Retain old config files?")
				if answer = vbyes then
					debug  "user chose to retain their old files, on their own head be it"
					OldConfigsPath = choice.path&mod_name&"\Old AdminMod Config Files"
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
				message Strings(REGISTRYERROR),vbokonly or vbCritical,"Shell Object Error"
				err.clear: DebugClose()
			else
				on error goto 0 ' return to hard errors
				debug "uninstaller was located at """&UninstallPath&""" it will now be run"
				debug "changing working directory for uninstallation"
				OldDir = shell.currentdirectory					
				if NewDir<>"" then shell.currentdirectory=shell.currentdirectory&"\tools"
				debug "executing "&UninstallPath&" "&chr(34)&choice.path&mod_name&chr(34)&" "&chr(34)&OldConfigsPath&chr(34)&" "&cstr(choice.issteam)
				Return= shell.run (UninstallPath&" "&chr(34)&choice.path&mod_name&chr(34)&" "&chr(34)&OldConfigsPath&chr(34)&" "&cstr(choice.issteam),,true)
				debug "uninstaller returned code "&return
				debug "restoring working directory for installation to continue"
				shell.currentdirectory=OldDir
				debug "displaying upgrade warning about (listen)server.cfg file cvars"
				message strings(CONFIGWARNING),vbokonly or vbInformation,"Upgrade installation advisory"
			end if

			if return<>0 then
				Message "Admin Mod was unable to uninstall your previous version of Admin Mod completely. Please refer to the release notes before attempting to continue."&"We apologise for the inconvienience."&vbCrlf&vbCrlf&"This installer will now exit.",vbOkonly or vbInformation,"Exiting"
				DebugClose()
			end If
		end select
	else
		if ExitLoopNow=true then
			debug "unable to locate uninstall_admin.vbs so uninstallation cannot be performed automatically"
			Message "You must uninstall your previous version of Admin Mod before you install Admin Mod "&version&"."&vbCrlf&vbCrlf&"This installer will now exit.",vbOkonly or vbExclamation,"Exiting"
			DebugClose()
		else
			debug "no previous installations located, continuing"
		end if
	end if
	debugends "CheckUninstall"
end sub
'----------------------------------------------------------------------------------------InputBox-Abstraction------------
public function input(Prompt,Title)
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
public function IsHostCscript()
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
public function IsLocalCVSDir()
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
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",vbOkOnly or vbCritical,"Internal script error"
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
		message "The GetString function failed to locate """&StringName&""" in the script file."&vbcrlf&vbcrlf&"If you have not tampered with this script and require help please go to the"&vbcrlf&"Admin Mod forums which can be found by following the help links from the"&vbcrlf&"www.adminmod.org front page."&vbcrlf&vbcrlf&"The installer will now exit.",vbOkOnly or vbCritical,"Internal script error"
		DebugClose()
	end if
end function
'------------------------------------------------------------------------------------------------------------------------
private sub indent
	IndentValue=IndentValue+1
end sub
'------------------------------------------------------------------------------------------------------------------------
private sub unindent
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
'public function OpenTextFile(filename,IOMode,OverWrite)
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
 			if NEUTER=0 then
				set file=fso.getfile(ToPath & right(frompath,len(frompath) - instrrev(frompath,"\")))
				if (file.attributes and FILE_ATTRIBUTE_READONLY)=FILE_ATTRIBUTE_READONLY then
					debug ToPath&" Is a read-only file, changing"
					file.attributes= (file.attributes Xor FILE_ATTRIBUTE_READONLY)
				end if
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
	message Errorstring,vbokonly or vbCritical,"Error"
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
		set TSD = FSD.createtextfile("install_admin.log",True)
		on error goto 0
		if err.number<>0 then
			Message "The installer has debug mode enabled. During initialisation the installer was unable to create the ""install_admin.log"" file in the installer directory. This problem prevents the installer from running."&vbcrlf&"Please run the installer again when it is possible to write to this file."&vbcrlf&vbcrlf&"This installer will now exit",vbokonly or vbExclamation,"Fatal error creating debug log"
			wscript.quit
		end if
		debug "debug file ""install_admin.log"" created sucessfully"&vbCrlf&"debug Filesystem object created sucessfully"&vbcrlf
		if DEBUGME>1 then debug "options list":debug "DEBUGME = " & DEBUGME:debug "NEUTER = "&NEUTER:debug "NOUNINSTALL = " & NOUNINSTALL:debug "NOSHORTCUT = "&NOSHORTCUT:debug "end of options"&vbcrlf
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
	wscript.quit
end sub
'----------------------------------------------------------------------------------------debug-writing-------------------
public sub debug(DebugString)
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

'public sub DebugSub(name,typestr)
'	debug "beginning " & name & " " & typestr
'	Indent()
'end sub

'public sub DebugEnd(name,typestr)
'	unindent()
'	debug "completed " & name & " " & typestr & vbcrlf
'end sub

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
'Click Cancel to disallow the Admin Mod installer from using your registry.(Advanced)
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
'This is a problem with your Windows Scripting Host installation and can be
'fixed by reinstalling/updating the Windows Scripting Host.
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
'The installer could not find an essential item (%REPLACEME%).
'
'Before you install Admin Mod you must run compile_all.bat To produce the
'compiled scripts Admin Mod uses To provide it's functionality. Compile_all.bat
'can be found in the \scripting\examples directory. Please run it and then
'execute this installation script again.
'</MISSINGFILE>
'<KEEPCONFIGS>
'In order to install Admin Mod %VERSION% the previous version will first be
'removed.
'
'Do you with to retain the configuration files (adminmod.cfg, wordlist.txt, 
'models.ini, etc...) from your previous installation? If so please be aware that
'if the previous version of Admin Mod installed is a version before %VERSION%
'then the adminmod.cfg file may be missing new and possibly important cvar
'entries. It is strongly suggested that you only retain config files if you are
'certain that they are appropriate to the version of Admin Mod being installed.
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
'script, or you can upgrade your scripting host installation or you can
'uninstall your previous version of Admin Mod manually.
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
'needs only a single exec line to be present in the normal (listen)server.cfg.
'
'if you are upgrading from a version before 2.50.50 then you will need to 
'manually remove all adminmod settings apart from the newly appended exec line
'at the end of the (listen)server.cfg file. 
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
'any errors or problems should be reported to Wraith, in the Admin mod Win32 
'forum.
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
'<NOCONFIGS>
'The installer was unable to locate either listenserver.cfg or server.cfg.
'These files would normally be appended with an exec entry which causes the
'adminmod.cfg to be executed when the server runs, this will not be the case
'on this server.
'
'You will need to manually add an exec entry (details of this can be found in the
'manual installation section in the supplied documentation) to a config file which
'you know will be executed each time the server or a map loads in order to be sure
'that the Admin Mod settings you specify are used rather than default values.
'</NOCONFIGS>
'<NOMODFOUND>
'The installer was unable to locate a Mod in the game directory you chose.
'It can not continue installing Admin Mod and will now exit.
'
'This is often caused on Steam servers when the liblist.game file is missing.
'This file holds vital information needed to install Admin Mod for you. You
'will have to make sure that a liblist.gam file exists with the correct information
'in the Mod directory before running this installer again.
'</NOMODFOUND>
'<STDFOOTER>
'Click Ok or press Enter to continue
'Click Cancel or enter no data to exit the installer.
'</STDFOOTER>
'<CVS-MESSAGE>
'You are running the installer from the source distribution of Admin Mod.
'Make sure that you have all binaries compiled successfully before you
'continue to run this installer. 
'
'This installer will only install the Admin Mod files when you run
'it from the source distribution of Admin Mod. Please make sure you
'have installed Metamod successfully prior to installing Admin Mod
'with this installer.
'</CVS-MESSAGE>
