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

# ifndef VRML97SCANNER_HPP
# define VRML97SCANNER_HPP

# include <string>
# include <stddef.h>
# include <iostream.h>
# include "antlr/TokenStream.hpp"

class Vrml97Utf8Scanner : public antlr::TokenStream {
    public:
        static int const EOF_;
        //
        // The following identifiers for constants must match those in the file
        // Vrml97TokenTypes.txt.
        //
        static int const PERIOD;
        static int const LBRACKET;
        static int const RBRACKET;
        static int const LBRACE;
        static int const RBRACE;
        static int const ID;
        static int const INTEGER;
        static int const REAL;
        static int const STRING;
        static int const KEYWORD_DEF;
        static int const KEYWORD_EVENTIN;
        static int const KEYWORD_EVENTOUT;
        static int const KEYWORD_EXPOSEDFIELD;
        static int const KEYWORD_EXTERNPROTO;
        static int const KEYWORD_FALSE;
        static int const KEYWORD_FIELD;
        static int const KEYWORD_IS;
        static int const KEYWORD_NULL;
        static int const KEYWORD_PROTO;
        static int const KEYWORD_ROUTE;
        static int const KEYWORD_TO;
        static int const KEYWORD_TRUE;
        static int const KEYWORD_USE;
        static int const NODE_SCRIPT;
        static int const FIELDTYPE_SFBOOL;
        static int const FIELDTYPE_SFCOLOR;
        static int const FIELDTYPE_SFFLOAT;
        static int const FIELDTYPE_SFIMAGE;
        static int const FIELDTYPE_SFINT32;
        static int const FIELDTYPE_SFNODE;
        static int const FIELDTYPE_SFROTATION;
        static int const FIELDTYPE_SFSTRING;
        static int const FIELDTYPE_SFTIME;
        static int const FIELDTYPE_SFVEC2F;
        static int const FIELDTYPE_SFVEC3F;
        static int const FIELDTYPE_MFCOLOR;
        static int const FIELDTYPE_MFFLOAT;
        static int const FIELDTYPE_MFINT32;
        static int const FIELDTYPE_MFNODE;
        static int const FIELDTYPE_MFROTATION;
        static int const FIELDTYPE_MFSTRING;
        static int const FIELDTYPE_MFTIME;
        static int const FIELDTYPE_MFVEC2F;
        static int const FIELDTYPE_MFVEC3F;
        
        Vrml97Utf8Scanner(istream &);
        
        antlr::RefToken nextToken();
        size_t line() const;
        size_t col() const;
        
    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);
        
        istream &   _istm;
        size_t _line;
        size_t _col;
        int         _c;
        int         _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
};

# endif
