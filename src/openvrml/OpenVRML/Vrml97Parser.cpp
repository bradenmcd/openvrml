/* $ANTLR 2.7.1: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#include "Vrml97Parser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 105 "Vrml97Parser.g"

# include <assert.h>
# include <antlr/CommonToken.hpp>
# include "doc2.hpp"
# include "VrmlNamespace.h"
# include "nodetype.h"
# include "VrmlNode.h"
# include "VrmlNodeScript.h"
# include "private.h"

namespace {
    bool isValidIdFirstChar(char);
    bool isValidIdRestChars(char);
    bool isWhitespaceChar(char);
    bool isNewlineChar(char);
    bool isHexDigit(char);
}

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

namespace {
    
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
}

#line 465 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"

#line 468 "Vrml97Parser.cpp"
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
	VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	
	try {      // for error handling
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				statement(mfNode, vrmlNamespace, doc);
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
	VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 584 "Vrml97Parser.g"
	
	VrmlNodePtr node;
	
#line 531 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(vrmlNamespace, doc);
#line 589 "Vrml97Parser.g"
			
			assert(node);
			mfNode.addNode(*node);
			
#line 545 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(vrmlNamespace, doc);
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

VrmlNodePtr  Vrml97Parser::nodeStatement(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 597 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 577 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=node(vrmlNamespace, doc, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 604 "Vrml97Parser.g"
		
		n = vrmlNamespace.findNode(id1->getText());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 602 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=node(vrmlNamespace, doc, std::string());
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
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EXTERNPROTO:
		{
			externproto(vrmlNamespace, doc);
			break;
		}
		case KEYWORD_PROTO:
		{
			proto(vrmlNamespace, doc);
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
	VrmlNamespace const & vrmlNamespace
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
#line 828 "Vrml97Parser.g"
		
		const VrmlNodePtr fromNode = vrmlNamespace.findNode(fromNodeId->getText());
		if (!fromNode) {
		throw antlr::SemanticException("Node \"" + fromNodeId->getText() + "\" has not been defined in this scope.");
		}
		
		VrmlField::VrmlFieldType fromInterfaceType = VrmlField::NO_FIELD;
		
		if (   ((fromInterfaceType = fromNode->type.hasEventOut(fromInterfaceId->getText())) == VrmlField::NO_FIELD)
		&& ((fromInterfaceType = fromNode->type.hasExposedField(fromInterfaceId->getText())) == VrmlField::NO_FIELD)) {
		
		VrmlNodeScript * fromScriptNode = 0;
		if ((fromScriptNode = fromNode->toScript())) {
		fromInterfaceType = fromScriptNode->hasEventOut(fromInterfaceId->getText());
		}
		
		if (fromInterfaceType == VrmlField::NO_FIELD) {
		throw antlr::SemanticException(fromNode->type.getId() + " has no eventOut or exposedField \"" + fromInterfaceId->getText() + "\".");
		}
		}
		
		const VrmlNodePtr toNode = vrmlNamespace.findNode(toNodeId->getText());
		if (!toNode) {
		throw antlr::SemanticException("Node \"" + toNodeId->getText() + "\" has not been defined in this scope.");
		}
		
		VrmlField::VrmlFieldType toInterfaceType = VrmlField::NO_FIELD;
		
		if (   ((toInterfaceType = toNode->type.hasEventIn(toInterfaceId->getText())) == VrmlField::NO_FIELD)
		&& ((toInterfaceType = toNode->type.hasExposedField(toInterfaceId->getText())) == VrmlField::NO_FIELD)) {
		
		VrmlNodeScript * toScriptNode = 0;
		if ((toScriptNode = toNode->toScript())) {
		toInterfaceType = toScriptNode->hasEventIn(toInterfaceId->getText());
		}
		
		if (toInterfaceType == VrmlField::NO_FIELD) {
		throw antlr::SemanticException(toNode->type.getId() + " has no eventIn or exposedField \"" + toInterfaceId->getText() + "\".");
		}
		}
		
		if (fromInterfaceType != toInterfaceType) {
		throw antlr::SemanticException("Routed interface types must match.");
		}
		
		fromNode->addRoute(fromInterfaceId->getText(), toNode, toInterfaceId->getText());
		
#line 716 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

VrmlNodePtr  Vrml97Parser::node(
	VrmlNamespace & vrmlNamespace, const Doc2 * doc,
     const std::string & nodeId
) {
#line 877 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 731 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 877 "Vrml97Parser.g"
	
	NodeTypePtr nodeType(0);
	
#line 738 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 889 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType("Script");
		assert(nodeType);
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.length() > 0) {
		n->setId(nodeId, &vrmlNamespace);
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 757 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				nodeBodyElement(vrmlNamespace, doc, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				scriptInterfaceDeclaration(vrmlNamespace, doc, *scriptNode);
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
#line 908 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType(nodeTypeId->getText());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.length() > 0) {
		n->setId(nodeId, &vrmlNamespace);
		}
		
#line 804 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(vrmlNamespace, doc, *n);
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
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_EXTERNPROTO);
		id = LT(1);
		match(ID);
#line 741 "Vrml97Parser.g"
		
		NodeTypePtr nodeType(new NodeType(id->getText()));
		
#line 840 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				externInterfaceDeclaration(*nodeType);
			}
			else {
				goto _loop22;
			}
			
		}
		_loop22:;
		}
		match(RBRACKET);
#line 745 "Vrml97Parser.g"
		
		VrmlMFString * urlList = 0;
		
#line 859 "Vrml97Parser.cpp"
		urlList=externprotoUrlList();
#line 749 "Vrml97Parser.g"
		
		nodeType->setUrl(*urlList, doc);
		delete urlList;
		vrmlNamespace.addNodeType(nodeType);
		
#line 867 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 620 "Vrml97Parser.g"
		
		NodeTypePtr nodeType(new NodeType(id->getText()));
		nodeType->setScope(vrmlNamespace);
		
#line 890 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				protoInterfaceDeclaration(vrmlNamespace, doc, *nodeType);
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		}
		match(RBRACKET);
		match(LBRACE);
		protoBody(doc, *nodeType);
		match(RBRACE);
#line 627 "Vrml97Parser.g"
		
		vrmlNamespace.addNodeType(nodeType);
		
#line 912 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc,
                          NodeType & nodeType
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 632 "Vrml97Parser.g"
	
	NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	
#line 932 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 639 "Vrml97Parser.g"
			
			if (nodeType.hasInterface(id0->getText()) != VrmlField::NO_FIELD) {
			throw antlr::SemanticException("Interface \"" + id0->getText()
			+ "\" already declared for " + nodeType.getId()
			+ " node type.");
			}
			
			switch (it) {
			case EVENTIN:
			nodeType.addEventIn(id0->getText(), ft);
			break;
			
			case EVENTOUT:
			nodeType.addEventOut(id0->getText(), ft);
			break;
			
			default:
			assert(false);
			}
			
#line 964 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
#line 660 "Vrml97Parser.g"
			
			VrmlField * fv = 0;
			
#line 978 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 664 "Vrml97Parser.g"
			
			const std::auto_ptr<VrmlField> autofv(fv);
			
			if (nodeType.hasInterface(id1->getText()) != VrmlField::NO_FIELD) {
			throw antlr::SemanticException("Interface \"" + id1->getText()
			+ "\" already declared for " + nodeType.getId()
			+ " node type.");
			}
			
			switch (it) {
			case FIELD:
			nodeType.addField(id1->getText(), ft, autofv.get());
			break;
			
			case EXPOSEDFIELD:
			nodeType.addExposedField(id1->getText(), ft, autofv.get());
			break;
			
			default:
			assert(false);
			}
			
#line 1003 "Vrml97Parser.cpp"
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
	Doc2 const * doc, NodeType & nodeType
) {
#line 698 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 1026 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((LA(1)==KEYWORD_EXTERNPROTO||LA(1)==KEYWORD_PROTO)) {
				protoStatement(*nodeType.getScope(), doc);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		}
		n=protoNodeStatement(doc, nodeType);
#line 703 "Vrml97Parser.g"
		
		assert(n);
		nodeType.addNode(*n);
		
#line 1047 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(doc, nodeType);
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

Vrml97Parser::NodeInterfaceType  Vrml97Parser::eventInterfaceType() {
#line 688 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 1071 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 689 "Vrml97Parser.g"
			it = EVENTIN;
#line 1080 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 690 "Vrml97Parser.g"
			it = EVENTOUT;
#line 1088 "Vrml97Parser.cpp"
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

VrmlField::VrmlFieldType  Vrml97Parser::fieldType() {
#line 1174 "Vrml97Parser.g"
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
#line 1108 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 1175 "Vrml97Parser.g"
			ft = VrmlField::MFCOLOR;
#line 1117 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 1176 "Vrml97Parser.g"
			ft = VrmlField::MFFLOAT;
#line 1125 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 1177 "Vrml97Parser.g"
			ft = VrmlField::MFINT32;
#line 1133 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 1178 "Vrml97Parser.g"
			ft = VrmlField::MFNODE;
#line 1141 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 1179 "Vrml97Parser.g"
			ft = VrmlField::MFROTATION;
#line 1149 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 1180 "Vrml97Parser.g"
			ft = VrmlField::MFSTRING;
#line 1157 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 1181 "Vrml97Parser.g"
			ft = VrmlField::MFTIME;
#line 1165 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 1182 "Vrml97Parser.g"
			ft = VrmlField::MFVEC2F;
#line 1173 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 1183 "Vrml97Parser.g"
			ft = VrmlField::MFVEC3F;
#line 1181 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 1184 "Vrml97Parser.g"
			ft = VrmlField::SFBOOL;
#line 1189 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 1185 "Vrml97Parser.g"
			ft = VrmlField::SFCOLOR;
#line 1197 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 1186 "Vrml97Parser.g"
			ft = VrmlField::SFFLOAT;
#line 1205 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 1187 "Vrml97Parser.g"
			ft = VrmlField::SFIMAGE;
#line 1213 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 1188 "Vrml97Parser.g"
			ft = VrmlField::SFINT32;
#line 1221 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 1189 "Vrml97Parser.g"
			ft = VrmlField::SFNODE;
#line 1229 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 1190 "Vrml97Parser.g"
			ft = VrmlField::SFROTATION;
#line 1237 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 1191 "Vrml97Parser.g"
			ft = VrmlField::SFSTRING;
#line 1245 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 1192 "Vrml97Parser.g"
			ft = VrmlField::SFTIME;
#line 1253 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 1193 "Vrml97Parser.g"
			ft = VrmlField::SFVEC2F;
#line 1261 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 1194 "Vrml97Parser.g"
			ft = VrmlField::SFVEC3F;
#line 1269 "Vrml97Parser.cpp"
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

Vrml97Parser::NodeInterfaceType  Vrml97Parser::fieldInterfaceType() {
#line 693 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 1289 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 694 "Vrml97Parser.g"
			it = FIELD;
#line 1298 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 695 "Vrml97Parser.g"
			it = EXPOSEDFIELD;
#line 1306 "Vrml97Parser.cpp"
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

VrmlField *  Vrml97Parser::fieldValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
) {
#line 1197 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1328 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&(    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) )) {
			fv=nodeFieldValue(vrmlNamespace, doc, ft);
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return fv;
}

VrmlNodePtr  Vrml97Parser::protoNodeStatement(
	Doc2 const * doc, NodeType & protoNodeType
) {
#line 723 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 1356 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(doc, protoNodeType, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 730 "Vrml97Parser.g"
		
		n = protoNodeType.getScope()->findNode(id1->getText());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 1381 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=protoNode(doc, protoNodeType, std::string());
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
	Doc2 const * doc, NodeType & nodeType
) {
#line 710 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 1404 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, nodeType);
#line 715 "Vrml97Parser.g"
			
			assert(n);
			nodeType.addNode(*n);
			
#line 1418 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*nodeType.getScope(), doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*nodeType.getScope());
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

VrmlNodePtr  Vrml97Parser::protoNode(
	const Doc2 * doc, NodeType & protoNodeType,
          const std::string & nodeId
) {
#line 1003 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 1451 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1003 "Vrml97Parser.g"
	
	NodeTypePtr nodeType(0);
	
#line 1458 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1015 "Vrml97Parser.g"
		
		nodeType = protoNodeType.getScope()->findType("Script");
		assert(nodeType);
		
		n = nodeType->newNode();
		
		if (!nodeId.empty()) {
		assert(protoNodeType.getScope());
		n->setId(nodeId, protoNodeType.getScope());
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 1478 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(doc, protoNodeType, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(doc, protoNodeType, *scriptNode);
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
#line 1035 "Vrml97Parser.g"
		
		nodeType = protoNodeType.getScope()->findType(nodeTypeId->getText());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (!nodeId.empty()) {
		assert(protoNodeType.getScope());
		n->setId(nodeId, protoNodeType.getScope());
		}
		
#line 1525 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(doc, protoNodeType, *n);
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
	NodeType & nodeType
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 756 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1557 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 762 "Vrml97Parser.g"
		
		switch (it) {
		case EVENTIN:
		nodeType.addEventIn(id->getText(), ft);
		break;
		
		case EVENTOUT:
		nodeType.addEventOut(id->getText(), ft);
		break;
		
		case EXPOSEDFIELD:
		nodeType.addExposedField(id->getText(), ft);
		break;
		
		case FIELD:
		nodeType.addField(id->getText(), ft);
		break;
		
		default:
		assert(false);
		}
		
#line 1587 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

VrmlMFString *  Vrml97Parser::externprotoUrlList() {
#line 791 "Vrml97Parser.g"
	VrmlMFString * msv = 0;
#line 1599 "Vrml97Parser.cpp"
#line 791 "Vrml97Parser.g"
	
	std::string s;
	
#line 1604 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 796 "Vrml97Parser.g"
			
			msv = new VrmlMFString(1, &s);
			
#line 1615 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 800 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 1625 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 805 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 1634 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			}
			match(RBRACKET);
#line 809 "Vrml97Parser.g"
			
			msv = new VrmlMFString(stringVector.size(), &stringVector[0]);
			
#line 1648 "Vrml97Parser.cpp"
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
	return msv;
}

Vrml97Parser::NodeInterfaceType  Vrml97Parser::interfaceType() {
#line 786 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 1668 "Vrml97Parser.cpp"
	
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
#line 1597 "Vrml97Parser.g"
	std::string str;
#line 1701 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		s = LT(1);
		match(STRING);
#line 1599 "Vrml97Parser.g"
		
		//
		// Why doesn't this work?
		//
		// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
		
		std::string temp(s->getText());
		str = std::string(temp.begin() + 1, temp.end() - 1);
		
#line 1717 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_14);
	}
	return str;
}

void Vrml97Parser::nodeBodyElement(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc,
                VrmlNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 924 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1736 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 930 "Vrml97Parser.g"
			
			if (   ((ft = node.type.hasField(id->getText())) == VrmlField::NO_FIELD)
			&& ((ft = node.type.hasExposedField(id->getText())) == VrmlField::NO_FIELD)) {
			
			throw antlr::SemanticException(node.type.getId()
			+ " node has no field or exposedField \""
			+ id->getText() + "\"");
			}
			
			VrmlField * fv = 0;
			
#line 1756 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 942 "Vrml97Parser.g"
			
			assert(fv);
			node.setField(id->getText(), *fv);
			delete fv;
			
#line 1764 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(vrmlNamespace);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(vrmlNamespace, doc);
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
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::scriptInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace,
                           const Doc2 * doc,
                           VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 951 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1802 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 959 "Vrml97Parser.g"
			
			if (   (node.hasInterface(id->getText()) != VrmlField::NO_FIELD)
			|| (node.type.hasInterface(id->getText()) != VrmlField::NO_FIELD)) {
			
			throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
			}
			
			switch (it) {
			case EVENTIN:
			node.addEventIn(id->getText(), ft);
			break;
			case EVENTOUT:
			node.addEventOut(id->getText(), ft);
			break;
			default:
			assert(false);
			}
			
#line 1832 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FIELD:
		{
			scriptFieldInterfaceDeclaration(vrmlNamespace, doc, node);
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
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::scriptFieldInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace,
                                const Doc2 * doc,
                                VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 980 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	VrmlField * fv = 0;
	
#line 1864 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(vrmlNamespace, doc, ft);
#line 988 "Vrml97Parser.g"
		
		assert(fv);
		
		const std::auto_ptr<VrmlField> autofv(fv);
		
		if (   (node.hasInterface(id->getText()) != VrmlField::NO_FIELD)
		|| (node.type.hasInterface(id->getText()) != VrmlField::NO_FIELD)) {
		
		throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
		}
		
		node.addField(id->getText(), ft, autofv.get());
		
#line 1886 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	Doc2 const * doc,
                     NodeType & protoNodeType,
                     VrmlNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1051 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1906 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_ROUTE:
		{
			routeStatement(*protoNodeType.getScope());
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*protoNodeType.getScope(), doc);
			break;
		}
		default:
			if (((LA(1)==ID))&&( node.type.hasEventIn(LT(1)->getText()) != VrmlField::NO_FIELD ||
	  node.type.hasEventOut(LT(1)->getText()) != VrmlField::NO_FIELD )) {
				eventId = LT(1);
				match(ID);
				isStatement(protoNodeType, node, eventId->getText());
			}
			else if ((LA(1)==ID)) {
				id = LT(1);
				match(ID);
#line 1062 "Vrml97Parser.g"
				
				if (   ((ft = node.type.hasField(id->getText())) == VrmlField::NO_FIELD)
				&& ((ft = node.type.hasExposedField(id->getText())) == VrmlField::NO_FIELD)) {
				
				throw antlr::SemanticException(node.type.getId()
				+ " node has no field or exposedField \""
				+ id->getText() + "\".");
				}
				
				VrmlField * fv = 0;
				
#line 1943 "Vrml97Parser.cpp"
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
					fv=protoFieldValue(doc, protoNodeType, ft);
#line 1076 "Vrml97Parser.g"
					
					assert(fv);
					node.setField(id->getText(), *fv);
					delete fv;
					
#line 1966 "Vrml97Parser.cpp"
					}
					break;
				}
				case KEYWORD_IS:
				{
					isStatement(protoNodeType, node, id->getText());
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::protoScriptInterfaceDeclaration(
	Doc2 const * doc, NodeType & protoNodeType, VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1115 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 2003 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1121 "Vrml97Parser.g"
			
			if (   (node.hasInterface(id->getText()) != VrmlField::NO_FIELD)
			|| (node.type.hasInterface(id->getText()) != VrmlField::NO_FIELD)) {
			
			throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
			}
			
			switch (it) {
			case EVENTIN:
			node.addEventIn(id->getText(), ft);
			break;
			case EVENTOUT:
			node.addEventOut(id->getText(), ft);
			break;
			default:
			assert(false);
			}
			
#line 2033 "Vrml97Parser.cpp"
			{
			switch ( LA(1)) {
			case KEYWORD_IS:
			{
				isStatement(protoNodeType, node, id->getText());
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
			protoScriptFieldInterfaceDeclaration(doc, protoNodeType, node);
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
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::isStatement(
	NodeType & protoNodeType,
            VrmlNode & node,
            std::string const & nodeInterfaceId
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 1098 "Vrml97Parser.g"
		
		//
		// Okay, I was going to write some code to make sure the IS
		// statement is valid before trying to add it, but NodeType has
		// poor facilities for determining the interface type from a given
		// interface id. Also, it does not appear that NodeType
		// differentiates between an exposedField zzz and field/eventIn/
		// eventOut triplet zzz/set_zzz/zzz_changed. This differentiation
		// probably needs to happen before IS statements can be checked
		// appropriately.
		// -- Braden McDaniel <braden@endoframe.com>, 8 Apr, 2000
		//
		
		protoNodeType.addIS(id->getText(), node, nodeInterfaceId);
		
#line 2105 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

VrmlField *  Vrml97Parser::protoFieldValue(
	Doc2 const * doc, NodeType & protoNodeType, VrmlField::VrmlFieldType ft
) {
#line 1203 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 2119 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&(    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) )) {
			fv=protoNodeFieldValue(doc, protoNodeType, ft);
#line 1206 "Vrml97Parser.g"
			
			assert(fv);
			
#line 2129 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 1210 "Vrml97Parser.g"
			
			assert(fv);
			
#line 2137 "Vrml97Parser.cpp"
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
	return fv;
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	Doc2 const * doc, NodeType & protoNodeType, VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1143 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	VrmlField * fv = 0;
	
#line 2161 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1149 "Vrml97Parser.g"
		
		if (   (node.hasInterface(id->getText()) != VrmlField::NO_FIELD)
		|| (node.type.hasInterface(id->getText()) != VrmlField::NO_FIELD)) {
		
		throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
		}
		
		
#line 2177 "Vrml97Parser.cpp"
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
			fv=protoFieldValue(doc, protoNodeType, ft);
#line 1160 "Vrml97Parser.g"
			
			assert(fv);
			node.addField(id->getText(), ft, fv);
			delete fv;
			
#line 2200 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
#line 1167 "Vrml97Parser.g"
			
			node.addField(id->getText(), ft);
					
#line 2210 "Vrml97Parser.cpp"
			isStatement(protoNodeType, node, id->getText());
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
		consumeUntil(_tokenSet_15);
	}
}

VrmlField *  Vrml97Parser::nodeFieldValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
) {
#line 1236 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 2233 "Vrml97Parser.cpp"
	
	if (((_tokenSet_16.member(LA(1))))&&( ft == VrmlField::SFNODE )) {
		fv=sfNodeValue(vrmlNamespace, doc);
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		fv=mfNodeValue(vrmlNamespace, doc);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

VrmlField *  Vrml97Parser::nonNodeFieldValue(
	VrmlField::VrmlFieldType ft
) {
#line 1215 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 2253 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((LA(1)==KEYWORD_FALSE||LA(1)==KEYWORD_TRUE))&&( ft == VrmlField::SFBOOL )) {
			fv=sfBoolValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFCOLOR )) {
			fv=sfColorValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFFLOAT )) {
			fv=sfFloatValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::SFIMAGE )) {
			fv=sfImageValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::SFINT32 )) {
			fv=sfInt32Value();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFROTATION )) {
			fv=sfRotationValue();
		}
		else if (((LA(1)==STRING))&&( ft == VrmlField::SFSTRING )) {
			fv=sfStringValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFTIME )) {
			fv=sfTimeValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFVEC2F )) {
			fv=sfVec2fValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFVEC3F )) {
			fv=sfVec3fValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFCOLOR )) {
			fv=mfColorValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFFLOAT )) {
			fv=mfFloatValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::MFINT32 )) {
			fv=mfInt32Value();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFROTATION )) {
			fv=mfRotationValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==STRING))&&( ft == VrmlField::MFSTRING )) {
			fv=mfStringValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFTIME )) {
			fv=mfTimeValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFVEC2F )) {
			fv=mfVec2fValue();
		}
		else if ((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL)) {
			fv=mfVec3fValue();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return fv;
}

VrmlField *  Vrml97Parser::protoNodeFieldValue(
	Doc2 const * doc, NodeType & protoNodeType, VrmlField::VrmlFieldType ft
) {
#line 1244 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 2328 "Vrml97Parser.cpp"
	
	if (((_tokenSet_16.member(LA(1))))&&( ft == VrmlField::SFNODE )) {
		fv=protoSfNodeValue(doc, protoNodeType);
#line 1249 "Vrml97Parser.g"
		
		assert(fv);
		
#line 2336 "Vrml97Parser.cpp"
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		fv=protoMfNodeValue(doc, protoNodeType);
#line 1253 "Vrml97Parser.g"
		
		assert(fv);
		
#line 2344 "Vrml97Parser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

VrmlSFBool *  Vrml97Parser::sfBoolValue() {
#line 1258 "Vrml97Parser.g"
	VrmlSFBool * sbv = new VrmlSFBool();
#line 2356 "Vrml97Parser.cpp"
#line 1258 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2361 "Vrml97Parser.cpp"
	
	try {      // for error handling
		val=boolValue();
#line 1262 "Vrml97Parser.g"
		*sbv = VrmlSFBool(val);
#line 2367 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sbv;
}

VrmlSFColor *  Vrml97Parser::sfColorValue() {
#line 1273 "Vrml97Parser.g"
	VrmlSFColor * scv = new VrmlSFColor();
#line 2380 "Vrml97Parser.cpp"
#line 1273 "Vrml97Parser.g"
	
	float c[3];
	
#line 2385 "Vrml97Parser.cpp"
	
	try {      // for error handling
		colorValue(c);
#line 1278 "Vrml97Parser.g"
		
		*scv = VrmlSFColor(c[0], c[1], c[2]);
		
#line 2393 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return scv;
}

VrmlSFFloat *  Vrml97Parser::sfFloatValue() {
#line 1328 "Vrml97Parser.g"
	VrmlSFFloat * sfv = new VrmlSFFloat(0.0f);
#line 2406 "Vrml97Parser.cpp"
#line 1328 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2411 "Vrml97Parser.cpp"
	
	try {      // for error handling
		f=floatValue();
#line 1333 "Vrml97Parser.g"
		
		*sfv = VrmlSFFloat(f);
		
#line 2419 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sfv;
}

VrmlSFImage *  Vrml97Parser::sfImageValue() {
#line 1366 "Vrml97Parser.g"
	VrmlSFImage * siv = new VrmlSFImage();
#line 2432 "Vrml97Parser.cpp"
#line 1366 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 2437 "Vrml97Parser.cpp"
	
	try {      // for error handling
		w=intValue();
		h=intValue();
		com=intValue();
#line 1371 "Vrml97Parser.g"
		
		std::vector<unsigned char> pixelVector;
		
#line 2447 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
				pixel=intValue();
#line 1376 "Vrml97Parser.g"
				
				// need to confirm the cross-platform-ness of this, it
				// looks kind of ugly but might in fact be ok. basically,
				// we read the value as an integer, then strip off the
				// bytes one by one.
						for (int i = com - 1; i >= 0; i--) {
				pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
				}
				
#line 2462 "Vrml97Parser.cpp"
			}
			else {
				goto _loop72;
			}
			
		}
		_loop72:;
		}
#line 1386 "Vrml97Parser.g"
		
		// if somebody gives us a really, really, really big
		// pixeltexture, then we will crash. in the age of dos
		// attacks, we have to assume that someone will feed us a
			    // too-big texture to see if we barf. good behavior
			    // would be to detect outsized w/h and bail. casting away
			    // the compiler warning is not helpful. there are other 
		// bigger bugs to fry, so I guess it's ok for now.
		//
		if (pixelVector.size() != (w * h * com)) {
		throw antlr::SemanticException("Wrong number of pixel values for SFImage.");
		}
		*siv = VrmlSFImage(w, h, com, &pixelVector[0]); // hmmmm...
		
#line 2486 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFInt32 *  Vrml97Parser::sfInt32Value() {
#line 1402 "Vrml97Parser.g"
	VrmlSFInt32 * siv = new VrmlSFInt32(0L);
#line 2499 "Vrml97Parser.cpp"
#line 1402 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2504 "Vrml97Parser.cpp"
	
	try {      // for error handling
		i=intValue();
#line 1407 "Vrml97Parser.g"
		
		*siv = VrmlSFInt32(i);
		
#line 2512 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFRotation *  Vrml97Parser::sfRotationValue() {
#line 1502 "Vrml97Parser.g"
	VrmlSFRotation * srv = new VrmlSFRotation();
#line 2525 "Vrml97Parser.cpp"
#line 1502 "Vrml97Parser.g"
	
	float r[4];
	
#line 2530 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1507 "Vrml97Parser.g"
		
		*srv = VrmlSFRotation(r);
		
#line 2538 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return srv;
}

VrmlSFString *  Vrml97Parser::sfStringValue() {
#line 1564 "Vrml97Parser.g"
	VrmlSFString * ssv = new VrmlSFString();
#line 2551 "Vrml97Parser.cpp"
#line 1564 "Vrml97Parser.g"
	
	std::string s;
	
#line 2556 "Vrml97Parser.cpp"
	
	try {      // for error handling
		s=stringValue();
#line 1569 "Vrml97Parser.g"
		
		*ssv = VrmlSFString(s);
		
#line 2564 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return ssv;
}

VrmlSFTime *  Vrml97Parser::sfTimeValue() {
#line 1610 "Vrml97Parser.g"
	VrmlSFTime * stv = new VrmlSFTime(0.0);
#line 2577 "Vrml97Parser.cpp"
#line 1610 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 2582 "Vrml97Parser.cpp"
	
	try {      // for error handling
		t=doubleValue();
#line 1615 "Vrml97Parser.g"
		
		*stv = VrmlSFTime(t);
		
#line 2590 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return stv;
}

VrmlSFVec2f *  Vrml97Parser::sfVec2fValue() {
#line 1648 "Vrml97Parser.g"
	VrmlSFVec2f * svv = new VrmlSFVec2f();
#line 2603 "Vrml97Parser.cpp"
#line 1648 "Vrml97Parser.g"
	
	float v[2];
	
#line 2608 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec2fValue(v);
#line 1653 "Vrml97Parser.g"
		
		*svv = VrmlSFVec2f(v[0], v[1]);
		
#line 2616 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlSFVec3f *  Vrml97Parser::sfVec3fValue() {
#line 1693 "Vrml97Parser.g"
	VrmlSFVec3f * svv = new VrmlSFVec3f();
#line 2629 "Vrml97Parser.cpp"
#line 1693 "Vrml97Parser.g"
	
	float v[3];
	
#line 2634 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec3fValue(v);
#line 1698 "Vrml97Parser.g"
		
		*svv = VrmlSFVec3f(v[0], v[1], v[2]);
		
#line 2642 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlMFColor *  Vrml97Parser::mfColorValue() {
#line 1283 "Vrml97Parser.g"
	VrmlMFColor * mcv = new VrmlMFColor();
#line 2655 "Vrml97Parser.cpp"
#line 1283 "Vrml97Parser.g"
	
	float c[3];
	
#line 2660 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			colorValue(c);
#line 1288 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(1, c);
			
#line 2672 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1292 "Vrml97Parser.g"
			
			std::vector<float> colorVector;
			
#line 2682 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					colorValue(c);
#line 1297 "Vrml97Parser.g"
					
					colorVector.push_back(c[0]);
					colorVector.push_back(c[1]);
					colorVector.push_back(c[2]);
					
#line 2693 "Vrml97Parser.cpp"
				}
				else {
					goto _loop62;
				}
				
			}
			_loop62:;
			}
			match(RBRACKET);
#line 1303 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(colorVector.size() / 3L, &colorVector[0]);
			
#line 2707 "Vrml97Parser.cpp"
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
	return mcv;
}

VrmlMFFloat *  Vrml97Parser::mfFloatValue() {
#line 1338 "Vrml97Parser.g"
	VrmlMFFloat * mfv = new VrmlMFFloat();
#line 2727 "Vrml97Parser.cpp"
#line 1338 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2732 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			f=floatValue();
#line 1343 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(1, &f);
			
#line 2744 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1347 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2754 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					f=floatValue();
#line 1352 "Vrml97Parser.g"
					
					floatVector.push_back(f);
					
#line 2763 "Vrml97Parser.cpp"
				}
				else {
					goto _loop68;
				}
				
			}
			_loop68:;
			}
			match(RBRACKET);
#line 1356 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(floatVector.size(), &floatVector[0]);
			
#line 2777 "Vrml97Parser.cpp"
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
	return mfv;
}

VrmlMFInt32 *  Vrml97Parser::mfInt32Value() {
#line 1412 "Vrml97Parser.g"
	VrmlMFInt32 * miv = new VrmlMFInt32();
#line 2797 "Vrml97Parser.cpp"
#line 1412 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2802 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case HEX_INTEGER:
		{
			i=intValue();
#line 1417 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(1, &i);
			
#line 2814 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1421 "Vrml97Parser.g"
			
			std::vector<long> longVector;
			
#line 2824 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
					i=intValue();
#line 1426 "Vrml97Parser.g"
					
					longVector.push_back(i);
					
#line 2833 "Vrml97Parser.cpp"
				}
				else {
					goto _loop76;
				}
				
			}
			_loop76:;
			}
			match(RBRACKET);
#line 1430 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(longVector.size(), &longVector[0]);
			
#line 2847 "Vrml97Parser.cpp"
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
	return miv;
}

VrmlMFRotation *  Vrml97Parser::mfRotationValue() {
#line 1512 "Vrml97Parser.g"
	VrmlMFRotation * mrv = new VrmlMFRotation();
#line 2867 "Vrml97Parser.cpp"
#line 1512 "Vrml97Parser.g"
	
	float r[4];
	
#line 2872 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1517 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(1, r);
			
#line 2884 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1521 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2894 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					rotationValue(r);
#line 1526 "Vrml97Parser.g"
					
					floatVector.push_back(r[0]);
					floatVector.push_back(r[1]);
					floatVector.push_back(r[2]);
					floatVector.push_back(r[3]);
					
#line 2906 "Vrml97Parser.cpp"
				}
				else {
					goto _loop89;
				}
				
			}
			_loop89:;
			}
			match(RBRACKET);
#line 1533 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(floatVector.size() / 4L, &floatVector[0]);
			
#line 2920 "Vrml97Parser.cpp"
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
	return mrv;
}

VrmlMFString *  Vrml97Parser::mfStringValue() {
#line 1574 "Vrml97Parser.g"
	VrmlMFString * msv = new VrmlMFString();
#line 2940 "Vrml97Parser.cpp"
#line 1574 "Vrml97Parser.g"
	
	std::string s;
	
#line 2945 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 1579 "Vrml97Parser.g"
			
			*msv = VrmlMFString(1, &s);
			
#line 2956 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1583 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 2966 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 1588 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 2975 "Vrml97Parser.cpp"
				}
				else {
					goto _loop94;
				}
				
			}
			_loop94:;
			}
			match(RBRACKET);
#line 1592 "Vrml97Parser.g"
			
			*msv = VrmlMFString(stringVector.size(), &stringVector[0]);
			
#line 2989 "Vrml97Parser.cpp"
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
	return msv;
}

VrmlMFTime *  Vrml97Parser::mfTimeValue() {
#line 1620 "Vrml97Parser.g"
	VrmlMFTime * mtv = new VrmlMFTime();
#line 3009 "Vrml97Parser.cpp"
#line 1620 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 3014 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			t=doubleValue();
#line 1625 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(1, &t);
			
#line 3026 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1629 "Vrml97Parser.g"
			
			std::vector<double> doubleVector;
			
#line 3036 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					t=doubleValue();
#line 1634 "Vrml97Parser.g"
					
					doubleVector.push_back(t);
					
#line 3045 "Vrml97Parser.cpp"
				}
				else {
					goto _loop99;
				}
				
			}
			_loop99:;
			}
			match(RBRACKET);
#line 1638 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(doubleVector.size(), &doubleVector[0]);
			
#line 3059 "Vrml97Parser.cpp"
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
	return mtv;
}

VrmlMFVec2f *  Vrml97Parser::mfVec2fValue() {
#line 1658 "Vrml97Parser.g"
	VrmlMFVec2f * mvv = new VrmlMFVec2f();
#line 3079 "Vrml97Parser.cpp"
#line 1658 "Vrml97Parser.g"
	
	float v[2];
	
#line 3084 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec2fValue(v);
#line 1663 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(1, v);
			
#line 3096 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1667 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 3106 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec2fValue(v);
#line 1672 "Vrml97Parser.g"
					
					floatVector.push_back(v[0]);
					floatVector.push_back(v[1]);
					
#line 3116 "Vrml97Parser.cpp"
				}
				else {
					goto _loop104;
				}
				
			}
			_loop104:;
			}
			match(RBRACKET);
#line 1677 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(floatVector.size() / 2L, &floatVector[0]);
			
#line 3130 "Vrml97Parser.cpp"
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
	return mvv;
}

VrmlMFVec3f *  Vrml97Parser::mfVec3fValue() {
#line 1703 "Vrml97Parser.g"
	VrmlMFVec3f * mvv = new VrmlMFVec3f();
#line 3150 "Vrml97Parser.cpp"
#line 1703 "Vrml97Parser.g"
	
	float v[3];
	
#line 3155 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec3fValue(v);
#line 1708 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(1, v);
			
#line 3167 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1712 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 3177 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec3fValue(v);
#line 1717 "Vrml97Parser.g"
					
					floatVector.push_back(v[0]);
					floatVector.push_back(v[1]);
					floatVector.push_back(v[2]);
					
#line 3188 "Vrml97Parser.cpp"
				}
				else {
					goto _loop109;
				}
				
			}
			_loop109:;
			}
			match(RBRACKET);
#line 1723 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(floatVector.size() / 3L, &floatVector[0]);
			
#line 3202 "Vrml97Parser.cpp"
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
	return mvv;
}

VrmlSFNode *  Vrml97Parser::sfNodeValue(
	VrmlNamespace & vrmlNamespace,
            Doc2 const * doc
) {
#line 1440 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 3225 "Vrml97Parser.cpp"
#line 1440 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3230 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 1447 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 3243 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
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
	return snv;
}

VrmlMFNode *  Vrml97Parser::mfNodeValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 1466 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 3270 "Vrml97Parser.cpp"
#line 1466 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3275 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 1471 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 3288 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=nodeStatement(vrmlNamespace, doc);
#line 1478 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 3302 "Vrml97Parser.cpp"
				}
				else {
					goto _loop82;
				}
				
			}
			_loop82:;
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
		consumeUntil(_tokenSet_12);
	}
	return mnv;
}

VrmlSFNode *  Vrml97Parser::protoSfNodeValue(
	Doc2 const * doc,
                 NodeType & protoNodeType
) {
#line 1453 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 3334 "Vrml97Parser.cpp"
#line 1453 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3339 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 1460 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 3352 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
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
		consumeUntil(_tokenSet_15);
	}
	return snv;
}

VrmlMFNode *  Vrml97Parser::protoMfNodeValue(
	Doc2 const * doc, NodeType & protoNodeType
) {
#line 1484 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 3379 "Vrml97Parser.cpp"
#line 1484 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3384 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 1489 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 3397 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=protoNodeStatement(doc, protoNodeType);
#line 1496 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 3411 "Vrml97Parser.cpp"
				}
				else {
					goto _loop85;
				}
				
			}
			_loop85:;
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
		consumeUntil(_tokenSet_15);
	}
	return mnv;
}

bool  Vrml97Parser::boolValue() {
#line 1265 "Vrml97Parser.g"
	bool val;
#line 3440 "Vrml97Parser.cpp"
#line 1265 "Vrml97Parser.g"
	
	val = false;
	
#line 3445 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_TRUE:
		{
			match(KEYWORD_TRUE);
#line 1269 "Vrml97Parser.g"
			val = true;
#line 3454 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FALSE:
		{
			match(KEYWORD_FALSE);
#line 1270 "Vrml97Parser.g"
			val = false;
#line 3462 "Vrml97Parser.cpp"
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
	return val;
}

void Vrml97Parser::colorValue(
	float c[3]
) {
#line 1308 "Vrml97Parser.g"
	
	float r(0.0f), g(0.0f), b(0.0f);
	
#line 3486 "Vrml97Parser.cpp"
	
	try {      // for error handling
		r=colorComponent();
		g=colorComponent();
		b=colorComponent();
#line 1313 "Vrml97Parser.g"
		
		c[0] = r;
		c[1] = g;
		c[2] = b;
		
#line 3498 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

float  Vrml97Parser::colorComponent() {
#line 1324 "Vrml97Parser.g"
	float val = 0.0f;
#line 3510 "Vrml97Parser.cpp"
	
	try {      // for error handling
		val=floatValue();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
	return val;
}

float  Vrml97Parser::floatValue() {
#line 1361 "Vrml97Parser.g"
	float val = 0.0f;
#line 3526 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			f0 = LT(1);
			match(REAL);
#line 1362 "Vrml97Parser.g"
			val = atof(f0->getText().c_str());
#line 3538 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			f1 = LT(1);
			match(INTEGER);
#line 1363 "Vrml97Parser.g"
			val = atof(f1->getText().c_str());
#line 3547 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_18);
	}
	return val;
}

long  Vrml97Parser::intValue() {
#line 1435 "Vrml97Parser.g"
	long val = 0;
#line 3567 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		{
			i0 = LT(1);
			match(INTEGER);
#line 1436 "Vrml97Parser.g"
			val = atol(i0->getText().c_str());
#line 3579 "Vrml97Parser.cpp"
			break;
		}
		case HEX_INTEGER:
		{
			i1 = LT(1);
			match(HEX_INTEGER);
#line 1437 "Vrml97Parser.g"
			val = strtol(i1->getText().c_str(), 0, 16);
#line 3588 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_19);
	}
	return val;
}

void Vrml97Parser::rotationValue(
	float r[4]
) {
#line 1541 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
	
#line 3612 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
		rot=floatValue();
#line 1546 "Vrml97Parser.g"
		
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
		
#line 3636 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

double  Vrml97Parser::doubleValue() {
#line 1643 "Vrml97Parser.g"
	double val = 0.0;
#line 3648 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			d0 = LT(1);
			match(REAL);
#line 1644 "Vrml97Parser.g"
			val = atof(d0->getText().c_str());
#line 3660 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			d1 = LT(1);
			match(INTEGER);
#line 1645 "Vrml97Parser.g"
			val = atof(d1->getText().c_str());
#line 3669 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_18);
	}
	return val;
}

void Vrml97Parser::vec2fValue(
	float v[2]
) {
#line 1682 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f);
	
#line 3693 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
#line 1687 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		
#line 3703 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

void Vrml97Parser::vec3fValue(
	float v[3]
) {
#line 1728 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f);
	
#line 3719 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
#line 1733 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		v[2] = z;
		
#line 3731 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
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
const unsigned long Vrml97Parser::_tokenSet_12_data_[] = { 26706752UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID "eventIn" "eventOut" "exposedField" "EXTERNPROTO" 
// "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_12(_tokenSet_12_data_,4);
const unsigned long Vrml97Parser::_tokenSet_13_data_[] = { 159662848UL, 0UL, 0UL, 0UL };
// RBRACE ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long Vrml97Parser::_tokenSet_14_data_[] = { 160949058UL, 0UL, 0UL, 0UL };
// EOF RBRACKET RBRACE ID STRING "DEF" "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long Vrml97Parser::_tokenSet_15_data_[] = { 26575616UL, 0UL, 0UL, 0UL };
// RBRACE ID "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_15(_tokenSet_15_data_,4);
const unsigned long Vrml97Parser::_tokenSet_16_data_[] = { 138428928UL, 0UL, 0UL, 0UL };
// ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_16(_tokenSet_16_data_,4);
const unsigned long Vrml97Parser::_tokenSet_17_data_[] = { 134234656UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_17(_tokenSet_17_data_,4);
const unsigned long Vrml97Parser::_tokenSet_18_data_[] = { 26711872UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID INTEGER REAL "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_18(_tokenSet_18_data_,4);
const unsigned long Vrml97Parser::_tokenSet_19_data_[] = { 26709824UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID INTEGER HEX_INTEGER "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_19(_tokenSet_19_data_,4);


