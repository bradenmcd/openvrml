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
# include <antlr/CommonToken.hpp>
# include "doc2.hpp"
# include "VrmlNamespace.h"
# include "node.h"
# include "script.h"
# include "proto.h"
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

Vrml97Scanner::Vrml97Scanner(::istream & istm)
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

} // namespace

namespace OpenVRML ANTLR_LBRACE


#line 473 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"

#line 476 "Vrml97Parser.cpp"
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
	OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc
) {
	
	try {      // for error handling
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				statement(scene, mfNode, vrmlNamespace, doc);
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
	OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc
) {
#line 599 "Vrml97Parser.g"
	
	OpenVRML::NodePtr node;
	OpenVRML::NodeTypePtr nodeType;
	
#line 546 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(scene, vrmlNamespace, doc);
#line 607 "Vrml97Parser.g"
			
			assert(node);
			mfNode.addNode(node);
			
#line 560 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(scene, vrmlNamespace, doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(vrmlNamespace);
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
	OpenVRML::VrmlScene & scene,
              OpenVRML::VrmlNamespace & vrmlNamespace,
              const OpenVRML::Doc2 * doc
) {
#line 615 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 594 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=node(scene, vrmlNamespace, doc, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 622 "Vrml97Parser.g"
		
		n.reset(vrmlNamespace.findNode(id1->getText()));
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in this scope.",
		std::string(), LT(0)->getLine());
		}
		
#line 621 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=node(scene, vrmlNamespace, doc, std::string());
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void Vrml97Parser::protoStatement(
	OpenVRML::VrmlScene & scene,
               OpenVRML::VrmlNamespace & scope,
               const OpenVRML::Doc2 * doc
) {
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EXTERNPROTO:
		{
			externproto(scene, scope, doc);
			break;
		}
		case KEYWORD_PROTO:
		{
			proto(scene, scope, doc);
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
	const OpenVRML::VrmlNamespace & vrmlNamespace
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
#line 855 "Vrml97Parser.g"
		
		using OpenVRML::FieldValue;
		using OpenVRML::Node;
		using OpenVRML::NodePtr;
		using OpenVRML::ScriptNode;
		using antlr::SemanticException;
		
		Node * const fromNode =
		vrmlNamespace.findNode(fromNodeId->getText());
		if (!fromNode) {
		throw SemanticException("Node \"" + fromNodeId->getText()
		+ "\" has not been defined in this scope.",
		std::string(), LT(0)->getLine());
		}
		
		FieldValue::Type fromInterfaceType =
		fromNode->nodeType.hasEventOut(fromInterfaceId->getText());
		if (fromInterfaceType == FieldValue::invalidType) {
		fromInterfaceType =
		fromNode->nodeType.hasExposedField(fromInterfaceId->getText());
		if (fromInterfaceType == FieldValue::invalidType) {
		throw SemanticException(fromNode->nodeType.id
		+ " has no eventOut or exposedField \""
		+ fromInterfaceId->getText() + "\".",
		std::string(), LT(0)->getLine());
		}
		}
		
		Node * const toNode = vrmlNamespace.findNode(toNodeId->getText());
		if (!toNode) {
		throw SemanticException("Node \"" + toNodeId->getText()
		+ "\" has not been defined in this scope.",
		std::string(), LT(0)->getLine());
		}
		
		FieldValue::Type toInterfaceType =
		toNode->nodeType.hasEventIn(toInterfaceId->getText());
		if (toInterfaceType == FieldValue::invalidType) {
		toInterfaceType =
		toNode->nodeType.hasExposedField(toInterfaceId->getText());
		if (toInterfaceType == FieldValue::invalidType) {
		throw SemanticException(toNode->nodeType.id
		+ " has no eventIn or exposedField \""
		+ toInterfaceId->getText() + "\".",
		std::string(), LT(0)->getLine());
		}
		}
		
		if (fromInterfaceType != toInterfaceType) {
		throw SemanticException("Routed interface types must match.",
		std::string(), LT(0)->getLine());
		}
		
		fromNode->addRoute(fromInterfaceId->getText(),
		NodePtr(toNode), toInterfaceId->getText());
		
#line 746 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

OpenVRML::NodePtr  Vrml97Parser::node(
	OpenVRML::VrmlScene & scene,
     OpenVRML::VrmlNamespace & scope,
     const OpenVRML::Doc2 * doc,
     const std::string & nodeId
) {
#line 913 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 763 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 913 "Vrml97Parser.g"
	
	using OpenVRML::NodeTypePtr;
	using OpenVRML::NodePtr;
	using OpenVRML::ScriptNode;
	using antlr::SemanticException;
	
	NodeTypePtr nodeType;
	
#line 775 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 927 "Vrml97Parser.g"
		
		n.reset(new ScriptNode(scene.scriptNodeClass));
		if (!nodeId.empty()) { n->setId(nodeId, &scope); }
		
		ScriptNode * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 788 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				nodeBodyElement(scope, doc, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				scriptInterfaceDeclaration(scope, doc, *scriptNode);
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
#line 938 "Vrml97Parser.g"
		
		nodeType = scope.findType(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		std::string(), LT(0)->getLine());
		}
		
		n = NodePtr(nodeType->createNode());
		
		if (!nodeId.empty()) { n->setId(nodeId, &scope); }
		
#line 834 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(scope, doc, *n);
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
	OpenVRML::VrmlScene & scene, OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 779 "Vrml97Parser.g"
	
	OpenVRML::NodeInterfaceSet interfaces;
	OpenVRML::MFString urlList;
	OpenVRML::NodeTypePtr nodeType;
	
#line 868 "Vrml97Parser.cpp"
	
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
#line 788 "Vrml97Parser.g"
		
		for (size_t i = 0; i < urlList.getLength(); ++i) {
			NodeClassMap::const_iterator pos =
		scene.nodeClassMap.find(urlList.getElement(i));
		if (pos != scene.nodeClassMap.end()) {
		nodeType = pos->second->createType(id->getText(),
		interfaces);
		break;
		}
		}
		scope.addNodeType(nodeType);
		
#line 902 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	OpenVRML::VrmlScene & scene,
      OpenVRML::VrmlNamespace & scope,
      const OpenVRML::Doc2 * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 644 "Vrml97Parser.g"
	
	NodeClassPtr nodeClass;
	
#line 921 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 650 "Vrml97Parser.g"
		nodeClass.reset(new ProtoNodeClass(scene));
#line 929 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				protoInterfaceDeclaration(scope, doc, static_cast<ProtoNodeClass &>(*nodeClass));
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		}
		match(RBRACKET);
		match(LBRACE);
		protoBody(doc, static_cast<ProtoNodeClass &>(*nodeClass));
		match(RBRACE);
#line 654 "Vrml97Parser.g"
		
		//
		// Add the new NodeClass (prototype definition) to the scene's
		// NodeClassMap.
		//
		// XXX Right now, this is just keyed on the PROTO's id, which won't
		// XXX work correctly: it could collide with a PROTO from a
		// XXX different scope. (The NodeClassMap is a global list of
		// XXX PROTOs.) Once VrmlNamespace has been retooled with a scope
		// XXX identifier, that can be used as the key here.
		//
		const NodeClassMap::value_type value(id->getText(), nodeClass);
		scene.nodeClassMap.insert(value);
		
		//
		// PROTO's implicitly introduce a new node type as well...
		//
		const NodeTypePtr nodeType =
		nodeClass->createType(id->getText(),
		static_cast<ProtoNodeClass &>(*nodeClass)
		.protoNodeType.getInterfaces());
		assert(nodeType);
		scope.addNodeType(nodeType);
		
#line 972 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	OpenVRML::VrmlNamespace & scope,
                          const OpenVRML::Doc2 * doc,
                          OpenVRML::ProtoNodeClass & proto
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 680 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using antlr::SemanticException;
	
	NodeInterface::Type it(NodeInterface::invalidType);
	OpenVRML::FieldValue::Type ft(FieldValue::invalidType);
	OpenVRML::FieldValuePtr fv;
	
#line 997 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 691 "Vrml97Parser.g"
			
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
			throw SemanticException(ex.what(), std::string(),
			LT(0)->getLine());
			}
			
#line 1028 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
			fv=fieldValue(proto.getScene(), scope, doc, ft);
#line 711 "Vrml97Parser.g"
			
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
			throw SemanticException(ex.what(), std::string(),
			LT(0)->getLine());
			}
			
