# Microsoft Developer Studio Generated NMAKE File, Based on mp.dsp
!IF "$(CFG)" == ""
CFG=mp - Win32 Release
!MESSAGE No configuration specified. Defaulting to mp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "mp - Win32 Release" && "$(CFG)" != "mp - Win32 Debug" && "$(CFG)" != "mp - Win32 Profile"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp.mak" CFG="mp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mp - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp - Win32 Release"

OUTDIR=.\Releasemp
INTDIR=.\Releasemp
# Begin Custom Macros
OutDir=.\Releasemp
# End Custom Macros

ALL : "$(OUTDIR)\mp.dll" ".\mp"


CLEAN :
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\users.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mp.dll"
	-@erase "$(OUTDIR)\mp.exp"
	-@erase "$(OUTDIR)\mp.lib"
	-@erase "mp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mp.pdb" /machine:I386 /def:".\mp.def" /out:"$(OUTDIR)\mp.dll" /implib:"$(OUTDIR)\mp.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\users.obj" \
	"$(INTDIR)\util.obj"

"$(OUTDIR)\mp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\Releasemp\mp.dll
TargetName=mp
InputPath=.\Releasemp\mp.dll
SOURCE="$(InputPath)"

".\mp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetPath) \half-life\mp\dlls
<< 
	

!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

OUTDIR=.\debugmp
INTDIR=.\debugmp
# Begin Custom Macros
OutDir=.\debugmp
# End Custom Macros

ALL : "$(OUTDIR)\mp.dll" "$(OUTDIR)\mp.bsc" ".\mp"


CLEAN :
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\dll.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\linkfunc.sbr"
	-@erase "$(INTDIR)\users.obj"
	-@erase "$(INTDIR)\users.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mp.bsc"
	-@erase "$(OUTDIR)\mp.dll"
	-@erase "$(OUTDIR)\mp.ilk"
	-@erase "$(OUTDIR)\mp.pdb"
	-@erase ".\Debug\mp.exp"
	-@erase ".\Debug\mp.lib"
	-@erase "mp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\engine" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dll.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\linkfunc.sbr" \
	"$(INTDIR)\users.sbr" \
	"$(INTDIR)\util.sbr"

"$(OUTDIR)\mp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mp.pdb" /debug /machine:I386 /def:".\mp.def" /out:"$(OUTDIR)\mp.dll" /implib:".\Debug\mp.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\users.obj" \
	"$(INTDIR)\util.obj"

"$(OUTDIR)\mp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\debugmp\mp.dll
TargetName=mp
InputPath=.\debugmp\mp.dll
SOURCE="$(InputPath)"

".\mp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetPath) \half-life\mp\dlls
<< 
	

!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

OUTDIR=.\Profilemp
INTDIR=.\Profilemp
# Begin Custom Macros
OutDir=.\Profilemp
# End Custom Macros

ALL : "$(OUTDIR)\mp.dll" "\half-life\mp\dlls\mp.dll" "\half-life\mp\dlls\mp.map"


CLEAN :
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\users.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mp.dll"
	-@erase "$(OUTDIR)\mp.exp"
	-@erase "$(OUTDIR)\mp.lib"
	-@erase "\half-life\mp\dlls\mp.dll"
	-@erase "\half-life\mp\dlls\mp.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fp"$(INTDIR)\mp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\mp.def" /out:"$(OUTDIR)\mp.dll" /implib:"$(OUTDIR)\mp.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\users.obj" \
	"$(INTDIR)\util.obj"

"$(OUTDIR)\mp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=.\Profilemp
InputPath=.\Profilemp\mp.dll
SOURCE="$(InputPath)"

"..\..\half-life\mp\dlls\mp.dll"	"..\..\half-life\mp\dlls\mp.map" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy $(TargetDir)\mp.dll \half-life\mp\dlls 
	copy $(TargetDir)\mp.map \half-life\mp\dlls
<< 
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mp.dep")
!INCLUDE "mp.dep"
!ELSE 
!MESSAGE Warning: cannot find "mp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mp - Win32 Release" || "$(CFG)" == "mp - Win32 Debug" || "$(CFG)" == "mp - Win32 Profile"
SOURCE=.\dll.cpp

!IF  "$(CFG)" == "mp - Win32 Release"


"$(INTDIR)\dll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Debug"


"$(INTDIR)\dll.obj"	"$(INTDIR)\dll.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Profile"


"$(INTDIR)\dll.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\h_export.cpp

!IF  "$(CFG)" == "mp - Win32 Release"


"$(INTDIR)\h_export.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Debug"


"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Profile"


"$(INTDIR)\h_export.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\linkfunc.cpp

!IF  "$(CFG)" == "mp - Win32 Release"


"$(INTDIR)\linkfunc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Debug"


"$(INTDIR)\linkfunc.obj"	"$(INTDIR)\linkfunc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Profile"


"$(INTDIR)\linkfunc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\users.cpp

!IF  "$(CFG)" == "mp - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\users.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mp - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\engine" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\users.obj"	"$(INTDIR)\users.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mp - Win32 Profile"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fp"$(INTDIR)\mp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\users.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\util.cpp

!IF  "$(CFG)" == "mp - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Debug"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mp - Win32 Profile"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

