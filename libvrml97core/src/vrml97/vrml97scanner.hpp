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

# ifndef VRML97SCANNER_HPP
# define VRML97SCANNER_HPP

# include <cstddef>
# include <string>
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
        std::size_t line() const;
        std::size_t col() const;
        
    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyScriptNode(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);
        
        istream &   _istm;
        std::size_t _line;
        std::size_t _col;
        char        _c;
        char        _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
        bool        _expectingNodeTypeId;
};

# endif
