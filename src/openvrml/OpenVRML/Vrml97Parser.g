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
# include "fieldvalueptr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace OpenVRML {
    class VrmlNamespace;
    class ScriptNode;
    class NodeType;
    class Doc2;
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
        
        size_t line() const;
        size_t col() const;

    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);

        std::istream & _istm;
        size_t      _line;
        size_t      _col;
        int         _c;
        int         _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
    };
}

namespace OpenVRML ANTLR_LBRACE
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

} // namespace

namespace OpenVRML ANTLR_LBRACE

}

options {
    language="Cpp";
}

class Vrml97Parser extends Parser;
options {
    k=1;
    importVocab=Vrml97;
}

vrmlScene[OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc]
    :   (statement[scene, mfNode, vrmlNamespace, doc])*
    ;

statement[OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc]
    {
        OpenVRML::NodePtr node;
        OpenVRML::NodeTypePtr nodeType;
    }
    : node=nodeStatement[scene, vrmlNamespace, doc] {
            assert(node);
            mfNode.addNode(node);
        }
    | protoStatement[scene, vrmlNamespace, doc]
    | routeStatement[vrmlNamespace]
    ;

nodeStatement[OpenVRML::VrmlScene & scene,
              OpenVRML::VrmlNamespace & vrmlNamespace,
              const OpenVRML::Doc2 * doc] 
returns [OpenVRML::NodePtr n]
options { defaultErrorHandler=false; }
    :   KEYWORD_DEF id0:ID n=node[scene, vrmlNamespace, doc, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
            n.reset(vrmlNamespace.findNode(id1->getText()));
            if (!n) {
                throw antlr::SemanticException("Node \"" + id1->getText()
                                    + "\" has not been defined in this scope.",
                                    std::string(), LT(0)->getLine());
            }
        }
    |   n=node[scene, vrmlNamespace, doc, std::string()]
    ;

protoStatement[OpenVRML::VrmlScene & scene,
               OpenVRML::VrmlNamespace & scope,
               const OpenVRML::Doc2 * doc]
    //
    // XXX What if the node type already exists in the scope? Probably need to
    // XXX handle an exception here.
    //
    : externproto[scene, scope, doc]
    | proto[scene, scope, doc]
    ;

proto[OpenVRML::VrmlScene & scene,
      OpenVRML::VrmlNamespace & scope,
      const OpenVRML::Doc2 * doc]
{
    NodeClassPtr nodeClass;
}
    : KEYWORD_PROTO id:ID { nodeClass.reset(new ProtoNodeClass(scene)); }
        LBRACKET
            (protoInterfaceDeclaration[scope, doc, static_cast<ProtoNodeClass &>(*nodeClass)])*
        RBRACKET LBRACE protoBody[doc, static_cast<ProtoNodeClass &>(*nodeClass)] RBRACE
        {
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
        }
    ;

protoInterfaceDeclaration[OpenVRML::VrmlNamespace & scope,
                          const OpenVRML::Doc2 * doc,
                          OpenVRML::ProtoNodeClass & proto]
{
    using OpenVRML::NodeInterface;
    using antlr::SemanticException;
    
    NodeInterface::Type it(NodeInterface::invalidType);
    OpenVRML::FieldValue::Type ft(FieldValue::invalidType);
    OpenVRML::FieldValuePtr fv;
}
    : it=eventInterfaceType ft=fieldType id0:ID {
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
        }
    | it=fieldInterfaceType ft=fieldType id1:ID
        fv=fieldValue[proto.scene, scope, doc, ft] {
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
        }
    ;

eventInterfaceType returns [OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType]
    :   KEYWORD_EVENTIN  { it = OpenVRML::NodeInterface::eventIn; }
    |   KEYWORD_EVENTOUT { it = OpenVRML::NodeInterface::eventOut; }
    ;

fieldInterfaceType returns [OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType]
    :   KEYWORD_FIELD        { it = OpenVRML::NodeInterface::field; }
    |   KEYWORD_EXPOSEDFIELD { it = OpenVRML::NodeInterface::exposedField; }
    ;

protoBody[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto]
{
    OpenVRML::NodePtr n;
}
    : (protoStatement[proto.scene, proto.protoNode.scope, doc])*
        n=protoNodeStatement[doc, proto] { assert(n); proto.addRootNode(n); }
        (protoBodyStatement[doc, proto])*
    ;

