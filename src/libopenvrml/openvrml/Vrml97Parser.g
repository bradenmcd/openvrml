// -*- Mode: Antlr; antlr-language: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2000  Braden N. McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

header "post_include_hpp" {
# include <memory>
# include "field.h"
# include "node_ptr.h"
# include "field_value_ptr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace openvrml {
    class scope;
    class script_node;
    class node_type;
}

namespace {
    class Vrml97Scanner : public antlr::TokenStream {
    public:
        static const int EOF_;
        //
        // The following identifiers for constants must match those in the file
        // Vrml97TokenTypes.txt.
        //
        static const int PERIOD;
        static const int LBRACKET;
        static const int RBRACKET;
        static const int LBRACE;
        static const int RBRACE;
        static const int ID;
        static const int INTEGER;
        static const int HEX_INTEGER;
        static const int REAL;
        static const int STRING;
        static const int KEYWORD_DEF;
        static const int KEYWORD_EVENTIN;
        static const int KEYWORD_EVENTOUT;
        static const int KEYWORD_EXPOSEDFIELD;
        static const int KEYWORD_EXTERNPROTO;
        static const int KEYWORD_FALSE;
        static const int KEYWORD_FIELD;
        static const int KEYWORD_IS;
        static const int KEYWORD_NULL;
        static const int KEYWORD_PROTO;
        static const int KEYWORD_ROUTE;
        static const int KEYWORD_TO;
        static const int KEYWORD_TRUE;
        static const int KEYWORD_USE;
        static const int FIELDTYPE_SFBOOL;
        static const int FIELDTYPE_SFCOLOR;
        static const int FIELDTYPE_SFFLOAT;
        static const int FIELDTYPE_SFIMAGE;
        static const int FIELDTYPE_SFINT32;
        static const int FIELDTYPE_SFNODE;
        static const int FIELDTYPE_SFROTATION;
        static const int FIELDTYPE_SFSTRING;
        static const int FIELDTYPE_SFTIME;
        static const int FIELDTYPE_SFVEC2F;
        static const int FIELDTYPE_SFVEC3F;
        static const int FIELDTYPE_MFCOLOR;
        static const int FIELDTYPE_MFFLOAT;
        static const int FIELDTYPE_MFINT32;
        static const int FIELDTYPE_MFNODE;
        static const int FIELDTYPE_MFROTATION;
        static const int FIELDTYPE_MFSTRING;
        static const int FIELDTYPE_MFTIME;
        static const int FIELDTYPE_MFVEC2F;
        static const int FIELDTYPE_MFVEC3F;

        Vrml97Scanner(std::istream &);

        virtual antlr::RefToken nextToken();

    private:
        void getNextChar();
        void identifyKeyword(antlr::Token &);
        void identifyFieldType(antlr::Token &);
        void identifyTerminalSymbol(antlr::Token &);

        std::istream & in_;
        size_t      line_;
        size_t      col_;
        int         c_;
        int         prev_char_;
        int         prev_token_type_;
        bool        read_too_much_;
        bool        expecting_field_type_;
    };
}

namespace openvrml ANTLR_LBRACE
}

header "pre_include_cpp" {
# include "Vrml97Parser.hpp"
ANTLR_RBRACE
}

header "post_include_cpp" {
# include <cctype>
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

}

options {
    language="Cpp";
}

class Vrml97Parser extends Parser;
options {
    k=1;
    importVocab=Vrml97;
}

{
public:
    Vrml97Parser(antlr::TokenStream & lexer, const std::string & uri):
        antlr::LLkParser(lexer, 1),
        uri(uri)
    {}

private:
    const std::string uri;
}

vrmlScene[openvrml::browser & browser,
          std::vector<node_ptr> & nodes]
{
    const scope_ptr scope(new Vrml97RootScope(browser, this->uri));
}
    :   (statement[browser, nodes, scope])*
    ;

statement[openvrml::browser & browser,
          std::vector<node_ptr> & nodes,
          const openvrml::scope_ptr & scope]
    {
        openvrml::node_ptr node;
        openvrml::node_type_ptr nodeType;
    }
    : node=nodeStatement[browser, scope] {
            //
            // If we are unable to parse a node, node will be null.
            //
            if (node) { nodes.push_back(node); }
        }
    | protoStatement[browser, scope]
    | routeStatement[*scope]
    ;