#line 1060 "Vrml97Parser.cpp"
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto
) {
#line 743 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 1083 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((LA(1)==KEYWORD_EXTERNPROTO||LA(1)==KEYWORD_PROTO)) {
				protoStatement(proto.getScene(), proto.protoNode.scope, doc);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		}
		n=protoNodeStatement(doc, proto);
#line 748 "Vrml97Parser.g"
		assert(n); proto.addRootNode(n);
#line 1101 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(doc, proto);
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
#line 733 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1125 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 734 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::eventIn;
#line 1134 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 735 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::eventOut;
#line 1142 "Vrml97Parser.cpp"
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
#line 1210 "Vrml97Parser.g"
	OpenVRML::FieldValue::Type ft = OpenVRML::FieldValue::invalidType;
#line 1162 "Vrml97Parser.cpp"
#line 1210 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 1167 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 1215 "Vrml97Parser.g"
			ft = FieldValue::mfcolor;
#line 1176 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 1216 "Vrml97Parser.g"
			ft = FieldValue::mffloat;
#line 1184 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 1217 "Vrml97Parser.g"
			ft = FieldValue::mfint32;
#line 1192 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 1218 "Vrml97Parser.g"
			ft = FieldValue::mfnode;
#line 1200 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 1219 "Vrml97Parser.g"
			ft = FieldValue::mfrotation;
#line 1208 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 1220 "Vrml97Parser.g"
			ft = FieldValue::mfstring;
#line 1216 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 1221 "Vrml97Parser.g"
			ft = FieldValue::mftime;
#line 1224 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 1222 "Vrml97Parser.g"
			ft = FieldValue::mfvec2f;
#line 1232 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 1223 "Vrml97Parser.g"
			ft = FieldValue::mfvec3f;
