//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <algorithm>
# include <numeric>
# include <assert.h>
# include "field.h"
# include "private.h"
# include "node.h"

using namespace OpenVRML;
using namespace OpenVRML_;

namespace {
    ::ostream & mffprint(::ostream &, float const * c, int n, int eltsize);
    ::ostream & mfdprint(::ostream &, double const * c, int n, int eltsize);
    
    const char * ftn[] = {
        "<invalid field type>",
        "SFBool",
        "SFColor",
        "SFFloat",
        "SFImage",
        "SFInt32",
        "SFNode",
        "SFRotation",
        "SFString",
        "SFTime",
        "SFVec2f",
        "SFVec3f",
        "MFColor",
        "MFFloat",
        "MFInt32",
        "MFNode",
        "MFRotation",
        "MFString",
        "MFTime",
        "MFVec2f",
        "MFVec3f"
    };
}

/**
 * @class OpenVRML::FieldValue
 *
 * @brief Abstract base class for the VRML field types.
 */

/**
 * @enum FieldValue::FieldType
 *
 * @brief Used to identify FieldValue types.
 *
 * These tags are typically used to designate an expected type or avoid a
 * <code>dynamic_cast</code>.
 */

/**
 * @var FieldValue::NO_FIELD
 *
 * @brief Zero value typically used to indicate failure.
 */

/**
 * @var FieldValue::SFBOOL
 *
 * @brief Designates an SFBool.
 */

/**
 * @var FieldValue::SFCOLOR
 *
 * @brief Designates an SFColor.
 */

/**
 * @var FieldValue::SFFLOAT
 *
 * @brief Designates an SFFloat.
 */

/**
 * @var FieldValue::SFIMAGE
 *
 * @brief Designates an SFImage.
 */

/**
 * @var FieldValue::SFINT32
 *
 * @brief Designates an SFInt32.
 */

/**
 * @var FieldValue::SFNODE
 *
 * @brief Designates an SFNode.
 */

/**
 * @var FieldValue::SFROTATION
 *
 * @brief Designates an SFRotation.
 */

/**
 * @var FieldValue::SFSTRING
 *
 * @brief Designates an SFString.
 */

/**
 * @var FieldValue::SFTIME
 *
 * @brief Designates an SFTime.
 */

/**
 * @var FieldValue::SFVEC2F
 *
 * @brief Designates an SFVec2f.
 */

/**
 * @var FieldValue::SFVEC3F
 *
 * @brief Designates an SFVec3f.
 */

/**
 * @var FieldValue::MFCOLOR
 *
 * @brief Designates an MFColor.
 */

/**
 * @var FieldValue::MFFLOAT
 *
 * @brief Designates an MFFloat.
 */

/**
 * @var FieldValue::MFINT32
 *
 * @brief Designates an MFInt32.
 */

/**
 * @var FieldValue::MFNODE
 *
 * @brief Designates an MFNode.
 */

/**
 * @var FieldValue::MFROTATION
 *
 * @brief Designates an MFRotation.
 */

/**
 * @var FieldValue::MFSTRING
 *
 * @brief Designates an MFString.
 */

/**
 * @var FieldValue::MFTIME
 *
 * @brief Designates an MFTime.
 */

/**
 * @var FieldValue::MFVEC2F
 *
 * @brief Designates an MFVec2f.
 */

/**
 * @var FieldValue::MFVEC3F
 *
 * @brief Designates an MFVec3f.
 */

/**
 * @brief Destructor. (Virtualized for abstract base class.)
 */
FieldValue::~FieldValue() {}

/**
 * @fn FieldValue * FieldValue::clone() const
 * @brief Virtual copy constructor.
 * @return a new FieldValue identical to this one.
 */

/**
 * @fn ostream& FieldValue::print(ostream& os) const
 * @brief Print the value of this FieldValue.
 * @param os an ostream
 * @return the ostream
 */

/**
 * @fn FieldValue::FieldType FieldValue::fieldType() const
 * @brief Get the field type.
 * @return the FieldType enumerant corresponding to this FieldValue's type
 */

/**
 * @brief A static method to convert a type name to an ID.
 *
 * @param type the (C-style) string name of a VRML field type
 *
 * @return the FieldType enumerant corresponding to the passed type name
 */
FieldValue::FieldType FieldValue::fieldType(const char * fieldTypeId) {
    for (size_t i(SFBOOL); i <= MFVEC3F; ++i) {
        if (strcmp(fieldTypeId, ftn[i]) == 0) {
            return static_cast<FieldType>(i);
        }
    }
    
    return NO_FIELD;
}

const char* FieldValue::getFieldName(const FieldType fieldType) {
  int ft = (int) fieldType;
  if ((ft > 0) && (ft <= (int) FieldValue::MFVEC3F))
    return ftn[ft];
  else
    return "<invalid field type>";
}


/**
 * @brief Get the type name of a field.
 * @return the (C-style) string name of this Field object's type.
 */
char const * FieldValue::fieldTypeName() const {
    assert((sizeof(ftn) / sizeof(char *)) >=
            static_cast<size_t>((this->fieldType() + 1)));
    return ftn[this->fieldType()];
}

/**
 * @fn const SFBool * FieldValue::toSFBool() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFColor * FieldValue::toSFColor() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFFloat * FieldValue::toSFFloat() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFImage * FieldValue::toSFImage() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFInt32 * FieldValue::toSFInt32() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFNode * FieldValue::toSFNode() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFRotation * FieldValue::toSFRotation() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFString * FieldValue::toSFString() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFTime * FieldValue::toSFTime() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFVec2f * FieldValue::toSFVec2f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const SFVec3f * FieldValue::toSFVec3f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFColor * FieldValue::toMFColor() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFFloat * FieldValue::toMFFloat() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFInt32 * FieldValue::toMFInt32() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFNode * FieldValue::toMFNode() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFRotation * FieldValue::toMFRotation() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFString * FieldValue::toMFString() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFVec2f * FieldValue::toMFVec2f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const MFVec3f * FieldValue::toMFVec3f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFBool * FieldValue::toSFBool() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFColor * FieldValue::toSFColor() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFFloat * FieldValue::toSFFloat() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFImage * FieldValue::toSFImage() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFInt32 * FieldValue::toSFInt32() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFNode * FieldValue::toSFNode() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFRotation * FieldValue::toSFRotation() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFString * FieldValue::toSFString() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFTime * FieldValue::toSFTime() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFVec2f * FieldValue::toSFVec2f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  SFVec3f * FieldValue::toSFVec3f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFColor * FieldValue::toMFColor() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFFloat * FieldValue::toMFFloat() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFInt32 * FieldValue::toMFInt32() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFNode * FieldValue::toMFNode() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFRotation * FieldValue::toMFRotation() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFString * FieldValue::toMFString() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFVec2f * FieldValue::toMFVec2f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  MFVec3f * FieldValue::toMFVec3f() 
 * @deprecated use <code>dynamic_cast</code>
 */

// Printing methods

ostream & ::operator<<(ostream & os, const FieldValue & f)
{ return f.print(os); }


/**
 * @class OpenVRML::SFBool
 *
 * @brief Encapsulates an SFBool value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value
 */
OpenVRML::SFBool::SFBool(bool value): d_value(value) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFBool::~SFBool() {}

ostream& OpenVRML::SFBool::print(ostream& os) const
{ return (os << (d_value ? "TRUE" : "FALSE")); }

