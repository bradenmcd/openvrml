# Microsoft Developer Studio Project File - Name="openvrmlcore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=openvrmlcore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "openvrmlcore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "openvrmlcore.mak" CFG="openvrmlcore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "openvrmlcore - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "openvrmlcore - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "DLL_EXPORT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\tplib\antlr\release\libantlr.lib regex.lib js32.lib libpng.lib libjpeg.lib zlib.lib jvm.lib wsock32.lib freetype211.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBC.lib" /out:".\Release\openvrmlcore.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /GR /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DLL_EXPORT" /YX /FD /LDd /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\tplib\antlr\debug\libantlr.lib js32.lib libpng.lib libjpeg.lib zlib.lib jvm.lib wsock32.lib freetype211_d.lib regex.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBC.lib" /out:".\Debug\openvrmlcore.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "openvrmlcore - Win32 Release"
# Name "openvrmlcore - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\OpenVRML\Audio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\browser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\bvolume.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Doc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\doc2.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\field.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\fieldvalueptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\MathUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\node.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodeclassptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodeptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodetypeptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\scope.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\scopeptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\script.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\System.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Viewer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\vrml97node.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlFrustum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.h
# End Source File
# End Group
# Begin Group "Source File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Audio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\browser.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /I "..\\" /I "..\..\..\..\lib" /D "__STDC__" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /w /W0 /I "../../../../lib" /I "../" /D "HAVE_CONFIG_H" /D "__STDC__"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\bvolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Doc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\doc2.cpp
# ADD CPP /I "..\\" /D "__STDC__" /D "HAVE_CONFIG_H"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\dummysound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\field.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\fieldvalueptr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Image.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /I "..\\" /D "XMD_H" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /w /W0 /I "..\\" /D "XMD_H" /D "HAVE_CONFIG_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\MathUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\node.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodeclassptr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodeptr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\nodetypeptr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\scope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\scopeptr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\script.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /I "..\\" /D "XP_PC" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /w /W0 /I "..\\" /D "XP_PC" /D "HAVE_CONFIG_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptJDK.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /I "..\\" /I "..\..\..\..\java" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /w /W0 /I "..\\" /I "..\..\..\..\java" /D "HAVE_CONFIG_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\System.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Viewer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\vrml97node.cpp
# ADD CPP /I "..\\" /D "HAVE_CONFIG_H" /Zm400
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlFrustum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.cpp
# End Source File
# End Group
# End Target
# End Project
