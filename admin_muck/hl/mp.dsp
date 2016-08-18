# Microsoft Developer Studio Project File - Name="mp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mp - Win32 Debug MM
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp.mak" CFG="mp - Win32 Debug MM"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 MYSQL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Release MM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug MM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 MySQL MM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug MySQL MM" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\Release\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\Release\admin.dll" /libpath:"..\amxdll\Release"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\mp___Win"
# PROP BASE Intermediate_Dir ".\mp___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir ".\debug\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\engine" /i "..\..\dlls" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\admin.def" /out:".\Debug\admin_D.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\mp___Win"
# PROP BASE Intermediate_Dir ".\mp___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Profile"
# PROP Intermediate_Dir ".\Profile\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Od /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /I "D:\temp\metamod-1.0-rc1\metamod" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D strncasecmp=strnicmp /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\mp.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\admin.def" /out:".\Profilemp/admin.dll"

!ELSEIF  "$(CFG)" == "mp - Win32 MYSQL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_MYSQL"
# PROP BASE Intermediate_Dir "mp___Win32_MYSQL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MYSQL"
# PROP Intermediate_Dir "MYSQL\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\MySQL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Releasemp/admin.bsc"
# ADD BSC32 /nologo /o".\MYSQL\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\admin.def" /out:".\Releasemp/admin.dll" /libpath:"E:\sdk2.0\SDK\SourceCode\new_admin\amxdll\Release"
# SUBTRACT BASE LINK32 /pdb:none /map /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL\admin.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT LINK32 /map /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "mp - Win32 Release MM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_Release_MM"
# PROP BASE Intermediate_Dir "mp___Win32_Release_MM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMM"
# PROP Intermediate_Dir "ReleaseMM\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /I "D:\temp\metamod-1.0-rc2\metamod" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /Zi /O2 /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /I "..\..\metamod\metamod" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Releasemp/admin.bsc"
# ADD BSC32 /nologo /o".\ReleaseMM\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\admin.def" /out:".\Releasemp/admin.dll" /libpath:"E:\sdk2.0\SDK\SourceCode\new_admin\amxdll\Release"
# SUBTRACT BASE LINK32 /profile /map /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\ReleaseMM\admin_MM.dll" /libpath:"..\amxdll\Release"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mp___Win32_Debug_MM"
# PROP BASE Intermediate_Dir "mp___Win32_Debug_MM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMM"
# PROP Intermediate_Dir "DebugMM\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /I "D:\temp\metamod-1.0-rc1\metamod" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /FR /FD /c
# SUBTRACT BASE CPP /X /u
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\engine" /I "..\..\pm_shared" /I "..\..\common" /I "..\..\dlls" /I "..\..\metamod\metamod" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_METAMOD" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\engine" /i "..\..\dlls" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\engine" /i "..\..\dlls" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /implib:".\Debug\mp.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:"admin.def" /out:"DebugMM\adminMM_D.dll" /implib:".\Debug\mp.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL MM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_MySQL_MM"
# PROP BASE Intermediate_Dir "mp___Win32_MySQL_MM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MySQL_MM"
# PROP Intermediate_Dir "MySQL_MM\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\MySQL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\metamod\metamod" /I "c:\MySQL\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\MYSQL\admin.bsc"
# ADD BSC32 /nologo /o".\MYSQL\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL\admin.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT BASE LINK32 /map /debug /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib c:\mysql\lib\opt\mysqlclient.lib c:\mysql\lib\opt\libmysql.lib c:\mysql\lib\opt\zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL_MM\admin_MM.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT LINK32 /map /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL MM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mp___Win32_Debug_MySQL_MM"
# PROP BASE Intermediate_Dir "mp___Win32_Debug_MySQL_MM"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_MySQL_MM"
# PROP Intermediate_Dir "Debug_MySQL_MM\obj"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\metamod\metamod" /I "..\..\MySQL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Od /I "..\..\engine" /I "..\..\common" /I "..\..\pm_shared" /I "..\..\dlls" /I "..\..\metamod\metamod" /I "..\..\MySQL" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "HAVE_CONFIG_H" /D "USE_MYSQL" /D "_WIN32_" /D "USE_METAMOD" /D "_DEBUG" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# SUBTRACT BASE MTL /mktyplib203
# ADD MTL /nologo /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\MYSQL\admin.bsc"
# ADD BSC32 /nologo /o".\MYSQL\admin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\admin.def" /out:".\MYSQL_MM\admin_MM.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT BASE LINK32 /map /debug /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mysqlclient.lib libmysql.lib zlib.lib Ws2_32.lib /nologo /subsystem:windows /dll /pdb:none /debug /debugtype:both /machine:I386 /def:".\admin.def" /out:".\debug_MYSQL_MM\admin_MM.dll" /libpath:"..\amxdll\Release" /libpath:"..\..\MySQL\opt"
# SUBTRACT LINK32 /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "mp - Win32 Release"
# Name "mp - Win32 Debug"
# Name "mp - Win32 Profile"
# Name "mp - Win32 MYSQL"
# Name "mp - Win32 Release MM"
# Name "mp - Win32 Debug MM"
# Name "mp - Win32 MySQL MM"
# Name "mp - Win32 Debug MySQL MM"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\admin_commands.cpp
# End Source File
# Begin Source File