protoBodyStatement[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto]
{
    OpenVRML::NodePtr n;
}
    : n=protoNodeStatement[doc, proto] { assert(n); proto.addRootNode(n); }
    | protoStatement[proto.scene, proto.protoNode.scope, doc]
    | routeStatement[proto.protoNode.scope]
    ;

protoNodeStatement[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto]
returns [OpenVRML::NodePtr n]
options { defaultErrorHandler=false; }
{
    using antlr::SemanticException;
}
    : KEYWORD_DEF id0:ID n=protoNode[doc, proto, id0->getText()]
    | KEYWORD_USE id1:ID {
            n.reset(proto.protoNode.scope.findNode(id1->getText()));
            if (!n) {
                throw SemanticException("Node \"" + id1->getText()
                                    + "\" has not been defined in this scope.",
                                    std::string(), LT(0)->getLine());
            }
        }
    | n=protoNode[doc, proto, std::string()]
    ;

externproto[OpenVRML::VrmlScene & scene, OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc]
{
    OpenVRML::NodeInterfaceSet interfaces;
    OpenVRML::MFString urlList;
    OpenVRML::NodeTypePtr nodeType;
}
    : KEYWORD_EXTERNPROTO id:ID LBRACKET
        (externInterfaceDeclaration[interfaces])* RBRACKET
        urlList=externprotoUrlList {
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
        }
    ;

externInterfaceDeclaration[OpenVRML::NodeInterfaceSet & interfaces]
        {
            using OpenVRML::NodeInterface;
            using OpenVRML::FieldValue;
            using antlr::SemanticException;
            NodeInterface::Type it(NodeInterface::invalidType);
            FieldValue::Type ft(FieldValue::invalidType);
        }
    :   it=interfaceType ft=fieldType id:ID {
            const NodeInterface interface(it, ft, id->getText());
            try {
                interfaces.add(interface);
            } catch (std::invalid_argument & ex) {
            	throw SemanticException(ex.what(), std::string(),
                                        LT(0)->getLine());
            }
        }
    ;

interfaceType
returns [OpenVRML::NodeInterface::Type it = OpenVRML::NodeInterface::invalidType]
    : it=eventInterfaceType
    | it=fieldInterfaceType
    ;

externprotoUrlList returns [OpenVRML::MFString urlList]
        {
            using std::string;
            using std::vector;
            using OpenVRML::MFString;
            
            string s;
        }
    :   s=stringValue { urlList = MFString(1, &s); }
    |   LBRACKET { vector<string> stringVector; }
        ( s=stringValue { stringVector.push_back(s); } )* RBRACKET
        { urlList = MFString(stringVector.size(), &stringVector[0]); }
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
        }
    ;

node[OpenVRML::VrmlScene & scene,
     OpenVRML::VrmlNamespace & scope,
     const OpenVRML::Doc2 * doc,
     const std::string & nodeId]
returns [OpenVRML::NodePtr n]
options { defaultErrorHandler = false; }
{
    using OpenVRML::NodeTypePtr;
    using OpenVRML::NodePtr;
    using OpenVRML::ScriptNode;
    using antlr::SemanticException;

    NodeTypePtr nodeType;
}
    : { !LT(1)->getText().compare("Script") }? scriptId:ID {
            n.reset(new ScriptNode(scene.scriptNodeClass));
            if (!nodeId.empty()) { n->setId(nodeId, &scope); }
            
            ScriptNode * const scriptNode = n->toScript();
            assert(scriptNode);
        } LBRACE (
            nodeBodyElement[scope, doc, *n]
            | scriptInterfaceDeclaration[scope, doc, *scriptNode]
        )* RBRACE
        
    | nodeTypeId:ID {
            nodeType = scope.findType(nodeTypeId->getText());
            if (!nodeType) {
                throw SemanticException("Unknown node type \""
                                        + nodeTypeId->getText() + "\".",
                                        std::string(), LT(0)->getLine());
            }
            
            n = NodePtr(nodeType->createNode());
            
            if (!nodeId.empty()) { n->setId(nodeId, &scope); }
        } LBRACE (nodeBodyElement[scope, doc, *n])* RBRACE
    ;

