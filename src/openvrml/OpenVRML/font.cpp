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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef OPENVRML_HAVE_FREETYPEFONTS
 
#include "font.h"


using namespace OpenVRML;

/**
 *
 * @brief It uses the FreeType2 library to extract information from font files 
 *        present in the system. This can handle the files which is scalable and 
 *        FreeType can read like TrueType font (*.ttf), Type1 PostScript 
 *        PostScript Font ASCII (*.pfa) or Type1 PostScript Font Binary (*.pfb) etc.
 *
 * @todo Use Unicode (UCS-2, UCS-4) as defined by ISO 10646-1:1993
 */

#define STEPSIZE  0.2       // stepsize for increment to draw contour

// Followings are macros to define system dependent font files. Probably 
// these can go to configure script, since these are system dependent.

//#ifdef _WIN32
// #define FONTSDIR                 "c:/winnt/fonts"
// #define SERIFPLAIN               FONTSDIR"/times.ttf"
// #define SERIFBOLD                FONTSDIR"/timesbd.ttf"
// #define SERIFITALIC              FONTSDIR"/timesi.ttf"
// #define SERIFBOLDITALIC          FONTSDIR"/timesbi.ttf"
// #define SANSPLAIN                FONTSDIR"/arial.ttf"
// #define SANSBOLD                 FONTSDIR"/arialbd.ttf"
// #define SANSITALIC               FONTSDIR"/ariali.ttf"
// #define SANSBOLDITALIC           FONTSDIR"/arialbi.ttf"
// #define TYPEWRITERPLAIN          FONTSDIR"/cour.ttf"
// #define TYPEWRITERBOLD           FONTSDIR"/courbd.ttf"
// #define TYPEWRITERITALIC         FONTSDIR"/couri.ttf"
// #define TYPEWRITERBOLDITALIC    FONTSDIR"/courbi.ttf"
//#else
// #define FONTSDIR                 "/usr/lib/X11/fonts/Type1"
// #define SERIFPLAIN               FONTSDIR"/lcdxrr.pfa"
// #define SERIFBOLD                FONTSDIR"/lcdxrb.pfa"
// #define SERIFITALIC              FONTSDIR"/lcdxro.pfa"
// #define SERIFBOLDITALIC          FONTSDIR"/lcdxrbo.pfa"
// #define SANSPLAIN                FONTSDIR"/lcdxsr.pfa"
// #define SANSBOLD                 FONTSDIR"/lcdxsb.pfa"
// #define SANSITALIC               FONTSDIR"/lcdxso.pfa"
// #define SANSBOLDITALIC           FONTSDIR"/lcdxsbo.pfa"
// #define TYPEWRITERPLAIN          FONTSDIR"/cour.pfa"
// #define TYPEWRITERBOLD           FONTSDIR"/courb.pfa"
// #define TYPEWRITERITALIC         FONTSDIR"/couri.pfa"
// #define TYPEWRITERBOLDITALIC     FONTSDIR"/courbi.pfa"
//#endif

/**
 * @class OpenVRML::FontLibrary
 *
 * @brief A class to encapsulate the FreeType Library. It ensures
 *        that only one FT_Library is in existence at any one time. Clients
 *        can't create this instance and must access it's methods through
 *        static <code>FontLibrary::Instance()</code> function.
 */

/**
 * @brief The static instance of FreeType library handle
 *
 */
FT_Library FontLibrary::lib;
/**
 * @brief The static instance of FontLibrary object
 *
 */
FontLibrary FontLibrary::fontlib;
/**
 * @brief Default private constructor to stop clients creating its own
 *        FT_Library objects. 
 *
 * @exception std::bad_alloc        if memory allocation fails.
 */
FontLibrary::FontLibrary(void) throw (std::bad_alloc) {
    if(lib)return;
    FT_Error error = FT_Init_FreeType(&lib);
    if(error) lib = 0;
}
/**
 * @brief Destructor to dispose the FreeType library
 *
 */
FontLibrary::~FontLibrary(void) throw (){
    if(lib) FT_Done_FreeType(lib);
}
/**
 * @brief Global access point to the single FontLibrary object
 * @return the handle to FreeType library instance.
 */