#line 1240 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 1224 "Vrml97Parser.g"
			ft = FieldValue::sfbool;
#line 1248 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 1225 "Vrml97Parser.g"
			ft = FieldValue::sfcolor;
#line 1256 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 1226 "Vrml97Parser.g"
			ft = FieldValue::sffloat;
#line 1264 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 1227 "Vrml97Parser.g"
			ft = FieldValue::sfimage;
#line 1272 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 1228 "Vrml97Parser.g"
			ft = FieldValue::sfint32;
#line 1280 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 1229 "Vrml97Parser.g"
			ft = FieldValue::sfnode;
#line 1288 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 1230 "Vrml97Parser.g"
			ft = FieldValue::sfrotation;
#line 1296 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 1231 "Vrml97Parser.g"
			ft = FieldValue::sfstring;
#line 1304 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 1232 "Vrml97Parser.g"
			ft = FieldValue::sftime;
#line 1312 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 1233 "Vrml97Parser.g"
			ft = FieldValue::sfvec2f;
#line 1320 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 1234 "Vrml97Parser.g"
			ft = FieldValue::sfvec3f;
#line 1328 "Vrml97Parser.cpp"
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
#line 738 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1348 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 739 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::field;
#line 1357 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 740 "Vrml97Parser.g"
			it = OpenVRML::NodeInterface::exposedField;
#line 1365 "Vrml97Parser.cpp"
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
	OpenVRML::VrmlScene & scene,
           OpenVRML::VrmlNamespace & scope,
           const OpenVRML::Doc2 * doc,
           OpenVRML::FieldValue::Type ft
) {
#line 1237 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 1390 "Vrml97Parser.cpp"
#line 1237 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 1395 "Vrml97Parser.cpp"
	
	if (((_tokenSet_10.member(LA(1))))&&( (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) )) {
		fv=nodeFieldValue(scene, scope, doc, ft);
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto
) {
#line 761 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 1415 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 761 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 1422 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(doc, proto, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 768 "Vrml97Parser.g"
		
		n.reset(proto.protoNode.scope.findNode(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in this scope.",
		std::string(), LT(0)->getLine());
		}
		
#line 1447 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=protoNode(doc, proto, std::string());
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto
) {
#line 752 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 1470 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, proto);
#line 756 "Vrml97Parser.g"
			assert(n); proto.addRootNode(n);
#line 1481 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(proto.getScene(), proto.protoNode.scope, doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(proto.protoNode.scope);
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto,
          const std::string & nodeId
) {
#line 1035 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 1514 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1035 "Vrml97Parser.g"
	
	using OpenVRML::NodeTypePtr;
	using OpenVRML::NodePtr;
	using OpenVRML::ScriptNode;
	using antlr::SemanticException;
	
	NodeTypePtr nodeType;
	
#line 1526 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1047 "Vrml97Parser.g"
		
		n.reset(new ScriptNode(proto.getScene().scriptNodeClass));
		if (!nodeId.empty()) { n->setId(nodeId, &proto.protoNode.scope); }
		
		ScriptNode * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 1539 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(doc, proto, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(doc, proto, *scriptNode);
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
#line 1059 "Vrml97Parser.g"
		
		nodeType = proto.protoNode.scope.findType(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		std::string(), LT(0)->getLine());
		}
		n = nodeType->createNode();
		if (!nodeId.empty()) { n->setId(nodeId, &proto.protoNode.scope); }
		
#line 1583 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(doc, proto, *n);
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
#line 802 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 1618 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 810 "Vrml97Parser.g"
		
		const NodeInterface interface(it, ft, id->getText());
		try {
		interfaces.add(interface);
		} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(), std::string(),
		LT(0)->getLine());
		}
		
#line 1635 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

OpenVRML::MFString  Vrml97Parser::externprotoUrlList() {
#line 827 "Vrml97Parser.g"
	OpenVRML::MFString urlList;
#line 1647 "Vrml97Parser.cpp"
#line 827 "Vrml97Parser.g"
	
	using std::string;
	using std::vector;
	using OpenVRML::MFString;
	
	string s;
	
#line 1656 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 835 "Vrml97Parser.g"
			urlList = MFString(1, &s);
#line 1665 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 836 "Vrml97Parser.g"
			vector<string> stringVector;
#line 1673 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 837 "Vrml97Parser.g"
					stringVector.push_back(s);
#line 1680 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			}
			match(RBRACKET);
#line 838 "Vrml97Parser.g"
			urlList = MFString(stringVector.size(), &stringVector[0]);
#line 1692 "Vrml97Parser.cpp"
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
#line 821 "Vrml97Parser.g"
	OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType;
#line 1712 "Vrml97Parser.cpp"
	
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
#line 1558 "Vrml97Parser.g"
	std::string str;
#line 1745 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	s = LT(1);
	match(STRING);
#line 1561 "Vrml97Parser.g"
	
	//
	// Why doesn't this work?
	//
	// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
	
	std::string temp(s->getText());
	str = std::string(temp.begin() + 1, temp.end() - 1);
	
#line 1760 "Vrml97Parser.cpp"
	return str;
}

