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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
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

SOURCE=..\..\..\..\..\lib\antlr\ANTLRException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\AST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTArray.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTNULLType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTPair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTRefCount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\BaseAST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\BitSet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharStreamException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CircularQueue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonAST.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonASTWithHiddenTokens.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonHiddenStreamToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\config.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\InputBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\LexerSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\LLkParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\MismatchedCharException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\MismatchedTokenException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\NoViableAltException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\NoViableAltForCharException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\Parser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ParserSharedInputState.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\RecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\RefCount.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\SemanticException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\String.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\Token.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamBasicFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamHiddenTokenFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamIOException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamRecognitionException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamRetryException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamSelector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TreeParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TreeParserSharedInputState.hpp
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ANTLRException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTFactory.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ASTRefCount.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\BaseAST.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\BitSet.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CharScanner.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonAST.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonASTWithHiddenTokens.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonHiddenStreamToken.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\CommonToken.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\InputBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\LexerSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\LLkParser.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\MismatchedCharException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\MismatchedTokenException.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\NoViableAltException.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\NoViableAltForCharException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\Parser.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\ParserSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\RecognitionException.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\String.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\Token.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenBuffer.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamBasicFilter.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamHiddenTokenFilter.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TokenStreamSelector.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TreeParser.cpp
# ADD CPP /w /W0 /GR
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\lib\antlr\TreeParserSharedInputState.cpp

!IF  "$(CFG)" == "libantlr - Win32 Release"

# ADD CPP /w /W0

!ELSEIF  "$(CFG)" == "libantlr - Win32 Debug"

# ADD CPP /w /W0 /GR-

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