FT_Library & FontLibrary::Instance(void) {
    return lib;
}
/**
 * @class OpenVRML::FontFace
 *
 * @brief A class to provide an abstraction layer for the FreeType Face
 */

/**
 * @brief The static instance of default Face object
 *
 */
FontFace * FontFace::defFface = 0;

/**
 * @brief the constructor with default values
 *
 */
FontFace::FontFace() throw () {
     this->horizontal = true;
     this->leftToRight = true;
     this->topToBottom = true;
     this->spacing = 1.0;
     this->justify[0] = "BEGIN";
     this->justify[1] = "FIRST";
     this->error = 0;
};

/**
 * @brief Destructor to dispose the face object
 */
FontFace::~FontFace() throw (){
}

/**
 * @brief To set justify field which determines the allignment of text
 *
 * @param length   the length of the passed array
 * @param justify  an array of std::string
 
 * @exception std::bad_alloc    if memory allocation fails.
 */
void FontFace::setJustify(size_t length, const std::string* justify) 
       throw (std::bad_alloc){
       std::copy(justify, justify + length, this->justify);
       if(this->justify[0].size() == 0)this->justify[0] = "BEGIN";
       if(this->justify[1].size() == 0)this->justify[1] = "FIRST";
}

/**
 * @brief Set the boolean value indicating whether the text should be horizontal or
 *      vertical.
 *
 * @param horizontal @c TRUE if the text should be horizontal, or @c FALSE if the text
 *      should be vertical.
 */
void FontFace::setHorizontal(bool horizontal) throw () {
    this->horizontal = horizontal;
}

/**
 * @brief Set the boolean value indicating whether the text should be rendered
 *      left-to-right.
 *
 * @param leftToRight @c TRUE if the text should be rendered left-to-right,
 *      or @c FALSE if the text should be rendered right-to-left.
 */
void FontFace::setLeftToRight(bool leftToRight) throw () {
    this->leftToRight = leftToRight;
}

/**
 * @brief Set the boolean value indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @param topToBottom  @c TRUE if the text should be rendered top-to-bottom, 
 *      or @c FALSE if the text should be rendered bottom-to-top.
 */
void FontFace::setTopToBottom(bool topToBottom) throw () {
    this->topToBottom = topToBottom;
}

/**
 * @brief Set the spacing for the text.
 *
 * @param spacing the spacing for the text.
 */
void FontFace::setSpacing(float spacing) throw () { 
     this->spacing = spacing ;
}

/**
 * @brief Set the language code.
 *
 * @param language the language code.
 */
void FontFace::setLanguage(const std::string & language) throw () { 
     this->language = language ;
}
/**
 * @brief To create a font face object by using a font file 
 *
 * @param family_size the length of the passed array of family
 * @param family      an array of std::string that specifies a seq. of family names 
 *                    in preference order
 * @param style       feild specifies the style of Text
 * @param size        specifies the nominal height of Text in local coordinate
 */ 