void Vrml97Parser::nodeBodyElement(
	OpenVRML::VrmlNamespace & scope,
                const OpenVRML::Doc2 * doc,
                OpenVRML::Node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 952 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 1777 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 961 "Vrml97Parser.g"
			
			ft = node.nodeType.hasField(id->getText());
			if (ft == FieldValue::invalidType) {
			ft = node.nodeType.hasExposedField(id->getText());
			if (ft == FieldValue::invalidType) {
			throw SemanticException(node.nodeType.id
			+ " node has no field or exposedField \""
			+ id->getText() + "\"",
			std::string(), LT(0)->getLine());
			}
			}
			
#line 1798 "Vrml97Parser.cpp"
			fv=fieldValue(node.nodeType.nodeClass.getScene(), scope, doc, ft);
#line 973 "Vrml97Parser.g"
			
			assert(fv);
			node.setField(id->getText(), *fv);
			
#line 1805 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(scope);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(node.nodeType.nodeClass.getScene(), scope, doc);
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
	OpenVRML::VrmlNamespace & scope,
                           const OpenVRML::Doc2 * doc,
                           OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 981 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 1846 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 991 "Vrml97Parser.g"
			
			if (node.nodeType.hasInterface(id->getText())
			!= FieldValue::invalidType) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script node.",
			std::string(), LT(0)->getLine());
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
			
#line 1876 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FIELD:
		{
			scriptFieldInterfaceDeclaration(scope, doc, node);
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
	OpenVRML::VrmlNamespace & scope,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1012 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft = FieldValue::invalidType;
	FieldValuePtr fv;
	
#line 1911 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(node.nodeType.nodeClass.getScene(), scope, doc, ft);
#line 1023 "Vrml97Parser.g"
		
		assert(fv);
		if (node.nodeType.hasInterface(id->getText())
		!= FieldValue::invalidType) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script node.",
		std::string(), LT(0)->getLine());
		}
		node.addField(id->getText(), fv);
		
#line 1930 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	const OpenVRML::Doc2 * doc,
                     OpenVRML::ProtoNodeClass & proto,
                     OpenVRML::Node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1072 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 1954 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_ROUTE:
		{
			routeStatement(proto.protoNode.scope);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(proto.getScene(), proto.protoNode.scope, doc);
			break;
		}
		default:
			if (((LA(1)==ID))&&( node.nodeType.hasField(LT(1)->getText())
        || node.nodeType.hasExposedField(LT(1)->getText()) )) {
				id = LT(1);
				match(ID);
#line 1083 "Vrml97Parser.g"
				
				ft = node.nodeType.hasField(id->getText());
				if (ft == FieldValue::invalidType) {
				ft = node.nodeType.hasExposedField(id->getText());
				if (ft == FieldValue::invalidType) {
				throw SemanticException(node.nodeType.id
				+ " node has no field or exposedField \""
				+ id->getText() + "\".",
				std::string(), LT(0)->getLine());
				}
				}
				
#line 1987 "Vrml97Parser.cpp"
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
					fv=protoFieldValue(doc, proto, ft);
#line 1096 "Vrml97Parser.g"
					
					assert(fv);
					node.setField(id->getText(), *fv);
					
#line 2009 "Vrml97Parser.cpp"
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
	const OpenVRML::Doc2 * doc,
                                OpenVRML::ProtoNodeClass & proto,
                                OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1120 "Vrml97Parser.g"
	
	using OpenVRML::NodeInterface;
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	NodeInterface::Type it(NodeInterface::invalidType);
	FieldValue::Type ft(FieldValue::invalidType);
	
#line 2056 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1130 "Vrml97Parser.g"
			
			if (node.nodeType.hasInterface(id->getText())) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script node.",
			std::string(), LT(0)->getLine());
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
			
#line 2085 "Vrml97Parser.cpp"
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
			protoScriptFieldInterfaceDeclaration(doc, proto, node);
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
	const OpenVRML::Doc2 * doc,
                OpenVRML::ProtoNodeClass & proto,
                OpenVRML::FieldValue::Type ft
) {
#line 1251 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2137 "Vrml97Parser.cpp"
#line 1251 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2142 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&( (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) )) {
			fv=protoNodeFieldValue(doc, proto, ft);
#line 1259 "Vrml97Parser.g"
			assert(fv);
#line 2149 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 1260 "Vrml97Parser.g"
			assert(fv);
#line 2155 "Vrml97Parser.cpp"
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
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 1110 "Vrml97Parser.g"
		
		try {
		proto.addIS(node, nodeInterfaceId, id->getText());
		} catch (std::invalid_argument & ex) {
		throw antlr::SemanticException(ex.what(), std::string(),
		LT(0)->getLine());
		}
		
#line 2189 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	const OpenVRML::Doc2 * doc,
                                     OpenVRML::ProtoNodeClass & proto,
                                     OpenVRML::ScriptNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  protoFieldId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1150 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	using antlr::SemanticException;
	
	FieldValue::Type ft(FieldValue::invalidType);
	FieldValuePtr fv;
	
#line 2213 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1160 "Vrml97Parser.g"
		
		if (node.nodeType.hasInterface(id->getText())) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script node.",
		std::string(), LT(0)->getLine());
		}
		
		
