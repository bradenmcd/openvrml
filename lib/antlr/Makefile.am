LIBRARY_VERSION = 0:0:0
#                 | | |
#          +------+ | +---+
#          |        |     |
#       current:revision:age
#          |        |     |
#          |        |     +- Increment if interfaces have been added
#          |        |        Set to zero if interfaces have been removed or
#          |        |        changed
#          |        +- Increment if source code has changed
#          |           Set to zero if current is incremented
#          +- Increment if interfaces have been added, removed or changed

if DEBUG
else
DEBUG_CPPFLAGS = -DNDEBUG
endif

AM_CPPFLAGS = $(DEBUG_CPPFLAGS)

lib_LTLIBRARIES = @INSTALL_LIBANTLR@
noinst_LTLIBRARIES = @NOINSTALL_LIBANTLR@
EXTRA_LTLIBRARIES = libantlr.la
libantlr_la_LDFLAGS = @LIBANTLR_RPATH_FLAG@ -version-info $(LIBRARY_VERSION)

libantlr_la_SOURCES = \
	ANTLRException.cpp \
	ASTFactory.cpp \
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
	LexerSharedInputState.cpp \
        MismatchedCharException.cpp \
	MismatchedTokenException.cpp \
	NoViableAltException.cpp \
        NoViableAltForCharException.cpp \
	Parser.cpp \
	ParserSharedInputState.cpp \
        RecognitionException.cpp \
	String.cpp \
	Token.cpp \
	TokenBuffer.cpp \
	TokenStreamBasicFilter.cpp \
	TokenStreamHiddenTokenFilter.cpp \
	TokenStreamSelector.cpp \
	TreeParser.cpp \
        TreeParserSharedInputState.cpp

antlrincludedir = $(includedir)@ANTLR_INCLUDEDIR@

antlrinclude_HEADERS = \
	ANTLRException.hpp \
	AST.hpp \
	ASTArray.hpp \
	ASTFactory.hpp \
	ASTNULLType.hpp \
	ASTPair.hpp \
        ASTRefCount.hpp \
        BaseAST.hpp \
	BitSet.hpp \
	CharBuffer.hpp \
	CharScanner.hpp \
        CharStreamException.hpp \
        CharStreamIOException.hpp \
        CircularQueue.hpp \
        CommonAST.hpp \
        CommonASTWithHiddenTokens.hpp \
	CommonHiddenStreamToken.hpp \
	CommonToken.hpp \
	InputBuffer.hpp \
	LLkParser.hpp \
	LexerSharedInputState.hpp \
        MismatchedCharException.hpp \
	MismatchedTokenException.hpp \
	NoViableAltException.hpp \
        NoViableAltForCharException.hpp \
	Parser.hpp \
	ParserSharedInputState.hpp \
        RecognitionException.hpp \
	RefCount.hpp \
	SemanticException.hpp \
	String.hpp \
	Token.hpp \
	TokenBuffer.hpp \
	TokenStream.hpp \
	TokenStreamBasicFilter.hpp \
        TokenStreamException.hpp \
	TokenStreamHiddenTokenFilter.hpp \
        TokenStreamIOException.hpp \
        TokenStreamRecognitionException.hpp \
        TokenStreamRetryException.hpp \
	TokenStreamSelector.hpp \
	TreeParser.hpp \
        TreeParserSharedInputState.hpp \
	config.hpp
