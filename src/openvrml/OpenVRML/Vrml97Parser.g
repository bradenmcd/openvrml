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
# include "nodeptr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace OpenVRML {
    class VrmlNamespace;
    class ScriptNode;
    class NodeType;
    class Doc2;
}

namespace ANTLR_LBRACE
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

        Vrml97Scanner(istream &);

        virtual antlr::RefToken nextToken();
        
        size_t line() const;
        size_t col() const;

    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);

        istream &   _istm;
        size_t      _line;
        size_t      _col;
        int         _c;
        int         _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
    };
}

header "pre_include_cpp" {
# include "Vrml97Parser.hpp"
ANTLR_RBRACE
}

header "post_include_cpp" {
# include <assert.h>
# include <antlr/CommonToken.hpp>
# include "doc2.hpp"
# include "VrmlNamespace.h"
# include "node.h"
# include "script.h"
# include "private.h"

using namespace OpenVRML;
using namespace OpenVRML_;

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
        enum NodeInterfaceType {
            INVALID_NODE_INTERFACE_TYPE, EVENTIN, EVENTOUT, FIELD, EXPOSEDFIELD
        };
}

vrmlScene[OpenVRML::MFNode & mfNode, OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc]
    :   (statement[mfNode, vrmlNamespace, doc])*
    ;

statement[OpenVRML::MFNode & mfNode, OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc]
        {
            OpenVRML::NodePtr node;
        }
    :   node=nodeStatement[vrmlNamespace, doc]
        {
            assert(node);
            mfNode.addNode(*node);
        }
    |   protoStatement[vrmlNamespace, doc]
    |   routeStatement[vrmlNamespace]
    ;

nodeStatement[OpenVRML::VrmlNamespace & vrmlNamespace,
              const OpenVRML::Doc2 * doc] 
returns [OpenVRML::NodePtr n]
options {
    defaultErrorHandler=false;
}
    :   KEYWORD_DEF id0:ID n=node[vrmlNamespace, doc, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
            n.reset(vrmlNamespace.findNode(id1->getText()));
            if (!n) {
                throw antlr::SemanticException("Node \"" + id1->getText()
                                    + "\" has not been defined in this scope.",
                                    std::string(), LT(0)->getLine());
            }
        }
    |   n=node[vrmlNamespace, doc, std::string()]
    ;

protoStatement[OpenVRML::VrmlNamespace & vrmlNamespace,
               const OpenVRML::Doc2 * doc]
    :   externproto[vrmlNamespace, doc]
    |   proto[vrmlNamespace, doc]
    ;

proto[OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc]
    :   KEYWORD_PROTO id:ID
        {
            using OpenVRML::NodeTypePtr;
            
            NodeTypePtr nodeType(new NodeType(id->getText()));
            nodeType->setScope(vrmlNamespace);
        }
        LBRACKET (
            protoInterfaceDeclaration[vrmlNamespace, doc, *nodeType]
        )* RBRACKET LBRACE protoBody[doc, *nodeType] RBRACE
        {
            vrmlNamespace.addNodeType(nodeType);
        }
    ;