#line 2229 "Vrml97Parser.cpp"
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
			fv=protoFieldValue(doc, proto, ft);
#line 1169 "Vrml97Parser.g"
			
			assert(fv);
			node.addField(id->getText(), fv);
			
#line 2251 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
			{
			match(KEYWORD_IS);
			protoFieldId = LT(1);
			match(ID);
#line 1175 "Vrml97Parser.g"
			
			//
			// First, get the field value from the ProtoNodeClass'
			// fieldValueMap.
			//
			ProtoNodeClass::DefaultValueMap::const_iterator pos =
			proto.defaultValueMap.find(protoFieldId->getText());
			if (pos == proto.defaultValueMap.end()) {
			throw SemanticException("Proto has no field \""
			+ protoFieldId->getText()
			+ "\".", std::string(),
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
			} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(), std::string(),
			LT(0)->getLine());
			}
			
#line 2293 "Vrml97Parser.cpp"
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
	OpenVRML::VrmlScene & scene,
               OpenVRML::VrmlNamespace & scope,
               const OpenVRML::Doc2 * doc,
               OpenVRML::FieldValue::Type ft
) {
#line 1289 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2319 "Vrml97Parser.cpp"
#line 1289 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2324 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == FieldValue::sfnode )) {
		fv=sfNodeValue(scene, scope, doc);
	}
	else if ((_tokenSet_15.member(LA(1)))) {
		fv=mfNodeValue(scene, scope, doc);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::nonNodeFieldValue(
	OpenVRML::FieldValue::Type ft
) {
#line 1263 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2344 "Vrml97Parser.cpp"
#line 1263 "Vrml97Parser.g"
	
	using OpenVRML::FieldValue;
	
#line 2349 "Vrml97Parser.cpp"
	
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
	const OpenVRML::Doc2 * doc,
                    OpenVRML::ProtoNodeClass & proto,
                    OpenVRML::FieldValue::Type ft
) {
#line 1302 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr fv;
#line 2419 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == OpenVRML::FieldValue::sfnode )) {
		fv=protoSfNodeValue(doc, proto);
	}
	else if ((_tokenSet_15.member(LA(1)))) {
		fv=protoMfNodeValue(doc, proto);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfBoolValue() {
#line 1311 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr sbv;
#line 2437 "Vrml97Parser.cpp"
#line 1311 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2442 "Vrml97Parser.cpp"
	
	val=boolValue();
#line 1316 "Vrml97Parser.g"
	sbv.reset(new SFBool(val));
#line 2447 "Vrml97Parser.cpp"
	return sbv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfColorValue() {
#line 1325 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr scv;
#line 2454 "Vrml97Parser.cpp"
#line 1325 "Vrml97Parser.g"
	
	float c[3];
	
#line 2459 "Vrml97Parser.cpp"
	
	colorValue(c);
#line 1330 "Vrml97Parser.g"
	scv.reset(new SFColor(c[0], c[1], c[2]));
#line 2464 "Vrml97Parser.cpp"
	return scv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfFloatValue() {
#line 1368 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr sfv;
#line 2471 "Vrml97Parser.cpp"
#line 1368 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2476 "Vrml97Parser.cpp"
	
	f=floatValue();
#line 1373 "Vrml97Parser.g"
	sfv.reset(new SFFloat(f));
#line 2481 "Vrml97Parser.cpp"
	return sfv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfImageValue() {
#line 1393 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr siv;
#line 2488 "Vrml97Parser.cpp"
#line 1393 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 2493 "Vrml97Parser.cpp"
	
	w=intValue();
	h=intValue();
	com=intValue();
#line 1399 "Vrml97Parser.g"
	std::vector<unsigned char> pixelVector;
#line 2500 "Vrml97Parser.cpp"
	{
	for (;;) {
		if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
			pixel=intValue();
#line 1402 "Vrml97Parser.g"
			
			// need to confirm the cross-platform-ness of this, it
			// looks kind of ugly but might in fact be ok. basically,
			// we read the value as an integer, then strip off the
			// bytes one by one.
					for (int i = com - 1; i >= 0; i--) {
			pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
			}
			
#line 2515 "Vrml97Parser.cpp"
		}
		else {
			goto _loop73;
		}
		
	}
	_loop73:;
	}
#line 1412 "Vrml97Parser.g"
	
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
	std::string(), LT(0)->getLine());
	}
	siv.reset(new SFImage(w, h, com, &pixelVector[0]));
	
#line 2540 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfInt32Value() {
#line 1429 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr siv;
#line 2547 "Vrml97Parser.cpp"
#line 1429 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2552 "Vrml97Parser.cpp"
	
	i=intValue();
#line 1434 "Vrml97Parser.g"
	siv.reset(new SFInt32(i));
#line 2557 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfRotationValue() {
#line 1498 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr srv;
#line 2564 "Vrml97Parser.cpp"
#line 1498 "Vrml97Parser.g"
	float r[4];
#line 2567 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1500 "Vrml97Parser.g"
		srv.reset(new SFRotation(r));
#line 2573 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return srv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfStringValue() {
#line 1543 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr ssv;
#line 2586 "Vrml97Parser.cpp"
#line 1543 "Vrml97Parser.g"
	std::string s;
#line 2589 "Vrml97Parser.cpp"
	
	s=stringValue();
#line 1546 "Vrml97Parser.g"
	ssv.reset(new SFString(s));
#line 2594 "Vrml97Parser.cpp"
	return ssv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfTimeValue() {
#line 1572 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr stv;
#line 2601 "Vrml97Parser.cpp"
#line 1572 "Vrml97Parser.g"
	double t(0.0);
#line 2604 "Vrml97Parser.cpp"
	
	t=doubleValue();
#line 1575 "Vrml97Parser.g"
	stv.reset(new SFTime(t));
#line 2609 "Vrml97Parser.cpp"
	return stv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfVec2fValue() {
#line 1593 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr svv;
#line 2616 "Vrml97Parser.cpp"
#line 1593 "Vrml97Parser.g"
	float v[2];
#line 2619 "Vrml97Parser.cpp"
	
	vec2fValue(v);
#line 1596 "Vrml97Parser.g"
	svv.reset(new SFVec2f(v[0], v[1]));
#line 2624 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::sfVec3fValue() {
#line 1618 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr svv;
#line 2631 "Vrml97Parser.cpp"
#line 1618 "Vrml97Parser.g"
	
	float v[3];
	
#line 2636 "Vrml97Parser.cpp"
	
	vec3fValue(v);
#line 1623 "Vrml97Parser.g"
	svv.reset(new SFVec3f(v[0], v[1], v[2]));
#line 2641 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::FieldValuePtr  Vrml97Parser::mfColorValue() {
#line 1333 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mcv;
#line 2648 "Vrml97Parser.cpp"
#line 1333 "Vrml97Parser.g"
	
	float c[3];
	
#line 2653 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		colorValue(c);
#line 1338 "Vrml97Parser.g"
		mcv.reset(new MFColor(1, c));
#line 2662 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1339 "Vrml97Parser.g"
		std::vector<float> colorVector;
#line 2670 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				colorValue(c);
#line 1340 "Vrml97Parser.g"
				colorVector.push_back(c[0]);
				colorVector.push_back(c[1]);
				colorVector.push_back(c[2]);
#line 2679 "Vrml97Parser.cpp"
			}
			else {
				goto _loop63;
			}
			
		}
		_loop63:;
		}
		match(RBRACKET);
#line 1343 "Vrml97Parser.g"
		mcv.reset(new MFColor(colorVector.size() / 3L, &colorVector[0]));
#line 2691 "Vrml97Parser.cpp"
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
#line 1376 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mfv;
#line 2705 "Vrml97Parser.cpp"
#line 1376 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2710 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		f=floatValue();
#line 1381 "Vrml97Parser.g"
		mfv.reset(new MFFloat(1, &f));
#line 2719 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1382 "Vrml97Parser.g"
		std::vector<float> floatVector;
#line 2727 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				f=floatValue();
#line 1383 "Vrml97Parser.g"
				floatVector.push_back(f);
#line 2734 "Vrml97Parser.cpp"
			}
			else {
				goto _loop69;
			}
			
		}
		_loop69:;
		}
		match(RBRACKET);