void FontFace::openFace(size_t family_size, const std::string* family, 
       const std::string & style, float size) throw (std::bad_alloc){
       std::string temp_style = style;
       std::string filename;
       if(temp_style.size() == 0) temp_style = "PLAIN";
       for (size_t i(0); i < family_size; ++i) {
         this->error = 1;
         std::string tmp = family[i];
         if(tmp.size() == 0) tmp = "SERIF";
         tmp = tmp + temp_style;
         if (tmp == "SERIFPLAIN")filename = SERIFPLAIN;
         else if (tmp == "SERIFBOLD")filename = SERIFBOLD;
         else if (tmp == "SERIFITALIC")filename = SERIFITALIC;
         else if (tmp == "SERIFBOLDITALIC")filename = SERIFBOLDITALIC;
         else if (tmp == "SANSPLAIN")filename = SANSPLAIN;
         else if (tmp == "SANSBOLD")filename = SANSBOLD;
         else if (tmp == "SANSITALIC")filename = SANSITALIC;
         else if (tmp == "SANSBOLDITALIC")filename = SANSBOLDITALIC;
         else if (tmp == "TYPEWRITERPLAIN")filename = TYPEWRITERPLAIN;
         else if (tmp == "TYPEWRITERBOLD")filename = TYPEWRITERBOLD;
         else if (tmp == "TYPEWRITERITALIC")filename = TYPEWRITERITALIC;
         else if (tmp == "TYPEWRITERBOLDITALIC")filename = TYPEWRITERBOLDITALIC;
         if (!filename.empty())
         this->error = FT_New_Face(FontLibrary::Instance(),filename.c_str(),0,&this->face);
//       success? go out of the loop 
         if(this->error == 0)break;
       }
//     should support at least SERIF (the default)
	   if(this->error)
         this->error = FT_New_Face(FontLibrary::Instance(),
                                SERIFPLAIN,0,&this->face);
       if(this->error)return;

//     Allow only scalable truetype or type1 font
       if(!FT_IS_SCALABLE(this->face)){
        this->error = 20;
        return;
       }

// pointsize is the point size of the font to generate. A point is essentially 1/72th
// of an inch.

       float pointsize = .029 ; 
       this->error = FT_Set_Charmap(this->face,this->face->charmaps[0]);
       this->error = FT_Set_Char_Size(this->face, 
                                       static_cast<FT_F26Dot6>(2048 * 64),0,0,0);
       this->scale = static_cast<double>(size * pointsize)/static_cast<double>(72);
}
 
/**
 * @brief Get the boolean value indicating whether there is an error in freetype lib 
 *
 * @return @c TRUE if there is an error, or @c FALSE if there is no error
 */
bool FontFace::getError() const throw () {
    return(this->error == 0 ? false : true);
}

/**
 * @brief To create static instance of default FontFace object (defFface)
 *
 * @return static instance of default FontFace
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
FontFace & FontFace::getDefFontFace() throw (std::bad_alloc) {
 if (!defFface){
  defFface = new FontFace();
  defFface->justify[0] = "BEGIN";
  defFface->justify[1] = "FIRST";
  defFface->error = FT_New_Face(FontLibrary::Instance(),
                                SERIFPLAIN,0,&defFface->face);

// Allow only scalable truetype or Type1 font
  if(!FT_IS_SCALABLE(defFface->face))
    defFface->error = 20;
  if(!defFface->error){
   float pointsize = .029; 
   defFface->error = FT_Set_Charmap(defFface->face,defFface->face->charmaps[0]);
   defFface->error = FT_Set_Char_Size(defFface->face, 
                                      static_cast<FT_F26Dot6>(2048 * 64),0,0,0);
   defFface->scale = static_cast<double>(pointsize)/static_cast<double>(72.);
  }
 }
 return *defFface;
}

/**
 * @brief Process the input string
 *   
 * @param value the input string
 *
 */
void FontFace::ProcessText(const std::string & value) throw (){
  int length = value.length();
  for (int i=0; i < length; i++)
    ProcessChar(value[i]);
}

/**
 * @brief Vectorise the character and insert it into glyphcontainer with
 *    glyph_index.
 *
 * @param ch The input character
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void FontFace::ProcessChar(const unsigned char ch) throw (std::bad_alloc){
// Is this glyph_index already in glyphcontainer ?
   if(getVectoriser(ch))return;
// otherwise put it into glyphcontainer
   else {
//   load a single glyph within a given glyph slot
     FT_UInt glyph_index = FT_Get_Char_Index(this->face,static_cast <FT_ULong>(ch));
     error = FT_Load_Glyph(this->face, glyph_index, FT_LOAD_DEFAULT);
     FT_Glyph gl;
     this->error = FT_Get_Glyph(this->face->glyph,&gl);
     if(this->error)return;
     FontVectoriser *vect = new FontVectoriser(gl,this->face->glyph,this->scale);
     this->error = vect->process();
     if(this->error)return;
     this->glyphcontainer[glyph_index] = vect;
   }
}

/**
 * @brief Get FontVectoriser object from glyphcontainer with given 
 *     character.
 *
 * @param glyph_index      the glyph index
 * @return FontVectoriser  if it is in glyphcontainer otherwise zero. 
 *
 */
