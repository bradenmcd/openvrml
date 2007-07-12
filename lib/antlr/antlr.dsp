# Microsoft Developer Studio Project File - Name="antlr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=antlr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "antlr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "antlr.mak" CFG="antlr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "antlr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "antlr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "antlr - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTLR_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTLR_EXPORTS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"c:/winnt/system32/antlr.dll"

!ELSEIF  "$(CFG)" == "antlr - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTLR_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTLR_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:/winnt/system32/antlrD.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "antlr - Win32 Release"
# Name "antlr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ANTLRException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ASTFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ASTRefCount.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BaseAST.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BitSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CharBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CharScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CommonAST.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CommonASTWithHiddenTokens.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CommonHiddenStreamToken.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CommonToken.cpp
# End Source File
# Begin Source File

SOURCE=.\src\InputBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LexerSharedInputState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LLkParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MismatchedCharException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MismatchedTokenException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NoViableAltException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NoViableAltForCharException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ParserSharedInputState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RecognitionException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\String.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Token.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TokenBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TokenStreamBasicFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TokenStreamHiddenTokenFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TokenStreamSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TreeParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TreeParserSharedInputState.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\antlr\ANTLRException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\AST.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ASTArray.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ASTFactory.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ASTNULLType.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ASTPair.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ASTRefCount.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\BaseAST.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\BitSet.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CharBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CharScanner.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CharStreamException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CharStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CircularQueue.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CommonAST.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CommonASTWithHiddenTokens.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CommonHiddenStreamToken.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\CommonToken.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\config.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\InputBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\LexerSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\LLkParser.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\MismatchedCharException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\MismatchedTokenException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\NoViableAltException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\NoViableAltForCharException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\Parser.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\ParserSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\RecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\RefCount.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\SemanticException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\String.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\Token.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenBuffer.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStream.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamBasicFilter.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamHiddenTokenFilter.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamRecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamRetryException.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TokenStreamSelector.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TreeParser.hpp
# End Source File
# Begin Source File

SOURCE=.\antlr\TreeParserSharedInputState.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
