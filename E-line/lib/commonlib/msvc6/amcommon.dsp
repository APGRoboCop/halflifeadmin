# Microsoft Developer Studio Project File - Name="amcommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=amcommon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "amcommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amcommon.mak" CFG="amcommon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amcommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "amcommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "amcommon - Win32 ReleaseMySQL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "amcommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W2 /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "amcommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "amcommon - Win32 ReleaseMySQL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "amcommon___Win32_ReleaseMySQL"
# PROP BASE Intermediate_Dir "amcommon___Win32_ReleaseMySQL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMySQL"
# PROP Intermediate_Dir "ReleaseMySQL\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W2 /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W2 /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "amcommon - Win32 Release"
# Name "amcommon - Win32 Debug"
# Name "amcommon - Win32 ReleaseMySQL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\md5.c
# End Source File
# Begin Source File

SOURCE=..\src\my_getopt.c
# End Source File
# Begin Source File

SOURCE=..\src\ufc_crypt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\crypt.h

!IF  "$(CFG)" == "amcommon - Win32 Release"

# PROP Intermediate_Dir "Release\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 Debug"

# PROP Intermediate_Dir "Debug\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 ReleaseMySQL"

# PROP BASE Intermediate_Dir "Release\obj"
# PROP Intermediate_Dir "Release\obj"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\include\getopt.h

!IF  "$(CFG)" == "amcommon - Win32 Release"

# PROP Intermediate_Dir "Release\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 Debug"

# PROP Intermediate_Dir "Debug\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 ReleaseMySQL"

# PROP BASE Intermediate_Dir "Release\obj"
# PROP Intermediate_Dir "Release\obj"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\include\md5.h

!IF  "$(CFG)" == "amcommon - Win32 Release"

# PROP Intermediate_Dir "Release\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 Debug"

# PROP Intermediate_Dir "Debug\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 ReleaseMySQL"

# PROP BASE Intermediate_Dir "Release\obj"
# PROP Intermediate_Dir "Release\obj"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\include\my_getopt.h

!IF  "$(CFG)" == "amcommon - Win32 Release"

# PROP Intermediate_Dir "Release\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 Debug"

# PROP Intermediate_Dir "Debug\obj"

!ELSEIF  "$(CFG)" == "amcommon - Win32 ReleaseMySQL"

# PROP BASE Intermediate_Dir "Release\obj"
# PROP Intermediate_Dir "Release\obj"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
