###############################################################################
# $Id$
###############################################################################

# The support lib
L_TARGET := libantlr.a
WIN_SOURCE := dll.cpp
SOURCE     := ANTLRUtil.cpp \
				  ASTFactory.cpp \
				  ASTNULLType.cpp \
				  ASTRefCount.cpp \
				  BaseAST.cpp \
				  BitSet.cpp \
				  CharBuffer.cpp \
				  CharScanner.cpp \
				  CommonAST.cpp \
				  CommonASTWithHiddenTokens.cpp \
				  CommonHiddenStreamToken.cpp \
				  CommonToken.cpp \
				  InputBuffer.cpp \
				  LLkParser.cpp \
				  MismatchedCharException.cpp \
				  MismatchedTokenException.cpp \
				  NoViableAltException.cpp \
				  NoViableAltForCharException.cpp \
				  Parser.cpp \
				  RecognitionException.cpp \
				  String.cpp \
				  Token.cpp \
				  TokenBuffer.cpp \
				  TokenStreamBasicFilter.cpp \
				  TokenStreamHiddenTokenFilter.cpp \
				  TokenStreamSelector.cpp \
				  TreeParser.cpp

L_OBJS   := $(SOURCE:.cpp=.o)

ifndef TOPDIR
TOPDIR  = $(shell pwd)/../../..
endif

include $(TOPDIR)/scripts/Config.make
include $(TOPDIR)/scripts/Rules.make

EXTRA_CXXFLAGS = -I..
