# Microsoft Developer Studio Project File - Name="fdlibm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fdlibm - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fdlibm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fdlibm.mak" CFG="fdlibm - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fdlibm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "fdlibm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\fdlibm__"
# PROP BASE Intermediate_Dir ".\fdlibm__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\fdlibm__"
# PROP Intermediate_Dir ".\fdlibm__"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\fdlibm_0"
# PROP BASE Intermediate_Dir ".\fdlibm_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\fdlibm_0"
# PROP Intermediate_Dir ".\fdlibm_0"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "fdlibm - Win32 Release"
# Name "fdlibm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_acos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_acosh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_asin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_atan2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_atanh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_cosh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_exp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_fmod.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_gamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_gamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_hypot.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_j0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_j1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_jn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_lgamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_lgamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_log10.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_pow.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_rem_pio2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_remainder.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_scalb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_sinh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\e_sqrt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\fdlibm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\k_cos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\k_rem_pio2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\k_sin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\k_standard.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\k_tan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_asinh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_atan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_cbrt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_ceil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_copysign.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_cos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_erf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_expm1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_fabs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_finite.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_floor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_frexp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_ilogb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_isnan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_ldexp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_lib_version.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_log1p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_logb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_matherr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_modf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_nextafter.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_rint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_scalbn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_signgam.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_significand.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_sin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_tan.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\s_tanh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_acos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_acosh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_asin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_atan2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_atanh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_cosh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_exp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_fmod.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_gamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_gamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_hypot.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_j0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_j1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_jn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_lgamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_lgamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_log10.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_pow.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_remainder.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_scalb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_sinh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\libjs\fdlibm\w_sqrt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