#line 1384 "Vrml97Parser.g"
		mfv.reset(new MFFloat(floatVector.size(), &floatVector[0]));
#line 2746 "Vrml97Parser.cpp"
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
#line 1437 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr miv;
#line 2760 "Vrml97Parser.cpp"
#line 1437 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2765 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case HEX_INTEGER:
	{
		i=intValue();
#line 1442 "Vrml97Parser.g"
		miv.reset(new MFInt32(1, &i));
#line 2774 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1443 "Vrml97Parser.g"
		std::vector<long> longVector;
#line 2782 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
				i=intValue();
#line 1444 "Vrml97Parser.g"
				longVector.push_back(i);
#line 2789 "Vrml97Parser.cpp"
			}
			else {
				goto _loop77;
			}
			
		}
		_loop77:;
		}
		match(RBRACKET);
#line 1445 "Vrml97Parser.g"
		miv.reset(new MFInt32(longVector.size(), &longVector[0]));
#line 2801 "Vrml97Parser.cpp"
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
#line 1503 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mrv;
#line 2815 "Vrml97Parser.cpp"
#line 1503 "Vrml97Parser.g"
	float r[4];
#line 2818 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1505 "Vrml97Parser.g"
			mrv.reset(new MFRotation(1, r));
#line 2828 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1506 "Vrml97Parser.g"
			std::vector<float> floatVector;
#line 2836 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					rotationValue(r);
#line 1507 "Vrml97Parser.g"
					floatVector.push_back(r[0]);
					floatVector.push_back(r[1]);
					floatVector.push_back(r[2]);
					floatVector.push_back(r[3]);
#line 2846 "Vrml97Parser.cpp"
				}
				else {
					goto _loop90;
				}
				
			}
			_loop90:;
			}
			match(RBRACKET);
