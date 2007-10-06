SUBDIRS = @gtkglext_subdir@
DIST_SUBDIRS = gtkglext

noinst_HEADERS = \
        antlr/antlr/ANTLRException.hpp \
        antlr/antlr/ANTLRUtil.hpp \
        antlr/antlr/ASTArray.hpp \
        antlr/antlr/AST.hpp \
        antlr/antlr/ASTArray.hpp \
        antlr/antlr/ASTFactory.hpp \
        antlr/antlr/ASTNULLType.hpp \
        antlr/antlr/ASTPair.hpp \
        antlr/antlr/ASTRefCount.hpp \
        antlr/antlr/BaseAST.hpp \
        antlr/antlr/BitSet.hpp \
        antlr/antlr/CharBuffer.hpp \
        antlr/antlr/CharInputBuffer.hpp \
        antlr/antlr/CharScanner.hpp \
        antlr/antlr/CharStreamException.hpp \
        antlr/antlr/CharStreamIOException.hpp \
        antlr/antlr/CircularQueue.hpp \
        antlr/antlr/CommonAST.hpp \
        antlr/antlr/CommonASTWithHiddenTokens.hpp \
        antlr/antlr/CommonHiddenStreamToken.hpp \
        antlr/antlr/CommonToken.hpp \
        antlr/antlr/InputBuffer.hpp \
        antlr/antlr/IOException.hpp \
        antlr/antlr/LLkParser.hpp \
        antlr/antlr/LexerSharedInputState.hpp \
        antlr/antlr/MismatchedCharException.hpp \
        antlr/antlr/MismatchedTokenException.hpp \
        antlr/antlr/NoViableAltException.hpp \
        antlr/antlr/NoViableAltForCharException.hpp \
        antlr/antlr/Parser.hpp \
        antlr/antlr/ParserSharedInputState.hpp \
        antlr/antlr/RecognitionException.hpp \
        antlr/antlr/RefCount.hpp \
        antlr/antlr/SemanticException.hpp \
        antlr/antlr/String.hpp \
        antlr/antlr/Token.hpp \
        antlr/antlr/TokenBuffer.hpp \
        antlr/antlr/TokenStream.hpp \
        antlr/antlr/TokenStreamBasicFilter.hpp \
        antlr/antlr/TokenStreamException.hpp \
        antlr/antlr/TokenStreamHiddenTokenFilter.hpp \
        antlr/antlr/TokenStreamIOException.hpp \
        antlr/antlr/TokenStreamRecognitionException.hpp \
        antlr/antlr/TokenStreamRetryException.hpp \
        antlr/antlr/TokenStreamSelector.hpp \
        antlr/antlr/TreeParser.hpp \
        antlr/antlr/TreeParserSharedInputState.hpp \
        antlr/antlr/config.hpp

AM_CPPFLAGS = -I$(top_srcdir)/lib/antlr -DANTLR_EXPORTS

noinst_LTLIBRARIES = antlr/libantlr.la
antlr_libantlr_la_LDFLAGS = -avoid-version

antlr_libantlr_la_SOURCES = \
        antlr/src/ANTLRUtil.cpp \
        antlr/src/ASTFactory.cpp \
        antlr/src/ASTNULLType.cpp \
        antlr/src/ASTRefCount.cpp \
        antlr/src/BaseAST.cpp \
        antlr/src/BitSet.cpp \
        antlr/src/CharBuffer.cpp \
        antlr/src/CharScanner.cpp \
        antlr/src/CommonAST.cpp \
        antlr/src/CommonASTWithHiddenTokens.cpp \
        antlr/src/CommonHiddenStreamToken.cpp \
        antlr/src/CommonToken.cpp \
        antlr/src/InputBuffer.cpp \
        antlr/src/LLkParser.cpp \
        antlr/src/MismatchedCharException.cpp \
        antlr/src/MismatchedTokenException.cpp \
        antlr/src/NoViableAltException.cpp \
        antlr/src/NoViableAltForCharException.cpp \
        antlr/src/Parser.cpp \
        antlr/src/RecognitionException.cpp \
        antlr/src/String.cpp \
        antlr/src/Token.cpp \
        antlr/src/TokenBuffer.cpp \
        antlr/src/TokenRefCount.cpp \
        antlr/src/TokenStreamBasicFilter.cpp \
        antlr/src/TokenStreamHiddenTokenFilter.cpp \
        antlr/src/TokenStreamSelector.cpp \
        antlr/src/TreeParser.cpp

EXTRA_DIST = \
        antlr/Makefile.in \
        antlr/doxygen.cfg \
        antlr/antlr/Makefile.in \
        antlr/contrib/bcb4/README \
        antlr/contrib/bcb4/antlr.cpp \
        antlr/contrib/bcb4/antlr.bpr \
        antlr/scripts/cr_stripper.sh \
        antlr/scripts/make_change_log.tcl \
        antlr/src/Makefile.in \
        antlr/src/dll.cpp