OpenVRML::FieldValue * OpenVRML::SFBool::clone() const {
    return new SFBool(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFBool::fieldType() const {
    return SFBOOL;
}

/**
 * @brief Get the value.
 *
 * @return the value of this SFBool
 */
bool OpenVRML::SFBool::get() const {
    return this->d_value;
}

/**
 * @brief Set the value.
 *
 * @param value the new value
 */
void OpenVRML::SFBool::set(bool value) {
    this->d_value = value;
}


/**
 * @class OpenVRML::SFColor
 *
 * @brief Encapsulates an SFColor value.
 */

/**
 * @brief Construct with the default value, (0, 0, 0).
 */
OpenVRML::SFColor::SFColor() {
    this->d_rgb[0] = 0.0f;
    this->d_rgb[1] = 0.0f;
    this->d_rgb[2] = 0.0f;
}

/**
 * @brief Construct a SFColor.
 *
 * @param rgb a 3-element array
 */
OpenVRML::SFColor::SFColor(const float rgb[3]) {
    this->d_rgb[0] = rgb[0];
    this->d_rgb[1] = rgb[1];
    this->d_rgb[2] = rgb[2];
}

/**
 * @brief Construct a SFColor
 *
 * @param r red component
 * @param g green component
 * @param b blue component
 */
OpenVRML::SFColor::SFColor(float r, float g, float b)
{ d_rgb[0] = r; d_rgb[1] = g; d_rgb[2] = b; }

/**
 * @brief Destructor.
 */
OpenVRML::SFColor::~SFColor() {}

ostream & OpenVRML::SFColor::print(ostream& os) const
{ return (os << d_rgb[0] << ' ' << d_rgb[1] << ' ' << d_rgb[2]); }

OpenVRML::FieldValue * OpenVRML::SFColor::clone() const
{ return new SFColor(d_rgb[0],d_rgb[1],d_rgb[2]); }

OpenVRML::FieldValue::FieldType OpenVRML::SFColor::fieldType() const {
    return SFCOLOR;
}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index an index from 0 - 2
 */
float OpenVRML::SFColor::operator[](size_t index) const {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 *
 * @param index an index from 0 - 2
 */
float & OpenVRML::SFColor::operator[](size_t index) {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Get the red component.
 *
 * @return the red component value
 */
float OpenVRML::SFColor::getR() const {
    return this->d_rgb[0];
}

/**
 * @brief Get the green component.
 *
 * @return the green component value
 */
float OpenVRML::SFColor::getG() const {
    return this->d_rgb[1];
}

/**
 * @brief Get the blue component.
 * @return the blue component value
 */
float OpenVRML::SFColor::getB() const {
    return this->d_rgb[2];
}

/**
 * @brief Get the value.
 *
 * @return a reference to a 3-element array comprising the RGB value
 */
const float (&OpenVRML::SFColor::get() const)[3] {
    return this->d_rgb;
}

/**
 * @brief Set the value.
 *
 * @param rgb a 3-element vector comprising a RGB value
 */
void OpenVRML::SFColor::set(const float rgb[3]) {
    this->d_rgb[0] = rgb[0];
    this->d_rgb[1] = rgb[1];
    this->d_rgb[2] = rgb[2];
}

// Conversion functions between RGB each in [0,1] and HSV with  
// h in [0,360), s,v in [0,1]. From Foley, van Dam p615-616.

/**
 * @brief Convert a color from HSV to RGB.
 *
 * Convert from HSV (with(with <var>h</var> in [0,360), <var>s</var>,
 * <var>v</var> in [0,1]) to RGB (with each component in [0,1]).
 *
 * @param hsv a 3-element array comprising an HSV value
 * @retval rgb a 3-element array comprising an RGB value
 */
void OpenVRML::SFColor::HSVtoRGB(const float hsv[3], float rgb[3])
{
    float h = hsv[0];
    if (hsv[1] == 0.0) {
        rgb[0] = rgb[1] = rgb[2] = hsv[2];
    } else {
        if (h >= 360.0) {
            h -= 360.0;
        }
        h /= 60.0;
        const double i = floor(h);
        const double f = h - i;
        const float p = hsv[2] * (1.0 - hsv[1]);
        const float q = hsv[2] * (1.0 - hsv[1] * f);
        const float t = hsv[2] * (1.0 - hsv[1] * (1.0 - f));
        switch (static_cast<int>(i)) {
	    default:
	    case 0: rgb[0] = hsv[2]; rgb[1] = t; rgb[2] = p; break;
	    case 1: rgb[0] = q; rgb[1] = hsv[2]; rgb[2] = p; break;
	    case 2: rgb[0] = p; rgb[1] = hsv[2]; rgb[2] = t; break;
	    case 3: rgb[0] = p; rgb[1] = q; rgb[2] = hsv[2]; break;
	    case 4: rgb[0] = t; rgb[1] = p; rgb[2] = hsv[2]; break;
	    case 5: rgb[0] = hsv[2]; rgb[1] = p; rgb[2] = q; break;
	}
    }
}

/**
 * @brief Convert a color from RGB to HSV.
 *
 * Convert from RGB (with each component in [0,1]) to HSV (with <var>h</var> in
 * [0,360), <var>s</var>, <var>v</var> in [0,1]).
 *
 * @param rgb a 3-element array comprising an RGB value
 * @retval hsv a 3-element array comprising an HSV value
 */
void OpenVRML::SFColor::RGBtoHSV(const float rgb[3], float hsv[3]) {
    const float maxrgb = *std::max_element(rgb, rgb + 3);
    const float minrgb = *std::min_element(rgb, rgb + 3);
    
    hsv[0] = 0.0;
    hsv[1] = (maxrgb > 0.0) ? ((maxrgb - minrgb) / maxrgb) : 0.0;
    hsv[2] = maxrgb;
    
    if (hsv[1] != 0.0) {
        const float rc = (maxrgb - rgb[0]) / (maxrgb - minrgb);
        const float gc = (maxrgb - rgb[1]) / (maxrgb - minrgb);
        const float bc = (maxrgb - rgb[2]) / (maxrgb - minrgb);
        
        if (rgb[0] == maxrgb) {
            hsv[0] = bc - gc;
        } else if (rgb[1] == maxrgb) {
            hsv[0] = 2 + rc - bc;
        } else {
            hsv[0] = 4 + gc - rc;
        }
        
        hsv[0] *= 60.0;
        if (hsv[0] < 0.0) {
            hsv[0] += 360.0;
        }
    }
}

/**
 * @brief Set the value using HSV.
 *
 * @param h the hue component
 * @param s the saturation component
 * @param v the value component
 */
void OpenVRML::SFColor::setHSV(float h, float s, float v) {
    const float hsv[3] = { h, s, v };
    HSVtoRGB(hsv, this->d_rgb);
}

/**
 * @brief Get the value expressed in HSV.
 *
 * @retval hsv a 3-element array comprising the HSV value.
 */
void OpenVRML::SFColor::getHSV(float hsv[3]) const {
    RGBtoHSV(this->d_rgb, hsv);
}


/**
 * @class OpenVRML::SFFloat
 *
 * @brief Encapsulates an SFFloat value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value
 */
OpenVRML::SFFloat::SFFloat(float value): d_value(value) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFFloat::~SFFloat() {}

ostream & OpenVRML::SFFloat::print(ostream& os) const
{ return (os << d_value); }

OpenVRML::FieldValue * OpenVRML::SFFloat::clone() const {
    return new SFFloat(d_value);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFFloat::fieldType() const {
    return SFFLOAT;
}

/**
 * @brief Get value.
 *
 * @return the SFFloat value
 */
float OpenVRML::SFFloat::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new value
 */
void OpenVRML::SFFloat::set(float value) {
    this->d_value = value;
}

/**
 * @class OpenVRML::SFImage
 *
 * A single uncompressed 2-dimensional pixel image. The first
 * hexadecimal value is the lower left pixel and the last value is the
 * upper right pixel.Pixel values are limited to 256 levels of
 * intensity. A one-component image specifies one-byte greyscale
 * values. A two-component image specifies the intensity in the first
 * (high) byte and the alpha opacity in the second (low) byte. A
 * three-component image specifies the red component in the first
 * (high) byte, followed by the green and blue components.
 * Four-component images specify the alpha opacity byte after
 * red/green/blue.
 */

/**
 * Construct the default SFImage.
 */
OpenVRML::SFImage::SFImage(): d_w(0), d_h(0), d_nc(0), d_pixels(0) {}

/**
 * @brief Create an SFImage.
 *
 * Note that the pixels read from lower left to upper right, which
 * is a reflection around the y-axis from the "normal" convention.
 * <p>
 * Note also that width and height are specified in pixels, and a
 * pixel may be more than one byte wide. For example, an image with
 * a width and height of 16, and nc==3, would have a pixel array
 * w*h*nc = 16*16*3 = 768 bytes long. See the class intro above for
 * the interpretation of different pixel depths.
 * 
 * @param width width in pixels
 * @param height height in pixels
 * @param components number of components/pixel (see above)
 * @param pixels the caller owns the bytes, so this ctr makes a copy
 *
 */
OpenVRML::SFImage::SFImage(size_t width, size_t height, size_t components,
                           const unsigned char * pixels):
        d_w(0L), d_h(0L), d_nc(0L), d_pixels(0L) {
    const size_t nbytes = width * height * components;
    try {
        this->d_pixels = new unsigned char[nbytes];
        this->d_w = width;
        this->d_h = height;
        this->d_nc = components;
        std::copy(pixels, pixels + (width * height * components),
                  this->d_pixels);
    } catch (std::bad_alloc & ex) {
# ifndef NDEBUG
        cerr << ex.what() << endl;
# endif
    }
}

/**
 * @brief Copy constructor.
 */
OpenVRML::SFImage::SFImage(const SFImage& rhs): d_w(0), d_h(0), d_nc(0),
        d_pixels(0) {
    const size_t nbytes = rhs.d_w * rhs.d_h * rhs.d_nc;
    try {
        this->d_pixels = new unsigned char[nbytes];
        this->d_w = rhs.d_w;
        this->d_h = rhs.d_h;
        this->d_nc = rhs.d_nc;
        std::copy(rhs.d_pixels, rhs.d_pixels + nbytes, this->d_pixels);
    } catch (std::bad_alloc & ex) {
# ifndef NDEBUG
        cerr << ex.what() << endl;
# endif
    }
}

/**
 * @brief Destructor.
 */
OpenVRML::SFImage::~SFImage()
{
  delete [] d_pixels; 
}

/**
 * @brief Assignment.
 */
OpenVRML::SFImage & OpenVRML::SFImage::operator=(const SFImage & rhs) {
    if (this != &rhs) {
        delete [] this->d_pixels;
        this->d_w = this->d_h = this->d_nc = 0L;
        const size_t nbytes = rhs.d_w * rhs.d_h * rhs.d_nc;
        try {
            this->d_pixels = new unsigned char[nbytes];
            this->d_w = rhs.d_w;
            this->d_h = rhs.d_h;
            this->d_nc = rhs.d_nc;
            std::copy(rhs.d_pixels, rhs.d_pixels + nbytes, this->d_pixels);
        } catch (std::bad_alloc & ex) {
# ifndef NDEBUG
            cerr << ex.what() << endl;
# endif
        }
    }
    return *this;
}

ostream& OpenVRML::SFImage::print(ostream& os) const
{
  os << d_w << " " << d_h << " " << d_nc;

  size_t np = d_w*d_h;
  unsigned char *p = d_pixels;

  for (size_t i=0; i<np; ++i)
    {
      unsigned int pixval = 0;
      for (size_t j=0; j<d_nc; ++j)
	pixval = (pixval << 8) | *p++;
      os << pixval << " ";
    }
  return os;
}

OpenVRML::FieldValue * OpenVRML::SFImage::clone() const {
    return new SFImage(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFImage::fieldType() const {
    return SFIMAGE;
}

/**
 * @brief Get the image width.
 *
 * @return the image width
 */
size_t OpenVRML::SFImage::getWidth() const {
    return this->d_w;
}

/**
 * @brief Get the image height.
 *
 * @return the image height
 */
size_t OpenVRML::SFImage::getHeight() const {
    return this->d_h;
}

/**
 * @brief Get the number of components.
 *
 * @return the number of components
 */
size_t OpenVRML::SFImage::getComponents() const {
    return this->d_nc;
}

/**
 * @brief Get the pixel data.
 *
 * @return a pointer to the array of pixel data.
 */
const unsigned char * OpenVRML::SFImage::getPixels() const {
    return this->d_pixels;
}

/**
 * @brief Set the image.
 *
 * @param width width in pixels
 * @param height height in pixels
 * @param components number of components
 * @param pixels array of (width * height * components) bytes comprising the
 *        image data.
 */
void OpenVRML::SFImage::set(size_t width, size_t height, size_t components,
                            const unsigned char * pixels) {
    delete this->d_pixels;
    
    this->d_w = width;
    this->d_h = height;
    this->d_nc = components;
    
    this->d_pixels = new unsigned char[width * height * components];
    std::copy(pixels, pixels + (width * height * components), this->d_pixels);
}

//void SFImage::setSize(size_t w, size_t h) {
//    this->d_w = w;
//    this->d_h = h;
//}


/**
 * @class OpenVRML::SFInt32
 *
 * @brief Encapsulates an SFInt32 value.
 */

/**
 * @brief Constructor.
 * @param value initial value
 */
OpenVRML::SFInt32::SFInt32(long value): d_value(value) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFInt32::~SFInt32() {}

ostream & OpenVRML::SFInt32::print(ostream& os) const
{ return (os << d_value); }

OpenVRML::FieldValue * OpenVRML::SFInt32::clone() const {
    return new SFInt32(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFInt32::fieldType() const {
    return SFINT32;
}

/**
 * @brief Get value.
 *
 * @return the integer value
 */
long OpenVRML::SFInt32::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new integer value
 */
void OpenVRML::SFInt32::set(long value) {
    this->d_value = value;
}


/**
 * @class OpenVRML::SFNode
 *
 * @brief Encapsulates an SFNode.
 */

/**
 * @brief Constructor.
 *
 * @param node a NodePtr
 */
OpenVRML::SFNode::SFNode(const NodePtr & node): node(node) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFNode::~SFNode() {}

ostream & OpenVRML::SFNode::print(ostream & os) const {
    if (!this->node) {
        return os << "NULL" << endl;
    }
    return os << *this->node << endl;
}

OpenVRML::FieldValue * OpenVRML::SFNode::clone() const {
    return new SFNode(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFNode::fieldType() const {
    return FieldValue::SFNODE;
}

/**
 * @brief Get value.
 *
 * @return a smart pointer to this object's Node
 */
const OpenVRML::NodePtr & OpenVRML::SFNode::get() const {
    return this->node;
}

/**
 * @brief Set value.
 *
 * @param node a smart pointer to a Node, or to 0 if setting this
 *             SFNode to <code>NULL</code>
 */
void OpenVRML::SFNode::set(const NodePtr & node) {
    this->node = NodePtr(node);
}


/**
 * @class OpenVRML::SFRotation
 *
 * @brief Encapsulates an SFRotation.
 *
 * Per the VRML97 specification, the axis of an SFRotation is a normalized
 * vector (5.8). The specification leaves undefined how to deal with an
 * attempt to construct an SFRotation from an axis vector that is not
 * normalized. In order to allow users of the library to minimize the number
 * of normalizations, OpenVRML takes the following approach:
 *
 * <ul>
 * <li>Attempts to construct an SFRotation axis from a vector that is not
 * normalized will yield an assertion failure (abort) unless NDEBUG is defined
 * when compiling the library (in which case truly wacky behavior could
 * result).
 * <li>Assignment to individual components of the axis will result in the
 * axis being re-normalized upon each assignment.
 * </ul>
 */

/**
 * @brief Default constructor.
 *
 * Construct with the default value, (0, 0, 1, 0).
 */
OpenVRML::SFRotation::SFRotation() {
    this->d_x[0] = 0.0; // x
    this->d_x[1] = 0.0; // y
    this->d_x[2] = 1.0; // z
    this->d_x[3] = 0.0; // angle
}

/**
 * @brief Constructor.
 *
 * @param rotation a 4-element array.
 *
 * @pre The first three elements of the argument array constitute a
 *      normalized vector.
 */
OpenVRML::SFRotation::SFRotation(const float rot[4]) {
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(rot), 1.0));
    
    std::copy(rot, rot + 4, this->d_x);
}

/**
 * @brief Constructor.
 *
 * @param x the <var>x</var>-component of the axis of rotation
 * @param y the <var>y</var>-component of the axis of rotation
 * @param z the <var>z</var>-component of the axis of rotation
 * @param angle the rotation angle
 *
 * @pre The first three arguments constitute a normalized vector.
 */
OpenVRML::SFRotation::SFRotation(float x, float y, float z, float angle) {
    this->d_x[0] = x;
    this->d_x[1] = y;
    this->d_x[2] = z;
    
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(this->d_x), 1.0));
    
    this->d_x[3] = angle;
}

/**
 * @brief Constructor.
 *
 * @param axis a normalized vector to use as the axis of rotation
 * @param angle the rotation angle
 *
 * @pre The first argument is a normalized vector.
 */
OpenVRML::SFRotation::SFRotation(const SFVec3f & axis, float angle) {
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(axis.get()), 1.0));
    
    std::copy(axis.get(), axis.get() + 3, this->d_x);
    this->d_x[3] = angle;
}

/**
 * @brief Construct a rotation between two vectors.
 *
 * Construct a SFRotation equal to the rotation between two different
 * vectors.
 *
 * @param fromVector the starting vector
 * @param toVector the ending vector
 */
OpenVRML::SFRotation::SFRotation(const SFVec3f & fromVector,
                                 const SFVec3f & toVector) {
    this->setAxis(fromVector.cross(toVector));
    this->d_x[3] = acos(fromVector.dot(toVector) /
                        (fromVector.length() * toVector.length()));
}

/**
 * @brief Destructor.
 */
OpenVRML::SFRotation::~SFRotation() {}

ostream & OpenVRML::SFRotation::print(ostream& os) const
{ return (os <<d_x[0] << " " <<d_x[1] << " " <<d_x[2]<< " " <<d_x[3]); }

OpenVRML::FieldValue * OpenVRML::SFRotation::clone() const
{ return new SFRotation(d_x[0],d_x[1],d_x[2],d_x[3]); }

OpenVRML::FieldValue::FieldType OpenVRML::SFRotation::fieldType() const 
{ return SFROTATION; }

/**
 * @brief Get the <var>x</var>-component of the rotation axis.
 *
 * @return the <var>x</var>-component of the rotation axis.
 */
float OpenVRML::SFRotation::getX() const {
    return this->d_x[0];
}

namespace {
    void normalizeAxis_(float axis[3]) {
        const float axisLength = length(axis);
        if (fpequal(axisLength, 0.0)) {
            axis[2] = 1.0;
        } else {
            normalize(axis);
        }
    }
}

/**
 * @brief Set the <var>x</var>-component of the rotation axis.
 *
 * @param value
 */
void OpenVRML::SFRotation::setX(float value) {
    this->d_x[0] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the <var>y</var>-component of the rotation axis.
 *
 * @return the <var>y</var>-component of the rotation axis
 */
float OpenVRML::SFRotation::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component of the rotation axis.
 *
 * @param value
 */
void OpenVRML::SFRotation::setY(float value) {
    this->d_x[1] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the <var>z</var>-component of the rotation axis.
 *
 * @return the <var>z</var>-component of the rotation axis
 */
float OpenVRML::SFRotation::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component of the rotation axis
 *
 * @param value
 */
void OpenVRML::SFRotation::setZ(float value) {
    this->d_x[2] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the rotation angle.
 *
 * @return the rotation angle
 */
float OpenVRML::SFRotation::getAngle() const {
    return this->d_x[3];
}

/**
 * @brief Set the rotation angle.
 *
 * @param value
 */
void OpenVRML::SFRotation::setAngle(float value) {
    this->d_x[3] = value;
}

/**
 * @brief Get the value of this rotation.
 *
 * @return a reference to a 4-element array.
 */
const float (&OpenVRML::SFRotation::get() const)[4] {
    return this->d_x;
}

/**
 * @brief Set the value of this rotation.
 *
 * @param rot a 4-element array
 *
 * @pre The first three elements of <var>rot</var> constitute a normalized
 *      vector.
 */
void OpenVRML::SFRotation::set(const float rot[4]) {
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(rot), 1.0));
    
    std::copy(rot, rot + 4, this->d_x);
}

/**
 * @brief Get the axis of rotation as a SFVec3f.
 *
 * @return the axis of rotation
 */
const OpenVRML::SFVec3f OpenVRML::SFRotation::getAxis() const {
    return SFVec3f(this->d_x[0], this->d_x[1], this->d_x[2]);
}

/**
 * @brief Set the axis of rotation using a SFVec3f.
 *
 * @param axis the new rotation axis
 *
 * @pre <var>axis</var> is a normalized vector.
 */
void OpenVRML::SFRotation::setAxis(const SFVec3f & axis) {
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(axis.get()), 1.0));
    
    std::copy(axis.get(), axis.get() + 3, this->d_x);
}

/**
 * @brief Get the inverse.
 *
 * @return a SFRotation that is the inverse of this one
 */
const OpenVRML::SFRotation OpenVRML::SFRotation::inverse() const {
    SFRotation result(*this);
    result.d_x[3] = -d_x[3];
    return result;
}

namespace {
    void multQuat(const float quat1[4], const float quat2[4], float result[4]) {
        result[3] = quat1[3]*quat2[3] - quat1[0]*quat2[0]
                - quat1[1]*quat2[1] - quat1[2]*quat2[2];

        result[0] = quat1[3]*quat2[0] + quat1[0]*quat2[3]
                + quat1[1]*quat2[2] - quat1[2]*quat2[1];

        result[1] = quat1[3]*quat2[1] + quat1[1]*quat2[3]
                + quat1[2]*quat2[0] - quat1[0]*quat2[2];

        result[2] = quat1[3]*quat2[2] + quat1[2]*quat2[3]
                + quat1[0]*quat2[1] - quat1[1]*quat2[0];
    }
}

/**
 * @brief Multiply two rotations.
 *
 * @param rotation the rotation by which to multiply this one
 *
 * @return the result rotation
 */
const OpenVRML::SFRotation
        OpenVRML::SFRotation::multiply(const SFRotation & rot) const {
    // convert to quaternions
    float quatUS[4], quatVec[4];
    this->toQuaternion(quatUS);
    rot.toQuaternion(quatVec);
    
    // multiply quaternions
    float resultQuat[4];
    multQuat(quatUS, quatVec, resultQuat);
    
    // now convert back to axis/angle
    SFRotation result;
    result.fromQuaternion(resultQuat);
    return result;
}

void OpenVRML::SFRotation::toQuaternion(float theQuat[4]) const {
    const float sintd2 = sin(d_x[3] * 0.5);
    const float len =
            sqrt((d_x[0] * d_x[0]) + (d_x[1] * d_x[1]) + (d_x[2] * d_x[2]));
    const float f = sintd2 / len;
    theQuat[3] = cos(d_x[3] * 0.5);
    theQuat[0] = d_x[0] * f;
    theQuat[1] = d_x[1] * f;
    theQuat[2] = d_x[2] * f;
}

void OpenVRML::SFRotation::fromQuaternion(const float theQuat[4]) {
    double sina2 = sqrt(theQuat[0]*theQuat[0] + theQuat[1]*theQuat[1]
                    + theQuat[2]*theQuat[2]);
    const double angle = (2.0*atan2(sina2, theQuat[3]));

    if (sina2 >= 1e-8) {
	sina2 = 1.0/sina2;
        this->d_x[0] = theQuat[0] * sina2;
        this->d_x[1] = theQuat[1] * sina2;
        this->d_x[2] = theQuat[2] * sina2;
        this->d_x[3] = angle;
    } else {
        this->d_x[0] = 0.0;
        this->d_x[1] = 1.0;
        this->d_x[2] = 0.0;
        this->d_x[3] = 0.0;
    }
}

/**
 * @brief Multiply the matrix corresponding to this rotation by a vector.
 *
 * @todo IMPLEMENT ME!
 *
 * @param vec vector by which to multiply this rotation
 * @return the result of multiplying this rotation by vec
 */
const OpenVRML::SFVec3f
        OpenVRML::SFRotation::multVec(const SFVec3f & vec) const {
    return SFVec3f();
}

/**
 * @brief Perform a <b>S</b>pherical <b>L</b>inear Int<b>ERP</b>olation.
 *
 * @param destRotation the destination rotation
 * @param t the interval fraction
 */
const OpenVRML::SFRotation
        OpenVRML::SFRotation::slerp(const SFRotation & destRotation,
                                    float t) const {
    float fromQuat[4], toQuat[4];
    this->toQuaternion(fromQuat);
    destRotation.toQuaternion(toQuat);
    
    //
    // Calculate cosine.
    //
    double cosom = std::inner_product(fromQuat, fromQuat + 4, toQuat, 0.0);
    
    //
    // Adjust signs (if necessary).
    //
    float to1[4];
    if (cosom < 0.0) {
        cosom = -cosom;
        to1[0] = -toQuat[0];
        to1[1] = -toQuat[1];
        to1[2] = -toQuat[2];
        to1[3] = -toQuat[3];
    } else {
        std::copy(toQuat, toQuat + 4, to1);
    }
    
    //
    // Calculate coefficients.
    //
    double scale0, scale1;
    if ((1.0 - cosom) > FPTOLERANCE) {
        const double omega = acos(cosom);
        const double sinom = sin(omega);
        scale0 = sin((1.0 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    } else {
        //
        // "From" and "to" quaternions are very close, so do linear
        // interpolation.
        //
        scale0 = 1.0 - t;
        scale1 = t;
    }
    
    //
    // Calculate the final values.
    //
    float resultQuat[4];
    resultQuat[0] = (scale0 * fromQuat[0]) + (scale1 * to1[0]);
    resultQuat[1] = (scale0 * fromQuat[1]) + (scale1 * to1[1]);
    resultQuat[2] = (scale0 * fromQuat[2]) + (scale1 * to1[2]);
    resultQuat[3] = (scale0 * fromQuat[3]) + (scale1 * to1[3]);
    
    SFRotation result;
    result.fromQuaternion(resultQuat);
    
    return result;
}


/**
 * @class OpenVRML::SFString
 *
 * @brief Encapsulates an SFString.
 */

/**
 * @brief Constructor.
 *
 * @param value
 */
OpenVRML::SFString::SFString(const std::string & value): value(value) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFString::~SFString() {}

/**
 * @brief Get value.
 *
 * @return a string
 */
const std::string & OpenVRML::SFString::get() const {
    return this->value;
}

/**
 * @brief Set value.
 *
 * @param value
 */
void OpenVRML::SFString::set(const std::string & value) {
    this->value = value;
}

ostream & OpenVRML::SFString::print(ostream& os) const
{ return (os << '\"' << this->value.c_str() << '\"'); }

OpenVRML::FieldValue * OpenVRML::SFString::clone() const {
    return new SFString(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFString::fieldType() const {
    return SFSTRING;
}


/**
 * @class OpenVRML::SFTime
 *
 * @brief Encapsulates an SFTime value.
 */

/**
 * @brief Constructor
 *
 * @param value initial value
 */
OpenVRML::SFTime::SFTime(double value): d_value(value) {}

/**
 * @brief Destructor.
 */
OpenVRML::SFTime::~SFTime() {}

ostream & OpenVRML::SFTime::print(ostream& os) const
{ return (os << d_value); }

OpenVRML::FieldValue * OpenVRML::SFTime::clone() const {
    return new SFTime(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::SFTime::fieldType() const {
    return SFTIME;
}

/**
 * @brief Get value.
 *
 * @return the value.
 */
double OpenVRML::SFTime::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new value
 */
void OpenVRML::SFTime::set(double value) {
    this->d_value = value;
}


/**
 * @class OpenVRML::SFVec2f
 *
 * @brief Encapsulates a SFVec2f value.
 */

/**
 * @brief Construct a SFVec2f with the default values, (0, 0).
 */
OpenVRML::SFVec2f::SFVec2f() {
    this->d_x[0] = this->d_x[1] = 0;
}
        
/**
 * @brief Construct a SFVec2f.
 *
 * @param vec a 2-element array
 */
OpenVRML::SFVec2f::SFVec2f(const float vec[2]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
}
        
/**
 * @brief Construct a SFVec2f.
 *
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 */
OpenVRML::SFVec2f::SFVec2f(float x, float y) {
    this->d_x[0] = x;
    this->d_x[1] = y;
}

/**
 * @brief Destructor.
 */
OpenVRML::SFVec2f::~SFVec2f() {}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 1. 0 corresponds to the
 *              <var>x</var>-component, and 1 corresponds to the
 *              <var>y</var>-component.
 */
float OpenVRML::SFVec2f::operator[](size_t index) const {
    assert(index < 2);
    return this->d_x[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 *
 * @param index a value from 0 - 1. 0 corresponds to the
 *              <var>x</var>-component, and 1 corresponds to the
 *              <var>y</var>-component.
 */
float & OpenVRML::SFVec2f::operator[](size_t index) {
    assert(index < 2);
    return this->d_x[index];
}

ostream & OpenVRML::SFVec2f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1]); }

OpenVRML::FieldValue * OpenVRML::SFVec2f::clone() const {
    return new SFVec2f(*this);
}

/**
 * @brief Get the field type.
 *
 * @return the FieldType enumerant corresponding to this field type
 */
OpenVRML::FieldValue::FieldType OpenVRML::SFVec2f::fieldType() const {
    return SFVEC2F;
}

/**
 * @brief Get the x component.
 */
float OpenVRML::SFVec2f::getX() const {
    return this->d_x[0];
}
    
/**
 * @brief Set the x component.
 */
void OpenVRML::SFVec2f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the y component.
 */
float OpenVRML::SFVec2f::getY() const {
    return this->d_x[1];
}
        
/**
 * @brief Set the y component.
 */
void OpenVRML::SFVec2f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the value of this vector.
 *
 * @returns a reference to a 2-element array.
 */
const float (&OpenVRML::SFVec2f::get() const)[2] {
    return this->d_x;
}

/**
 * @brief Set the value of this vector.
 *
 * @param vec a 2-element array.
 */
void OpenVRML::SFVec2f::set(const float vec[2]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
}

/**
 * @brief Add two vectors.
 *
 * @param vec the vector to add to this one.
 *
 * @return a SFVec2f with a value that is the passed SFVec2f added,
 *         componentwise, to this object.
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::add(const SFVec2f & vec) const {
    SFVec2f result(*this);
    result.d_x[0] += this->d_x[0];
    result.d_x[1] += this->d_x[1];
    return result;
}

/**
 * @brief Divide this vector by a scalar.
 * @param number a scalar value.
 * @return a SFVec2f with a value that is the object divided by the
 *         passed numeric value.
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::divide(float number) const {
    SFVec2f result(*this);
    result.d_x[0] /= number;
    result.d_x[1] /= number;
    return result;
}

/**
 * @brief Dot product.
 *
 * @param vec
 *
 * @return the dot product of this vector and vec.
 */
double OpenVRML::SFVec2f::dot(const SFVec2f & vec) const {
    return (this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1]);
}

/**
 * @brief Geometric length.
 *
 * @return the length of this vector.
 */
double OpenVRML::SFVec2f::length() const
{
  return sqrt(d_x[0] * d_x[0] + d_x[1] * d_x[1]);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param number a scalar value
 *
 * @return a SFVec2f with a value that is the object multiplied by the
 *         passed numeric value.
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::multiply(float number) const {
    SFVec2f result(*this);
    result.d_x[0] *= number;
    result.d_x[1] *= number;
    return result;
}

/**
 * @brief Negate.
 *
 * @return a SFVec2f that the result of negating this vector.
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::negate() const {
    SFVec2f result;
    result.d_x[0] = -this->d_x[0];
    result.d_x[1] = -this->d_x[1];
    return result;
}

/**
 * @brief Normalize.
 *
 * @return a SFVec2f that is this vector normalized.
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::normalize() const {
    const double len = this->length();
    if (fpzero(len)) {
        return *this;
    }
    SFVec2f result(*this);
    result.d_x[0] /= len;
    result.d_x[1] /= len;
    return result;
}

/**
 * @brief Take the difference of two vectors.
 * @param vec the vector to subtract from this one
 * @return a SFVec2f that is the difference between this vector and vec
 */
const OpenVRML::SFVec2f OpenVRML::SFVec2f::subtract(const SFVec2f & vec) const {
    SFVec2f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    return result;
}


/**
 * @class OpenVRML::SFVec3f
 *
 * @brief Encapsulates a SFVec3f value.
 */

/**
 * @brief Construct a SFVec3f with the default value, (0, 0, 0).
 */
OpenVRML::SFVec3f::SFVec3f() {}

/**
 * @brief Construct a SFVec3f.
 *
 * @param vec a 3-element array
 */
OpenVRML::SFVec3f::SFVec3f(const float vec[3]) {
    std::copy(vec, vec + 3, this->d_x);
}

/**
 * @brief Construct a SFVec3f.
 *
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 * @param z the <var>z</var>-component
 */
OpenVRML::SFVec3f::SFVec3f(float x, float y, float z)
{ d_x[0] = x; d_x[1] = y; d_x[2] = z; }

/**
 * @brief Destructor.
 */
OpenVRML::SFVec3f::~SFVec3f() {}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 2. 0 corresponds to the
 *              <var>x</var>-component, 1 corresponds to the
 *              <var>y</var>-component, and 2 corresponds to the
 *              <var>z</var>-component.
 */
float OpenVRML::SFVec3f::operator[](size_t index) const {
    assert(index < 3);
    return this->d_x[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 *
 * @param index a value from 0 - 2. 0 corresponds to the
 *              <var>x</var>-component, 1 corresponds to the
 *              <var>y</var>-component, and 2 corresponds to the
 *              <var>z</var>-component.
 */
float & OpenVRML::SFVec3f::operator[](size_t index) {
    assert(index < 3);
    return this->d_x[index];
}

ostream& OpenVRML::SFVec3f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1] << " " << d_x[2]); }

OpenVRML::FieldValue * OpenVRML::SFVec3f::clone() const
{ return new SFVec3f(*this); }

OpenVRML::FieldValue::FieldType OpenVRML::SFVec3f::fieldType() const {
    return SFVEC3F;
}

/**
 * @brief Get the <var>x</var>-component
 *
 * @return the <var>x</var>-component of this vector
 */
float OpenVRML::SFVec3f::getX() const {
    return this->d_x[0];
}

/**
 * @brief Set the <var>x</var>-component
 *,
 * @param value
 */
void OpenVRML::SFVec3f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the <var>y</var>-component
 *
 * @return the <var>y</var>-component of this vector
 */
float OpenVRML::SFVec3f::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component
 *
 * @param value
 */
void OpenVRML::SFVec3f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the <var>z</var>-component
 *
 * @return the <var>z</var>-component of this vector
 */
float OpenVRML::SFVec3f::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component
 *
 * @param value
 */
void OpenVRML::SFVec3f::setZ(float value) {
    this->d_x[2] = value;
}

/**
 * @brief Get the vector value.
 *
 * @return a reference to a 3-element array
 */
const float (&OpenVRML::SFVec3f::get() const)[3] {
    return this->d_x;
}

/**
 * @brief Set the vector value.
 *
 * @param vec a 3-element array
 */
void OpenVRML::SFVec3f::set(const float vec[3]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
    this->d_x[2] = vec[2];
}

/**
 * @brief Add this vector and vec component-wise.
 *
 * @param vec
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::add(const SFVec3f & vec) const {
    SFVec3f result(*this);
    result.d_x[0] += vec.d_x[0];
    result.d_x[1] += vec.d_x[1];
    result.d_x[2] += vec.d_x[2];
    return result;
}

/**
 * @brief Get the cross product of this vector and vec.
 *
 * @param vec
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::cross(const SFVec3f & vec) const {
    SFVec3f result;
    result.d_x[0] = (this->d_x[1] * vec.d_x[2]) - (this->d_x[2] * vec.d_x[1]);
    result.d_x[1] = (this->d_x[2] * vec.d_x[0]) - (this->d_x[0] * vec.d_x[2]);
    result.d_x[2] = (this->d_x[0] * vec.d_x[1]) - (this->d_x[1] * vec.d_x[0]);
    return result;
}

/**
 * @brief Get the result of dividing this vector by number.
 *
 * @param number
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::divide(float number) const {
    SFVec3f result(*this);
    result.d_x[0] /= number;
    result.d_x[1] /= number;
    result.d_x[2] /= number;
    return result;
}

/**
 * @brief Get the dot product of this vector and vec.
 *
 * @param vec
 */
double OpenVRML::SFVec3f::dot(const SFVec3f & vec) const {
    return ((this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1])
            + (this->d_x[2] * vec.d_x[2]));
}

/**
 * @brief Get the length of this vector.
 *
 * @return the geometric length of the vector.
 */
double OpenVRML::SFVec3f::length() const {
    const double len = sqrt((d_x[0] * d_x[0])
                          + (d_x[1] * d_x[1])
                          + (d_x[2] * d_x[2]));
    return fpzero(len) ? 0.0 : len;
}

/**
 * @brief Multiply by a scalar.
 *
 * @param number
 *
 * @return the product
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::multiply(float number) const {
    SFVec3f result(*this);
    result.d_x[0] *= number;
    result.d_x[1] *= number;
    result.d_x[2] *= number;
    return result;
}

/**
 * @brief Negate.
 *
 * @return the negatation of this vector
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::negate() const {
    SFVec3f result(*this);
    result.d_x[0] = -result.d_x[0];
    result.d_x[1] = -result.d_x[1];
    result.d_x[2] = -result.d_x[2];
    return result;
}

/**
 * @brief Normalize.
 *
 * @return a copy of this vector normalized
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::normalize() const {
    const double len = this->length();
    SFVec3f result(*this);
    if (!fpzero(len)) {
        result.d_x[0] /= len;
        result.d_x[1] /= len;
        result.d_x[2] /= len;
    }
  
    return result;
}

/**
 * @brief Subtract.
 *
 * @param vec
 *
 * @return the difference between this vector and vec
 */
const OpenVRML::SFVec3f OpenVRML::SFVec3f::subtract(const SFVec3f & vec) const {
    SFVec3f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    result.d_x[2] -= vec.d_x[2];
    return result;
}


/**
 * @class OpenVRML::MFColor
 *
 * @brief Encapsulates a MFColor.
 */

class OpenVRML::MFColor::FData { // reference counted float data
public:
  FData(size_t n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
  ~FData() { delete [] d_v; }

  FData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  size_t d_refs;			// number of objects using this data
  size_t d_n;			// size (in floats) of d_v
  float *d_v;			// data vector
};

/**
 * @brief Construct from a float array.
 *
 * @param length the number of RGB triplets in the array
 * @param colors a float array comprising RGB triplets
 */
OpenVRML::MFColor::MFColor(size_t length, float const * colors):
        d_data(new FData(length * 3)) {
    if (colors) {
        std::copy(colors, colors + (length * 3), this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfColor the object to copy
 */
OpenVRML::MFColor::MFColor(const MFColor & mfColor):
        d_data(mfColor.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFColor::~MFColor() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param rhs value to assign to this object
 *
 * @return a reference to this object
 */
OpenVRML::MFColor & OpenVRML::MFColor::operator=(const MFColor & rhs) {
  if (this != &rhs) {
    d_data->deref();
    d_data = rhs.d_data->ref();
  }
  return *this;
}

/**
 * @brief Get value.
 *
 * @return a pointer to an array comprising RGB triplets
 */
const float * OpenVRML::MFColor::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 *
 * Copies the contents of a <code>float</code> array.
 *
 * @param length the number of RGB triplets in the array
 * @param colors an array comprising RGB triplets
 */
void OpenVRML::MFColor::set(size_t length, const float * colors) {
    length *= 3;
    this->d_data->deref();
    this->d_data = new FData(length);
    if (colors) {
        std::copy(colors, colors + (length), this->d_data->d_v);
    } else {
        this->setLength(length);
    }
}

/**
 * @brief Get element.
 *
 * @return a pointer to a 3-element array comprising an RGB triplet
 */
const float * OpenVRML::MFColor::getElement(size_t index) const {
    assert(index < this->getLength());
    return (this->d_data->d_v + (index * 3L));
}

/**
 * @brief Set element.
 *
 * @param index the index of the element to set
 * @param value a 3-element float array comprising the new color value
 */
void OpenVRML::MFColor::setElement(size_t index, const float value[3]) {
    assert(index < this->getLength());
    std::copy(value, value + 3, this->d_data->d_v + (index * 3));
}

/**
 * @brief Get the length.
 *
 * @return the number of color values (RGB triplets)
 */
size_t OpenVRML::MFColor::getLength() const {
    return (this->d_data->d_n / 3L);
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default color, (0, 0, 0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 */
void OpenVRML::MFColor::setLength(size_t length) {
    length *= 3;
    FData * const newData = new FData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        std::fill(newData->d_v + this->d_data->d_n, newData->d_v + length,
                  0.0f);
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFColor::insertElement(size_t index, const float value[3])
{
  FData* newData;

  newData = new FData(d_data->d_n + 3);
  memcpy(newData->d_v, d_data->d_v, 3 * index * sizeof(float));
  memcpy(newData->d_v + 3 * index, value, 3 * sizeof(float));
  memcpy(newData->d_v + 3 * (index + 1), d_data->d_v + 3 * index, 
	 (d_data->d_n - 3 * index) * sizeof(float));
  d_data->deref();
  d_data = newData;
}

void OpenVRML::MFColor::removeElement(size_t index)
{
  if (3 * index < d_data->d_n)
  {
    d_data->d_n -= 3;
    memcpy(d_data->d_v + 3 * index, d_data->d_v + 3 * (index + 1), 
	   (d_data->d_n - 3 * index) * sizeof(float));
  }	
}

OpenVRML::FieldValue * OpenVRML::MFColor::clone() const {
    return new MFColor(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFColor::fieldType() const {
    return MFCOLOR;
}

ostream & OpenVRML::MFColor::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 3L); }


/**
 * @class OpenVRML::MFFloat
 *
 * @brief Encapsulates a MFFloat.
 */

class OpenVRML::MFFloat::FData { // reference counted float data
public:
  FData(int n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
  ~FData() { delete [] d_v; }

  FData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  int d_refs;			// number of MF* objects using this data
  int d_n;			// size (in floats) of d_v
  float *d_v;			// data vector
};

/**
 * @brief Construct from a float array.
 *
 * @param length the number of floats in the array
 * @param numbers a pointer to a float array
 */
OpenVRML::MFFloat::MFFloat(size_t length, float const * numbers):
        d_data(new FData(length)) {
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfFloat the object to copy
 */
OpenVRML::MFFloat::MFFloat(const MFFloat & mfFloat):
        d_data(mfFloat.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFFloat::~MFFloat() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfFloat the object to copy
 */
OpenVRML::MFFloat & OpenVRML::MFFloat::operator=(const MFFloat & mfFloat) {
    if (this != &mfFloat) {
        this->d_data->deref();
        this->d_data = mfFloat.d_data->ref();
    }
    return *this;
}

/**
 * @brief Get value.
 *
 * @return a pointer to a float array
 */
const float * OpenVRML::MFFloat::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 *
 * @param length the number of float values
 * @param numbers a pointer to a float array
 */
void OpenVRML::MFFloat::set(size_t length, const float * numbers) {
    this->d_data->deref();
    this->d_data = new FData(length);
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
float OpenVRML::MFFloat::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFFloat::setElement(size_t index, float value) {
    assert(index < this->getLength());
    this->d_data->d_v[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t OpenVRML::MFFloat::getLength() const {
    return this->d_data->d_n;
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default (0.0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 */
void OpenVRML::MFFloat::setLength(size_t length) {
    FData * const newData = new FData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        std::fill(newData->d_v + this->d_data->d_n, newData->d_v + length,
                  0.0f);
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFFloat::insertElement(size_t index, float value)
{
  FData* newData;

  newData = new FData(d_data->d_n + 1);
  memcpy(newData->d_v, d_data->d_v, index * sizeof(float));
  newData->d_v[index] = value;
  memcpy(newData->d_v + (index + 1), d_data->d_v + index, 
	 (d_data->d_n - index) * sizeof(float));
  d_data->deref();
  d_data = newData;
}

void OpenVRML::MFFloat::removeElement(size_t index)
{
  if (index < d_data->d_n)
  {
    d_data->d_n--;
    memcpy(d_data->d_v + index, d_data->d_v + (index + 1), 
	   (d_data->d_n - index) * sizeof(float));
  }	
}

OpenVRML::FieldValue * OpenVRML::MFFloat::clone() const {
    return new MFFloat(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFFloat::fieldType() const {
    return MFFLOAT;
}

ostream & OpenVRML::MFFloat::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 1); }


/**
 * @class OpenVRML::MFInt32
 *
 * @brief Encapsulates a MFInt32.
 */

class OpenVRML::MFInt32::IData { // reference counted int data
public:
  IData(long n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new long[n] : 0) {}
  ~IData() { delete [] d_v; }

  IData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  long d_refs;    // number of MFInt objects using this data
  size_t d_n;     // size (in ints) of d_v
  long * d_v;     // data vector
};

/**
 * @brief Construct from a long array.
 *
 * @param length the number of integer values
 * @param numbers a pointer to a long array
 */
OpenVRML::MFInt32::MFInt32(size_t length, const long * numbers):
        d_data(new IData(length)) {
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfInt32 the object to copy
 */
OpenVRML::MFInt32::MFInt32(const MFInt32 & mfInt32):
        d_data(mfInt32.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFInt32::~MFInt32() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfInt32 the object to copy into this one
 *
 * @return a reference to this object
 */
OpenVRML::MFInt32 & OpenVRML::MFInt32::operator=(const MFInt32 & mfInt32)
{
  if (this != &mfInt32) {
    d_data->deref();
    d_data = mfInt32.d_data->ref();
  }
  return *this;
}

/**
 * @brief Get value.
 *
 * @return a pointer to the long array comprising the integer values owned by
 *         this object
 */
const long * OpenVRML::MFInt32::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 *
 * @param length the number of integer values
 * @param numbers a pointer to a long array
 */
void OpenVRML::MFInt32::set(size_t length, const long * numbers) {
    d_data->deref();
    d_data = new IData(length);
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
long OpenVRML::MFInt32::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFInt32::setElement(size_t index, long value) {
    assert(index < this->getLength());
    this->d_data->d_v[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of integer values
 */
size_t OpenVRML::MFInt32::getLength() const {
    return this->d_data->d_n;
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default (0.0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 */
void OpenVRML::MFInt32::setLength(size_t length) {
    IData * const newData = new IData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        std::fill(newData->d_v + this->d_data->d_n, newData->d_v + length, 0L);
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFInt32::insertElement(size_t index, long value)
{
  IData* newData;

  newData = new IData(d_data->d_n + 1);
  memcpy(newData->d_v, d_data->d_v, index * sizeof(long));
  newData->d_v[index] = value;
  memcpy(newData->d_v + (index + 1), d_data->d_v + index, 
	 (d_data->d_n - index) * sizeof(long));
  d_data->deref();
  d_data = newData;
}

void OpenVRML::MFInt32::removeElement(size_t index)
{
  if (index < d_data->d_n)
  {
    (d_data->d_n)--;
    memcpy(d_data->d_v + index, d_data->d_v + (index + 1), 
	   (d_data->d_n - index) * sizeof(int));
  }
}

OpenVRML::FieldValue * OpenVRML::MFInt32::clone() const {
    return new MFInt32(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFInt32::fieldType() const {
    return MFINT32;
}

ostream & OpenVRML::MFInt32::print(ostream& os) const
{
  size_t n = this->getLength();
  const long * c = get();

  if (n == 1)
    os << *c;
  else
    {
      os << '[';
      for (size_t i=0; i<n; ++i, ++c)
	{
	  os << *c;
	  os << ((i < n-1) ? ", " : " ");
	}
      os << ']';
    }

  return os;
}


/**
 * @class OpenVRML::MFNode
 *
 * @brief Encapsulates a MFNode.
 *
 * Since individual nodes are refcounted, no attempt is made to refcount
 * MFNode.
 */

/**
 * @brief Construct from an array of Node pointers.
 *
 * @param length the length of the array
 * @param nodes a pointer to an array of Node pointers
 */
OpenVRML::MFNode::MFNode(const size_t length, const NodePtr * nodes):
        nodes(length) {
    if (nodes) {
        std::copy(nodes, nodes + length, this->nodes.begin());
    }
}

/**
 * @brief Destructor.
 */
OpenVRML::MFNode::~MFNode() {}

/**
 * @brief Get element.
 *
 * @param index
 * @return a smart pointer to a Node
 */
const OpenVRML::NodePtr &
        OpenVRML::MFNode::getElement(const size_t index) const throw () {
    assert(index < this->nodes.size());
    return this->nodes[index];
}

/**
 * @brief Remove all elements.
 */
void OpenVRML::MFNode::clear()
{
    this->setLength(0);
}

/**
 * @brief Set element.
 *
 * @param index
 * @param node
 */
void OpenVRML::MFNode::setElement(const size_t index, const NodePtr & node) {
    assert(index < this->nodes.size());
    this->nodes[index] = node;
}

/**
 * @brief Get the length.
 *
 * @return the number of nodes in the array
 */
size_t OpenVRML::MFNode::getLength() const throw () {
    return this->nodes.size();
}

/**
 * @brief Set the length.
 *
 * Set the length of the node array. If the new length is less than the
 * current length, the array is truncated. If the length is greater than
 * the current length, the new positions at the end of the array are filled
 * with null NodePtrs.
 *
 * @param length the new length
 */
void OpenVRML::MFNode::setLength(const size_t length) {
    this->nodes.resize(length);
}

/**
 * @brief Determine if a node exists in this MFNode.
 *
 * @param node
 */
bool OpenVRML::MFNode::exists(const Node & node) const {
    for (std::vector<NodePtr>::const_iterator i(this->nodes.begin());
            i != this->nodes.end(); ++i) {
        if (i->get() == &node) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Add a node.
 *
 * Add <var>node</var> to the array if it isn't already part of this MFNode.
 * This method will <strong>not</strong> add NULLs.
 *
 * @param node a pointer to the node to add
 *
 * @return <code>true</code> if a node was added, <code>false</code>
 *         otherwise
 */
bool OpenVRML::MFNode::addNode(Node & node) {
    if (!this->exists(node)) {
        this->nodes.push_back(NodePtr(&node));
        return true;
    }
    
    return false;
}

/**
 * @brief Remove a node.
 *
 * Remove <var>node</var> from the array, if it exists here. This method
 * will <strong>not</strong> remove NULLs.
 *
 * @param node a pointer to the node to remove
 * @return <code>true</code> if a node was removed, <code>false</code>
 *         otherwise
 */
bool OpenVRML::MFNode::removeNode(const Node & node) {
    for (std::vector<NodePtr>::iterator i(this->nodes.begin());
            i != this->nodes.end(); ++i) {
        if (i->get() == &node) {
            this->nodes.erase(i);
	    return true;
        }
    }
    return false;
}

void OpenVRML::MFNode::insertElement(size_t index, const NodePtr & node)
{
  assert(index < this->nodes.size());
  this->nodes.insert(this->nodes.begin() + index, node);
}

void OpenVRML::MFNode::removeElement(size_t index)
{
  assert(index < this->nodes.size());
  this->nodes.erase(this->nodes.begin() + index);
}

OpenVRML::FieldValue * OpenVRML::MFNode::clone() const
{ return new MFNode(*this); }

OpenVRML::FieldValue::FieldType OpenVRML::MFNode::fieldType() const {
    return MFNODE;
}

ostream & OpenVRML::MFNode::print(ostream & os) const {
    if (this->nodes.size() != 1) {
        os << '[';
    }
    for (std::vector<NodePtr>::const_iterator i(this->nodes.begin());
            i != this->nodes.end(); ++i) {
        os << **i << endl;
    }
    if (this->nodes.size() != 1) {
        os << ']';
    }
    
    return os;
}


/**
 * @class OpenVRML::MFRotation
 *
 * @brief Encapsulates an MFRotation.
 */

class OpenVRML::MFRotation::FData { // reference counted float data
public:
  FData(int n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
  ~FData() { delete [] d_v; }

  FData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  int d_refs;			// number of MF* objects using this data
  int d_n;			// size (in floats) of d_v
  float *d_v;			// data vector
};

/**
 * @brief Construct from an array of rotation values.
 *
 * @param length the number of rotation values in the passed array
 * @param rotations a pointer to an array of rotation values
 */
OpenVRML::MFRotation::MFRotation(size_t length, const float * rotations):
        d_data(new FData(length * 4)) {
    if (rotations) {
        std::copy(rotations, rotations + (length * 4), this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfrotation the object to copy
 */
OpenVRML::MFRotation::MFRotation(const MFRotation & mfrotation):
        d_data(mfrotation.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFRotation::~MFRotation() {
    this->d_data->deref();
}

/**
 * @brief Assignment operator.
 *
 * @param mfrotation the object to copy into this one
 */
OpenVRML::MFRotation &
        OpenVRML::MFRotation::operator=(const MFRotation & mfrotation) {
    if (this != &mfrotation) {
        this->d_data->deref();
        this->d_data = mfrotation.d_data->ref();
    }
    return *this;
}

/**
 * @brief Get the rotations.
 *
 * @return a pointer to an array of rotation values
 */
const float * OpenVRML::MFRotation::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the rotation values.
 *
 * @param length the number of rotation values in the passed array
 * @param rotations a pointer to an array of rotation values
 */
void OpenVRML::MFRotation::set(size_t length, const float * rotations) {
    this->d_data->deref();
    this->d_data = new FData(length * 4);
    if (rotations) {
        std::copy(rotations, rotations + (length * 4), this->d_data->d_v);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
const float * OpenVRML::MFRotation::getElement(size_t index) const {
    assert(index * 4 < this->d_data->d_n);
    return this->d_data->d_v + (index * 4);
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFRotation::setElement(size_t index, const float value[4]) {
    assert(index * 4 < this->d_data->d_n);
    std::copy(value, value + 4, this->d_data->d_v + (index * 4));
}

/**
 * @brief Get the length.
 *
 * @return the number of rotation values
 */
size_t OpenVRML::MFRotation::getLength() const {
    return (this->d_data->d_n / 4);
}

/**
 * @brief Set the length.
 *
 * If the new length is smaller than the current length, the array is
 * truncated. If the new length is greater than the current length, the
 * new values are initialized to the default rotation (0, 0, 1, 0).
 *
 * @param length the new length
 */
void OpenVRML::MFRotation::setLength(size_t length) {
    length *= 4;
    FData * const newData = new FData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        for (size_t i = this->d_data->d_n; i < length; i += 4) {
            newData->d_v[i] = 0.0f;
            newData->d_v[i + 1] = 0.0f;
            newData->d_v[i + 2] = 1.0f;
            newData->d_v[i + 3] = 0.0f;
        }
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFRotation::insertElement(size_t index, const float value[3])
{
  FData* newData;

  newData = new FData(d_data->d_n + 4);
  memcpy(newData->d_v, d_data->d_v, 4 * index * sizeof(float));
  memcpy(newData->d_v + 4 * index, value, 4 * sizeof(float));
  memcpy(newData->d_v + 4 * (index + 1), d_data->d_v + 4 * index, 
	 (d_data->d_n - 4 * index) * sizeof(float));
  d_data->deref();
  d_data = newData;
  d_data->d_n++;
}

void OpenVRML::MFRotation::removeElement(size_t index)
{
  if (4 * index < d_data->d_n)
  {
    d_data->d_n -= 4;
    memcpy(d_data->d_v + 4 * index, d_data->d_v + 4 * (index + 1),
	   (d_data->d_n - 4 * index) * sizeof(float));
  }
}

OpenVRML::FieldValue * OpenVRML::MFRotation::clone() const {
    return new MFRotation(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFRotation::fieldType() const {
    return MFROTATION;
}

ostream & OpenVRML::MFRotation::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 4); }


/**
 * @class OpenVRML::MFString
 *
 * @brief Encapsulates a MFString.
 */

/**
 * @brief Constructor.
 *
 * @param length the length of the passed array
 * @param values an array of std::string
 */
OpenVRML::MFString::MFString(size_t length, const std::string * values):
        values(length) {
    if (values) {
        std::copy(values, values + length, this->values.begin());
    }
}

/**
 * @brief Destructor.
 */
OpenVRML::MFString::~MFString() {}

/**
 * @brief Get an element of the array.
 *
 * @param index the index of the element to retrieve
 *
 * @return the array element
 */
const std::string & OpenVRML::MFString::getElement(size_t index) const {
    assert(index < this->values.size());
    return this->values[index];
}

/**
 * @brief Set an element of the array.
 *
 * @param index the index of the element to set
 * @param value the new value
 */
void OpenVRML::MFString::setElement(size_t index, const std::string & value) {
    assert(index < this->values.size());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of values in the array
 */
size_t OpenVRML::MFString::getLength() const {
    return this->values.size();
}

/**
 *
 * Set the length of the vector of std::strings. If the new length is
 * greater than the current length, the new positions are filled with
 * empty std::strings.
 *
 * @param length the new length
 */
void OpenVRML::MFString::setLength(const size_t length) {
  this->values.resize(length);
}

void OpenVRML::MFString::insertElement(size_t index, const std::string & value)
{
  assert(index < this->values.size());
  this->values.insert(this->values.begin() + index, value);
}

void OpenVRML::MFString::removeElement(size_t index)
{
  assert(index < this->values.size());
  this->values.erase(this->values.begin() + index);
}

OpenVRML::FieldValue * OpenVRML::MFString::clone() const
{ return new MFString(*this); }

OpenVRML::FieldValue::FieldType OpenVRML::MFString::fieldType() const {
    return MFSTRING;
}

ostream & OpenVRML::MFString::print(ostream& os) const
{
  int n = getLength();

  if (n != 1) os << '[';
  for (int i=0; i<n; ++i)
    os << '\"' << (this->values[i]).c_str() << "\" ";
  if (n != 1) os << ']';

  return os;
}


/**
 * @class OpenVRML::MFTime
 *
 * @brief Encapsulates a MFTime.
 */

//
// Reference-counted double data
//
class OpenVRML::MFTime::DData {
public:
    DData(size_t = 0);
    ~DData();

    DData * ref();
    void deref();

    unsigned int refs;      // number of MFTime objects using this data
    size_t size;            // size of d_v
    double * const data;    // data vector

private:
    //
    // This class is not copyable. Copy ctor and operator= are declared
    // private and not defined.
    //
    DData(DData const &);
    DData & operator=(DData const &);
};

OpenVRML::MFTime::DData::DData(size_t size): refs(1), size(size),
        data(size > 0 ? new double[size] : 0) {}

OpenVRML::MFTime::DData::~DData() {
    delete [] data;
}

OpenVRML::MFTime::DData * MFTime::DData::ref() {
    ++refs;
    return this;
}

void OpenVRML::MFTime::DData::deref() {
    if (--refs == 0) {
        delete this;
    }
}

/**
 * @brief Construct from an array of time values.
 *
 * @param length the number of time values in the passed array
 * @param times a pointer to an array of time values
 */
OpenVRML::MFTime::MFTime(size_t length, const double * times):
        d_data(new DData(length)) {
    if (times) {
        std::copy(times, times + length, this->d_data->data);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mftime the object to copy
 */
OpenVRML::MFTime::MFTime(const MFTime & mftime): d_data(mftime.d_data->ref())
{}

/**
 * @brief Destructor.
 */
OpenVRML::MFTime::~MFTime() {
    this->d_data->deref();
}

/**
 * @brief Assignment operator.
 *
 * @param mftime the object to copy into this one
 */
OpenVRML::MFTime & OpenVRML::MFTime::operator=(const MFTime & mftime) {
    if (this != &mftime) {
        d_data->deref();
        d_data = mftime.d_data->ref();
    }
    return *this;
}

/**
 * @brief Get value.
 *
 * @return a pointer to a double array
 */
const double * OpenVRML::MFTime::get() const {
    return this->d_data->data;
}

/**
 * @brief Set value.
 *
 * @param length the number of time values
 * @param times a pointer to a double array
 */
void OpenVRML::MFTime::set(size_t length, const double * times) {
    this->d_data->deref();
    this->d_data = new DData(length);
    if (times) {
        std::copy(times, times + length, this->d_data->data);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
double OpenVRML::MFTime::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->data[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFTime::setElement(size_t index, double value) {
    assert(index < this->getLength());
    this->d_data->data[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t OpenVRML::MFTime::getLength() const {
    return d_data->size;
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default (0.0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 */
void OpenVRML::MFTime::setLength(size_t length) {
    DData * const newData = new DData(length);
    if (length > this->d_data->size) {
        std::copy(this->d_data->data, this->d_data->data + this->d_data->size,
                  newData->data);
        std::fill(newData->data + this->d_data->size, newData->data + length,
                  0.0f);
    } else {
        std::copy(this->d_data->data, this->d_data->data + length,
                  newData->data);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFTime::insertElement(size_t index, double value)
{
  DData* newData;

  newData = new DData(d_data->size + 1);
  memcpy(newData->data, d_data->data, index * sizeof(double));
  newData->data[index] = value;
  memcpy(newData->data + (index + 1), d_data->data + index, 
	 (d_data->size - index) * sizeof(double));
  d_data->deref();
  d_data = newData;
}

void OpenVRML::MFTime::removeElement(size_t index)
{
  if (index < d_data->size)
  {
    d_data->size--;
    memcpy(d_data->data + index, d_data->data + (index + 1), 
	   (d_data->size - index) * sizeof(double));
  }
}

OpenVRML::FieldValue * OpenVRML::MFTime::clone() const {
    return new MFTime(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFTime::fieldType() const {
    return MFTIME;
}

ostream & OpenVRML::MFTime::print(ostream & os) const {
    return mfdprint(os, get(), getLength(), 1);
}


/**
 * @class OpenVRML::MFVec2f
 *
 * @brief Encapsulates an MFVec2f.
 */

class OpenVRML::MFVec2f::FData { // reference counted float data
public:
  FData(size_t n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
  ~FData() { delete [] d_v; }

  FData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  size_t d_refs;			// number of objects using this data
  size_t d_n;			// size (in floats) of d_v
  float *d_v;			// data vector
};

/**
 * @brief Construct from an array of vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of vector values
 */
OpenVRML::MFVec2f::MFVec2f(size_t length, const float * vecs):
        d_data(new FData(length * 2)) {
    if (vecs) {
        std::copy(vecs, vecs + (length * 2), this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfvec2f the object to copy
 */
OpenVRML::MFVec2f::MFVec2f(const MFVec2f & mfvec2f):
        d_data(mfvec2f.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFVec2f::~MFVec2f() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfvec2f the object to copy into this one
 */
OpenVRML::MFVec2f & OpenVRML::MFVec2f::operator=(const MFVec2f & mfvec2f) {
    if (this != &mfvec2f) {
        this->d_data->deref();
        this->d_data = mfvec2f.d_data->ref();
    }
    return *this;
}

/**
 * @brief Get the vector values.
 *
 * @return a pointer to an array of 2-D vector values
 */
const float * OpenVRML::MFVec2f::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of 2-D vector values
 */
void OpenVRML::MFVec2f::set(size_t length, const float * vecs) {
    length *= 2;
    this->d_data->deref();
    this->d_data = new FData(length);
    if (vecs) {
        std::copy(vecs, vecs + length, this->d_data->d_v);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
const float * OpenVRML::MFVec2f::getElement(size_t index) const {
    assert((index * 2) < this->d_data->d_n);
    return (this->d_data->d_v + (index * 2));
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFVec2f::setElement(size_t index, const float value[2]) {
    assert((index * 2) < this->d_data->d_n);
    std::copy(value, value + 2, this->d_data->d_v + (index * 2));
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t OpenVRML::MFVec2f::getLength() const {
    return (this->d_data->d_n / 2);
}

/**
 * @brief Set the length.
 *
 * If the new length is smaller than the current length, the array is
 * truncated. If the new length is greater than the current length, the
 * new values are initialized to the default vector (0, 0).
 *
 * @param length the new length
 */
void OpenVRML::MFVec2f::setLength(size_t length) {
    length *= 2;
    FData * const newData = new FData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        std::fill(newData->d_v + this->d_data->d_n, newData->d_v + length,
                  0.0f);
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFVec2f::insertElement(size_t index, const float data[2])
{
  FData* newData;

  newData = new FData(d_data->d_n + 2);
  memcpy(newData->d_v, d_data->d_v, 2 * index * sizeof(float));
  memcpy(newData->d_v + 2 * index, data, 2 * sizeof(float));
  memcpy(newData->d_v + 2 * (index + 1), d_data->d_v + 2 * index, 
	 (d_data->d_n - 2 * index) * sizeof(float));
  d_data->deref();
  d_data = newData;
  d_data->d_n++;
}

void OpenVRML::MFVec2f::removeElement(size_t index)
{
  if (2 * index < d_data->d_n)
  {
    d_data->d_n -= 2;
    memcpy(d_data->d_v + 2 * index, d_data->d_v + 2 * (index + 1), 
	   (d_data->d_n - 2 * index) * sizeof(float));
  }
}

OpenVRML::FieldValue * OpenVRML::MFVec2f::clone() const {
    return new MFVec2f(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFVec2f::fieldType() const {
    return MFVEC2F;
}

ostream & OpenVRML::MFVec2f::print(ostream & os) const
{ return mffprint(os, get(), getLength(), 2); }


/**
 * @class OpenVRML::MFVec3f
 *
 * @brief Encapsulates an MFVec3f.
 */

class OpenVRML::MFVec3f::FData { // reference counted float data
public:
  FData(size_t n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
  ~FData() { delete [] d_v; }

  FData *ref() { ++d_refs; return this; }
  void deref() { if (--d_refs == 0) delete this; }

  size_t d_refs;			// number of objects using this data
  size_t d_n;			// size (in floats) of d_v
  float *d_v;			// data vector
};

/**
 * @brief Construct from an array of vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of vector values
 */
OpenVRML::MFVec3f::MFVec3f(size_t length, const float * vecs):
        d_data(new FData(length * 3)) {
    if (vecs) {
        std::copy(vecs, vecs + (length * 3), this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfvec3f the object to copy
 */
OpenVRML::MFVec3f::MFVec3f(const MFVec3f & mfvec3f):
        d_data(mfvec3f.d_data->ref()) {}

/**
 * @brief Destructor.
 */
OpenVRML::MFVec3f::~MFVec3f() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfvec3f the object to copy into this one
 */
OpenVRML::MFVec3f & OpenVRML::MFVec3f::operator=(const MFVec3f & mfvec3f) {
    if (this != &mfvec3f) {
        this->d_data->deref();
        this->d_data = mfvec3f.d_data->ref();
    }
    return *this;
}

/**
 * @brief Get the vector values.
 *
 * @return a pointer to an array of 3-D vector values
 */
const float * OpenVRML::MFVec3f::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of 3-D vector values
 */
void OpenVRML::MFVec3f::set(size_t length, const float * vecs) {
    length *= 3;
    this->d_data->deref();
    this->d_data = new FData(length);
    if (vecs) {
        std::copy(vecs, vecs + length, this->d_data->d_v);
    }
}

/**
 * @brief Get element.
 *
 * @param index
 */
const float * OpenVRML::MFVec3f::getElement(size_t index) const {
    assert((index * 3) < this->d_data->d_n);
    return (this->d_data->d_v + (index * 3));
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void OpenVRML::MFVec3f::setElement(size_t index, const float value[3]) {
    assert((index * 3) < this->d_data->d_n);
    std::copy(value, value + 3, this->d_data->d_v + (index * 3));
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t OpenVRML::MFVec3f::getLength() const {
    return (this->d_data->d_n / 3);
}

/**
 * @brief Set the length.
 *
 * If the new length is smaller than the current length, the array is
 * truncated. If the new length is greater than the current length, the
 * new values are initialized to the default vector (0, 0, 0).
 *
 * @param length the new length
 */
void OpenVRML::MFVec3f::setLength(size_t length) {
    length *= 3;
    FData * const newData = new FData(length);
    if (length > this->d_data->d_n) {
        std::copy(this->d_data->d_v, this->d_data->d_v + this->d_data->d_n,
                  newData->d_v);
        std::fill(newData->d_v + this->d_data->d_n, newData->d_v + length,
                  0.0f);
    } else {
        std::copy(this->d_data->d_v, this->d_data->d_v + length, newData->d_v);
    }
    this->d_data->deref();
    this->d_data = newData;
}

void OpenVRML::MFVec3f::insertElement(size_t index, const float value[3])
{
  FData* newData;

  newData = new FData(d_data->d_n + 3);
  memcpy(newData->d_v, d_data->d_v, 3 * index * sizeof(float));
  memcpy(newData->d_v + 3 * index, value, 3 * sizeof(float));
  memcpy(newData->d_v + 3 * (index + 1), d_data->d_v + 3 * index,
	 (d_data->d_n - 3 * index) * sizeof(float));
  d_data->deref();
  d_data = newData;
  d_data->d_n++;
}

void OpenVRML::MFVec3f::removeElement(size_t index)
{
  if (3 * index < d_data->d_n)
  {
    d_data->d_n -= 3;
    memcpy(d_data->d_v + 3 * index, d_data->d_v + 3 * (index + 1),
	   (d_data->d_n - 3 * index) * sizeof(float));
  }
}

OpenVRML::FieldValue * OpenVRML::MFVec3f::clone() const {
    return new MFVec3f(*this);
}

OpenVRML::FieldValue::FieldType OpenVRML::MFVec3f::fieldType() const {
    return MFVEC3F;
}

ostream & OpenVRML::MFVec3f::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 3); }

// Generic MF float and double print functions

namespace {
    
    ostream& mffprint( ostream& os, float const * c, int n, int eltsize )
    {
        int e;
        
        if (n == 1) {
            for (e=0; e<eltsize; ++e) {
                os << c[e] << ((e < eltsize-1) ? " " : "");
            }
        } else {
            os << '[';
            for (int i=0; i<n; ++i, c+=eltsize) {
                for (e=0; e<eltsize; ++e) {
                    os << c[e] << ((e < eltsize-1) ? " " : "");
                }
            
	    os << ((i < n-1) ? ", " : " ");
	    }
            os << ']';
        }
        
        return os;
    }
    
    ostream & mfdprint(ostream & os, double const * c, int n, int eltsize)
    {
        int e;
        
        if (n == 1) {
            for (e=0; e<eltsize; ++e) {
                os << c[e] << ((e < eltsize-1) ? " " : "");
            }
        } else {
            os << '[';
            for (int i=0; i<n; ++i, c+=eltsize) {
                for (e=0; e<eltsize; ++e) {
                    os << c[e] << ((e < eltsize-1) ? " " : "");
                }
            
	    os << ((i < n-1) ? ", " : " ");
	    }
            os << ']';
        }
        
        return os;
    }
}


// Define the const and non-const generic and specific safe downcast methods

#define DOWNCAST(_t) \
const OpenVRML::_t * OpenVRML::FieldValue::to##_t() const { return 0; }    \
      OpenVRML::_t * OpenVRML::FieldValue::to##_t()       { return 0; }    \
const OpenVRML::_t * OpenVRML::_t ::to##_t() const { return this; } \
      OpenVRML::_t * OpenVRML::_t ::to##_t()       { return this; }

DOWNCAST(SFBool)
DOWNCAST(SFColor)
DOWNCAST(SFFloat)
DOWNCAST(SFImage)
DOWNCAST(SFInt32)
DOWNCAST(SFNode)
DOWNCAST(SFRotation)
DOWNCAST(SFString)
DOWNCAST(SFTime)
DOWNCAST(SFVec2f)
DOWNCAST(SFVec3f)

DOWNCAST(MFColor)
DOWNCAST(MFFloat)
DOWNCAST(MFInt32)
DOWNCAST(MFNode)
DOWNCAST(MFRotation)
DOWNCAST(MFString)
DOWNCAST(MFVec2f)
DOWNCAST(MFVec3f)