SOURCE=.\admin_mod.cpp
# End Source File
# Begin Source File

SOURCE=.\amutil.h
# End Source File
# Begin Source File

SOURCE=.\amx.h
# End Source File
# Begin Source File

SOURCE=.\bfixed.cpp
# End Source File
# Begin Source File

SOURCE=.\cbase.cpp
# End Source File
# Begin Source File

SOURCE=.\CLinkList.cpp
# End Source File
# Begin Source File

SOURCE=.\CLinkList.h
# End Source File
# Begin Source File

SOURCE=.\CPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\CPlugin.h
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp

!IF  "$(CFG)" == "mp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

!ELSEIF  "$(CFG)" == "mp - Win32 MYSQL"

!ELSEIF  "$(CFG)" == "mp - Win32 Release MM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL MM"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL MM"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex.c
# End Source File
# Begin Source File

SOURCE=.\regex.h
# End Source File
# Begin Source File

SOURCE=.\statics.cpp
# End Source File
# Begin Source File

SOURCE=.\statics.h
# End Source File
# Begin Source File

SOURCE=.\sutils.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\users.cpp

!IF  "$(CFG)" == "mp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

!ELSEIF  "$(CFG)" == "mp - Win32 MYSQL"

!ELSEIF  "$(CFG)" == "mp - Win32 Release MM"

# SUBTRACT BASE CPP /D "USE_METAMOD"
# SUBTRACT CPP /D "USE_METAMOD"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MM"

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL MM"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL MM"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\users.h
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\adminmod.rc

!IF  "$(CFG)" == "mp - Win32 Release"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0xc09 /d "NO_METAMOD"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0xc09 /d "NO_METAMOD_DEBUG"

!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

!ELSEIF  "$(CFG)" == "mp - Win32 MYSQL"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407 /d "MYSQL"

!ELSEIF  "$(CFG)" == "mp - Win32 Release MM"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0xc09 /d "USE_METAMOD"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MM"

# ADD BASE RSC /l 0x407
# ADD RSC /l 0xc09 /d "USE_METAMOD_DEBUG"

!ELSEIF  "$(CFG)" == "mp - Win32 MySQL MM"

# PROP Intermediate_Dir "MySQL_MM"
# ADD BASE RSC /l 0x407 /d "MYSQL"
# ADD RSC /l 0x407 /d "MYSQL_MM"

!ELSEIF  "$(CFG)" == "mp - Win32 Debug MySQL MM"

# PROP BASE Intermediate_Dir "MySQL_MM"
# PROP Intermediate_Dir "MySQL_MM"
# ADD BASE RSC /l 0x407 /d "MYSQL_MM"
# ADD RSC /l 0x407 /d "MYSQL_MM"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
