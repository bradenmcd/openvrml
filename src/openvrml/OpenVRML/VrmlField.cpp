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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VrmlField.h"
#include "MathUtils.h"
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <string.h>

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
 * @class VrmlField
 * Abstract base class for the VRML field types.
 */

/**
 * @brief Destructor. (Virtualized for abstract base class.)
 */
VrmlField::~VrmlField() {}

/**
 * @fn VrmlField * VrmlField::clone() const
 * @brief Virtual copy constructor.
 * @return a new VrmlField identical to this one.
 */

/**
 * @fn ostream& VrmlField::print(ostream& os) const
 * @brief Print the value of this VrmlField.
 * @param os an ostream
 * @return the ostream
 */

/**
 * @fn VrmlField::VrmlFieldType VrmlField::fieldType() const
 * @brief Get the field type.
 * @return the VrmlFieldType enumerant corresponding to this VrmlField's type
 */

/**
 * @brief A static method to convert a type name to an ID.
 * @param type the (C-style) string name of a VRML field type
 * @return the VrmlFieldType enumerant corresponding to the passed type name
 */
VrmlField::VrmlFieldType VrmlField::fieldType(char const *type) {
    for (size_t i(SFBOOL); i <= MFVEC3F; ++i) {
        if (strcmp(type, ftn[i]) == 0) {
            return static_cast<VrmlFieldType>(i);
        }
    }
    
    return NO_FIELD;
}

/**
 * @brief Get the type name of a field.
 * @return the (C-style) string name of this Field object's type.
 */
char const * VrmlField::fieldTypeName() const {
    assert((sizeof(ftn) / sizeof(char *)) >=
            static_cast<size_t>((this->fieldType() + 1)));
    return ftn[this->fieldType()];
}

/**
 * @fn const VrmlSFBool * VrmlField::toSFBool() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFColor * VrmlField::toSFColor() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFFloat * VrmlField::toSFFloat() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFImage * VrmlField::toSFImage() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFInt32 * VrmlField::toSFInt32() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFNode * VrmlField::toSFNode() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFRotation * VrmlField::toSFRotation() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFString * VrmlField::toSFString() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFTime * VrmlField::toSFTime() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFVec2f * VrmlField::toSFVec2f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlSFVec3f * VrmlField::toSFVec3f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFColor * VrmlField::toMFColor() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFFloat * VrmlField::toMFFloat() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFInt32 * VrmlField::toMFInt32() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFNode * VrmlField::toMFNode() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFRotation * VrmlField::toMFRotation() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFString * VrmlField::toMFString() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFVec2f * VrmlField::toMFVec2f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn const VrmlMFVec3f * VrmlField::toMFVec3f() const
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFBool * VrmlField::toSFBool() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFColor * VrmlField::toSFColor() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFFloat * VrmlField::toSFFloat() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFImage * VrmlField::toSFImage() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFInt32 * VrmlField::toSFInt32() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFNode * VrmlField::toSFNode() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFRotation * VrmlField::toSFRotation() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFString * VrmlField::toSFString() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFTime * VrmlField::toSFTime() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFVec2f * VrmlField::toSFVec2f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlSFVec3f * VrmlField::toSFVec3f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFColor * VrmlField::toMFColor() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFFloat * VrmlField::toMFFloat() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFInt32 * VrmlField::toMFInt32() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFNode * VrmlField::toMFNode() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFRotation * VrmlField::toMFRotation() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFString * VrmlField::toMFString() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFVec2f * VrmlField::toMFVec2f() 
 * @deprecated use <code>dynamic_cast</code>
 */

/**
 * @fn  VrmlMFVec3f * VrmlField::toMFVec3f() 
 * @deprecated use <code>dynamic_cast</code>
 */

// Printing methods

ostream& operator<<(ostream& os, const VrmlField& f)
{ return f.print(os); }


/**
 * @class VrmlSFBool
 * @brief Encapsulates an SFBool value.
 */
#include "VrmlSFBool.h"

/**
 * @brief Constructor.
 * @param value initial value
 */
VrmlSFBool::VrmlSFBool(bool value) : d_value(value) {}

ostream& VrmlSFBool::print(ostream& os) const
{ return (os << (d_value ? "TRUE" : "FALSE")); }

VrmlField * VrmlSFBool::clone() const {
    return new VrmlSFBool(*this);
}

VrmlField::VrmlFieldType VrmlSFBool::fieldType() const { return SFBOOL; }

/**
 * @brief Get the value.
 * @return the value of this SFBool
 */
bool VrmlSFBool::get() const {
    return this->d_value;
}

/**
 * @brief Set the value.
 * @param value the new value
 */
void VrmlSFBool::set(bool value) {
    this->d_value = value;
}


/**
 * @class VrmlSFColor
 * @brief Encapsulates an SFColor value.
 */
#include "VrmlSFColor.h"

/**
 * @brief Construct with the default value, (0, 0, 0).
 */
VrmlSFColor::VrmlSFColor() {
    this->d_rgb[0] = 0.0f;
    this->d_rgb[1] = 0.0f;
    this->d_rgb[2] = 0.0f;
}

/**
 * @brief Construct a VrmlSFColor.
 * @param rgb a 3-element array
 */
VrmlSFColor::VrmlSFColor(const float rgb[3]) {
    this->d_rgb[0] = rgb[0];
    this->d_rgb[1] = rgb[1];
    this->d_rgb[2] = rgb[2];
}

/**
 * @brief Construct a VrmlSFColor
 * @param r red component
 * @param g green component
 * @param b blue component
 */
VrmlSFColor::VrmlSFColor(float r, float g, float b)
{ d_rgb[0] = r; d_rgb[1] = g; d_rgb[2] = b; }

ostream& VrmlSFColor::print(ostream& os) const
{ return (os << d_rgb[0] << ' ' << d_rgb[1] << ' ' << d_rgb[2]); }

VrmlField *VrmlSFColor::clone() const
{ return new VrmlSFColor(d_rgb[0],d_rgb[1],d_rgb[2]); }

VrmlField::VrmlFieldType VrmlSFColor::fieldType() const { return SFCOLOR; }

/**
 * @brief Array element dereference operator (const version).
 * @param index an index from 0 - 2
 */