nodeBodyElement[OpenVRML::VrmlNamespace & scope,
                const OpenVRML::Doc2 * doc,
                OpenVRML::Node & node]
{
    using OpenVRML::FieldValue;
    using antlr::SemanticException;
    FieldValue::Type ft(FieldValue::invalidType);
    FieldValuePtr fv;
}
    : id:ID {
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
        }
        fv=fieldValue[node.nodeType.nodeClass.scene, scope, doc, ft] {
            assert(fv);
            node.setField(id->getText(), *fv);
        }
    |   routeStatement[scope]
    |   protoStatement[node.nodeType.nodeClass.scene, scope, doc]
    ;

scriptInterfaceDeclaration[OpenVRML::VrmlNamespace & scope,
                           const OpenVRML::Doc2 * doc,
                           OpenVRML::ScriptNode & node]
    {
        using OpenVRML::NodeInterface;
        using OpenVRML::FieldValue;
        using antlr::SemanticException;
        NodeInterface::Type it(NodeInterface::invalidType);
        FieldValue::Type ft(FieldValue::invalidType);
    }
    : it=eventInterfaceType ft=fieldType id:ID {
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
        }
    | scriptFieldInterfaceDeclaration[scope, doc, node]
    ;

scriptFieldInterfaceDeclaration[OpenVRML::VrmlNamespace & scope,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node]
{
    using OpenVRML::FieldValue;
    using antlr::SemanticException;

    FieldValue::Type ft = FieldValue::invalidType;
    FieldValuePtr fv;
}
    : KEYWORD_FIELD ft=fieldType id:ID
        fv=fieldValue[node.nodeType.nodeClass.scene, scope, doc, ft] {
            assert(fv);
            if (node.nodeType.hasInterface(id->getText())
                    != FieldValue::invalidType) {
                throw SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
            }
            node.addField(id->getText(), fv);
        }
    ;

protoNode[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto,
          const std::string & nodeId]
returns [OpenVRML::NodePtr n]
options { defaultErrorHandler=false; }
{
    using OpenVRML::NodeTypePtr;
    using OpenVRML::NodePtr;
    using OpenVRML::ScriptNode;
    using antlr::SemanticException;

    NodeTypePtr nodeType;
}
    : { !LT(1)->getText().compare("Script") }? scriptId:ID {
            n.reset(new ScriptNode(proto.scene.scriptNodeClass));
            if (!nodeId.empty()) { n->setId(nodeId, &proto.protoNode.scope); }
            
            ScriptNode * const scriptNode = n->toScript();
            assert(scriptNode);
        }
        LBRACE (
            protoNodeBodyElement[doc, proto, *n]
            | protoScriptInterfaceDeclaration[doc, proto, *scriptNode]
        )* RBRACE

    | nodeTypeId:ID {
            nodeType = proto.protoNode.scope.findType(nodeTypeId->getText());
            if (!nodeType) {
                throw SemanticException("Unknown node type \""
                                        + nodeTypeId->getText() + "\".",
                                        std::string(), LT(0)->getLine());
            }
            n = nodeType->createNode();
            if (!nodeId.empty()) { n->setId(nodeId, &proto.protoNode.scope); }
        }
        LBRACE (protoNodeBodyElement[doc, proto, *n])* RBRACE
    ;

protoNodeBodyElement[const OpenVRML::Doc2 * doc,
                     OpenVRML::ProtoNodeClass & proto,
                     OpenVRML::Node & node]
{
    using OpenVRML::FieldValue;
    using antlr::SemanticException;

    FieldValue::Type ft(FieldValue::invalidType);
    FieldValuePtr fv;
}
    : { node.nodeType.hasField(LT(1)->getText())
        || node.nodeType.hasExposedField(LT(1)->getText()) }? id:ID {
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
        } (
            (
                fv=protoFieldValue[doc, proto, ft] {
                    assert(fv);
                    node.setField(id->getText(), *fv);
                }
            )
            | isStatement[proto, node, id->getText()]
        )
    | eventId:ID isStatement[proto, node, eventId->getText()]
    | routeStatement[proto.protoNode.scope]
    | protoStatement[proto.scene, proto.protoNode.scope, doc]
    ;

