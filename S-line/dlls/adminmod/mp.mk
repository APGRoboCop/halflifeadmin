##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Win32_Release_MM
ProjectName            :=mp
ConfigurationName      :=Win32_Release_MM
WorkspacePath          :=E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod
ProjectPath            :=E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod
IntermediateDirectory  :=./ReleaseMM/obj
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ryan
Date                   :=16/08/2016
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/MinGW/bin/g++.exe
SharedObjectLinkerName :=C:/MinGW/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=./ReleaseMM/admin_mm.dll
Preprocessors          :=$(PreprocessorSwitch)NDEBUG $(PreprocessorSwitch)WIN32 $(PreprocessorSwitch)_WINDOWS $(PreprocessorSwitch)VALVE_DLL $(PreprocessorSwitch)HAVE_CONFIG_H $(PreprocessorSwitch)USE_METAMOD 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="mp.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW/bin/windres.exe
LinkOptions            :=  -O0
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)..\..\commonlib\include $(IncludeSwitch)..\..\..\..\hlsdk2015\engine $(IncludeSwitch)..\..\..\..\hlsdk2015\common $(IncludeSwitch)..\..\..\..\hlsdk2015\pm_shared $(IncludeSwitch)..\..\..\..\hlsdk2015\dlls $(IncludeSwitch)..\..\..\..\hlsdk2015\public $(IncludeSwitch)..\..\..\..\metamod-p-37\metamod $(IncludeSwitch)..\amxdll $(IncludeSwitch)..\..\tools\amxconvert 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)amcommon $(LibrarySwitch)kernel32 $(LibrarySwitch)user32 $(LibrarySwitch)gdi32 $(LibrarySwitch)winspool $(LibrarySwitch)comdlg32 $(LibrarySwitch)advapi32 $(LibrarySwitch)shell32 $(LibrarySwitch)ole32 $(LibrarySwitch)oleaut32 $(LibrarySwitch)uuid 
ArLibs                 :=  "amcommon.a" "kernel32.a" "user32.a" "gdi32.a" "winspool.a" "comdlg32.a" "advapi32.a" "shell32.a" "ole32.a" "oleaut32.a" "uuid.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)..\..\commonlib\msvc6\Release 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW/bin/ar.exe rcu
CXX      := C:/MinGW/bin/g++.exe
CC       := C:/MinGW/bin/gcc.exe
CXXFLAGS :=  -g -Wall -O2 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/admin_commands.cpp$(ObjectSuffix) $(IntermediateDirectory)/admin_mod.cpp$(ObjectSuffix) $(IntermediateDirectory)/AmFSNode.cpp$(ObjectSuffix) $(IntermediateDirectory)/amlibc.c$(ObjectSuffix) $(IntermediateDirectory)/amutil.cpp$(ObjectSuffix) $(IntermediateDirectory)/amxconv_l.cpp$(ObjectSuffix) $(IntermediateDirectory)/authid.cpp$(ObjectSuffix) $(IntermediateDirectory)/cbase.cpp$(ObjectSuffix) $(IntermediateDirectory)/CLinkList.cpp$(ObjectSuffix) $(IntermediateDirectory)/CPlugin.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/dll.cpp$(ObjectSuffix) $(IntermediateDirectory)/h_export.cpp$(ObjectSuffix) $(IntermediateDirectory)/regex.c$(ObjectSuffix) $(IntermediateDirectory)/statics.cpp$(ObjectSuffix) $(IntermediateDirectory)/sutils.cpp$(ObjectSuffix) $(IntermediateDirectory)/timer.cpp$(ObjectSuffix) $(IntermediateDirectory)/users.cpp$(ObjectSuffix) $(IntermediateDirectory)/util.cpp$(ObjectSuffix) $(IntermediateDirectory)/version.cpp$(ObjectSuffix) $(IntermediateDirectory)/adminmod.rc$(ObjectSuffix) \
	



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "E:\Dropbox\src\halflifeadmin\S-line\dlls\adminmod/.build-release"
	@echo rebuilt > "E:\Dropbox\src\halflifeadmin\S-line\dlls\adminmod/.build-release/mp"

MakeIntermediateDirs:
	@$(MakeDirCommand) "./ReleaseMM/obj"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./ReleaseMM/obj"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/admin_commands.cpp$(ObjectSuffix): admin_commands.cpp $(IntermediateDirectory)/admin_commands.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/admin_commands.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/admin_commands.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/admin_commands.cpp$(DependSuffix): admin_commands.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/admin_commands.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/admin_commands.cpp$(DependSuffix) -MM admin_commands.cpp