const FontVectoriser * FontFace::getVectoriser(unsigned char ch) throw(){
   FT_UInt glyph_index = FT_Get_Char_Index(this->face,static_cast <FT_ULong>(ch));
   if(!glyph_index)return 0;
   GlyphContainer::const_iterator GCI = this->glyphcontainer.find(glyph_index);
   if(GCI != this->glyphcontainer.end())
      return GCI->second;
   else return 0;
}

/**
 * @brief Get the descriptor for the text justification.
 *
 * @return pointer of string value array describing the characteristics of the text
 *      justification.
 */
const std::string * FontFace::getJustify() const throw () {
    return this->justify;
}

/**
 * @brief Get the boolean value indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c TRUE if the text should be horizontal, or @c FALSE if the text
 *      should be vertical.
 */
bool FontFace::getHorizontal() const throw () {
    return this->horizontal;
}

/**
 * @brief Get the boolean value indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c TRUE if the text should be rendered left-to-right, or @c FALSE if
 *      the text should be rendered right-to-left.
 */
bool FontFace::getLeftToRight() const throw () {
    return this->leftToRight;
}

/**
 * @brief Get the boolean value indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c TRUE if the text should be rendered top-to-bottom, or @c FALSE if
 *      the text should be rendered bottom-to-top.
 */
bool FontFace::getTopToBottom() const throw () {
    return this->topToBottom;
}

/**
 * @brief Get the advance width of the text.
 *
 * @return the advance width of the text.
 */
float FontFace::getAdvanceWidth() const throw () {
   int advancewidth = 2450;    // advance in font unit   
   return advancewidth * this->scale * this->spacing;
}

/**
 * @brief Get the advance height of the text.
 *
 * @return the advance height of the text.
 */
float FontFace::getAdvanceHeight() const throw () {
   int advanceheight = 2450;    // advance in font unit   
   return advanceheight * this->scale * this->spacing;
}