float VrmlSFColor::operator[](size_t index) const {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 * @param index an index from 0 - 2
 */
float & VrmlSFColor::operator[](size_t index) {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Get the red component.
 * @return the red component value
 */
float VrmlSFColor::getR() const {
    return this->d_rgb[0];
}

/**
 * @brief Get the green component.
 * @return the green component value
 */
float VrmlSFColor::getG() const {
    return this->d_rgb[1];
}

/**
 * @brief Get the blue component.
 * @return the blue component value
 */
float VrmlSFColor::getB() const {
    return this->d_rgb[2];
}

/**
 * @brief Get the value.
 * @return a pointer to a 3-element array comprising the RGB value
 */
const float * VrmlSFColor::get() const {
    return this->d_rgb;
}

/**
 * @brief Set the value.
 * @param rgb a 3-element vector comprising a RGB value
 */
void VrmlSFColor::set(const float rgb[3]) {
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
void VrmlSFColor::HSVtoRGB(const float hsv[3], float rgb[3])
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
void VrmlSFColor::RGBtoHSV(const float rgb[3], float hsv[3]) {
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
 * @param h the hue component
 * @param s the saturation component
 * @param v the value component
 */
void VrmlSFColor::setHSV(float h, float s, float v)
{
    const float hsv[3] = { h, s, v };
    HSVtoRGB(hsv, this->d_rgb);
}

/**
 * @brief Get the value expressed in HSV.
 * @retval hsv a 3-element array comprising the HSV value.
 */
void VrmlSFColor::getHSV(float hsv[3]) const {
    RGBtoHSV(this->d_rgb, hsv);
}


/**
 * @class VrmlSFFloat
 * @brief Encapsulates an SFFloat value.
 */
#include "VrmlSFFloat.h"

/**
 * @brief Constructor.
 * @param value initial value
 */
VrmlSFFloat::VrmlSFFloat(float value) : d_value(value) {}

ostream& VrmlSFFloat::print(ostream& os) const
{ return (os << d_value); }

VrmlField *VrmlSFFloat::clone() const { return new VrmlSFFloat(d_value); }

VrmlField::VrmlFieldType VrmlSFFloat::fieldType() const { return SFFLOAT; }

/**
 * @brief Get value.
 * @return the SFFloat value
 */
float VrmlSFFloat::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 * @param value the new value
 */
void VrmlSFFloat::set(float value) {
    this->d_value = value;
}

/**
 * @class VrmlSFImage
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
 *
 */
#include "VrmlSFImage.h"

/**
 * Construct the default SFImage.
 */
VrmlSFImage::VrmlSFImage(): d_w(0), d_h(0), d_nc(0), d_pixels(0) {}

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
VrmlSFImage::VrmlSFImage(size_t width, size_t height, size_t components,
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
VrmlSFImage::VrmlSFImage(const VrmlSFImage& rhs): d_w(0), d_h(0), d_nc(0),
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
VrmlSFImage::~VrmlSFImage()
{
  delete [] d_pixels; 
}

/**
 * @brief Assignment.
 */
VrmlSFImage & VrmlSFImage::operator=(const VrmlSFImage & rhs) {
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

ostream& VrmlSFImage::print(ostream& os) const
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

VrmlField * VrmlSFImage::clone() const {
    return new VrmlSFImage(*this);
}

VrmlField::VrmlFieldType VrmlSFImage::fieldType() const { return SFIMAGE; }

/**
 * @brief Get the image width.
 * @return the image width
 */
size_t VrmlSFImage::getWidth() const {
    return this->d_w;
}

/**
 * @brief Get the image height.
 * @return the image height
 */
size_t VrmlSFImage::getHeight() const {
    return this->d_h;
}

/**
 * @brief Get the number of components.
 * @return the number of components
 */
size_t VrmlSFImage::getComponents() const {
    return this->d_nc;
}

/**
 * @brief Get the pixel data.
 * @return a pointer to the array of pixel data.
 */
const unsigned char * VrmlSFImage::getPixels() const {
    return this->d_pixels;
}

/**
 * @brief Set the image.
 * @param width width in pixels
 * @param height height in pixels
 * @param components number of components
 * @param pixels array of (width * height * components) bytes comprising the
 *        image data.
 */
void VrmlSFImage::set(size_t width, size_t height, size_t components,
                      const unsigned char * pixels) {
    delete this->d_pixels;
    
    this->d_w = width;
    this->d_h = height;
    this->d_nc = components;
    
    this->d_pixels = new unsigned char[width * height * components];
    std::copy(pixels, pixels + (width * height * components), this->d_pixels);
}

//void VrmlSFImage::setSize(size_t w, size_t h) {
//    this->d_w = w;
//    this->d_h = h;
//}


/**
 * @class VrmlSFInt32
 * @brief Encapsulates an SFInt32 value.
 */
#include "VrmlSFInt32.h"

/**
 * @brief Constructor.
 * @param value initial value
 */
VrmlSFInt32::VrmlSFInt32(long value): d_value(value) {}

ostream& VrmlSFInt32::print(ostream& os) const
{ return (os << d_value); }

VrmlField *VrmlSFInt32::clone() const { return new VrmlSFInt32(d_value); }

VrmlField::VrmlFieldType VrmlSFInt32::fieldType() const { return SFINT32; }

/**
 * @brief Get value.
 * @return the integer value
 */
long VrmlSFInt32::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 * @param value the new integer value
 */
void VrmlSFInt32::set(long value) {
    this->d_value = value;
}


/**
 * @class VrmlSFNode
 * @brief Encapsulates an SFNode.
 */
#include "VrmlSFNode.h"
#include "VrmlNode.h"

/**
 * @brief Constructor.
 * @param node a pointer to a VrmlNode
 */
VrmlSFNode::VrmlSFNode(VrmlNode * node) : d_value(node)
{ if (d_value) d_value->reference(); }

/**
 * @brief Copy constructor.
 * @param rhs the VrmlSFNode to copy
 */
VrmlSFNode::VrmlSFNode(const VrmlSFNode & rhs): d_value(rhs.d_value)
{ if (d_value) d_value->reference(); }

/**
 * @brief Destructor.
 */
VrmlSFNode::~VrmlSFNode() { if (d_value) d_value->dereference(); }

/**
 * @brief Assignment operator.
 * @param rhs the VrmlSFNode to copy into this object
 */
VrmlSFNode& VrmlSFNode::operator=(const VrmlSFNode& rhs)
{
  if (this == &rhs) return *this;
  if (d_value) d_value->dereference();
  d_value = rhs.d_value;
  if (d_value) d_value->reference();
  return *this;
}

ostream& VrmlSFNode::print(ostream& os) const
{ return os << *(d_value) << endl; }

VrmlField *VrmlSFNode::clone() const { return new VrmlSFNode(d_value); }

VrmlField::VrmlFieldType VrmlSFNode::fieldType() const { return SFNODE; }

/**
 * @brief Get value.
 * @return a pointer to this object's VrmlNode
 */
VrmlNode * VrmlSFNode::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 * @param node a pointer to a VrmlNode, or 0 if setting this SFNode to
 *             <code>NULL</code>
 */
void VrmlSFNode::set(VrmlNode * node)
{
  if (d_value) d_value->dereference();
  if ((d_value = node) != 0) d_value->reference();
}


/**
 * @class VrmlSFRotation
 * @brief Encapsulates an SFRotation.
 */
#include "VrmlSFRotation.h"

/**
 * @brief Default constructor.
 *
 * Construct with the default value, (0, 0, 1, 0).
 */
VrmlSFRotation::VrmlSFRotation() {
    this->d_x[0] = 0.0; // x
    this->d_x[1] = 0.0; // y
    this->d_x[2] = 1.0; // z
    this->d_x[3] = 0.0; // angle
}

/**
 * @brief Constructor.
 * @param rotation a 4-element array.
 */
VrmlSFRotation::VrmlSFRotation(const float rotation[4]) {
    std::copy(rotation, rotation + 4, this->d_x);
}

/**
 * @brief Constructor.
 * @param x the <var>x</var>-component of the axis of rotation
 * @param y the <var>y</var>-component of the axis of rotation
 * @param z the <var>z</var>-component of the axis of rotation
 * @param angle the rotation angle
 */
VrmlSFRotation::VrmlSFRotation(float x, float y, float z, float angle) {
    this->d_x[0] = x;
    this->d_x[1] = y;
    this->d_x[2] = z;
    this->d_x[3] = angle;
}

/**
 * @brief Constructor.
 * @param axis the axis of rotation
 * @param angle the rotation angle
 */
VrmlSFRotation::VrmlSFRotation(const VrmlSFVec3f & axis, float angle) {
    std::copy(axis.get(), axis.get() + 3, this->d_x);
    this->d_x[3] = angle;
}

/**
 * @brief Construct a rotation between two vectors.
 *
 * Construct a VrmlSFRotation equal to the rotation between two different
 * vectors.
 *
 * @param fromVector the starting vector
 * @param toVector the ending vector
 */
VrmlSFRotation::VrmlSFRotation(const VrmlSFVec3f & fromVector,
                               const VrmlSFVec3f & toVector) {
    this->setAxis(fromVector.cross(toVector));
    this->d_x[3] = acos(fromVector.dot(toVector) /
                        (fromVector.length() * toVector.length()));
}

/**
 * @brief Array element dereference operator (const version).
 * @param index a value from 0 - 3. 0 corresponds to the
 *              <var>x</var>-component of the rotation axis, 1 corresponds to
 *              the <var>y</var>-component of the rotation axis, 2 corresponds
 *              to the <var>z</var>-component of the rotation axis, and 3
 *              corresponds to the rotation angle.
 */
float VrmlSFRotation::operator[](size_t index) const {
    assert(index < 4);
    return this->d_x[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 * @param index a value from 0 - 3. 0 corresponds to the
 *              <var>x</var>-component of the rotation axis, 1 corresponds to
 *              the <var>y</var>-component of the rotation axis, 2 corresponds
 *              to the <var>z</var>-component of the rotation axis, and 3
 *              corresponds to the rotation angle.
 */
float & VrmlSFRotation::operator[](size_t index) {
    assert(index < 4);
    return this->d_x[index];
}

ostream& VrmlSFRotation::print(ostream& os) const
{ return (os <<d_x[0] << " " <<d_x[1] << " " <<d_x[2]<< " " <<d_x[3]); }

VrmlField *VrmlSFRotation::clone() const
{ return new VrmlSFRotation(d_x[0],d_x[1],d_x[2],d_x[3]); }

VrmlField::VrmlFieldType VrmlSFRotation::fieldType() const 
{ return SFROTATION; }

/**
 * @brief Get the <var>x</var>-component of the rotation axis.
 * @return the <var>x</var>-component of the rotation axis.
 */
float VrmlSFRotation::getX() const {
    return this->d_x[0];
}

/**
 * @brief Set the <var>x</var>-component of the rotation axis.
 * @param value
 */
void VrmlSFRotation::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the <var>y</var>-component of the rotation axis.
 * @return the <var>y</var>-component of the rotation axis
 */
float VrmlSFRotation::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component of the rotation axis.
 * @param value
 */
void VrmlSFRotation::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the <var>z</var>-component of the rotation axis.
 * @return the <var>z</var>-component of the rotation axis
 */
float VrmlSFRotation::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component of the rotation axis
 * @param value
 */
void VrmlSFRotation::setZ(float value) {
    this->d_x[2] = value;
}

/**
 * @brief Get the rotation angle.
 * @return the rotation angle
 */
float VrmlSFRotation::getAngle() const {
    return this->d_x[3];
}

/**
 * @brief Set the rotation angle.
 * @param value
 */
void VrmlSFRotation::setAngle(float value) {
    this->d_x[3] = value;
}

/**
 * @brief Get the value of this rotation.
 * @return a pointer to a 4-element array.
 */
const float * VrmlSFRotation::get() const {
    return this->d_x;
}

/**
 * @brief Set the value of this rotation.
 * @param rotation a 4-element array
 */
void VrmlSFRotation::set(const float rotation[4]) {
    std::copy(rotation, rotation + 4, this->d_x);
}

/**
 * @brief Get the axis of rotation as a VrmlSFVec3f.
 * @return the axis of rotation
 */
const VrmlSFVec3f VrmlSFRotation::getAxis() const {
    return VrmlSFVec3f(this->d_x[0], this->d_x[1], this->d_x[2]);
}

/**
 * @brief Set the axis of rotation using a VrmlSFVec3f.
 * @param axis the new rotation axis
 */
void VrmlSFRotation::setAxis(const VrmlSFVec3f & axis) {
    std::copy(axis.get(), axis.get() + 3, this->d_x);
}

/**
 * @brief Get the inverse.
 * @return a VrmlSFRotation that is the inverse of this one
 */
const VrmlSFRotation VrmlSFRotation::inverse() const {
    VrmlSFRotation result(*this);
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
 * @param rotation the rotation by which to multiply this one
 * @return the result rotation
 */
const VrmlSFRotation VrmlSFRotation::multiply(const VrmlSFRotation & rot)
        const {
    // convert to quaternions
    float quatUS[4], quatVec[4];
    this->toQuaternion(quatUS);
    rot.toQuaternion(quatVec);
    
    // multiply quaternions
    float resultQuat[4];
    multQuat(quatUS, quatVec, resultQuat);
    
    // now convert back to axis/angle
    VrmlSFRotation result;
    result.fromQuaternion(resultQuat);
    return result;
}

void VrmlSFRotation::toQuaternion(float theQuat[4]) const {
    const float sintd2 = sin(d_x[3] * 0.5);
    const float len =
            sqrt((d_x[0] * d_x[0]) + (d_x[1] * d_x[1]) + (d_x[2] * d_x[2]));
    const float f = sintd2 / len;
    theQuat[3] = cos(d_x[3] * 0.5);
    theQuat[0] = d_x[0] * f;
    theQuat[1] = d_x[1] * f;
    theQuat[2] = d_x[2] * f;
}

void VrmlSFRotation::fromQuaternion(const float theQuat[4]) {
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
const VrmlSFVec3f VrmlSFRotation::multVec(const VrmlSFVec3f & vec) const {
    return VrmlSFVec3f();
}

/**
 * @brief Perform a <b>S</b>pherical <b>L</b>inear Int<b>ERP</b>olation.
 *
 * @param destRotation the destination rotation
 * @param t the interval fraction
 */
const VrmlSFRotation VrmlSFRotation::slerp(const VrmlSFRotation & destRotation,
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
    
    VrmlSFRotation result;
    result.fromQuaternion(resultQuat);
    
    return result;
}


/**
 * @class VrmlSFString
 * @brief Encapsulates an SFString.
 */
#include "VrmlSFString.h"

/**
 * @brief Constructor
 * @param s a C-style string
 */
VrmlSFString::VrmlSFString(const char *s)
{ if (s) { d_s = new char[strlen(s)+1]; strcpy(d_s,s); } else d_s = 0; }

/**
 * @brief Copy constructor.
 * @param sfs the VrmlSFString to copy
 */
VrmlSFString::VrmlSFString(const VrmlSFString &sfs)
{
  const char *s = sfs.get();
  if (s) { d_s = new char[strlen(s)+1]; strcpy(d_s,s); } else d_s = 0; 
}

/**
 * @brief Destructor.
 */
VrmlSFString::~VrmlSFString() { if (d_s) delete [] d_s; }

/**
 * @brief Get value.
 * @return a C-style string
 */
const char * VrmlSFString::get() const {
    return this->d_s;
}

/**
 * @brief Set value.
 * @param s a C-style string
 */
void VrmlSFString::set(const char *s)
{
  if (d_s) delete [] d_s;
  if (s) { d_s = new char[strlen(s)+1]; strcpy(d_s,s); }
  else d_s = 0;
}

/**
 * @brief Assignment.
 * @param rhs value to assign to this object
 */
VrmlSFString& VrmlSFString::operator=(const VrmlSFString& rhs)
{ if (this != &rhs) set(rhs.d_s); return *this; }

ostream& VrmlSFString::print(ostream& os) const
{ return (os << '\"' << d_s << '\"'); }

VrmlField *VrmlSFString::clone() const { return new VrmlSFString(d_s); }

VrmlField::VrmlFieldType VrmlSFString::fieldType() const { return SFSTRING; }


/**
 * @class VrmlSFTime
 * @brief Encapsulates an SFTime value.
 */
#include "VrmlSFTime.h"

/**
 * @brief Constructor
 * @param value initial value
 */
VrmlSFTime::VrmlSFTime(double value) : d_value(value) {}

ostream& VrmlSFTime::print(ostream& os) const
{ return (os << d_value); }

VrmlField * VrmlSFTime::clone() const {
    return new VrmlSFTime(*this);
}

VrmlField::VrmlFieldType VrmlSFTime::fieldType() const { return SFTIME; }

/**
 * @brief Get value.
 * @return the value.
 */
double VrmlSFTime::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 * @param value the new value
 */
void VrmlSFTime::set(double value) {
    this->d_value = value;
}


/**
 * @class VrmlSFVec2f
 * @brief Encapsulates a SFVec2f value.
 */
#include "VrmlSFVec2f.h"

/**
 * @brief Construct a VrmlSFVec2f with the default values, (0, 0).
 */
VrmlSFVec2f::VrmlSFVec2f() {
    this->d_x[0] = this->d_x[1] = 0;
}
        
/**
 * @brief Construct a VrmlSFVec2f.
 * @param vec a 2-element array
 */
VrmlSFVec2f::VrmlSFVec2f(const float vec[2]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
}
        
/**
 * @brief Construct a VrmlSFVec2f.
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 */
VrmlSFVec2f::VrmlSFVec2f(float x, float y) {
    this->d_x[0] = x;
    this->d_x[1] = y;
}

/**
 * @brief Array element dereference operator (const version).
 * @param index a value from 0 - 1. 0 corresponds to the
 *              <var>x</var>-component, and 1 corresponds to the
 *              <var>y</var>-component.
 */
float VrmlSFVec2f::operator[](size_t index) const {
    assert(index < 2);
    return this->d_x[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 * @param index a value from 0 - 1. 0 corresponds to the
 *              <var>x</var>-component, and 1 corresponds to the
 *              <var>y</var>-component.
 */
float & VrmlSFVec2f::operator[](size_t index) {
    assert(index < 2);
    return this->d_x[index];
}

ostream& VrmlSFVec2f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1]); }

VrmlField * VrmlSFVec2f::clone() const {
    return new VrmlSFVec2f(*this);
}

/**
 * @brief Get the field type.
 * @return the VrmlFieldType enumerant corresponding to this field type
 */
VrmlField::VrmlFieldType VrmlSFVec2f::fieldType() const {
    return SFVEC2F;
}

/**
 * @brief Get the x component.
 */
float VrmlSFVec2f::getX() const {
    return this->d_x[0];
}
    
/**
 * @brief Set the x component.
 */
void VrmlSFVec2f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the y component.
 */
float VrmlSFVec2f::getY() const {
    return this->d_x[1];
}
        
/**
 * @brief Set the y component.
 */
void VrmlSFVec2f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the value of this vector.
 * @returns a pointer to a 2-element array.
 */
const float * VrmlSFVec2f::get() const {
    return this->d_x;
}

/**
 * @brief Set the value of this vector.
 * @param vec a 2-element array.
 */
void VrmlSFVec2f::set(const float vec[2]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
}

/**
 * @brief Add two vectors.
 * @param vec the vector to add to this one.
 * @return a VrmlSFVec2f with a value that is the passed SFVec2f added,
 *         componentwise, to this object.
 */
const VrmlSFVec2f VrmlSFVec2f::add(const VrmlSFVec2f & vec) const {
    VrmlSFVec2f result(*this);
    result.d_x[0] += this->d_x[0];
    result.d_x[1] += this->d_x[1];
    return result;
}

/**
 * @brief Divide this vector by a scalar.
 * @param number a scalar value.
 * @return a VrmlSFVec2f with a value that is the object divided by the
 *         passed numeric value.
 */
const VrmlSFVec2f VrmlSFVec2f::divide(float number) const {
    VrmlSFVec2f result(*this);
    result.d_x[0] /= number;
    result.d_x[1] /= number;
    return result;
}

/**
 * @brief Dot product.
 * @param vec
 * @return the dot product of this vector and vec.
 */
double VrmlSFVec2f::dot(const VrmlSFVec2f & vec) const {
    return (this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1]);
}

/**
 * @brief Geometric length.
 * @return the length of this vector.
 */
double VrmlSFVec2f::length() const
{
  return sqrt(d_x[0] * d_x[0] + d_x[1] * d_x[1]);
}

/**
 * @brief Multiply by a scalar.
 * @param number a scalar value
 * @return a VrmlSFVec2f with a value that is the object multiplied by the
 *         passed numeric value.
 */
const VrmlSFVec2f VrmlSFVec2f::multiply(float number) const {
    VrmlSFVec2f result(*this);
    result.d_x[0] *= number;
    result.d_x[1] *= number;
    return result;
}

/**
 * @brief Negate.
 * @return a VrmlSFVec2f that the result of negating this vector.
 */
const VrmlSFVec2f VrmlSFVec2f::negate() const {
    VrmlSFVec2f result;
    result.d_x[0] = -this->d_x[0];
    result.d_x[1] = -this->d_x[1];
    return result;
}

/**
 * @brief Normalize.
 * @return a VrmlSFVec2f that is this vector normalized.
 */
const VrmlSFVec2f VrmlSFVec2f::normalize() const {
    const double len = this->length();
    if (FPZERO(len)) {
        return *this;
    }
    VrmlSFVec2f result(*this);
    result.d_x[0] /= len;
    result.d_x[1] /= len;
    return result;
}

/**
 * @brief Take the difference of two vectors.
 * @param vec the vector to subtract from this one
 * @return a VrmlSFVec2f that is the difference between this vector and vec
 */
const VrmlSFVec2f VrmlSFVec2f::subtract(const VrmlSFVec2f & vec) const {
    VrmlSFVec2f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    return result;
}


/**
 * @class VrmlSFVec3f
 * @brief Encapsulates a SFVec3f value.
 */
#include "VrmlSFVec3f.h"

/**
 * @brief Construct a VrmlSFVec3f with the default value, (0, 0, 0).
 */
VrmlSFVec3f::VrmlSFVec3f() {}

/**
 * @brief Construct a VrmlSFVec3f.
 * @param vec a 3-element array
 */
VrmlSFVec3f::VrmlSFVec3f(const float vec[3]) {
    std::copy(vec, vec + 3, this->d_x);
}

/**
 * @brief Construct a VrmlSFVec3f.
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 * @param z the <var>z</var>-component
 */
VrmlSFVec3f::VrmlSFVec3f(float x, float y, float z)
{ d_x[0] = x; d_x[1] = y; d_x[2] = z; }

/**
 * @brief Array element dereference operator (const version).
 * @param index a value from 0 - 2. 0 corresponds to the
 *              <var>x</var>-component, 1 corresponds to the
 *              <var>y</var>-component, and 2 corresponds to the
 *              <var>z</var>-component.
 */
float VrmlSFVec3f::operator[](size_t index) const {
    assert(index < 3);
    return this->d_x[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 * @param index a value from 0 - 2. 0 corresponds to the
 *              <var>x</var>-component, 1 corresponds to the
 *              <var>y</var>-component, and 2 corresponds to the
 *              <var>z</var>-component.
 */
float & VrmlSFVec3f::operator[](size_t index) {
    assert(index < 3);
    return this->d_x[index];
}

ostream& VrmlSFVec3f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1] << " " << d_x[2]); }

VrmlField *VrmlSFVec3f::clone() const
{ return new VrmlSFVec3f(*this); }

VrmlField::VrmlFieldType VrmlSFVec3f::fieldType() const { return SFVEC3F; }

/**
 * @brief Get the <var>x</var>-component
 * @return the <var>x</var>-component of this vector
 */
float VrmlSFVec3f::getX() const {
    return this->d_x[0];
}

/**
 * @brief Set the <var>x</var>-component
 * @param value
 */
void VrmlSFVec3f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the <var>y</var>-component
 * @return the <var>y</var>-component of this vector
 */
float VrmlSFVec3f::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component
 * @param value
 */
void VrmlSFVec3f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the <var>z</var>-component
 * @return the <var>z</var>-component of this vector
 */
float VrmlSFVec3f::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component
 * @param value
 */
void VrmlSFVec3f::setZ(float value) {
    this->d_x[2] = value;
}

/**
 * @brief Get the vector value.
 * @return a pointer to a 3-element array
 */
const float * VrmlSFVec3f::get() const {
    return this->d_x;
}

/**
 * @brief Set the vector value.
 * @param vec a 3-element array
 */
void VrmlSFVec3f::set(const float vec[3]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
    this->d_x[2] = vec[2];
}

/**
 * @brief Add this vector and vec component-wise.
 * @param vec
 */
const VrmlSFVec3f VrmlSFVec3f::add(const VrmlSFVec3f & vec) const {
    VrmlSFVec3f result(*this);
    result.d_x[0] += vec.d_x[0];
    result.d_x[1] += vec.d_x[1];
    result.d_x[2] += vec.d_x[2];
    return result;
}

/**
 * @brief Get the cross product of this vector and vec.
 * @param vec
 */
const VrmlSFVec3f VrmlSFVec3f::cross(const VrmlSFVec3f & vec) const {
    VrmlSFVec3f result;
    result.d_x[0] = (this->d_x[1] * vec.d_x[2]) - (this->d_x[2] * vec.d_x[1]);
    result.d_x[1] = (this->d_x[2] * vec.d_x[0]) - (this->d_x[0] * vec.d_x[2]);
    result.d_x[2] = (this->d_x[0] * vec.d_x[1]) - (this->d_x[1] * vec.d_x[0]);
    return result;
}

/**
 * @brief Get the result of dividing this vector by number.
 * @param number
 */
const VrmlSFVec3f VrmlSFVec3f::divide(float number) const {
    VrmlSFVec3f result(*this);
    result.d_x[0] /= number;
    result.d_x[1] /= number;
    result.d_x[2] /= number;
    return result;
}

/**
 * @brief Get the dot product of this vector and vec.
 * @param vec
 */
double VrmlSFVec3f::dot(const VrmlSFVec3f & vec) const {
    return ((this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1])
            + (this->d_x[2] * vec.d_x[2]));
}

/**
 * @brief Get the length of this vector.
 * @return the geometric length of the vector.
 */
double VrmlSFVec3f::length() const {
    const double len = sqrt((d_x[0] * d_x[0])
                          + (d_x[1] * d_x[1])
                          + (d_x[2] * d_x[2]));
    return FPZERO(len) ? 0.0 : len;
}

/**
 * Multiply by a scalar.
 * @param number
 * @return the product
 */
const VrmlSFVec3f VrmlSFVec3f::multiply(float number) const {
    VrmlSFVec3f result(*this);
    result.d_x[0] *= number;
    result.d_x[1] *= number;
    result.d_x[2] *= number;
    return result;
}

/**
 * @brief Negate.
 * @return the negatation of this vector
 */
const VrmlSFVec3f VrmlSFVec3f::negate() const {
    VrmlSFVec3f result(*this);
    result.d_x[0] = -result.d_x[0];
    result.d_x[1] = -result.d_x[1];
    result.d_x[2] = -result.d_x[2];
    return result;
}

/**
 * Normalize.
 * @return a copy of this vector normalized
 */
const VrmlSFVec3f VrmlSFVec3f::normalize() const {
    const double len = this->length();
    VrmlSFVec3f result(*this);
    if (!FPZERO(len)) {
        result.d_x[0] /= len;
        result.d_x[1] /= len;
        result.d_x[2] /= len;
    }
  
    return result;
}

/**
 * @brief Subtract.
 * @param vec
 * @return the difference between this vector and vec
 */
const VrmlSFVec3f VrmlSFVec3f::subtract(const VrmlSFVec3f & vec) const {
    VrmlSFVec3f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    result.d_x[2] -= vec.d_x[2];
    return result;
}


/**
 * @class VrmlMFColor
 * @brief Encapsulates a MFColor.
 */
#include "VrmlMFColor.h"

class VrmlMFColor::FData {			// reference counted float data
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
 * @brief Construct from a single color value.
 * @param r red component
 * @param g green component
 * @param b blue component
 */
VrmlMFColor::VrmlMFColor(float r, float g, float b) : d_data(new FData(3))
{ d_data->d_v[0] = r; d_data->d_v[1] = g; d_data->d_v[2] = b; }

/**
 * @brief Construct from a float array.
 * @param length the number of RGB triplets in the array
 * @param colors a float array comprising RGB triplets
 */
VrmlMFColor::VrmlMFColor(size_t length, float const * colors):
        d_data(new FData(length * 3)) {
    if (colors) {
        std::copy(colors, colors + (length * 3), this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 * @param mfColor the object to copy
 */
VrmlMFColor::VrmlMFColor(const VrmlMFColor & mfColor):
        d_data(mfColor.d_data->ref()) {}

/**
 * @brief Destructor.
 */
VrmlMFColor::~VrmlMFColor() { d_data->deref(); }

/**
 * @brief Assignment operator.
 * @param rhs value to assign to this object
 * @return a reference to this object
 */
VrmlMFColor& VrmlMFColor::operator=(const VrmlMFColor& rhs)
{
  if (this != &rhs) {
    d_data->deref();
    d_data = rhs.d_data->ref();
  }
  return *this;
}

/**
 * @brief Get value.
 * @return a pointer to an array comprising RGB triplets
 */
const float * VrmlMFColor::get() const {
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
void VrmlMFColor::set(size_t length, const float * colors) {
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
 * @brief Get the length.
 * @return the number of color values (RGB triplets)
 */
size_t VrmlMFColor::getLength() const {
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
void VrmlMFColor::setLength(size_t length) {
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

/**
 * @brief Array element dereference operator.
 * @return a pointer to a 3-element array comprising an RGB triplet
 */
const float * VrmlMFColor::operator[](size_t index) const {
    assert(index < this->getLength());
    return (this->d_data->d_v + (index * 3L));
}

VrmlField *VrmlMFColor::clone() const { return new VrmlMFColor(*this); }

VrmlField::VrmlFieldType VrmlMFColor::fieldType() const { return MFCOLOR; }

ostream& VrmlMFColor::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 3L); }


/**
 * @class VrmlMFFloat
 * Encapsulates a MFFloat.
 */
#include "VrmlMFFloat.h"

class VrmlMFFloat::FData {			// reference counted float data
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
 * @brief Constructor.
 * @param value
 */
VrmlMFFloat::VrmlMFFloat(float value) : d_data(new FData(1)) 
{ d_data->d_v[0] = value; }

/**
 * @brief Construct from a float array.
 * @param length the number of floats in the array
 * @param numbers a pointer to a float array
 */
VrmlMFFloat::VrmlMFFloat(size_t length, float const * numbers):
        d_data(new FData(length)) {
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 * @param mfFloat the object to copy
 */
VrmlMFFloat::VrmlMFFloat(const VrmlMFFloat & mfFloat):
        d_data(mfFloat.d_data->ref()) {}

/**
 * @brief Destructor.
 */
VrmlMFFloat::~VrmlMFFloat() { d_data->deref(); }

/**
 * @brief Assignment operator.
 * @param mfFloat the object to copy
 */
VrmlMFFloat& VrmlMFFloat::operator=(const VrmlMFFloat & mfFloat) {
    if (this != &mfFloat) {
        this->d_data->deref();
        this->d_data = mfFloat.d_data->ref();
    }
    return *this;
}

/**
 * @brief Array element dereference operator.
 * @param index
 */
const float VrmlMFFloat::operator[](size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Get value.
 * @return a pointer to a float array
 */
const float * VrmlMFFloat::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 * @param length the number of float values
 * @param numbers a pointer to a float array
 */
void VrmlMFFloat::set(size_t length, const float * numbers) {
    this->d_data->deref();
    this->d_data = new FData(length);
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Get the length.
 * @return the number of float values
 */
size_t VrmlMFFloat::getLength() const {
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
void VrmlMFFloat::setLength(size_t length) {
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

VrmlField *VrmlMFFloat::clone() const	{ return new VrmlMFFloat(*this); }

VrmlField::VrmlFieldType VrmlMFFloat::fieldType() const { return MFFLOAT; }

ostream& VrmlMFFloat::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 1); }


/**
 * @class VrmlMFInt32
 * @brief Encapsulates a MFInt32.
 */
#include "VrmlMFInt32.h"

class VrmlMFInt32::IData {			// reference counted int data
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
 * @brief Constructor.
 * @param number a single initial value
 */
VrmlMFInt32::VrmlMFInt32(long number): d_data(new IData(1)) {
    d_data->d_v[0] = number;
}

/**
 * @brief Construct from a long array.
 * @param length the number of integer values
 * @param numbers a pointer to a long array
 */
VrmlMFInt32::VrmlMFInt32(size_t length, const long * numbers):
        d_data(new IData(length)) {
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Copy constructor.
 * @param mfInt32 the object to copy
 */
VrmlMFInt32::VrmlMFInt32(const VrmlMFInt32 & mfInt32):
        d_data(mfInt32.d_data->ref()) {}

/**
 * @brief Destructor.
 */
VrmlMFInt32::~VrmlMFInt32() { d_data->deref(); }

/**
 * @brief Assignment operator.
 * @param mfInt32 the object to copy into this one
 * @return a reference to this object
 */
VrmlMFInt32 & VrmlMFInt32::operator=(const VrmlMFInt32 & mfInt32)
{
  if (this != &mfInt32) {
    d_data->deref();
    d_data = mfInt32.d_data->ref();
  }
  return *this;
}

/**
 * @brief Array element dereference operator.
 * @param index an array index
 * @return the integer value at the given index
 */
long VrmlMFInt32::operator[](size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Get value.
 * @return a pointer to the long array comprising the integer values owned by
 *         this object
 */
const long * VrmlMFInt32::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 * @param length the number of integer values
 * @param numbers a pointer to a long array
 */
void VrmlMFInt32::set(size_t length, const long * numbers) {
    d_data->deref();
    d_data = new IData(length);
    if (numbers) {
        std::copy(numbers, numbers + length, this->d_data->d_v);
    }
}

/**
 * @brief Get the length.
 * @return the number of integer values
 */
size_t VrmlMFInt32::getLength() const {
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
void VrmlMFInt32::setLength(size_t length) {
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

VrmlField * VrmlMFInt32::clone() const { return new VrmlMFInt32(*this); }

VrmlField::VrmlFieldType VrmlMFInt32::fieldType() const { return MFINT32; }

ostream& VrmlMFInt32::print(ostream& os) const
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
 * @class VrmlMFNode
 *
 * @brief Encapsulates a MFNode.
 *
 * Since individual nodes are refcounted, no attempt is made to refcount
 * MFNode.
 */
#include "VrmlMFNode.h"

/**
 * @brief Constructor.
 *
 * @param node a pointer to a VrmlNode; can be null
 */
VrmlMFNode::VrmlMFNode(VrmlNode * node):
        d_v(new VrmlNode* [1]), d_allocated(1), d_size(1) {
    d_v[0] = node ? node->reference() : 0;
}

/**
 * @brief Construct from an array of VrmlNode pointers.
 *
 * @param length the length of the array
 * @param nodes a pointer to an array of VrmlNode pointers
 */
VrmlMFNode::VrmlMFNode(size_t length, VrmlNode * const * v):
        d_v(new VrmlNode * [length]), d_allocated(length), d_size(length) {
    if (v) {
        for (size_t i(0); i < length; ++i) {
            d_v[i] = v[i] ? (v[i]->reference()) : 0;
        }
    } else {
        std::fill(d_v, d_v + length, static_cast<VrmlNode *>(0));
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfNode the object to copy
 */
VrmlMFNode::VrmlMFNode(VrmlMFNode const & mfnode):
        d_v(new VrmlNode * [mfnode.d_size]), d_allocated(mfnode.d_size),
        d_size(mfnode.d_size) {
    for (size_t i(0); i < mfnode.d_size; ++i) {
        d_v[i] = mfnode.d_v[i] ? (mfnode.d_v[i]->reference()) : 0;
    }
}

/**
 * @brief Destructor.
 */
VrmlMFNode::~VrmlMFNode()
{ 
    for (size_t i(0); i < d_size; ++i) {
        if (d_v[i]) d_v[i]->dereference();
    }
    delete [] d_v;
}

/**
 * @brief Assignment operator.
 *
 * @param mfNode the object to copy into this one
 */
VrmlMFNode & VrmlMFNode::operator=(const VrmlMFNode & mfnode) {
    if (this != &mfnode) {
        size_t i(0);
        for (; i < d_size; ++i) {
            if (d_v[i]) {
                d_v[i]->dereference();
            }
        }
        
        if (d_allocated < mfnode.d_size) {
            delete [] d_v;
            d_size = d_allocated = 0;
            d_v = 0;
            d_v = new VrmlNode*[mfnode.d_size];
            d_allocated = mfnode.d_size;
        }
        
        d_size = mfnode.d_size;
        
        for (i = 0; i < d_size; ++i) {
            d_v[i] = mfnode.d_v[i] ? mfnode.d_v[i]->reference() : 0;
        }
    }
    
    return *this;
}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index
 */
VrmlNode * VrmlMFNode::operator[](size_t index) const {
    return this->d_v[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 *
 * @param index
 */
VrmlNode * & VrmlMFNode::operator[](size_t index) {
    return this->d_v[index];
}

/**
 * @brief Get the length.
 *
 * @return the number of nodes in the array
 */
size_t VrmlMFNode::getLength() const {
    return this->d_size;
}

/**
 * @brief Set the length.
 *
 * Set the length of the node array. If the new length is less than the
 * current length, the array is truncated. If the length is greater than
 * the current length, the new positions at the end of the array are set
 * to NULL.
 *
 * @param length the new length
 */
void VrmlMFNode::setLength(size_t length) {
    if (length < this->d_size) {
        for (size_t i = length; i < this->d_size; ++i) {
            this->d_v[i]->dereference();
            this->d_v[i] = 0;
        }
    } else if (length > this->d_allocated) {
        this->realloc(length);
    }
    for (size_t i = this->d_size; i < length; ++i) {
        this->d_v[i] = 0;
    }
    this->d_size = length;
}

void VrmlMFNode::realloc(size_t newSize) {
    VrmlNode * * newNodes = new VrmlNode *[newSize];
    std::copy(this->d_v, this->d_v + this->d_size, newNodes);
    delete [] this->d_v;
    this->d_v = newNodes;
    this->d_allocated = newSize;
}

/**
 * @brief Determine if a node exists in this MFNode.
 *
 * @param node
 */
bool VrmlMFNode::exists(const VrmlNode & node) const {
    for (size_t i(0); i < this->d_size; ++i) {
        if (this->d_v[i] == &node) {
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
 * @return <code>true</code> if a node was added, <code>false</code>
 *         otherwise
 */
bool VrmlMFNode::addNode(VrmlNode * node) {
    if (node && !exists(*node)) {
        if (this->d_allocated < d_size + 1) {
            this->realloc(this->d_allocated + 10);
        }
        
        this->d_v[this->d_size++] = node->reference();
        
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
bool VrmlMFNode::removeNode(VrmlNode * node) {
    if (node) {
        for (size_t i = 0; i < this->d_size; ++i) {
            if (this->d_v[i] == node) {
	        d_v[i]->dereference();
                for (size_t j = i; j < this->d_size - 1; ++j) {
                    this->d_v[j] = this->d_v[j + 1];
                }
	        --this->d_size;
	        return true;
            }
        }
    }
    return false;
}

VrmlField *VrmlMFNode::clone() const
{ return new VrmlMFNode(*this); }

VrmlField::VrmlFieldType VrmlMFNode::fieldType() const { return MFNODE; }


/**
 * @class VrmlMFRotation
 * Encapsulates a MFRotation.
 */
#include "VrmlMFRotation.h"

VrmlMFRotation::VrmlMFRotation() : d_data(new FData(0))
{}

VrmlMFRotation::VrmlMFRotation(float x, float y, float z, float r)
  : d_data(new FData(4))
{ d_data->d_v[0]=x; d_data->d_v[1]=y; d_data->d_v[2]=z; d_data->d_v[3]=r; }

VrmlMFRotation::VrmlMFRotation(size_t n, float const * v) : d_data(new FData(4*n))
{
  if (v) memcpy(d_data->d_v, v, 4*n*sizeof(float));
}  

VrmlMFRotation::VrmlMFRotation(const VrmlMFRotation &src)
  : d_data(src.d_data->ref()) {}

VrmlMFRotation::~VrmlMFRotation() { d_data->deref(); }

void VrmlMFRotation::set(size_t n, const float * v)
{
  d_data->deref();
  d_data = new FData(4*n);
  if (v) memcpy(d_data->d_v, v, 4*n*sizeof(float));
}

VrmlMFRotation& VrmlMFRotation::operator=(const VrmlMFRotation& rhs)
{
  if (this != &rhs) {
    d_data->deref();
    d_data = rhs.d_data->ref();
  }
  return *this;
}

VrmlField *VrmlMFRotation::clone() const { return new VrmlMFRotation(*this); }

VrmlField::VrmlFieldType VrmlMFRotation::fieldType() const { return MFROTATION; }

ostream& VrmlMFRotation::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 4); }


/**
 * @class VrmlMFString
 * Encapsulates a MFString.
 */
#include "VrmlMFString.h"

VrmlMFString::VrmlMFString()
  : d_v(0), d_allocated(0), d_size(0)
{}

VrmlMFString::VrmlMFString(char const * s)
  : d_v(new char*[1]), d_allocated(1), d_size(1)
{
    if (s) {
        d_v[0] = new char[strlen(s)+1];
        strcpy(d_v[0],s);
    } else {
        d_v[0] = 0;
    }
}

VrmlMFString::VrmlMFString(size_t n, char const * const * v)
  : d_v(new char *[n]), d_allocated(n), d_size(n)
{
    if (v) {
        for (size_t i(0); i < n; ++i, ++v) {
            if (*v) {
                d_v[i] = new char[strlen(*v)+1];
                strcpy(d_v[i], *v);
            } else {
                d_v[i] = 0;
            }
        }
    } else {
        std::fill(d_v, d_v + n, static_cast<char *>(0));
    }
}


VrmlMFString::VrmlMFString(VrmlMFString const & rhs)
  : d_v(new char *[rhs.d_size]), d_allocated(rhs.d_size), d_size(rhs.d_size)
{
    for (size_t i(0); i < rhs.d_size; ++i) {
        if (rhs.d_v[i]) {
            d_v[i] = new char[strlen(rhs.d_v[i])+1];
            strcpy(d_v[i], rhs.d_v[i]);
        } else {
            d_v[i] = 0;
        }
    }
}

VrmlMFString::~VrmlMFString()
{
    for (size_t i(0); i < d_size; ++i) {
        delete [] d_v[i];
    }
    delete [] d_v;
}

void VrmlMFString::set(size_t n, char const * const v[])
{
    for (size_t i=0; i<d_size; ++i) {
        delete [] d_v[i];
    }
    
    if (d_allocated < n) {
        delete [] d_v;
        d_v = 0;
        d_allocated = d_size = 0;
        d_v = new char*[n];
        d_allocated = n;
    }
    d_size = n;
    
    for (size_t j = 0; j < n; ++j) {
        if (v[j]) {
            d_v[j] = new char[ strlen(v[j]) + 1 ];
            strcpy(d_v[j], v[j]);
        } else {
            d_v[j] = 0;
        }
    }
}

VrmlMFString& VrmlMFString::operator=(const VrmlMFString& rhs)
{
  if (this != &rhs) set( rhs.d_size, rhs.d_v );
  return *this;
}

VrmlField *VrmlMFString::clone() const
{ return new VrmlMFString(*this); }

VrmlField::VrmlFieldType VrmlMFString::fieldType() const { return MFSTRING; }

size_t VrmlMFString::getLength() const
{
    return d_size;
}

char const * const * VrmlMFString::get() const
{
    return d_v;
}

char const * VrmlMFString::get(size_t index) const
{
    return (*this)[index];
}

char const * VrmlMFString::operator[](size_t index) const
{
    assert(index < d_size);
    return d_v[index];
}


/**
 * @class VrmlMFTime
 * Encapsulates a MFTime.
 */
# include "VrmlMFTime.h"

//
// Reference-counted double data
//
class VrmlMFTime::DData {
    public:
        DData(size_t = 0);
        ~DData();
        
        DData * ref();
        void deref();
        
        unsigned int refs;      // number of VrmlMFTime objects using this data
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

VrmlMFTime::DData::DData(size_t size)
  : refs(1), size(size), data(size > 0 ? new double[size] : 0)
{}

VrmlMFTime::DData::~DData()
{
    delete [] data;
}

VrmlMFTime::DData * VrmlMFTime::DData::ref()
{
    ++refs;
    return this;
}

void VrmlMFTime::DData::deref()
{
    if (--refs == 0) {
        delete this;
    }
}

VrmlMFTime::VrmlMFTime()
  : d_data(new DData(0))
{}

VrmlMFTime::VrmlMFTime(double t)
  : d_data(new DData(1))
{
    d_data->data[0] = t;
}

VrmlMFTime::VrmlMFTime(size_t size, double const * v)
  : d_data(new DData(size))
{
    if (v) {
        std::copy(v, v + size, d_data->data);
    }
}

VrmlMFTime::VrmlMFTime(VrmlMFTime const & c)
  : d_data(c.d_data->ref())
{}

VrmlMFTime::~VrmlMFTime()
{
    d_data->deref();
}

VrmlMFTime & VrmlMFTime::operator=(VrmlMFTime const & c)
{
    if (this != &c) {
        d_data->deref();
        d_data = c.d_data->ref();
    }
    return *this;
}

void VrmlMFTime::set(size_t size, double const * v)
{
    d_data->deref();
    d_data = new DData(size);
    if (v) {
        std::copy(v, v + size, d_data->data);
    }
}

double const * VrmlMFTime::get() const
{
    return d_data->data;
}

double const & VrmlMFTime::operator[](size_t index) const
{
    assert(index >= 0 && index < d_data->size);
    
    return d_data->data[index];
}

size_t VrmlMFTime::getLength() const
{
    return d_data->size;
}

VrmlField * VrmlMFTime::clone() const
{
    return new VrmlMFTime(*this);
}

VrmlField::VrmlFieldType VrmlMFTime::fieldType() const
{
    return MFTIME;
}

ostream & VrmlMFTime::print(ostream & os) const
{
    return mfdprint(os, get(), getLength(), 1);
}


/**
 * @class VrmlMFVec2f
 * Encapsulates a MFVec2f.
 */
#include "VrmlMFVec2f.h"

VrmlMFVec2f::VrmlMFVec2f() : d_data(new FData(0)) {}

VrmlMFVec2f::VrmlMFVec2f(float x, float y) : d_data(new FData(2))
{ d_data->d_v[0] = x; d_data->d_v[1] = y; }

VrmlMFVec2f::VrmlMFVec2f(size_t n, const float * v) : d_data(new FData(2*n))
{
  if (v) memcpy(d_data->d_v, v, 2*n*sizeof(float));
}

VrmlMFVec2f::VrmlMFVec2f(const VrmlMFVec2f &source) :
  d_data(source.d_data->ref()) 
{}

VrmlMFVec2f::~VrmlMFVec2f() { d_data->deref(); }

void VrmlMFVec2f::set(size_t n, const float * v)
{
  d_data->deref();
  d_data = new FData(2*n);
  if (v) memcpy(d_data->d_v, v, 2*n*sizeof(float));
}

VrmlMFVec2f& VrmlMFVec2f::operator=(const VrmlMFVec2f& rhs)
{
  if (this != &rhs) {
    d_data->deref();
    d_data = rhs.d_data->ref();
  }
  return *this;
}

VrmlField *VrmlMFVec2f::clone() const { return new VrmlMFVec2f(*this); }

VrmlField::VrmlFieldType VrmlMFVec2f::fieldType() const { return MFVEC2F; }


/**
 * @class VrmlMFVec3f
 * Encapsulates a MFVec3f.
 */
#include "VrmlMFVec3f.h"


VrmlMFVec3f::VrmlMFVec3f(size_t n) : d_data(new FData(n)) {}

VrmlMFVec3f::VrmlMFVec3f(float x, float y, float z) : d_data(new FData(3))
{ d_data->d_v[0] = x; d_data->d_v[1] = y; d_data->d_v[2] = z; }

VrmlMFVec3f::VrmlMFVec3f(size_t n, const float * v) : d_data(new FData(3*n))
{
  if (v) memcpy(d_data->d_v, v, 3*n*sizeof(float));
}

VrmlMFVec3f::VrmlMFVec3f(const VrmlMFVec3f &source) :
  d_data(source.d_data->ref()) 
{}

VrmlMFVec3f::~VrmlMFVec3f() { d_data->deref(); }


void VrmlMFVec3f::set(size_t n, const float * v)
{
  d_data->deref();
  d_data = new FData(3*n);
  if (v) memcpy(d_data->d_v, v, 3*n*sizeof(float));
}

VrmlMFVec3f& VrmlMFVec3f::operator=(const VrmlMFVec3f& rhs)
{
  if (this != &rhs) {
    d_data->deref();
    d_data = rhs.d_data->ref();
  }
  return *this;
}

VrmlField *VrmlMFVec3f::clone() const { return new VrmlMFVec3f(*this); }

VrmlField::VrmlFieldType VrmlMFVec3f::fieldType() const { return MFVEC3F; }

const float * VrmlMFVec3f::get() const {
    return this->d_data->d_v;
}

float * VrmlMFVec3f::get() {
    return this->d_data->d_v;
}

const float * VrmlMFVec3f::operator[](size_t index) const {
    return (this->d_data->d_v + (index * 3));
}

float * VrmlMFVec3f::operator[](size_t index) {
    return (this->d_data->d_v + (index * 3));
}

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


ostream& VrmlMFVec2f::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 2); }

ostream& VrmlMFVec3f::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 3); }

ostream& VrmlMFNode::print(ostream& os) const
{
  int n = getLength();

  if (n != 1) os << '[';
  for (int i=0; i<n; ++i)
    os << *(d_v[i]) << endl;
  if (n != 1) os << ']';

  return os;
}


ostream& VrmlMFString::print(ostream& os) const
{
  int n = getLength();

  if (n != 1) os << '[';
  for (int i=0; i<n; ++i)
    os << '\"' << (d_v[i]) << "\" ";
  if (n != 1) os << ']';

  return os;
}


// Define the const and non-const generic and specific safe downcast methods

#define DOWNCAST(_t) \
const Vrml##_t * VrmlField::to##_t() const { return 0; }    \
      Vrml##_t * VrmlField::to##_t()       { return 0; }    \
const Vrml##_t * Vrml##_t ::to##_t() const { return this; } \
      Vrml##_t * Vrml##_t ::to##_t()       { return this; }

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