$(IntermediateDirectory)/admin_commands.cpp$(PreprocessSuffix): admin_commands.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/admin_commands.cpp$(PreprocessSuffix)admin_commands.cpp

$(IntermediateDirectory)/admin_mod.cpp$(ObjectSuffix): admin_mod.cpp $(IntermediateDirectory)/admin_mod.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/admin_mod.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/admin_mod.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/admin_mod.cpp$(DependSuffix): admin_mod.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/admin_mod.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/admin_mod.cpp$(DependSuffix) -MM admin_mod.cpp

$(IntermediateDirectory)/admin_mod.cpp$(PreprocessSuffix): admin_mod.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/admin_mod.cpp$(PreprocessSuffix)admin_mod.cpp

$(IntermediateDirectory)/AmFSNode.cpp$(ObjectSuffix): AmFSNode.cpp $(IntermediateDirectory)/AmFSNode.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/AmFSNode.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AmFSNode.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AmFSNode.cpp$(DependSuffix): AmFSNode.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AmFSNode.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/AmFSNode.cpp$(DependSuffix) -MM AmFSNode.cpp

$(IntermediateDirectory)/AmFSNode.cpp$(PreprocessSuffix): AmFSNode.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AmFSNode.cpp$(PreprocessSuffix)AmFSNode.cpp

$(IntermediateDirectory)/amlibc.c$(ObjectSuffix): amlibc.c $(IntermediateDirectory)/amlibc.c$(DependSuffix)
	$(CC) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/amlibc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/amlibc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/amlibc.c$(DependSuffix): amlibc.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/amlibc.c$(ObjectSuffix) -MF$(IntermediateDirectory)/amlibc.c$(DependSuffix) -MM amlibc.c

$(IntermediateDirectory)/amlibc.c$(PreprocessSuffix): amlibc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/amlibc.c$(PreprocessSuffix)amlibc.c

$(IntermediateDirectory)/amutil.cpp$(ObjectSuffix): amutil.cpp $(IntermediateDirectory)/amutil.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/amutil.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/amutil.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/amutil.cpp$(DependSuffix): amutil.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/amutil.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/amutil.cpp$(DependSuffix) -MM amutil.cpp

$(IntermediateDirectory)/amutil.cpp$(PreprocessSuffix): amutil.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/amutil.cpp$(PreprocessSuffix)amutil.cpp

$(IntermediateDirectory)/amxconv_l.cpp$(ObjectSuffix): ../../tools/amxconvert/amxconv_l.cpp $(IntermediateDirectory)/amxconv_l.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/tools/amxconvert/amxconv_l.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/amxconv_l.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/amxconv_l.cpp$(DependSuffix): ../../tools/amxconvert/amxconv_l.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/amxconv_l.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/amxconv_l.cpp$(DependSuffix) -MM ../../tools/amxconvert/amxconv_l.cpp

$(IntermediateDirectory)/amxconv_l.cpp$(PreprocessSuffix): ../../tools/amxconvert/amxconv_l.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/amxconv_l.cpp$(PreprocessSuffix)../../tools/amxconvert/amxconv_l.cpp

$(IntermediateDirectory)/authid.cpp$(ObjectSuffix): authid.cpp $(IntermediateDirectory)/authid.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/authid.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/authid.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/authid.cpp$(DependSuffix): authid.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/authid.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/authid.cpp$(DependSuffix) -MM authid.cpp

$(IntermediateDirectory)/authid.cpp$(PreprocessSuffix): authid.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/authid.cpp$(PreprocessSuffix)authid.cpp

$(IntermediateDirectory)/cbase.cpp$(ObjectSuffix): cbase.cpp $(IntermediateDirectory)/cbase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/cbase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cbase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cbase.cpp$(DependSuffix): cbase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/cbase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/cbase.cpp$(DependSuffix) -MM cbase.cpp

$(IntermediateDirectory)/cbase.cpp$(PreprocessSuffix): cbase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cbase.cpp$(PreprocessSuffix)cbase.cpp

$(IntermediateDirectory)/CLinkList.cpp$(ObjectSuffix): CLinkList.cpp $(IntermediateDirectory)/CLinkList.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/CLinkList.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CLinkList.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CLinkList.cpp$(DependSuffix): CLinkList.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CLinkList.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CLinkList.cpp$(DependSuffix) -MM CLinkList.cpp

$(IntermediateDirectory)/CLinkList.cpp$(PreprocessSuffix): CLinkList.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CLinkList.cpp$(PreprocessSuffix)CLinkList.cpp

