# Microsoft Developer Studio Project File - Name="libvrml97js" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libvrml97js - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libvrml97js.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libvrml97js.mak" CFG="libvrml97js - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libvrml97js - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libvrml97js - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libvrml97js - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\libvrml97js___Wi1"
# PROP BASE Intermediate_Dir ".\libvrml97js___Wi1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Release\libvrml97js32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "libvrml97js - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\libvrml97js___Wi2"
# PROP BASE Intermediate_Dir ".\libvrml97js___Wi2"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\libvrml97js32.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "libvrml97js - Win32 Release"
# Name "libvrml97js - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\src\vrml97js\jsapi.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsarena.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsarray.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsatom.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsbool.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jscntxt.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdate.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdbgapi.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdtoa.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsemit.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsexn.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsfun.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsgc.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jshash.c
# End Source File
# Begin Source File

SOURCE=.\jsinitarrayapi.c

!IF  "$(CFG)" == "libvrml97js - Win32 Release"

# ADD CPP /I "..\src\vrml97js"

!ELSEIF  "$(CFG)" == "libvrml97js - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsinterp.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslock.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslog2.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslong.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsmath.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsnum.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsobj.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsopcode.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsparse.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsprf.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsregexp.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscan.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscope.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscript.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsstr.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsutil.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsxdrapi.c
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\prmjtime.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\src\vrml97js\jsapi.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsarena.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsarray.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsatom.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsbit.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsbool.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsclist.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jscntxt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jscompat.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsconfig.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jscpucfg.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdate.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdbgapi.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsdtoa.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsemit.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsexn.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsfile.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsfun.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsgc.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jshash.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsinterp.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslibmath.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslock.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jslong.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsmath.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsnum.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsobj.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsopcode.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsosdep.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsotypes.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsparse.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsprf.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsprvtd.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jspubtd.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsregexp.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscan.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscope.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsscript.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsstddef.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsstr.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jstypes.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsutil.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsxdrapi.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\prmjtime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\src\vrml97js\js.msg
# End Source File
# Begin Source File

SOURCE=..\src\vrml97js\jsopcode.tbl
# End Source File
# End Target
# End Project
