# Microsoft Developer Studio Project File - Name="openvrml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=openvrml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "openvrml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "openvrml.mak" CFG="openvrml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "openvrml - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "openvrml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "openvrml - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GR /GX /O2 /I ".\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "DLL_EXPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 .\tplib\antlr\release\libantlr.lib js32.lib opengl32.lib glu32.lib glut32.lib libpng.lib libjpeg.lib zlib.lib wsock32.lib jvm.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBC.lib" /out:".\Release\openvrml.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /LDd /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 .\tplib\antlr\debug\libantlr.lib js32.lib opengl32.lib glu32.lib glut32.lib libpng.lib libjpeg.lib zlib.lib wsock32.lib jvm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBC.lib" /out:".\Debug\openvrml.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "openvrml - Win32 Release"
# Name "openvrml - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Audio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\bvolume.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Doc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\doc2.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\field.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\MathUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\node.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\nodeptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\nodetypeptr.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\openvrml-gl\OpenVRML\GL\OpenGLEvent.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\proto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Route.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\script.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\ScriptJDK.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\System.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Viewer.h
# End Source File
# Begin Source File

SOURCE="..\..\..\src\openvrml-gl\OpenVRML\GL\ViewerOpenGL.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\vrml97node.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlFrustum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlNamespace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlScene.h
# End Source File
# End Group
# Begin Group "Source File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Audio.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\bvolume.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Doc.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\doc2.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\dummysound.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\field.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Image.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0 /D "XMD_H"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT" /D "XMD_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\MathUtils.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /Gd /Zp8 /Ze /w /W0 /Od /I ".\\" /D "DLL_EXPORT"
# SUBTRACT CPP /Gy

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\node.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\nodeptr.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\nodetypeptr.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\openvrml-gl\OpenVRML\GL\OpenGLEvent.cpp"

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\proto.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Route.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\script.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /D "XP_PC"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "XP_PC"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\ScriptJDK.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\System.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I "." /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\Viewer.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\src\openvrml-gl\OpenVRML\GL\ViewerOpenGL.cpp"

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0 /I "..\..\..\src\openvrml"
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /I "..\..\..\src\openvrml" /D "DLL_EXPORT"
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\vrml97node.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlFrustum.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlMatrix.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlNamespace.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I "." /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\openvrml\OpenVRML\VrmlScene.cpp

!IF  "$(CFG)" == "openvrml - Win32 Release"

# ADD CPP /w /W0 /GR /I "..\..\..\lib"

!ELSEIF  "$(CFG)" == "openvrml - Win32 Debug"

# ADD CPP /w /W0 /I ".\\" /I "..\..\..\lib" /D "DLL_EXPORT"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
