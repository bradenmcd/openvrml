/* $ANTLR 2.7.2: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#line 114 "Vrml97Parser.g"

# include "Vrml97Parser.hpp"
ANTLR_RBRACE

#line 8 "Vrml97Parser.cpp"
#include "Vrml97Parser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 119 "Vrml97Parser.g"

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

Vrml97Scanner::Vrml97Scanner(std::istream & in):
    in_(in),
    line_(1),
    col_(0),
    c_(' '),
    prev_char_('\0'),
    prev_token_type_(0),
    read_too_much_(false),
    expecting_field_type_(false)
{}

antlr::RefToken Vrml97Scanner::nextToken()
{
    using std::string;
    using antlr::RefToken;
    using antlr::CommonToken;

    RefToken token(new CommonToken);
    string token_string;

    if (this->read_too_much_) {
        this->read_too_much_ = false;
    } else {
        this->getNextChar();
    }

    while (isWhitespaceChar(this->c_) || this->c_ == '#') {
        if (this->c_ == '#') {
            while (!(isNewlineChar(this->c_) || this->c_ == EOF)) {
                this->getNextChar();
            }
        } else {
            getNextChar();
        }
    }

    if (this->c_ == EOF) {
        token->setType(EOF_);
    } else if (isValidIdFirstChar(this->c_)) {
        //
        // in an identifier or a keyword
        //
        token->setType(ID);

        while (isValidIdRestChars(this->c_)) {
            token_string += this->c_;
            getNextChar();
        }

        read_too_much_ = true;

        token->setText(token_string);

        if (expecting_field_type_) {
            this->identifyFieldType(*token);
            expecting_field_type_ = false;
        }
        this->identifyKeyword(*token);

    } else if (this->c_ == '.'
            || this->c_ == '+'
            || this->c_ == '-'
            || isdigit(this->c_)) {
        //
        // probably in a number
        //

        if (this->c_ == '+' || this->c_ == '-') {
            token_string += this->c_;
            this->getNextChar();
            read_too_much_ = true;
        }

        if (isdigit(this->c_)) {
            //
            // definitely in a number
            //
            token->setType(INTEGER);

            token_string += this->c_;

            getNextChar();

            if (this->prev_char_ == '0' && (this->c_ == 'x' || this->c_ == 'X')) {
                //
                // in an integer expressed in hexadecimal
                //
                token->setType(HEX_INTEGER);

                token_string += this->c_;
                getNextChar();
                while (isHexDigit(this->c_)) {
                    token_string += this->c_;
                    getNextChar();
                }
            } else {
                while (isdigit(this->c_)) {
                    token_string += this->c_;
                    getNextChar();
                }
            }

            if (this->c_ == '.') {
                //
                // in a floating-point number
                //
                token->setType(REAL);

                token_string += this->c_;
                getNextChar();

                while (isdigit(this->c_)) {
                    token_string += this->c_;
                    getNextChar();
                }

                if (this->c_ == 'E' || this->c_ == 'e') {
                    //
                    // in an exponent
                    //
                    token_string += this->c_;
                    getNextChar();

                    if ((c_ == '+') || (c_ == '-') || isdigit(this->c_)) {
                        //
                        // exponent may be signed
                        //
                        token_string += this->c_;
                        getNextChar();

                        while (isdigit(this->c_)) {
                            token_string += this->c_;
                            getNextChar();
                        }
                    }
                }
            } else if (this->c_ == 'E' || this->c_ == 'e') {
                //
                // in an exponent
                //
                token->setType(REAL);

                token_string += this->c_;
                getNextChar();

                if (this->c_ == '+' || this->c_ == '-' || isdigit(this->c_)) {
                    //
                    // exponent may be signed
                    //
                    token_string += this->c_;
                    getNextChar();

                    while (isdigit(this->c_)) {
                        token_string += this->c_;
                        getNextChar();
                    }
                }
            }

            this->read_too_much_ = true;

        } else if (this->c_ == '.') {
            //
            // in a floating-point number or a lone full-stop (as in a ROUTE)
            //

            token_string += this->c_;
            getNextChar();

            if (isdigit(this->c_)) {

                token->setType(REAL);

                while (isdigit(this->c_)) {
                    token_string += this->c_;
                    getNextChar();
                }

                if ((this->c_ == 'E') || (this->c_ == 'e')) {
                    //
                    // in an exponent
                    //
                    token_string += this->c_;
                    getNextChar();

                    if ((c_ == '+') || (c_ == '-') || isdigit(this->c_)) {
                        //
                        // exponent may be signed
                        //
                        token_string += this->c_;
                        getNextChar();

                        while (isdigit(this->c_)) {
                            token_string += this->c_;
                            getNextChar();
                        }
                    }
                }
            } else {
                token->setType(PERIOD);
            }

            this->read_too_much_ = true;
        }

        token->setText(token_string);

    } else if (this->c_ == '"') {
        //
        // in a string
        //
        token->setType(STRING);

        token_string += this->c_;
        this->getNextChar();

        char prev_char('\0');
        while (this->c_ != '"' || prev_char == '\\') {
            token_string += this->c_;
            prev_char = this->c_;
            this->getNextChar();
        }
        token_string += this->c_; // add the closing quote

        token->setText(token_string);

    } else {
        //
        // terminal symbol or unidentified character
        //
        token_string += this->c_;

        token->setText(token_string);

        this->identifyTerminalSymbol(*token);
    }

    token->setLine(this->line_);
    token->setColumn(this->col_);
    this->prev_token_type_ = token->getType();

    return token;
}

inline void Vrml97Scanner::getNextChar()
{
    this->prev_char_ = this->c_;
    this->c_ = this->in_.get();
    ++this->col_; // Increment the column count;

    //
    // Increment the line count (and reset the column count to zero) if the
    // current character is a newline character EXCEPT if the current character
    // is a linefeed AND the previous character is a carriage return.
    //
    if (isNewlineChar(this->c_)) {
        if (!((this->c_ == 0x0a) && (this->prev_char_ == 0x0d))) {
            ++this->line_;
            this->col_ = 0;
        }
    }
}

inline void Vrml97Scanner::identifyKeyword(antlr::Token & token)
{
    std::string const token_text(token.getText());
    if      (token_text == "DEF")          { token.setType(KEYWORD_DEF); }
    else if (token_text == "eventIn")      { expecting_field_type_ = true;
                                             token.setType(KEYWORD_EVENTIN); }
    else if (token_text == "eventOut")     { expecting_field_type_ = true;
                                             token.setType(KEYWORD_EVENTOUT); }
    else if (token_text == "exposedField") { expecting_field_type_ = true;
                                             token.setType(KEYWORD_EXPOSEDFIELD); }
    else if (token_text == "EXTERNPROTO")  { token.setType(KEYWORD_EXTERNPROTO); }
    else if (token_text == "FALSE")        { token.setType(KEYWORD_FALSE); }
    else if (token_text == "field")        { expecting_field_type_ = true;
                                             token.setType(KEYWORD_FIELD); }
    else if (token_text == "IS")           { token.setType(KEYWORD_IS); }
    else if (token_text == "NULL")         { token.setType(KEYWORD_NULL); }
    else if (token_text == "PROTO")        { token.setType(KEYWORD_PROTO); }
    else if (token_text == "ROUTE")        { token.setType(KEYWORD_ROUTE); }
    else if (token_text == "TO")           { token.setType(KEYWORD_TO); }
    else if (token_text == "TRUE")         { token.setType(KEYWORD_TRUE); }
    else if (token_text == "USE")          { token.setType(KEYWORD_USE); }
}

inline void Vrml97Scanner::identifyFieldType(antlr::Token & token)
{
    assert(this->expecting_field_type_);
    std::string const token_text(token.getText());
    if      (token_text == "SFBool")     { token.setType(FIELDTYPE_SFBOOL); }
    else if (token_text == "SFColor")    { token.setType(FIELDTYPE_SFCOLOR); }
    else if (token_text == "SFFloat")    { token.setType(FIELDTYPE_SFFLOAT); }
    else if (token_text == "SFImage")    { token.setType(FIELDTYPE_SFIMAGE); }
    else if (token_text == "SFInt32")    { token.setType(FIELDTYPE_SFINT32); }
    else if (token_text == "SFNode")     { token.setType(FIELDTYPE_SFNODE); }
    else if (token_text == "SFRotation") { token.setType(FIELDTYPE_SFROTATION); }
    else if (token_text == "SFString")   { token.setType(FIELDTYPE_SFSTRING); }
    else if (token_text == "SFTime")     { token.setType(FIELDTYPE_SFTIME); }
    else if (token_text == "SFVec2f")    { token.setType(FIELDTYPE_SFVEC2F); }
    else if (token_text == "SFVec3f")    { token.setType(FIELDTYPE_SFVEC3F); }
    else if (token_text == "MFColor")    { token.setType(FIELDTYPE_MFCOLOR); }
    else if (token_text == "MFFloat")    { token.setType(FIELDTYPE_MFFLOAT); }
    else if (token_text == "MFInt32")    { token.setType(FIELDTYPE_MFINT32); }
    else if (token_text == "MFNode")     { token.setType(FIELDTYPE_MFNODE); }
    else if (token_text == "MFRotation") { token.setType(FIELDTYPE_MFROTATION); }
    else if (token_text == "MFString")   { token.setType(FIELDTYPE_MFSTRING); }
    else if (token_text == "MFTime")     { token.setType(FIELDTYPE_MFTIME); }
    else if (token_text == "MFVec2f")    { token.setType(FIELDTYPE_MFVEC2F); }
    else if (token_text == "MFVec3f")    { token.setType(FIELDTYPE_MFVEC3F); }
}

inline void Vrml97Scanner::identifyTerminalSymbol(antlr::Token & token)
{
    std::string const token_text(token.getText());
    if      (token_text == "[") { token.setType(LBRACKET); }
    else if (token_text == "]") { token.setType(RBRACKET); }
    else if (token_text == "{") { token.setType(LBRACE); }
    else if (token_text == "}") { token.setType(RBRACE); }
}

inline bool isValidIdFirstChar(const char c)
{
    if (((c >= 0x30) && (c <= 0x39))
            || (c == 0x2b)
            || (c == 0x2d)
            || !isValidIdRestChars(c)) {
        return false;
    }
    return true;
}

inline bool isValidIdRestChars(const char c)
{
    if (       c <= 0x20
            || c == 0x22
            || c == 0x23
            || c == 0x27
            || c == 0x2c
            || c == 0x2e
            || c == 0x5b
            || c == 0x5c
            || c == 0x5d
            || c == 0x7b
            || c == 0x7d
            || c == 0x7f) {
        return false;
    }
    return true;
}

inline bool isWhitespaceChar(const char c)
{
    if (       c == 0x0d      // carriage return
            || c == 0x0a      // linefeed
            || c == 0x20      // space
            || c == 0x09      // tab
            || c == 0x2c) {   // comma
        return true;
    }
    return false;
}

inline bool isNewlineChar(const char c)
{
    return ((c == 0x0a) || (c == 0x0d));
}

inline bool isHexDigit(const char c)
{
    if (          isdigit(c)
            || c == 'A' || c == 'a'
            || c == 'B' || c == 'b'
            || c == 'C' || c == 'c'
            || c == 'D' || c == 'd'
            || c == 'E' || c == 'e'
            || c == 'F' || c == 'f') {
        return true;
    }
    return false;
}

struct interface_id_equals_ :
        std::unary_function<OpenVRML::node_interface, bool> {
    explicit interface_id_equals_(const std::string & interface_id):
        interface_id(&interface_id)
    {}

    bool operator()(const OpenVRML::node_interface & interface) const
    {
        return interface.id == *this->interface_id;
    }

private:
    const std::string * interface_id;
};

} // namespace

namespace OpenVRML ANTLR_LBRACE


#line 484 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"
#line 486 "Vrml97Parser.cpp"
Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

Vrml97Parser::Vrml97Parser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

void Vrml97Parser::vrmlScene(
	OpenVRML::browser & browser,
          std::vector<node_ptr> & nodes
) {
#line 611 "Vrml97Parser.g"
	
	const scope_ptr scope(new Vrml97RootScope(browser, this->uri));
	
#line 520 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				statement(browser, nodes, scope);
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void Vrml97Parser::statement(
	OpenVRML::browser & browser,
          std::vector<node_ptr> & nodes,
          const OpenVRML::scope_ptr & scope
) {
#line 619 "Vrml97Parser.g"
	
	OpenVRML::node_ptr node;
	OpenVRML::node_type_ptr nodeType;
	
#line 553 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(browser, scope);
#line 626 "Vrml97Parser.g"
			
			//
			// If we are unable to parse a node, node will be null.
			//
			if (node) { nodes.push_back(node); }
			
#line 569 "Vrml97Parser.cpp"
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

OpenVRML::node_ptr  Vrml97Parser::nodeStatement(
	OpenVRML::browser & browser,
              const OpenVRML::scope_ptr & scope
) {
#line 636 "Vrml97Parser.g"
	OpenVRML::node_ptr n;
#line 602 "Vrml97Parser.cpp"
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
#line 642 "Vrml97Parser.g"
			
			using antlr::SemanticException;
			assert(scope);
			n.reset(scope->find_node(id1->getText()));
			if (!n) {
			throw SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.",
			this->uri,
			id1->getLine(),
			id1->getColumn());
			}
			
#line 633 "Vrml97Parser.cpp"
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
#line 656 "Vrml97Parser.g"
		
		reportError(ex);
		
#line 652 "Vrml97Parser.cpp"
	}
	return n;
}

void Vrml97Parser::protoStatement(
	OpenVRML::browser & browser,
               const OpenVRML::scope_ptr & scope
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
	const OpenVRML::scope & scope
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
#line 905 "Vrml97Parser.g"
		
		using OpenVRML::field_value;
		using OpenVRML::node;
		using OpenVRML::node_ptr;
		using antlr::SemanticException;
		
		node * const fromNode = scope.find_node(fromNodeId->getText());
		if (!fromNode) {
		throw SemanticException("Node \"" + fromNodeId->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		fromNodeId->getLine(),
		fromNodeId->getColumn());
		}
		
		node * const toNode = scope.find_node(toNodeId->getText());
		if (!toNode) {
		throw SemanticException("Node \"" + toNodeId->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		toNodeId->getLine(),
		toNodeId->getColumn());
		}
		
		try {
		fromNode->add_route(fromInterfaceId->getText(),
		node_ptr(toNode), toInterfaceId->getText());
		} catch (std::runtime_error & ex) {
		throw SemanticException(ex.what(),
		this->uri,
		toInterfaceId->getLine(),
		toInterfaceId->getColumn());
		}
		
#line 745 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

OpenVRML::node_ptr  Vrml97Parser::node(
	OpenVRML::browser & browser,
     const OpenVRML::scope_ptr & scope,
     const std::string & nodeId
) {
#line 943 "Vrml97Parser.g"
	OpenVRML::node_ptr n;
#line 761 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 943 "Vrml97Parser.g"
	
	using OpenVRML::node_type_ptr;
	using OpenVRML::node_ptr;
	using OpenVRML::script_node;
	using antlr::SemanticException;
	
	node_type_ptr nodeType;
	
#line 773 "Vrml97Parser.cpp"
	
	if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 956 "Vrml97Parser.g"
		
		n.reset(new script_node(browser.script_node_class_, scope));
		if (!nodeId.empty()) { n->id(nodeId); }
		
		script_node * const scriptNode = n->to_script();
		assert(scriptNode);
		
#line 786 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
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
		} // ( ... )*
		match(RBRACE);
	}
	else if ((LA(1) == ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 967 "Vrml97Parser.g"
		
		nodeType = scope->find_type(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		this->uri,
		nodeTypeId->getLine(),
		nodeTypeId->getColumn());
		}
		
		n = node_ptr(nodeType->create_node(scope));
		
		if (!nodeId.empty()) { n->id(nodeId); }
		
#line 834 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(scope, *n);
			}
			else {
				goto _loop33;
			}
			
		}
		_loop33:;
		} // ( ... )*
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externproto(
	OpenVRML::browser & browser, const OpenVRML::scope_ptr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 834 "Vrml97Parser.g"
	
	OpenVRML::node_interface_set interfaces;
	OpenVRML::mfstring urlList;
	OpenVRML::node_type_ptr nodeType;
	
#line 867 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_EXTERNPROTO);
		id = LT(1);
		match(ID);
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				externInterfaceDeclaration(interfaces);
			}
			else {
				goto _loop22;
			}
			
		}
		_loop22:;
		} // ( ... )*
		match(RBRACKET);
		urlList=externprotoUrlList();
#line 842 "Vrml97Parser.g"
		
		for (size_t i = 0; i < urlList.value.size(); ++i) {
			browser::node_class_map_t::const_iterator pos =
		browser.node_class_map.find(urlList.value[i]);
		if (pos != browser.node_class_map.end()) {
		nodeType = pos->second->create_type(id->getText(),
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
		if (nodeType) { scope->add_type(nodeType); }
		
#line 909 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	OpenVRML::browser & browser,
      const OpenVRML::scope_ptr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 670 "Vrml97Parser.g"
	
	node_class_ptr nodeClass;
	scope_ptr protoScope;
	
#line 928 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 676 "Vrml97Parser.g"
		
		nodeClass.reset(new ProtoNodeClass(browser));
		protoScope.reset(new OpenVRML::scope(id->getText(), scope));
		
#line 939 "Vrml97Parser.cpp"
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
#line 681 "Vrml97Parser.g"
				
				// XXX Hmm... We give each interface declaration its own scope
				// XXX here. This is wasteful; Vrml97RootScope is expensive and
				// XXX we only *really* need this for SFNode and MFNode
				// XXX fields/exposedFields.
				scope_ptr interfaceDeclScope(new Vrml97RootScope(browser,
				this->uri));
				
#line 953 "Vrml97Parser.cpp"
				protoInterfaceDeclaration(interfaceDeclScope,
                                      static_cast<ProtoNodeClass &>(*nodeClass));
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		} // ( ... )*
		match(RBRACKET);
		match(LBRACE);
		protoBody(protoScope,
                                     static_cast<ProtoNodeClass &>(*nodeClass));
		match(RBRACE);
#line 694 "Vrml97Parser.g"
		
		//
		// Add the new NodeClass (prototype definition) to the browser's
		// NodeClassMap.
		//
		// First, construct the id for the node implementation.
		//
		std::string implId = scope->id;
		while (protoScope) {
		implId += '#' + protoScope->id;
		protoScope = protoScope->parent;
		}
		const browser::node_class_map_t::value_type
		value(implId, nodeClass);
		browser.node_class_map.insert(value);
		
		//
		// PROTO's implicitly introduce a new node type as well...
		//
		const node_interface_set & interfaces =
		static_cast<ProtoNodeClass &>(*nodeClass)
		.protoNodeType.interfaces();
		const node_type_ptr nodeType =
		nodeClass->create_type(id->getText(), interfaces);
		assert(nodeType);
		assert(scope);
		scope->add_type(nodeType);
		
#line 998 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	const OpenVRML::scope_ptr & scope,
                          OpenVRML::ProtoNodeClass & proto
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 724 "Vrml97Parser.g"
	
	using OpenVRML::node_interface;
	using antlr::SemanticException;
	
	node_interface::type_id it(node_interface::invalid_type_id);
	OpenVRML::field_value::type_id ft(field_value::invalid_type_id);
	OpenVRML::field_value_ptr fv;
	
#line 1022 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 734 "Vrml97Parser.g"
			
			try {
			switch (it) {
			case node_interface::eventin_id:
			proto.addEventIn(ft, id0->getText());
			break;
			
			case node_interface::eventout_id:
			proto.addEventOut(ft, id0->getText());
			break;
			
			default:
			assert(false);
			}
			} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(),
			this->uri,
			id0->getLine(),
			id0->getColumn());
			}
			
#line 1055 "Vrml97Parser.cpp"
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
			case node_interface::field_id:
			proto.addField(id1->getText(), fv);
			break;
			
			case node_interface::exposedfield_id:
			proto.addExposedField(id1->getText(), fv);
			break;
			
			default:
			assert(false);
			}
			} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(),
			this->uri,
			id1->getLine(),
			id1->getColumn());
			}
			
#line 1089 "Vrml97Parser.cpp"
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
	const OpenVRML::scope_ptr & scope,
          OpenVRML::ProtoNodeClass & proto
) {
#line 792 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	
#line 1113 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == KEYWORD_EXTERNPROTO || LA(1) == KEYWORD_PROTO)) {
				protoStatement(proto.browser, scope);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		} // ( ... )*
		n=protoNodeStatement(proto, scope);
#line 798 "Vrml97Parser.g"
		assert(n); proto.addRootNode(n);
#line 1131 "Vrml97Parser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(proto, scope);
			}
			else {
				goto _loop17;
			}
			
		}
		_loop17:;
		} // ( ... )*
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_7);
	}
}

OpenVRML::node_interface::type_id  Vrml97Parser::eventInterfaceType() {
#line 780 "Vrml97Parser.g"
	OpenVRML::node_interface::type_id it = OpenVRML::node_interface::invalid_type_id;
#line 1155 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 781 "Vrml97Parser.g"
			it = OpenVRML::node_interface::eventin_id;
#line 1164 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 782 "Vrml97Parser.g"
			it = OpenVRML::node_interface::eventout_id;
#line 1172 "Vrml97Parser.cpp"
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

OpenVRML::field_value::type_id  Vrml97Parser::fieldType() {
#line 1295 "Vrml97Parser.g"
	OpenVRML::field_value::type_id ft = OpenVRML::field_value::invalid_type_id;
#line 1192 "Vrml97Parser.cpp"
#line 1295 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	
#line 1197 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 1300 "Vrml97Parser.g"
			ft = field_value::mfcolor_id;
#line 1206 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 1301 "Vrml97Parser.g"
			ft = field_value::mffloat_id;
#line 1214 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 1302 "Vrml97Parser.g"
			ft = field_value::mfint32_id;
#line 1222 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 1303 "Vrml97Parser.g"
			ft = field_value::mfnode_id;
#line 1230 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 1304 "Vrml97Parser.g"
			ft = field_value::mfrotation_id;
#line 1238 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 1305 "Vrml97Parser.g"
			ft = field_value::mfstring_id;
#line 1246 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 1306 "Vrml97Parser.g"
			ft = field_value::mftime_id;
#line 1254 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 1307 "Vrml97Parser.g"
			ft = field_value::mfvec2f_id;
#line 1262 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 1308 "Vrml97Parser.g"
			ft = field_value::mfvec3f_id;
#line 1270 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 1309 "Vrml97Parser.g"
			ft = field_value::sfbool_id;
#line 1278 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 1310 "Vrml97Parser.g"
			ft = field_value::sfcolor_id;
#line 1286 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 1311 "Vrml97Parser.g"
			ft = field_value::sffloat_id;
#line 1294 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 1312 "Vrml97Parser.g"
			ft = field_value::sfimage_id;
#line 1302 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 1313 "Vrml97Parser.g"
			ft = field_value::sfint32_id;
#line 1310 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 1314 "Vrml97Parser.g"
			ft = field_value::sfnode_id;
#line 1318 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 1315 "Vrml97Parser.g"
			ft = field_value::sfrotation_id;
#line 1326 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 1316 "Vrml97Parser.g"
			ft = field_value::sfstring_id;
#line 1334 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 1317 "Vrml97Parser.g"
			ft = field_value::sftime_id;
#line 1342 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 1318 "Vrml97Parser.g"
			ft = field_value::sfvec2f_id;
#line 1350 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 1319 "Vrml97Parser.g"
			ft = field_value::sfvec3f_id;
#line 1358 "Vrml97Parser.cpp"
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

OpenVRML::node_interface::type_id  Vrml97Parser::fieldInterfaceType() {
#line 785 "Vrml97Parser.g"
	OpenVRML::node_interface::type_id it =
            OpenVRML::node_interface::invalid_type_id;
#line 1379 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 788 "Vrml97Parser.g"
			it = OpenVRML::node_interface::field_id;
#line 1388 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 789 "Vrml97Parser.g"
			it = OpenVRML::node_interface::exposedfield_id;
#line 1396 "Vrml97Parser.cpp"
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

OpenVRML::field_value_ptr  Vrml97Parser::fieldValue(
	OpenVRML::browser & browser,
           const OpenVRML::scope_ptr & scope,
           OpenVRML::field_value::type_id ft
) {
#line 1322 "Vrml97Parser.g"
	OpenVRML::field_value_ptr fv;
#line 1420 "Vrml97Parser.cpp"
#line 1322 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	
#line 1425 "Vrml97Parser.cpp"
	
	if (((_tokenSet_10.member(LA(1))))&&( (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) )) {
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

OpenVRML::node_ptr  Vrml97Parser::protoNodeStatement(
	OpenVRML::ProtoNodeClass & proto,
                   const OpenVRML::scope_ptr & scope
) {
#line 812 "Vrml97Parser.g"
	OpenVRML::node_ptr n;
#line 1446 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 812 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 1453 "Vrml97Parser.cpp"
	
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
#line 820 "Vrml97Parser.g"
		
		n.reset(scope->find_node(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in this "
		+ "scope.",
		this->uri,
		id1->getLine(),
		id1->getColumn());
		}
		
#line 1481 "Vrml97Parser.cpp"
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
                   const OpenVRML::scope_ptr & scope
) {
#line 802 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	
#line 1505 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 807 "Vrml97Parser.g"
			assert(n); proto.addRootNode(n);
#line 1516 "Vrml97Parser.cpp"
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

OpenVRML::node_ptr  Vrml97Parser::protoNode(
	OpenVRML::ProtoNodeClass & proto,
          const OpenVRML::scope_ptr & scope,
          const std::string & nodeId
) {
#line 1076 "Vrml97Parser.g"
	OpenVRML::node_ptr n;
#line 1550 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1076 "Vrml97Parser.g"
	
	using OpenVRML::node_type_ptr;
	using OpenVRML::node_ptr;
	using OpenVRML::script_node;
	using antlr::SemanticException;
	
	node_type_ptr nodeType;
	
#line 1562 "Vrml97Parser.cpp"
	
	if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1089 "Vrml97Parser.g"
		
		n.reset(new script_node(proto.browser.script_node_class_, scope));
		if (!nodeId.empty()) { n->id(nodeId); }
		
		script_node * const scriptNode = n->to_script();
		assert(scriptNode);
		
#line 1575 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
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
		} // ( ... )*
		match(RBRACE);
	}
	else if ((LA(1) == ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 1101 "Vrml97Parser.g"
		
		nodeType = scope->find_type(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		this->uri,
		nodeTypeId->getLine(),
		nodeTypeId->getColumn());
		}
		n = nodeType->create_node(scope);
		if (!nodeId.empty()) { n->id(nodeId); }
		
#line 1621 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(proto, scope, *n);
			}
			else {
				goto _loop41;
			}
			
		}
		_loop41:;
		} // ( ... )*
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externInterfaceDeclaration(
	OpenVRML::node_interface_set & interfaces
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 864 "Vrml97Parser.g"
	
	using OpenVRML::node_interface;
	using OpenVRML::field_value;
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 1656 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 872 "Vrml97Parser.g"
		
		const node_interface interface(it, ft, id->getText());
		try {
		interfaces.add(interface);
		} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(),
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 1675 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

OpenVRML::mfstring  Vrml97Parser::externprotoUrlList() {
#line 891 "Vrml97Parser.g"
	OpenVRML::mfstring urlList;
#line 1687 "Vrml97Parser.cpp"
#line 891 "Vrml97Parser.g"
	
	using std::string;
	using OpenVRML::mfstring;
	
	string s;
	
#line 1695 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 898 "Vrml97Parser.g"
			urlList.value.push_back(s);
#line 1704 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == STRING)) {
					s=stringValue();
#line 899 "Vrml97Parser.g"
					urlList.value.push_back(s);
#line 1716 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			} // ( ... )*
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

OpenVRML::node_interface::type_id  Vrml97Parser::interfaceType() {
#line 885 "Vrml97Parser.g"
	OpenVRML::node_interface::type_id it = OpenVRML::node_interface::invalid_type_id;
#line 1745 "Vrml97Parser.cpp"
	
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
#line 1660 "Vrml97Parser.g"
	std::string str;
#line 1778 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	s = LT(1);
	match(STRING);
#line 1663 "Vrml97Parser.g"
	
	//
	// Why doesn't this work?
	//
	// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
	
	std::string temp(s->getText());
	str = std::string(temp.begin() + 1, temp.end() - 1);
	
#line 1793 "Vrml97Parser.cpp"
	return str;
}

void Vrml97Parser::nodeBodyElement(
	const OpenVRML::scope_ptr & scope,
                OpenVRML::node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 983 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	using antlr::SemanticException;
	field_value::type_id ft(field_value::invalid_type_id);
	field_value_ptr fv;
	
#line 1809 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 991 "Vrml97Parser.g"
			
			ft = node.type.has_field(id->getText());
			if (ft == field_value::invalid_type_id) {
			ft = node.type.has_exposedfield(id->getText());
			if (ft == field_value::invalid_type_id) {
			throw SemanticException(node.type.id + " node has no "
			"field or exposedField \""
			+ id->getText() + "\"",
			this->uri,
			id->getLine(),
			id->getColumn());
			}
			}
			
#line 1832 "Vrml97Parser.cpp"
			fv=fieldValue(node.type._class.browser, scope, ft);
#line 1005 "Vrml97Parser.g"
			
			assert(fv);
			node.field(id->getText(), *fv);
			
#line 1839 "Vrml97Parser.cpp"
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
			protoStatement(node.type._class.browser, scope);
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
	const OpenVRML::scope_ptr & scope,
                           OpenVRML::script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1013 "Vrml97Parser.g"
	
	using OpenVRML::node_interface;
	using OpenVRML::field_value;
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 1879 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1022 "Vrml97Parser.g"
			
			const node_interface_set::const_iterator pos =
			node.node::type.interfaces().find(id->getText());
			if (pos != node.node::type.interfaces().end()) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script "
			"node.",
			this->uri,
			id->getLine(),
			id->getColumn());
			}
			switch (it) {
			case node_interface::eventin_id:
			node.add_eventin(ft, id->getText());
			break;
			case node_interface::eventout_id:
			node.add_eventout(ft, id->getText());
			break;
			default:
			assert(false);
			}
			
#line 1913 "Vrml97Parser.cpp"
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
	const OpenVRML::scope_ptr & scope,
                                OpenVRML::script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1047 "Vrml97Parser.g"
	
	using std::find_if;
	using OpenVRML::field_value;
	using antlr::SemanticException;
	
	field_value::type_id ft = field_value::invalid_type_id;
	field_value_ptr fv;
	
#line 1948 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(node.node::type._class.browser, scope, ft);
#line 1058 "Vrml97Parser.g"
		
		assert(fv);
		const node_interface_set & interfaces =
		node.node::type.interfaces();
		const node_interface_set::const_iterator pos =
		interfaces.find(id->getText());
		if (pos != interfaces.end()) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script "
		"node.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		node.add_field(id->getText(), fv);
		
#line 1973 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	OpenVRML::ProtoNodeClass & proto,
                     const OpenVRML::scope_ptr & scope,
                     OpenVRML::node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1116 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	using antlr::SemanticException;
	
	field_value::type_id ft(field_value::invalid_type_id);
	field_value_ptr fv;
	
#line 1997 "Vrml97Parser.cpp"
	
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
			if (((LA(1) == ID))&&( node.type.has_field(LT(1)->getText())
        || node.type.has_exposedfield(LT(1)->getText()) )) {
				id = LT(1);
				match(ID);
#line 1127 "Vrml97Parser.g"
				
				ft = node.type.has_field(id->getText());
				if (ft == field_value::invalid_type_id) {
				ft = node.type.has_exposedfield(id->getText());
				if (ft == field_value::invalid_type_id) {
				throw SemanticException(node.type.id + " node has no "
				"field or exposedField \""
				+ id->getText() + "\".",
				this->uri,
				id->getLine(),
				id->getColumn());
				}
				}
				
#line 2032 "Vrml97Parser.cpp"
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
#line 1142 "Vrml97Parser.g"
					
					assert(fv);
					node.field(id->getText(), *fv);
					
#line 2054 "Vrml97Parser.cpp"
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
			else if ((LA(1) == ID)) {
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
                                const OpenVRML::scope_ptr & scope,
                                OpenVRML::script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1177 "Vrml97Parser.g"
	
	using OpenVRML::node_interface;
	using OpenVRML::field_value;
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 2101 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1187 "Vrml97Parser.g"
			
			const node_interface_set::const_iterator pos =
			node.node::type.interfaces().find(id->getText());
			if (pos != node.node::type.interfaces().end()) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script "
			"node.",
			this->uri,
			id->getLine(),
			id->getColumn());
			}
			switch (it) {
			case node_interface::eventin_id:
			node.add_eventin(ft, id->getText());
			break;
			case node_interface::eventout_id:
			node.add_eventout(ft, id->getText());
			break;
			default:
			assert(false);
			}
			
#line 2135 "Vrml97Parser.cpp"
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

OpenVRML::field_value_ptr  Vrml97Parser::protoFieldValue(
	OpenVRML::ProtoNodeClass & proto,
                const OpenVRML::scope_ptr & scope,
                OpenVRML::field_value::type_id ft
) {
#line 1335 "Vrml97Parser.g"
	OpenVRML::field_value_ptr fv;
#line 2187 "Vrml97Parser.cpp"
#line 1335 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	
#line 2192 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&( (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) )) {
			fv=protoNodeFieldValue(proto, scope, ft);
#line 1343 "Vrml97Parser.g"
			assert(fv);
#line 2199 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 1344 "Vrml97Parser.g"
			assert(fv);
#line 2205 "Vrml97Parser.cpp"
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
	OpenVRML::ProtoNodeClass & proto, OpenVRML::node & node,
            std::string const & nodeInterfaceId
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1154 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 2229 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 1159 "Vrml97Parser.g"
		
		try {
		proto.addIS(node, nodeInterfaceId, id->getText());
		} catch (std::bad_alloc & ex) {
		throw;
		} catch (std::runtime_error & ex) {
		//
		// ex should be UnsupportedInterface, node_interfaceTypeMismatch,
		// or FieldValueTypeMismatch.
		//
		throw SemanticException(ex.what(),
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 2252 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	OpenVRML::ProtoNodeClass & proto,
                                     const OpenVRML::scope_ptr & scope,
                                     OpenVRML::script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  protoFieldId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1212 "Vrml97Parser.g"
	
	using std::find_if;
	using OpenVRML::field_value;
	using antlr::SemanticException;
	
	field_value::type_id ft(field_value::invalid_type_id);
	field_value_ptr fv;
	
#line 2277 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1223 "Vrml97Parser.g"
		
		//
		// We need to check if the fieldId is an exact match for any
		// existing interface for the Script node; so, we don't use
		// node_interface_set::findInterface.
		//
		const node_interface_set & interfaces =
		node.node::type.interfaces();
		const node_interface_set::const_iterator pos =
		find_if(interfaces.begin(), interfaces.end(),
		interface_id_equals_(id->getText()));
		if (pos != interfaces.end()) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script "
		"node.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 2305 "Vrml97Parser.cpp"
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
#line 1244 "Vrml97Parser.g"
			
			assert(fv);
			node.add_field(id->getText(), fv);
			
#line 2327 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
			{
			match(KEYWORD_IS);
			protoFieldId = LT(1);
			match(ID);
#line 1250 "Vrml97Parser.g"
			
			//
			// First, get the field value from the ProtoNodeClass'
			// fieldValueMap.
			//
			ProtoNodeClass::DefaultValueMap::const_iterator pos =
			proto.defaultValueMap.find(protoFieldId->getText());
			if (pos == proto.defaultValueMap.end()) {
			throw SemanticException("Proto has no field \""
			+ protoFieldId->getText()
			+ "\".",
			this->uri,
			protoFieldId->getLine(),
			protoFieldId->getColumn());
			}
			
			//
			// Now, add the field to the Script node with the default
			// value.
			//
			node.add_field(id->getText(), pos->second);
			
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
			throw SemanticException(ex.what(),
			this->uri,
			protoFieldId->getLine(),
			protoFieldId->getColumn());
			}
			
#line 2379 "Vrml97Parser.cpp"
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

OpenVRML::field_value_ptr  Vrml97Parser::nodeFieldValue(
	OpenVRML::browser & browser,
               const OpenVRML::scope_ptr & scope,
               OpenVRML::field_value::type_id ft
) {
#line 1373 "Vrml97Parser.g"
	OpenVRML::field_value_ptr fv;
#line 2404 "Vrml97Parser.cpp"
#line 1373 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	
#line 2409 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == field_value::sfnode_id )) {
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

OpenVRML::field_value_ptr  Vrml97Parser::nonNodeFieldValue(
	OpenVRML::field_value::type_id ft
) {
#line 1347 "Vrml97Parser.g"
	OpenVRML::field_value_ptr fv = OpenVRML::field_value_ptr(0);
#line 2429 "Vrml97Parser.cpp"
#line 1347 "Vrml97Parser.g"
	
	using OpenVRML::field_value;
	
#line 2434 "Vrml97Parser.cpp"
	
	if (((LA(1) == KEYWORD_FALSE || LA(1) == KEYWORD_TRUE))&&( ft == field_value::sfbool_id )) {
		fv=sfBoolValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfcolor_id )) {
		fv=sfColorValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sffloat_id )) {
		fv=sfFloatValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::sfimage_id )) {
		fv=sfImageValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::sfint32_id )) {
		fv=sfInt32Value();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfrotation_id )) {
		fv=sfRotationValue();
	}
	else if (((LA(1) == STRING))&&( ft == field_value::sfstring_id )) {
		fv=sfStringValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sftime_id )) {
		fv=sfTimeValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec2f_id )) {
		fv=sfVec2fValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec3f_id )) {
		fv=sfVec3fValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfcolor_id )) {
		fv=mfColorValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mffloat_id )) {
		fv=mfFloatValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::mfint32_id )) {
		fv=mfInt32Value();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfrotation_id )) {
		fv=mfRotationValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == STRING))&&( ft == field_value::mfstring_id )) {
		fv=mfStringValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mftime_id )) {
		fv=mfTimeValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfvec2f_id )) {
		fv=mfVec2fValue();
	}
	else if ((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL)) {
		fv=mfVec3fValue();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

OpenVRML::field_value_ptr  Vrml97Parser::protoNodeFieldValue(
	OpenVRML::ProtoNodeClass & proto,
                    const OpenVRML::scope_ptr & scope,
                    OpenVRML::field_value::type_id ft
) {
#line 1385 "Vrml97Parser.g"
	OpenVRML::field_value_ptr fv;
#line 2504 "Vrml97Parser.cpp"
	
	if (((_tokenSet_14.member(LA(1))))&&( ft == OpenVRML::field_value::sfnode_id )) {
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

OpenVRML::field_value_ptr  Vrml97Parser::sfBoolValue() {
#line 1395 "Vrml97Parser.g"
	OpenVRML::field_value_ptr sbv;
#line 2522 "Vrml97Parser.cpp"
#line 1395 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2527 "Vrml97Parser.cpp"
	
	val=boolValue();
#line 1400 "Vrml97Parser.g"
	sbv.reset(new sfbool(val));
#line 2532 "Vrml97Parser.cpp"
	return sbv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfColorValue() {
#line 1409 "Vrml97Parser.g"
	OpenVRML::field_value_ptr scv;
#line 2539 "Vrml97Parser.cpp"
#line 1409 "Vrml97Parser.g"
	
	color c;
	
#line 2544 "Vrml97Parser.cpp"
	
	colorValue(c);
#line 1414 "Vrml97Parser.g"
	scv.reset(new sfcolor(c));
#line 2549 "Vrml97Parser.cpp"
	return scv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfFloatValue() {
#line 1457 "Vrml97Parser.g"
	OpenVRML::field_value_ptr sfv;
#line 2556 "Vrml97Parser.cpp"
#line 1457 "Vrml97Parser.g"
	
	float f;
	
#line 2561 "Vrml97Parser.cpp"
	
	f=floatValue();
#line 1462 "Vrml97Parser.g"
	sfv.reset(new sffloat(f));
#line 2566 "Vrml97Parser.cpp"
	return sfv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfImageValue() {
#line 1483 "Vrml97Parser.g"
	OpenVRML::field_value_ptr siv;
#line 2573 "Vrml97Parser.cpp"
#line 1483 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 2578 "Vrml97Parser.cpp"
	
	w=intValue();
	h=intValue();
	com=intValue();
#line 1489 "Vrml97Parser.g"
	std::vector<unsigned char> pixelVector;
#line 2585 "Vrml97Parser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
			pixel=intValue();
#line 1492 "Vrml97Parser.g"
			
			// need to confirm the cross-platform-ness of this, it
			// looks kind of ugly but might in fact be ok. basically,
			// we read the value as an integer, then strip off the
			// bytes one by one.
					for (int i = com - 1; i >= 0; i--) {
			pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
			}
			
#line 2600 "Vrml97Parser.cpp"
		}
		else {
			goto _loop73;
		}
		
	}
	_loop73:;
	} // ( ... )*
#line 1502 "Vrml97Parser.g"
	
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
	this->uri,
	LT(1)->getLine(),
	LT(1)->getColumn());
	}
	siv.reset(new sfimage(w, h, com, &pixelVector[0]));
	
#line 2627 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfInt32Value() {
#line 1521 "Vrml97Parser.g"
	OpenVRML::field_value_ptr siv;
#line 2634 "Vrml97Parser.cpp"
#line 1521 "Vrml97Parser.g"
	
	long i;
	
#line 2639 "Vrml97Parser.cpp"
	
	i=intValue();
#line 1526 "Vrml97Parser.g"
	siv.reset(new sfint32(i));
#line 2644 "Vrml97Parser.cpp"
	return siv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfRotationValue() {
#line 1599 "Vrml97Parser.g"
	OpenVRML::field_value_ptr srv;
#line 2651 "Vrml97Parser.cpp"
#line 1599 "Vrml97Parser.g"
	rotation r;
#line 2654 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1601 "Vrml97Parser.g"
		srv.reset(new sfrotation(r));
#line 2660 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_16);
	}
	return srv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfStringValue() {
#line 1642 "Vrml97Parser.g"
	OpenVRML::field_value_ptr ssv;
#line 2673 "Vrml97Parser.cpp"
#line 1642 "Vrml97Parser.g"
	std::string s;
#line 2676 "Vrml97Parser.cpp"
	
	s=stringValue();
#line 1645 "Vrml97Parser.g"
	ssv.reset(new sfstring(s));
#line 2681 "Vrml97Parser.cpp"
	return ssv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfTimeValue() {
#line 1674 "Vrml97Parser.g"
	OpenVRML::field_value_ptr stv;
#line 2688 "Vrml97Parser.cpp"
#line 1674 "Vrml97Parser.g"
	double t(0.0);
#line 2691 "Vrml97Parser.cpp"
	
	t=doubleValue();
#line 1677 "Vrml97Parser.g"
	stv.reset(new sftime(t));
#line 2696 "Vrml97Parser.cpp"
	return stv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfVec2fValue() {
#line 1697 "Vrml97Parser.g"
	OpenVRML::field_value_ptr svv;
#line 2703 "Vrml97Parser.cpp"
#line 1697 "Vrml97Parser.g"
	vec2f v;
#line 2706 "Vrml97Parser.cpp"
	
	vec2fValue(v);
#line 1700 "Vrml97Parser.g"
	svv.reset(new sfvec2f(v));
#line 2711 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::field_value_ptr  Vrml97Parser::sfVec3fValue() {
#line 1724 "Vrml97Parser.g"
	OpenVRML::field_value_ptr svv;
#line 2718 "Vrml97Parser.cpp"
#line 1724 "Vrml97Parser.g"
	
	vec3f v;
	
#line 2723 "Vrml97Parser.cpp"
	
	vec3fValue(v);
#line 1729 "Vrml97Parser.g"
	svv.reset(new sfvec3f(v));
#line 2728 "Vrml97Parser.cpp"
	return svv;
}

OpenVRML::field_value_ptr  Vrml97Parser::mfColorValue() {
#line 1417 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mcv =
            OpenVRML::field_value_ptr(new mfcolor);
#line 2736 "Vrml97Parser.cpp"
#line 1417 "Vrml97Parser.g"
	
	color c;
	mfcolor & colors = static_cast<mfcolor &>(*mcv);
	
#line 2742 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		colorValue(c);
#line 1425 "Vrml97Parser.g"
		colors.value.push_back(c);
#line 2751 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				colorValue(c);
#line 1426 "Vrml97Parser.g"
				colors.value.push_back(c);
#line 2763 "Vrml97Parser.cpp"
			}
			else {
				goto _loop63;
			}
			
		}
		_loop63:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfFloatValue() {
#line 1465 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mfv =
            OpenVRML::field_value_ptr(new mffloat);
#line 2787 "Vrml97Parser.cpp"
#line 1465 "Vrml97Parser.g"
	
	float f;
	mffloat & floats = static_cast<mffloat &>(*mfv);
	
#line 2793 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		f=floatValue();
#line 1473 "Vrml97Parser.g"
		floats.value.push_back(f);
#line 2802 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				f=floatValue();
#line 1474 "Vrml97Parser.g"
				floats.value.push_back(f);
#line 2814 "Vrml97Parser.cpp"
			}
			else {
				goto _loop69;
			}
			
		}
		_loop69:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfInt32Value() {
#line 1529 "Vrml97Parser.g"
	OpenVRML::field_value_ptr miv =
            OpenVRML::field_value_ptr(new mfint32);
#line 2838 "Vrml97Parser.cpp"
#line 1529 "Vrml97Parser.g"
	
	long i;
	mfint32 & int32s = static_cast<mfint32 &>(*miv);
	
#line 2844 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case HEX_INTEGER:
	{
		i=intValue();
#line 1537 "Vrml97Parser.g"
		int32s.value.push_back(i);
#line 2853 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
				i=intValue();
#line 1538 "Vrml97Parser.g"
				int32s.value.push_back(i);
#line 2865 "Vrml97Parser.cpp"
			}
			else {
				goto _loop77;
			}
			
		}
		_loop77:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfRotationValue() {
#line 1604 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mrv =
            OpenVRML::field_value_ptr(new mfrotation);
#line 2889 "Vrml97Parser.cpp"
#line 1604 "Vrml97Parser.g"
	
	rotation r;
	mfrotation & rotations = static_cast<mfrotation &>(*mrv);
	
#line 2895 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1611 "Vrml97Parser.g"
			rotations.value.push_back(r);
#line 2905 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == INTEGER || LA(1) == REAL)) {
					rotationValue(r);
#line 1612 "Vrml97Parser.g"
					rotations.value.push_back(r);
#line 2917 "Vrml97Parser.cpp"
				}
				else {
					goto _loop90;
				}
				
			}
			_loop90:;
			} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfStringValue() {
#line 1648 "Vrml97Parser.g"
	OpenVRML::field_value_ptr msv =
            OpenVRML::field_value_ptr(new mfstring);
#line 2947 "Vrml97Parser.cpp"
#line 1648 "Vrml97Parser.g"
	
	std::string s;
	mfstring & strings = static_cast<mfstring &>(*msv);
	
#line 2953 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 1656 "Vrml97Parser.g"
		strings.value.push_back(s);
#line 2961 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == STRING)) {
				s=stringValue();
#line 1657 "Vrml97Parser.g"
				strings.value.push_back(s);
#line 2973 "Vrml97Parser.cpp"
			}
			else {
				goto _loop95;
			}
			
		}
		_loop95:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfTimeValue() {
#line 1680 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mtv = OpenVRML::field_value_ptr(new mftime);
#line 2996 "Vrml97Parser.cpp"
#line 1680 "Vrml97Parser.g"
	
	double t;
	mftime & times = static_cast<mftime &>(*mtv);
	
#line 3002 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		t=doubleValue();
#line 1687 "Vrml97Parser.g"
		times.value.push_back(t);
#line 3011 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				t=doubleValue();
#line 1688 "Vrml97Parser.g"
				times.value.push_back(t);
#line 3023 "Vrml97Parser.cpp"
			}
			else {
				goto _loop100;
			}
			
		}
		_loop100:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfVec2fValue() {
#line 1703 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mvv =
            OpenVRML::field_value_ptr(new mfvec2f);
#line 3047 "Vrml97Parser.cpp"
#line 1703 "Vrml97Parser.g"
	
	vec2f v;
	mfvec2f & vec2fs = static_cast<mfvec2f &>(*mvv);
	
#line 3053 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2fValue(v);
#line 1711 "Vrml97Parser.g"
		vec2fs.value.push_back(v);
#line 3062 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec2fValue(v);
#line 1712 "Vrml97Parser.g"
				vec2fs.value.push_back(v);
#line 3074 "Vrml97Parser.cpp"
			}
			else {
				goto _loop105;
			}
			
		}
		_loop105:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::mfVec3fValue() {
#line 1732 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mvv =
            OpenVRML::field_value_ptr(new mfvec3f);
#line 3098 "Vrml97Parser.cpp"
#line 1732 "Vrml97Parser.g"
	
	vec3f v;
	mfvec3f & vec3fs = static_cast<mfvec3f &>(*mvv);
	
#line 3104 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3fValue(v);
#line 1740 "Vrml97Parser.g"
		vec3fs.value.push_back(v);
#line 3113 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec3fValue(v);
#line 1741 "Vrml97Parser.g"
				vec3fs.value.push_back(v);
#line 3125 "Vrml97Parser.cpp"
			}
			else {
				goto _loop110;
			}
			
		}
		_loop110:;
		} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::sfNodeValue(
	OpenVRML::browser & browser,
            const OpenVRML::scope_ptr & scope
) {
#line 1547 "Vrml97Parser.g"
	OpenVRML::field_value_ptr snv;
#line 3151 "Vrml97Parser.cpp"
#line 1547 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	
#line 3156 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1553 "Vrml97Parser.g"
			snv.reset(new sfnode(n));
#line 3167 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1554 "Vrml97Parser.g"
			snv.reset(new sfnode);
#line 3175 "Vrml97Parser.cpp"
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

OpenVRML::field_value_ptr  Vrml97Parser::mfNodeValue(
	OpenVRML::browser & browser,
            const OpenVRML::scope_ptr & scope
) {
#line 1567 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mnv = OpenVRML::field_value_ptr(new mfnode);
#line 3198 "Vrml97Parser.cpp"
#line 1567 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3204 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1574 "Vrml97Parser.g"
			if (n) { nodes.value.push_back(n); }
#line 3215 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
					n=nodeStatement(browser, scope);
#line 1576 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3229 "Vrml97Parser.cpp"
				}
				else {
					goto _loop83;
				}
				
			}
			_loop83:;
			} // ( ... )*
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

OpenVRML::field_value_ptr  Vrml97Parser::protoSfNodeValue(
	OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::scope_ptr & scope
) {
#line 1557 "Vrml97Parser.g"
	OpenVRML::field_value_ptr snv;
#line 3261 "Vrml97Parser.cpp"
#line 1557 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	
#line 3266 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 1563 "Vrml97Parser.g"
			snv.reset(new sfnode(n));
#line 3277 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1564 "Vrml97Parser.g"
			snv.reset(new sfnode);
#line 3285 "Vrml97Parser.cpp"
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

OpenVRML::field_value_ptr  Vrml97Parser::protoMfNodeValue(
	OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::scope_ptr & scope
) {
#line 1582 "Vrml97Parser.g"
	OpenVRML::field_value_ptr mnv = OpenVRML::field_value_ptr(new mfnode);
#line 3308 "Vrml97Parser.cpp"
#line 1582 "Vrml97Parser.g"
	
	OpenVRML::node_ptr n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3314 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(proto, scope);
#line 1589 "Vrml97Parser.g"
			
			if (n) { nodes.value.push_back(n); }
			
#line 3327 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
					n=protoNodeStatement(proto, scope);
#line 1593 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3341 "Vrml97Parser.cpp"
				}
				else {
					goto _loop86;
				}
				
			}
			_loop86:;
			} // ( ... )*
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
#line 1403 "Vrml97Parser.g"
	bool val = false;
#line 3370 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_TRUE:
	{
		match(KEYWORD_TRUE);
#line 1405 "Vrml97Parser.g"
		val = true;
#line 3378 "Vrml97Parser.cpp"
		break;
	}
	case KEYWORD_FALSE:
	{
		match(KEYWORD_FALSE);
#line 1406 "Vrml97Parser.g"
		val = false;
#line 3386 "Vrml97Parser.cpp"
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
#line 1429 "Vrml97Parser.g"
	
	float r, g, b;
	
#line 3404 "Vrml97Parser.cpp"
	
	r=colorComponent();
	g=colorComponent();
	b=colorComponent();
#line 1434 "Vrml97Parser.g"
	c.r(r);
	c.g(g);
	c.b(b);
#line 3413 "Vrml97Parser.cpp"
}

float  Vrml97Parser::colorComponent() {
#line 1442 "Vrml97Parser.g"
	float val = 0.0f;
#line 3419 "Vrml97Parser.cpp"
	
	val=floatValue();
#line 1444 "Vrml97Parser.g"
	
	if (val < 0.0 || val > 1.0) {
	this->reportWarning("Color component values must be from 0 to "
	"1.");
	if (val < 0.0) {
	val = 0.0;
	} else if (val > 1.0) {
	val = 1.0;
	}
	}
	
#line 3434 "Vrml97Parser.cpp"
	return val;
}

float  Vrml97Parser::floatValue() {
#line 1477 "Vrml97Parser.g"
	float val;
#line 3441 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		f0 = LT(1);
		match(REAL);
#line 1479 "Vrml97Parser.g"
		std::istringstream(f0->getText()) >> val;
#line 3452 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		f1 = LT(1);
		match(INTEGER);
#line 1480 "Vrml97Parser.g"
		std::istringstream(f1->getText()) >> val;
#line 3461 "Vrml97Parser.cpp"
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
#line 1541 "Vrml97Parser.g"
	long val;
#line 3475 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case INTEGER:
	{
		i0 = LT(1);
		match(INTEGER);
#line 1543 "Vrml97Parser.g"
		std::istringstream(i0->getText()) >> val;
#line 3486 "Vrml97Parser.cpp"
		break;
	}
	case HEX_INTEGER:
	{
		i1 = LT(1);
		match(HEX_INTEGER);
#line 1544 "Vrml97Parser.g"
		std::istringstream(i1->getText()) >> val;
#line 3495 "Vrml97Parser.cpp"
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
#line 1618 "Vrml97Parser.g"
	
	using OpenVRML_::fpequal;
	float x, y, z, angle;
	
#line 3514 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
	angle=floatValue();
#line 1624 "Vrml97Parser.g"
	
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
	
#line 3537 "Vrml97Parser.cpp"
}

double  Vrml97Parser::doubleValue() {
#line 1691 "Vrml97Parser.g"
	double val = 0.0;
#line 3543 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		d0 = LT(1);
		match(REAL);
#line 1693 "Vrml97Parser.g"
		std::istringstream(d0->getText()) >> val;
#line 3554 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		d1 = LT(1);
		match(INTEGER);
#line 1694 "Vrml97Parser.g"
		std::istringstream(d1->getText()) >> val;
#line 3563 "Vrml97Parser.cpp"
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
#line 1715 "Vrml97Parser.g"
	
	float x, y;
	
#line 3581 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
#line 1720 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
#line 3588 "Vrml97Parser.cpp"
}

void Vrml97Parser::vec3fValue(
	vec3f & v
) {
#line 1744 "Vrml97Parser.g"
	
	float x, y, z;
	
#line 3598 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
#line 1749 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
	v.z(z);
#line 3607 "Vrml97Parser.cpp"
}

void Vrml97Parser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory )
{
}
const char* Vrml97Parser::tokenNames[] = {
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


