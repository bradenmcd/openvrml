# Microsoft Developer Studio Project File - Name="lookat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=lookat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lookat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lookat.mak" CFG="lookat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lookat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lookat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lookat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "OPENVRML_DLL_IMPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../release/openvrml.lib glut32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"LIBC"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lookat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../debug/openvrml.lib glut32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBC.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "lookat - Win32 Release"
# Name "lookat - Win32 Debug"
# Begin Group "Source File"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\..\..\src\lookat\lookat.cpp

!IF  "$(CFG)" == "lookat - Win32 Release"

# ADD CPP /w /W0 /I "..\\" /I "..\..\..\..\src\openvrml" /I "..\..\..\..\src\openvrml-gl"

!ELSEIF  "$(CFG)" == "lookat - Win32 Debug"

# ADD CPP /MDd /Ze /Gm /GR- /Od /I "..\..\..\..\src\openvrml" /D "OPENVRML_DLL_IMPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\lookat\ViewerGlut.cpp

!IF  "$(CFG)" == "lookat - Win32 Release"

# ADD CPP /w /W0 /I "..\\" /I "..\..\..\..\src\openvrml" /I "..\..\..\..\src\openvrml-gl"
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "lookat - Win32 Debug"

# ADD CPP /I "..\\" /I "..\..\..\..\src\openvrml" /I "..\..\..\..\src\openvrml-gl" /D "OPENVRML_DLL_IMPORT"
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# End Group
# Begin Group "Include files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\..\..\src\lookat\glutint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\lookat\ViewerGlut.h
# End Source File
# End Group
# End Target
# End Project
