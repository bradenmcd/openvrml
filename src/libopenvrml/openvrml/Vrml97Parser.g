// -*- Mode: Antlr; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2000, 2001, 2003, 2004, 2005, 2006  Braden N. McDaniel
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
# include <boost/shared_ptr.hpp>
# include "field_value.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace openvrml {
    class scope;
    class script_node;
    class node_type;
}

namespace {

    class OPENVRML_LOCAL Vrml97Scanner : public antlr::TokenStream {
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

    protected:
        void expectFieldType();

        virtual bool isValidIdFirstChar(char);
        virtual bool isValidIdRestChars(char);
        virtual bool isWhitespaceChar(char);
        virtual bool isNewlineChar(char);

        virtual bool identifyKeyword(antlr::Token &);
        virtual bool identifyFieldType(antlr::Token &);
        virtual bool identifyTerminalSymbol(antlr::Token &);

    private:
        void getNextChar();

        std::istream & in_;
        size_t      line_;
        int         col_;
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
# include <antlr/CommonToken.hpp>
# include <boost/lexical_cast.hpp>
# include <private.h>
# include "scope.h"
# include "script.h"

namespace {

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
    col_(-1),
    c_(' '),
    prev_char_('\0'),
    prev_token_type_(0),
    read_too_much_(false),
    expecting_field_type_(false)
{}

antlr::RefToken Vrml97Scanner::nextToken()
{
    using std::ostringstream;
    using antlr::RefToken;
    using antlr::CommonToken;

    typedef std::istream::traits_type traits_type;

    RefToken token(new CommonToken);
    ostringstream token_string;
    token_string.unsetf(ostringstream::skipws);

    if (this->read_too_much_) {
        this->read_too_much_ = false;
    } else {
        this->getNextChar();
    }

    while (this->isWhitespaceChar(this->c_) || this->c_ == '#') {
        if (this->c_ == '#') {
            while (!(this->isNewlineChar(this->c_) || this->c_ == EOF)) {
                this->getNextChar();
            }
        } else {
            getNextChar();
        }
    }

    token->setLine(int(this->line_));
    token->setColumn(int(this->col_));

    if (this->c_ == EOF) {
        token->setType(EOF_);
    } else if (this->isValidIdFirstChar(this->c_)) {
        //
        // in an identifier or a keyword
        //

        while (this->isValidIdRestChars(this->c_)) {
            token_string.put(traits_type::to_char_type(this->c_));
            getNextChar();
        }

        read_too_much_ = true;

        token->setText(token_string.str());

        if (expecting_field_type_ && this->identifyFieldType(*token)) {
            this->expecting_field_type_ = false;
        } else if (!this->identifyKeyword(*token)) {
            token->setType(ID);
        }
    } else if (this->c_ == '.'
            || this->c_ == '+'
            || this->c_ == '-'
            || isdigit(this->c_)) {
        //
        // probably in a number
        //

        if (this->c_ == '+' || this->c_ == '-') {
            token_string.put(traits_type::to_char_type(this->c_));
            this->getNextChar();
            read_too_much_ = true;
        }

        if (isdigit(this->c_)) {
            //
            // definitely in a number
            //
            token->setType(INTEGER);

            token_string.put(traits_type::to_char_type(this->c_));

            getNextChar();

            if (this->prev_char_ == '0'
                    && (this->c_ == 'x' || this->c_ == 'X')) {
                //
                // in an integer expressed in hexadecimal
                //
                token->setType(HEX_INTEGER);

                token_string.put(traits_type::to_char_type(this->c_));
                getNextChar();
                while (isxdigit(this->c_)) {
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();
                }
            } else {
                while (isdigit(this->c_)) {
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();
                }
            }

            if (this->c_ == '.') {
                //
                // in a floating-point number
                //
                token->setType(REAL);

                token_string.put(traits_type::to_char_type(this->c_));
                getNextChar();

                while (isdigit(this->c_)) {
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();
                }

                if (this->c_ == 'E' || this->c_ == 'e') {
                    //
                    // in an exponent
                    //
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();

                    if ((c_ == '+') || (c_ == '-') || isdigit(this->c_)) {
                        //
                        // exponent may be signed
                        //
                        token_string.put(traits_type::to_char_type(this->c_));
                        getNextChar();

                        while (isdigit(this->c_)) {
                            token_string.put(traits_type::to_char_type(this->c_));
                            getNextChar();
                        }
                    }
                }
            } else if (this->c_ == 'E' || this->c_ == 'e') {
                //
                // in an exponent
                //
                token->setType(REAL);

                token_string.put(traits_type::to_char_type(this->c_));
                getNextChar();

                if (this->c_ == '+' || this->c_ == '-' || isdigit(this->c_)) {
                    //
                    // exponent may be signed
                    //
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();

                    while (isdigit(this->c_)) {
                        token_string.put(traits_type::to_char_type(this->c_));
                        getNextChar();
                    }
                }
            }

            this->read_too_much_ = true;

        } else if (this->c_ == '.') {
            //
            // in a floating-point number or a lone full-stop (as in a ROUTE)
            //

            token_string.put(traits_type::to_char_type(this->c_));
            getNextChar();

            if (isdigit(this->c_)) {

                token->setType(REAL);

                while (isdigit(this->c_)) {
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();
                }

                if ((this->c_ == 'E') || (this->c_ == 'e')) {
                    //
                    // in an exponent
                    //
                    token_string.put(traits_type::to_char_type(this->c_));
                    getNextChar();

                    if ((c_ == '+') || (c_ == '-') || isdigit(this->c_)) {
                        //
                        // exponent may be signed
                        //
                        token_string.put(traits_type::to_char_type(this->c_));
                        getNextChar();

                        while (isdigit(this->c_)) {
                            token_string.put(traits_type::to_char_type(this->c_));
                            getNextChar();
                        }
                    }
                }
            } else {
                token->setType(PERIOD);
            }

            this->read_too_much_ = true;
        }

        token->setText(token_string.str());

    } else if (this->c_ == '"') {
        //
        // in a string
        //
        token->setType(STRING);

        token_string.put(traits_type::to_char_type(this->c_));
        this->getNextChar();

        char prev_char('\0');
        while (this->c_ != '"' || prev_char == '\\') {
            token_string.put(traits_type::to_char_type(this->c_));
            prev_char = this->c_;
            this->getNextChar();
        }

        //
        // add the closing quote
        //
        token_string.put(traits_type::to_char_type(this->c_));

        token->setText(token_string.str());

    } else {
        //
        // terminal symbol or unidentified character
        //
        token_string.put(traits_type::to_char_type(this->c_));

        token->setText(token_string.str());

        this->identifyTerminalSymbol(*token);
    }

    this->prev_token_type_ = token->getType();

    return token;
}

inline void Vrml97Scanner::getNextChar()
{
    this->prev_char_ = this->c_;
    this->c_ = this->in_.get();
    ++this->col_; // Increment the column count;

    //
    // Increment the line count (and reset the column count to -1) if the
    // current character is a newline character EXCEPT if the current character
    // is a linefeed AND the previous character is a carriage return.
    //
    if (this->isNewlineChar(this->c_)) {
        if (!((this->c_ == 0x0a) && (this->prev_char_ == 0x0d))) {
            ++this->line_;
            this->col_ = -1;
        }
    }
}

inline void Vrml97Scanner::expectFieldType()
{
    this->expecting_field_type_ = true;
}

inline bool Vrml97Scanner::isValidIdFirstChar(const char c)
{
    if (((c >= 0x30) && (c <= 0x39))
            || (c == 0x2b)
            || (c == 0x2d)
            || !this->isValidIdRestChars(c)) {
        return false;
    }
    return true;
}

inline bool Vrml97Scanner::isValidIdRestChars(const char c)
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

inline bool Vrml97Scanner::isWhitespaceChar(const char c)
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

inline bool Vrml97Scanner::isNewlineChar(const char c)
{
    return ((c == 0x0a) || (c == 0x0d));
}

inline bool Vrml97Scanner::identifyKeyword(antlr::Token & token)
{
    std::string const token_text(token.getText());
    if      (token_text == "DEF")          { token.setType(KEYWORD_DEF); }
    else if (token_text == "eventIn")      { this->expectFieldType();
                                             token.setType(KEYWORD_EVENTIN); }
    else if (token_text == "eventOut")     { this->expectFieldType();
                                             token.setType(KEYWORD_EVENTOUT); }
    else if (token_text == "exposedField") { this->expectFieldType();
                                             token.setType(KEYWORD_EXPOSEDFIELD); }
    else if (token_text == "EXTERNPROTO")  { token.setType(KEYWORD_EXTERNPROTO); }
    else if (token_text == "FALSE")        { token.setType(KEYWORD_FALSE); }
    else if (token_text == "field")        { this->expectFieldType();
                                             token.setType(KEYWORD_FIELD); }
    else if (token_text == "IS")           { token.setType(KEYWORD_IS); }
    else if (token_text == "NULL")         { token.setType(KEYWORD_NULL); }
    else if (token_text == "PROTO")        { token.setType(KEYWORD_PROTO); }
    else if (token_text == "ROUTE")        { token.setType(KEYWORD_ROUTE); }
    else if (token_text == "TO")           { token.setType(KEYWORD_TO); }
    else if (token_text == "TRUE")         { token.setType(KEYWORD_TRUE); }
    else if (token_text == "USE")          { token.setType(KEYWORD_USE); }
    else                                   { return false; }
    return true;
}

inline bool Vrml97Scanner::identifyFieldType(antlr::Token & token)
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
    else                                 { return false; }
    return true;
}

inline bool Vrml97Scanner::identifyTerminalSymbol(antlr::Token & token)
{
    std::string const token_text(token.getText());
    if      (token_text == "[") { token.setType(LBRACKET); }
    else if (token_text == "]") { token.setType(RBRACKET); }
    else if (token_text == "{") { token.setType(LBRACE); }
    else if (token_text == "}") { token.setType(RBRACE); }
    else                        { return false; }
    return true;
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
    Vrml97Parser(openvrml::browser & b,
                 antlr::TokenStream & lexer,
                 const std::string & uri):
        antlr::LLkParser(lexer, 1),
        browser_(&b)
    {
        this->Parser::setFilename(uri);
    }

    virtual void consume()
    {
        this->last_token_ = this->LT(1);
        this->LLkParser::consume();
    }

    virtual void reportError(const antlr::RecognitionException & ex)
    {
        std::ostringstream out;
        out << ex.getFilename() << ':' << ex.getLine() << ':' << ex.getColumn()
            << ": error: " << ex.getMessage();
        this->browser_->err(out.str());
    }

    virtual void reportError(const std::string & s)
    {
        std::ostringstream out;
        out << this->getFilename() << ':' << this->last_token_->getLine()
            << ':' << this->last_token_->getColumn() << ": error: " << s;
        this->browser_->err(out.str());
    }

    virtual void reportWarning(const std::string & s)
    {
        std::ostringstream out;
        out << this->getFilename() << ':' << this->last_token_->getLine()
            << ':' << this->last_token_->getColumn() << ": warning: " << s;
        this->browser_->err(out.str());
    }

private:
    antlr::RefToken last_token_;
    openvrml::browser * browser_;
}

vrmlScene[const openvrml::scene & scene,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
          std::map<std::string, std::string> & meta_data]
options { defaultErrorHandler=false; }
{
    boost::ignore_unused_variable_warning(meta_data);

    const profile & p = ::profile_registry_.at(vrml97_profile::id);
    std::auto_ptr<scope> root_scope_auto_ptr =
        p.create_root_scope(scene.browser(), this->getFilename());
    const boost::shared_ptr<scope> root_scope(root_scope_auto_ptr);
}
    :   (statement[scene, nodes, root_scope])*
    ;

statement[const openvrml::scene & scene,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
          const boost::shared_ptr<openvrml::scope> & scope]
options { defaultErrorHandler=false; }
{
    boost::intrusive_ptr<openvrml::node> node;
}
    :   node=nodeStatement[scene, scope, std::string()] {
            //
            // If we are unable to parse a node, node will be null.
            //
            if (node) { nodes.push_back(node); }
        }
    |   protoStatement[scene, scope]
    |   routeStatement[*scope]
    ;

nodeStatement[const openvrml::scene & scene,
              const boost::shared_ptr<openvrml::scope> & scope,
              const std::string & script_node_id]
returns [boost::intrusive_ptr<openvrml::node> n]
options { defaultErrorHandler=false; }
    :   KEYWORD_DEF id0:ID n=node[scene, scope, id0->getText()]
    |   KEYWORD_USE id1:ID {
            if (id1->getText() == script_node_id) {
                //
                // Script node self-reference.
                //
                n = openvrml::node::self_tag;
            } else {
                using antlr::SemanticException;
                assert(scope);
                n = boost::intrusive_ptr<openvrml::node>(
                    scope->find_node(id1->getText()));
                if (!n) {
                    throw SemanticException("node \"" + id1->getText()
                                            + "\" has not been defined in "
                                            + "this scope",
                                            this->getFilename(),
                                            id1->getLine(),
                                            id1->getColumn());
                }
            }
        }
    |   n=node[scene, scope, std::string()]
    ;

protoStatement[const openvrml::scene & scene,
               const boost::shared_ptr<openvrml::scope> & scope]
options { defaultErrorHandler=false; }
    //
    // XXX What if the node type already exists in the scope? Probably need to
    // XXX handle an exception here.
    //
    :   externproto[scene, scope]
    |   proto[scene, scope]
    ;

proto[const openvrml::scene & scene,
      const boost::shared_ptr<openvrml::scope> & scope]
options { defaultErrorHandler=false; }
{
    using std::string;
    using std::vector;
    using boost::shared_ptr;
    using boost::dynamic_pointer_cast;

    node_interface_set interfaces;
    proto_node_metatype::default_value_map_t default_value_map;
    vector<boost::intrusive_ptr<openvrml::node> > impl_nodes;
    proto_node_metatype::is_map_t is_map;
    proto_node_metatype::routes_t routes;
}
    :   KEYWORD_PROTO id:ID {
            assert(scope);
            const boost::shared_ptr<openvrml::scope>
                proto_scope(new openvrml::scope(id->getText(), scope));
        } LBRACKET (protoInterfaceDeclaration[scene,
                                              scope,
                                              id->getText(),
                                              interfaces,
                                              default_value_map])* RBRACKET
        LBRACE protoBody[scene,
                         proto_scope,
                         interfaces,
                         impl_nodes,
                         is_map,
                         routes] RBRACE {
            const shared_ptr<openvrml::node_metatype>
                node_metatype(
                    new proto_node_metatype(path(*proto_scope),
                                         scene.browser(),
                                         interfaces,
                                         default_value_map,
                                         impl_nodes,
                                         is_map,
                                         routes));
            //
            // Add the new node_metatype (prototype definition) to the browser's
            // node_metatype_map.
            //
            scene.browser().add_node_metatype(node_metatype->id(), node_metatype);

            if (!dynamic_pointer_cast<proto_node_metatype>(
                    scene.browser().node_metatype(
                        node_metatype_id(this->getFilename())))) {
                scene.browser()
                    .add_node_metatype(node_metatype_id(this->getFilename()),
                                       node_metatype);
            }

            //
            // PROTOs implicitly introduce a new node type as well.
            //
            const shared_ptr<node_type> node_type =
                node_metatype->create_type(id->getText(), interfaces);
            assert(node_type);
            if (!scope->add_type(node_type).second) {
                using antlr::SemanticException;
                throw SemanticException("node type \"" + node_type->id()
                                        + "\" has already been defined in "
                                        "this scope",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

protoInterfaceDeclaration[
    const openvrml::scene & scene,
    const boost::shared_ptr<const openvrml::scope> & outer_scope,
    const std::string & proto_id,
    node_interface_set & interfaces,
    proto_node_metatype::default_value_map_t & default_value_map]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;

    node_interface::type_id it;
    field_value::type_id ft;
    boost::shared_ptr<field_value> fv;
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
                                        this->getFilename(),
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
            const boost::shared_ptr<openvrml::scope> field_decl_scope(
                new scope(proto_id + '.' + id1->getText(), outer_scope));
        }
        fv=fieldValue[scene, field_decl_scope, ft, std::string()] {
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
                                        this->getFilename(),
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

protoBody[const openvrml::scene & scene,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & interfaces,
          std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
          proto_node_metatype::is_map_t & is_map,
          proto_node_metatype::routes_t & routes]
options { defaultErrorHandler=false; }
{
    assert(scope);
    assert(impl_nodes.empty());
    assert(is_map.empty());
    assert(routes.empty());

    boost::intrusive_ptr<openvrml::node> n;
}
    :   (protoStatement[scene, scope])* n=protoNodeStatement[scene,
                                                             scope,
                                                             interfaces,
                                                             is_map,
                                                             routes,
                                                             std::string()]
        {
            assert(n);
            impl_nodes.push_back(n);
        }
        (protoBodyStatement[scene,
                            scope,
                            interfaces,
                            impl_nodes,
                            is_map,
                            routes])*
    ;

protoBodyStatement[
    const openvrml::scene & scene,
    const boost::shared_ptr<openvrml::scope> & scope,
    const node_interface_set & interfaces,
    std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
    proto_node_metatype::is_map_t & is_map,
    proto_node_metatype::routes_t & routes]
options { defaultErrorHandler=false; }
{
    assert(scope);
    assert(!impl_nodes.empty());

    boost::intrusive_ptr<openvrml::node> n;
}
    :   n=protoNodeStatement[scene,
                             scope,
                             interfaces,
                             is_map,
                             routes,
                             std::string()] {
            assert(n);
            impl_nodes.push_back(n);
        }
    |   protoStatement[scene, scope]
    |   protoRouteStatement[*scope, routes]
    ;

protoNodeStatement[const openvrml::scene & scene,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const node_interface_set & proto_interfaces,
                   proto_node_metatype::is_map_t & is_map,
                   proto_node_metatype::routes_t & routes,
                   const std::string & script_node_id]
returns [boost::intrusive_ptr<openvrml::node> n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
}
    :   KEYWORD_DEF id0:ID n=protoNode[scene,
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
                n = node::self_tag;
            } else {
                n = boost::intrusive_ptr<openvrml::node>(
                    scope->find_node(id1->getText()));
                if (!n) {
                    throw SemanticException("node \"" + id1->getText()
                                            + "\" has not been defined in "
                                            + "this scope",
                                            this->getFilename(),
                                            id1->getLine(),
                                            id1->getColumn());
                }
            }
        }
    |   n=protoNode[scene,
                    scope,
                    proto_interfaces,
                    is_map,
                    routes,
                    std::string()]
    ;

externproto[const openvrml::scene & scene,
            const boost::shared_ptr<openvrml::scope> & scope]
options { defaultErrorHandler=false; }
{
    using std::string;
    using std::vector;
    using boost::shared_ptr;

    openvrml::node_interface_set interfaces;
    openvrml::mfstring uri_list;
    shared_ptr<node_type> node_type;

    //
    // If we are parsing an "anonymous" stream--that is, one passed to
    // browser::create_vrml_from_stream--we cannot use the URI associated with
    // the stream to resolve relative URIs.  So in this instance, we get the
    // base URI from browser::world_url.  If browser::world_url is an empty
    // string, we call create_file_url with an empty (relative) uri.
    //
    const ::uri base_uri = anonymous_stream_id(::uri(this->getFilename()))
        ? scene.browser().world_url().empty()
            ? create_file_url(::uri())
            : ::uri(scene.browser().world_url())
        : ::uri(this->getFilename());
}
    :   KEYWORD_EXTERNPROTO id:ID LBRACKET
        (externInterfaceDeclaration[interfaces])* RBRACKET
        uri_list=externprotoUrlList {
            const vector<string> & alt_uris = uri_list.value();
            for (vector<string>::const_iterator resource_id = alt_uris.begin();
                 resource_id != alt_uris.end();
                 ++resource_id) {
                const ::uri absolute_uri = relative(::uri(*resource_id))
                    ? ::uri(*resource_id).resolve_against(base_uri)
                    : ::uri(*resource_id);
                const shared_ptr<openvrml::node_metatype> node_metatype =
                    scene.browser().node_metatype(node_metatype_id(absolute_uri));
                if (node_metatype) {
                    node_type = node_metatype->create_type(id->getText(),
                                                           interfaces);
                    break;
                }
            }

            if (!node_type) {
                const shared_ptr<node_metatype> externproto_class(
                    new externproto_node_metatype(
                        path(*scope) + '#' + id->getText(),
                        scene,
                        alt_uris,
                        scene.browser().load_proto_thread_group_));

                scene.browser().add_node_metatype(externproto_class->id(),
                                                  externproto_class);
                for (vector<string>::const_iterator resource_id =
                         alt_uris.begin();
                     resource_id != alt_uris.end();
                     ++resource_id) {
                    const ::uri absolute_uri = relative(::uri(*resource_id))
                        ? ::uri(*resource_id).resolve_against(base_uri)
                        : ::uri(*resource_id);
                    scene.browser()
                        .add_node_metatype(node_metatype_id(absolute_uri),
                                           externproto_class);
                }

                node_type = externproto_class->create_type(id->getText(),
                                                           interfaces);
            }

            assert(node_type);

            if (!scope->add_type(node_type).second) {
                using antlr::SemanticException;
                throw SemanticException("node type \"" + node_type->id()
                                        + "\" has already been defined in "
                                        + "this scope",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
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
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    ;

interfaceType returns [node_interface::type_id interface_type]
options { defaultErrorHandler=false; }
    :   interface_type=eventInterfaceType
    |   interface_type=fieldInterfaceType
    ;

externprotoUrlList returns [mfstring urlList]
options { defaultErrorHandler=false; }
{
    using std::string;
    using std::vector;
    using openvrml::mfstring;

    string s;
}
    :   s=stringValue { urlList.value(vector<string>(1, s)); }
    |   LBRACKET {
            vector<string> value;
        } (s=stringValue { value.push_back(s); })* RBRACKET {
            urlList.value(value);
        }
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
                throw SemanticException("node \"" + from_node_id->getText()
                                        + "\" has not been defined in this "
                                        "scope",
                                        this->getFilename(),
                                        from_node_id->getLine(),
                                        from_node_id->getColumn());
            }

            openvrml::node * const to_node =
                scope.find_node(to_node_id->getText());
            if (!to_node) {
                throw SemanticException("node \"" + to_node_id->getText()
                                        + "\" has not been defined in this "
                                        "scope",
                                        this->getFilename(),
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
                                        this->getFilename(),
                                        from_node_id->getLine(),
                                        from_node_id->getColumn());
            }
        }
    ;

protoRouteStatement[const openvrml::scope & scope,
                    proto_node_metatype::routes_t & routes]
options { defaultErrorHandler=false; }
    :   KEYWORD_ROUTE from_node_id:ID PERIOD eventout_id:ID
        KEYWORD_TO to_node_id:ID PERIOD eventin_id:ID
        {
            using std::runtime_error;
            using antlr::SemanticException;

            openvrml::node * const from_node =
                scope.find_node(from_node_id->getText());
            if (!from_node) {
                throw SemanticException("node \"" + from_node_id->getText()
                                        + "\" has not been defined in this "
                                        "scope",
                                        this->getFilename(),
                                        from_node_id->getLine(),
                                        from_node_id->getColumn());
            }

            openvrml::node * const to_node =
                scope.find_node(to_node_id->getText());
            if (!to_node) {
                throw SemanticException("node \"" + to_node_id->getText()
                                        + "\" has not been defined in this "
                                        "scope",
                                        this->getFilename(),
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
                from_node->type().interfaces();
            const node_interface_set::const_iterator from_interface =
                find_if(from_node_interfaces.begin(),
                        from_node_interfaces.end(),
                        bind2nd(node_interface_matches_eventout(),
                                eventout_id->getText()));
            if (from_interface == from_node_interfaces.end()) {
                throw SemanticException(from_node->type().id() + " node has "
                                        "no eventOut \""
                                        + eventout_id->getText() + "\"",
                                        this->getFilename(),
                                        eventout_id->getLine(),
                                        eventout_id->getColumn());
            }

            const node_interface_set & to_node_interfaces =
                to_node->type().interfaces();
            const node_interface_set::const_iterator to_interface =
                find_if(to_node_interfaces.begin(), to_node_interfaces.end(),
                        bind2nd(node_interface_matches_eventin(),
                                eventin_id->getText()));
            if (to_interface == to_node_interfaces.end()) {
                throw SemanticException(to_node->type().id() + " node has no "
                                        "eventIn \"" + eventin_id->getText()
                                        + "\"",
                                        this->getFilename(),
                                        eventin_id->getLine(),
                                        eventin_id->getColumn());
            }

            if (to_interface->field_type != from_interface->field_type) {
                throw SemanticException("mismatch between eventOut and "
                                        "eventIn types",
                                        this->getFilename(),
                                        eventin_id->getLine(),
                                        eventin_id->getColumn());
            }

            const proto_node_metatype::route r(*from_node, eventout_id->getText(),
                                            *to_node, eventin_id->getText());
            routes.push_back(r);
        }
    ;

node[const openvrml::scene & scene,
     const boost::shared_ptr<openvrml::scope> & scope,
     const std::string & node_id]
returns [boost::intrusive_ptr<openvrml::node> n]
options { defaultErrorHandler = false; }
{
    using antlr::SemanticException;
    using boost::intrusive_ptr;

    initial_value_map initial_values;
    node_interface_set interfaces;
    boost::shared_ptr<openvrml::node_type> node_type;
}
    :   { !LT(1)->getText().compare("Script") }? scriptId:ID LBRACE
            (   nodeBodyElement[scene,
                                scope,
                                script_node_interface_set,
                                initial_values]
            |   scriptInterfaceDeclaration[scene,
                                           scope,
                                           interfaces,
                                           initial_values,
                                           node_id]
            )* RBRACE {
            n = intrusive_ptr<openvrml::node>(
                new script_node(scene.browser().script_node_metatype_,
                                scope,
                                interfaces,
                                initial_values));
            if (!node_id.empty()) { n->id(node_id); }
        }
    |   nodeTypeId:ID {
            node_type = scope->find_type(nodeTypeId->getText());
            if (!node_type) {
                throw SemanticException("unknown node type \""
                                        + nodeTypeId->getText() + "\"",
                                        this->getFilename(),
                                        nodeTypeId->getLine(),
                                        nodeTypeId->getColumn());
            }

        } LBRACE (nodeBodyElement[scene,
                                  scope,
                                  node_type->interfaces(),
                                  initial_values])*
        RBRACE {
            n = node_type->create_node(scope, initial_values);

            if (!node_id.empty()) { n->id(node_id); }
        }
    ;
    exception
    catch [std::invalid_argument & ex] {
        throw SemanticException(ex.what(),
                                this->getFilename(),
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }
    catch [unsupported_interface & ex] {
        throw SemanticException(ex.what(),
                                this->getFilename(),
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }
    catch [std::bad_cast &] {
        throw SemanticException("incorrect value type for field or "
                                "exposedField",
                                this->getFilename(),
                                LT(1)->getLine(),
                                LT(1)->getColumn());
    }

nodeBodyElement[const openvrml::scene & scene,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & interfaces,
                initial_value_map & initial_values]
options { defaultErrorHandler=false; }
{
    using std::find_if;
    using std::bind2nd;
    using antlr::SemanticException;
    field_value::type_id ft = field_value::invalid_type_id;
    boost::shared_ptr<field_value> fv;
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
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
            ft = interface->field_type;
        } fv=fieldValue[scene, scope, ft, std::string()] {
            assert(fv);
            const bool succeeded =
                initial_values.insert(make_pair(id->getText(), fv)).second;
            if (!succeeded) {
                throw SemanticException("value for " + id->getText()
                                        + " already declared",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    |   routeStatement[*scope]
    |   protoStatement[scene, scope]
    ;

scriptInterfaceDeclaration[const openvrml::scene & scene,
                           const boost::shared_ptr<openvrml::scope> & scope,
                           node_interface_set & interfaces,
                           initial_value_map & initial_values,
                           const std::string & node_id]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
    node_interface::type_id it(node_interface::invalid_type_id);
    field_value::type_id ft(field_value::invalid_type_id);
}
    :   it=eventInterfaceType ft=fieldType id:ID {
            const node_interface interface(it, ft, id->getText());
            const bool succeeded = interfaces.insert(interface).second;
            if (!succeeded) {
                using std::string;
                using boost::lexical_cast;
                throw SemanticException("Interface \""
                                        + lexical_cast<string>(interface)
                                        + "\" conflicts with previous "
                                        "declaration",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        }
    |   scriptFieldInterfaceDeclaration[scene,
                                        scope,
                                        interfaces,
                                        initial_values,
                                        node_id]
    ;

scriptFieldInterfaceDeclaration[
    const openvrml::scene & scene,
    const boost::shared_ptr<openvrml::scope> & scope,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id]
options { defaultErrorHandler=false; }
{
    using std::find_if;
    using antlr::SemanticException;

    field_value::type_id ft = field_value::invalid_type_id;
    boost::shared_ptr<field_value> fv;
}
    :   KEYWORD_FIELD ft=fieldType id:ID fv=fieldValue[scene,
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
                throw SemanticException("interface \"" + id->getText()
                                        + "\" already declared for Script "
                                        "node",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
            succeeded = initial_values.insert(make_pair(id->getText(), fv))
                        .second;
            assert(succeeded);
        }
    ;

protoNode[const openvrml::scene & scene,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & proto_interfaces,
          proto_node_metatype::is_map_t & is_map,
          proto_node_metatype::routes_t & routes,
          const std::string & node_id]
returns [boost::intrusive_ptr<openvrml::node> n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;

    initial_value_map initial_values;
    node_interface_set interfaces;
    is_list is_mappings;
    boost::shared_ptr<openvrml::node_type> node_type;
}
    :   (
            { !LT(1)->getText().compare("Script") }? scriptId:ID LBRACE
            (   protoNodeBodyElement[scene,
                                     scope,
                                     proto_interfaces,
                                     is_map,
                                     routes,
                                     script_node_interface_set,
                                     initial_values,
                                     is_mappings]
            |   protoScriptInterfaceDeclaration[scene,
                                                scope,
                                                proto_interfaces,
                                                is_map,
                                                routes,
                                                node_id,
                                                interfaces,
                                                initial_values,
                                                is_mappings]
            )* RBRACE {
                n = boost::intrusive_ptr<openvrml::node>(
                    new script_node(scene.browser().script_node_metatype_,
                                    scope,
                                    interfaces,
                                    initial_values));
                if (!node_id.empty()) { n->id(node_id); }
            }
        |   nodeTypeId:ID {
                node_type = scope->find_type(nodeTypeId->getText());
                if (!node_type) {
                    throw SemanticException("unknown node type \""
                                            + nodeTypeId->getText() + "\"",
                                            this->getFilename(),
                                            nodeTypeId->getLine(),
                                            nodeTypeId->getColumn());
                }
            }
            LBRACE (protoNodeBodyElement[scene,
                                         scope,
                                         proto_interfaces,
                                         is_map,
                                         routes,
                                         node_type->interfaces(),
                                         initial_values,
                                         is_mappings])* RBRACE {
                n = node_type->create_node(scope, initial_values);
                if (!node_id.empty()) { n->id(node_id); }
            }
        ) {
            for (is_list::const_iterator is_mapping = is_mappings.begin();
                 is_mapping != is_mappings.end();
                 ++is_mapping) {
                typedef proto_node_metatype::is_target is_target;
                is_map.insert(make_pair(is_mapping->second,
                                        is_target(*n, is_mapping->first)));
            }
        }
    ;

protoNodeBodyElement[const openvrml::scene & scene,
                     const boost::shared_ptr<openvrml::scope> & scope,
                     const node_interface_set & proto_interfaces,
                     proto_node_metatype::is_map_t & is_map,
                     proto_node_metatype::routes_t & routes,
                     const node_interface_set & node_interfaces,
                     initial_value_map & initial_values,
                     is_list & is_mappings]
options { defaultErrorHandler=false; }
{
    using std::string;
    using antlr::SemanticException;

    boost::shared_ptr<field_value> fv;
}
    :   interface_id:ID {
            const node_interface_set::const_iterator impl_node_interface =
                find_interface(node_interfaces, interface_id->getText());
            if (impl_node_interface == node_interfaces.end()) {
                throw SemanticException("node has no interface \""
                                        + interface_id->getText() + "\"",
                                        this->getFilename(),
                                        interface_id->getLine(),
                                        interface_id->getColumn());
            }
        } ( {impl_node_interface->type == node_interface::field_id
            || impl_node_interface->type == node_interface::exposedfield_id}?
            (   fv=protoFieldValue[scene,
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
        |   isStatement[impl_node_interface->id, is_mappings]
        )
    |   protoRouteStatement[*scope, routes]
    |   protoStatement[scene, scope]
    ;

isStatement[const std::string & impl_node_interface_id,
            is_list & is_mappings]
options { defaultErrorHandler=false; }
    :   KEYWORD_IS id:ID {
            is_mappings.insert(make_pair(impl_node_interface_id,
                                         id->getText()));
        }
    ;

protoScriptInterfaceDeclaration[
    const openvrml::scene & scene,
    const boost::shared_ptr<openvrml::scope> & scope,
    const node_interface_set & proto_interfaces,
    proto_node_metatype::is_map_t & is_map,
    proto_node_metatype::routes_t & routes,
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
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        } (isStatement[id->getText(), is_mappings])?
    |   protoScriptFieldInterfaceDeclaration[scene,
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
    const openvrml::scene & scene,
    const boost::shared_ptr<openvrml::scope> & scope,
    const node_interface_set & proto_interfaces,
    proto_node_metatype::is_map_t & is_map,
    proto_node_metatype::routes_t & routes,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id,
    is_list & is_mappings]
options { defaultErrorHandler=false; }
{
    field_value::type_id ft;
    boost::shared_ptr<field_value> fv;
    bool succeeded;
}
    :   KEYWORD_FIELD ft=fieldType id:ID {
            succeeded =
                interfaces.insert(node_interface(node_interface::field_id,
                                                 ft,
                                                 id->getText()))
                    .second;
            if (!succeeded) {
                using antlr::SemanticException;
                throw SemanticException("interface \"" + id->getText()
                                        + "\" already declared for Script "
                                        "node",
                                        this->getFilename(),
                                        id->getLine(),
                                        id->getColumn());
            }
        } (
            fv=protoFieldValue[scene,
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
        |   isStatement[id->getText(), is_mappings] {
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
    :   FIELDTYPE_MFCOLOR    { ft = field_value::mfcolor_id; }
    |   FIELDTYPE_MFFLOAT    { ft = field_value::mffloat_id; }
    |   FIELDTYPE_MFINT32    { ft = field_value::mfint32_id; }
    |   FIELDTYPE_MFNODE     { ft = field_value::mfnode_id; }
    |   FIELDTYPE_MFROTATION { ft = field_value::mfrotation_id; }
    |   FIELDTYPE_MFSTRING   { ft = field_value::mfstring_id; }
    |   FIELDTYPE_MFTIME     { ft = field_value::mftime_id; }
    |   FIELDTYPE_MFVEC2F    { ft = field_value::mfvec2f_id; }
    |   FIELDTYPE_MFVEC3F    { ft = field_value::mfvec3f_id; }
    |   FIELDTYPE_SFBOOL     { ft = field_value::sfbool_id; }
    |   FIELDTYPE_SFCOLOR    { ft = field_value::sfcolor_id; }
    |   FIELDTYPE_SFFLOAT    { ft = field_value::sffloat_id; }
    |   FIELDTYPE_SFIMAGE    { ft = field_value::sfimage_id; }
    |   FIELDTYPE_SFINT32    { ft = field_value::sfint32_id; }
    |   FIELDTYPE_SFNODE     { ft = field_value::sfnode_id; }
    |   FIELDTYPE_SFROTATION { ft = field_value::sfrotation_id; }
    |   FIELDTYPE_SFSTRING   { ft = field_value::sfstring_id; }
    |   FIELDTYPE_SFTIME     { ft = field_value::sftime_id; }
    |   FIELDTYPE_SFVEC2F    { ft = field_value::sfvec2f_id; }
    |   FIELDTYPE_SFVEC3F    { ft = field_value::sfvec3f_id; }
    ;

fieldValue[const openvrml::scene & scene,
           const boost::shared_ptr<openvrml::scope> & scope,
           const openvrml::field_value::type_id ft,
           const std::string & node_id]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
    :   { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=nodeFieldValue[scene, scope, ft, node_id]
    |   fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[const openvrml::scene & scene,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & proto_interfaces,
                proto_node_metatype::is_map_t & is_map,
                proto_node_metatype::routes_t & routes,
                const field_value::type_id ft,
                const std::string & script_node_id]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
    :   { (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) }?
        fv=protoNodeFieldValue[scene,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft,
                               script_node_id] {
            assert(fv);
        }
    |   fv=nonNodeFieldValue[ft] { assert(fv); }
    ;

nonNodeFieldValue[openvrml::field_value::type_id ft]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
    :   { ft == field_value::sfbool_id }? fv=sfBoolValue
    |   { ft == field_value::sfcolor_id }? fv=sfColorValue
    |   { ft == field_value::sffloat_id }? fv=sfFloatValue
    |   { ft == field_value::sfimage_id }? fv=sfImageValue
    |   { ft == field_value::sfint32_id }? fv=sfInt32Value
    |   { ft == field_value::sfrotation_id }? fv=sfRotationValue
    |   { ft == field_value::sfstring_id }? fv=sfStringValue
    |   { ft == field_value::sftime_id }? fv=sfTimeValue
    |   { ft == field_value::sfvec2f_id }? fv=sfVec2fValue
    |   { ft == field_value::sfvec3f_id }? fv=sfVec3fValue
    |   { ft == field_value::mfcolor_id }? fv=mfColorValue
    |   { ft == field_value::mffloat_id }? fv=mfFloatValue
    |   { ft == field_value::mfint32_id }? fv=mfInt32Value
    |   { ft == field_value::mfrotation_id }? fv=mfRotationValue
    |   { ft == field_value::mfstring_id }? fv=mfStringValue
    |   { ft == field_value::mftime_id }? fv=mfTimeValue
    |   { ft == field_value::mfvec2f_id }? fv=mfVec2fValue
    |   fv=mfVec3fValue
    ;

nodeFieldValue[const openvrml::scene & scene,
               const boost::shared_ptr<openvrml::scope> & scope,
               openvrml::field_value::type_id ft,
               const std::string & script_node_id]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    :   { ft == field_value::sfnode_id }? fv=sfNodeValue[scene,
                                                         scope,
                                                         script_node_id]
    |   fv=mfNodeValue[scene, scope, script_node_id]
    ;

protoNodeFieldValue[const openvrml::scene & scene,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_metatype::is_map_t & is_map,
                    proto_node_metatype::routes_t & routes,
                    field_value::type_id ft,
                    const std::string & script_node_id]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
    :   { ft == field_value::sfnode_id }? fv=protoSfNodeValue[scene,
                                                              scope,
                                                              proto_interfaces,
                                                              is_map,
                                                              routes,
                                                              script_node_id]
    |   fv=protoMfNodeValue[scene,
                            scope,
                            proto_interfaces,
                            is_map,
                            routes,
                            script_node_id]
    ;

sfBoolValue returns [boost::shared_ptr<field_value> sbv]
options { defaultErrorHandler=false; }
{
    bool val(false);
}
    :   val=boolValue { sbv.reset(new sfbool(val)); }
    ;

boolValue returns [bool val = false]
options { defaultErrorHandler=false; }
    :   KEYWORD_TRUE { val = true; }
    |   KEYWORD_FALSE { val = false; }
    ;

sfColorValue returns [boost::shared_ptr<field_value> scv]
options { defaultErrorHandler=false; }
{
    color c;
}
    :   colorValue[c] { scv.reset(new sfcolor(c)); }
    ;

mfColorValue
returns [boost::shared_ptr<field_value> mcv =
            boost::shared_ptr<field_value>(new mfcolor)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    color c;
    mfcolor & colors = static_cast<mfcolor &>(*mcv);
}
    :   colorValue[c] { colors.value(vector<color>(1, c)); }
    |   LBRACKET {
            vector<color> value;
        } (colorValue[c] { value.push_back(c); })* RBRACKET {
            colors.value(value);
        }
    ;

colorValue[color & c]
options { defaultErrorHandler=false; }
{
    float r, g, b;
}
    :   r=colorComponent g=colorComponent b=colorComponent { c.r(r);
                                                             c.g(g);
                                                             c.b(b); }
    ;

//
// Issue a warning if a color component is not in [0, 1].
//
colorComponent returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    :   val=floatValue {
            if (val < 0.0 || val > 1.0) {
                this->reportWarning(
                    "color component values must be from 0 to 1");
                if (val < 0.0) {
                    val = 0.0;
                } else if (val > 1.0) {
                    val = 1.0;
                }
            }
        }
    ;

sfFloatValue returns [boost::shared_ptr<field_value> sfv]
options { defaultErrorHandler=false; }
{
    float f;
}
    :   f=floatValue { sfv.reset(new sffloat(f)); }
    ;

mfFloatValue
returns [boost::shared_ptr<field_value> mfv =
            boost::shared_ptr<field_value>(new mffloat)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    float f;
    mffloat & floats = static_cast<mffloat &>(*mfv);
}
    :   f=floatValue { floats.value(vector<float>(1, f)); }
    |   LBRACKET {
            vector<float> value;
        } (f=floatValue { value.push_back(f); })* RBRACKET {
            floats.value(value);
        }
    ;

floatValue returns [float val]
options { defaultErrorHandler=false; }
    :   f0:REAL     { val = float(atof(f0->getText().c_str())); }
    |   f1:INTEGER  { val = float(atol(f1->getText().c_str())); }
    ;

sfImageValue returns [boost::shared_ptr<field_value> siv]
options { defaultErrorHandler=false; }
{
    image img;
}
    :   imageValue[img] {
            siv.reset(new sfimage(img));
        }
    ;

imageValue[image & img]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
    size_t x, y, comp;
    int32 pixel;
}
    :   x=intValue y=intValue comp=intValue {
            img.comp(comp);
            img.resize(x, y);
            const size_t img_size = x * y;
            for (size_t pixel_index = 0; pixel_index < img_size; ++pixel_index)
            ANTLR_LBRACE
        } (
            pixel=intValue {
                img.pixel(pixel_index, pixel);
            }
        ) {
            ANTLR_RBRACE
        }
    ;

sfInt32Value returns [boost::shared_ptr<field_value> siv]
options { defaultErrorHandler=false; }
{
    long i;
}
    :   i=intValue { siv.reset(new sfint32(i)); }
    ;

mfInt32Value
returns [boost::shared_ptr<field_value> miv =
            boost::shared_ptr<field_value>(new mfint32)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    long i;
    mfint32 & int32s = static_cast<mfint32 &>(*miv);
}
    :   i=intValue { int32s.value(vector<int32>(1, i)); }
    |   LBRACKET {
            vector<int32> value;
        } (i=intValue { value.push_back(i); })* RBRACKET {
            int32s.value(value);
        }
    ;

intValue returns [int32 val]
options { defaultErrorHandler=false; }
    :   i0:INTEGER {
            val = atol(i0->getText().c_str());
        }
    |   i1:HEX_INTEGER {
            val = int32(strtoul(i1->getText().c_str() + 2, 0, 16));
        }
    ;

sfNodeValue[const openvrml::scene & scene,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id]
returns [boost::shared_ptr<field_value> snv]
options { defaultErrorHandler=false; }
{
    boost::intrusive_ptr<openvrml::node> n;
}
    :   n=nodeStatement[scene, scope, script_node_id] {
            snv.reset(new sfnode(n));
        }
    |   KEYWORD_NULL { snv.reset(new sfnode); }
    ;

protoSfNodeValue[const openvrml::scene & scene,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_metatype::is_map_t & is_map,
                 proto_node_metatype::routes_t & routes,
                 const std::string & script_node_id]
returns [boost::shared_ptr<field_value> snv]
options { defaultErrorHandler=false; }
{
    boost::intrusive_ptr<openvrml::node> n;
}
    :   n=protoNodeStatement[scene,
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

mfNodeValue[const openvrml::scene & scene,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id]
returns [boost::shared_ptr<field_value> mnv =
         boost::shared_ptr<field_value>(new mfnode)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    boost::intrusive_ptr<openvrml::node> n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    :   n=nodeStatement[scene, scope, script_node_id] {
            if (n) { nodes.value(mfnode::value_type(1, n)); }
        }
    |   LBRACKET {
            mfnode::value_type value;
        } (
            n=nodeStatement[scene, scope, script_node_id] {
                if (n) { value.push_back(n); }
            }
        )* RBRACKET {
            nodes.value(value);
        }
    ;

protoMfNodeValue[const openvrml::scene & scene,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_metatype::is_map_t & is_map,
                 proto_node_metatype::routes_t & routes,
                 const std::string & script_node_id]
returns [boost::shared_ptr<field_value> mnv =
         boost::shared_ptr<field_value>(new mfnode)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    boost::intrusive_ptr<openvrml::node> n;
    mfnode & nodes = static_cast<mfnode &>(*mnv);
}
    :   n=protoNodeStatement[scene,
                             scope,
                             proto_interfaces,
                             is_map,
                             routes,
                             script_node_id] {
            if (n) { nodes.value(mfnode::value_type(1, n)); }
        }
    |   LBRACKET {
            mfnode::value_type value;
        } (
            n=protoNodeStatement[scene,
                                 scope,
                                 proto_interfaces,
                                 is_map,
                                 routes,
                                 script_node_id] {
                if (n) { value.push_back(n); }
            }
        )* RBRACKET {
            nodes.value(value);
        }
    ;

sfRotationValue returns [boost::shared_ptr<field_value> srv]
options { defaultErrorHandler=false; }
{
    rotation r;
}
    :   rotationValue[r] { srv.reset(new sfrotation(r)); }
    ;

mfRotationValue
returns [boost::shared_ptr<field_value> mrv =
         boost::shared_ptr<field_value>(new mfrotation)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    rotation r;
    mfrotation & rotations = static_cast<mfrotation &>(*mrv);
}
    :   rotationValue[r] { rotations.value(vector<rotation>(1, r)); }
    |   LBRACKET {
            vector<rotation> value;
        } (rotationValue[r] { value.push_back(r); })* RBRACKET {
            rotations.value(value);
        }
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
    :   x=floatValue y=floatValue z=floatValue angle=floatValue {
            vec3f axis(x, y, z);

            const float axisLength = axis.length();
            if (!fequal(axisLength, 1.0f)) {
                this->reportWarning("axis component of a rotation must be "
                                    "a normalized vector");
                if (fequal(axisLength, 0.0f)) {
                    axis.z(1.0);
                } else {
                    axis = axis.normalize();
                }
            }
            r = rotation(axis, angle);
        }
    ;

sfStringValue returns [boost::shared_ptr<field_value> ssv]
options { defaultErrorHandler=false; }
{
    std::string s;
}
    :   s=stringValue { ssv.reset(new sfstring(s)); }
    ;

mfStringValue
returns [boost::shared_ptr<field_value> msv =
         boost::shared_ptr<field_value>(new mfstring)]
options { defaultErrorHandler=false; }
{
    using std::string;
    using std::vector;

    string s;
    mfstring & strings = static_cast<mfstring &>(*msv);
}
    :   s=stringValue { strings.value(vector<string>(1, s)); }
    |   LBRACKET {
            vector<string> value;
        } (s=stringValue { value.push_back(s); })* RBRACKET {
            strings.value(value);
        }
    ;

stringValue returns [std::string str]
options { defaultErrorHandler=false; }
    :   s:STRING {
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

sfTimeValue returns [boost::shared_ptr<field_value> stv]
options { defaultErrorHandler=false; }
{
    double t(0.0);
}
    :   t=doubleValue { stv.reset(new sftime(t)); }
    ;

mfTimeValue
returns [boost::shared_ptr<field_value> mtv =
         boost::shared_ptr<field_value>(new mftime)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    double t;
    mftime & times = static_cast<mftime &>(*mtv);
}
    :   t=doubleValue { times.value(vector<double>(1, t)); }
    |   LBRACKET {
            vector<double> value;
        } (t=doubleValue { value.push_back(t); })* RBRACKET {
            times.value(value);
        }
    ;

doubleValue returns [double val = 0.0]
options { defaultErrorHandler=false; }
    :   d0:REAL    { val = atof(d0->getText().c_str()); }
    |   d1:INTEGER { val = double(atol(d1->getText().c_str())); }
    ;

sfVec2fValue returns [boost::shared_ptr<field_value> svv]
options { defaultErrorHandler=false; }
{
    vec2f v;
}
    :   vec2fValue[v] { svv.reset(new sfvec2f(v)); }
    ;

mfVec2fValue
returns [boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec2f)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    vec2f v;
    mfvec2f & vec2fs = static_cast<mfvec2f &>(*mvv);
}
    :   vec2fValue[v] { vec2fs.value(vector<vec2f>(1, v)); }
    |   LBRACKET {
            vector<vec2f> value;
        } (vec2fValue[v] { value.push_back(v); })* RBRACKET {
            vec2fs.value(value);
        }
    ;

vec2fValue[vec2f & v]
options { defaultErrorHandler=false; }
{
    float x, y;
}
    :   x=floatValue y=floatValue { v.x(x);
                                    v.y(y); }
    ;

sfVec3fValue returns [boost::shared_ptr<field_value> svv]
options { defaultErrorHandler=false; }
{
    vec3f v;
}
    :   vec3fValue[v] { svv.reset(new sfvec3f(v)); }
    ;

mfVec3fValue
returns [boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec3f)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    vec3f v;
    mfvec3f & vec3fs = static_cast<mfvec3f &>(*mvv);
}
    :   vec3fValue[v] { vec3fs.value(vector<vec3f>(1, v)); }
    |   LBRACKET {
            vector<vec3f> value;
        } (vec3fValue[v] { value.push_back(v); })* RBRACKET {
            vec3fs.value(value);
        }
    ;

vec3fValue[vec3f & v]
options { defaultErrorHandler=false; }
{
    float x, y, z;
}
    :   x=floatValue y=floatValue z=floatValue { v.x(x);
                                                 v.y(y);
                                                 v.z(z); }
    ;
