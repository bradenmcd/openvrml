# Microsoft Developer Studio Project File - Name="libcpp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcpp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcpp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcpp.mak" CFG="libcpp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcpp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libcpp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcpp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"antlr.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del *.obj	del *.idb	del *.pch
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcpp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"antlrd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del *.obj	del *.pdb	del *.idb	del *.pch
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcpp - Win32 Release"
# Name "libcpp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ANTLRException.cpp
# End Source File
# Begin Source File

SOURCE=.\ASTFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\ASTRefCount.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseAST.cpp
# End Source File
# Begin Source File

SOURCE=.\BitSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CharBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CharScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonAST.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonASTWithHiddenTokens.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonHiddenStreamToken.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonToken.cpp
# End Source File
# Begin Source File

SOURCE=.\InputBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\LexerSharedInputState.cpp
# End Source File
# Begin Source File

SOURCE=.\LLkParser.cpp
# End Source File
# Begin Source File

SOURCE=.\MismatchedCharException.cpp
# End Source File
# Begin Source File

SOURCE=.\MismatchedTokenException.cpp
# End Source File
# Begin Source File

SOURCE=.\NoViableAltException.cpp
# End Source File
# Begin Source File

SOURCE=.\NoViableAltForCharException.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\ParserSharedInputState.cpp
# End Source File
# Begin Source File

SOURCE=.\RecognitionException.cpp
# End Source File
# Begin Source File

SOURCE=.\String.cpp
# End Source File
# Begin Source File

SOURCE=.\Token.cpp
# End Source File
# Begin Source File

SOURCE=.\TokenBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\TokenStreamBasicFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\TokenStreamHiddenTokenFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\TokenStreamSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeParser.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeParserSharedInputState.cpp
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
# End Target
# End Project