/**
 * @brief Get bounding box by given string 
 *
 * @param value   the input string
 * @return xmin   the xmin of bbox
 * @return ymin   the ymin of bbox
 * @return xmax   the xmax of bbox
 * @return ymax   the ymax of bbox
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void FontFace::getBbox(const std::string & value, float & xmin, float & ymin,
               float & xmax, float & ymax) throw (std::bad_alloc){
   int length = value.length();
   xmin = xmax = ymin = ymax = 0.0;
   float advx = 0.0, advy = 0.0;
   int dist = 1800;    // Guess some distance in font unit   
   for (int i=0; i < length; i++){
    FT_UInt glyph_index = FT_Get_Char_Index(this->face,static_cast <FT_ULong>(value[i]));
    if(!glyph_index)return;
//  load a single glyph within a given glyph slot
    this->error = FT_Load_Glyph(this->face, glyph_index, FT_LOAD_DEFAULT);
    FT_Glyph gl;
    this->error = FT_Get_Glyph(this->face->glyph,&gl);
    if(this->error)return;
    FT_BBox bbox;
    FT_Glyph_Get_CBox(gl,ft_glyph_bbox_unscaled,&bbox);
    float new_val;
    new_val = ((bbox.xMin/64.0f) * scale) + advx;
//    new_val = ((bbox.xMin >> 6) * scale) + advx;
    xmin = new_val < xmin ? new_val : xmin;
    new_val = ((bbox.yMin/64.0f) * scale) + advy;
//    new_val = ((bbox.yMin >> 6) * scale) + advy;
    ymin = new_val < ymin ? new_val : ymin;
    new_val = ((bbox.xMax/64.0f) * scale) + advx;
//    new_val = ((bbox.xMax >> 6) * scale) + advx;
    xmax = new_val > xmax ? new_val : xmax;
    new_val = ((bbox.yMax/64.0f) * scale) + advy;
//    new_val = ((bbox.yMax >> 6) * scale) + advy;
    ymax = new_val > ymax ? new_val : ymax;
    if (this->horizontal){
      if(FT_HAS_HORIZONTAL(this->face))
       advx += (face->glyph->metrics.horiAdvance/64.0f) * scale;
//      advx += (face->glyph->metrics.horiAdvance >> 6) * scale;
      else
// Guess some distance 
       advx += dist * scale;
    }
    else {
       if(FT_HAS_VERTICAL(this->face))
		advy += (face->glyph->metrics.height/64.0f) * scale;
//        advy += (face->glyph->metrics.height >> 6) * scale;
       else
// Guess some distance 
        advy += dist * scale;
	}
  }
}

/**
 * @brief Get bounding box by given character 
 *
 * @param ch      the input character
 * @return xmin   the xmin of bbox
 * @return ymin   the ymin of bbox
 * @return xmax   the xmax of bbox
 * @return ymax   the ymax of bbox
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void FontFace::getBbox(const unsigned long ch, float & xmin,
               float & ymin,float & xmax,float & ymax) throw(std::bad_alloc){
   xmin = xmax = ymin = ymax = 0.0;
   FT_UInt glyph_index = FT_Get_Char_Index(this->face,static_cast <FT_ULong>(ch));
   if(!glyph_index)return;
// load a single glyph within a given glyph slot
   error = FT_Load_Glyph(this->face, glyph_index, FT_LOAD_DEFAULT);
   FT_Glyph gl;
   this->error = FT_Get_Glyph(face->glyph,&gl);
   if(this->error)return;
   FT_BBox bbox;
   FT_Glyph_Get_CBox(gl,ft_glyph_bbox_unscaled,&bbox);
   xmin = (bbox.xMin/64.0f) * scale;
   ymin = (bbox.yMin/64.0f) * scale;
   xmax = (bbox.xMax/64.0f) * scale;
   ymax = (bbox.yMax/64.0f) * scale;
//   xmin = (bbox.xMin >> 6) * scale;
//   ymin = (bbox.yMin >> 6) * scale;
//   xmax = (bbox.xMax >> 6) * scale;
//   ymax = (bbox.yMax >> 6) * scale;
}
/**
 * @class OpenVRML::FontVectoriser
 * @brief A class to convert font outlines into set of contours. Points are stored
 * in contourlist. 
 *
 */

/**
 * @brief the constructor
 *
 */
FontVectoriser::FontVectoriser(const FT_Glyph glyph, const FT_GlyphSlot glyphslot,
                               float scale) throw (): 
   contour_index(-1),scale(scale){
   this->ftoutline = reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
   this->height = (glyphslot->metrics.height/64.0f) * this->scale;
   this->width = (glyphslot->metrics.width/64.0f) * this->scale;
// Guess some distance for horizontal or vertical advance in font unit
   int dist = 1800;
   if(FT_HAS_HORIZONTAL(glyphslot->face))
    this->horiadvance = (glyphslot->metrics.horiAdvance/64.0f) * this->scale;
   else
// Guess some distance 
    this->horiadvance = dist * this->scale;
//   this->height = (glyphslot->metrics.height >> 6) * this->scale;
//   this->width = (glyphslot->metrics.width >> 6) * this->scale;   
//   this->horiadvance = (glyphslot->metrics.horiAdvance >> 6) * this->scale;
   if(FT_HAS_VERTICAL(glyphslot->face))
    this->vertadvance = (glyphslot->metrics.vertAdvance/64.0f) * this->scale;
   else
// Guess some distance    
   this->vertadvance = dist * this->scale;
   this->contourlist = new Contour[this->ftoutline.n_contours];
// callbacks for freetype glyph decomposition into outlines
   interface_funcs.move_to = reinterpret_cast<FT_Outline_MoveTo_Func>(moveToFunc);
   interface_funcs.line_to = reinterpret_cast<FT_Outline_LineTo_Func>(lineToFunc);
   interface_funcs.conic_to = reinterpret_cast<FT_Outline_ConicTo_Func>(conicToFunc);
   interface_funcs.cubic_to = reinterpret_cast<FT_Outline_CubicTo_Func>(cubicToFunc);
   interface_funcs.shift = 0;
   interface_funcs.delta = 0;
}