isStatement[OpenVRML::ProtoNodeClass & proto, OpenVRML::Node & node,
            std::string const & nodeInterfaceId]
    : KEYWORD_IS id:ID {
            try {
                proto.addIS(node, nodeInterfaceId, id->getText());
            } catch (std::invalid_argument & ex) {
                throw antlr::SemanticException(ex.what(), std::string(),
                                               LT(0)->getLine());
            }
        }
    ;

protoScriptInterfaceDeclaration[const OpenVRML::Doc2 * doc,
                                OpenVRML::ProtoNodeClass & proto,
                                OpenVRML::ScriptNode & node]
{
    using OpenVRML::NodeInterface;
    using OpenVRML::FieldValue;
    using antlr::SemanticException;
    NodeInterface::Type it(NodeInterface::invalidType);
    FieldValue::Type ft(FieldValue::invalidType);
}
    : it=eventInterfaceType ft=fieldType id:ID {
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
        } (isStatement[proto, node, id->getText()])?
    |   protoScriptFieldInterfaceDeclaration[doc, proto, node]
    ;

protoScriptFieldInterfaceDeclaration[const OpenVRML::Doc2 * doc,
                                     OpenVRML::ProtoNodeClass & proto,
                                     OpenVRML::ScriptNode & node]
{
    using OpenVRML::FieldValue;
    using antlr::SemanticException;

    FieldValue::Type ft(FieldValue::invalidType);
    FieldValuePtr fv;
}
    : KEYWORD_FIELD ft=fieldType id:ID {
            if (node.nodeType.hasInterface(id->getText())) {
                throw SemanticException("Interface \"" + id->getText()
                                    + "\" already declared for Script node.",
                                    std::string(), LT(0)->getLine());
            }
            
        } (
            (
                fv=protoFieldValue[doc, proto, ft] {
                    assert(fv);
                    node.addField(id->getText(), fv);
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
                }
            )
        )
    ;

fieldType
returns [OpenVRML::FieldValue::Type ft = OpenVRML::FieldValue::invalidType]
{
    using OpenVRML::FieldValue;
}
    : FIELDTYPE_MFCOLOR     { ft = FieldValue::mfcolor; }
    | FIELDTYPE_MFFLOAT     { ft = FieldValue::mffloat; }
    | FIELDTYPE_MFINT32     { ft = FieldValue::mfint32; }
    | FIELDTYPE_MFNODE      { ft = FieldValue::mfnode; }
    | FIELDTYPE_MFROTATION  { ft = FieldValue::mfrotation; }
    | FIELDTYPE_MFSTRING    { ft = FieldValue::mfstring; }
    | FIELDTYPE_MFTIME      { ft = FieldValue::mftime; }
    | FIELDTYPE_MFVEC2F     { ft = FieldValue::mfvec2f; }
    | FIELDTYPE_MFVEC3F     { ft = FieldValue::mfvec3f; }
    | FIELDTYPE_SFBOOL      { ft = FieldValue::sfbool; }
    | FIELDTYPE_SFCOLOR     { ft = FieldValue::sfcolor; }
    | FIELDTYPE_SFFLOAT     { ft = FieldValue::sffloat; }
    | FIELDTYPE_SFIMAGE     { ft = FieldValue::sfimage; }
    | FIELDTYPE_SFINT32     { ft = FieldValue::sfint32; }
    | FIELDTYPE_SFNODE      { ft = FieldValue::sfnode; }
    | FIELDTYPE_SFROTATION  { ft = FieldValue::sfrotation; }
    | FIELDTYPE_SFSTRING    { ft = FieldValue::sfstring; }
    | FIELDTYPE_SFTIME      { ft = FieldValue::sftime; }
    | FIELDTYPE_SFVEC2F     { ft = FieldValue::sfvec2f; }
    | FIELDTYPE_SFVEC3F     { ft = FieldValue::sfvec3f; }
    ;

fieldValue[OpenVRML::VrmlScene & scene,
           OpenVRML::VrmlNamespace & scope,
           const OpenVRML::Doc2 * doc,
           OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValuePtr fv]
options { defaultErrorHandler=false; }
{
    using OpenVRML::FieldValue;
}
    : { (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) }?
        fv=nodeFieldValue[scene, scope, doc, ft]
    | fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[const OpenVRML::Doc2 * doc,
                OpenVRML::ProtoNodeClass & proto,
                OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValuePtr fv]
{
    using OpenVRML::FieldValue;
}
    : { (ft == FieldValue::sfnode) || (ft == FieldValue::mfnode) }?
        fv=protoNodeFieldValue[doc, proto, ft] { assert(fv); }
    | fv=nonNodeFieldValue[ft] { assert(fv); }
    ;