#line 1511 "Vrml97Parser.g"
			mrv.reset(new MFRotation(floatVector.size() / 4L, &floatVector[0]));
#line 2858 "Vrml97Parser.cpp"
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
#line 1549 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr msv;
#line 2878 "Vrml97Parser.cpp"
#line 1549 "Vrml97Parser.g"
	std::string s;
#line 2881 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 1552 "Vrml97Parser.g"
		msv.reset(new MFString(1, &s));
#line 2889 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1553 "Vrml97Parser.g"
		std::vector<std::string> stringVector;
#line 2897 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==STRING)) {
				s=stringValue();
#line 1554 "Vrml97Parser.g"
				stringVector.push_back(s);
#line 2904 "Vrml97Parser.cpp"
			}
			else {
				goto _loop95;
			}
			
		}
		_loop95:;
		}
		match(RBRACKET);
#line 1555 "Vrml97Parser.g"
		msv.reset(new MFString(stringVector.size(), &stringVector[0]));
#line 2916 "Vrml97Parser.cpp"
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
#line 1578 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mtv;
#line 2930 "Vrml97Parser.cpp"
#line 1578 "Vrml97Parser.g"
	double t(0.0);
#line 2933 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		t=doubleValue();
#line 1581 "Vrml97Parser.g"
		mtv.reset(new MFTime(1, &t));
#line 2942 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1582 "Vrml97Parser.g"
		std::vector<double> doubleVector;
#line 2950 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				t=doubleValue();
#line 1583 "Vrml97Parser.g"
				doubleVector.push_back(t);
#line 2957 "Vrml97Parser.cpp"
			}
			else {
				goto _loop100;
			}
			
		}
		_loop100:;
		}
		match(RBRACKET);
#line 1584 "Vrml97Parser.g"
		mtv.reset(new MFTime(doubleVector.size(), &doubleVector[0]));
#line 2969 "Vrml97Parser.cpp"
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
#line 1599 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mvv;
#line 2983 "Vrml97Parser.cpp"
#line 1599 "Vrml97Parser.g"
	float v[2];
#line 2986 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2fValue(v);
#line 1602 "Vrml97Parser.g"
		mvv.reset(new MFVec2f(1, v));
#line 2995 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1603 "Vrml97Parser.g"
		std::vector<float> floatVector;
#line 3003 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				vec2fValue(v);
#line 1604 "Vrml97Parser.g"
				floatVector.push_back(v[0]);
				floatVector.push_back(v[1]);
#line 3011 "Vrml97Parser.cpp"
			}
			else {
				goto _loop105;
			}
			
		}
		_loop105:;
		}
		match(RBRACKET);
#line 1606 "Vrml97Parser.g"
		mvv.reset(new MFVec2f(floatVector.size() / 2L, &floatVector[0]));
#line 3023 "Vrml97Parser.cpp"
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
#line 1626 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mvv;
#line 3037 "Vrml97Parser.cpp"
#line 1626 "Vrml97Parser.g"
	
	float v[3];
	
#line 3042 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3fValue(v);
#line 1631 "Vrml97Parser.g"
		mvv.reset(new MFVec3f(1, v));
#line 3051 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1632 "Vrml97Parser.g"
		std::vector<float> floatVector;
#line 3059 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==REAL)) {
				vec3fValue(v);
#line 1633 "Vrml97Parser.g"
				floatVector.push_back(v[0]);
				floatVector.push_back(v[1]);
				floatVector.push_back(v[2]);
#line 3068 "Vrml97Parser.cpp"
			}
			else {
				goto _loop110;
			}
			
		}
		_loop110:;
		}
		match(RBRACKET);
#line 1636 "Vrml97Parser.g"
		mvv.reset(new MFVec3f(floatVector.size() / 3L, &floatVector[0]));
