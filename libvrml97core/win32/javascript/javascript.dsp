# Microsoft Developer Studio Project File - Name="javascript" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=javascript - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "javascript.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "javascript.mak" CFG="javascript - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "javascript - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "javascript - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "javascript - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\javascript___Wi1"
# PROP BASE Intermediate_Dir ".\javascript___Wi1"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBC" /out:".\Release\javascript32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "javascript - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\javascript___Wi2"
# PROP BASE Intermediate_Dir ".\javascript___Wi2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "XP_PC" /D "JSFILE" /D "EXPORT_JS_API" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\javascript32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "javascript - Win32 Release"
# Name "javascript - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsaddr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsarena.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsarray.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsatom.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsbool.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jscntxt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdbgapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsemit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsexn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsfun.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsgc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jshash.c
# End Source File
# Begin Source File

SOURCE=.\jsinitarrayapi.c

!IF  "$(CFG)" == "javascript - Win32 Release"

# ADD CPP /I "..\..\src\vrml97\libjs"

!ELSEIF  "$(CFG)" == "javascript - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslock.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslog2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslong.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsmath.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsnum.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsobj.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsopcode.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsparse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsprf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsregexp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscope.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscript.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsutil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsxdrapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\prmjtime.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsarena.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsarray.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsatom.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsbit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsbool.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsclist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jscntxt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jscompat.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jscpucfg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdbgapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsdtoa.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsemit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsexn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsfile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsfun.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsgc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jshash.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsinterp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslibmath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslock.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jslong.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsmath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsnum.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsopcode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsosdep.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsotypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsparse.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsprf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsprvtd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jspubtd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsregexp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscan.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscope.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsscript.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsstddef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsstr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jstypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsxdrapi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\prmjtime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\js.msg
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\jsopcode.tbl
# End Source File
# End Target
# End Project
