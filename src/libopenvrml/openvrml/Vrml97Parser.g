// -*- Mode: Antlr; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2000, 2001, 2003, 2004  Braden N. McDaniel
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

    //
    // Per-node list of IS mappings. A multimap is used for no other reason
    // than that redundancies are checked later.
    //
    typedef std::multimap<std::string, std::string> is_list;
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

const openvrml::node_interface script_node_interfaces[] = {
    openvrml::node_interface(openvrml::node_interface::exposedfield_id,
                             openvrml::field_value::mfstring_id,
                             "url"),
    openvrml::node_interface(openvrml::node_interface::field_id,
                             openvrml::field_value::sfbool_id,
                             "directOutput"),
    openvrml::node_interface(openvrml::node_interface::field_id,
                             openvrml::field_value::sfbool_id,
                             "mustEvaluate")
};

const openvrml::node_interface_set
    script_node_interface_set(script_node_interfaces,
                              script_node_interfaces + 3);

} // namespace

namespace openvrml ANTLR_LBRACE

} // header "post_include_cpp"

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
options { defaultErrorHandler=false; }
{
    const scope_ptr scope(new Vrml97RootScope(browser, this->uri));
}
    :   (statement[browser, nodes, scope])*
    ;

statement[openvrml::browser & browser,
          std::vector<node_ptr> & nodes,
          const scope_ptr & scope]
options { defaultErrorHandler=false; }
    {
        node_ptr node;
        node_type_ptr nodeType;
    }
    : node=nodeStatement[browser, scope, std::string()] {
            //
            // If we are unable to parse a node, node will be null.
            //
            if (node) { nodes.push_back(node); }
        }
    | protoStatement[browser, scope]
    | routeStatement[*scope]
    ;

nodeStatement[openvrml::browser & browser,
              const scope_ptr & scope,
              const std::string & script_node_id]
returns [node_ptr n]
options { defaultErrorHandler=false; }
    :   KEYWORD_DEF id0:ID n=node[browser, scope, id0->getText()]
    |   KEYWORD_USE id1:ID {
            if (id1->getText() == script_node_id) {
                //
                // Script node self-reference.
                //
                n = node_ptr::self;
            } else {
                using antlr::SemanticException;
                assert(scope);
                n.reset(scope->find_node(id1->getText()));
                if (!n) {
                    throw SemanticException("Node \"" + id1->getText()
                                            + "\" has not been defined in "
                                            + "this scope.",
                                            this->uri,
                                            id1->getLine(),
                                            id1->getColumn());
                }
            }
        }
    |   n=node[browser, scope, std::string()]
    ;

protoStatement[openvrml::browser & browser, const scope_ptr & scope]
options { defaultErrorHandler=false; }
    //
    // XXX What if the node type already exists in the scope? Probably need to
    // XXX handle an exception here.
    //
    : externproto[browser, scope]
    | proto[browser, scope]
    ;

proto[openvrml::browser & browser, const scope_ptr & scope]
options { defaultErrorHandler=false; }
{
    using std::vector;

    node_interface_set interfaces;
    proto_node_class::default_value_map_t default_value_map;
    vector<node_ptr> impl_nodes;
    proto_node_class::is_map_t is_map;
    proto_node_class::routes_t routes;
}
    :   KEYWORD_PROTO id:ID {
            scope_ptr proto_scope(new openvrml::scope(id->getText(), scope));
        } LBRACKET (protoInterfaceDeclaration[browser,
                                              scope,
                                              id->getText(),
                                              interfaces,
                                              default_value_map])* RBRACKET
        LBRACE protoBody[browser,
                         proto_scope,
                         interfaces,
                         impl_nodes,
                         is_map,
                         routes] RBRACE {
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
        }
    ;