$(IntermediateDirectory)/CPlugin.cpp$(ObjectSuffix): CPlugin.cpp $(IntermediateDirectory)/CPlugin.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/CPlugin.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CPlugin.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CPlugin.cpp$(DependSuffix): CPlugin.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CPlugin.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CPlugin.cpp$(DependSuffix) -MM CPlugin.cpp

$(IntermediateDirectory)/CPlugin.cpp$(PreprocessSuffix): CPlugin.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CPlugin.cpp$(PreprocessSuffix)CPlugin.cpp

$(IntermediateDirectory)/dll.cpp$(ObjectSuffix): dll.cpp $(IntermediateDirectory)/dll.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/dll.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dll.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dll.cpp$(DependSuffix): dll.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dll.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dll.cpp$(DependSuffix) -MM dll.cpp

$(IntermediateDirectory)/dll.cpp$(PreprocessSuffix): dll.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dll.cpp$(PreprocessSuffix)dll.cpp

$(IntermediateDirectory)/h_export.cpp$(ObjectSuffix): h_export.cpp $(IntermediateDirectory)/h_export.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/h_export.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/h_export.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/h_export.cpp$(DependSuffix): h_export.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/h_export.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/h_export.cpp$(DependSuffix) -MM h_export.cpp

$(IntermediateDirectory)/h_export.cpp$(PreprocessSuffix): h_export.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/h_export.cpp$(PreprocessSuffix)h_export.cpp

$(IntermediateDirectory)/regex.c$(ObjectSuffix): regex.c $(IntermediateDirectory)/regex.c$(DependSuffix)
	$(CC) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/regex.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/regex.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/regex.c$(DependSuffix): regex.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/regex.c$(ObjectSuffix) -MF$(IntermediateDirectory)/regex.c$(DependSuffix) -MM regex.c

$(IntermediateDirectory)/regex.c$(PreprocessSuffix): regex.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/regex.c$(PreprocessSuffix)regex.c

$(IntermediateDirectory)/statics.cpp$(ObjectSuffix): statics.cpp $(IntermediateDirectory)/statics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/statics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/statics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/statics.cpp$(DependSuffix): statics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/statics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/statics.cpp$(DependSuffix) -MM statics.cpp

$(IntermediateDirectory)/statics.cpp$(PreprocessSuffix): statics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/statics.cpp$(PreprocessSuffix)statics.cpp

$(IntermediateDirectory)/sutils.cpp$(ObjectSuffix): sutils.cpp $(IntermediateDirectory)/sutils.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/sutils.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sutils.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sutils.cpp$(DependSuffix): sutils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sutils.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/sutils.cpp$(DependSuffix) -MM sutils.cpp

$(IntermediateDirectory)/sutils.cpp$(PreprocessSuffix): sutils.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sutils.cpp$(PreprocessSuffix)sutils.cpp

$(IntermediateDirectory)/timer.cpp$(ObjectSuffix): timer.cpp $(IntermediateDirectory)/timer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/timer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/timer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/timer.cpp$(DependSuffix): timer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/timer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/timer.cpp$(DependSuffix) -MM timer.cpp

$(IntermediateDirectory)/timer.cpp$(PreprocessSuffix): timer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/timer.cpp$(PreprocessSuffix)timer.cpp

$(IntermediateDirectory)/users.cpp$(ObjectSuffix): users.cpp $(IntermediateDirectory)/users.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/users.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/users.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/users.cpp$(DependSuffix): users.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/users.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/users.cpp$(DependSuffix) -MM users.cpp

$(IntermediateDirectory)/users.cpp$(PreprocessSuffix): users.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/users.cpp$(PreprocessSuffix)users.cpp

$(IntermediateDirectory)/util.cpp$(ObjectSuffix): util.cpp $(IntermediateDirectory)/util.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/util.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/util.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/util.cpp$(DependSuffix): util.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/util.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/util.cpp$(DependSuffix) -MM util.cpp

$(IntermediateDirectory)/util.cpp$(PreprocessSuffix): util.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/util.cpp$(PreprocessSuffix)util.cpp

$(IntermediateDirectory)/version.cpp$(ObjectSuffix): version.cpp $(IntermediateDirectory)/version.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/version.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/version.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/version.cpp$(DependSuffix): version.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/version.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/version.cpp$(DependSuffix) -MM version.cpp

$(IntermediateDirectory)/version.cpp$(PreprocessSuffix): version.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/version.cpp$(PreprocessSuffix)version.cpp

$(IntermediateDirectory)/adminmod.rc$(ObjectSuffix): adminmod.rc
	$(RcCompilerName) -i "E:/Dropbox/src/halflifeadmin/S-line/dlls/adminmod/adminmod.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/adminmod.rc$(ObjectSuffix) $(RcIncludePath)

-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./ReleaseMM/obj/


