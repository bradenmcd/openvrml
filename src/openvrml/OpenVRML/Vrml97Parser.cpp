/* $ANTLR 2.7.1: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#line 116 "Vrml97Parser.g"

# include "Vrml97Parser.hpp"
ANTLR_RBRACE

#line 8 "Vrml97Parser.cpp"
#include "Vrml97Parser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 121 "Vrml97Parser.g"

# include <assert.h>
# include <ctype.h>
# include <iostream>
# include <antlr/CommonToken.hpp>
# include "scope.h"
# include "script.h"
# include "private.h"

namespace {

bool isValidIdFirstChar(char);
bool isValidIdRestChars(char);
bool isWhitespaceChar(char);
bool isNewlineChar(char);
bool isHexDigit(char);

const int Vrml97Scanner::EOF_                   (antlr::Token::EOF_TYPE);

//
// The values and names for these constants *must* match those expressed in the
// file Vrml97TokenTypes.txt.
//
const int Vrml97Scanner::PERIOD                 (4);
const int Vrml97Scanner::LBRACKET               (5);
const int Vrml97Scanner::RBRACKET               (6);
const int Vrml97Scanner::LBRACE                 (7);
const int Vrml97Scanner::RBRACE                 (8);
const int Vrml97Scanner::ID                     (9);
const int Vrml97Scanner::INTEGER                (10);
const int Vrml97Scanner::HEX_INTEGER            (11);
const int Vrml97Scanner::REAL                   (12);
const int Vrml97Scanner::STRING                 (13);
const int Vrml97Scanner::KEYWORD_DEF            (14);
const int Vrml97Scanner::KEYWORD_EVENTIN        (15);
const int Vrml97Scanner::KEYWORD_EVENTOUT       (16);
const int Vrml97Scanner::KEYWORD_EXPOSEDFIELD   (17);
const int Vrml97Scanner::KEYWORD_EXTERNPROTO    (18);
const int Vrml97Scanner::KEYWORD_FALSE          (19);
const int Vrml97Scanner::KEYWORD_FIELD          (20);
const int Vrml97Scanner::KEYWORD_IS             (21);
const int Vrml97Scanner::KEYWORD_NULL           (22);
const int Vrml97Scanner::KEYWORD_PROTO          (23);
const int Vrml97Scanner::KEYWORD_ROUTE          (24);
const int Vrml97Scanner::KEYWORD_TO             (25);
const int Vrml97Scanner::KEYWORD_TRUE           (26);
const int Vrml97Scanner::KEYWORD_USE            (27);
const int Vrml97Scanner::FIELDTYPE_SFBOOL       (28);
const int Vrml97Scanner::FIELDTYPE_SFCOLOR      (29);
const int Vrml97Scanner::FIELDTYPE_SFFLOAT      (30);
const int Vrml97Scanner::FIELDTYPE_SFIMAGE      (31);
const int Vrml97Scanner::FIELDTYPE_SFINT32      (32);
const int Vrml97Scanner::FIELDTYPE_SFNODE       (33);
const int Vrml97Scanner::FIELDTYPE_SFROTATION   (34);
const int Vrml97Scanner::FIELDTYPE_SFSTRING     (35);
const int Vrml97Scanner::FIELDTYPE_SFTIME       (36);
const int Vrml97Scanner::FIELDTYPE_SFVEC2F      (37);
const int Vrml97Scanner::FIELDTYPE_SFVEC3F      (38);
const int Vrml97Scanner::FIELDTYPE_MFCOLOR      (39);
const int Vrml97Scanner::FIELDTYPE_MFFLOAT      (40);
const int Vrml97Scanner::FIELDTYPE_MFINT32      (41);
const int Vrml97Scanner::FIELDTYPE_MFNODE       (42);
const int Vrml97Scanner::FIELDTYPE_MFROTATION   (43);
const int Vrml97Scanner::FIELDTYPE_MFSTRING     (44);
const int Vrml97Scanner::FIELDTYPE_MFTIME       (45);
const int Vrml97Scanner::FIELDTYPE_MFVEC2F      (46);
const int Vrml97Scanner::FIELDTYPE_MFVEC3F      (47);

Vrml97Scanner::Vrml97Scanner(std::istream & istm)
  : _istm(istm), _line(1), _col(0), _c(' '), _prevChar('\0'), _prevTokenType(0),
    _readTooMuch(false), _expectingFieldType(false)
{}

antlr::RefToken Vrml97Scanner::nextToken()
{
    using std::string;
    using antlr::RefToken;
    using antlr::CommonToken;

    RefToken    token(new CommonToken);
    string      tokenString;

    if (_readTooMuch) {
        _readTooMuch = false;
    } else {
        _getNextChar();
    }

    while (isWhitespaceChar(_c) || (_c == '#')) {
        if (_c == '#') {
            while (!(isNewlineChar(_c) || _c == EOF)) { _getNextChar(); }
        } else {
            _getNextChar();
        }
    }

    if (_c == EOF) {
        token->setType(EOF_);
    } else if (isValidIdFirstChar(_c)) {
        //
        // in an identifier or a keyword
        //
        token->setType(ID);

        while (isValidIdRestChars(_c)) {
            tokenString += _c;
            _getNextChar();
        }

        _readTooMuch = true;

        token->setText(tokenString);

        if (_expectingFieldType) {
            _identifyFieldType(*token);
            _expectingFieldType = false;
        }
        _identifyKeyword(*token);

    } else if ((_c == '.') || (_c == '+') || (_c == '-') || isdigit(_c)) {
        //
        // probably in a number
        //

        if ((_c == '+') || (_c == '-')) {
            tokenString += _c;
            _getNextChar();
            _readTooMuch = true;
        }

        if (isdigit(_c)) {
            //
            // definitely in a number
            //
            token->setType(INTEGER);

            tokenString += _c;

            _getNextChar();

            if ((_prevChar == '0') && ((_c == 'x') || (_c == 'X'))) {
                //
                // in an integer expressed in hexadecimal
                //
                token->setType(HEX_INTEGER);

                tokenString += _c;
                _getNextChar();
                while (isHexDigit(_c)) {
                    tokenString += _c;
                    _getNextChar();
                }
            } else {
                while (isdigit(_c)) {
                    tokenString += _c;
                    _getNextChar();
                }
            }

            if (_c == '.') {
                //
                // in a floating-point number
                //
                token->setType(REAL);

                tokenString += _c;
                _getNextChar();

                while (isdigit(_c)) {
                    tokenString += _c;
                    _getNextChar();
                }

                if ((_c == 'E') || (_c == 'e')) {
                    //
                    // in an exponent
                    //
                    tokenString += _c;
                    _getNextChar();

                    if ((_c == '+') || (_c == '-') || isdigit(_c)) {
                        //
                        // exponent may be signed
                        //
                        tokenString += _c;
                        _getNextChar();

                        while (isdigit(_c)) {
                            tokenString += _c;
                            _getNextChar();
                        }
                    }
                }
            } else if ((_c == 'E') || (_c == 'e')) {
                //
                // in an exponent
                //
                token->setType(REAL);

                tokenString += _c;
                _getNextChar();

                if ((_c == '+') || (_c == '-') || isdigit(_c)) {
                    //
                    // exponent may be signed
                    //
                    tokenString += _c;
                    _getNextChar();

                    while (isdigit(_c)) {
                        tokenString += _c;
                        _getNextChar();
                    }
                }
            }

            _readTooMuch = true;

        } else if (_c == '.') {
            //
            // in a floating-point number or a lone full-stop (as in a ROUTE)
            //

            tokenString += _c;
            _getNextChar();

            if (isdigit(_c)) {

                token->setType(REAL);

                while (isdigit(_c)) {
                    tokenString += _c;
                    _getNextChar();
                }

                if ((_c == 'E') || (_c == 'e')) {
                    //
                    // in an exponent
                    //
                    tokenString += _c;
                    _getNextChar();

                    if ((_c == '+') || (_c == '-') || isdigit(_c)) {
                        //
                        // exponent may be signed
                        //
                        tokenString += _c;
                        _getNextChar();

                        while (isdigit(_c)) {
                            tokenString += _c;
                            _getNextChar();
                        }
                    }
                }
            } else {
                token->setType(PERIOD);
            }

            _readTooMuch = true;
        }

        token->setText(tokenString);

    } else if (_c == '"') {
        //
        // in a string
        //
        token->setType(STRING);

        tokenString += _c;
        _getNextChar();

        char prevChar('\0');
        while ((_c != '"') || (prevChar == '\\')) {
            tokenString += _c;
            prevChar = _c;
            _getNextChar();
        }
        tokenString += _c; // add the closing quote

        token->setText(tokenString);

    } else {
        //
        // terminal symbol or unidentified character
        //
        tokenString += _c;

        token->setText(tokenString);

        _identifyTerminalSymbol(*token);
    }

    token->setLine(_line);
    _prevTokenType = token->getType();

    return token;
}

size_t Vrml97Scanner::line() const
{
    return _line;
}

size_t Vrml97Scanner::col() const
{
    return _col;
}

void Vrml97Scanner::_getNextChar() {
    _prevChar = _c;
    _c = _istm.get();
    ++_col; // Increment the column count;

    //
    // Increment the line count (and reset the column count to zero) if the
    // current character is a newline character EXCEPT if the current character
    // is a linefeed AND the previous character is a carriage return.
    //
    if (isNewlineChar(_c)) {
        if (!((_c == 0x0a) && (_prevChar == 0x0d))) {
            ++_line;
            _col = 0;
        }
    }
}

void Vrml97Scanner::_identifyKeyword(antlr::Token & token)
{
    std::string const tokenText(token.getText());

    if      (tokenText == "DEF")            { token.setType(KEYWORD_DEF); }
    else if (tokenText == "eventIn")        { _expectingFieldType = true;
                                              token.setType(KEYWORD_EVENTIN); }
    else if (tokenText == "eventOut")       { _expectingFieldType = true;
                                              token.setType(KEYWORD_EVENTOUT); }
    else if (tokenText == "exposedField")   { _expectingFieldType = true;
                                              token.setType(KEYWORD_EXPOSEDFIELD); }
    else if (tokenText == "EXTERNPROTO")    { token.setType(KEYWORD_EXTERNPROTO); }
    else if (tokenText == "FALSE")          { token.setType(KEYWORD_FALSE); }
    else if (tokenText == "field")          { _expectingFieldType = true;
                                              token.setType(KEYWORD_FIELD); }
    else if (tokenText == "IS")             { token.setType(KEYWORD_IS); }
    else if (tokenText == "NULL")           { token.setType(KEYWORD_NULL); }
    else if (tokenText == "PROTO")          { token.setType(KEYWORD_PROTO); }
    else if (tokenText == "ROUTE")          { token.setType(KEYWORD_ROUTE); }
    else if (tokenText == "TO")             { token.setType(KEYWORD_TO); }
    else if (tokenText == "TRUE")           { token.setType(KEYWORD_TRUE); }
    else if (tokenText == "USE")            { token.setType(KEYWORD_USE); }
}

void Vrml97Scanner::_identifyFieldType(antlr::Token & token)
{
    assert(_expectingFieldType);

    std::string const tokenText(token.getText());

    if      (tokenText == "SFBool")     { token.setType(FIELDTYPE_SFBOOL); }
    else if (tokenText == "SFColor")    { token.setType(FIELDTYPE_SFCOLOR); }
    else if (tokenText == "SFFloat")    { token.setType(FIELDTYPE_SFFLOAT); }
    else if (tokenText == "SFImage")    { token.setType(FIELDTYPE_SFIMAGE); }
    else if (tokenText == "SFInt32")    { token.setType(FIELDTYPE_SFINT32); }
    else if (tokenText == "SFNode")     { token.setType(FIELDTYPE_SFNODE); }
    else if (tokenText == "SFRotation") { token.setType(FIELDTYPE_SFROTATION); }
    else if (tokenText == "SFString")   { token.setType(FIELDTYPE_SFSTRING); }
    else if (tokenText == "SFTime")     { token.setType(FIELDTYPE_SFTIME); }
    else if (tokenText == "SFVec2f")    { token.setType(FIELDTYPE_SFVEC2F); }
    else if (tokenText == "SFVec3f")    { token.setType(FIELDTYPE_SFVEC3F); }
    else if (tokenText == "MFColor")    { token.setType(FIELDTYPE_MFCOLOR); }
    else if (tokenText == "MFFloat")    { token.setType(FIELDTYPE_MFFLOAT); }
    else if (tokenText == "MFInt32")    { token.setType(FIELDTYPE_MFINT32); }
    else if (tokenText == "MFNode")     { token.setType(FIELDTYPE_MFNODE); }
    else if (tokenText == "MFRotation") { token.setType(FIELDTYPE_MFROTATION); }
    else if (tokenText == "MFString")   { token.setType(FIELDTYPE_MFSTRING); }
    else if (tokenText == "MFTime")     { token.setType(FIELDTYPE_MFTIME); }
    else if (tokenText == "MFVec2f")    { token.setType(FIELDTYPE_MFVEC2F); }
    else if (tokenText == "MFVec3f")    { token.setType(FIELDTYPE_MFVEC3F); }
}

void Vrml97Scanner::_identifyTerminalSymbol(antlr::Token & token)
{
    std::string const tokenText(token.getText());

    if      (tokenText == "[")  { token.setType(LBRACKET); }
    else if (tokenText == "]")  { token.setType(RBRACKET); }
    else if (tokenText == "{")  { token.setType(LBRACE); }
    else if (tokenText == "}")  { token.setType(RBRACE); }
}

bool isValidIdFirstChar(char c) {
    if (((c >= 0x30) && (c <= 0x39))
        || (c == 0x2b)
        || (c == 0x2d)
        || !isValidIdRestChars(c)) {

        return false;
    }

    return true;
}

bool isValidIdRestChars(char c) {
    if (   (c <= 0x20)
        || (c == 0x22)
        || (c == 0x23)
        || (c == 0x27)
        || (c == 0x2c)
        || (c == 0x2e)
        || (c == 0x5b)
        || (c == 0x5c)
        || (c == 0x5d)
        || (c == 0x7b)
        || (c == 0x7d)
        || (c == 0x7f)) {

        return false;
    }

    return true;
}

bool isWhitespaceChar(char c) {
    if (   (c == 0x0d)      // carriage return
        || (c == 0x0a)      // linefeed
        || (c == 0x20)      // space
        || (c == 0x09)      // tab
        || (c == 0x2c)) {   // comma

        return true;
    }

    return false;
}

bool isNewlineChar(char c) {
    return ((c == 0x0a) || (c == 0x0d));
}

bool isHexDigit(char c) {
    if (          isdigit(c)
        || (c == 'A') || (c == 'a')
        || (c == 'B') || (c == 'b')
        || (c == 'C') || (c == 'c')
        || (c == 'D') || (c == 'd')
        || (c == 'E') || (c == 'e')
        || (c == 'F') || (c == 'f')) {

        return true;
    }

    return false;
}

struct InterfaceIdEquals_ : std::unary_function<OpenVRML::NodeInterface, bool> {
    explicit InterfaceIdEquals_(const std::string & interfaceId):
        interfaceId(&interfaceId)
    {}

    bool operator()(const OpenVRML::NodeInterface & interface) const
    {
        return interface.id == *this->interfaceId;
    }

private:
    const std::string * interfaceId;
};

} // namespace

namespace OpenVRML ANTLR_LBRACE


#line 486 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"

#line 489 "Vrml97Parser.cpp"
Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
	setTokenNames(_tokenNames);
}

void Vrml97Parser::vrmlScene(
	OpenVRML::Browser & browser,
          std::vector<NodePtr> & nodes
) {
#line 618 "Vrml97Parser.g"
	
	const ScopePtr scope(new Vrml97RootScope(browser, this->uri));
	
#line 528 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				statement(browser, nodes, scope);
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void Vrml97Parser::statement(
	OpenVRML::Browser & browser,
          std::vector<NodePtr> & nodes,
          const OpenVRML::ScopePtr & scope
) {
#line 626 "Vrml97Parser.g"
	
	OpenVRML::NodePtr node;
	OpenVRML::NodeTypePtr nodeType;
	
#line 561 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(browser, scope);
#line 633 "Vrml97Parser.g"
			
			//
			// If we are unable to parse a node, node will be null.
			//
			if (node) { nodes.push_back(node); }
			
#line 577 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(browser, scope);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*scope);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_2);
	}
}

OpenVRML::NodePtr  Vrml97Parser::nodeStatement(
	OpenVRML::Browser & browser,
              const OpenVRML::ScopePtr & scope
) {
#line 643 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 610 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_DEF:
		{
			match(KEYWORD_DEF);
			id0 = LT(1);
			match(ID);
			n=node(browser, scope, id0->getText());
			break;
		}
		case KEYWORD_USE:
		{
			match(KEYWORD_USE);
			id1 = LT(1);
			match(ID);
#line 649 "Vrml97Parser.g"
			
			assert(scope);
			n.reset(scope->findNode(id1->getText()));
			if (!n) {
			throw antlr::SemanticException("Node \"" + id1->getText()
			+ "\" has not been defined in this scope.",
			this->uri, LT(0)->getLine());
			}
			
#line 639 "Vrml97Parser.cpp"
			break;
		}
		case ID:
		{
			n=node(browser, scope, std::string());
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (antlr::RecognitionException & ex) {
#line 661 "Vrml97Parser.g"
		
		reportError(ex);
		
#line 658 "Vrml97Parser.cpp"
	}
	return n;
}

void Vrml97Parser::protoStatement(
	OpenVRML::Browser & browser,
               const OpenVRML::ScopePtr & scope
) {
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EXTERNPROTO:
		{
			externproto(browser, scope);
			break;
		}
		case KEYWORD_PROTO:
		{
			proto(browser, scope);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::routeStatement(
	const OpenVRML::Scope & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  fromNodeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fromInterfaceId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  toNodeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  toInterfaceId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_ROUTE);
		fromNodeId = LT(1);
		match(ID);
		match(PERIOD);
		fromInterfaceId = LT(1);
		match(ID);
		match(KEYWORD_TO);
		toNodeId = LT(1);
		match(ID);
		match(PERIOD);
		toInterfaceId = LT(1);
		match(ID);
#line 894 "Vrml97Parser.g"
		
		using OpenVRML::FieldValue;
		using OpenVRML::Node;
		using OpenVRML::NodePtr;
		using antlr::SemanticException;
		
		Node * const fromNode = scope.findNode(fromNodeId->getText());
		if (!fromNode) {
		throw SemanticException("Node \"" + fromNodeId->getText()
		+ "\" has not been defined in this scope.",
		this->uri, LT(0)->getLine());
		}
		
		Node * const toNode = scope.findNode(toNodeId->getText());
		if (!toNode) {
		throw SemanticException("Node \"" + toNodeId->getText()
		+ "\" has not been defined in this scope.",
		this->uri, LT(0)->getLine());
		}
		
		try {
		fromNode->addRoute(fromInterfaceId->getText(),
			                           NodePtr(toNode), toInterfaceId->getText());
		} catch (std::runtime_error & ex) {
		throw SemanticException(ex.what(), this->uri, LT(0)->getLine());
		}
		
#line 742 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

OpenVRML::NodePtr  Vrml97Parser::node(
	OpenVRML::Browser & browser,
     const OpenVRML::ScopePtr & scope,
     const std::string & nodeId
) {
#line 923 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 758 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 923 "Vrml97Parser.g"
	
	using OpenVRML::NodeTypePtr;
	using OpenVRML::NodePtr;
	using OpenVRML::ScriptNode;
	using antlr::SemanticException;
	
	NodeTypePtr nodeType;
	
#line 770 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 936 "Vrml97Parser.g"
		
		n.reset(new ScriptNode(browser.scriptNodeClass, scope));
		if (!nodeId.empty()) { n->setId(nodeId); }
		
		ScriptNode * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 783 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				nodeBodyElement(scope, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				scriptInterfaceDeclaration(scope, *scriptNode);
				break;
			}
			default:
			{
				goto _loop31;
			}
			}
		}
		_loop31:;
		}
		match(RBRACE);
	}
	else if ((LA(1)==ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 947 "Vrml97Parser.g"
		
		nodeType = scope->findType(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		this->uri, LT(0)->getLine());
		}
		
		n = NodePtr(nodeType->createNode(scope));
		
		if (!nodeId.empty()) { n->setId(nodeId); }
		
#line 829 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(scope, *n);
			}
			else {
				goto _loop33;
			}
			
		}
		_loop33:;
		}
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externproto(
	OpenVRML::Browser & browser, const OpenVRML::ScopePtr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 826 "Vrml97Parser.g"
	
	OpenVRML::NodeInterfaceSet interfaces;
	OpenVRML::MFString urlList;
	OpenVRML::NodeTypePtr nodeType;
	
#line 862 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_EXTERNPROTO);
		id = LT(1);
		match(ID);
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				externInterfaceDeclaration(interfaces);
			}
			else {
				goto _loop22;
			}
			
		}
		_loop22:;
		}
		match(RBRACKET);
		urlList=externprotoUrlList();
#line 834 "Vrml97Parser.g"
		
		for (size_t i = 0; i < urlList.value.size(); ++i) {
			Browser::NodeClassMap::const_iterator pos =
		browser.nodeClassMap.find(urlList.value[i]);
		if (pos != browser.nodeClassMap.end()) {
		nodeType = pos->second->createType(id->getText(),
		interfaces);
		break;
		}
		}
		//
		// If we weren't able to create a NodeType, that means that we
		// don't already have a NodeClass for the node. Currently we only
		// support referring to existing NodeClasses with EXTERNPROTO;
		// adding new NodeClasses via EXTERNPROTO is not supported. In
		// practice, this means that the ordinary way of using EXTERNPROTOs
		// in VRML worlds will fail.
		//
		if (nodeType) { scope->addNodeType(nodeType); }
		
#line 904 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	OpenVRML::Browser & browser,
      const OpenVRML::ScopePtr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 675 "Vrml97Parser.g"
	
	NodeClassPtr nodeClass;
	ScopePtr protoScope;
	
#line 923 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 681 "Vrml97Parser.g"
		
		nodeClass.reset(new ProtoNodeClass(browser));
		protoScope.reset(new Scope(id->getText(), scope));
		
#line 934 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
#line 686 "Vrml97Parser.g"
				
				// XXX Hmm... We give each interface declaration its own scope
				// XXX here. This is wasteful; Vrml97RootScope is expensive and
				// XXX we only *really* need this for SFNode and MFNode
				// XXX fields/exposedFields.
				ScopePtr interfaceDeclScope(new Vrml97RootScope(browser,
				this->uri));
				
#line 948 "Vrml97Parser.cpp"
				protoInterfaceDeclaration(interfaceDeclScope,
                                      static_cast<ProtoNodeClass &>(*nodeClass));
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		}
		match(RBRACKET);
		match(LBRACE);
		protoBody(protoScope,
                                     static_cast<ProtoNodeClass &>(*nodeClass));
		match(RBRACE);
#line 699 "Vrml97Parser.g"
		
		//
		// Add the new NodeClass (prototype definition) to the Browser's
		// NodeClassMap.
		//
		// First, construct the id for the node implementation.
		//
		std::string implId = scope->id;
		while (protoScope) {
		implId += '#' + protoScope->id;
		protoScope = protoScope->parent;
		}
		const Browser::NodeClassMap::value_type value(implId, nodeClass);
		browser.nodeClassMap.insert(value);
		
		//
		// PROTO's implicitly introduce a new node type as well...
		//
		const NodeTypePtr nodeType =
		nodeClass->createType(id->getText(),
		static_cast<ProtoNodeClass &>(*nodeClass)
		.protoNodeType.getInterfaces());
		assert(nodeType);
		assert(scope);
		scope->addNodeType(nodeType);
		
#line 991 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	const OpenVRML::ScopePtr & scope,
                          OpenVRML::ProtoNodeClass & proto
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 727 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using antlr::SemanticException;
	
	NodeInterface::Type it(NodeInterface::invalidType);
	OpenVRML::FieldValue::Type ft(FieldValue::invalidType);
	OpenVRML::FieldValuePtr fv;
	
#line 1015 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 737 "Vrml97Parser.g"
			
			try {
			switch (it) {
			case NodeInterface::eventIn:
			proto.addEventIn(ft, id0->getText());
			break;
			
			case NodeInterface::eventOut:
			proto.addEventOut(ft, id0->getText());
			break;
			
			default:
			assert(false);
			}
			} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(), this->uri, LT(0)->getLine());
			}
			
#line 1045 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
			fv=fieldValue(proto.browser, scope, ft);
#line 756 "Vrml97Parser.g"
			
			assert(fv);
			try {
			switch (it) {
			case NodeInterface::field:
			proto.addField(id1->getText(), fv);
			break;
			
			case NodeInterface::exposedField:
			proto.addExposedField(id1->getText(), fv);
			break;
			
			default:
			assert(false);
			}
			} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(), this->uri, LT(0)->getLine());
			}
			
#line 1076 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void Vrml97Parser::protoBody(
	const OpenVRML::ScopePtr & scope,
          OpenVRML::ProtoNodeClass & proto
) {
#line 787 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 1100 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((LA(1)==KEYWORD_EXTERNPROTO||LA(1)==KEYWORD_PROTO)) {
				protoStatement(proto.browser, scope);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		}
		n=protoNodeStatement(proto, scope);
#line 793 "Vrml97Parser.g"
		assert(n); proto.addRootNode(n);
#line 1118 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(proto, scope);
			}
			else {
				goto _loop17;
			}
			
		}
		_loop17:;
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_7);
	}
}

OpenVRML::NodeInterface::Type  Vrml97Parser::eventInterfaceType() {
#line 777 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1142 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 778 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::eventIn;
#line 1151 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 779 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::eventOut;
#line 1159 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

OpenVRML::FieldValue::Type  Vrml97Parser::fieldType() {
#line 1246 "Vrml97Parser.g"
	OpenVRML::FieldValue::Type ft = OpenVRML::FieldValue::invalidType;
#line 1179 "Vrml97Parser.cpp"
#line 1246 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 1184 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 1251 "Vrml97Parser.g"
			ft = FieldValue::mfcolor;
#line 1193 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 1252 "Vrml97Parser.g"
			ft = FieldValue::mffloat;
#line 1201 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 1253 "Vrml97Parser.g"
			ft = FieldValue::mfint32;
#line 1209 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 1254 "Vrml97Parser.g"
			ft = FieldValue::mfnode;
#line 1217 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 1255 "Vrml97Parser.g"
			ft = FieldValue::mfrotation;
#line 1225 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 1256 "Vrml97Parser.g"
			ft = FieldValue::mfstring;
#line 1233 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 1257 "Vrml97Parser.g"
			ft = FieldValue::mftime;
#line 1241 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 1258 "Vrml97Parser.g"
			ft = FieldValue::mfvec2f;
#line 1249 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 1259 "Vrml97Parser.g"
			ft = FieldValue::mfvec3f;
#line 1257 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 1260 "Vrml97Parser.g"
			ft = FieldValue::sfbool;
#line 1265 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 1261 "Vrml97Parser.g"
			ft = FieldValue::sfcolor;
#line 1273 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 1262 "Vrml97Parser.g"
			ft = FieldValue::sffloat;
#line 1281 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 1263 "Vrml97Parser.g"
			ft = FieldValue::sfimage;
#line 1289 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 1264 "Vrml97Parser.g"
			ft = FieldValue::sfint32;
#line 1297 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 1265 "Vrml97Parser.g"
			ft = FieldValue::sfnode;
#line 1305 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 1266 "Vrml97Parser.g"
			ft = FieldValue::sfrotation;
#line 1313 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 1267 "Vrml97Parser.g"
			ft = FieldValue::sfstring;
#line 1321 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 1268 "Vrml97Parser.g"
			ft = FieldValue::sftime;
#line 1329 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 1269 "Vrml97Parser.g"
			ft = FieldValue::sfvec2f;
#line 1337 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 1270 "Vrml97Parser.g"
			ft = FieldValue::sfvec3f;
#line 1345 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_9);
	}
	return ft;
}

OpenVRML::NodeInterface::Type  Vrml97Parser::fieldInterfaceType() {
#line 782 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1365 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 783 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::field;
#line 1374 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 784 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::exposedField;
#line 1382 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

OpenVRML::FieldValuePtr  Vrml97Parser::fieldValue(
	OpenVRML::Browser & browser,
           const OpenVRML::ScopePtr & scope,
           OpenVRML::FieldValue::Type ft
) {
#line 1273 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 1406 "Vrml97Parser.cpp"
#line 1273 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 1411 "Vrml97Parser.cpp"
	
	if (((_tokenSet_10.member(LA(1))))&&( (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) )) {
		fv=nodeFieldValue(browser, scope, ft);
	}
	else if ((_tokenSet_11.member(LA(1)))) {
		fv=nonNodeFieldValue(ft);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::NodePtr  Vrml97Parser::protoNodeStatement(
	OpenVRML::ProtoNodeClass & proto,
                   const OpenVRML::ScopePtr & scope
) {
#line 807 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 1432 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 807 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 1439 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(proto, scope, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 815 "Vrml97Parser.g"
		
		n.reset(scope->findNode(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in this scope.",
		this->uri, LT(0)->getLine());
		}
		
#line 1464 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=protoNode(proto, scope, std::string());
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void Vrml97Parser::protoBodyStatement(
	OpenVRML::ProtoNodeClass & proto,
                   const OpenVRML::ScopePtr & scope
) {
#line 797 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 1488 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 802 "Vrml97Parser.g"
			assert(n); proto.addRootNode(n);
#line 1499 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(proto.browser, scope);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*scope);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
}

OpenVRML::NodePtr  Vrml97Parser::protoNode(
	OpenVRML::ProtoNodeClass & proto,
          const OpenVRML::ScopePtr & scope,
          const std::string & nodeId
) {
#line 1045 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 1533 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1045 "Vrml97Parser.g"
	
	using OpenVRML::NodeTypePtr;
	using OpenVRML::NodePtr;
	using OpenVRML::ScriptNode;
	using antlr::SemanticException;
	
	NodeTypePtr nodeType;
	
#line 1545 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1058 "Vrml97Parser.g"
		
		n.reset(new ScriptNode(proto.browser.scriptNodeClass, scope));
		if (!nodeId.empty()) { n->setId(nodeId); }
		
		ScriptNode * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 1558 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(proto, scope, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(proto, scope, *scriptNode);
				break;
			}
			default:
			{
				goto _loop39;
			}
			}
		}
		_loop39:;
		}
		match(RBRACE);
	}
	else if ((LA(1)==ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 1070 "Vrml97Parser.g"
		
		nodeType = scope->findType(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		this->uri, LT(0)->getLine());
		}
		n = nodeType->createNode(scope);
		if (!nodeId.empty()) { n->setId(nodeId); }
		
#line 1602 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(proto, scope, *n);
			}
			else {
				goto _loop41;
			}
			
		}
		_loop41:;
		}
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externInterfaceDeclaration(
	OpenVRML::NodeInterfaceSet & interfaces
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 856 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 1637 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 864 "Vrml97Parser.g"
		
		const NodeInterface interface(it, ft, id->getText());
		try {
		interfaces.add(interface);
		} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(), this->uri, LT(0)->getLine());
		}
		
#line 1653 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

OpenVRML::MFString  Vrml97Parser::externprotoUrlList() {
#line 880 "Vrml97Parser.g"
	OpenVRML::MFString urlList;
#line 1665 "Vrml97Parser.cpp"
#line 880 "Vrml97Parser.g"
	
	using std::string;
	using OpenVRML::MFString;
	
	string s;
	
#line 1673 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 887 "Vrml97Parser.g"
			urlList.value.push_back(s);
#line 1682 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 888 "Vrml97Parser.g"
					urlList.value.push_back(s);
#line 1694 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
	return urlList;
}

OpenVRML::NodeInterface::Type  Vrml97Parser::interfaceType() {
#line 874 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1723 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

std::string  Vrml97Parser::stringValue() {
#line 1603 "Vrml97Parser.g"
	std::string str;
#line 1756 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	s = LT(1);
	match(STRING);
#line 1606 "Vrml97Parser.g"
	
	//
	// Why doesn't this work?
	//
	// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
	
	std::string temp(s->getText());
	str = std::string(temp.begin() + 1, temp.end() - 1);
	
#line 1771 "Vrml97Parser.cpp"
	return str;
}

void Vrml97Parser::nodeBodyElement(
	const OpenVRML::ScopePtr & scope,
                OpenVRML::Node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 961 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 1787 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 969 "Vrml97Parser.g"
			
			ft = node.nodeType.hasField(id->getText());
			if (ft == FieldValue::invalidType) {
			ft = node.nodeType.hasExposedField(id->getText());
			if (ft == FieldValue::invalidType) {
			throw SemanticException(node.nodeType.id
			+ " node has no field or exposedField \""
			+ id->getText() + "\"",
			this->uri, LT(0)->getLine());
			}
			}
			
#line 1808 "Vrml97Parser.cpp"
			fv=fieldValue(node.nodeType.nodeClass.browser, scope, ft);
#line 981 "Vrml97Parser.g"
			
			assert(fv);
			node.setField(id->getText(), *fv);
			
#line 1815 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*scope);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(node.nodeType.nodeClass.browser, scope);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::scriptInterfaceDeclaration(
	const OpenVRML::ScopePtr & scope,
                           OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 989 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 1855 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 998 "Vrml97Parser.g"
			
			const NodeInterfaceSet::const_iterator pos =
			node.nodeType.getInterfaces().findInterface(id->getText());
			if (pos != node.nodeType.getInterfaces().end()) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script node.",
			this->uri, LT(0)->getLine());
			}
			switch (it) {
			case NodeInterface::eventIn:
			node.addEventIn(ft, id->getText());
			break;
			case NodeInterface::eventOut:
			node.addEventOut(ft, id->getText());
			break;
			default:
			assert(false);
			}
			
#line 1886 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FIELD:
		{
			scriptFieldInterfaceDeclaration(scope, node);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::scriptFieldInterfaceDeclaration(
	const OpenVRML::ScopePtr & scope,
                                OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1020 "Vrml97Parser.g"
	
	using std::find_if;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft = FieldValue::invalidType;
	FieldValuePtr fv;
	
#line 1921 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(node.nodeType.nodeClass.browser, scope, ft);
#line 1031 "Vrml97Parser.g"
		
		assert(fv);
		const NodeInterfaceSet & interfaces = node.nodeType.getInterfaces();
		const NodeInterfaceSet::const_iterator pos =
		interfaces.findInterface(id->getText());
		if (pos != interfaces.end()) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script node.",
		this->uri, LT(0)->getLine());
		}
		node.addField(id->getText(), fv);
		
#line 1942 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	OpenVRML::ProtoNodeClass & proto,
                     const OpenVRML::ScopePtr & scope,
                     OpenVRML::Node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1083 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 1966 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_ROUTE:
		{
			routeStatement(*scope);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(proto.browser, scope);
			break;
		}
		default:
			if (((LA(1)==ID))&&( node.nodeType.hasField(LT(1)->getText())
        || node.nodeType.hasExposedField(LT(1)->getText()) )) {
				id = LT(1);
				match(ID);
#line 1094 "Vrml97Parser.g"
				
				ft = node.nodeType.hasField(id->getText());
				if (ft == FieldValue::invalidType) {
				ft = node.nodeType.hasExposedField(id->getText());
				if (ft == FieldValue::invalidType) {
				throw SemanticException(node.nodeType.id
				+ " node has no field or exposedField \""
				+ id->getText() + "\".",
				this->uri, LT(0)->getLine());
				}
				}
				
#line 1999 "Vrml97Parser.cpp"
				{
				switch ( LA(1)) {
				case LBRACKET:
				case ID:
				case INTEGER:
				case HEX_INTEGER:
				case REAL:
				case STRING:
				case KEYWORD_DEF:
				case KEYWORD_FALSE:
				case KEYWORD_NULL:
				case KEYWORD_TRUE:
				case KEYWORD_USE:
				{
					{
					fv=protoFieldValue(proto, scope, ft);
#line 1107 "Vrml97Parser.g"
					
					assert(fv);
					node.setField(id->getText(), *fv);
					
#line 2021 "Vrml97Parser.cpp"
					}
					break;
				}
				case KEYWORD_IS:
				{
					isStatement(proto, node, id->getText());
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
			}
			else if ((LA(1)==ID)) {
				eventId = LT(1);
				match(ID);
				isStatement(proto, node, eventId->getText());
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoScriptInterfaceDeclaration(
	OpenVRML::ProtoNodeClass & proto,
                                const OpenVRML::ScopePtr & scope,
                                OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1139 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 2068 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1149 "Vrml97Parser.g"
			
			const NodeInterfaceSet::const_iterator pos =
			node.nodeType.getInterfaces().findInterface(id->getText());
			if (pos != node.nodeType.getInterfaces().end()) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script node.",
			this->uri, LT(0)->getLine());
			}
			switch (it) {
			case NodeInterface::eventIn:
			node.addEventIn(ft, id->getText());
			break;
			case NodeInterface::eventOut:
			node.addEventOut(ft, id->getText());
			break;
			default:
			assert(false);
			}
			
#line 2099 "Vrml97Parser.cpp"
			{
			switch ( LA(1)) {
			case KEYWORD_IS:
			{
				isStatement(proto, node, id->getText());
				break;
			}
			case RBRACE:
			case ID:
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_FIELD:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		case KEYWORD_FIELD:
		{
			protoScriptFieldInterfaceDeclaration(proto, scope, node);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

OpenVRML::FieldValuePtr  Vrml97Parser::protoFieldValue(
	OpenVRML::ProtoNodeClass & proto,
                const OpenVRML::ScopePtr & scope,
                OpenVRML::FieldValue::Type ft
) {
#line 1286 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2151 "Vrml97Parser.cpp"
#line 1286 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2156 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&( (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) )) {
			fv=protoNodeFieldValue(proto, scope, ft);
#line 1294 "Vrml97Parser.g"
			assert(fv);
#line 2163 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 1295 "Vrml97Parser.g"
			assert(fv);
#line 2169 "Vrml97Parser.cpp"
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
	return fv;
}

void Vrml97Parser::isStatement(
	OpenVRML::ProtoNodeClass & proto, OpenVRML::Node & node,
            std::string const & nodeInterfaceId
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1119 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 2193 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 1124 "Vrml97Parser.g"
		
		try {
		proto.addIS(node, nodeInterfaceId, id->getText());
		} catch (std::bad_alloc & ex) {
		throw;
		} catch (std::runtime_error & ex) {
		//
		// ex should be UnsupportedInterface, NodeInterfaceTypeMismatch,
		// or FieldValueTypeMismatch.
		//
		throw SemanticException(ex.what(), this->uri, LT(0)->getLine());
		}
		
#line 2213 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	OpenVRML::ProtoNodeClass & proto,
                                     const OpenVRML::ScopePtr & scope,
                                     OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  protoFieldId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1171 "Vrml97Parser.g"
	
	using std::find_if;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 2238 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1182 "Vrml97Parser.g"
		
		//
		// We need to check if the fieldId is an exact match for any
		// existing interface for the Script node; so, we don't use
		// NodeInterfaceSet::findInterface.
		//
		const NodeInterfaceSet & interfaces = node.nodeType.getInterfaces();
		const NodeInterfaceSet::const_iterator pos =
		find_if(interfaces.begin(), interfaces.end(),
		InterfaceIdEquals_(id->getText()));
		if (pos != interfaces.end()) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script node.",
		this->uri, LT(0)->getLine());
		}
		
#line 2262 "Vrml97Parser.cpp"
		{
		switch ( LA(1)) {
		case LBRACKET:
		case ID:
		case INTEGER:
		case HEX_INTEGER:
		case REAL:
		case STRING:
		case KEYWORD_DEF:
		case KEYWORD_FALSE:
		case KEYWORD_NULL:
		case KEYWORD_TRUE:
		case KEYWORD_USE:
		{
			{
			fv=protoFieldValue(proto, scope, ft);
#line 1199 "Vrml97Parser.g"
			
			assert(fv);
			node.addField(id->getText(), fv);
			
#line 2284 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
			{
			match(KEYWORD_IS);
			protoFieldId = LT(1);
			match(ID);
#line 1205 "Vrml97Parser.g"
			
			//
			// First, get the field value from the ProtoNodeClass'
			// fieldValueMap.
			//
			ProtoNodeClass::DefaultValueMap::const_iterator pos =
			proto.defaultValueMap.find(protoFieldId->getText());
			if (pos == proto.defaultValueMap.end()) {
			throw SemanticException("Proto has no field \""
			+ protoFieldId->getText()
			+ "\".", this->uri,
			LT(0)->getLine());
			}
			
			//
			// Now, add the field to the Script node with the default
			// value.
			//
			node.addField(id->getText(), pos->second);
			
			//
			// Finally, add the IS mapping.
			//
			try {
			proto.addIS(node, id->getText(),
			protoFieldId->getText());
			} catch (std::bad_alloc & ex) {
			throw;
			} catch (std::runtime_error & ex) {
			//
			// ex should be UnsupportedInterface,
			// NodeInterfaceTypeMismatch, or FieldValueTypeMismatch.
			//
			throw SemanticException(ex.what(), this->uri,
			LT(0)->getLine());
			}
			
#line 2332 "Vrml97Parser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

OpenVRML::FieldValuePtr  Vrml97Parser::nodeFieldValue(
	OpenVRML::Browser & browser,
               const OpenVRML::ScopePtr & scope,
               OpenVRML::FieldValue::Type ft
) {
#line 1324 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2357 "Vrml97Parser.cpp"
#line 1324 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2362 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == FieldValue::sfnode )) {
		fv=sfNodeValue(browser, scope);
	}
	else if ((_tokenSet_15.member(LA(1)))) {
		fv=mfNodeValue(browser, scope);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::nonNodeFieldValue(
	OpenVRML::FieldValue::Type ft
) {
#line 1298 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv = OpenVRML::FieldValuePtr(0);
#line 2382 "Vrml97Parser.cpp"
#line 1298 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2387 "Vrml97Parser.cpp"
	
	if (((LA(1)==KEYWORD_FALSE||LA(1)==KEYWORD_TRUE))&&( ft == FieldValue::sfbool )) {
		fv=sfBoolValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sfcolor )) {
		fv=sfColorValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sffloat )) {
		fv=sfFloatValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == FieldValue::sfimage )) {
		fv=sfImageValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == FieldValue::sfint32 )) {
		fv=sfInt32Value();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sfrotation )) {
		fv=sfRotationValue();
	}
	else if (((LA(1)==STRING))&&( ft == FieldValue::sfstring )) {
		fv=sfStringValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sftime )) {
		fv=sfTimeValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sfvec2f )) {
		fv=sfVec2fValue();
	}
	else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::sfvec3f )) {
		fv=sfVec3fValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::mfcolor )) {
		fv=mfColorValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::mffloat )) {
		fv=mfFloatValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == FieldValue::mfint32 )) {
		fv=mfInt32Value();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::mfrotation )) {
		fv=mfRotationValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==STRING))&&( ft == FieldValue::mfstring )) {
		fv=mfStringValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::mftime )) {
		fv=mfTimeValue();
	}
	else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == FieldValue::mfvec2f )) {
		fv=mfVec2fValue();
	}
	else if ((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL)) {
		fv=mfVec3fValue();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::protoNodeFieldValue(
	OpenVRML::ProtoNodeClass & proto,
                    const OpenVRML::ScopePtr & scope,
                    OpenVRML::FieldValue::Type ft
) {
#line 1336 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2457 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == OpenVRML::FieldValue::sfnode )) {
		fv=protoSfNodeValue(proto, scope);
	}
	else if ((_tokenSet_15.member(LA(1)))) {
		fv=protoMfNodeValue(proto, scope);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfBoolValue() {
#line 1346 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr sbv;
#line 2475 "Vrml97Parser.cpp"
#line 1346 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2480 "Vrml97Parser.cpp"
	
	val=boolValue();
#line 1351 "Vrml97Parser.g"
	sbv.reset(new SFBool(val));
#line 2485 "Vrml97Parser.cpp"
	return sbv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfColorValue() {
#line 1360 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr scv;
#line 2492 "Vrml97Parser.cpp"
#line 1360 "Vrml97Parser.g"
	
	color c;
	
#line 2497 "Vrml97Parser.cpp"
	
	colorValue(c);
#line 1365 "Vrml97Parser.g"
	scv.reset(new SFColor(c));
#line 2502 "Vrml97Parser.cpp"
	return scv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfFloatValue() {
#line 1406 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr sfv;
#line 2509 "Vrml97Parser.cpp"
#line 1406 "Vrml97Parser.g"
	
	float f;
	
#line 2514 "Vrml97Parser.cpp"
	
	f=floatValue();
#line 1411 "Vrml97Parser.g"
	sfv.reset(new SFFloat(f));
#line 2519 "Vrml97Parser.cpp"
	return sfv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfImageValue() {
#line 1431 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr siv;
#line 2526 "Vrml97Parser.cpp"
#line 1431 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 2531 "Vrml97Parser.cpp"
	
	w=intValue();
	h=intValue();
	com=intValue();
#line 1437 "Vrml97Parser.g"
	std::vector<unsigned char> pixelVector;
#line 2538 "Vrml97Parser.cpp"
	{
	for (;;) {
		if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
			pixel=intValue();
#line 1440 "Vrml97Parser.g"
			
			// need to confirm the cross-platform-ness of this, it
			// looks kind of ugly but might in fact be ok. basically,
			// we read the value as an integer, then strip off the
			// bytes one by one.
					for (int i = com - 1; i >= 0; i--) {
			pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
			}
			
#line 2553 "Vrml97Parser.cpp"
		}
		else {
			goto _loop73;
		}
		
	}
	_loop73:;
	}
#line 1450 "Vrml97Parser.g"
	
	// if somebody gives us a really, really, really big
	// pixeltexture, then we will crash. in the age of dos
	// attacks, we have to assume that someone will feed us a
		    // too-big texture to see if we barf. good behavior
		    // would be to detect outsized w/h and bail. casting away
		    // the compiler warning is not helpful. there are other
	// bigger bugs to fry, so I guess it's ok for now.
	//
	if (pixelVector.size() != (w * h * com)) {
	throw antlr::SemanticException("Wrong number of pixel values for SFImage.",
	this->uri, LT(0)->getLine());
	}
	siv.reset(new SFImage(w, h, com, &pixelVector[0]));
	
#line 2578 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfInt32Value() {
#line 1467 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr siv;
#line 2585 "Vrml97Parser.cpp"
#line 1467 "Vrml97Parser.g"
	
	long i;
	
#line 2590 "Vrml97Parser.cpp"
	
	i=intValue();
#line 1472 "Vrml97Parser.g"
	siv.reset(new SFInt32(i));
#line 2595 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfRotationValue() {
#line 1544 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr srv;
#line 2602 "Vrml97Parser.cpp"
#line 1544 "Vrml97Parser.g"
	rotation r;
#line 2605 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1546 "Vrml97Parser.g"
		srv.reset(new SFRotation(r));
#line 2611 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return srv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfStringValue() {
#line 1586 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr ssv;
#line 2624 "Vrml97Parser.cpp"
#line 1586 "Vrml97Parser.g"
	std::string s;
#line 2627 "Vrml97Parser.cpp"
	
	s=stringValue();
#line 1589 "Vrml97Parser.g"
	ssv.reset(new SFString(s));
#line 2632 "Vrml97Parser.cpp"
	return ssv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfTimeValue() {
#line 1617 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr stv;
#line 2639 "Vrml97Parser.cpp"
#line 1617 "Vrml97Parser.g"
	double t(0.0);
#line 2642 "Vrml97Parser.cpp"
	
	t=doubleValue();
#line 1620 "Vrml97Parser.g"
	stv.reset(new SFTime(t));
#line 2647 "Vrml97Parser.cpp"
	return stv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfVec2fValue() {
#line 1640 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr svv;
#line 2654 "Vrml97Parser.cpp"
#line 1640 "Vrml97Parser.g"
	vec2f v;
#line 2657 "Vrml97Parser.cpp"
	
	vec2fValue(v);
#line 1643 "Vrml97Parser.g"
	svv.reset(new SFVec2f(v));
#line 2662 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfVec3fValue() {
#line 1666 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr svv;
#line 2669 "Vrml97Parser.cpp"
#line 1666 "Vrml97Parser.g"
	
	vec3f v;
	
#line 2674 "Vrml97Parser.cpp"
	
	vec3fValue(v);
#line 1671 "Vrml97Parser.g"
	svv.reset(new SFVec3f(v));
#line 2679 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfColorValue() {
#line 1368 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mcv = OpenVRML::FieldValuePtr(new MFColor);
#line 2686 "Vrml97Parser.cpp"
#line 1368 "Vrml97Parser.g"
	
	color c;
	MFColor & colors = static_cast<MFColor &>(*mcv);
	
#line 2692 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		colorValue(c);
#line 1375 "Vrml97Parser.g"
		colors.value.push_back(c);
#line 2701 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				colorValue(c);
#line 1376 "Vrml97Parser.g"
				colors.value.push_back(c);
#line 2713 "Vrml97Parser.cpp"
			}
			else {
				goto _loop63;
			}
			
		}
		_loop63:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mcv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfFloatValue() {
#line 1414 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mfv = OpenVRML::FieldValuePtr(new MFFloat);
#line 2736 "Vrml97Parser.cpp"
#line 1414 "Vrml97Parser.g"
	
	float f;
	MFFloat & floats = static_cast<MFFloat &>(*mfv);
	
#line 2742 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		f=floatValue();
#line 1421 "Vrml97Parser.g"
		floats.value.push_back(f);
#line 2751 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				f=floatValue();
#line 1422 "Vrml97Parser.g"
				floats.value.push_back(f);
#line 2763 "Vrml97Parser.cpp"
			}
			else {
				goto _loop69;
			}
			
		}
		_loop69:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mfv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfInt32Value() {
#line 1475 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr miv = OpenVRML::FieldValuePtr(new MFInt32);
#line 2786 "Vrml97Parser.cpp"
#line 1475 "Vrml97Parser.g"
	
	long i;
	MFInt32 & int32s = static_cast<MFInt32 &>(*miv);
	
#line 2792 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case HEX_INTEGER:
	{
		i=intValue();
#line 1482 "Vrml97Parser.g"
		int32s.value.push_back(i);
#line 2801 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
				i=intValue();
#line 1483 "Vrml97Parser.g"
				int32s.value.push_back(i);
#line 2813 "Vrml97Parser.cpp"
			}
			else {
				goto _loop77;
			}
			
		}
		_loop77:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return miv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfRotationValue() {
#line 1549 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mrv = OpenVRML::FieldValuePtr(new MFRotation);
#line 2836 "Vrml97Parser.cpp"
#line 1549 "Vrml97Parser.g"
	
	rotation r;
	MFRotation & rotations = static_cast<MFRotation &>(*mrv);
	
#line 2842 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1555 "Vrml97Parser.g"
			rotations.value.push_back(r);
#line 2852 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					rotationValue(r);
#line 1556 "Vrml97Parser.g"
					rotations.value.push_back(r);
#line 2864 "Vrml97Parser.cpp"
				}
				else {
					goto _loop90;
				}
				
			}
			_loop90:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return mrv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfStringValue() {
#line 1592 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr msv = OpenVRML::FieldValuePtr(new MFString);
#line 2893 "Vrml97Parser.cpp"
#line 1592 "Vrml97Parser.g"
	
	std::string s;
	MFString & strings = static_cast<MFString &>(*msv);
	
#line 2899 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 1599 "Vrml97Parser.g"
		strings.value.push_back(s);
#line 2907 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==STRING)) {
				s=stringValue();
#line 1600 "Vrml97Parser.g"
				strings.value.push_back(s);
#line 2919 "Vrml97Parser.cpp"
			}
			else {
				goto _loop95;
			}
			
		}
		_loop95:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return msv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfTimeValue() {
#line 1623 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mtv = OpenVRML::FieldValuePtr(new MFTime);
#line 2942 "Vrml97Parser.cpp"
#line 1623 "Vrml97Parser.g"
	
	double t;
	MFTime & times = static_cast<MFTime &>(*mtv);
	
#line 2948 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		t=doubleValue();
#line 1630 "Vrml97Parser.g"
		times.value.push_back(t);
#line 2957 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				t=doubleValue();
#line 1631 "Vrml97Parser.g"
				times.value.push_back(t);
#line 2969 "Vrml97Parser.cpp"
			}
			else {
				goto _loop100;
			}
			
		}
		_loop100:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mtv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfVec2fValue() {
#line 1646 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mvv = OpenVRML::FieldValuePtr(new MFVec2f);
#line 2992 "Vrml97Parser.cpp"
#line 1646 "Vrml97Parser.g"
	
	vec2f v;
	MFVec2f & vec2fs = static_cast<MFVec2f &>(*mvv);
	
#line 2998 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2fValue(v);
#line 1653 "Vrml97Parser.g"
		vec2fs.value.push_back(v);
#line 3007 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				vec2fValue(v);
#line 1654 "Vrml97Parser.g"
				vec2fs.value.push_back(v);
#line 3019 "Vrml97Parser.cpp"
			}
			else {
				goto _loop105;
			}
			
		}
		_loop105:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfVec3fValue() {
#line 1674 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mvv = OpenVRML::FieldValuePtr(new MFVec3f);
#line 3042 "Vrml97Parser.cpp"
#line 1674 "Vrml97Parser.g"
	
	vec3f v;
	MFVec3f & vec3fs = static_cast<MFVec3f &>(*mvv);
	
#line 3048 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3fValue(v);
#line 1681 "Vrml97Parser.g"
		vec3fs.value.push_back(v);
#line 3057 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				vec3fValue(v);
#line 1682 "Vrml97Parser.g"
				vec3fs.value.push_back(v);
#line 3069 "Vrml97Parser.cpp"
			}
			else {
				goto _loop110;
			}
			
		}
		_loop110:;
		}
		match(RBRACKET);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfNodeValue(
	OpenVRML::Browser & browser,
            const OpenVRML::ScopePtr & scope
) {
#line 1492 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr snv;
#line 3095 "Vrml97Parser.cpp"
#line 1492 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 3100 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1498 "Vrml97Parser.g"
			snv.reset(new SFNode(n));
#line 3111 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1499 "Vrml97Parser.g"
			snv.reset(new SFNode);
#line 3119 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return snv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfNodeValue(
	OpenVRML::Browser & browser,
            const OpenVRML::ScopePtr & scope
) {
#line 1512 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mnv = OpenVRML::FieldValuePtr(new MFNode);
#line 3142 "Vrml97Parser.cpp"
#line 1512 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	MFNode & nodes = static_cast<MFNode &>(*mnv);
	
#line 3148 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1519 "Vrml97Parser.g"
			if (n) { nodes.value.push_back(n); }
#line 3159 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=nodeStatement(browser, scope);
#line 1521 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3173 "Vrml97Parser.cpp"
				}
				else {
					goto _loop83;
				}
				
			}
			_loop83:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return mnv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::protoSfNodeValue(
	OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::ScopePtr & scope
) {
#line 1502 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr snv;
#line 3205 "Vrml97Parser.cpp"
#line 1502 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 3210 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 1508 "Vrml97Parser.g"
			snv.reset(new SFNode(n));
#line 3221 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1509 "Vrml97Parser.g"
			snv.reset(new SFNode);
#line 3229 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
	return snv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::protoMfNodeValue(
	OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::ScopePtr & scope
) {
#line 1527 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mnv = OpenVRML::FieldValuePtr(new MFNode);
#line 3252 "Vrml97Parser.cpp"
#line 1527 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	MFNode & nodes = static_cast<MFNode &>(*mnv);
	
#line 3258 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 1534 "Vrml97Parser.g"
			
			if (n) { nodes.value.push_back(n); }
			
#line 3271 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=protoNodeStatement(proto, scope);
#line 1538 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3285 "Vrml97Parser.cpp"
				}
				else {
					goto _loop86;
				}
				
			}
			_loop86:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
	return mnv;
}

bool  Vrml97Parser::boolValue() {
#line 1354 "Vrml97Parser.g"
	bool val = false;
#line 3314 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_TRUE:
	{
		match(KEYWORD_TRUE);
#line 1356 "Vrml97Parser.g"
		val = true;
#line 3322 "Vrml97Parser.cpp"
		break;
	}
	case KEYWORD_FALSE:
	{
		match(KEYWORD_FALSE);
#line 1357 "Vrml97Parser.g"
		val = false;
#line 3330 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void Vrml97Parser::colorValue(
	color & c
) {
#line 1379 "Vrml97Parser.g"
	
	float r, g, b;
	
#line 3348 "Vrml97Parser.cpp"
	
	r=colorComponent();
	g=colorComponent();
	b=colorComponent();
#line 1384 "Vrml97Parser.g"
	c.r(r);
	c.g(g);
	c.b(b);
#line 3357 "Vrml97Parser.cpp"
}

float  Vrml97Parser::colorComponent() {
#line 1392 "Vrml97Parser.g"
	float val = 0.0f;
#line 3363 "Vrml97Parser.cpp"
	
	val=floatValue();
#line 1394 "Vrml97Parser.g"
	
	if (val < 0.0 || val > 1.0) {
	this->reportWarning("Color component values must be from 0 to 1.");
	if (val < 0.0) {
	val = 0.0;
	} else if (val > 1.0) {
	val = 1.0;
	}
	}
	
#line 3377 "Vrml97Parser.cpp"
	return val;
}

float  Vrml97Parser::floatValue() {
#line 1425 "Vrml97Parser.g"
	float val;
#line 3384 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		f0 = LT(1);
		match(REAL);
#line 1427 "Vrml97Parser.g"
		std::istringstream(f0->getText()) >> val;
#line 3395 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		f1 = LT(1);
		match(INTEGER);
#line 1428 "Vrml97Parser.g"
		std::istringstream(f1->getText()) >> val;
#line 3404 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

long  Vrml97Parser::intValue() {
#line 1486 "Vrml97Parser.g"
	long val;
#line 3418 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case INTEGER:
	{
		i0 = LT(1);
		match(INTEGER);
#line 1488 "Vrml97Parser.g"
		std::istringstream(i0->getText()) >> val;
#line 3429 "Vrml97Parser.cpp"
		break;
	}
	case HEX_INTEGER:
	{
		i1 = LT(1);
		match(HEX_INTEGER);
#line 1489 "Vrml97Parser.g"
		std::istringstream(i1->getText()) >> val;
#line 3438 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void Vrml97Parser::rotationValue(
	rotation & r
) {
#line 1562 "Vrml97Parser.g"
	
	using OpenVRML_::fpequal;
	float x, y, z, angle;
	
#line 3457 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
	angle=floatValue();
#line 1568 "Vrml97Parser.g"
	
	r.x(x);
	r.y(y);
	r.z(z);
	r.angle(angle);
	
	const float axisLength = r.axis().length();
	if (!fpequal(axisLength, 1.0)) {
	this->reportWarning("The axis component of a rotation must be a normalized vector.");
	if (fpequal(axisLength, 0.0)) {
	r.z(1.0);
	} else {
	r.axis(r.axis().normalize());
	}
	}
	
#line 3480 "Vrml97Parser.cpp"
}

double  Vrml97Parser::doubleValue() {
#line 1634 "Vrml97Parser.g"
	double val = 0.0;
#line 3486 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		d0 = LT(1);
		match(REAL);
#line 1636 "Vrml97Parser.g"
		std::istringstream(d0->getText()) >> val;
#line 3497 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		d1 = LT(1);
		match(INTEGER);
#line 1637 "Vrml97Parser.g"
		std::istringstream(d1->getText()) >> val;
#line 3506 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void Vrml97Parser::vec2fValue(
	vec2f & v
) {
#line 1657 "Vrml97Parser.g"
	
	float x, y;
	
#line 3524 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
#line 1662 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
#line 3531 "Vrml97Parser.cpp"
}

void Vrml97Parser::vec3fValue(
	vec3f & v
) {
#line 1685 "Vrml97Parser.g"
	
	float x, y, z;
	
#line 3541 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
#line 1690 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
	v.z(z);
#line 3550 "Vrml97Parser.cpp"
}

const char* Vrml97Parser::_tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	".",
	"[",
	"]",
	"{",
	"}",
	"an identifier",
	"an integer value",
	"an integer value",
	"a floating point value",
	"a string",
	"\"DEF\"",
	"\"eventIn\"",
	"\"eventOut\"",
	"\"exposedField\"",
	"\"EXTERNPROTO\"",
	"\"FALSE\"",
	"\"field\"",
	"\"IS\"",
	"\"NULL\"",
	"\"PROTO\"",
	"\"ROUTE\"",
	"\"TO\"",
	"\"TRUE\"",
	"\"USE\"",
	"FIELDTYPE_SFBOOL",
	"FIELDTYPE_SFCOLOR",
	"FIELDTYPE_SFFLOAT",
	"FIELDTYPE_SFIMAGE",
	"FIELDTYPE_SFINT32",
	"FIELDTYPE_SFNODE",
	"FIELDTYPE_SFROTATION",
	"FIELDTYPE_SFSTRING",
	"FIELDTYPE_SFTIME",
	"FIELDTYPE_SFVEC2F",
	"FIELDTYPE_SFVEC3F",
	"FIELDTYPE_MFCOLOR",
	"FIELDTYPE_MFFLOAT",
	"FIELDTYPE_MFINT32",
	"FIELDTYPE_MFNODE",
	"FIELDTYPE_MFROTATION",
	"FIELDTYPE_MFSTRING",
	"FIELDTYPE_MFTIME",
	"FIELDTYPE_MFVEC2F",
	"FIELDTYPE_MFVEC3F",
	0
};

const unsigned long Vrml97Parser::_tokenSet_0_data_[] = { 159662592UL, 0UL, 0UL, 0UL };
// ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long Vrml97Parser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long Vrml97Parser::_tokenSet_2_data_[] = { 159662594UL, 0UL, 0UL, 0UL };
// EOF ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long Vrml97Parser::_tokenSet_3_data_[] = { 160809730UL, 0UL, 0UL, 0UL };
// EOF RBRACE ID "DEF" "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" 
// "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long Vrml97Parser::_tokenSet_4_data_[] = { 25428480UL, 0UL, 0UL, 0UL };
// ID "EXTERNPROTO" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long Vrml97Parser::_tokenSet_5_data_[] = { 1277952UL, 0UL, 0UL, 0UL };
// "eventIn" "eventOut" "exposedField" "field" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long Vrml97Parser::_tokenSet_6_data_[] = { 1278016UL, 0UL, 0UL, 0UL };
// RBRACKET "eventIn" "eventOut" "exposedField" "field" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long Vrml97Parser::_tokenSet_7_data_[] = { 256UL, 0UL, 0UL, 0UL };
// RBRACE 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long Vrml97Parser::_tokenSet_8_data_[] = { 4026531840UL, 65535UL, 0UL, 0UL };
// FIELDTYPE_SFBOOL FIELDTYPE_SFCOLOR FIELDTYPE_SFFLOAT FIELDTYPE_SFIMAGE 
// FIELDTYPE_SFINT32 FIELDTYPE_SFNODE FIELDTYPE_SFROTATION FIELDTYPE_SFSTRING 
// FIELDTYPE_SFTIME FIELDTYPE_SFVEC2F FIELDTYPE_SFVEC3F FIELDTYPE_MFCOLOR 
// FIELDTYPE_MFFLOAT FIELDTYPE_MFINT32 FIELDTYPE_MFNODE FIELDTYPE_MFROTATION 
// FIELDTYPE_MFSTRING FIELDTYPE_MFTIME FIELDTYPE_MFVEC2F FIELDTYPE_MFVEC3F 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long Vrml97Parser::_tokenSet_9_data_[] = { 512UL, 0UL, 0UL, 0UL };
// ID 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long Vrml97Parser::_tokenSet_10_data_[] = { 138428960UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long Vrml97Parser::_tokenSet_11_data_[] = { 67648544UL, 0UL, 0UL, 0UL };
// LBRACKET INTEGER HEX_INTEGER REAL STRING "FALSE" "TRUE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_11(_tokenSet_11_data_,4);
const unsigned long Vrml97Parser::_tokenSet_12_data_[] = { 159662848UL, 0UL, 0UL, 0UL };
// RBRACE ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_12(_tokenSet_12_data_,4);
const unsigned long Vrml97Parser::_tokenSet_13_data_[] = { 26575616UL, 0UL, 0UL, 0UL };
// RBRACE ID "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long Vrml97Parser::_tokenSet_14_data_[] = { 138428928UL, 0UL, 0UL, 0UL };
// ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long Vrml97Parser::_tokenSet_15_data_[] = { 134234656UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_15(_tokenSet_15_data_,4);
const unsigned long Vrml97Parser::_tokenSet_16_data_[] = { 26706752UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID "eventIn" "eventOut" "exposedField" "EXTERNPROTO" 
// "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_16(_tokenSet_16_data_,4);