nodeStatement[openvrml::browser & browser,
              const openvrml::scope_ptr & scope]
returns [openvrml::node_ptr n]
options { defaultErrorHandler=false; }
    :   KEYWORD_DEF id0:ID n=node[browser, scope, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
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
        }
    |   n=node[browser, scope, std::string()]
    ;
    exception
    catch [antlr::RecognitionException & ex] {
        reportError(ex);
    }

protoStatement[openvrml::browser & browser,
               const openvrml::scope_ptr & scope]
    //
    // XXX What if the node type already exists in the scope? Probably need to
    // XXX handle an exception here.
    //
    : externproto[browser, scope]
    | proto[browser, scope]
    ;

proto[openvrml::browser & browser,
      const openvrml::scope_ptr & scope]
{
    node_class_ptr nodeClass;
    scope_ptr protoScope;
}
    : KEYWORD_PROTO id:ID {
            nodeClass.reset(new ProtoNodeClass(browser));
            protoScope.reset(new openvrml::scope(id->getText(), scope));
        }
        LBRACKET (
            {
                // XXX Hmm... We give each interface declaration its own scope
                // XXX here. This is wasteful; Vrml97RootScope is expensive and
                // XXX we only *really* need this for SFNode and MFNode
                // XXX fields/exposedFields.
                scope_ptr interfaceDeclScope(new Vrml97RootScope(browser,
                                                                 this->uri));
            }
            protoInterfaceDeclaration[
                interfaceDeclScope,
                static_cast<ProtoNodeClass &>(*nodeClass)]
        )* RBRACKET LBRACE protoBody[protoScope,
                                     static_cast<ProtoNodeClass &>(*nodeClass)]
        RBRACE
        {
            //
            // Add the new node_class (prototype definition) to the browser's
            // node_class_map.
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
            // PROTOs implicitly introduce a new node type as well...
            //
            const node_interface_set & interfaces =
                static_cast<ProtoNodeClass &>(*nodeClass)
                    .protoNodeType.interfaces();
            const node_type_ptr nodeType =
                nodeClass->create_type(id->getText(), interfaces);
            assert(nodeType);
            assert(scope);
            if (!scope->add_type(nodeType)) {
                using antlr::SemanticException;
                throw SemanticException("Node type \"" + nodeType->id
                                        + "\" has already been defined in "
                                        "this scope.",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

protoInterfaceDeclaration[const openvrml::scope_ptr & scope,
                          openvrml::ProtoNodeClass & proto]
{
    using openvrml::node_interface;
    using antlr::SemanticException;

    node_interface::type_id it(node_interface::invalid_type_id);
    openvrml::field_value::type_id ft(field_value::invalid_type_id);
    openvrml::field_value_ptr fv;
}
    : it=eventInterfaceType ft=fieldType id0:ID {
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
        }
    | it=fieldInterfaceType ft=fieldType id1:ID
        fv=fieldValue[proto.browser, scope, ft] {
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
        }
    ;

eventInterfaceType returns [openvrml::node_interface::type_id it = openvrml::node_interface::invalid_type_id]
    :   KEYWORD_EVENTIN  { it = openvrml::node_interface::eventin_id; }
    |   KEYWORD_EVENTOUT { it = openvrml::node_interface::eventout_id; }
    ;

fieldInterfaceType
returns [openvrml::node_interface::type_id it =
            openvrml::node_interface::invalid_type_id]
    :   KEYWORD_FIELD        { it = openvrml::node_interface::field_id; }
    |   KEYWORD_EXPOSEDFIELD { it = openvrml::node_interface::exposedfield_id; }
    ;

protoBody[const openvrml::scope_ptr & scope,
          openvrml::ProtoNodeClass & proto]
{
    openvrml::node_ptr n;
}
    : (protoStatement[proto.browser, scope])*
        n=protoNodeStatement[proto, scope] { assert(n); proto.addRootNode(n); }
        (protoBodyStatement[proto, scope])*
    ;

protoBodyStatement[openvrml::ProtoNodeClass & proto,
                   const openvrml::scope_ptr & scope]
{
    openvrml::node_ptr n;
}
    : n=protoNodeStatement[proto, scope] { assert(n); proto.addRootNode(n); }
    | protoStatement[proto.browser, scope]
    | routeStatement[*scope]
    ;

protoNodeStatement[openvrml::ProtoNodeClass & proto,
                   const openvrml::scope_ptr & scope]
returns [openvrml::node_ptr n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
}
    : KEYWORD_DEF id0:ID n=protoNode[proto, scope, id0->getText()]
    | KEYWORD_USE id1:ID {
            n.reset(scope->find_node(id1->getText()));
            if (!n) {
                throw SemanticException("Node \"" + id1->getText()
                                        + "\" has not been defined in this "
                                        + "scope.",
                                        this->uri,
                                        id1->getLine(),
                                        id1->getColumn());
            }
        }
    | n=protoNode[proto, scope, std::string()]
    ;

externproto[openvrml::browser & browser, const openvrml::scope_ptr & scope]
{
    openvrml::node_interface_set interfaces;
    openvrml::mfstring urlList;
    openvrml::node_type_ptr nodeType;
}
    : KEYWORD_EXTERNPROTO id:ID LBRACKET
        (externInterfaceDeclaration[interfaces])* RBRACKET
        urlList=externprotoUrlList {
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
        }
    ;

externInterfaceDeclaration[openvrml::node_interface_set & interfaces]
        {
            using openvrml::node_interface;
            using openvrml::field_value;
            using antlr::SemanticException;
            node_interface::type_id it(node_interface::invalid_type_id);
            field_value::type_id ft(field_value::invalid_type_id);
        }
    :   it=interfaceType ft=fieldType id:ID {
            const node_interface interface(it, ft, id->getText());
            try {
                interfaces.add(interface);
            } catch (std::invalid_argument & ex) {
            	throw SemanticException(ex.what(),
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

interfaceType
returns [openvrml::node_interface::type_id it = openvrml::node_interface::invalid_type_id]
    : it=eventInterfaceType
    | it=fieldInterfaceType
    ;

externprotoUrlList returns [openvrml::mfstring urlList]
{
    using std::string;
    using openvrml::mfstring;

    string s;
}
    :   s=stringValue { urlList.value.push_back(s); }
    |   LBRACKET ( s=stringValue { urlList.value.push_back(s); } )* RBRACKET
    ;

routeStatement[const openvrml::scope & scope]
    :   KEYWORD_ROUTE fromNodeId:ID PERIOD fromInterfaceId:ID
        KEYWORD_TO toNodeId:ID PERIOD toInterfaceId:ID
        {
            using openvrml::field_value;
            using openvrml::node;
            using openvrml::node_ptr;
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
        }
    ;

node[openvrml::browser & browser,
     const openvrml::scope_ptr & scope,
     const std::string & nodeId]
returns [openvrml::node_ptr n]
options { defaultErrorHandler = false; }
{
    using openvrml::node_type_ptr;
    using openvrml::node_ptr;
    using openvrml::script_node;
    using antlr::SemanticException;

    node_type_ptr nodeType;
}
    : { !LT(1)->getText().compare("Script") }? scriptId:ID {
            n.reset(new script_node(browser.script_node_class_, scope));
            if (!nodeId.empty()) { n->id(nodeId); }

            script_node * const scriptNode = n->to_script();
            assert(scriptNode);
        } LBRACE (
            nodeBodyElement[scope, *n]
            | scriptInterfaceDeclaration[scope, *scriptNode]
        )* RBRACE

    | nodeTypeId:ID {
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
        } LBRACE (nodeBodyElement[scope, *n])* RBRACE
    ;

nodeBodyElement[const openvrml::scope_ptr & scope,
                openvrml::node & node]
{
    using openvrml::field_value;
    using antlr::SemanticException;
    field_value::type_id ft(field_value::invalid_type_id);
    field_value_ptr fv;
}
    : id:ID {
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
        }
        fv=fieldValue[node.type.node_class.browser, scope, ft] {
            assert(fv);
            node.field(id->getText(), *fv);
        }
    |   routeStatement[*scope]
    |   protoStatement[node.type.node_class.browser, scope]
    ;

scriptInterfaceDeclaration[const openvrml::scope_ptr & scope,
                           openvrml::script_node & node]
    {
        using openvrml::node_interface;
        using openvrml::field_value;
        using antlr::SemanticException;
        node_interface::type_id it(node_interface::invalid_type_id);
        field_value::type_id ft(field_value::invalid_type_id);
    }
    : it=eventInterfaceType ft=fieldType id:ID {
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
        }
    | scriptFieldInterfaceDeclaration[scope, node]
    ;

scriptFieldInterfaceDeclaration[const openvrml::scope_ptr & scope,
                                openvrml::script_node & node]
{
    using std::find_if;
    using openvrml::field_value;
    using antlr::SemanticException;

    field_value::type_id ft = field_value::invalid_type_id;
    field_value_ptr fv;
}
    : KEYWORD_FIELD ft=fieldType id:ID
        fv=fieldValue[node.node::type.node_class.browser, scope, ft] {
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
        }
    ;

protoNode[openvrml::ProtoNodeClass & proto,
          const openvrml::scope_ptr & scope,
          const std::string & nodeId]
returns [openvrml::node_ptr n]
options { defaultErrorHandler=false; }
{
    using openvrml::node_type_ptr;
    using openvrml::node_ptr;
    using openvrml::script_node;
    using antlr::SemanticException;

    node_type_ptr nodeType;
}
    : { !LT(1)->getText().compare("Script") }? scriptId:ID {
            n.reset(new script_node(proto.browser.script_node_class_, scope));
            if (!nodeId.empty()) { n->id(nodeId); }

            script_node * const scriptNode = n->to_script();
            assert(scriptNode);
        }
        LBRACE (
            protoNodeBodyElement[proto, scope, *n]
            | protoScriptInterfaceDeclaration[proto, scope, *scriptNode]
        )* RBRACE

    | nodeTypeId:ID {
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
        }
        LBRACE (protoNodeBodyElement[proto, scope, *n])* RBRACE
    ;

protoNodeBodyElement[openvrml::ProtoNodeClass & proto,
                     const openvrml::scope_ptr & scope,
                     openvrml::node & node]
{
    using openvrml::field_value;
    using antlr::SemanticException;

    field_value::type_id ft(field_value::invalid_type_id);
    field_value_ptr fv;
}
    : { node.type.has_field(LT(1)->getText())
        || node.type.has_exposedfield(LT(1)->getText()) }? id:ID {
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
        } (
            (
                fv=protoFieldValue[proto, scope, ft] {
                    assert(fv);
                    node.field(id->getText(), *fv);
                }
            )
            | isStatement[proto, node, id->getText()]
        )
    | eventId:ID isStatement[proto, node, eventId->getText()]
    | routeStatement[*scope]
    | protoStatement[proto.browser, scope]
    ;

isStatement[openvrml::ProtoNodeClass & proto, openvrml::node & node,
            std::string const & nodeInterfaceId]
{
    using antlr::SemanticException;
}
    : KEYWORD_IS id:ID {
            try {
                proto.addIS(node, nodeInterfaceId, id->getText());
            } catch (std::bad_alloc &) {
                throw;
            } catch (std::runtime_error & ex) {
                //
                // ex should be unsupported_interface,
                // node_interface_type_mismatch, or field_value_type_mismatch.
                //
                throw SemanticException(ex.what(),
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

protoScriptInterfaceDeclaration[openvrml::ProtoNodeClass & proto,
                                const openvrml::scope_ptr & scope,
                                openvrml::script_node & node]
{
    using openvrml::node_interface;
    using openvrml::field_value;
    using antlr::SemanticException;
    node_interface::type_id it(node_interface::invalid_type_id);
    field_value::type_id ft(field_value::invalid_type_id);
}
    : it=eventInterfaceType ft=fieldType id:ID {
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
        } (isStatement[proto, node, id->getText()])?
    |   protoScriptFieldInterfaceDeclaration[proto, scope, node]
    ;

protoScriptFieldInterfaceDeclaration[openvrml::ProtoNodeClass & proto,
                                     const openvrml::scope_ptr & scope,
                                     openvrml::script_node & node]
{
    using std::find_if;
    using openvrml::field_value;
    using antlr::SemanticException;

    field_value::type_id ft(field_value::invalid_type_id);
    field_value_ptr fv;
}
    : KEYWORD_FIELD ft=fieldType id:ID {
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
        } (
            (
                fv=protoFieldValue[proto, scope, ft] {
                    assert(fv);
                    node.add_field(id->getText(), fv);
                }
            )
            | (
                KEYWORD_IS protoFieldId:ID {
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
                    } catch (std::bad_alloc &) {
                        throw;
                    } catch (std::runtime_error & ex) {
                        //
                        // ex should be unsupported_interface,
                        // node_interface_type_mismatch, or
                        // field_value_type_mismatch.
                        //
                        throw SemanticException(ex.what(),
                                                this->uri,
                                                protoFieldId->getLine(),
                                                protoFieldId->getColumn());
                    }
                }
            )
        )
    ;

fieldType
returns [openvrml::field_value::type_id ft =
         openvrml::field_value::invalid_type_id]
{
    using openvrml::field_value;
}
    : FIELDTYPE_MFCOLOR     { ft = field_value::mfcolor_id; }
    | FIELDTYPE_MFFLOAT     { ft = field_value::mffloat_id; }
    | FIELDTYPE_MFINT32     { ft = field_value::mfint32_id; }
    | FIELDTYPE_MFNODE      { ft = field_value::mfnode_id; }
    | FIELDTYPE_MFROTATION  { ft = field_value::mfrotation_id; }
    | FIELDTYPE_MFSTRING    { ft = field_value::mfstring_id; }
    | FIELDTYPE_MFTIME      { ft = field_value::mftime_id; }
    | FIELDTYPE_MFVEC2F     { ft = field_value::mfvec2f_id; }
    | FIELDTYPE_MFVEC3F     { ft = field_value::mfvec3f_id; }
    | FIELDTYPE_SFBOOL      { ft = field_value::sfbool_id; }
    | FIELDTYPE_SFCOLOR     { ft = field_value::sfcolor_id; }
    | FIELDTYPE_SFFLOAT     { ft = field_value::sffloat_id; }
    | FIELDTYPE_SFIMAGE     { ft = field_value::sfimage_id; }
    | FIELDTYPE_SFINT32     { ft = field_value::sfint32_id; }
    | FIELDTYPE_SFNODE      { ft = field_value::sfnode_id; }
    | FIELDTYPE_SFROTATION  { ft = field_value::sfrotation_id; }
    | FIELDTYPE_SFSTRING    { ft = field_value::sfstring_id; }
    | FIELDTYPE_SFTIME      { ft = field_value::sftime_id; }
    | FIELDTYPE_SFVEC2F     { ft = field_value::sfvec2f_id; }
    | FIELDTYPE_SFVEC3F     { ft = field_value::sfvec3f_id; }
    ;

fieldValue[openvrml::browser & browser,
           const openvrml::scope_ptr & scope,
           openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    : { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=nodeFieldValue[browser, scope, ft]
    | fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[openvrml::ProtoNodeClass & proto,
                const openvrml::scope_ptr & scope,
                openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv]
{
    using openvrml::field_value;
}
    : { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=protoNodeFieldValue[proto, scope, ft] { assert(fv); }
    | fv=nonNodeFieldValue[ft] { assert(fv); }
    ;

nonNodeFieldValue[openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv = openvrml::field_value_ptr(0)]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    : { ft == field_value::sfbool_id }? fv=sfBoolValue
    | { ft == field_value::sfcolor_id }? fv=sfColorValue
    | { ft == field_value::sffloat_id }? fv=sfFloatValue
    | { ft == field_value::sfimage_id }? fv=sfImageValue
    | { ft == field_value::sfint32_id }? fv=sfInt32Value
    | { ft == field_value::sfrotation_id }? fv=sfRotationValue
    | { ft == field_value::sfstring_id }? fv=sfStringValue
    | { ft == field_value::sftime_id }? fv=sfTimeValue
    | { ft == field_value::sfvec2f_id }? fv=sfVec2fValue
    | { ft == field_value::sfvec3f_id }? fv=sfVec3fValue
    | { ft == field_value::mfcolor_id }? fv=mfColorValue
    | { ft == field_value::mffloat_id }? fv=mfFloatValue
    | { ft == field_value::mfint32_id }? fv=mfInt32Value
    | { ft == field_value::mfrotation_id }? fv=mfRotationValue
    | { ft == field_value::mfstring_id }? fv=mfStringValue
    | { ft == field_value::mftime_id }? fv=mfTimeValue
    | { ft == field_value::mfvec2f_id }? fv=mfVec2fValue
    | fv=mfVec3fValue
    ;

nodeFieldValue[openvrml::browser & browser,
               const openvrml::scope_ptr & scope,
               openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    : { ft == field_value::sfnode_id }? fv=sfNodeValue[browser, scope]
    | fv=mfNodeValue[browser, scope]
    ;

protoNodeFieldValue[openvrml::ProtoNodeClass & proto,
                    const openvrml::scope_ptr & scope,
                    openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv]
options { defaultErrorHandler=false; }
    : { ft == openvrml::field_value::sfnode_id }?
        fv=protoSfNodeValue[proto, scope]
    | fv=protoMfNodeValue[proto, scope]
    ;

sfBoolValue returns [openvrml::field_value_ptr sbv]
options { defaultErrorHandler=false; }
{
    bool val(false);
}
    : val=boolValue { sbv.reset(new sfbool(val)); }
    ;

boolValue returns [bool val = false]
options { defaultErrorHandler=false; }
    : KEYWORD_TRUE { val = true; }
    | KEYWORD_FALSE { val = false; }
    ;

sfColorValue returns [openvrml::field_value_ptr scv]
options { defaultErrorHandler=false; }
{
    color c;
}
    : colorValue[c] { scv.reset(new sfcolor(c)); }
    ;

mfColorValue
returns [openvrml::field_value_ptr mcv =
            openvrml::field_value_ptr(new mfcolor)]
options { defaultErrorHandler=false; }
{
    color c;
    mfcolor & colors = static_cast<mfcolor &>(*mcv);
}
    : colorValue[c] { colors.value.push_back(c); }
    | LBRACKET (colorValue[c] { colors.value.push_back(c); })* RBRACKET
    ;

colorValue[color & c]
options { defaultErrorHandler=false; }
{
    float r, g, b;
}
    : r=colorComponent g=colorComponent b=colorComponent { c.r(r);
                                                           c.g(g);
                                                           c.b(b); }
    ;

//
// Issue a warning if a color component is not in [0, 1].
//
colorComponent returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    : val=floatValue {
            if (val < 0.0 || val > 1.0) {
                this->reportWarning("Color component values must be from 0 to "
                                    "1.");
                if (val < 0.0) {
                    val = 0.0;
                } else if (val > 1.0) {
                    val = 1.0;
                }
            }
        }
    ;

sfFloatValue returns [openvrml::field_value_ptr sfv]
options { defaultErrorHandler=false; }
{
    float f;
}
    : f=floatValue { sfv.reset(new sffloat(f)); }
    ;

mfFloatValue
returns [openvrml::field_value_ptr mfv =
            openvrml::field_value_ptr(new mffloat)]
options { defaultErrorHandler=false; }
{
    float f;
    mffloat & floats = static_cast<mffloat &>(*mfv);
}
    : f=floatValue { floats.value.push_back(f); }
    | LBRACKET (f=floatValue { floats.value.push_back(f); })* RBRACKET
    ;

floatValue returns [float val]
options { defaultErrorHandler=false; }
    : f0:REAL     { std::istringstream(f0->getText()) >> val; }
    | f1:INTEGER  { std::istringstream(f1->getText()) >> val; }
    ;

sfImageValue returns [openvrml::field_value_ptr siv]
options { defaultErrorHandler=false; }
{
    unsigned long w(0L), h(0L), com(0L), pixel(0L);
}
    : w=intValue h=intValue com=intValue
        { std::vector<unsigned char> pixelVector; }
        (
            pixel=intValue
            {
                // need to confirm the cross-platform-ness of this, it
                // looks kind of ugly but might in fact be ok. basically,
                // we read the value as an integer, then strip off the
                // bytes one by one.
                for (int32 i = com - 1; i >= 0; i--) {
                    const unsigned char component =
                        static_cast<unsigned char>(pixel >> (8 * i) & 0xff);
                    pixelVector.push_back(component);
                }
            }
        )*
        {
            // if somebody gives us a really, really, really big
            // pixeltexture, then we will crash. in the age of dos
            // attacks, we have to assume that someone will feed us a
            // too-big texture to see if we barf. good behavior
            // would be to detect outsized w/h and bail. casting away
            // the compiler warning is not helpful. there are other
            // bigger bugs to fry, so I guess it's ok for now.
            //
            if (pixelVector.size() != (w * h * com)) {
                throw antlr::SemanticException("Wrong number of pixel values "
                                               "for SFImage.",
                                               this->uri,
                                               LT(1)->getLine(),
                                               LT(1)->getColumn());
            }
            siv.reset(new sfimage(w, h, com, &pixelVector[0]));
        }
    ;

sfInt32Value returns [openvrml::field_value_ptr siv]
options { defaultErrorHandler=false; }
{
    long i;
}
    : i=intValue { siv.reset(new sfint32(i)); }
    ;

mfInt32Value
returns [openvrml::field_value_ptr miv =
            openvrml::field_value_ptr(new mfint32)]
options { defaultErrorHandler=false; }
{
    long i;
    mfint32 & int32s = static_cast<mfint32 &>(*miv);
}
    : i=intValue { int32s.value.push_back(i); }
    | LBRACKET (i=intValue { int32s.value.push_back(i); })* RBRACKET
    ;

intValue returns [long val]
options { defaultErrorHandler=false; }
    :   i0:INTEGER      { std::istringstream(i0->getText()) >> val; }
    |  	i1:HEX_INTEGER  { std::istringstream(i1->getText()) >> val; }
    ;

sfNodeValue[openvrml::browser & browser,
            const openvrml::scope_ptr & scope]
returns [openvrml::field_value_ptr snv]
{
    openvrml::node_ptr n;
}
    : n=nodeStatement[browser, scope] { snv.reset(new sfnode(n)); }
    | KEYWORD_NULL                    { snv.reset(new sfnode); }
    ;

protoSfNodeValue[openvrml::ProtoNodeClass & proto,
                 const openvrml::scope_ptr & scope]
returns [openvrml::field_value_ptr snv]
{
    openvrml::node_ptr n;
}
    : n=protoNodeStatement[proto, scope] { snv.reset(new sfnode(n)); }
    | KEYWORD_NULL                       { snv.reset(new sfnode); }
    ;

mfNodeValue[openvrml::browser & browser,
            const openvrml::scope_ptr & scope]
returns [openvrml::field_value_ptr mnv = openvrml::field_value_ptr(new mfnode)]
{
    openvrml::node_ptr n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    : n=nodeStatement[browser, scope] { if (n) { nodes.value.push_back(n); } }
    | LBRACKET (
            n=nodeStatement[browser, scope] {
                if (n) { nodes.value.push_back(n); }
            }
        )* RBRACKET
    ;

protoMfNodeValue[openvrml::ProtoNodeClass & proto,
                 const openvrml::scope_ptr & scope]
returns [openvrml::field_value_ptr mnv = openvrml::field_value_ptr(new mfnode)]
{
    openvrml::node_ptr n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    : n=protoNodeStatement[proto, scope] {
            if (n) { nodes.value.push_back(n); }
        }
    | LBRACKET (
            n=protoNodeStatement[proto, scope] {
                if (n) { nodes.value.push_back(n); }
            }
        )* RBRACKET
    ;

sfRotationValue returns [openvrml::field_value_ptr srv]
    { rotation r; }
    : rotationValue[r] { srv.reset(new sfrotation(r)); }
    ;

mfRotationValue
returns [openvrml::field_value_ptr mrv =
         openvrml::field_value_ptr(new mfrotation)]
{
    rotation r;
    mfrotation & rotations = static_cast<mfrotation &>(*mrv);
}
    : rotationValue[r] { rotations.value.push_back(r); }
    | LBRACKET (rotationValue[r] { rotations.value.push_back(r); })* RBRACKET
    ;

//
// Issue a warning here if the vector isn't normalized.
//
rotationValue[rotation & r]
options { defaultErrorHandler=false; }
    {
        using openvrml_::fequal;
        float x, y, z, angle;
    }
    : x=floatValue y=floatValue z=floatValue angle=floatValue {
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
        }
    ;

sfStringValue returns [openvrml::field_value_ptr ssv]
options { defaultErrorHandler=false; }
    { std::string s; }
    : s=stringValue { ssv.reset(new sfstring(s)); }
    ;

mfStringValue
returns [openvrml::field_value_ptr msv =
         openvrml::field_value_ptr(new mfstring)]
options { defaultErrorHandler=false; }
{
    std::string s;
    mfstring & strings = static_cast<mfstring &>(*msv);
}
    : s=stringValue { strings.value.push_back(s); }
    | LBRACKET (s=stringValue { strings.value.push_back(s); })* RBRACKET
    ;

stringValue returns [std::string str]
options { defaultErrorHandler=false; }
    :   s:STRING
        {
            const std::string temp(s->getText());
            str = std::string(temp.begin() + 1, temp.end() - 1);
        }
    ;

sfTimeValue returns [openvrml::field_value_ptr stv]
options { defaultErrorHandler=false; }
    { double t(0.0); }
    : t=doubleValue { stv.reset(new sftime(t)); }
    ;

mfTimeValue
returns [openvrml::field_value_ptr mtv = openvrml::field_value_ptr(new mftime)]
options { defaultErrorHandler=false; }
{
    double t;
    mftime & times = static_cast<mftime &>(*mtv);
}
    : t=doubleValue { times.value.push_back(t); }
    | LBRACKET (t=doubleValue { times.value.push_back(t); })* RBRACKET
    ;

doubleValue returns [double val = 0.0]
options { defaultErrorHandler=false; }
    : d0:REAL    { std::istringstream(d0->getText()) >> val; }
    | d1:INTEGER { std::istringstream(d1->getText()) >> val; }
    ;

sfVec2fValue returns [openvrml::field_value_ptr svv]
options { defaultErrorHandler=false; }
    { vec2f v; }
    : vec2fValue[v] { svv.reset(new sfvec2f(v)); }
    ;

mfVec2fValue
returns [openvrml::field_value_ptr mvv =
         openvrml::field_value_ptr(new mfvec2f)]
options { defaultErrorHandler=false; }
{
    vec2f v;
    mfvec2f & vec2fs = static_cast<mfvec2f &>(*mvv);
}
    : vec2fValue[v] { vec2fs.value.push_back(v); }
    | LBRACKET (vec2fValue[v] { vec2fs.value.push_back(v); })* RBRACKET
    ;

vec2fValue[vec2f & v]
options { defaultErrorHandler=false; }
{
    float x, y;
}
    : x=floatValue y=floatValue { v.x(x);
                                  v.y(y); }
    ;

sfVec3fValue returns [openvrml::field_value_ptr svv]
options { defaultErrorHandler=false; }
{
    vec3f v;
}
    : vec3fValue[v] { svv.reset(new sfvec3f(v)); }
    ;

mfVec3fValue
returns [openvrml::field_value_ptr mvv =
         openvrml::field_value_ptr(new mfvec3f)]
options { defaultErrorHandler=false; }
{
    vec3f v;
    mfvec3f & vec3fs = static_cast<mfvec3f &>(*mvv);
}
    : vec3fValue[v] { vec3fs.value.push_back(v); }
    | LBRACKET (vec3fValue[v] { vec3fs.value.push_back(v); })* RBRACKET
    ;

vec3fValue[vec3f & v]
options { defaultErrorHandler=false; }
{
    float x, y, z;
}
    : x=floatValue y=floatValue z=floatValue { v.x(x);
                                               v.y(y);
                                               v.z(z); }
    ;