protoInterfaceDeclaration[
    openvrml::browser & browser,
    const scope_ptr & outer_scope,
    const std::string & proto_id,
    node_interface_set & interfaces,
    proto_node_class::default_value_map_t & default_value_map]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;

    node_interface::type_id it;
    field_value::type_id ft;
    field_value_ptr fv;
}
    :   it=eventInterfaceType ft=fieldType id0:ID {
            const node_interface interface(it, ft, id0->getText());
            const bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
                throw SemanticException("Interface \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->uri,
                                        id0->getLine(),
                                        id0->getColumn());
            }
        }
    |   it=fieldInterfaceType ft=fieldType id1:ID
        {
            //
            // The field value declaration should have access to the node
            // types in the outer scope.
            //
            const scope_ptr field_decl_scope(
                new scope(proto_id + '.' + id1->getText(), outer_scope));
        }
        fv=fieldValue[browser, field_decl_scope, ft, std::string()] {
            assert(fv);

            const node_interface interface(it, ft, id1->getText());
            bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
                throw SemanticException("Interface \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->uri,
                                        id1->getLine(),
                                        id1->getColumn());
            }
            succeeded =
                default_value_map.insert(make_pair(id1->getText(), fv)).second;
            assert(succeeded);
        }
    ;

eventInterfaceType returns [node_interface::type_id it]
options { defaultErrorHandler=false; }
    :   KEYWORD_EVENTIN  { it = node_interface::eventin_id; }
    |   KEYWORD_EVENTOUT { it = node_interface::eventout_id; }
    ;

fieldInterfaceType returns [node_interface::type_id it]
options { defaultErrorHandler=false; }
    :   KEYWORD_FIELD        { it = node_interface::field_id; }
    |   KEYWORD_EXPOSEDFIELD { it = node_interface::exposedfield_id; }
    ;

protoBody[openvrml::browser & browser,
          const scope_ptr & scope,
          const node_interface_set & interfaces,
          std::vector<node_ptr> & impl_nodes,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes]
options { defaultErrorHandler=false; }
{
    assert(scope);
    assert(impl_nodes.empty());
    assert(is_map.empty());
    assert(routes.empty());

    node_ptr n;
}
    :   (protoStatement[browser, scope])*
            n=protoNodeStatement[browser,
                                 scope,
                                 interfaces,
                                 is_map,
                                 routes,
                                 std::string()]
        {
            assert(n);
            impl_nodes.push_back(n);
        }
        (protoBodyStatement[browser,
                            scope,
                            interfaces,
                            impl_nodes,
                            is_map,
                            routes])*
    ;

protoBodyStatement[openvrml::browser & browser,
                   const scope_ptr & scope,
                   const node_interface_set & interfaces,
                   std::vector<node_ptr> & impl_nodes,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes]
options { defaultErrorHandler=false; }
{
    assert(scope);
    assert(!impl_nodes.empty());

    node_ptr n;
}
    :   n=protoNodeStatement[browser,
                             scope,
                             interfaces,
                             is_map,
                             routes,
                             std::string()] {
            assert(n);
            impl_nodes.push_back(n);
        }
    | protoStatement[browser, scope]
    | protoRouteStatement[*scope, routes]
    ;

protoNodeStatement[openvrml::browser & browser,
                   const openvrml::scope_ptr & scope,
                   const node_interface_set & proto_interfaces,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes,
                   const std::string & script_node_id]
returns [node_ptr n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
}
    :   KEYWORD_DEF id0:ID n=protoNode[browser,
                                       scope,
                                       proto_interfaces,
                                       is_map,
                                       routes,
                                       id0->getText()]
    |   KEYWORD_USE id1:ID {
            if (id1->getText() == script_node_id) {
                //
                // Script node self-reference.
                //
                n = node_ptr::self;
            } else {
                n.reset(scope->find_node(id1->getText()));
                if (!n) {
                    throw SemanticException("Node \"" + id1->getText()
                                            + "\" has not been defined in "
                                            + "this scope.",
                                            this->uri,
                                            id1->getLine(),
                                            id1->getColumn());
                }
            }
        }
    |   n=protoNode[browser,
                    scope,
                    proto_interfaces,
                    is_map,
                    routes,
                    std::string()]
    ;

