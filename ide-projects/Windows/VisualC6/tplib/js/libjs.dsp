# Microsoft Developer Studio Project File - Name="libjs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libjs - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libjs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libjs.mak" CFG="libjs - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libjs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libjs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libjs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\libjs___Wi1"
# PROP BASE Intermediate_Dir ".\libjs___Wi1"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Release\libjs.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "libjs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\libjs___Wi2"
# PROP BASE Intermediate_Dir ".\libjs___Wi2"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\libjs.dll"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "libjs - Win32 Release"
# Name "libjs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsarena.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsarray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsatom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsbool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jscntxt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdbgapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsemit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsexn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsfun.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsgc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jshash.c
# End Source File
# Begin Source File

SOURCE=.\jsinitarrayapi.c
# ADD CPP /I "..\..\..\..\..\lib\js"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslog2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslong.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsmath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsnum.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsobj.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsopcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsparse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsprf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsregexp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscope.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscript.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsxdrapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\prmjtime.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsarena.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsarray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsatom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsbit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsbool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsclist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jscntxt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jscompat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jscpucfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdbgapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsdtoa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsemit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsexn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsfun.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsgc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jshash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsinterp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslibmath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jslong.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsmath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsnum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsobj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsopcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsosdep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsotypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsparse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsprf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsprvtd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jspubtd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsregexp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscope.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsscript.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsstddef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsstr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jstypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsxdrapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\prmjtime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\js.msg
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\js\jsopcode.tbl
# End Source File
# End Target
# End Project