nonNodeFieldValue[OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValuePtr fv]
options { defaultErrorHandler=false; }
{
    using OpenVRML::FieldValue;
}
    : { ft == FieldValue::sfbool }? fv=sfBoolValue
    | { ft == FieldValue::sfcolor }? fv=sfColorValue
    | { ft == FieldValue::sffloat }? fv=sfFloatValue
    | { ft == FieldValue::sfimage }? fv=sfImageValue
    | { ft == FieldValue::sfint32 }? fv=sfInt32Value
    | { ft == FieldValue::sfrotation }? fv=sfRotationValue
    | { ft == FieldValue::sfstring }? fv=sfStringValue
    | { ft == FieldValue::sftime }? fv=sfTimeValue
    | { ft == FieldValue::sfvec2f }? fv=sfVec2fValue
    | { ft == FieldValue::sfvec3f }? fv=sfVec3fValue
    | { ft == FieldValue::mfcolor }? fv=mfColorValue
    | { ft == FieldValue::mffloat }? fv=mfFloatValue
    | { ft == FieldValue::mfint32 }? fv=mfInt32Value
    | { ft == FieldValue::mfrotation }? fv=mfRotationValue
    | { ft == FieldValue::mfstring }? fv=mfStringValue
    | { ft == FieldValue::mftime }? fv=mfTimeValue
    | { ft == FieldValue::mfvec2f }? fv=mfVec2fValue
    | fv=mfVec3fValue
    ;

nodeFieldValue[OpenVRML::VrmlScene & scene,
               OpenVRML::VrmlNamespace & scope,
               const OpenVRML::Doc2 * doc,
               OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValuePtr fv]
options { defaultErrorHandler=false; }
{
    using OpenVRML::FieldValue;
}
    : { ft == FieldValue::sfnode }? fv=sfNodeValue[scene, scope, doc]
    | fv=mfNodeValue[scene, scope, doc]
    ;

protoNodeFieldValue[const OpenVRML::Doc2 * doc,
                    OpenVRML::ProtoNodeClass & proto,
                    OpenVRML::FieldValue::Type ft]
returns [OpenVRML::FieldValuePtr fv]
options { defaultErrorHandler=false; }
    : { ft == OpenVRML::FieldValue::sfnode }? fv=protoSfNodeValue[doc, proto]
    | fv=protoMfNodeValue[doc, proto]
    ;

sfBoolValue returns [OpenVRML::FieldValuePtr sbv]
options { defaultErrorHandler=false; }
{
    bool val(false);
}
    : val=boolValue { sbv.reset(new SFBool(val)); }
    ;

boolValue returns [bool val = false]
options { defaultErrorHandler=false; }
    : KEYWORD_TRUE { val = true; }
    | KEYWORD_FALSE { val = false; }
    ;

sfColorValue returns [OpenVRML::FieldValuePtr scv]
options { defaultErrorHandler=false; }
{
    float c[3];
}
    : colorValue[c] { scv.reset(new SFColor(c[0], c[1], c[2])); }
    ;

mfColorValue returns [OpenVRML::FieldValuePtr mcv]
options { defaultErrorHandler=false; }
{
    float c[3];
}
    : colorValue[c] { mcv.reset(new MFColor(1, c)); }
    | LBRACKET { std::vector<float> colorVector; }
        (colorValue[c] { colorVector.push_back(c[0]);
                         colorVector.push_back(c[1]);
                         colorVector.push_back(c[2]); })* RBRACKET
        { mcv.reset(new MFColor(colorVector.size() / 3L, &colorVector[0])); }
    ;

colorValue[float c[3]]
options { defaultErrorHandler=false; }
{
    float r(0.0f), g(0.0f), b(0.0f);
}
    : r=colorComponent g=colorComponent b=colorComponent { c[0] = r;
                                                           c[1] = g;
                                                           c[2] = b; }
    ;

//
// Issue a warning if a color component is not in [0, 1].
//
colorComponent returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    : val=floatValue {
            if (val < 0.0 || val > 1.0) {
                this->reportWarning("Color component values must be from 0 to 1.");
            }
        }
    ;

