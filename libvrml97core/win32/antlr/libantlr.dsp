# Microsoft Developer Studio Project File - Name="libantlr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libantlr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libantlr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libantlr.mak" CFG="libantlr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libantlr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libantlr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libantlr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libantlr - Win32 Release"
# Name "libantlr - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ANTLRException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\AST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTArray.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTNULLType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTPair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTRefCount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\BaseAST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\BitSet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharStreamException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CircularQueue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonAST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonASTWithHiddenTokens.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonHiddenStreamToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\config.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\InputBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\LexerSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\LLkParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\MismatchedCharException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\MismatchedTokenException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\NoViableAltException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\NoViableAltForCharException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\Parser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ParserSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\RecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\RefCount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\SemanticException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\String.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\Token.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamBasicFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamHiddenTokenFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamRecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamRetryException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamSelector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TreeParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TreeParserSharedInputState.hpp
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ANTLRException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTFactory.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ASTRefCount.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\BaseAST.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\BitSet.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CharScanner.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonAST.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonASTWithHiddenTokens.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonHiddenStreamToken.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\CommonToken.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\InputBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\LexerSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\LLkParser.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\MismatchedCharException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\MismatchedTokenException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\NoViableAltException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\NoViableAltForCharException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\Parser.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\ParserSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\RecognitionException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\String.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\Token.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamBasicFilter.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamHiddenTokenFilter.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TokenStreamSelector.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TreeParser.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /GR

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\vrml97\antlr\TreeParserSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /GR-

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
