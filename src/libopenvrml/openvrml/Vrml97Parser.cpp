/* $ANTLR 2.7.2: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#line 112 "Vrml97Parser.g"

# include "Vrml97Parser.hpp"
ANTLR_RBRACE

#line 8 "Vrml97Parser.cpp"
#include "Vrml97Parser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 117 "Vrml97Parser.g"

# include <cctype>
# include <iostream>
# include <antlr/CommonToken.hpp>
# include <boost/lexical_cast.hpp>
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

            if (this->prev_char_ == '0'
                    && (this->c_ == 'x' || this->c_ == 'X')) {
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

    token->setLine(int(this->line_));
    token->setColumn(int(this->col_));
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
        std::unary_function<openvrml::node_interface, bool> {
    explicit interface_id_equals_(const std::string & interface_id):
        interface_id(&interface_id)
    {}

    bool operator()(const openvrml::node_interface & interface) const
    {
        return interface.id == *this->interface_id;
    }

private:
    const std::string * interface_id;
};

} // namespace

namespace openvrml ANTLR_LBRACE


#line 485 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"
#line 487 "Vrml97Parser.cpp"
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
	openvrml::browser & browser,
          std::vector<node_ptr> & nodes
) {
#line 610 "Vrml97Parser.g"
	
	const scope_ptr scope(new Vrml97RootScope(browser, this->uri));
	
#line 521 "Vrml97Parser.cpp"
	
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
	openvrml::browser & browser,
          std::vector<node_ptr> & nodes,
          const scope_ptr & scope
) {
#line 618 "Vrml97Parser.g"
	
	node_ptr node;
	node_type_ptr nodeType;
	
#line 554 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(browser, scope);
#line 625 "Vrml97Parser.g"
			
			//
			// If we are unable to parse a node, node will be null.
			//
			if (node) { nodes.push_back(node); }
			
#line 570 "Vrml97Parser.cpp"
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

node_ptr  Vrml97Parser::nodeStatement(
	openvrml::browser & browser, const scope_ptr & scope
) {
#line 635 "Vrml97Parser.g"
	node_ptr n;
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
#line 640 "Vrml97Parser.g"
			
			using antlr::SemanticException;
			assert(scope);
			n.reset(scope->find_node(id1->getText()));
			if (!n) {
			throw SemanticException("Node \"" + id1->getText()
			+ "\" has not been defined in this "
			"scope.",
			this->uri,
			id1->getLine(),
			id1->getColumn());
			}
			
#line 635 "Vrml97Parser.cpp"
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
		
#line 654 "Vrml97Parser.cpp"
	}
	return n;
}

void Vrml97Parser::protoStatement(
	openvrml::browser & browser, const scope_ptr & scope
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
	const openvrml::scope & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  from_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventout_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  to_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventin_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_ROUTE);
		from_node_id = LT(1);
		match(ID);
		match(PERIOD);
		eventout_id = LT(1);
		match(ID);
		match(KEYWORD_TO);
		to_node_id = LT(1);
		match(ID);
		match(PERIOD);
		eventin_id = LT(1);
		match(ID);
#line 956 "Vrml97Parser.g"
		
		using std::runtime_error;
		using antlr::SemanticException;
		
		openvrml::node * const from_node =
		scope.find_node(from_node_id->getText());
		if (!from_node) {
		throw SemanticException("Node \"" + from_node_id->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		from_node_id->getLine(),
		from_node_id->getColumn());
		}
		
		openvrml::node * const to_node =
		scope.find_node(to_node_id->getText());
		if (!to_node) {
		throw SemanticException("Node \"" + to_node_id->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		to_node_id->getLine(),
		to_node_id->getColumn());
		}
		
		try {
		add_route(*from_node, eventout_id->getText(),
		*to_node, eventin_id->getText());
		} catch (runtime_error & ex) {
		throw SemanticException(ex.what(),
		this->uri,
		from_node_id->getLine(),
		from_node_id->getColumn());
		}
		
#line 746 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

node_ptr  Vrml97Parser::node(
	openvrml::browser & browser,
     const scope_ptr & scope,
     const std::string & nodeId
) {
#line 1067 "Vrml97Parser.g"
	node_ptr n;
#line 762 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1067 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
	node_type_ptr nodeType;
	
#line 771 "Vrml97Parser.cpp"
	
	if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1077 "Vrml97Parser.g"
		
		n.reset(new script_node(browser.script_node_class_, scope));
		if (!nodeId.empty()) { n->id(nodeId); }
		
		script_node * const scriptNode = node_cast<script_node *>(n.get());
		assert(scriptNode);
		
#line 784 "Vrml97Parser.cpp"
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
				goto _loop32;
			}
			}
		}
		_loop32:;
		} // ( ... )*
		match(RBRACE);
	}
	else if ((LA(1) == ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 1088 "Vrml97Parser.g"
		
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
		
#line 832 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(scope, *n);
			}
			else {
				goto _loop34;
			}
			
		}
		_loop34:;
		} // ( ... )*
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externproto(
	openvrml::browser & browser, const openvrml::scope_ptr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 876 "Vrml97Parser.g"
	
	openvrml::node_interface_set interfaces;
	openvrml::mfstring urlList;
	openvrml::node_type_ptr nodeType;
	
#line 865 "Vrml97Parser.cpp"
	
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
#line 884 "Vrml97Parser.g"
		
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
		// If we weren't able to create a node_type, that means that we
		// don't already have a node_class for the node. Currently we only
		// support referring to existing node_classes with EXTERNPROTO;
		// adding new node_classes via EXTERNPROTO is not supported. In
		// practice, this means that the ordinary way of using EXTERNPROTOs
		// in VRML worlds will fail.
		//
		if (nodeType) {
		if (!scope->add_type(nodeType)) {
		using antlr::SemanticException;
		throw SemanticException("Node type \"" + nodeType->id
		+ "\" has already been defined in "
		" this scope.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		}
		
#line 917 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	openvrml::browser & browser, const scope_ptr & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 669 "Vrml97Parser.g"
	
	using std::vector;
	
	node_interface_set interfaces;
	proto_node_class::default_value_map_t default_value_map;
	vector<node_ptr> impl_nodes;
	proto_node_class::is_map_t is_map;
	proto_node_class::routes_t routes;
	
#line 940 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 679 "Vrml97Parser.g"
		
		scope_ptr proto_scope(new openvrml::scope(id->getText(), scope));
		
#line 950 "Vrml97Parser.cpp"
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
#line 682 "Vrml97Parser.g"
				
				// XXX
				// XXX Hmm... We give each interface declaration its own scope
				// XXX here. This is wasteful; Vrml97RootScope is expensive and
				// XXX we only *really* need this for SFNode and MFNode
				// XXX fields/exposedFields.
				// XXX
				scope_ptr interface_decl_scope(
				new Vrml97RootScope(browser, this->uri));
				
#line 966 "Vrml97Parser.cpp"
				protoInterfaceDeclaration(browser,
                                      interface_decl_scope,
                                      interfaces,
                                      default_value_map);
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		} // ( ... )*
		match(RBRACKET);
		match(LBRACE);
		protoBody(browser,
                                     proto_scope,
                                     interfaces,
                                     impl_nodes,
                                     is_map,
                                     routes);
		match(RBRACE);
#line 702 "Vrml97Parser.g"
		
		node_class_ptr node_class(new proto_node_class(browser,
		interfaces,
		default_value_map,
		impl_nodes,
		is_map,
		routes));
		//
		// Add the new node_class (prototype definition) to the browser's
		// node_class_map.
		//
		// First, construct the id for the node implementation.
		//
		std::string impl_id;
		do {
		impl_id = '#' + proto_scope->id + impl_id;
		} while ((proto_scope = proto_scope->parent));
		impl_id = scope->id + impl_id;
		browser.node_class_map.insert(make_pair(impl_id, node_class));
		
		//
		// PROTOs implicitly introduce a new node type as well.
		//
		const node_type_ptr node_type =
		node_class->create_type(id->getText(), interfaces);
		assert(node_type);
		assert(scope);
		if (!scope->add_type(node_type)) {
		using antlr::SemanticException;
		throw SemanticException("Node type \"" + node_type->id
		+ "\" has already been defined in "
		"this scope.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 1026 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	
    openvrml::browser & browser,
    const scope_ptr & scope,
    node_interface_set & interfaces,
    proto_node_class::default_value_map_t & default_value_map

) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 741 "Vrml97Parser.g"
	
	using std::invalid_argument;
	using antlr::SemanticException;
	
	node_interface::type_id it;
	field_value::type_id ft;
	field_value_ptr fv;
	
#line 1054 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 754 "Vrml97Parser.g"
			
			try {
			add_interface(interfaces,
			node_interface(it, ft, id0->getText()));
			} catch (invalid_argument & ex) {
			throw SemanticException(ex.what(),
			this->uri,
			id0->getLine(),
			id0->getColumn());
			}
			
#line 1077 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
			fv=fieldValue(browser, scope, ft);
#line 766 "Vrml97Parser.g"
			
			assert(fv);
			try {
			add_interface(interfaces,
			node_interface(it, ft, id1->getText()));
			} catch (invalid_argument & ex) {
			throw SemanticException(ex.what(),
			this->uri,
			id0->getLine(),
			id0->getColumn());
			}
			const bool succeeded =
			default_value_map.insert(make_pair(id1->getText(), fv)).second;
			assert(succeeded);
			
#line 1104 "Vrml97Parser.cpp"
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
	openvrml::browser & browser,
          const scope_ptr & scope,
          const node_interface_set & interfaces,
          std::vector<node_ptr> & impl_nodes,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes
) {
#line 793 "Vrml97Parser.g"
	
	assert(scope);
	assert(impl_nodes.empty());
	assert(is_map.empty());
	assert(routes.empty());
	
	node_ptr n;
	
#line 1137 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == KEYWORD_EXTERNPROTO || LA(1) == KEYWORD_PROTO)) {
				protoStatement(browser, scope);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		} // ( ... )*
		n=protoNodeStatement(browser, scope, interfaces, is_map, routes);
#line 809 "Vrml97Parser.g"
		
		assert(n);
		impl_nodes.push_back(n);
		
#line 1158 "Vrml97Parser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(browser,
                            scope,
                            interfaces,
                            impl_nodes,
                            is_map,
                            routes);
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

node_interface::type_id  Vrml97Parser::eventInterfaceType() {
#line 783 "Vrml97Parser.g"
	node_interface::type_id it;
#line 1187 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 784 "Vrml97Parser.g"
			it = node_interface::eventin_id;
#line 1196 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 785 "Vrml97Parser.g"
			it = node_interface::eventout_id;
#line 1204 "Vrml97Parser.cpp"
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

openvrml::field_value::type_id  Vrml97Parser::fieldType() {
#line 1518 "Vrml97Parser.g"
	openvrml::field_value::type_id ft =
         openvrml::field_value::invalid_type_id;
#line 1225 "Vrml97Parser.cpp"
#line 1518 "Vrml97Parser.g"
	
	using openvrml::field_value;
	
#line 1230 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 1524 "Vrml97Parser.g"
			ft = field_value::mfcolor_id;
#line 1239 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 1525 "Vrml97Parser.g"
			ft = field_value::mffloat_id;
#line 1247 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 1526 "Vrml97Parser.g"
			ft = field_value::mfint32_id;
#line 1255 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 1527 "Vrml97Parser.g"
			ft = field_value::mfnode_id;
#line 1263 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 1528 "Vrml97Parser.g"
			ft = field_value::mfrotation_id;
#line 1271 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 1529 "Vrml97Parser.g"
			ft = field_value::mfstring_id;
#line 1279 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 1530 "Vrml97Parser.g"
			ft = field_value::mftime_id;
#line 1287 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 1531 "Vrml97Parser.g"
			ft = field_value::mfvec2f_id;
#line 1295 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 1532 "Vrml97Parser.g"
			ft = field_value::mfvec3f_id;
#line 1303 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 1533 "Vrml97Parser.g"
			ft = field_value::sfbool_id;
#line 1311 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 1534 "Vrml97Parser.g"
			ft = field_value::sfcolor_id;
#line 1319 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 1535 "Vrml97Parser.g"
			ft = field_value::sffloat_id;
#line 1327 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 1536 "Vrml97Parser.g"
			ft = field_value::sfimage_id;
#line 1335 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 1537 "Vrml97Parser.g"
			ft = field_value::sfint32_id;
#line 1343 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 1538 "Vrml97Parser.g"
			ft = field_value::sfnode_id;
#line 1351 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 1539 "Vrml97Parser.g"
			ft = field_value::sfrotation_id;
#line 1359 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 1540 "Vrml97Parser.g"
			ft = field_value::sfstring_id;
#line 1367 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 1541 "Vrml97Parser.g"
			ft = field_value::sftime_id;
#line 1375 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 1542 "Vrml97Parser.g"
			ft = field_value::sfvec2f_id;
#line 1383 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 1543 "Vrml97Parser.g"
			ft = field_value::sfvec3f_id;
#line 1391 "Vrml97Parser.cpp"
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

node_interface::type_id  Vrml97Parser::fieldInterfaceType() {
#line 788 "Vrml97Parser.g"
	node_interface::type_id it;
#line 1411 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 789 "Vrml97Parser.g"
			it = node_interface::field_id;
#line 1420 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 790 "Vrml97Parser.g"
			it = node_interface::exposedfield_id;
#line 1428 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::fieldValue(
	openvrml::browser & browser,
           const openvrml::scope_ptr & scope,
           openvrml::field_value::type_id ft
) {
#line 1546 "Vrml97Parser.g"
	openvrml::field_value_ptr fv;
#line 1452 "Vrml97Parser.cpp"
#line 1546 "Vrml97Parser.g"
	
	using openvrml::field_value;
	
#line 1457 "Vrml97Parser.cpp"
	
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

node_ptr  Vrml97Parser::protoNodeStatement(
	openvrml::browser & browser,
                   const openvrml::scope_ptr & scope,
                   const node_interface_set & proto_interfaces,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes
) {
#line 841 "Vrml97Parser.g"
	node_ptr n;
#line 1481 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 841 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
#line 1488 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(browser,
                                       scope,
                                       proto_interfaces,
                                       is_map,
                                       routes,
                                       id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 857 "Vrml97Parser.g"
		
		n.reset(scope->find_node(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in this "
		+ "scope.",
		this->uri,
		id1->getLine(),
		id1->getColumn());
		}
		
#line 1521 "Vrml97Parser.cpp"
		break;
	}
	case ID:
	{
		n=protoNode(browser,
                    scope,
                    proto_interfaces,
                    is_map,
                    routes,
                    std::string());
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
	openvrml::browser & browser,
                   const scope_ptr & scope,
                   const node_interface_set & interfaces,
                   std::vector<node_ptr> & impl_nodes,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes
) {
#line 821 "Vrml97Parser.g"
	
	assert(scope);
	assert(impl_nodes.size() == 1);
	
	node_ptr n;
	
#line 1557 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(browser, scope, interfaces, is_map, routes);
#line 833 "Vrml97Parser.g"
			
			assert(n);
			impl_nodes.push_back(n);
			
#line 1571 "Vrml97Parser.cpp"
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
			protoRouteStatement(*scope, routes);
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

void Vrml97Parser::protoRouteStatement(
	const openvrml::scope & scope,
                    proto_node_class::routes_t & routes
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  from_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventout_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  to_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventin_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_ROUTE);
		from_node_id = LT(1);
		match(ID);
		match(PERIOD);
		eventout_id = LT(1);
		match(ID);
		match(KEYWORD_TO);
		to_node_id = LT(1);
		match(ID);
		match(PERIOD);
		eventin_id = LT(1);
		match(ID);
#line 998 "Vrml97Parser.g"
		
		using std::runtime_error;
		using antlr::SemanticException;
		
		openvrml::node * const from_node =
		scope.find_node(from_node_id->getText());
		if (!from_node) {
		throw SemanticException("Node \"" + from_node_id->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		from_node_id->getLine(),
		from_node_id->getColumn());
		}
		
		openvrml::node * const to_node =
		scope.find_node(to_node_id->getText());
		if (!to_node) {
		throw SemanticException("Node \"" + to_node_id->getText()
		+ "\" has not been defined in this "
		"scope.",
		this->uri,
		to_node_id->getLine(),
		to_node_id->getColumn());
		}
		
		//
		// Routes outside PROTO definitions get added with add_route, which
		// does the appropriate validation.  However, in a PROTO
		// definition, we don't actually establish a route; the actual
		// routes (event_listeners) are established when the PROTO is
		// instantiated.  So, we need to do validation here.
		//
		const field_value::type_id eventout_type =
		from_node->type.has_eventout(eventout_id->getText());
		if (eventout_type == field_value::invalid_type_id) {
		throw SemanticException(from_node->type.id + " node has no "
		"eventOut \"" + eventout_id->getText()
		+ "\".",
		this->uri,
		eventout_id->getLine(),
		eventout_id->getColumn());
		}
		
		const field_value::type_id eventin_type =
		to_node->type.has_eventin(eventin_id->getText());
		if (eventin_type == field_value::invalid_type_id) {
		throw SemanticException(to_node->type.id + " node has no "
		"eventIn \"" + eventin_id->getText()
		+ "\".",
		this->uri,
		eventin_id->getLine(),
		eventin_id->getColumn());
		}
		
		if (eventin_type != eventout_type) {
		throw SemanticException("Mismatch between eventOut and "
		"eventIn types.",
		this->uri,
		eventin_id->getLine(),
		eventin_id->getColumn());
		}
		
		const proto_node_class::route r(*from_node, eventout_id->getText(),
		*to_node, eventin_id->getText());
		routes.push_back(r);
		
#line 1688 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

node_ptr  Vrml97Parser::protoNode(
	openvrml::browser & browser,
          const scope_ptr & scope,
          const node_interface_set & proto_interfaces,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes,
          const std::string & nodeId
) {
#line 1191 "Vrml97Parser.g"
	node_ptr n;
#line 1707 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1191 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	
	node_type_ptr nodeType;
	
#line 1716 "Vrml97Parser.cpp"
	
	if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 1204 "Vrml97Parser.g"
		
		n.reset(new script_node(browser.script_node_class_, scope));
		if (!nodeId.empty()) { n->id(nodeId); }
		
		script_node * const scriptNode = node_cast<script_node *>(n.get());
		assert(scriptNode);
		
#line 1729 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(browser,
                                 scope,
                                 proto_interfaces,
                                 is_map,
                                 routes,
                                 *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(browser,
                                              scope,
                                              proto_interfaces,
                                              is_map,
                                              routes,
                                              *scriptNode);
				break;
			}
			default:
			{
				goto _loop40;
			}
			}
		}
		_loop40:;
		} // ( ... )*
		match(RBRACE);
	}
	else if ((LA(1) == ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 1226 "Vrml97Parser.g"
		
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
		
#line 1785 "Vrml97Parser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(browser,
                                     scope,
                                     proto_interfaces,
                                     is_map,
                                     routes,
                                     *n);
			}
			else {
				goto _loop42;
			}
			
		}
		_loop42:;
		} // ( ... )*
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externInterfaceDeclaration(
	openvrml::node_interface_set & interfaces
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 916 "Vrml97Parser.g"
	
	using openvrml::node_interface;
	using openvrml::field_value;
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 1825 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 924 "Vrml97Parser.g"
		
		const node_interface interface(it, ft, id->getText());
		try {
		add_interface(interfaces, interface);
		} catch (std::invalid_argument & ex) {
			throw SemanticException(ex.what(),
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 1844 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

mfstring  Vrml97Parser::externprotoUrlList() {
#line 942 "Vrml97Parser.g"
	mfstring urlList;
#line 1856 "Vrml97Parser.cpp"
#line 942 "Vrml97Parser.g"
	
	using std::string;
	using openvrml::mfstring;
	
	string s;
	
#line 1864 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 949 "Vrml97Parser.g"
			urlList.value.push_back(s);
#line 1873 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == STRING)) {
					s=stringValue();
#line 950 "Vrml97Parser.g"
					urlList.value.push_back(s);
#line 1885 "Vrml97Parser.cpp"
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

node_interface::type_id  Vrml97Parser::interfaceType() {
#line 937 "Vrml97Parser.g"
	node_interface::type_id interface_type;
#line 1914 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			interface_type=eventInterfaceType();
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			interface_type=fieldInterfaceType();
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
	return interface_type;
}

std::string  Vrml97Parser::stringValue() {
#line 1910 "Vrml97Parser.g"
	std::string str;
#line 1947 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	s = LT(1);
	match(STRING);
#line 1913 "Vrml97Parser.g"
	
	const std::string temp(s->getText());
	str = std::string(temp.begin() + 1, temp.end() - 1);
	
#line 1957 "Vrml97Parser.cpp"
	return str;
}

void Vrml97Parser::nodeBodyElement(
	const scope_ptr & scope, openvrml::node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1104 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	field_value::type_id ft(field_value::invalid_type_id);
	field_value_ptr fv;
	
#line 1971 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 1110 "Vrml97Parser.g"
			
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
			
#line 1994 "Vrml97Parser.cpp"
			fv=fieldValue(node.type.node_class.browser, scope, ft);
#line 1124 "Vrml97Parser.g"
			
			assert(fv);
			node.field(id->getText(), *fv);
			
#line 2001 "Vrml97Parser.cpp"
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
			protoStatement(node.type.node_class.browser, scope);
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
		consumeUntil(_tokenSet_14);
	}
}

void Vrml97Parser::scriptInterfaceDeclaration(
	const scope_ptr & scope, script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1132 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 2038 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1138 "Vrml97Parser.g"
			
			const node_interface_set::const_iterator pos =
			find_interface(node.node::type.interfaces(),
			id->getText());
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
			
#line 2073 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_14);
	}
}

void Vrml97Parser::scriptFieldInterfaceDeclaration(
	const scope_ptr & scope, script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1164 "Vrml97Parser.g"
	
	using std::find_if;
	using antlr::SemanticException;
	
	field_value::type_id ft = field_value::invalid_type_id;
	field_value_ptr fv;
	
#line 2106 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(node.node::type.node_class.browser, scope, ft);
#line 1173 "Vrml97Parser.g"
		
		assert(fv);
		const node_interface_set & interfaces =
		node.node::type.interfaces();
		const node_interface_set::const_iterator pos =
		find_interface(interfaces, id->getText());
		if (pos != interfaces.end()) {
		throw SemanticException("Interface \"" + id->getText()
		+ "\" already declared for Script "
		"node.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		node.add_field(id->getText(), fv);
		
#line 2131 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_14);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	openvrml::browser & browser,
                     const scope_ptr & scope,
                     const node_interface_set & proto_interfaces,
                     proto_node_class::is_map_t & is_map,
                     proto_node_class::routes_t & routes,
                     openvrml::node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  interface_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1246 "Vrml97Parser.g"
	
	using std::string;
	using antlr::SemanticException;
	
	field_value::type_id ft;
	field_value_ptr fv;
	
#line 2157 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			interface_id = LT(1);
			match(ID);
#line 1259 "Vrml97Parser.g"
			
			const node_interface_set::const_iterator impl_node_interface =
			find_interface(node.type.interfaces(),
			interface_id->getText());
			if (impl_node_interface == node.type.interfaces().end()) {
			throw SemanticException(node.type.id + " node has no "
			"interface \""
			+ interface_id->getText() + "\".",
			this->uri,
			interface_id->getLine(),
			interface_id->getColumn());
			}
			
#line 2179 "Vrml97Parser.cpp"
			{
			if (!(impl_node_interface->type == node_interface::field_id
             || impl_node_interface->type == node_interface::exposedfield_id))
				throw ANTLR_USE_NAMESPACE(antlr)SemanticException("impl_node_interface->type == node_interface::field_id\n             || impl_node_interface->type == node_interface::exposedfield_id");
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
				fv=protoFieldValue(browser,
                                       scope,
                                       proto_interfaces,
                                       is_map,
                                       routes,
                                       impl_node_interface->field_type);
#line 1280 "Vrml97Parser.g"
				
				assert(fv);
				node.field(interface_id->getText(), *fv);
				
#line 2209 "Vrml97Parser.cpp"
				break;
			}
			case KEYWORD_IS:
			{
				isStatement(proto_interfaces,
                                is_map,
                                node,
                                *impl_node_interface);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
			break;
		}
		case KEYWORD_ROUTE:
		{
			protoRouteStatement(*scope, routes);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(browser, scope);
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
		consumeUntil(_tokenSet_14);
	}
}

void Vrml97Parser::protoScriptInterfaceDeclaration(
	openvrml::browser & browser,
                                const openvrml::scope_ptr & scope,
                                const node_interface_set & proto_interfaces,
                                proto_node_class::is_map_t & is_map,
                                proto_node_class::routes_t & routes,
                                openvrml::script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1363 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	node_interface::type_id it;
	field_value::type_id ft;
	
#line 2268 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 1374 "Vrml97Parser.g"
			
			const node_interface_set::const_iterator pos =
			find_interface(node.node::type.interfaces(), id->getText());
			if (pos != node.node::type.interfaces().end()) {
			throw SemanticException("Interface \"" + id->getText()
			+ "\" already declared for Script "
			"node.",
			this->uri,
			id->getLine(),
			id->getColumn());
			}
			node_interface_set::const_iterator interface;
			switch (it) {
			case node_interface::eventin_id:
			interface = node.add_eventin(ft, id->getText());
			break;
			case node_interface::eventout_id:
			interface = node.add_eventout(ft, id->getText());
			break;
			default:
			assert(false);
			}
			
#line 2303 "Vrml97Parser.cpp"
			{
			switch ( LA(1)) {
			case KEYWORD_IS:
			{
				isStatement(proto_interfaces, is_map, node, *interface);
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
			protoScriptFieldInterfaceDeclaration(browser,
                                             scope,
                                             proto_interfaces,
                                             is_map,
                                             routes,
                                             node);
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
		consumeUntil(_tokenSet_14);
	}
}

field_value_ptr  Vrml97Parser::protoFieldValue(
	openvrml::browser & browser,
                const scope_ptr & scope,
                const node_interface_set & proto_interfaces,
                proto_node_class::is_map_t & is_map,
                proto_node_class::routes_t & routes,
                const field_value::type_id ft
) {
#line 1559 "Vrml97Parser.g"
	field_value_ptr fv;
#line 2363 "Vrml97Parser.cpp"
	
	if (((_tokenSet_10.member(LA(1))))&&( (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) )) {
		fv=protoNodeFieldValue(browser,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft);
#line 1573 "Vrml97Parser.g"
		
		assert(fv);
		
#line 2376 "Vrml97Parser.cpp"
	}
	else if ((_tokenSet_11.member(LA(1)))) {
		fv=nonNodeFieldValue(ft);
#line 1576 "Vrml97Parser.g"
		assert(fv);
#line 2382 "Vrml97Parser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

void Vrml97Parser::isStatement(
	const node_interface_set & proto_interfaces,
            proto_node_class::is_map_t & is_map,
            openvrml::node & node,
            const node_interface & impl_node_interface
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1294 "Vrml97Parser.g"
	
	using std::string;
	using boost::lexical_cast;
	using antlr::SemanticException;
	
#line 2404 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 1303 "Vrml97Parser.g"
		
		node_interface_set::const_iterator proto_interface =
		find_interface(proto_interfaces, id->getText());
		if (proto_interface == proto_interfaces.end()) {
		throw SemanticException("PROTO has no interface \""
		+ id->getText() + "\".",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
		//
		// An exposedField in the PROTO implementation can be mapped to
		// an exposedField, field, eventIn, or eventOut in the PROTO
		// PROTO interface.  Otherwise, the PROTO interface and
		// implementation interface types must agree.
		//
		assert(proto_interface->type
		!= node_interface::invalid_type_id);
		assert(impl_node_interface.type
		!= node_interface::invalid_type_id);
		
		if (proto_interface->type != impl_node_interface.type
		&& impl_node_interface.type != node_interface::exposedfield_id)
		{
		throw SemanticException(
		"Cannot map a(n) "
		+ lexical_cast<string>(proto_interface->type)
		+ " in the PROTO interface to a(n) "
		+ lexical_cast<string>(impl_node_interface.type)
		+ " in the PROTO implementation.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
		assert(proto_interface->field_type
		!= field_value::invalid_type_id);
		assert(impl_node_interface.field_type
		!= field_value::invalid_type_id);
		
		if (proto_interface->field_type != impl_node_interface.field_type)
		{
		throw SemanticException("Type mismatch.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
		//
		// Finally, add the IS mapping.
		//
		const proto_node_class::is_map_t::value_type
		value(proto_interface->id,
		proto_node_class::is_target(node,
		impl_node_interface.id));
		is_map.insert(value);
		
#line 2469 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_14);
	}
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	
    openvrml::browser & browser,
    const scope_ptr & scope,
    const node_interface_set & proto_interfaces,
    proto_node_class::is_map_t & is_map,
    proto_node_class::routes_t & routes,
    script_node & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  proto_field_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1405 "Vrml97Parser.g"
	
	using std::auto_ptr;
	using std::find_if;
	using std::string;
	using boost::lexical_cast;
	using antlr::SemanticException;
	
	field_value::type_id ft;
	field_value_ptr fv;
	
#line 2500 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1422 "Vrml97Parser.g"
		
		//
		// We need to check if the fieldId is an exact match for any
		// existing interface for the Script node; so, we don't use
		// node_interface_set::find.
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
		
#line 2528 "Vrml97Parser.cpp"
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
			fv=protoFieldValue(browser,
                                   scope,
                                   proto_interfaces,
                                   is_map,
                                   routes,
                                   ft);
#line 1448 "Vrml97Parser.g"
			
			assert(fv);
			node.add_field(id->getText(), fv);
			
#line 2555 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
			match(KEYWORD_IS);
			proto_field_id = LT(1);
			match(ID);
#line 1453 "Vrml97Parser.g"
			
			auto_ptr<field_value> value = field_value::create(ft);
			const node_interface_set::const_iterator impl_node_interface =
			node.add_field(id->getText(), field_value_ptr(value));
			
			const node_interface_set::const_iterator proto_interface =
			find_interface(proto_interfaces,
			proto_field_id->getText());
			if (proto_interface == proto_interfaces.end()) {
			throw SemanticException("PROTO has no interface \""
			+ proto_field_id->getText()
			+ "\".",
			this->uri,
			proto_field_id->getLine(),
			proto_field_id->getColumn());
			}
			
			assert(proto_interface->type
			!= node_interface::invalid_type_id);
			assert(impl_node_interface->type
			!= node_interface::invalid_type_id);
			
			assert(impl_node_interface->type
			!= node_interface::exposedfield_id);
			
			if (proto_interface->type != impl_node_interface->type) {
			throw SemanticException(
			"Cannot map a "
			+ lexical_cast<string>(proto_interface->type)
			+ " in the PROTO interface to a "
			+ lexical_cast<string>(impl_node_interface->type)
			+ " in the PROTO implementation.",
			this->uri,
			proto_field_id->getLine(),
			proto_field_id->getColumn());
			}
			
			assert(proto_interface->field_type
			!= field_value::invalid_type_id);
			assert(impl_node_interface->field_type
			!= field_value::invalid_type_id);
			
			if (proto_interface->field_type
			!= impl_node_interface->field_type) {
			throw SemanticException("Type mismatch.",
			this->uri,
			proto_field_id->getLine(),
			proto_field_id->getColumn());
			}
			
			//
			// Finally, add the IS mapping.
			//
			{
			const proto_node_class::is_map_t::value_type
			value(proto_interface->id,
			proto_node_class::is_target(
			node,
			impl_node_interface->id));
			is_map.insert(value);
			}
			
#line 2627 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_14);
	}
}

openvrml::field_value_ptr  Vrml97Parser::nodeFieldValue(
	openvrml::browser & browser,
               const openvrml::scope_ptr & scope,
               openvrml::field_value::type_id ft
) {
#line 1605 "Vrml97Parser.g"
	openvrml::field_value_ptr fv;
#line 2651 "Vrml97Parser.cpp"
#line 1605 "Vrml97Parser.g"
	
	using openvrml::field_value;
	
#line 2656 "Vrml97Parser.cpp"
	
	if (((_tokenSet_15.member(LA(1))))&&( ft == field_value::sfnode_id )) {
		fv=sfNodeValue(browser, scope);
	}
	else if ((_tokenSet_16.member(LA(1)))) {
		fv=mfNodeValue(browser, scope);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

openvrml::field_value_ptr  Vrml97Parser::nonNodeFieldValue(
	openvrml::field_value::type_id ft
) {
#line 1579 "Vrml97Parser.g"
	openvrml::field_value_ptr fv = openvrml::field_value_ptr();
#line 2676 "Vrml97Parser.cpp"
#line 1579 "Vrml97Parser.g"
	
	using openvrml::field_value;
	
#line 2681 "Vrml97Parser.cpp"
	
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

field_value_ptr  Vrml97Parser::protoNodeFieldValue(
	openvrml::browser & browser,
                    const scope_ptr & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_class::is_map_t & is_map,
                    proto_node_class::routes_t & routes,
                    field_value::type_id ft
) {
#line 1617 "Vrml97Parser.g"
	field_value_ptr fv;
#line 2754 "Vrml97Parser.cpp"
	
	if (((_tokenSet_15.member(LA(1))))&&( ft == field_value::sfnode_id )) {
		fv=protoSfNodeValue(browser, scope, proto_interfaces, is_map, routes);
	}
	else if ((_tokenSet_16.member(LA(1)))) {
		fv=protoMfNodeValue(browser, scope, proto_interfaces, is_map, routes);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

openvrml::field_value_ptr  Vrml97Parser::sfBoolValue() {
#line 1630 "Vrml97Parser.g"
	openvrml::field_value_ptr sbv;
#line 2772 "Vrml97Parser.cpp"
#line 1630 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2777 "Vrml97Parser.cpp"
	
	val=boolValue();
#line 1635 "Vrml97Parser.g"
	sbv.reset(new sfbool(val));
#line 2782 "Vrml97Parser.cpp"
	return sbv;
}

openvrml::field_value_ptr  Vrml97Parser::sfColorValue() {
#line 1644 "Vrml97Parser.g"
	openvrml::field_value_ptr scv;
#line 2789 "Vrml97Parser.cpp"
#line 1644 "Vrml97Parser.g"
	
	color c;
	
#line 2794 "Vrml97Parser.cpp"
	
	colorValue(c);
#line 1649 "Vrml97Parser.g"
	scv.reset(new sfcolor(c));
#line 2799 "Vrml97Parser.cpp"
	return scv;
}

openvrml::field_value_ptr  Vrml97Parser::sfFloatValue() {
#line 1692 "Vrml97Parser.g"
	openvrml::field_value_ptr sfv;
#line 2806 "Vrml97Parser.cpp"
#line 1692 "Vrml97Parser.g"
	
	float f;
	
#line 2811 "Vrml97Parser.cpp"
	
	f=floatValue();
#line 1697 "Vrml97Parser.g"
	sfv.reset(new sffloat(f));
#line 2816 "Vrml97Parser.cpp"
	return sfv;
}

openvrml::field_value_ptr  Vrml97Parser::sfImageValue() {
#line 1718 "Vrml97Parser.g"
	openvrml::field_value_ptr siv;
#line 2823 "Vrml97Parser.cpp"
#line 1718 "Vrml97Parser.g"
	
	using antlr::SemanticException;
	size_t x, y, comp;
	int32 pixel;
	size_t pixel_index = 0;
	
#line 2831 "Vrml97Parser.cpp"
	
	x=intValue();
	y=intValue();
	comp=intValue();
#line 1726 "Vrml97Parser.g"
	
	image img(x, y, comp);
	
#line 2840 "Vrml97Parser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
			pixel=intValue();
#line 1729 "Vrml97Parser.g"
			
			if (!(pixel_index < x * y)) {
			throw SemanticException("Too many pixel values for "
			"SFImage.",
			this->uri,
			LT(1)->getLine(),
			LT(1)->getColumn());
			}
			img.pixel(pixel_index++, pixel);
			
#line 2856 "Vrml97Parser.cpp"
		}
		else {
			goto _loop73;
		}
		
	}
	_loop73:;
	} // ( ... )*
#line 1739 "Vrml97Parser.g"
	
	if (pixel_index != x * y) {
	throw antlr::SemanticException("Insufficient pixel values for "
	"SFImage.",
	this->uri,
	LT(1)->getLine(),
	LT(1)->getColumn());
	}
	siv.reset(new sfimage(img));
	
#line 2876 "Vrml97Parser.cpp"
	return siv;
}

openvrml::field_value_ptr  Vrml97Parser::sfInt32Value() {
#line 1751 "Vrml97Parser.g"
	openvrml::field_value_ptr siv;
#line 2883 "Vrml97Parser.cpp"
#line 1751 "Vrml97Parser.g"
	
	long i;
	
#line 2888 "Vrml97Parser.cpp"
	
	i=intValue();
#line 1756 "Vrml97Parser.g"
	siv.reset(new sfint32(i));
#line 2893 "Vrml97Parser.cpp"
	return siv;
}

openvrml::field_value_ptr  Vrml97Parser::sfRotationValue() {
#line 1849 "Vrml97Parser.g"
	openvrml::field_value_ptr srv;
#line 2900 "Vrml97Parser.cpp"
#line 1849 "Vrml97Parser.g"
	rotation r;
#line 2903 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1851 "Vrml97Parser.g"
		srv.reset(new sfrotation(r));
#line 2909 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_17);
	}
	return srv;
}

openvrml::field_value_ptr  Vrml97Parser::sfStringValue() {
#line 1892 "Vrml97Parser.g"
	openvrml::field_value_ptr ssv;
#line 2922 "Vrml97Parser.cpp"
#line 1892 "Vrml97Parser.g"
	std::string s;
#line 2925 "Vrml97Parser.cpp"
	
	s=stringValue();
#line 1895 "Vrml97Parser.g"
	ssv.reset(new sfstring(s));
#line 2930 "Vrml97Parser.cpp"
	return ssv;
}

openvrml::field_value_ptr  Vrml97Parser::sfTimeValue() {
#line 1919 "Vrml97Parser.g"
	openvrml::field_value_ptr stv;
#line 2937 "Vrml97Parser.cpp"
#line 1919 "Vrml97Parser.g"
	double t(0.0);
#line 2940 "Vrml97Parser.cpp"
	
	t=doubleValue();
#line 1922 "Vrml97Parser.g"
	stv.reset(new sftime(t));
#line 2945 "Vrml97Parser.cpp"
	return stv;
}

openvrml::field_value_ptr  Vrml97Parser::sfVec2fValue() {
#line 1942 "Vrml97Parser.g"
	openvrml::field_value_ptr svv;
#line 2952 "Vrml97Parser.cpp"
#line 1942 "Vrml97Parser.g"
	vec2f v;
#line 2955 "Vrml97Parser.cpp"
	
	vec2fValue(v);
#line 1945 "Vrml97Parser.g"
	svv.reset(new sfvec2f(v));
#line 2960 "Vrml97Parser.cpp"
	return svv;
}

openvrml::field_value_ptr  Vrml97Parser::sfVec3fValue() {
#line 1969 "Vrml97Parser.g"
	openvrml::field_value_ptr svv;
#line 2967 "Vrml97Parser.cpp"
#line 1969 "Vrml97Parser.g"
	
	vec3f v;
	
#line 2972 "Vrml97Parser.cpp"
	
	vec3fValue(v);
#line 1974 "Vrml97Parser.g"
	svv.reset(new sfvec3f(v));
#line 2977 "Vrml97Parser.cpp"
	return svv;
}

openvrml::field_value_ptr  Vrml97Parser::mfColorValue() {
#line 1652 "Vrml97Parser.g"
	openvrml::field_value_ptr mcv =
            openvrml::field_value_ptr(new mfcolor);
#line 2985 "Vrml97Parser.cpp"
#line 1652 "Vrml97Parser.g"
	
	color c;
	mfcolor & colors = static_cast<mfcolor &>(*mcv);
	
#line 2991 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		colorValue(c);
#line 1660 "Vrml97Parser.g"
		colors.value.push_back(c);
#line 3000 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				colorValue(c);
#line 1661 "Vrml97Parser.g"
				colors.value.push_back(c);
#line 3012 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::mfFloatValue() {
#line 1700 "Vrml97Parser.g"
	openvrml::field_value_ptr mfv =
            openvrml::field_value_ptr(new mffloat);
#line 3036 "Vrml97Parser.cpp"
#line 1700 "Vrml97Parser.g"
	
	float f;
	mffloat & floats = static_cast<mffloat &>(*mfv);
	
#line 3042 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		f=floatValue();
#line 1708 "Vrml97Parser.g"
		floats.value.push_back(f);
#line 3051 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				f=floatValue();
#line 1709 "Vrml97Parser.g"
				floats.value.push_back(f);
#line 3063 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::mfInt32Value() {
#line 1759 "Vrml97Parser.g"
	openvrml::field_value_ptr miv =
            openvrml::field_value_ptr(new mfint32);
#line 3087 "Vrml97Parser.cpp"
#line 1759 "Vrml97Parser.g"
	
	long i;
	mfint32 & int32s = static_cast<mfint32 &>(*miv);
	
#line 3093 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case HEX_INTEGER:
	{
		i=intValue();
#line 1767 "Vrml97Parser.g"
		int32s.value.push_back(i);
#line 3102 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
				i=intValue();
#line 1768 "Vrml97Parser.g"
				int32s.value.push_back(i);
#line 3114 "Vrml97Parser.cpp"
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

field_value_ptr  Vrml97Parser::mfRotationValue() {
#line 1854 "Vrml97Parser.g"
	field_value_ptr mrv = field_value_ptr(new mfrotation);
#line 3137 "Vrml97Parser.cpp"
#line 1854 "Vrml97Parser.g"
	
	rotation r;
	mfrotation & rotations = static_cast<mfrotation &>(*mrv);
	
#line 3143 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1860 "Vrml97Parser.g"
			rotations.value.push_back(r);
#line 3153 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == INTEGER || LA(1) == REAL)) {
					rotationValue(r);
#line 1861 "Vrml97Parser.g"
					rotations.value.push_back(r);
#line 3165 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_17);
	}
	return mrv;
}

openvrml::field_value_ptr  Vrml97Parser::mfStringValue() {
#line 1898 "Vrml97Parser.g"
	openvrml::field_value_ptr msv =
         openvrml::field_value_ptr(new mfstring);
#line 3195 "Vrml97Parser.cpp"
#line 1898 "Vrml97Parser.g"
	
	std::string s;
	mfstring & strings = static_cast<mfstring &>(*msv);
	
#line 3201 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 1906 "Vrml97Parser.g"
		strings.value.push_back(s);
#line 3209 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == STRING)) {
				s=stringValue();
#line 1907 "Vrml97Parser.g"
				strings.value.push_back(s);
#line 3221 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::mfTimeValue() {
#line 1925 "Vrml97Parser.g"
	openvrml::field_value_ptr mtv = openvrml::field_value_ptr(new mftime);
#line 3244 "Vrml97Parser.cpp"
#line 1925 "Vrml97Parser.g"
	
	double t;
	mftime & times = static_cast<mftime &>(*mtv);
	
#line 3250 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		t=doubleValue();
#line 1932 "Vrml97Parser.g"
		times.value.push_back(t);
#line 3259 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				t=doubleValue();
#line 1933 "Vrml97Parser.g"
				times.value.push_back(t);
#line 3271 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::mfVec2fValue() {
#line 1948 "Vrml97Parser.g"
	openvrml::field_value_ptr mvv =
         openvrml::field_value_ptr(new mfvec2f);
#line 3295 "Vrml97Parser.cpp"
#line 1948 "Vrml97Parser.g"
	
	vec2f v;
	mfvec2f & vec2fs = static_cast<mfvec2f &>(*mvv);
	
#line 3301 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2fValue(v);
#line 1956 "Vrml97Parser.g"
		vec2fs.value.push_back(v);
#line 3310 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec2fValue(v);
#line 1957 "Vrml97Parser.g"
				vec2fs.value.push_back(v);
#line 3322 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::mfVec3fValue() {
#line 1977 "Vrml97Parser.g"
	openvrml::field_value_ptr mvv =
         openvrml::field_value_ptr(new mfvec3f);
#line 3346 "Vrml97Parser.cpp"
#line 1977 "Vrml97Parser.g"
	
	vec3f v;
	mfvec3f & vec3fs = static_cast<mfvec3f &>(*mvv);
	
#line 3352 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3fValue(v);
#line 1985 "Vrml97Parser.g"
		vec3fs.value.push_back(v);
#line 3361 "Vrml97Parser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec3fValue(v);
#line 1986 "Vrml97Parser.g"
				vec3fs.value.push_back(v);
#line 3373 "Vrml97Parser.cpp"
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

openvrml::field_value_ptr  Vrml97Parser::sfNodeValue(
	openvrml::browser & browser,
            const openvrml::scope_ptr & scope
) {
#line 1781 "Vrml97Parser.g"
	openvrml::field_value_ptr snv;
#line 3399 "Vrml97Parser.cpp"
#line 1781 "Vrml97Parser.g"
	
	openvrml::node_ptr n;
	
#line 3404 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1787 "Vrml97Parser.g"
			snv.reset(new sfnode(n));
#line 3415 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1788 "Vrml97Parser.g"
			snv.reset(new sfnode);
#line 3423 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_17);
	}
	return snv;
}

openvrml::field_value_ptr  Vrml97Parser::mfNodeValue(
	openvrml::browser & browser,
            const openvrml::scope_ptr & scope
) {
#line 1809 "Vrml97Parser.g"
	openvrml::field_value_ptr mnv = openvrml::field_value_ptr(new mfnode);
#line 3446 "Vrml97Parser.cpp"
#line 1809 "Vrml97Parser.g"
	
	openvrml::node_ptr n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3452 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(browser, scope);
#line 1816 "Vrml97Parser.g"
			if (n) { nodes.value.push_back(n); }
#line 3463 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
					n=nodeStatement(browser, scope);
#line 1818 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3477 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_17);
	}
	return mnv;
}

field_value_ptr  Vrml97Parser::protoSfNodeValue(
	openvrml::browser & browser,
                 const scope_ptr & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes
) {
#line 1791 "Vrml97Parser.g"
	field_value_ptr snv;
#line 3512 "Vrml97Parser.cpp"
#line 1791 "Vrml97Parser.g"
	
	node_ptr n;
	
#line 3517 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(browser, scope, proto_interfaces, is_map, routes);
#line 1801 "Vrml97Parser.g"
			
			snv.reset(new sfnode(n));
			
#line 3530 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
#line 1804 "Vrml97Parser.g"
			
			snv.reset(new sfnode);
			
#line 3540 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_14);
	}
	return snv;
}

field_value_ptr  Vrml97Parser::protoMfNodeValue(
	openvrml::browser & browser,
                 const scope_ptr & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes
) {
#line 1824 "Vrml97Parser.g"
	field_value_ptr mnv = field_value_ptr(new mfnode);
#line 3566 "Vrml97Parser.cpp"
#line 1824 "Vrml97Parser.g"
	
	node_ptr n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3572 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(browser, scope, proto_interfaces, is_map, routes);
#line 1835 "Vrml97Parser.g"
			
			if (n) { nodes.value.push_back(n); }
			
#line 3585 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{ // ( ... )*
			for (;;) {
				if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
					n=protoNodeStatement(browser,
                                 scope,
                                 proto_interfaces,
                                 is_map,
                                 routes);
#line 1843 "Vrml97Parser.g"
					
					if (n) { nodes.value.push_back(n); }
					
#line 3603 "Vrml97Parser.cpp"
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
		consumeUntil(_tokenSet_14);
	}
	return mnv;
}

bool  Vrml97Parser::boolValue() {
#line 1638 "Vrml97Parser.g"
	bool val = false;
#line 3632 "Vrml97Parser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_TRUE:
	{
		match(KEYWORD_TRUE);
#line 1640 "Vrml97Parser.g"
		val = true;
#line 3640 "Vrml97Parser.cpp"
		break;
	}
	case KEYWORD_FALSE:
	{
		match(KEYWORD_FALSE);
#line 1641 "Vrml97Parser.g"
		val = false;
#line 3648 "Vrml97Parser.cpp"
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
#line 1664 "Vrml97Parser.g"
	
	float r, g, b;
	
#line 3666 "Vrml97Parser.cpp"
	
	r=colorComponent();
	g=colorComponent();
	b=colorComponent();
#line 1669 "Vrml97Parser.g"
	c.r(r);
	c.g(g);
	c.b(b);
#line 3675 "Vrml97Parser.cpp"
}

float  Vrml97Parser::colorComponent() {
#line 1677 "Vrml97Parser.g"
	float val = 0.0f;
#line 3681 "Vrml97Parser.cpp"
	
	val=floatValue();
#line 1679 "Vrml97Parser.g"
	
	if (val < 0.0 || val > 1.0) {
	this->reportWarning("Color component values must be from 0 to "
	"1.");
	if (val < 0.0) {
	val = 0.0;
	} else if (val > 1.0) {
	val = 1.0;
	}
	}
	
#line 3696 "Vrml97Parser.cpp"
	return val;
}

float  Vrml97Parser::floatValue() {
#line 1712 "Vrml97Parser.g"
	float val;
#line 3703 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		f0 = LT(1);
		match(REAL);
#line 1714 "Vrml97Parser.g"
		std::istringstream(f0->getText()) >> val;
#line 3714 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		f1 = LT(1);
		match(INTEGER);
#line 1715 "Vrml97Parser.g"
		std::istringstream(f1->getText()) >> val;
#line 3723 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

int32  Vrml97Parser::intValue() {
#line 1771 "Vrml97Parser.g"
	int32 val;
#line 3737 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case INTEGER:
	{
		i0 = LT(1);
		match(INTEGER);
#line 1773 "Vrml97Parser.g"
		
		std::istringstream(i0->getText()) >> val;
		
#line 3750 "Vrml97Parser.cpp"
		break;
	}
	case HEX_INTEGER:
	{
		i1 = LT(1);
		match(HEX_INTEGER);
#line 1776 "Vrml97Parser.g"
		
		std::istringstream(i1->getText()) >> std::hex >> val;
		
#line 3761 "Vrml97Parser.cpp"
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
#line 1867 "Vrml97Parser.g"
	
	using openvrml_::fequal;
	float x, y, z, angle;
	
#line 3780 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
	angle=floatValue();
#line 1873 "Vrml97Parser.g"
	
	r.x(x);
	r.y(y);
	r.z(z);
	r.angle(angle);
	
	const float axisLength = r.axis().length();
	if (!fequal<float>()(axisLength, 1.0f)) {
	this->reportWarning("The axis component of a rotation must be "
	"a normalized vector.");
	if (fequal<float>()(axisLength, 0.0f)) {
	r.z(1.0);
	} else {
	r.axis(r.axis().normalize());
	}
	}
	
#line 3804 "Vrml97Parser.cpp"
}

double  Vrml97Parser::doubleValue() {
#line 1936 "Vrml97Parser.g"
	double val = 0.0;
#line 3810 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		d0 = LT(1);
		match(REAL);
#line 1938 "Vrml97Parser.g"
		std::istringstream(d0->getText()) >> val;
#line 3821 "Vrml97Parser.cpp"
		break;
	}
	case INTEGER:
	{
		d1 = LT(1);
		match(INTEGER);
#line 1939 "Vrml97Parser.g"
		std::istringstream(d1->getText()) >> val;
#line 3830 "Vrml97Parser.cpp"
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
#line 1960 "Vrml97Parser.g"
	
	float x, y;
	
#line 3848 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
#line 1965 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
#line 3855 "Vrml97Parser.cpp"
}

void Vrml97Parser::vec3fValue(
	vec3f & v
) {
#line 1989 "Vrml97Parser.g"
	
	float x, y, z;
	
#line 3865 "Vrml97Parser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
#line 1994 "Vrml97Parser.g"
	v.x(x);
	v.y(y);
	v.z(z);
#line 3874 "Vrml97Parser.cpp"
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
const unsigned long Vrml97Parser::_tokenSet_13_data_[] = { 160809728UL, 0UL, 0UL, 0UL };
// RBRACE ID "DEF" "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" "ROUTE" 
// "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long Vrml97Parser::_tokenSet_14_data_[] = { 26575616UL, 0UL, 0UL, 0UL };
// RBRACE ID "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long Vrml97Parser::_tokenSet_15_data_[] = { 138428928UL, 0UL, 0UL, 0UL };
// ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_15(_tokenSet_15_data_,4);
const unsigned long Vrml97Parser::_tokenSet_16_data_[] = { 134234656UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_16(_tokenSet_16_data_,4);
const unsigned long Vrml97Parser::_tokenSet_17_data_[] = { 26706752UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID "eventIn" "eventOut" "exposedField" "EXTERNPROTO" 
// "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_17(_tokenSet_17_data_,4);


