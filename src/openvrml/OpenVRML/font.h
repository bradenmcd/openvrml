//
// OpenVRML
//
// Copyright (C) 2002  S. K. Bose
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

#ifndef OPENVRML_FONT_H
#define OPENVRML_FONT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef OPENVRML_HAVE_FREETYPEFONTS

#include "common.h"
#include <map>
#include <vector>
#include <string>
#include <ft2build.h>

#include FT_FREETYPE_H  
#include FT_GLYPH_H
#include FT_OUTLINE_H 


namespace OpenVRML {

struct Vertex {
         float V[2];
};

class FontVectoriser;

typedef std::vector<Vertex> Contour;
typedef std::map<FT_UInt,FontVectoriser *> GlyphContainer;

    class FontLibrary {
      public:
        // Access point to Single FT_Library object
        static FT_Library & Instance();
        ~FontLibrary(void) throw ();
      private:
        // make private constructor
        FontLibrary(void) throw (std::bad_alloc);
        static FontLibrary fontlib;
        static FT_Library lib;
    };


    class FontFace {
      public:
         // Default constructor
         FontFace() throw ();
         virtual ~FontFace() throw ();
         void setJustify(size_t length, const std::string* justify)
                         throw (std::bad_alloc);
         void setHorizontal(bool horizontal) throw ();
         void setLeftToRight(bool leftToRight) throw ();
         void setTopToBottom(bool topToBottom) throw ();
         void setSpacing(float spacing) throw ();
         void setLanguage(const std::string &) throw ();
         void openFace(size_t family_size, const std::string* family, 
                       const std::string & style, float size) throw (std::bad_alloc);
         bool getError(void) const throw ();
         void ProcessChar(const unsigned char ch) throw (std::bad_alloc);
         void ProcessText(const std::string & value) throw ();
         const FontVectoriser * getVectoriser(unsigned char ch) throw();
         const std::string * getJustify() const throw ();
         bool getHorizontal() const throw ();
         bool getLeftToRight() const throw ();
         bool getTopToBottom() const throw ();
         float getAdvanceWidth() const throw ();
         float getAdvanceHeight() const throw ();
         void getBbox(const std::string & value, float & xmin,
                      float & ymin,float & xmax,float & ymax) throw(std::bad_alloc);
         void getBbox(const unsigned long c, float & xmin,
                      float & ymin,float & xmax,float & ymax)throw(std::bad_alloc);
         static FontFace & getDefFontFace() throw (std::bad_alloc);
      private:
         FT_Face face;
         FT_Error error;
         bool horizontal;
         bool leftToRight;
         bool topToBottom;
         float spacing;
         float scale;
         std::string language;
         static FontFace * defFface;
         std::string justify[2];
         GlyphContainer glyphcontainer;
	};

    class FontVectoriser {
      public:
         FontVectoriser(const FT_Glyph glyph, const FT_GlyphSlot glyphslot,
                        float scale) throw();
         virtual ~FontVectoriser() throw();
         int process() throw();
         int getTotContours() const throw();
         const Contour & getContourlist() const throw();
         float getHoriadvance() const throw();
         float getVertadvance() const throw();
         bool getOrientation() const throw();
      private:
         float scale;
         Contour *contourlist;
         int contour_index;
         Vertex last_vertex;
         FT_Outline  ftoutline;
         FT_Outline_Funcs interface_funcs;
         float height;
         float width;
         float horiadvance;
         float vertadvance;
         static void EvaluateCurve(const FontVectoriser* vector,  
                     float array[][2],int size) throw(std::bad_alloc);
         static int moveToFunc(const FT_Vector* to, 
                    const FontVectoriser *vectoriser) throw(std::bad_alloc);
         static int lineToFunc(const FT_Vector* to, 
                    const FontVectoriser *vectoriser) throw(std::bad_alloc);
         static int conicToFunc(const FT_Vector* control, const FT_Vector* to, 
                    const FontVectoriser *vectoriser) throw(std::bad_alloc);  
         static int cubicToFunc(const FT_Vector* control1, 
                                const FT_Vector* control2,
                                const FT_Vector* to, 
                                const FontVectoriser *vectoriser) 
                                throw(std::bad_alloc);
    };
}

#endif

#endif // OPENVRML_FONT_H
