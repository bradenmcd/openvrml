# Microsoft Developer Studio Project File - Name="libjavascript" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libjavascript - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libjavascript.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libjavascript.mak" CFG="libjavascript - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libjavascript - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libjavascript - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libjavascript - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\libjavascript___Wi1"
# PROP BASE Intermediate_Dir ".\libjavascript___Wi1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "XP_PC" /D "JSFILE" /D "EXPORT_JS_API" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Release\libjavascript32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "libjavascript - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\libjavascript___Wi2"
# PROP BASE Intermediate_Dir ".\libjavascript___Wi2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "XP_PC" /D "JSFILE" /D "EXPORT_JS_API" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\libjavascript32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "libjavascript - Win32 Release"
# Name "libjavascript - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsarena.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsarray.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsatom.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsbool.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jscntxt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdbgapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsemit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsexn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsfun.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsgc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jshash.c
# End Source File
# Begin Source File

SOURCE=.\jsinitarrayapi.c
# ADD CPP /I "..\..\src\vrml97\javascript"
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslock.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslog2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslong.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsmath.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsnum.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsobj.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsopcode.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsparse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsprf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsregexp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscope.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscript.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsutil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsxdrapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\prmjtime.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsarena.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsarray.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsatom.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsbit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsbool.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsclist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jscntxt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jscompat.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jscpucfg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdbgapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsdtoa.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsemit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsexn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsfile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsfun.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsgc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jshash.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsinterp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslibmath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslock.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jslong.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsmath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsnum.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsopcode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsosdep.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsotypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsparse.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsprf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsprvtd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jspubtd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsregexp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscan.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscope.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsscript.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsstddef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsstr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jstypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsxdrapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\prmjtime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\js.msg
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\javascript\jsopcode.tbl
# End Source File
# End Target
# End Project
