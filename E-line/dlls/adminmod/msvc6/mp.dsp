# Microsoft Developer Studio Project File - Name="mp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp.mak" CFG="mp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 MySQL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug MySQL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\mp___Win"
# PROP BASE Intermediate_Dir ".\mp___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Profile"
# PROP Intermediate_Dir ".\Profile\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /Od /I "..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\metamod\metamod" /I "..\amxdll" /I "..\..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\..\metamod\metamod" /I "..\..\amxdll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D strncasecmp=strnicmp /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NO_METAMOD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\admin.def" /out:".\Profilemp/admin.dll"

!ELSEIF  "$(CFG)" == "mp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_Release"
# PROP BASE Intermediate_Dir "mp___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\..\metamod\metamod" /I "..\..\amxdll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "USE_METAMOD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Release/admin.bsc"
# ADD BSC32 /nologo /o".\Release\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\admin.def" /out:".\Releasemp/admin.dll"
# SUBTRACT BASE LINK32 /profile /map /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\Release\admin_MM.dll" /libpath:"..\amxdll\msvc6\Release"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mp___Win32_Debug"
# PROP BASE Intermediate_Dir "mp___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /FR /FD /c
# SUBTRACT BASE CPP /X /u
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\..\metamod\metamod" /I "..\..\amxdll" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /D "USE_METAMOD_DEBUG" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\engine" /i "..\..\dlls" /d "_DEBUG"
# ADD RSC /l 0x409 /d "USE_METAMOD_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:"admin.def" /out:"Debug\adminMM_D.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_MySQL"
# PROP BASE Intermediate_Dir "mp___Win32_MySQL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MySQL"
# PROP Intermediate_Dir "MySQL\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\MySQL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\..\..\MySQL" /I "..\..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\..\metamod\metamod" /I "..\..\amxdll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "MYSQL_MM"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\MYSQL\admin.bsc"
# ADD BSC32 /nologo /o".\MYSQL\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL\admin.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT BASE LINK32 /map /debug /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL\admin_MM_mysql.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\..\..\MySQL\opt"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_Debug_MySQL"
# PROP BASE Intermediate_Dir "mp___Win32_Debug_MySQL"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_MySQL"
# PROP Intermediate_Dir "Debug_MySQL\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\metamod\metamod" /I "..\..\MySQL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /Od /I "..\..\..\..\MySQL" /I "..\..\..\..\hlsdk\multiplayer source\engine" /I "..\..\..\..\hlsdk\multiplayer source\common" /I "..\..\..\..\hlsdk\multiplayer source\pm_shared" /I "..\..\..\..\hlsdk\multiplayer source\dlls" /I "..\..\..\..\metamod\metamod" /I "..\..\amxdll" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /D "_DEBUG" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "MYSQL_MM"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Debug_MYSQL\admin.bsc"
# ADD BSC32 /nologo /o".\Debug_MYSQL\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL_MM\admin_MM.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT BASE LINK32 /map /debug /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /debug /debugtype:both /machine:I386 /nodefaultlib:"libcmtd.lib" /def:".\admin.def" /out:".\debug_MYSQL_MM\admin_MM.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\..\..\MySQL\debug"

!ENDIF 

# Begin Target

# Name "mp - Win32 Release"
# Name "mp - Win32 Debug"
# Name "mp - Win32 MySQL"
# Name "mp - Win32 Debug MySQL"
# Name "mp - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\admin_commands.cpp
# End Source File
# Begin Source File

SOURCE=..\admin_mod.cpp
# End Source File
# Begin Source File

SOURCE=..\amlibc.c
# End Source File
# Begin Source File

SOURCE=..\amutil.cpp
# End Source File
# Begin Source File

SOURCE=..\authid.cpp
# End Source File
# Begin Source File

SOURCE=..\builtin_commands.cpp
# End Source File
# Begin Source File

SOURCE=..\cbase.cpp
# End Source File
# Begin Source File

SOURCE=..\cevent.cpp
# End Source File
# Begin Source File

SOURCE=..\ceventmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\CLinkList.cpp
# End Source File
# Begin Source File

SOURCE=..\commandhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\CPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\dll.cpp
# End Source File
# Begin Source File

SOURCE=..\events.cpp
# End Source File
# Begin Source File

SOURCE=..\events_cstrike.cpp
# End Source File
# Begin Source File

SOURCE=..\events_valve.cpp
# End Source File
# Begin Source File

SOURCE=..\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\h_export.cpp
# End Source File
# Begin Source File

SOURCE=..\md5.c
# End Source File
# Begin Source File

SOURCE=..\regex.c
# End Source File
# Begin Source File

SOURCE=..\statics.cpp
# End Source File
# Begin Source File

SOURCE=..\sutils.cpp
# End Source File
# Begin Source File

SOURCE=..\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\users.cpp
# End Source File
# Begin Source File

SOURCE=..\util.cpp
# End Source File
# Begin Source File

SOURCE=..\version.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\adminmod.rc

!IF  "$(CFG)" == "mp - Win32 Profile"

!ELSEIF  "$(CFG)" == "mp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\amlibc.h
# End Source File
# Begin Source File

SOURCE=..\amutil.h
# End Source File
# Begin Source File

SOURCE=..\authid.h
# End Source File
# Begin Source File

SOURCE=..\builtin_commands.h
# End Source File
# Begin Source File

SOURCE=..\cevent.h
# End Source File
# Begin Source File

SOURCE=..\ceventmanager.h
# End Source File
# Begin Source File

SOURCE=..\CLinkList.h
# End Source File
# Begin Source File

SOURCE=..\commandhandler.h
# End Source File
# Begin Source File

SOURCE=..\config.h
# End Source File
# Begin Source File

SOURCE=..\CPlugin.h
# End Source File
# Begin Source File

SOURCE=..\events_cstrike.h
# End Source File
# Begin Source File

SOURCE=..\events_valve.h
# End Source File
# Begin Source File

SOURCE=..\extdll.h
# End Source File
# Begin Source File

SOURCE=..\fileio.h
# End Source File
# Begin Source File

SOURCE=..\gnuregex.h
# End Source File
# Begin Source File

SOURCE=..\md5.h
# End Source File
# Begin Source File

SOURCE=..\osdefs.h
# End Source File
# Begin Source File

SOURCE=..\regex.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\sclinux.h
# End Source File
# Begin Source File

SOURCE=..\statics.h
# End Source File
# Begin Source File

SOURCE=..\timer.h
# End Source File
# Begin Source File

SOURCE=..\users.h
# End Source File
# Begin Source File

SOURCE=..\util.h
# End Source File
# Begin Source File

SOURCE=..\version.h
# End Source File
# End Group
# End Target
# End Project