sfFloatValue returns [OpenVRML::FieldValuePtr sfv]
options { defaultErrorHandler=false; }
{
    float f(0.0f);
}
    : f=floatValue { sfv.reset(new SFFloat(f)); }
    ;

mfFloatValue returns [OpenVRML::FieldValuePtr mfv]
options { defaultErrorHandler=false; }
{
    float f(0.0f);
}
    : f=floatValue { mfv.reset(new MFFloat(1, &f)); }
    | LBRACKET { std::vector<float> floatVector; }
        (f=floatValue { floatVector.push_back(f); })* RBRACKET
        { mfv.reset(new MFFloat(floatVector.size(), &floatVector[0])); }
    ;

floatValue returns [float val = 0.0f]
options { defaultErrorHandler=false; }
    : f0:REAL     { val = atof(f0->getText().c_str()); }
    | f1:INTEGER  { val = atof(f1->getText().c_str()); }
    ;

sfImageValue returns [OpenVRML::FieldValuePtr siv]
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
            siv.reset(new SFImage(w, h, com, &pixelVector[0]));
        }
    ;

sfInt32Value returns [OpenVRML::FieldValuePtr siv]
options { defaultErrorHandler=false; }
{
    long i(0L);
}
    : i=intValue { siv.reset(new SFInt32(i)); }
    ;

mfInt32Value returns [OpenVRML::FieldValuePtr miv]
options { defaultErrorHandler=false; }
{
    long i(0L);
}
    : i=intValue { miv.reset(new MFInt32(1, &i)); }
    | LBRACKET { std::vector<long> longVector; }
        (i=intValue { longVector.push_back(i); })* RBRACKET
        { miv.reset(new MFInt32(longVector.size(), &longVector[0])); }
    ;

intValue returns [long val = 0]
options { defaultErrorHandler=false; }
    :   i0:INTEGER      { val = atol(i0->getText().c_str()); }
    |  	i1:HEX_INTEGER  { val = strtol(i1->getText().c_str(), 0, 16); }
    ;

sfNodeValue[OpenVRML::VrmlScene & scene,
            OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc]
returns [OpenVRML::FieldValuePtr snv]
{
    OpenVRML::NodePtr n;
}
    : n=nodeStatement[scene, scope, doc] { snv.reset(new SFNode(n)); }
    | KEYWORD_NULL                       { snv.reset(new SFNode); }
    ;

protoSfNodeValue[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto]
returns [OpenVRML::FieldValuePtr snv]
{
    OpenVRML::NodePtr n;
}
    : n=protoNodeStatement[doc, proto] { snv.reset(new SFNode(n)); }
    | KEYWORD_NULL                     { snv.reset(new SFNode); }
    ;

mfNodeValue[OpenVRML::VrmlScene & scene,
            OpenVRML::VrmlNamespace & scope,
            const OpenVRML::Doc2 * doc]
returns [OpenVRML::FieldValuePtr mnv]
    { OpenVRML::NodePtr n; }
    : n=nodeStatement[scene, scope, doc] { mnv.reset(new MFNode(1, &n)); }
    | LBRACKET { mnv.reset(new MFNode); } (
            n=nodeStatement[scene, scope, doc] {
                static_cast<MFNode &>(*mnv).addNode(n);
            }
        )* RBRACKET
    ;

protoMfNodeValue[const OpenVRML::Doc2 * doc, OpenVRML::ProtoNodeClass & proto]
returns [OpenVRML::FieldValuePtr mnv]
    { OpenVRML::NodePtr n; }
    : n=protoNodeStatement[doc, proto] { mnv.reset(new MFNode(1, &n)); }
    | LBRACKET { mnv.reset(new MFNode); } (
            n=protoNodeStatement[doc, proto] {
                static_cast<MFNode &>(*mnv).addNode(n);
            }
        )* RBRACKET
    ;

sfRotationValue returns [OpenVRML::FieldValuePtr srv]
    { float r[4]; }
    : rotationValue[r] { srv.reset(new SFRotation(r)); }
    ;

