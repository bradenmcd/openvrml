/*
LibVRML97: a library for loading and viewing VRML

The authors make NO WARRANTY or representation, either express or implied, with
respect to this software, its quality, accuracy, merchantability, or fitness for
a particular purpose.  This software is provided "AS IS", and you, its user,
assume the entire risk as to its quality and accuracy.

This software is copyright (C) the Open VRML Advancement League
All Rights Reserved except as specified below.

Permission is hereby granted to use, copy, modify, and distribute this software
(or portions thereof) for any purpose, without fee, subject to these conditions:

 (1) If any part of the source code for this software is distributed, then this
    file must be included, with this copyright and no-warranty notice unaltered;
    and any additions, deletions, or changes to the original files must be
    clearly indicated in accompanying documentation.

 (2) If only executable code is distributed, then the accompanying documentation
    must state that "this software is based in part on the LibVRML97 library".

 (3) Permission for use of this software is granted only if the user accepts
    full responsibility for any undesirable consequences; the authors accept NO
    LIABILITY for damages of any kind.

These conditions apply to any software derived from or based on the LibVRML97
code, not just to the unmodified library.  If you use our work, you ought to
acknowledge us.

We specifically permit and encourage the use of this software as the basis of
commercial products, provided that all warranty or liability claims are assumed
by the product vendor.
*/

# include <cassert>
# include <iostream.h>
# include "vrml97scanner.hpp"
# include "antlr/CommonToken.hpp"

namespace {
    bool isValidIdFirstChar(char);
    bool isValidIdRestChars(char);
    bool isWhitespaceChar(char);
    bool isNewlineChar(char);
    bool isHexDigit(char);
}

int const Vrml97Utf8Scanner::EOF_                   (antlr::Token::EOF_TYPE);

//
// The values and names for these constants *must* match those expressed in the
// file Vrml97TokenTypes.txt.
//
int const Vrml97Utf8Scanner::PERIOD                 (4);
int const Vrml97Utf8Scanner::LBRACKET               (5);
int const Vrml97Utf8Scanner::RBRACKET               (6);
int const Vrml97Utf8Scanner::LBRACE                 (7);
int const Vrml97Utf8Scanner::RBRACE                 (8);
int const Vrml97Utf8Scanner::ID                     (9);
int const Vrml97Utf8Scanner::INTEGER                (10);
int const Vrml97Utf8Scanner::REAL                   (11);
int const Vrml97Utf8Scanner::STRING                 (12);
int const Vrml97Utf8Scanner::KEYWORD_DEF            (13);
int const Vrml97Utf8Scanner::KEYWORD_EVENTIN        (14);
int const Vrml97Utf8Scanner::KEYWORD_EVENTOUT       (15);
int const Vrml97Utf8Scanner::KEYWORD_EXPOSEDFIELD   (16);
int const Vrml97Utf8Scanner::KEYWORD_EXTERNPROTO    (17);
int const Vrml97Utf8Scanner::KEYWORD_FALSE          (18);
int const Vrml97Utf8Scanner::KEYWORD_FIELD          (19);
int const Vrml97Utf8Scanner::KEYWORD_IS             (20);
int const Vrml97Utf8Scanner::KEYWORD_NULL           (21);
int const Vrml97Utf8Scanner::KEYWORD_PROTO          (22);
int const Vrml97Utf8Scanner::KEYWORD_ROUTE          (23);
int const Vrml97Utf8Scanner::KEYWORD_TO             (24);
int const Vrml97Utf8Scanner::KEYWORD_TRUE           (25);
int const Vrml97Utf8Scanner::KEYWORD_USE            (26);
int const Vrml97Utf8Scanner::NODE_SCRIPT            (27);
int const Vrml97Utf8Scanner::FIELDTYPE_SFBOOL       (28);
int const Vrml97Utf8Scanner::FIELDTYPE_SFCOLOR      (29);
int const Vrml97Utf8Scanner::FIELDTYPE_SFFLOAT      (30);
int const Vrml97Utf8Scanner::FIELDTYPE_SFIMAGE      (31);
int const Vrml97Utf8Scanner::FIELDTYPE_SFINT32      (32);
int const Vrml97Utf8Scanner::FIELDTYPE_SFNODE       (33);
int const Vrml97Utf8Scanner::FIELDTYPE_SFROTATION   (34);
int const Vrml97Utf8Scanner::FIELDTYPE_SFSTRING     (35);
int const Vrml97Utf8Scanner::FIELDTYPE_SFTIME       (36);
int const Vrml97Utf8Scanner::FIELDTYPE_SFVEC2F      (37);
int const Vrml97Utf8Scanner::FIELDTYPE_SFVEC3F      (38);
int const Vrml97Utf8Scanner::FIELDTYPE_MFCOLOR      (39);
int const Vrml97Utf8Scanner::FIELDTYPE_MFFLOAT      (40);
int const Vrml97Utf8Scanner::FIELDTYPE_MFINT32      (41);
int const Vrml97Utf8Scanner::FIELDTYPE_MFNODE       (42);
int const Vrml97Utf8Scanner::FIELDTYPE_MFROTATION   (43);
int const Vrml97Utf8Scanner::FIELDTYPE_MFSTRING     (44);
int const Vrml97Utf8Scanner::FIELDTYPE_MFTIME       (45);
int const Vrml97Utf8Scanner::FIELDTYPE_MFVEC2F      (46);
int const Vrml97Utf8Scanner::FIELDTYPE_MFVEC3F      (47);

Vrml97Utf8Scanner::Vrml97Utf8Scanner(::istream & istm)
  : _istm(istm), _line(1), _col(0), _c(' '), _prevChar('\0'), _prevTokenType(0),
    _readTooMuch(false), _expectingFieldType(false)
{}

antlr::RefToken Vrml97Utf8Scanner::nextToken()
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
        while ((_c != '"') && (prevChar != '\\')) {
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

std::size_t Vrml97Utf8Scanner::line() const
{
    return _line;
}

std::size_t Vrml97Utf8Scanner::col() const
{
    return _col;
}

void Vrml97Utf8Scanner::_getNextChar() {
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

void Vrml97Utf8Scanner::_identifyKeyword(antlr::Token & token)
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

void Vrml97Utf8Scanner::_identifyFieldType(antlr::Token & token)
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

void Vrml97Utf8Scanner::_identifyTerminalSymbol(antlr::Token & token)
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