externproto[openvrml::browser & browser, const openvrml::scope_ptr & scope]
options { defaultErrorHandler=false; }
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
options { defaultErrorHandler=false; }
{
    using openvrml::node_interface;
    using openvrml::field_value;
    using antlr::SemanticException;
    node_interface::type_id it(node_interface::invalid_type_id);
    field_value::type_id ft(field_value::invalid_type_id);
}
    :   it=interfaceType ft=fieldType id:ID {
            const node_interface interface(it, ft, id->getText());
            bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
            	throw SemanticException("Interface \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

interfaceType returns [node_interface::type_id interface_type]
options { defaultErrorHandler=false; }
    : interface_type=eventInterfaceType
    | interface_type=fieldInterfaceType
    ;

externprotoUrlList returns [mfstring urlList]
options { defaultErrorHandler=false; }
{
    using std::string;
    using openvrml::mfstring;

    string s;
}
    :   s=stringValue { urlList.value.push_back(s); }
    |   LBRACKET ( s=stringValue { urlList.value.push_back(s); } )* RBRACKET
    ;

routeStatement[const openvrml::scope & scope]
options { defaultErrorHandler=false; }
    :   KEYWORD_ROUTE from_node_id:ID PERIOD eventout_id:ID
        KEYWORD_TO to_node_id:ID PERIOD eventin_id:ID
        {
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
                //
                // Redundant routing is ignored (4.10.2), so we don't care
                // if add_route returns false.
                //
                add_route(*from_node, eventout_id->getText(),
                          *to_node, eventin_id->getText());
            } catch (runtime_error & ex) {
                throw SemanticException(ex.what(),
                                        this->uri,
                                        from_node_id->getLine(),
                                        from_node_id->getColumn());
            }
        }
    ;

protoRouteStatement[const openvrml::scope & scope,
                    proto_node_class::routes_t & routes]
options { defaultErrorHandler=false; }
    :   KEYWORD_ROUTE from_node_id:ID PERIOD eventout_id:ID
        KEYWORD_TO to_node_id:ID PERIOD eventin_id:ID
        {
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
            const node_interface_set & from_node_interfaces =
                from_node->type.interfaces();
            const node_interface_set::const_iterator from_interface =
                find_if(from_node_interfaces.begin(),
                        from_node_interfaces.end(),
                        bind2nd(node_interface_matches_eventout(),
                                eventout_id->getText()));
            if (from_interface == from_node_interfaces.end()) {
                throw SemanticException(from_node->type.id + " node has no "
                                        "eventOut \"" + eventout_id->getText()
                                        + "\".",
                                        this->uri,
                                        eventout_id->getLine(),
                                        eventout_id->getColumn());
            }

            const node_interface_set & to_node_interfaces =
                to_node->type.interfaces();
            const node_interface_set::const_iterator to_interface =
                find_if(to_node_interfaces.begin(), to_node_interfaces.end(),
                        bind2nd(node_interface_matches_eventin(),
                                eventin_id->getText()));
            if (to_interface == to_node_interfaces.end()) {
                throw SemanticException(to_node->type.id + " node has no "
                                        "eventIn \"" + eventin_id->getText()
                                        + "\".",
                                        this->uri,
                                        eventin_id->getLine(),
                                        eventin_id->getColumn());
            }

            if (to_interface->field_type != from_interface->field_type) {
                throw SemanticException("Mismatch between eventOut and "
                                        "eventIn types.",
                                        this->uri,
                                        eventin_id->getLine(),
                                        eventin_id->getColumn());
            }

            const proto_node_class::route r(*from_node, eventout_id->getText(),
                                            *to_node, eventin_id->getText());
            routes.push_back(r);
        }
    ;

node[openvrml::browser & browser,
     const scope_ptr & scope,
     const std::string & node_id]
returns [node_ptr n]
options { defaultErrorHandler = false; }
{
    using antlr::SemanticException;

    initial_value_map initial_values;
    node_interface_set interfaces;
    node_type_ptr nodeType;
}
    : { !LT(1)->getText().compare("Script") }? scriptId:ID LBRACE (
            nodeBodyElement[browser,
                            scope,
                            script_node_interface_set,
                            initial_values]
            | scriptInterfaceDeclaration[browser,
                                         scope,
                                         interfaces,
                                         initial_values,
                                         node_id]
        )* RBRACE {
            n.reset(new script_node(browser.script_node_class_,
                                    scope,
                                    interfaces,
                                    initial_values));
            if (!node_id.empty()) { n->id(node_id); }
        }
    | nodeTypeId:ID {
            nodeType = scope->find_type(nodeTypeId->getText());
            if (!nodeType) {
                throw SemanticException("Unknown node type \""
                                        + nodeTypeId->getText() + "\".",
                                        this->uri,
                                        nodeTypeId->getLine(),
                                        nodeTypeId->getColumn());
            }

        } LBRACE (nodeBodyElement[browser,
                                  scope,
                                  nodeType->interfaces(),
                                  initial_values])*
        RBRACE {
            n = node_ptr(nodeType->create_node(scope, initial_values));

            if (!node_id.empty()) { n->id(node_id); }
        }
    ;
    exception
    catch [std::invalid_argument & ex] {
        throw SemanticException(ex.what(),
                                this->uri,
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }
    catch [unsupported_interface & ex] {
        throw SemanticException(ex.what(),
                                this->uri,
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }
    catch [std::bad_cast & ex] {
        throw SemanticException("Incorrect value type for field or "
                                "exposedField.",
                                this->uri,
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }

nodeBodyElement[browser & b,
                const scope_ptr & scope,
                const node_interface_set & interfaces,
                initial_value_map & initial_values]
options { defaultErrorHandler=false; }
{
    using std::find_if;
    using std::bind2nd;
    using antlr::SemanticException;
    field_value::type_id ft = field_value::invalid_type_id;
    field_value_ptr fv;
}
    :   id:ID {
            node_interface_set::const_iterator interface =
                find_if(interfaces.begin(), interfaces.end(),
                        bind2nd(node_interface_matches_field(),
                                id->getText()));
            if (interface == interfaces.end()) {
                interface =
                    find_if(interfaces.begin(), interfaces.end(),
                            bind2nd(node_interface_matches_exposedfield(),
                                    id->getText()));
            }
            if (interface == interfaces.end()) {
                throw SemanticException("Node has no field or exposedField \""
                                        + id->getText() + "\"",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
            ft = interface->field_type;
        } fv=fieldValue[b, scope, ft, std::string()] {
            assert(fv);
            const bool succeeded =
                initial_values.insert(make_pair(id->getText(), fv)).second;
            if (!succeeded) {
                throw SemanticException("Value for " + id->getText()
                                        + " already declared.",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    |   routeStatement[*scope]
    |   protoStatement[b, scope]
    ;

scriptInterfaceDeclaration[browser & b,
                           const scope_ptr & scope,
                           node_interface_set & interfaces,
                           initial_value_map & initial_values,
                           const std::string & node_id]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
    node_interface::type_id it(node_interface::invalid_type_id);
    field_value::type_id ft(field_value::invalid_type_id);
}
    : it=eventInterfaceType ft=fieldType id:ID {
            const node_interface interface(it, ft, id->getText());
            const bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
                throw SemanticException("Interface \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    | scriptFieldInterfaceDeclaration[b,
                                      scope,
                                      interfaces,
                                      initial_values,
                                      node_id]
    ;

scriptFieldInterfaceDeclaration[browser & b,
                                const scope_ptr & scope,
                                node_interface_set & interfaces,
                                initial_value_map & initial_values,
                                const std::string & script_node_id]
options { defaultErrorHandler=false; }
{
    using std::find_if;
    using antlr::SemanticException;

    field_value::type_id ft = field_value::invalid_type_id;
    field_value_ptr fv;
}
    :   KEYWORD_FIELD ft=fieldType id:ID fv=fieldValue[b,
                                                       scope,
                                                       ft,
                                                       script_node_id] {
            assert(fv);
            bool succeeded =
                interfaces.insert(node_interface(node_interface::field_id,
                                                 ft,
                                                 id->getText()))
                    .second;
            if (!succeeded) {
                throw SemanticException("Interface \"" + id->getText()
                                        + "\" already declared for Script "
                                        "node.",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
            succeeded = initial_values.insert(make_pair(id->getText(), fv))
                        .second;
            assert(succeeded);
        }
    ;

protoNode[openvrml::browser & browser,
          const scope_ptr & scope,
          const node_interface_set & proto_interfaces,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes,
          const std::string & node_id]
returns [node_ptr n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;

    initial_value_map initial_values;
    node_interface_set interfaces;
    is_list is_mappings;
    node_type_ptr nodeType;
}
    : (
            { !LT(1)->getText().compare("Script") }? scriptId:ID LBRACE (
                protoNodeBodyElement[browser,
                                     scope,
                                     proto_interfaces,
                                     is_map,
                                     routes,
                                     script_node_interface_set,
                                     initial_values,
                                     is_mappings]
                | protoScriptInterfaceDeclaration[browser,
                                                  scope,
                                                  proto_interfaces,
                                                  is_map,
                                                  routes,
                                                  node_id,
                                                  interfaces,
                                                  initial_values,
                                                  is_mappings]
            )* RBRACE {
                n.reset(new script_node(browser.script_node_class_,
                                        scope,
                                        interfaces,
                                        initial_values));
                if (!node_id.empty()) { n->id(node_id); }
            }

        | nodeTypeId:ID {
                nodeType = scope->find_type(nodeTypeId->getText());
                if (!nodeType) {
                    throw SemanticException("Unknown node type \""
                                            + nodeTypeId->getText() + "\".",
                                            this->uri,
                                            nodeTypeId->getLine(),
                                            nodeTypeId->getColumn());
                }
            }
            LBRACE (protoNodeBodyElement[browser,
                                         scope,
                                         proto_interfaces,
                                         is_map,
                                         routes,
                                         nodeType->interfaces(),
                                         initial_values,
                                         is_mappings])* RBRACE {
                n = nodeType->create_node(scope, initial_values);
                if (!node_id.empty()) { n->id(node_id); }
            }
        ) {
            for (is_list::const_iterator is_mapping = is_mappings.begin();
                 is_mapping != is_mappings.end();
                 ++is_mapping) {
                typedef proto_node_class::is_target is_target;
                is_map.insert(make_pair(is_mapping->second,
                                        is_target(*n, is_mapping->first)));
            }
        }
    ;

protoNodeBodyElement[openvrml::browser & browser,
                     const scope_ptr & scope,
                     const node_interface_set & proto_interfaces,
                     proto_node_class::is_map_t & is_map,
                     proto_node_class::routes_t & routes,
                     const node_interface_set & node_interfaces,
                     initial_value_map & initial_values,
                     is_list & is_mappings]
options { defaultErrorHandler=false; }
{
    using std::string;
    using antlr::SemanticException;

    field_value_ptr fv;
}
    :   interface_id:ID {
            const node_interface_set::const_iterator impl_node_interface =
                find_interface(node_interfaces, interface_id->getText());
            if (impl_node_interface == node_interfaces.end()) {
                throw SemanticException("Node has no interface \""
                                        + interface_id->getText() + "\".",
                                        this->uri,
                                        interface_id->getLine(),
                                        interface_id->getColumn());
            }
        } ( {impl_node_interface->type == node_interface::field_id
            || impl_node_interface->type == node_interface::exposedfield_id}?
            (
                fv=protoFieldValue[browser,
                                   scope,
                                   proto_interfaces,
                                   is_map,
                                   routes,
                                   impl_node_interface->field_type,
                                   std::string()] {
                    assert(fv);
                    bool succeeded =
                        initial_values.insert(
                            make_pair(interface_id->getText(), fv)).second;
                    assert(succeeded);
                }
            |   isStatement[impl_node_interface->id, is_mappings]
            )
        | isStatement[impl_node_interface->id, is_mappings]
        )
    |   protoRouteStatement[*scope, routes]
    |   protoStatement[browser, scope]
    ;

isStatement[const std::string & impl_node_interface_id,
            is_list & is_mappings]
options { defaultErrorHandler=false; }
    :   KEYWORD_IS id:ID {
            is_mappings.insert(make_pair(impl_node_interface_id,
                                         id->getText()));
        }
    ;

protoScriptInterfaceDeclaration[openvrml::browser & browser,
                                const openvrml::scope_ptr & scope,
                                const node_interface_set & proto_interfaces,
                                proto_node_class::is_map_t & is_map,
                                proto_node_class::routes_t & routes,
                                const std::string & script_node_id,
                                node_interface_set & interfaces,
                                initial_value_map & initial_values,
                                is_list & is_mappings]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
    node_interface::type_id it;
    field_value::type_id ft;
}
    :   it=eventInterfaceType ft=fieldType id:ID {
            const node_interface interface(it, ft, id->getText());
            const bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
                throw SemanticException("Interface  \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        } (isStatement[id->getText(), is_mappings])?
    |   protoScriptFieldInterfaceDeclaration[browser,
                                             scope,
                                             proto_interfaces,
                                             is_map,
                                             routes,
                                             interfaces,
                                             initial_values,
                                             script_node_id,
                                             is_mappings]
    ;

protoScriptFieldInterfaceDeclaration[
    openvrml::browser & browser,
    const scope_ptr & scope,
    const node_interface_set & proto_interfaces,
    proto_node_class::is_map_t & is_map,
    proto_node_class::routes_t & routes,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id,
    is_list & is_mappings]
options { defaultErrorHandler=false; }
{
    field_value::type_id ft;
    field_value_ptr fv;
    bool succeeded;
}
    : KEYWORD_FIELD ft=fieldType id:ID {
            succeeded =
                interfaces.insert(node_interface(node_interface::field_id,
                                                 ft,
                                                 id->getText()))
                    .second;
            if (!succeeded) {
                using antlr::SemanticException;
                throw SemanticException("Interface \"" + id->getText()
                                        + "\" already declared for Script "
                                        "node.",
                                        this->uri,
                                        id->getLine(),
                                        id->getColumn());
            }
        } (
            fv=protoFieldValue[browser,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft,
                               script_node_id] {
                assert(fv);
                succeeded = initial_values.insert(make_pair(id->getText(), fv))
                    .second;
                assert(succeeded);
            }
            | isStatement[id->getText(), is_mappings] {
                using std::auto_ptr;
                using boost::shared_ptr;

                //
                // The field needs some default value as a placeholder. This
                // is never actually used in a PROTO instance.
                //
                auto_ptr<field_value> value = field_value::create(ft);
                succeeded =
                    initial_values.insert(
                        make_pair(id->getText(),
                                  shared_ptr<field_value>(value)))
                    .second;
                assert(succeeded);
            }
        )
    ;

fieldType
returns [openvrml::field_value::type_id ft =
         openvrml::field_value::invalid_type_id]
options { defaultErrorHandler=false; }
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
           const openvrml::field_value::type_id ft,
           const std::string & node_id]
returns [openvrml::field_value_ptr fv]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    : { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=nodeFieldValue[browser, scope, ft, node_id]
    | fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[openvrml::browser & browser,
                const scope_ptr & scope,
                const node_interface_set & proto_interfaces,
                proto_node_class::is_map_t & is_map,
                proto_node_class::routes_t & routes,
                const field_value::type_id ft,
                const std::string & script_node_id]
returns [field_value_ptr fv]
options { defaultErrorHandler=false; }
    : { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=protoNodeFieldValue[browser,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft,
                               script_node_id] {
            assert(fv);
        }
    | fv=nonNodeFieldValue[ft] { assert(fv); }
    ;

nonNodeFieldValue[openvrml::field_value::type_id ft]
returns [openvrml::field_value_ptr fv = openvrml::field_value_ptr()]
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
               openvrml::field_value::type_id ft,
               const std::string & script_node_id]
returns [openvrml::field_value_ptr fv]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    :   { ft == field_value::sfnode_id }?
            fv=sfNodeValue[browser, scope, script_node_id]
    |   fv=mfNodeValue[browser, scope, script_node_id]
    ;

protoNodeFieldValue[openvrml::browser & browser,
                    const scope_ptr & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_class::is_map_t & is_map,
                    proto_node_class::routes_t & routes,
                    field_value::type_id ft,
                    const std::string & script_node_id]
returns [field_value_ptr fv]
options { defaultErrorHandler=false; }
    :   { ft == field_value::sfnode_id }?
            fv=protoSfNodeValue[browser,
                                scope,
                                proto_interfaces,
                                is_map, routes,
                                script_node_id]
    |   fv=protoMfNodeValue[browser,
                            scope,
                            proto_interfaces,
                            is_map,
                            routes,
                            script_node_id]
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
    using antlr::SemanticException;
    size_t x, y, comp;
    int32 pixel;
    size_t pixel_index = 0;
}
    :   x=intValue y=intValue comp=intValue {
            image img(x, y, comp);
        } (
            pixel=intValue {
                if (!(pixel_index < x * y)) {
                    throw SemanticException("Too many pixel values for "
                                            "SFImage.",
                                            this->uri,
                                            LT(1)->getLine(),
                                            LT(1)->getColumn());
                }
                img.pixel(pixel_index++, pixel);
            }
        )* {
            if (pixel_index != x * y) {
                throw antlr::SemanticException("Insufficient pixel values for "
                                               "SFImage.",
                                               this->uri,
                                               LT(1)->getLine(),
                                               LT(1)->getColumn());
            }
            siv.reset(new sfimage(img));
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

intValue returns [int32 val]
options { defaultErrorHandler=false; }
    :   i0:INTEGER {
            std::istringstream(i0->getText()) >> val;
        }
    |  	i1:HEX_INTEGER  {
            std::istringstream(i1->getText()) >> std::hex >> val;
        }
    ;

sfNodeValue[openvrml::browser & browser,
            const openvrml::scope_ptr & scope,
            const std::string & script_node_id]
returns [openvrml::field_value_ptr snv]
options { defaultErrorHandler=false; }
{
    openvrml::node_ptr n;
}
    :   n=nodeStatement[browser, scope, script_node_id] {
            snv.reset(new sfnode(n));
        }
    |   KEYWORD_NULL { snv.reset(new sfnode); }
    ;

protoSfNodeValue[openvrml::browser & browser,
                 const scope_ptr & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id]
returns [field_value_ptr snv]
options { defaultErrorHandler=false; }
{
    node_ptr n;
}
    :   n=protoNodeStatement[browser,
                             scope,
                             proto_interfaces,
                             is_map,
                             routes,
                             script_node_id] {
            snv.reset(new sfnode(n));
        }
    |   KEYWORD_NULL {
            snv.reset(new sfnode);
        }
    ;

mfNodeValue[openvrml::browser & browser,
            const openvrml::scope_ptr & scope,
            const std::string & script_node_id]
returns [openvrml::field_value_ptr mnv = openvrml::field_value_ptr(new mfnode)]
options { defaultErrorHandler=false; }
{
    openvrml::node_ptr n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    :   n=nodeStatement[browser, scope, script_node_id] {
            if (n) { nodes.value.push_back(n); }
        }
    |   LBRACKET (
            n=nodeStatement[browser, scope, script_node_id] {
                if (n) { nodes.value.push_back(n); }
            }
        )* RBRACKET
    ;

protoMfNodeValue[openvrml::browser & browser,
                 const scope_ptr & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id]
returns [field_value_ptr mnv = field_value_ptr(new mfnode)]
options { defaultErrorHandler=false; }
{
    node_ptr n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    :   n=protoNodeStatement[browser,
                             scope,
                             proto_interfaces,
                             is_map,
                             routes,
                             script_node_id] {
            if (n) { nodes.value.push_back(n); }
        }
    |   LBRACKET (
            n=protoNodeStatement[browser,
                                 scope,
                                 proto_interfaces,
                                 is_map,
                                 routes,
                                 script_node_id] {
                if (n) { nodes.value.push_back(n); }
            }
        )* RBRACKET
    ;

sfRotationValue returns [openvrml::field_value_ptr srv]
options { defaultErrorHandler=false; }
    { rotation r; }
    : rotationValue[r] { srv.reset(new sfrotation(r)); }
    ;

mfRotationValue
returns [field_value_ptr mrv = field_value_ptr(new mfrotation)]
options { defaultErrorHandler=false; }
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
            using std::string;
            const string & token_text(s->getText());
            //
            // "Unescape" the string.
            //
            bool escape = false;
            for (string::const_iterator pos = token_text.begin() + 1;
                 pos != token_text.end() - 1;
                 ++pos) {
                if (*pos == '\\' && !escape) {
                    escape = true;
                } else {
                    str.push_back(*pos);
                    escape = false;
                }
            }
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