mfRotationValue returns [OpenVRML::FieldValuePtr mrv]
    { float r[4]; }
    : rotationValue[r] { mrv.reset(new MFRotation(1, r)); }
    | LBRACKET { std::vector<float> floatVector; }
        (rotationValue[r] { floatVector.push_back(r[0]);
                            floatVector.push_back(r[1]);
                            floatVector.push_back(r[2]);
                            floatVector.push_back(r[3]); })* RBRACKET
        { mrv.reset(new MFRotation(floatVector.size() / 4L, &floatVector[0])); }
    ;

//
// Issue a warning here if the vector isn't normalized.
//
rotationValue[float r[4]]
options { defaultErrorHandler=false; }
    {
        using OpenVRML_::length;
        using OpenVRML_::fpequal;
        using OpenVRML_::normalize;
        float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
    }
    : x=floatValue y=floatValue z=floatValue rot=floatValue {
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

sfStringValue returns [OpenVRML::FieldValuePtr ssv]
options { defaultErrorHandler=false; }
    { std::string s; }
    : s=stringValue { ssv.reset(new SFString(s)); }
    ;

mfStringValue returns [OpenVRML::FieldValuePtr msv]
options { defaultErrorHandler=false; }
    { std::string s; }
    : s=stringValue { msv.reset(new MFString(1, &s)); }
    | LBRACKET { std::vector<std::string> stringVector; }
        (s=stringValue { stringVector.push_back(s); })* RBRACKET
        { msv.reset(new MFString(stringVector.size(), &stringVector[0])); }
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

sfTimeValue returns [OpenVRML::FieldValuePtr stv]
options { defaultErrorHandler=false; }
    { double t(0.0); }
    : t=doubleValue { stv.reset(new SFTime(t)); }
    ;

mfTimeValue returns [OpenVRML::FieldValuePtr mtv]
options { defaultErrorHandler=false; }
    { double t(0.0); }
    : t=doubleValue { mtv.reset(new MFTime(1, &t)); }
    | LBRACKET { std::vector<double> doubleVector; }
        (t=doubleValue { doubleVector.push_back(t); })* RBRACKET
        { mtv.reset(new MFTime(doubleVector.size(), &doubleVector[0])); }
    ;

doubleValue returns [double val = 0.0]
options { defaultErrorHandler=false; }
    :   d0:REAL     { val = atof(d0->getText().c_str()); }
    |   d1:INTEGER  { val = atof(d1->getText().c_str()); }
    ;

sfVec2fValue returns [OpenVRML::FieldValuePtr svv]
options { defaultErrorHandler=false; }
    { float v[2]; }
    : vec2fValue[v] { svv.reset(new SFVec2f(v[0], v[1])); }
    ;

mfVec2fValue returns [OpenVRML::FieldValuePtr mvv]
options { defaultErrorHandler=false; }
    { float v[2]; }
    : vec2fValue[v] { mvv.reset(new MFVec2f(1, v)); }
    | LBRACKET { std::vector<float> floatVector; }
        (vec2fValue[v] { floatVector.push_back(v[0]);
                         floatVector.push_back(v[1]); })* RBRACKET
        { mvv.reset(new MFVec2f(floatVector.size() / 2L, &floatVector[0])); }
    ;

vec2fValue[float v[2]]
options { defaultErrorHandler=false; }
{
    float x(0.0f), y(0.0f);
}
    : x=floatValue y=floatValue { v[0] = x;
                                  v[1] = y; }
    ;

sfVec3fValue returns [OpenVRML::FieldValuePtr svv]
options { defaultErrorHandler=false; }
{
    float v[3];
}
    : vec3fValue[v] { svv.reset(new SFVec3f(v[0], v[1], v[2])); }
    ;

mfVec3fValue returns [OpenVRML::FieldValuePtr mvv]
options { defaultErrorHandler=false; }
{
    float v[3];
}
    : vec3fValue[v] { mvv.reset(new MFVec3f(1, v)); }
    | LBRACKET { std::vector<float> floatVector; }
        (vec3fValue[v] { floatVector.push_back(v[0]);
                         floatVector.push_back(v[1]);
                         floatVector.push_back(v[2]); })* RBRACKET
        { mvv.reset(new MFVec3f(floatVector.size() / 3L, &floatVector[0])); }
    ;

vec3fValue[float v[3]]
options { defaultErrorHandler=false; }
{
    float x(0.0f), y(0.0f), z(0.0f);
}
    : x=floatValue y=floatValue z=floatValue { v[0] = x;
                                               v[1] = y;
                                               v[2] = z; }
    ;