#line 3080 "Vrml97Parser.cpp"
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
	OpenVRML::VrmlScene & scene,
            OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc
) {
#line 1454 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr snv;
#line 3098 "Vrml97Parser.cpp"
#line 1454 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 3103 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(scene, scope, doc);
#line 1461 "Vrml97Parser.g"
			snv.reset(new SFNode(n));
#line 3114 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1462 "Vrml97Parser.g"
			snv.reset(new SFNode);
#line 3122 "Vrml97Parser.cpp"
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
	OpenVRML::VrmlScene & scene,
            OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc
) {
#line 1474 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mnv;
#line 3146 "Vrml97Parser.cpp"
#line 1474 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 3149 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(scene, scope, doc);
#line 1479 "Vrml97Parser.g"
			mnv.reset(new MFNode(1, &n));
#line 3160 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1480 "Vrml97Parser.g"
			mnv.reset(new MFNode);
#line 3168 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=nodeStatement(scene, scope, doc);
#line 1481 "Vrml97Parser.g"
					
					static_cast<MFNode &>(*mnv).addNode(n);
					
#line 3177 "Vrml97Parser.cpp"
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto
) {
#line 1465 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr snv;
#line 3208 "Vrml97Parser.cpp"
#line 1465 "Vrml97Parser.g"
	
	OpenVRML::NodePtr n;
	
#line 3213 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, proto);
#line 1470 "Vrml97Parser.g"
			snv.reset(new SFNode(n));
#line 3224 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1471 "Vrml97Parser.g"
			snv.reset(new SFNode);
#line 3232 "Vrml97Parser.cpp"
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
	const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto
) {
#line 1487 "Vrml97Parser.g"
	OpenVRML::FieldValuePtr mnv;
#line 3254 "Vrml97Parser.cpp"
#line 1487 "Vrml97Parser.g"
	OpenVRML::NodePtr n;
#line 3257 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, proto);
#line 1490 "Vrml97Parser.g"
			mnv.reset(new MFNode(1, &n));
#line 3268 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1491 "Vrml97Parser.g"
			mnv.reset(new MFNode);
#line 3276 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=protoNodeStatement(doc, proto);
#line 1492 "Vrml97Parser.g"
					
					static_cast<MFNode &>(*mnv).addNode(n);
					
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
#line 1319 "Vrml97Parser.g"
	bool val = false;
#line 3314 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_TRUE:
	{
		match(KEYWORD_TRUE);
#line 1321 "Vrml97Parser.g"
		val = true;
#line 3322 "Vrml97Parser.cpp"
		break;
	}
	case KEYWORD_FALSE:
	{
		match(KEYWORD_FALSE);
#line 1322 "Vrml97Parser.g"
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
	float c[3]
) {
#line 1346 "Vrml97Parser.g"
	
	float r(0.0f), g(0.0f), b(0.0f);
	
#line 3348 "Vrml97Parser.cpp"
	
	r=colorComponent();
	g=colorComponent();
	b=colorComponent();
#line 1351 "Vrml97Parser.g"
	c[0] = r;
	c[1] = g;
	c[2] = b;
#line 3357 "Vrml97Parser.cpp"
}

float  Vrml97Parser::colorComponent() {
#line 1359 "Vrml97Parser.g"
	float val = 0.0f;
#line 3363 "Vrml97Parser.cpp"
	
	val=floatValue();
#line 1361 "Vrml97Parser.g"
	
	if (val < 0.0 || val > 1.0) {
	this->reportWarning("Color component values must be from 0 to 1.");
	}
	
#line 3372 "Vrml97Parser.cpp"
	return val;
}

float  Vrml97Parser::floatValue() {
#line 1387 "Vrml97Parser.g"
	float val = 0.0f;
#line 3379 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		f0 = LT(1);
		match(REAL);
#line 1389 "Vrml97Parser.g"
		val = atof(f0->getText().c_str());
#line 3390 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		f1 = LT(1);
		match(INTEGER);
#line 1390 "Vrml97Parser.g"
		val = atof(f1->getText().c_str());
#line 3399 "Vrml97Parser.cpp"
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
#line 1448 "Vrml97Parser.g"
	long val = 0;
#line 3413 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case INTEGER:
	{
		i0 = LT(1);
		match(INTEGER);
#line 1450 "Vrml97Parser.g"
		val = atol(i0->getText().c_str());
#line 3424 "Vrml97Parser.cpp"
		break;
	}
	case HEX_INTEGER:
	{
		i1 = LT(1);
		match(HEX_INTEGER);
#line 1451 "Vrml97Parser.g"
		val = strtol(i1->getText().c_str(), 0, 16);
#line 3433 "Vrml97Parser.cpp"
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
	float r[4]
) {
#line 1517 "Vrml97Parser.g"
	
	using OpenVRML_::length;
	using OpenVRML_::fpequal;
	using OpenVRML_::normalize;
	float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
	
#line 3454 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
	rot=floatValue();
#line 1525 "Vrml97Parser.g"
	
	r[0] = x;
	r[1] = y;
	r[2] = z;
	r[3] = rot;
	
	const float axisLength = length(r);
	if (!fpequal(axisLength, 1.0)) {
	this->reportWarning("The axis component of a rotation must be a normalized vector.");
	if (fpequal(axisLength, 0.0)) {
	r[2] = 1.0;
	} else {
	normalize(r);
	}
	}
	
#line 3477 "Vrml97Parser.cpp"
}

double  Vrml97Parser::doubleValue() {
#line 1587 "Vrml97Parser.g"
	double val = 0.0;
#line 3483 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		d0 = LT(1);
		match(REAL);
#line 1589 "Vrml97Parser.g"
		val = atof(d0->getText().c_str());
#line 3494 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		d1 = LT(1);
		match(INTEGER);
#line 1590 "Vrml97Parser.g"
		val = atof(d1->getText().c_str());
#line 3503 "Vrml97Parser.cpp"
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
	float v[2]
) {
#line 1609 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f);
	
#line 3521 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
#line 1614 "Vrml97Parser.g"
	v[0] = x;
	v[1] = y;
#line 3528 "Vrml97Parser.cpp"
}

void Vrml97Parser::vec3fValue(
	float v[3]
) {
#line 1639 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f);
	
#line 3538 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
#line 1644 "Vrml97Parser.g"
	v[0] = x;
	v[1] = y;
	v[2] = z;
#line 3547 "Vrml97Parser.cpp"
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