protoInterfaceDeclaration[OpenVRML::VrmlNamespace & vrmlNamespace,
                          const OpenVRML::Doc2 * doc,
                          OpenVRML::NodeType & nodeType]
        {
            using OpenVRML::FieldValue;
            NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
            FieldValue::Type ft = FieldValue::invalidType;
        }
    :   it=eventInterfaceType ft=fieldType id0:ID
        {
            if (nodeType.hasInterface(id0->getText()) != FieldValue::invalidType) {
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
        }
    |   it=fieldInterfaceType ft=fieldType id1:ID
        {
            FieldValue * fv = 0;
        }
        fv=fieldValue[vrmlNamespace, doc, ft]
        {
            const std::auto_ptr<FieldValue> autofv(fv);
            
            if (nodeType.hasInterface(id1->getText()) != FieldValue::invalidType) {
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
        }
    ;

eventInterfaceType returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    :   KEYWORD_EVENTIN  { it = EVENTIN; }
    |   KEYWORD_EVENTOUT { it = EVENTOUT; }
    ;

fieldInterfaceType returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    :   KEYWORD_FIELD        { it = FIELD; }
    |   KEYWORD_EXPOSEDFIELD { it = EXPOSEDFIELD; }
    ;

protoBody[const OpenVRML::Doc2 * doc, OpenVRML::NodeType & nodeType]
        {
            OpenVRML::NodePtr n;
        }
    :   (protoStatement[*nodeType.getScope(), doc])* n=protoNodeStatement[doc, nodeType]
        {
            assert(n);
            nodeType.addNode(*n);
        }
        (protoBodyStatement[doc, nodeType])*
    ;

protoBodyStatement[const OpenVRML::Doc2 * doc, OpenVRML::NodeType & nodeType]
        {
            OpenVRML::NodePtr n;
        }
    :   n=protoNodeStatement[doc, nodeType]
        {
            assert(n);
            nodeType.addNode(*n);
        }
    |   protoStatement[*nodeType.getScope(), doc]
    |   routeStatement[*nodeType.getScope()]
    ;

protoNodeStatement[const OpenVRML::Doc2 * doc,
                   OpenVRML::NodeType & protoNodeType]
returns [OpenVRML::NodePtr n]
options {
    defaultErrorHandler=false;
}
    :   KEYWORD_DEF id0:ID n=protoNode[doc, protoNodeType, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
            n.reset(protoNodeType.getScope()->findNode(id1->getText()));
            if (!n) {
                throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
            }
        }
    |   n=protoNode[doc, protoNodeType, std::string()]
    ;

externproto[OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc]
    :   KEYWORD_EXTERNPROTO id:ID
        {
            using OpenVRML::NodeTypePtr;
            
            NodeTypePtr nodeType(new NodeType(id->getText()));
        }
        LBRACKET (externInterfaceDeclaration[*nodeType])* RBRACKET
        {
            OpenVRML::MFString * urlList = 0;
        }
        urlList=externprotoUrlList
        {
            nodeType->setUrl(*urlList, doc);
            delete urlList;
            vrmlNamespace.addNodeType(nodeType);
        }
    ;

externInterfaceDeclaration[OpenVRML::NodeType & nodeType]
        {
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            OpenVRML::FieldValue::Type ft = OpenVRML::FieldValue::invalidType;
        }
    :   it=interfaceType ft=fieldType id:ID
        {
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
        }
    ;

interfaceType
returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    : it=eventInterfaceType
    | it=fieldInterfaceType
    ;

externprotoUrlList returns [OpenVRML::MFString * msv = 0]
        {
            std::string s;
        }
    :   s=stringValue
        {
            msv = new OpenVRML::MFString(1, &s);
        }
    |   LBRACKET
        {
            std::vector<std::string> stringVector;
        }
        (
            s=stringValue
            {
                stringVector.push_back(s);
            }
        )* RBRACKET
        {
            msv = new OpenVRML::MFString(stringVector.size(), &stringVector[0]);
        }
    ;

//
// When adding a route, we should check whether the nodes in question actually
// have the interfaces given in the ROUTE. This code should probably reside in
// the implementation of VrmlNode::addRoute(), and there is a comment in that
// section of the code indicating that it is intended to be added.  But it's not
// there now, and adding it could be complicated. (That is, it probably means
// VrmlNode::addRoute() should throw an exception.) So for now, I'm just going
// to add that code here in the parser. It shouldn't be too difficult to move it
// when the time comes.
// -- Braden McDaniel <braden@endoframe.com>, 6 Apr, 2000
//
routeStatement[const OpenVRML::VrmlNamespace & vrmlNamespace]
    :   KEYWORD_ROUTE fromNodeId:ID PERIOD fromInterfaceId:ID
        KEYWORD_TO toNodeId:ID PERIOD toInterfaceId:ID
        {
            using OpenVRML::FieldValue;
            using OpenVRML::Node;
            using OpenVRML::NodePtr;
            using OpenVRML::ScriptNode;
            
            Node * const fromNode =
                    vrmlNamespace.findNode(fromNodeId->getText());
            if (!fromNode) {
                throw antlr::SemanticException("Node \"" + fromNodeId->getText()
                        + "\" has not been defined in this scope.");
            }
            
            FieldValue::Type fromInterfaceType = FieldValue::invalidType;
            
            if (((fromInterfaceType = fromNode->type.hasEventOut(fromInterfaceId->getText())) == FieldValue::invalidType)
                && ((fromInterfaceType = fromNode->type.hasExposedField(fromInterfaceId->getText())) == FieldValue::invalidType)) {
                
                ScriptNode * fromScriptNode = 0;
                if ((fromScriptNode = fromNode->toScript())) {
                    fromInterfaceType = fromScriptNode->hasEventOut(fromInterfaceId->getText());
                }
                
                if (fromInterfaceType == FieldValue::invalidType) {
                    throw antlr::SemanticException(fromNode->type.getId() + " has no eventOut or exposedField \"" + fromInterfaceId->getText() + "\".");
                }
            }
            
            Node * const toNode =
                    vrmlNamespace.findNode(toNodeId->getText());
            if (!toNode) {
                throw antlr::SemanticException("Node \"" + toNodeId->getText()
                        + "\" has not been defined in this scope.");
            }
            
            FieldValue::Type toInterfaceType = FieldValue::invalidType;
            
            if (((toInterfaceType = toNode->type.hasEventIn(toInterfaceId->getText())) == FieldValue::invalidType)
                && ((toInterfaceType = toNode->type.hasExposedField(toInterfaceId->getText())) == FieldValue::invalidType)) {
                
                ScriptNode * toScriptNode = 0;
                if ((toScriptNode = toNode->toScript())) {
                    toInterfaceType = toScriptNode->hasEventIn(toInterfaceId->getText());
                }
                
                if (toInterfaceType == FieldValue::invalidType) {
                    throw antlr::SemanticException(toNode->type.getId()
                                        + " has no eventIn or exposedField \""
                                        + toInterfaceId->getText() + "\".");
                }
            }
            
            if (fromInterfaceType != toInterfaceType) {
                throw antlr::SemanticException("Routed interface types must "
                                                "match.");
            }
            
            fromNode->addRoute(fromInterfaceId->getText(),
                               NodePtr(toNode), toInterfaceId->getText());
        }
    ;

node[OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc,
     const std::string & nodeId]
returns [OpenVRML::NodePtr n]
options {
    defaultErrorHandler = false;
}
        {
            using OpenVRML::NodeTypePtr;
            using OpenVRML::NodePtr;
            using OpenVRML::ScriptNode;
            
            NodeTypePtr nodeType(0);
        }
    :
	{ !LT(1)->getText().compare("Script") }?
	scriptId:ID 
        {
            nodeType = vrmlNamespace.findType("Script");
            assert(nodeType);
            
            n = NodePtr(nodeType->newNode());
            
            if (nodeId.length() > 0) {
                n->setId(nodeId, &vrmlNamespace);
            }
            
            ScriptNode * const scriptNode = n->toScript();
            assert(scriptNode);
        }
        LBRACE (
            nodeBodyElement[vrmlNamespace, doc, *n]
            | scriptInterfaceDeclaration[vrmlNamespace, doc, *scriptNode]
        )* RBRACE
        
    |  	nodeTypeId:ID 
        {
            nodeType = vrmlNamespace.findType(nodeTypeId->getText());
            if (!nodeType) {
                throw antlr::SemanticException("Unknown node type \""
                                               + nodeTypeId->getText() + "\".",
                                               std::string(), LT(0)->getLine());
            }
            
            n = NodePtr(nodeType->newNode());
            
            if (nodeId.length() > 0) {
                n->setId(nodeId, &vrmlNamespace);
            }
        }
        LBRACE (nodeBodyElement[vrmlNamespace, doc, *n])* RBRACE
    ;

nodeBodyElement[OpenVRML::VrmlNamespace & vrmlNamespace,
                const OpenVRML::Doc2 * doc, OpenVRML::Node & node]
        {
            using OpenVRML::FieldValue;
            
            FieldValue::Type ft(FieldValue::invalidType);
        }
    :   id:ID
        {
            if (   ((ft = node.type.hasField(id->getText())) == FieldValue::invalidType)
                && ((ft = node.type.hasExposedField(id->getText())) == FieldValue::invalidType)) {
                
                throw antlr::SemanticException(node.type.getId()
                                    + " node has no field or exposedField \""
                                    + id->getText() + "\"",
                                    std::string(), LT(0)->getLine());
            }
            
            FieldValue * fv = 0;
        }
        fv=fieldValue[vrmlNamespace, doc, ft]
        {
            assert(fv);
            node.setField(id->getText(), *fv);
            delete fv;
        }
    |   routeStatement[vrmlNamespace]
    |   protoStatement[vrmlNamespace, doc]
    ;

scriptInterfaceDeclaration[OpenVRML::VrmlNamespace & vrmlNamespace,
                           const OpenVRML::Doc2 * doc,
                           OpenVRML::ScriptNode & node]
        {
            using OpenVRML::FieldValue;
            
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            FieldValue::Type ft(FieldValue::invalidType);
        }
    :   it=eventInterfaceType ft=fieldType id:ID
        {
            if (   (node.hasInterface(id->getText()) != FieldValue::invalidType)
                || (node.type.hasInterface(id->getText()) != FieldValue::invalidType)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
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
        }
    |   scriptFieldInterfaceDeclaration[vrmlNamespace, doc, node]
    ;

scriptFieldInterfaceDeclaration[OpenVRML::VrmlNamespace & vrmlNamespace,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node]
        {
            using OpenVRML::FieldValue;
            
            FieldValue::Type ft = FieldValue::invalidType;
            FieldValue * fv = 0;
        }
    :   KEYWORD_FIELD ft=fieldType id:ID fv=fieldValue[vrmlNamespace, doc, ft]
        {
            assert(fv);
            
            const std::auto_ptr<FieldValue> autofv(fv);
            
            if (   (node.hasInterface(id->getText()) != FieldValue::invalidType)
                || (node.type.hasInterface(id->getText()) != FieldValue::invalidType)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
            }
            
            node.addField(id->getText(), ft, autofv.get());
        }
    ;

protoNode[const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType,
          const std::string & nodeId]
returns [OpenVRML::NodePtr n]
options {
    defaultErrorHandler=false;
}
        {
            using OpenVRML::NodeTypePtr;
            using OpenVRML::NodePtr;
            using OpenVRML::ScriptNode;
            
            NodeTypePtr nodeType(0);
        }
    : 
	{ !LT(1)->getText().compare("Script") }?
	  scriptId:ID
        {
            nodeType = protoNodeType.getScope()->findType("Script");
            assert(nodeType);
            
            n = nodeType->newNode();
            
            if (!nodeId.empty()) {
                assert(protoNodeType.getScope());
                n->setId(nodeId, protoNodeType.getScope());
            }
            
            ScriptNode * const scriptNode = n->toScript();
            assert(scriptNode);
        }
        LBRACE (
            protoNodeBodyElement[doc, protoNodeType, *n]
            | protoScriptInterfaceDeclaration[doc, protoNodeType, *scriptNode]
        )* RBRACE

    | 	  nodeTypeId:ID
        {
            nodeType = protoNodeType.getScope()
                        ->findType(nodeTypeId->getText());
            if (!nodeType) {
                throw antlr::SemanticException("Unknown node type \""
                                               + nodeTypeId->getText() + "\".",
                                               std::string(), LT(0)->getLine());
            }
            
            n = NodePtr(nodeType->newNode());
            
            if (!nodeId.empty()) {
                assert(protoNodeType.getScope());
                n->setId(nodeId, protoNodeType.getScope());
            }
        }
        LBRACE (protoNodeBodyElement[doc, protoNodeType, *n])* RBRACE
    ;

protoNodeBodyElement[const OpenVRML::Doc2 * doc,
                     OpenVRML::NodeType & protoNodeType,
                     OpenVRML::Node & node]
        {
            using OpenVRML::FieldValue;
            
            FieldValue::Type ft(FieldValue::invalidType);
        }
    :   { node.type.hasEventIn(LT(1)->getText()) != FieldValue::invalidType ||
	  node.type.hasEventOut(LT(1)->getText()) != FieldValue::invalidType }?
      eventId:ID isStatement[protoNodeType, node, eventId->getText()]

    | id:ID 
        {
            if (   ((ft = node.type.hasField(id->getText())) == FieldValue::invalidType)
                && ((ft = node.type.hasExposedField(id->getText())) == FieldValue::invalidType)) {
                
                throw antlr::SemanticException(node.type.getId()
                                    + " node has no field or exposedField \""
                                    + id->getText() + "\".",
                                    std::string(), LT(0)->getLine());
            }
            
            FieldValue * fv = 0;
        }
        (
            (
                fv=protoFieldValue[doc, protoNodeType, ft]
                {
                    assert(fv);
                    node.setField(id->getText(), *fv);
                    delete fv;
                }
            )
            | isStatement[protoNodeType, node, id->getText()]
        )
    |   routeStatement[*protoNodeType.getScope()]
    |   protoStatement[*protoNodeType.getScope(), doc]
    ;

//
// As with the situation with ROUTEs, we probably want to move the code that 
// checks to see if the interface is valid and the types match into the
// implementation of NodeType.
// -- Braden McDaniel <braden@endoframe.com>, 8 Apr, 2000
//
isStatement[OpenVRML::NodeType & protoNodeType, OpenVRML::Node & node,
            std::string const & nodeInterfaceId]
    :   KEYWORD_IS id:ID
        {
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
        }
    ;

protoScriptInterfaceDeclaration[const OpenVRML::Doc2 * doc,
                                OpenVRML::NodeType & protoNodeType,
                                OpenVRML::ScriptNode & node]
        {
            using OpenVRML::FieldValue;
            
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            FieldValue::Type ft(FieldValue::invalidType);
        }
    :   it=eventInterfaceType ft=fieldType id:ID
        {
            if ((node.hasInterface(id->getText()) != FieldValue::invalidType)
                || (node.type.hasInterface(id->getText()) != FieldValue::invalidType)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
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
        }
        (isStatement[protoNodeType, node, id->getText()])?
    |   protoScriptFieldInterfaceDeclaration[doc, protoNodeType, node]
    ;

protoScriptFieldInterfaceDeclaration[const OpenVRML::Doc2 * doc,
                                     OpenVRML::NodeType & protoNodeType,
                                     OpenVRML::ScriptNode & node]
        {
            using OpenVRML::FieldValue;
            
            FieldValue::Type ft(FieldValue::invalidType);
            FieldValue * fv = 0;
        }
    :   KEYWORD_FIELD ft=fieldType id:ID
        {
            if ((node.hasInterface(id->getText()) != FieldValue::invalidType)
                || (node.type.hasInterface(id->getText()) != FieldValue::invalidType)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
            }
            
        }
        (
            (
                fv=protoFieldValue[doc, protoNodeType, ft]
                {
                    assert(fv);
                    node.addField(id->getText(), ft, fv);
                    delete fv;
                }
            )
            |
		{
                    node.addField(id->getText(), ft);
		}
		isStatement[protoNodeType, node, id->getText()]
        )
    ;

fieldType
returns [OpenVRML::FieldValue::Type ft = OpenVRML::FieldValue::invalidType]
        {
            using OpenVRML::FieldValue;
        }
    :   FIELDTYPE_MFCOLOR { ft = FieldValue::mfcolor; }
    |   FIELDTYPE_MFFLOAT { ft = FieldValue::mffloat; }
    |   FIELDTYPE_MFINT32 { ft = FieldValue::mfint32; }
    |   FIELDTYPE_MFNODE { ft = FieldValue::mfnode; }
    |   FIELDTYPE_MFROTATION { ft = FieldValue::mfrotation; }
    |   FIELDTYPE_MFSTRING { ft = FieldValue::mfstring; }
    |   FIELDTYPE_MFTIME { ft = FieldValue::mftime; }
    |   FIELDTYPE_MFVEC2F { ft = FieldValue::mfvec2f; }
    |   FIELDTYPE_MFVEC3F { ft = FieldValue::mfvec3f; }
    |   FIELDTYPE_SFBOOL { ft = FieldValue::sfbool; }
    |   FIELDTYPE_SFCOLOR { ft = FieldValue::sfcolor; }
    |   FIELDTYPE_SFFLOAT { ft = FieldValue::sffloat; }
    |   FIELDTYPE_SFIMAGE { ft = FieldValue::sfimage; }
    |   FIELDTYPE_SFINT32 { ft = FieldValue::sfint32; }
    |   FIELDTYPE_SFNODE { ft = FieldValue::sfnode; }
    |   FIELDTYPE_SFROTATION { ft = FieldValue::sfrotation; }
    |   FIELDTYPE_SFSTRING { ft = FieldValue::sfstring; }
    |   FIELDTYPE_SFTIME { ft = FieldValue::sftime; }
    |   FIELDTYPE_SFVEC2F { ft = FieldValue::sfvec2f; }
    |   FIELDTYPE_SFVEC3F { ft = FieldValue::sfvec3f; }
    ;

fieldValue[OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc,
           OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValue * fv = 0]
options { defaultErrorHandler=false; }
        {
            using OpenVRML::FieldValue;
        }
    :   {    (ft == FieldValue::sfnode)
          || (ft == FieldValue::mfnode) }? fv=nodeFieldValue[vrmlNamespace, doc, ft]
    |   fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType,
                OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValue * fv = 0]
        {
            using OpenVRML::FieldValue;
        }
    :   {    (ft == FieldValue::sfnode)
          || (ft == FieldValue::mfnode) }? fv=protoNodeFieldValue[doc, protoNodeType, ft]
        {
            assert(fv);
        }
    |   fv=nonNodeFieldValue[ft]
        {
            assert(fv);
        }
    ;

nonNodeFieldValue[OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValue * fv = 0]
options { defaultErrorHandler=false; }
        {
            using OpenVRML::FieldValue;
        }
    :   { ft == FieldValue::sfbool }? fv=sfBoolValue
    |   { ft == FieldValue::sfcolor }? fv=sfColorValue
    |   { ft == FieldValue::sffloat }? fv=sfFloatValue
    |   { ft == FieldValue::sfimage }? fv=sfImageValue
    |   { ft == FieldValue::sfint32 }? fv=sfInt32Value
    |   { ft == FieldValue::sfrotation }? fv=sfRotationValue
    |   { ft == FieldValue::sfstring }? fv=sfStringValue
    |   { ft == FieldValue::sftime }? fv=sfTimeValue
    |   { ft == FieldValue::sfvec2f }? fv=sfVec2fValue
    |   { ft == FieldValue::sfvec3f }? fv=sfVec3fValue
    |   { ft == FieldValue::mfcolor }? fv=mfColorValue
    |   { ft == FieldValue::mffloat }? fv=mfFloatValue
    |   { ft == FieldValue::mfint32 }? fv=mfInt32Value
    |   { ft == FieldValue::mfrotation }? fv=mfRotationValue
    |   { ft == FieldValue::mfstring }? fv=mfStringValue
    |   { ft == FieldValue::mftime }? fv=mfTimeValue
    |   { ft == FieldValue::mfvec2f }? fv=mfVec2fValue
    |   fv=mfVec3fValue
    ;

nodeFieldValue[OpenVRML::VrmlNamespace & vrmlNamespace,
               const OpenVRML::Doc2 * doc, OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValue * fv = 0]
options { defaultErrorHandler=false; }
        {
            using OpenVRML::FieldValue;
        }
    :   { ft == FieldValue::sfnode }? fv=sfNodeValue[vrmlNamespace, doc]
    |   fv=mfNodeValue[vrmlNamespace, doc]
    ;

protoNodeFieldValue[const OpenVRML::Doc2 * doc,
                    OpenVRML::NodeType & protoNodeType,
                    OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValue * fv = 0]
options { defaultErrorHandler=false; }
    :   { ft == OpenVRML::FieldValue::sfnode }?
            fv=protoSfNodeValue[doc, protoNodeType]
        {
            assert(fv);
        }
    |   fv=protoMfNodeValue[doc, protoNodeType]
        {
            assert(fv);
        }
    ;

sfBoolValue returns [OpenVRML::SFBool * sbv = new OpenVRML::SFBool()]
options { defaultErrorHandler=false; }
        {
            bool val(false);
        }
    :   val=boolValue { *sbv = OpenVRML::SFBool(val); }
    ;

boolValue returns [bool val]
options { defaultErrorHandler=false; }
        {
            val = false;
        }
    :   KEYWORD_TRUE { val = true; }
    |   KEYWORD_FALSE { val = false; }
    ;

sfColorValue returns [OpenVRML::SFColor * scv = new OpenVRML::SFColor()]
options { defaultErrorHandler=false; }
        {
            float c[3];
        }
    :   colorValue[c]
        {
            *scv = OpenVRML::SFColor(c[0], c[1], c[2]);
        }
    ;

mfColorValue returns [OpenVRML::MFColor * mcv = new OpenVRML::MFColor()]
options { defaultErrorHandler=false; }
        {
            float c[3];
        }
    :   colorValue[c]
        {
            *mcv = OpenVRML::MFColor(1, c);
        }
    |   LBRACKET
        {
            std::vector<float> colorVector;
        }
        (
            colorValue[c]
            {
                colorVector.push_back(c[0]);
                colorVector.push_back(c[1]);
                colorVector.push_back(c[2]);
            }
        )* RBRACKET
        {
            *mcv = OpenVRML::MFColor(colorVector.size() / 3L, &colorVector[0]);
        }
    ;

colorValue[float c[3]]
options { defaultErrorHandler=false; }
        {
            float r(0.0f), g(0.0f), b(0.0f);
        }
    :   r=colorComponent g=colorComponent b=colorComponent
        {
            c[0] = r;
            c[1] = g;
            c[2] = b;
        }
    ;

//
// colorComponent is defined separately from floatValue since, at some point, it
// might be useful to issue a warning if a color component is not in [0, 1].
//
colorComponent returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    :   val=floatValue
    ;

sfFloatValue returns [OpenVRML::SFFloat * sfv = new OpenVRML::SFFloat(0.0f)]
options { defaultErrorHandler=false; }
        {
            float f(0.0f);
        }
    :   f=floatValue
        {
            *sfv = OpenVRML::SFFloat(f);
        }
    ;

mfFloatValue returns [OpenVRML::MFFloat * mfv = new OpenVRML::MFFloat()]
options { defaultErrorHandler=false; }
        {
            float f(0.0f);
        }
    :   f=floatValue
        {
            *mfv = OpenVRML::MFFloat(1, &f);
        }
    |   LBRACKET
        {
            std::vector<float> floatVector;
        }
        (
            f=floatValue
            {
                floatVector.push_back(f);
            }
        )* RBRACKET
        {
            *mfv = OpenVRML::MFFloat(floatVector.size(), &floatVector[0]);
        }
    ;

floatValue returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    :   f0:REAL     { val = atof(f0->getText().c_str()); }
    |   f1:INTEGER  { val = atof(f1->getText().c_str()); }
    ;

sfImageValue returns [OpenVRML::SFImage * siv = new OpenVRML::SFImage()]
options { defaultErrorHandler=false; }
        {
            unsigned long w(0L), h(0L), com(0L), pixel(0L);
        }
    :   w=intValue h=intValue com=intValue
        {
            std::vector<unsigned char> pixelVector;
        }
        (
            pixel=intValue
            {
                // need to confirm the cross-platform-ness of this, it
                // looks kind of ugly but might in fact be ok. basically,
                // we read the value as an integer, then strip off the
                // bytes one by one.
		for (int i = com - 1; i >= 0; i--) {
                    pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
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
                throw antlr::SemanticException("Wrong number of pixel values for SFImage.",
                                               std::string(), LT(0)->getLine());
            }
            *siv = OpenVRML::SFImage(w, h, com, &pixelVector[0]); // hmmmm...
        }
    ;

sfInt32Value returns [OpenVRML::SFInt32 * siv = new OpenVRML::SFInt32(0L)]
options { defaultErrorHandler=false; }
        {
            long i(0L);
        }
    :   i=intValue
        {
            *siv = SFInt32(i);
        }
    ;

mfInt32Value returns [OpenVRML::MFInt32 * miv = new OpenVRML::MFInt32()]
options { defaultErrorHandler=false; }
        {
            long i(0L);
        }
    :   i=intValue
        {
            *miv = OpenVRML::MFInt32(1, &i);
        }
    |   LBRACKET
        {
            std::vector<long> longVector;
        }
        (
            i=intValue
            {
                longVector.push_back(i);
            }
        )* RBRACKET
        {
            *miv = OpenVRML::MFInt32(longVector.size(), &longVector[0]);
        }
    ;

intValue returns [long val = 0]
options { defaultErrorHandler=false; }
    :   i0:INTEGER { val = atol(i0->getText().c_str()); }
    |  	i1:HEX_INTEGER { val = strtol(i1->getText().c_str(), 0, 16); }
    ;

sfNodeValue[OpenVRML::VrmlNamespace & vrmlNamespace,
            const OpenVRML::Doc2 * doc]
returns [OpenVRML::SFNode * snv = new OpenVRML::SFNode()]
        {
            OpenVRML::NodePtr n;
        }
    :   n=nodeStatement[vrmlNamespace, doc]
        {
            *snv = OpenVRML::SFNode(n);
        }
    |   KEYWORD_NULL
    ;

protoSfNodeValue[const OpenVRML::Doc2 * doc,
                 OpenVRML::NodeType & protoNodeType]
returns [OpenVRML::SFNode * snv = new OpenVRML::SFNode()]
        {
            OpenVRML::NodePtr n;
        }
    :   n=protoNodeStatement[doc, protoNodeType]
        {
            *snv = OpenVRML::SFNode(n);
        }
    |   KEYWORD_NULL
    ;

mfNodeValue[OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc]
returns [OpenVRML::MFNode * mnv = new OpenVRML::MFNode()]
        {
            OpenVRML::NodePtr n;
        }
    : n=nodeStatement[vrmlNamespace, doc] { *mnv = OpenVRML::MFNode(1, &n); }
    | LBRACKET (n=nodeStatement[vrmlNamespace, doc] { mnv->addNode(*n); })*
        RBRACKET
    ;

protoMfNodeValue[const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType]
returns [OpenVRML::MFNode * mnv = new OpenVRML::MFNode()]
        {
            OpenVRML::NodePtr n;
        }
    : n=protoNodeStatement[doc, protoNodeType]
        { *mnv = OpenVRML::MFNode(1, &n); }
    | LBRACKET
        (n=protoNodeStatement[doc, protoNodeType] { mnv->addNode(*n); })*
        RBRACKET
    ;

sfRotationValue
returns [OpenVRML::SFRotation * srv = new OpenVRML::SFRotation()]
    { float r[4]; }
    : rotationValue[r] { *srv = OpenVRML::SFRotation(r); }
    ;

mfRotationValue
returns [OpenVRML::MFRotation * mrv = new OpenVRML::MFRotation()]
    { float r[4]; }
    : rotationValue[r] { *mrv = OpenVRML::MFRotation(1, r); }
    | LBRACKET { std::vector<float> floatVector; }
        (
            rotationValue[r]
            {
                floatVector.push_back(r[0]);
                floatVector.push_back(r[1]);
                floatVector.push_back(r[2]);
                floatVector.push_back(r[3]);
            }
        )* RBRACKET
        { *mrv = OpenVRML::MFRotation(floatVector.size() / 4L, &floatVector[0]); }
    ;

//
// Issue a warning here if the vector isn't normalized.
//
rotationValue[float r[4]]
options { defaultErrorHandler=false; }
    { float x(0.0f), y(0.0f), z(0.0f), rot(0.0f); }
    : x=floatValue y=floatValue z=floatValue rot=floatValue
        {
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
        }
    ;

sfStringValue returns [OpenVRML::SFString * ssv = new OpenVRML::SFString()]
options { defaultErrorHandler=false; }
    { std::string s; }
    : s=stringValue { *ssv = OpenVRML::SFString(s); }
    ;

mfStringValue returns [OpenVRML::MFString * msv = new OpenVRML::MFString()]
options { defaultErrorHandler=false; }
    { std::string s; }
    : s=stringValue { *msv = OpenVRML::MFString(1, &s); }
    | LBRACKET { std::vector<std::string> stringVector; }
        (s=stringValue { stringVector.push_back(s); })* RBRACKET
        { *msv = OpenVRML::MFString(stringVector.size(), &stringVector[0]); }
    ;

stringValue returns [std::string str]
options { defaultErrorHandler=false; }
    :   s:STRING
        {
            //
            // Why doesn't this work?
            //
            // str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
            
            std::string temp(s->getText());
            str = std::string(temp.begin() + 1, temp.end() - 1);
        }
    ;

sfTimeValue returns [OpenVRML::SFTime * stv = new OpenVRML::SFTime(0.0)]
options { defaultErrorHandler=false; }
    { double t(0.0); }
    : t=doubleValue { *stv = OpenVRML::SFTime(t); }
    ;

mfTimeValue returns [OpenVRML::MFTime * mtv = new OpenVRML::MFTime()]
options { defaultErrorHandler=false; }
    { double t(0.0); }
    : t=doubleValue { *mtv = MFTime(1, &t); }
    | LBRACKET { std::vector<double> doubleVector; }
        (t=doubleValue { doubleVector.push_back(t); })* RBRACKET
        { *mtv = OpenVRML::MFTime(doubleVector.size(), &doubleVector[0]); }
    ;

doubleValue returns [double val = 0.0]
options { defaultErrorHandler=false; }
    :   d0:REAL     { val = atof(d0->getText().c_str()); }
    |   d1:INTEGER  { val = atof(d1->getText().c_str()); }
    ;

sfVec2fValue returns [OpenVRML::SFVec2f * svv = new OpenVRML::SFVec2f()]
options { defaultErrorHandler=false; }
    { float v[2]; }
    : vec2fValue[v] { *svv = OpenVRML::SFVec2f(v[0], v[1]); }
    ;

mfVec2fValue returns [OpenVRML::MFVec2f * mvv = new OpenVRML::MFVec2f()]
options { defaultErrorHandler=false; }
    { float v[2]; }
    : vec2fValue[v] { *mvv = OpenVRML::MFVec2f(1, v); }
    | LBRACKET { std::vector<float> floatVector; }
        (
            vec2fValue[v]
            {
                floatVector.push_back(v[0]);
                floatVector.push_back(v[1]);
            }
        )* RBRACKET
        {
            *mvv = OpenVRML::MFVec2f(floatVector.size() / 2L, &floatVector[0]);
        }
    ;

vec2fValue[float v[2]]
options { defaultErrorHandler=false; }
        {
            float x(0.0f), y(0.0f);
        }
    :   x=floatValue y=floatValue
        {
            v[0] = x;
            v[1] = y;
        }
    ;

sfVec3fValue returns [OpenVRML::SFVec3f * svv = new OpenVRML::SFVec3f()]
options { defaultErrorHandler=false; }
        {
            float v[3];
        }
    :   vec3fValue[v]
        {
            *svv = OpenVRML::SFVec3f(v[0], v[1], v[2]);
        }
    ;

mfVec3fValue returns [OpenVRML::MFVec3f * mvv = new OpenVRML::MFVec3f()]
options { defaultErrorHandler=false; }
        {
            float v[3];
        }
    :   vec3fValue[v]
        {
            *mvv = OpenVRML::MFVec3f(1, v);
        }
    |   LBRACKET
        {
            std::vector<float> floatVector;
        }
        (
            vec3fValue[v]
            {
                floatVector.push_back(v[0]);
                floatVector.push_back(v[1]);
                floatVector.push_back(v[2]);
            }
        )* RBRACKET
        {
            *mvv = OpenVRML::MFVec3f(floatVector.size() / 3L, &floatVector[0]);
        }
    ;

vec3fValue[float v[3]]
options { defaultErrorHandler=false; }
        {
            float x(0.0f), y(0.0f), z(0.0f);
        }
    :   x=floatValue y=floatValue z=floatValue
        {
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }
    ;