/**
 * @brief Destructor.
 */
  FontVectoriser::~FontVectoriser() throw(){
  int ncontours=this->ftoutline.n_contours;
  for ( int i = 0; i < ncontours; i++)
     this->contourlist[i].clear();
  delete [] contourlist;
}

/**
 * @brief Process outlines into set of contours.
 *     The FreeType glyph decomposition routine traverses
 *     the outlines of the font by calling the various routines stored in
 *     interface_funcs.
 *
 */
int FontVectoriser::process() throw (){
  int error = FT_Outline_Decompose(&this->ftoutline, &this->interface_funcs, this);
  return error;
}


/**
 * @brief Get total number of contours.
 *
 * @return total number of contours.
 */
int FontVectoriser::getTotContours() const throw () {
    return this->ftoutline.n_contours; 
}

/**
 * @brief Get the pointer of the contourlist.
 *
 * @return  contourlist.
 */
const Contour & FontVectoriser::getContourlist() const throw(){
      return *this->contourlist;
}

/**
 * @brief Get the scaled horizontal advance width for the glyph.
 *
 * @return scaled horizontal advance width for the glyph.
 */
float FontVectoriser::getHoriadvance() const throw(){
      return this->horiadvance;
}

/**
 * @brief Get the scaled vertical advance height for the glyph.
 *
 * @return scaled vertical advance height for the glyph.
 */
float FontVectoriser::getVertadvance() const throw(){
      return this->vertadvance;
}

/**
 * @brief get the boolean value indicating the orientation of contour's vertices.
 *
 * @return @c TRUE if orietation of contour's vertices is clockwise, 
 *      or @c FALSE if orientation of contour's vertices is counter clockwise.
 */
bool FontVectoriser::getOrientation() const throw(){
	bool flag = (this->ftoutline.flags & 
                 ft_outline_reverse_fill) ==  ft_outline_reverse_fill ? false : true;
    return flag;
}

/**
 * @brief callback for FreeType glyph decomposition, Create a new contour and 
 *        put it into contourlist
 *
 */
int FontVectoriser::moveToFunc(const FT_Vector* to, const FontVectoriser* vector)
                    throw(std::bad_alloc){
    FontVectoriser* t = const_cast<FontVectoriser*>(vector); 
    float x = (to->x/64.0f)*t->scale;
    float y = (to->y/64.0f)*t->scale;
//    float x = (to->x >> 6)*t->scale;
//    float y = (to->y >> 6)*t->scale;
//  Create a new contour and put it into contourlist
    Contour *contour = new Contour;
    Vertex vertex;
    vertex.V[0] = x;
    vertex.V[1] = y;
    contour->push_back(vertex);
    t->last_vertex = vertex;
    t->contour_index++;
    t->contourlist[t->contour_index] = *contour;
    return 0;
}

/**
 * @brief callback for FreeType glyph decomposition. Put vertex info. in 
 *        contour
 *
 */
int FontVectoriser::lineToFunc(const FT_Vector* to, const FontVectoriser* vector)
                    throw(std::bad_alloc){
    FontVectoriser* t = const_cast<FontVectoriser*>(vector); 
    float x = (to->x/64.0f)*t->scale;
    float y = (to->y/64.0f)*t->scale;
//    float x = (to->x >> 6)*t->scale;
//    float y = (to->y >> 6)*t->scale;
    Vertex vertex;
    vertex.V[0] = x;
    vertex.V[1] = y;
    t->last_vertex = vertex;
    t->contourlist[t->contour_index].push_back(vertex);
    return 0;
}

/**
 * @brief callback for FreeType glyph decomposition. Calculate curve   
 *    coordinates by giving starting point, end point and one 
 *    control point using de-Casteljau algo. 
 */
