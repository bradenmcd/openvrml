# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=hs_regex - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to hs_regex - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "test_hs_regex - Win32 Release" && "$(CFG)" !=\
 "test_hs_regex - Win32 Debug" && "$(CFG)" != "hs_regex - Win32 Release" &&\
 "$(CFG)" != "hs_regex - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_hs_regex.mak" CFG="hs_regex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_hs_regex - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "test_hs_regex - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "hs_regex - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hs_regex - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project

!IF  "$(CFG)" == "test_hs_regex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "hs_regex - Win32 Release" "$(OUTDIR)\test_hs_regex.exe"

CLEAN : 
	-@erase ".\Release\test_hs_regex.exe"
	-@erase ".\Release\regex_test.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/test_hs_regex.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test_hs_regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/test_hs_regex.pdb" /machine:I386\
 /out:"$(OUTDIR)/test_hs_regex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/regex_test.obj" \
	".\Release\hs_regex.lib"

"$(OUTDIR)\test_hs_regex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "test_hs_regex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "hs_regex - Win32 Debug" "$(OUTDIR)\test_hs_regex.exe"

CLEAN : 
	-@erase ".\Debug\test_hs_regex.exe"
	-@erase ".\Debug\regex_test.obj"
	-@erase ".\Debug\test_hs_regex.ilk"
	-@erase ".\Debug\test_hs_regex.pdb"
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/test_hs_regex.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test_hs_regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/test_hs_regex.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/test_hs_regex.exe" 
LINK32_OBJS= \
	"$(INTDIR)/regex_test.obj" \
	".\Debug\hs_regex.lib"

"$(OUTDIR)\test_hs_regex.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hs_regex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "hs_regex\Release"
# PROP BASE Intermediate_Dir "hs_regex\Release"
# PROP BASE Target_Dir "hs_regex"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "hs_regex"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\hs_regex.dll"

CLEAN : 
	-@erase ".\Release\hs_regex.dll"
	-@erase ".\Release\regcomp.obj"
	-@erase ".\Release\regerror.obj"
	-@erase ".\Release\regexec.obj"
	-@erase ".\Release\regfree.obj"
	-@erase ".\Release\hs_regex.lib"
	-@erase ".\Release\hs_regex.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/hs_regex.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hs_regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/hs_regex.pdb" /machine:I386 /def:".\hs_regex.def"\
 /out:"$(OUTDIR)/hs_regex.dll" /implib:"$(OUTDIR)/hs_regex.lib" 
DEF_FILE= \
	".\hs_regex.def"
LINK32_OBJS= \
	"$(INTDIR)/regcomp.obj" \
	"$(INTDIR)/regerror.obj" \
	"$(INTDIR)/regexec.obj" \
	"$(INTDIR)/regfree.obj"

"$(OUTDIR)\hs_regex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hs_regex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "hs_regex\Debug"
# PROP BASE Intermediate_Dir "hs_regex\Debug"
# PROP BASE Target_Dir "hs_regex"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "hs_regex"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\hs_regex.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\hs_regex.dll"
	-@erase ".\Debug\regcomp.obj"
	-@erase ".\Debug\regerror.obj"
	-@erase ".\Debug\regexec.obj"
	-@erase ".\Debug\regfree.obj"
	-@erase ".\Debug\hs_regex.ilk"
	-@erase ".\Debug\hs_regex.lib"
	-@erase ".\Debug\hs_regex.exp"
	-@erase ".\Debug\hs_regex.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/hs_regex.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/hs_regex.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/hs_regex.pdb" /debug /machine:I386 /def:".\hs_regex.def"\
 /out:"$(OUTDIR)/hs_regex.dll" /implib:"$(OUTDIR)/hs_regex.lib" 
DEF_FILE= \
	".\hs_regex.def"
LINK32_OBJS= \
	"$(INTDIR)/regcomp.obj" \
	"$(INTDIR)/regerror.obj" \
	"$(INTDIR)/regexec.obj" \
	"$(INTDIR)/regfree.obj"

"$(OUTDIR)\hs_regex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "test_hs_regex - Win32 Release"
# Name "test_hs_regex - Win32 Debug"

!IF  "$(CFG)" == "test_hs_regex - Win32 Release"

!ELSEIF  "$(CFG)" == "test_hs_regex - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\regex_test.c
DEP_CPP_REGEX=\
	".\regex.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\regex_test.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "hs_regex"

!IF  "$(CFG)" == "test_hs_regex - Win32 Debug"

"hs_regex - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\test_hs_regex.mak CFG="hs_regex - Win32 Debug" 

!ELSEIF  "$(CFG)" == "test_hs_regex - Win32 Release"

"hs_regex - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\test_hs_regex.mak CFG="hs_regex - Win32 Release" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "hs_regex - Win32 Release"
# Name "hs_regex - Win32 Debug"

!IF  "$(CFG)" == "hs_regex - Win32 Release"

!ELSEIF  "$(CFG)" == "hs_regex - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\regcomp.c
DEP_CPP_REGCO=\
	{$(INCLUDE)}"\sys\types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\cclass.h"\
	".\cname.h"\
	

"$(INTDIR)\regcomp.obj" : $(SOURCE) $(DEP_CPP_REGCO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\hs_regex.def

!IF  "$(CFG)" == "hs_regex - Win32 Release"

!ELSEIF  "$(CFG)" == "hs_regex - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\regerror.c
DEP_CPP_REGER=\
	{$(INCLUDE)}"\sys\types.h"\
	".\regex.h"\
	".\utils.h"\
	

"$(INTDIR)\regerror.obj" : $(SOURCE) $(DEP_CPP_REGER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\regexec.c
DEP_CPP_REGEXE=\
	{$(INCLUDE)}"\sys\types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	".\engine.c"\
	

"$(INTDIR)\regexec.obj" : $(SOURCE) $(DEP_CPP_REGEXE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\regfree.c
DEP_CPP_REGFR=\
	{$(INCLUDE)}"\sys\types.h"\
	".\regex.h"\
	".\utils.h"\
	".\regex2.h"\
	

"$(INTDIR)\regfree.obj" : $(SOURCE) $(DEP_CPP_REGFR) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