int FontVectoriser::conicToFunc(const FT_Vector* control, const FT_Vector* to,
                    const FontVectoriser* vector) throw(std::bad_alloc){
    FontVectoriser* t = const_cast<FontVectoriser*>(vector);
// Array for storing values for de-Casteljau algo.
    float values[9][2];  
    values[0][0] = t->last_vertex.V[0];
    values[0][1] = t->last_vertex.V[1];
    values[1][0] = (control->x/64.0f)*t->scale;
    values[1][1] = (control->y/64.0f)*t->scale;
//    values[1][0] = (control->x >> 6)*t->scale;
//    values[1][1] = (control->y >> 6)*t->scale;
    t->last_vertex.V[0] = values[2][0] = (to->x/64.0f)*t->scale;
    t->last_vertex.V[1] = values[2][1] = (to->y/64.0f)*t->scale;
//    t->last_vertex.V[0] = values[2][0] = (to->x >> 6)*t->scale;
//    t->last_vertex.V[1] = values[2][1] = (to->y >> 6)*t->scale;
    EvaluateCurve(t,values,3);
    return 0;
}

/**
 * @brief callback for FreeType glyph decomposition. Calculate curve   
 *    coordinates by giving starting point, end point and two 
 *    control points using de-Casteljau algo. 
 */
int FontVectoriser::cubicToFunc(const FT_Vector* control1, const FT_Vector* control2, 
    const FT_Vector* to, const FontVectoriser* vector) throw(std::bad_alloc){
    FontVectoriser* t = const_cast<FontVectoriser*>(vector);
    float values[16][2];  // Array for storing values for de-Casteljau algo.
    values[0][0] = t->last_vertex.V[0];
    values[0][1] = t->last_vertex.V[1];
    values[1][0] = (control1->x/64.0f)*t->scale;
    values[1][1] = (control1->y/64.0f)*t->scale;
    values[2][0] = (control2->x/64.0f)*t->scale;
    values[2][1] = (control2->y/64.0f)*t->scale;
//    values[1][0] = (control1->x >> 6)*t->scale;
//    values[1][1] = (control1->y >> 6)*t->scale;
//    values[2][0] = (control2->x >> 6)*t->scale;
//    values[2][1] = (control2->y >> 6)*t->scale;
    t->last_vertex.V[0] = values[3][0] = (to->x/64.0f)*t->scale;
    t->last_vertex.V[1] = values[3][1] = (to->y/64.0f)*t->scale;
//    t->last_vertex.V[0] = values[3][0] = (to->x >> 6)*t->scale;
//    t->last_vertex.V[1] = values[3][1] = (to->y >> 6)*t->scale;
    EvaluateCurve(t,values,4);
    return 0;
}

/**
 * @brief De-Casteljau algorithm. 
 * This is a nice recursive algorithm defined by de-Casteljau which calculates for a 
 * given control polygon the point that lies on the bezier curve for any value of t, 
 * and can be used to evaluate and draw the Bezier spline without using the Bernstein 
 * polynomials.
 * The algorithm advances by creating in each step a polygons of degree one less than 
 * the one created in the previous step until there is only one point left, which is 
 * the point on the curve. The polygon vertices for each step are defined by linear 
 * interpolation of two consecutive vertices of the polygon from the previous step 
 * with a value of t (the parameter): 
 */

void FontVectoriser::EvaluateCurve(const FontVectoriser* vector,  float array[][2],
                 int size) throw(std::bad_alloc){
   float t;               // Parametric points 0 <= t <= 1
   for(int i = 0; i <= (1/STEPSIZE); i++){
     t = i*STEPSIZE;
     for(int j = 1; j < size; j++)
        for(int k = 0; k < (size - j); k++){
          array[j*size+k][0] = (1 - t) * array[(j-1)*size+k][0] 
                               + t * array[(j-1)*size+k+1][0];
          array[j*size+k][1] = (1 - t) * array[(j-1)*size+k][1] 
                               + t * array[(j-1)*size+k+1][1];     
        }
   Vertex vertex;
   vertex.V[0] = array[(size-1)*size][0];
   vertex.V[1] = array[(size-1)*size][1];
//Specify next vertex to be included on curve
   vector->contourlist[vector->contour_index].push_back(vertex);      
   }
}

#endif
