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

// Printing methods



namespace OpenVRML {

ostream & operator<<(ostream & os, const FieldValue & f)
{ return f.print(os); }

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
 * @class FieldValue
 *
 * @brief Abstract base class for the VRML field types.
 */

/**
 * @enum FieldValue::Type
 *
 * @brief Used to identify FieldValue types.
 *
 * These tags are typically used to designate an expected type or avoid a
 * <code>dynamic_cast</code>.
 */

/**
 * @var FieldValue::invalidType
 *
 * @brief Zero value typically used to indicate failure.
 */

/**
 * @var FieldValue::sfbool
 *
 * @brief Designates an SFBool.
 */

/**
 * @var FieldValue::sfcolor
 *
 * @brief Designates an SFColor.
 */

/**
 * @var FieldValue::sffloat
 *
 * @brief Designates an SFFloat.
 */

/**
 * @var FieldValue::sfimage
 *
 * @brief Designates an SFImage.
 */

/**
 * @var FieldValue::sfint32
 *
 * @brief Designates an SFInt32.
 */

/**
 * @var FieldValue::sfnode
 *
 * @brief Designates an SFNode.
 */

/**
 * @var FieldValue::sfrotation
 *
 * @brief Designates an SFRotation.
 */

/**
 * @var FieldValue::sfstring
 *
 * @brief Designates an SFString.
 */

/**
 * @var FieldValue::sftime
 *
 * @brief Designates an SFTime.
 */

/**
 * @var FieldValue::sfvec2f
 *
 * @brief Designates an SFVec2f.
 */

/**
 * @var FieldValue::sfvec3f
 *
 * @brief Designates an SFVec3f.
 */

/**
 * @var FieldValue::mfcolor
 *
 * @brief Designates an MFColor.
 */

/**
 * @var FieldValue::mffloat
 *
 * @brief Designates an MFFloat.
 */

/**
 * @var FieldValue::mfint32
 *
 * @brief Designates an MFInt32.
 */

/**
 * @var FieldValue::mfnode
 *
 * @brief Designates an MFNode.
 */

/**
 * @var FieldValue::mfrotation
 *
 * @brief Designates an MFRotation.
 */

/**
 * @var FieldValue::mfstring
 *
 * @brief Designates an MFString.
 */

/**
 * @var FieldValue::mftime
 *
 * @brief Designates an MFTime.
 */

/**
 * @var FieldValue::mfvec2f
 *
 * @brief Designates an MFVec2f.
 */

/**
 * @var FieldValue::mfvec3f
 *
 * @brief Designates an MFVec3f.
 */

/**
 * @brief Destructor. (Virtualized for abstract base class.)
 */
FieldValue::~FieldValue() {}

/**
 * @fn FieldValue * FieldValue::clone() const throw (std::bad_alloc)
 *
 * @brief Virtual copy constructor.
 *
 * @return a new FieldValue identical to this one.
 *
 * @throws std::bad_alloc if memory allocation fails.
 */

/**
 * @fn FieldValue & FieldValue::assign(const FieldValue & value) throw (std::bad_cast, std::bad_alloc)
 *
 * @brief Virtual assignment.
 *
 * @return this object.
 *
 * @throws std::bad_cast if value is not of the same concrete type as this
 *      object.
 */

/**
 * @fn ostream& FieldValue::print(ostream& os) const
 * @brief Print the value of this FieldValue.
 * @param os an ostream
 * @return the ostream
 */

/**
 * @fn FieldValue::Type FieldValue::type() const
 * @brief Get the field type.
 * @return the Type enumerant corresponding to this FieldValue's type
 */

/**
 * @brief A static method to convert a type name to an ID.
 *
 * @param type the (C-style) string name of a VRML field type
 *
 * @return the Type enumerant corresponding to the passed type name
 */
FieldValue::Type FieldValue::type(const char * typeId) {
    for (size_t i(sfbool); i <= mfvec3f; ++i) {
        if (strcmp(typeId, ftn[i]) == 0) {
            return static_cast<Type>(i);
        }
    }
    
    return invalidType;
}

const char* FieldValue::getFieldName(const Type type) {
  int ft = (int) type;
  if ((ft > 0) && (ft <= (int) FieldValue::mfvec3f))
    return ftn[ft];
  else
    return "<invalid field type>";
}


/**
 * @brief Get the type name of a field.
 * @return the (C-style) string name of this Field object's type.
 */
char const * FieldValue::typeName() const {
    assert((sizeof(ftn) / sizeof(char *)) >=
            static_cast<size_t>((this->type() + 1)));
    return ftn[this->type()];
}


/**
 * @class SFBool
 *
 * @brief Encapsulates an SFBool value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value
 */
SFBool::SFBool(bool value): d_value(value) {}

/**
 * @brief Destructor.
 */
SFBool::~SFBool() {}

FieldValue * SFBool::clone() const throw (std::bad_alloc) {
    return new SFBool(*this);
}

FieldValue & SFBool::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFBool &>(value));
}

ostream& SFBool::print(ostream& os) const
{ return (os << (d_value ? "TRUE" : "FALSE")); }

FieldValue::Type SFBool::type() const {
    return sfbool;
}

/**
 * @brief Get the value.
 *
 * @return the value of this SFBool
 */
bool SFBool::get() const {
    return this->d_value;
}

/**
 * @brief Set the value.
 *
 * @param value the new value
 */
void SFBool::set(bool value) {
    this->d_value = value;
}


/**
 * @class SFColor
 *
 * @brief Encapsulates an SFColor value.
 */

/**
 * @brief Construct with the default value, (0, 0, 0).
 */
SFColor::SFColor() {
    this->d_rgb[0] = 0.0f;
    this->d_rgb[1] = 0.0f;
    this->d_rgb[2] = 0.0f;
}

/**
 * @brief Construct a SFColor.
 *
 * @param rgb a 3-element array
 */
SFColor::SFColor(const float rgb[3]) {
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
SFColor::SFColor(float r, float g, float b)
{ d_rgb[0] = r; d_rgb[1] = g; d_rgb[2] = b; }

/**
 * @brief Destructor.
 */
SFColor::~SFColor() {}

FieldValue * SFColor::clone() const throw (std::bad_alloc) {
    return new SFColor(*this);
}

FieldValue & SFColor::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFColor &>(value));
}

ostream & SFColor::print(ostream& os) const
{ return (os << d_rgb[0] << ' ' << d_rgb[1] << ' ' << d_rgb[2]); }

FieldValue::Type SFColor::type() const {
    return sfcolor;
}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index an index from 0 - 2
 */
float SFColor::operator[](size_t index) const {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Array element dereference operator (non-const version).
 *
 * @param index an index from 0 - 2
 */
float & SFColor::operator[](size_t index) {
    assert(index < 3);
    return this->d_rgb[index];
}

/**
 * @brief Get the red component.
 *
 * @return the red component value
 */
float SFColor::getR() const {
    return this->d_rgb[0];
}

/**
 * @brief Get the green component.
 *
 * @return the green component value
 */
float SFColor::getG() const {
    return this->d_rgb[1];
}

/**
 * @brief Get the blue component.
 * @return the blue component value
 */
float SFColor::getB() const {
    return this->d_rgb[2];
}

/**
 * @brief Get the value.
 *
 * @return a reference to a 3-element array comprising the RGB value
 */
const float (&SFColor::get() const)[3] {
    return this->d_rgb;
}

/**
 * @brief Set the value.
 *
 * @param rgb a 3-element vector comprising a RGB value
 */
void SFColor::set(const float rgb[3]) {
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
void SFColor::HSVtoRGB(const float hsv[3], float rgb[3])
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
void SFColor::RGBtoHSV(const float rgb[3], float hsv[3]) {
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
void SFColor::setHSV(float h, float s, float v) {
    const float hsv[3] = { h, s, v };
    HSVtoRGB(hsv, this->d_rgb);
}

/**
 * @brief Get the value expressed in HSV.
 *
 * @retval hsv a 3-element array comprising the HSV value.
 */
void SFColor::getHSV(float hsv[3]) const {
    RGBtoHSV(this->d_rgb, hsv);
}


/**
 * @class SFFloat
 *
 * @brief Encapsulates an SFFloat value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value
 */
SFFloat::SFFloat(float value): d_value(value) {}

/**
 * @brief Destructor.
 */
SFFloat::~SFFloat() {}

ostream & SFFloat::print(ostream& os) const
{ return (os << d_value); }

FieldValue * SFFloat::clone() const throw (std::bad_alloc) {
    return new SFFloat(*this);
}

FieldValue & SFFloat::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFFloat &>(value));
}

FieldValue::Type SFFloat::type() const {
    return sffloat;
}

/**
 * @brief Get value.
 *
 * @return the SFFloat value
 */
float SFFloat::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new value
 */
void SFFloat::set(float value) {
    this->d_value = value;
}

/**
 * @class SFImage
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
SFImage::SFImage(): d_w(0), d_h(0), d_nc(0), d_pixels(0) {}

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
SFImage::SFImage(size_t width, size_t height, size_t components,
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
SFImage::SFImage(const SFImage& rhs): d_w(0), d_h(0), d_nc(0),
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
SFImage::~SFImage()
{
  delete [] d_pixels; 
}

/**
 * @brief Assignment.
 */
SFImage & SFImage::operator=(const SFImage & rhs) {
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

ostream& SFImage::print(ostream& os) const
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

FieldValue * SFImage::clone() const throw (std::bad_alloc) {
    return new SFImage(*this);
}

FieldValue & SFImage::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFImage &>(value));
}

FieldValue::Type SFImage::type() const {
    return sfimage;
}

/**
 * @brief Get the image width.
 *
 * @return the image width
 */
size_t SFImage::getWidth() const {
    return this->d_w;
}

/**
 * @brief Get the image height.
 *
 * @return the image height
 */
size_t SFImage::getHeight() const {
    return this->d_h;
}

/**
 * @brief Get the number of components.
 *
 * @return the number of components
 */
size_t SFImage::getComponents() const {
    return this->d_nc;
}

/**
 * @brief Get the pixel data.
 *
 * @return a pointer to the array of pixel data.
 */
const unsigned char * SFImage::getPixels() const {
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
void SFImage::set(size_t width, size_t height, size_t components,
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
 * @class SFInt32
 *
 * @brief Encapsulates an SFInt32 value.
 */

/**
 * @brief Constructor.
 * @param value initial value
 */
SFInt32::SFInt32(long value): d_value(value) {}

/**
 * @brief Destructor.
 */
SFInt32::~SFInt32() {}

ostream & SFInt32::print(ostream& os) const
{ return (os << d_value); }

FieldValue * SFInt32::clone() const throw (std::bad_alloc) {
    return new SFInt32(*this);
}

FieldValue & SFInt32::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFInt32 &>(value));
}

FieldValue::Type SFInt32::type() const {
    return sfint32;
}

/**
 * @brief Get value.
 *
 * @return the integer value
 */
long SFInt32::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new integer value
 */
void SFInt32::set(long value) {
    this->d_value = value;
}


/**
 * @class SFNode
 *
 * @brief Encapsulates an SFNode.
 */

/**
 * @brief Constructor.
 *
 * @param node a NodePtr
 */
SFNode::SFNode(const NodePtr & node): node(node) {}

/**
 * @brief Destructor.
 */
SFNode::~SFNode() {}

ostream & SFNode::print(ostream & os) const {
    if (!this->node) {
        return os << "NULL" << endl;
    }
    return os << *this->node << endl;
}

FieldValue * SFNode::clone() const throw (std::bad_alloc) {
    return new SFNode(*this);
}

FieldValue & SFNode::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFNode &>(value));
}

FieldValue::Type SFNode::type() const {
    return FieldValue::sfnode;
}

/**
 * @brief Get value.
 *
 * @return a smart pointer to this object's Node
 */
const NodePtr & SFNode::get() const {
    return this->node;
}

/**
 * @brief Set value.
 *
 * @param node a smart pointer to a Node, or to 0 if setting this
 *             SFNode to <code>NULL</code>
 */
void SFNode::set(const NodePtr & node) {
    this->node = NodePtr(node);
}


/**
 * @class SFRotation
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
SFRotation::SFRotation() {
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
SFRotation::SFRotation(const float rot[4]) {
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
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
SFRotation::SFRotation(float x, float y, float z, float angle) {
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
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
SFRotation::SFRotation(const SFVec3f & axis, float angle) {
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
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
SFRotation::SFRotation(const SFVec3f & fromVector,
                                 const SFVec3f & toVector) {
    this->setAxis(fromVector.cross(toVector));
    this->d_x[3] = acos(fromVector.dot(toVector) /
                        (fromVector.length() * toVector.length()));
}

/**
 * @brief Destructor.
 */
SFRotation::~SFRotation() {}

ostream & SFRotation::print(ostream& os) const
{ return (os <<d_x[0] << " " <<d_x[1] << " " <<d_x[2]<< " " <<d_x[3]); }

FieldValue * SFRotation::clone() const throw (std::bad_alloc) {
    return new SFRotation(*this);
}

FieldValue & SFRotation::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFRotation &>(value));
}

FieldValue::Type SFRotation::type() const 
{ return sfrotation; }

/**
 * @brief Get the <var>x</var>-component of the rotation axis.
 *
 * @return the <var>x</var>-component of the rotation axis.
 */
float SFRotation::getX() const {
    return this->d_x[0];
}

namespace {
    void normalizeAxis_(float axis[3]) {
        using OpenVRML_::length;
        using OpenVRML_::fpequal;
        using OpenVRML_::normalize;
    
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
void SFRotation::setX(float value) {
    this->d_x[0] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the <var>y</var>-component of the rotation axis.
 *
 * @return the <var>y</var>-component of the rotation axis
 */
float SFRotation::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component of the rotation axis.
 *
 * @param value
 */
void SFRotation::setY(float value) {
    this->d_x[1] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the <var>z</var>-component of the rotation axis.
 *
 * @return the <var>z</var>-component of the rotation axis
 */
float SFRotation::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component of the rotation axis
 *
 * @param value
 */
void SFRotation::setZ(float value) {
    this->d_x[2] = value;
    normalizeAxis_(this->d_x);
}

/**
 * @brief Get the rotation angle.
 *
 * @return the rotation angle
 */
float SFRotation::getAngle() const {
    return this->d_x[3];
}

/**
 * @brief Set the rotation angle.
 *
 * @param value
 */
void SFRotation::setAngle(float value) {
    this->d_x[3] = value;
}

/**
 * @brief Get the value of this rotation.
 *
 * @return a reference to a 4-element array.
 */
const float (&SFRotation::get() const)[4] {
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
void SFRotation::set(const float rot[4]) {
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
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
const SFVec3f SFRotation::getAxis() const {
    return SFVec3f(this->d_x[0], this->d_x[1], this->d_x[2]);
}

/**
 * @brief Set the axis of rotation using a SFVec3f.
 *
 * @param axis the new rotation axis
 *
 * @pre <var>axis</var> is a normalized vector.
 */
void SFRotation::setAxis(const SFVec3f & axis) {
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
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
const SFRotation SFRotation::inverse() const {
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
const SFRotation
        SFRotation::multiply(const SFRotation & rot) const {
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

void SFRotation::toQuaternion(float theQuat[4]) const {
    const float sintd2 = sin(d_x[3] * 0.5);
    const float len =
            sqrt((d_x[0] * d_x[0]) + (d_x[1] * d_x[1]) + (d_x[2] * d_x[2]));
    const float f = sintd2 / len;
    theQuat[3] = cos(d_x[3] * 0.5);
    theQuat[0] = d_x[0] * f;
    theQuat[1] = d_x[1] * f;
    theQuat[2] = d_x[2] * f;
}

void SFRotation::fromQuaternion(const float theQuat[4]) {
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
const SFVec3f
        SFRotation::multVec(const SFVec3f & vec) const {
    return SFVec3f();
}

/**
 * @brief Perform a <b>S</b>pherical <b>L</b>inear Int<b>ERP</b>olation.
 *
 * @param destRotation the destination rotation
 * @param t the interval fraction
 */
const SFRotation SFRotation::slerp(const SFRotation & destRotation,
                                   const float t) const {
    using OpenVRML_::fptolerance;
    
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
    if ((1.0 - cosom) > fptolerance) {
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
 * @class SFString
 *
 * @brief Encapsulates an SFString.
 */

/**
 * @brief Constructor.
 *
 * @param value
 */
SFString::SFString(const std::string & value): value(value) {}

/**
 * @brief Destructor.
 */
SFString::~SFString() {}

/**
 * @brief Get value.
 *
 * @return a string
 */
const std::string & SFString::get() const {
    return this->value;
}

/**
 * @brief Set value.
 *
 * @param value
 */
void SFString::set(const std::string & value) {
    this->value = value;
}

FieldValue * SFString::clone() const throw (std::bad_alloc) {
    return new SFString(*this);
}

FieldValue & SFString::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFString &>(value));
}

ostream & SFString::print(ostream& os) const
{ return (os << '\"' << this->value.c_str() << '\"'); }

FieldValue::Type SFString::type() const {
    return sfstring;
}


/**
 * @class SFTime
 *
 * @brief Encapsulates an SFTime value.
 */

/**
 * @brief Constructor
 *
 * @param value initial value
 */
SFTime::SFTime(double value): d_value(value) {}

/**
 * @brief Destructor.
 */
SFTime::~SFTime() {}

FieldValue * SFTime::clone() const throw (std::bad_alloc) {
    return new SFTime(*this);
}

FieldValue & SFTime::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFTime &>(value));
}

ostream & SFTime::print(ostream& os) const
{ return (os << d_value); }

FieldValue::Type SFTime::type() const {
    return sftime;
}

/**
 * @brief Get value.
 *
 * @return the value.
 */
double SFTime::get() const {
    return this->d_value;
}

/**
 * @brief Set value.
 *
 * @param value the new value
 */
void SFTime::set(double value) {
    this->d_value = value;
}


/**
 * @class SFVec2f
 *
 * @brief Encapsulates a SFVec2f value.
 */

/**
 * @brief Construct a SFVec2f with the default values, (0, 0).
 */
SFVec2f::SFVec2f() {
    this->d_x[0] = this->d_x[1] = 0;
}
        
/**
 * @brief Construct a SFVec2f.
 *
 * @param vec a 2-element array
 */
SFVec2f::SFVec2f(const float vec[2]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
}
        
/**
 * @brief Construct a SFVec2f.
 *
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 */
SFVec2f::SFVec2f(float x, float y) {
    this->d_x[0] = x;
    this->d_x[1] = y;
}

/**
 * @brief Destructor.
 */
SFVec2f::~SFVec2f() {}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 1. 0 corresponds to the
 *              <var>x</var>-component, and 1 corresponds to the
 *              <var>y</var>-component.
 */
float SFVec2f::operator[](size_t index) const {
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
float & SFVec2f::operator[](size_t index) {
    assert(index < 2);
    return this->d_x[index];
}

ostream & SFVec2f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1]); }

FieldValue * SFVec2f::clone() const throw (std::bad_alloc) {
    return new SFVec2f(*this);
}

FieldValue & SFVec2f::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFVec2f &>(value));
}

/**
 * @brief Get the field type.
 *
 * @return the Type enumerant corresponding to this field type
 */
FieldValue::Type SFVec2f::type() const {
    return sfvec2f;
}

/**
 * @brief Get the x component.
 */
float SFVec2f::getX() const {
    return this->d_x[0];
}
    
/**
 * @brief Set the x component.
 */
void SFVec2f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the y component.
 */
float SFVec2f::getY() const {
    return this->d_x[1];
}
        
/**
 * @brief Set the y component.
 */
void SFVec2f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the value of this vector.
 *
 * @returns a reference to a 2-element array.
 */
const float (&SFVec2f::get() const)[2] {
    return this->d_x;
}

/**
 * @brief Set the value of this vector.
 *
 * @param vec a 2-element array.
 */
void SFVec2f::set(const float vec[2]) {
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
const SFVec2f SFVec2f::add(const SFVec2f & vec) const {
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
const SFVec2f SFVec2f::divide(float number) const {
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
double SFVec2f::dot(const SFVec2f & vec) const {
    return (this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1]);
}

/**
 * @brief Geometric length.
 *
 * @return the length of this vector.
 */
double SFVec2f::length() const
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
const SFVec2f SFVec2f::multiply(float number) const {
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
const SFVec2f SFVec2f::negate() const {
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
const SFVec2f SFVec2f::normalize() const {
    using OpenVRML_::fpzero;
    
    const double len = this->length();
    if (fpzero(len)) { return *this; }
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
const SFVec2f SFVec2f::subtract(const SFVec2f & vec) const {
    SFVec2f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    return result;
}


/**
 * @class SFVec3f
 *
 * @brief Encapsulates a SFVec3f value.
 */

/**
 * @brief Construct a SFVec3f with the default value, (0, 0, 0).
 */
SFVec3f::SFVec3f() {}

/**
 * @brief Construct a SFVec3f.
 *
 * @param vec a 3-element array
 */
SFVec3f::SFVec3f(const float vec[3]) {
    std::copy(vec, vec + 3, this->d_x);
}

/**
 * @brief Construct a SFVec3f.
 *
 * @param x the <var>x</var>-component
 * @param y the <var>y</var>-component
 * @param z the <var>z</var>-component
 */
SFVec3f::SFVec3f(float x, float y, float z)
{ d_x[0] = x; d_x[1] = y; d_x[2] = z; }

/**
 * @brief Destructor.
 */
SFVec3f::~SFVec3f() {}

/**
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 2. 0 corresponds to the
 *              <var>x</var>-component, 1 corresponds to the
 *              <var>y</var>-component, and 2 corresponds to the
 *              <var>z</var>-component.
 */
float SFVec3f::operator[](size_t index) const {
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
float & SFVec3f::operator[](size_t index) {
    assert(index < 3);
    return this->d_x[index];
}

ostream& SFVec3f::print(ostream& os) const
{ return (os << d_x[0] << " " << d_x[1] << " " << d_x[2]); }

FieldValue * SFVec3f::clone() const throw (std::bad_alloc) {
    return new SFVec3f(*this);
}

FieldValue & SFVec3f::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFVec3f &>(value));
}

FieldValue::Type SFVec3f::type() const {
    return sfvec3f;
}

/**
 * @brief Get the <var>x</var>-component
 *
 * @return the <var>x</var>-component of this vector
 */
float SFVec3f::getX() const {
    return this->d_x[0];
}

/**
 * @brief Set the <var>x</var>-component
 *,
 * @param value
 */
void SFVec3f::setX(float value) {
    this->d_x[0] = value;
}

/**
 * @brief Get the <var>y</var>-component
 *
 * @return the <var>y</var>-component of this vector
 */
float SFVec3f::getY() const {
    return this->d_x[1];
}

/**
 * @brief Set the <var>y</var>-component
 *
 * @param value
 */
void SFVec3f::setY(float value) {
    this->d_x[1] = value;
}

/**
 * @brief Get the <var>z</var>-component
 *
 * @return the <var>z</var>-component of this vector
 */
float SFVec3f::getZ() const {
    return this->d_x[2];
}

/**
 * @brief Set the <var>z</var>-component
 *
 * @param value
 */
void SFVec3f::setZ(float value) {
    this->d_x[2] = value;
}

/**
 * @brief Get the vector value.
 *
 * @return a reference to a 3-element array
 */
const float (&SFVec3f::get() const)[3] {
    return this->d_x;
}

/**
 * @brief Set the vector value.
 *
 * @param vec a 3-element array
 */
void SFVec3f::set(const float vec[3]) {
    this->d_x[0] = vec[0];
    this->d_x[1] = vec[1];
    this->d_x[2] = vec[2];
}

/**
 * @brief Add this vector and vec component-wise.
 *
 * @param vec
 */
const SFVec3f SFVec3f::add(const SFVec3f & vec) const {
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
const SFVec3f SFVec3f::cross(const SFVec3f & vec) const {
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
const SFVec3f SFVec3f::divide(float number) const {
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
double SFVec3f::dot(const SFVec3f & vec) const {
    return ((this->d_x[0] * vec.d_x[0]) + (this->d_x[1] * vec.d_x[1])
            + (this->d_x[2] * vec.d_x[2]));
}

/**
 * @brief Get the length of this vector.
 *
 * @return the geometric length of the vector.
 */
double SFVec3f::length() const {
    using OpenVRML_::fpzero;
    
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
const SFVec3f SFVec3f::multiply(float number) const {
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
const SFVec3f SFVec3f::negate() const {
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
const SFVec3f SFVec3f::normalize() const {
    using OpenVRML_::fpzero;
    
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
const SFVec3f SFVec3f::subtract(const SFVec3f & vec) const {
    SFVec3f result(*this);
    result.d_x[0] -= vec.d_x[0];
    result.d_x[1] -= vec.d_x[1];
    result.d_x[2] -= vec.d_x[2];
    return result;
}


/**
 * @class MFColor
 *
 * @brief Encapsulates a MFColor.
 */

class MFColor::FData { // reference counted float data
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
MFColor::MFColor(size_t length, float const * colors):
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
MFColor::MFColor(const MFColor & mfColor):
        d_data(mfColor.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFColor::~MFColor() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param rhs value to assign to this object
 *
 * @return a reference to this object
 */
MFColor & MFColor::operator=(const MFColor & rhs) {
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
const float * MFColor::get() const {
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
void MFColor::set(size_t length, const float * colors) {
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
const float * MFColor::getElement(size_t index) const {
    assert(index < this->getLength());
    return (this->d_data->d_v + (index * 3L));
}

/**
 * @brief Set element.
 *
 * @param index the index of the element to set
 * @param value a 3-element float array comprising the new color value
 */
void MFColor::setElement(size_t index, const float value[3]) {
    assert(index < this->getLength());
    std::copy(value, value + 3, this->d_data->d_v + (index * 3));
}

/**
 * @brief Get the length.
 *
 * @return the number of color values (RGB triplets)
 */
size_t MFColor::getLength() const {
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
void MFColor::setLength(size_t length) {
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

void MFColor::insertElement(size_t index, const float value[3])
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

void MFColor::removeElement(size_t index)
{
  if (3 * index < d_data->d_n)
  {
    d_data->d_n -= 3;
    memcpy(d_data->d_v + 3 * index, d_data->d_v + 3 * (index + 1), 
	   (d_data->d_n - 3 * index) * sizeof(float));
  }	
}

FieldValue * MFColor::clone() const throw (std::bad_alloc) {
    return new MFColor(*this);
}

FieldValue & MFColor::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFColor &>(value));
}

FieldValue::Type MFColor::type() const {
    return mfcolor;
}

ostream & MFColor::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 3L); }


/**
 * @class MFFloat
 *
 * @brief Encapsulates a MFFloat.
 */

class MFFloat::FData { // reference counted float data
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
MFFloat::MFFloat(size_t length, float const * numbers):
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
MFFloat::MFFloat(const MFFloat & mfFloat):
        d_data(mfFloat.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFFloat::~MFFloat() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfFloat the object to copy
 */
MFFloat & MFFloat::operator=(const MFFloat & mfFloat) {
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
const float * MFFloat::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 *
 * @param length the number of float values
 * @param numbers a pointer to a float array
 */
void MFFloat::set(size_t length, const float * numbers) {
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
float MFFloat::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFFloat::setElement(size_t index, float value) {
    assert(index < this->getLength());
    this->d_data->d_v[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t MFFloat::getLength() const {
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
void MFFloat::setLength(size_t length) {
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

void MFFloat::insertElement(size_t index, float value)
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

void MFFloat::removeElement(size_t index)
{
  if (index < d_data->d_n)
  {
    d_data->d_n--;
    memcpy(d_data->d_v + index, d_data->d_v + (index + 1), 
	   (d_data->d_n - index) * sizeof(float));
  }	
}

FieldValue * MFFloat::clone() const throw (std::bad_alloc) {
    return new MFFloat(*this);
}

FieldValue & MFFloat::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFFloat &>(value));
}

FieldValue::Type MFFloat::type() const {
    return mffloat;
}

ostream & MFFloat::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 1); }


/**
 * @class MFInt32
 *
 * @brief Encapsulates a MFInt32.
 */

class MFInt32::IData { // reference counted int data
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
MFInt32::MFInt32(size_t length, const long * numbers):
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
MFInt32::MFInt32(const MFInt32 & mfInt32):
        d_data(mfInt32.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFInt32::~MFInt32() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfInt32 the object to copy into this one
 *
 * @return a reference to this object
 */
MFInt32 & MFInt32::operator=(const MFInt32 & mfInt32)
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
const long * MFInt32::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set value.
 *
 * @param length the number of integer values
 * @param numbers a pointer to a long array
 */
void MFInt32::set(size_t length, const long * numbers) {
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
long MFInt32::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->d_v[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFInt32::setElement(size_t index, long value) {
    assert(index < this->getLength());
    this->d_data->d_v[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of integer values
 */
size_t MFInt32::getLength() const {
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
void MFInt32::setLength(size_t length) {
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

void MFInt32::insertElement(size_t index, long value)
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

void MFInt32::removeElement(size_t index)
{
  if (index < d_data->d_n)
  {
    (d_data->d_n)--;
    memcpy(d_data->d_v + index, d_data->d_v + (index + 1), 
	   (d_data->d_n - index) * sizeof(int));
  }
}

FieldValue * MFInt32::clone() const throw (std::bad_alloc) {
    return new MFInt32(*this);
}

FieldValue & MFInt32::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFInt32 &>(value));
}

FieldValue::Type MFInt32::type() const {
    return mfint32;
}

ostream & MFInt32::print(ostream& os) const
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
 * @class MFNode
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
MFNode::MFNode(const size_t length, const NodePtr * nodes):
        nodes(length) {
    if (nodes) {
        std::copy(nodes, nodes + length, this->nodes.begin());
    }
}

/**
 * @brief Destructor.
 */
MFNode::~MFNode() {}

/**
 * @brief Get element.
 *
 * @param index
 * @return a smart pointer to a Node
 */
const NodePtr &
        MFNode::getElement(const size_t index) const throw () {
    assert(index < this->nodes.size());
    return this->nodes[index];
}

/**
 * @brief Remove all elements.
 */
void MFNode::clear()
{
    this->setLength(0);
}

/**
 * @brief Set element.
 *
 * @param index
 * @param node
 */
void MFNode::setElement(const size_t index, const NodePtr & node) {
    assert(index < this->nodes.size());
    this->nodes[index] = node;
}

/**
 * @brief Get the length.
 *
 * @return the number of nodes in the array
 */
size_t MFNode::getLength() const throw () {
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
void MFNode::setLength(const size_t length) {
    this->nodes.resize(length);
}

/**
 * @brief Determine if a node exists in this MFNode.
 *
 * @param node
 */
bool MFNode::exists(const Node & node) const {
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
 * Add @p node to the array if it isn't already part of this MFNode.
 * This method will <strong>not</strong> add NULLs.
 *
 * @param node a pointer to the node to add
 *
 * @return @c true if a node was added, @c false otherwise.
 */
bool MFNode::addNode(const NodePtr & node) {
    assert(node);
    if (!this->exists(*node)) {
        this->nodes.push_back(node);
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
bool MFNode::removeNode(const Node & node) {
    for (std::vector<NodePtr>::iterator i(this->nodes.begin());
            i != this->nodes.end(); ++i) {
        if (i->get() == &node) {
            this->nodes.erase(i);
	    return true;
        }
    }
    return false;
}

void MFNode::insertElement(size_t index, const NodePtr & node)
{
  assert(index < this->nodes.size());
  this->nodes.insert(this->nodes.begin() + index, node);
}

void MFNode::removeElement(size_t index)
{
  assert(index < this->nodes.size());
  this->nodes.erase(this->nodes.begin() + index);
}

FieldValue * MFNode::clone() const throw (std::bad_alloc) {
    return new MFNode(*this);
}

FieldValue & MFNode::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFNode &>(value));
}

FieldValue::Type MFNode::type() const {
    return mfnode;
}

ostream & MFNode::print(ostream & os) const {
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
 * @class MFRotation
 *
 * @brief Encapsulates an MFRotation.
 */

class MFRotation::FData { // reference counted float data
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
MFRotation::MFRotation(size_t length, const float * rotations):
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
MFRotation::MFRotation(const MFRotation & mfrotation):
        d_data(mfrotation.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFRotation::~MFRotation() {
    this->d_data->deref();
}

/**
 * @brief Assignment operator.
 *
 * @param mfrotation the object to copy into this one
 */
MFRotation &
        MFRotation::operator=(const MFRotation & mfrotation) {
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
const float * MFRotation::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the rotation values.
 *
 * @param length the number of rotation values in the passed array
 * @param rotations a pointer to an array of rotation values
 */
void MFRotation::set(size_t length, const float * rotations) {
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
const float * MFRotation::getElement(size_t index) const {
    assert(index * 4 < this->d_data->d_n);
    return this->d_data->d_v + (index * 4);
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFRotation::setElement(size_t index, const float value[4]) {
    assert(index * 4 < this->d_data->d_n);
    std::copy(value, value + 4, this->d_data->d_v + (index * 4));
}

/**
 * @brief Get the length.
 *
 * @return the number of rotation values
 */
size_t MFRotation::getLength() const {
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
void MFRotation::setLength(size_t length) {
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

void MFRotation::insertElement(size_t index, const float value[3])
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

void MFRotation::removeElement(size_t index)
{
  if (4 * index < d_data->d_n)
  {
    d_data->d_n -= 4;
    memcpy(d_data->d_v + 4 * index, d_data->d_v + 4 * (index + 1),
	   (d_data->d_n - 4 * index) * sizeof(float));
  }
}

FieldValue * MFRotation::clone() const throw (std::bad_alloc) {
    return new MFRotation(*this);
}

FieldValue & MFRotation::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFRotation &>(value));
}

FieldValue::Type MFRotation::type() const {
    return mfrotation;
}

ostream & MFRotation::print(ostream& os) const
{ return mffprint(os, get(), getLength(), 4); }


/**
 * @class MFString
 *
 * @brief Encapsulates a MFString.
 */

/**
 * @brief Constructor.
 *
 * @param length the length of the passed array
 * @param values an array of std::string
 */
MFString::MFString(size_t length, const std::string * values):
        values(length) {
    if (values) {
        std::copy(values, values + length, this->values.begin());
    }
}

/**
 * @brief Destructor.
 */
MFString::~MFString() {}

/**
 * @brief Get an element of the array.
 *
 * @param index the index of the element to retrieve
 *
 * @return the array element
 */
const std::string & MFString::getElement(size_t index) const {
    assert(index < this->values.size());
    return this->values[index];
}

/**
 * @brief Set an element of the array.
 *
 * @param index the index of the element to set
 * @param value the new value
 */
void MFString::setElement(size_t index, const std::string & value) {
    assert(index < this->values.size());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of values in the array
 */
size_t MFString::getLength() const {
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
void MFString::setLength(const size_t length) {
  this->values.resize(length);
}

void MFString::insertElement(size_t index, const std::string & value)
{
  assert(index < this->values.size());
  this->values.insert(this->values.begin() + index, value);
}

void MFString::removeElement(size_t index)
{
  assert(index < this->values.size());
  this->values.erase(this->values.begin() + index);
}

FieldValue * MFString::clone() const throw (std::bad_alloc) {
    return new MFString(*this);
}

FieldValue & MFString::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFString &>(value));
}

FieldValue::Type MFString::type() const {
    return mfstring;
}

ostream & MFString::print(ostream& os) const
{
  int n = getLength();

  if (n != 1) os << '[';
  for (int i=0; i<n; ++i)
    os << '\"' << (this->values[i]).c_str() << "\" ";
  if (n != 1) os << ']';

  return os;
}


/**
 * @class MFTime
 *
 * @brief Encapsulates a MFTime.
 */

//
// Reference-counted double data
//
class MFTime::DData {
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

MFTime::DData::DData(size_t size): refs(1), size(size),
        data(size > 0 ? new double[size] : 0) {}

MFTime::DData::~DData() {
    delete [] data;
}

MFTime::DData * MFTime::DData::ref() {
    ++refs;
    return this;
}

void MFTime::DData::deref() {
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
MFTime::MFTime(size_t length, const double * times):
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
MFTime::MFTime(const MFTime & mftime): d_data(mftime.d_data->ref())
{}

/**
 * @brief Destructor.
 */
MFTime::~MFTime() {
    this->d_data->deref();
}

/**
 * @brief Assignment operator.
 *
 * @param mftime the object to copy into this one
 */
MFTime & MFTime::operator=(const MFTime & mftime) {
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
const double * MFTime::get() const {
    return this->d_data->data;
}

/**
 * @brief Set value.
 *
 * @param length the number of time values
 * @param times a pointer to a double array
 */
void MFTime::set(size_t length, const double * times) {
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
double MFTime::getElement(size_t index) const {
    assert(index < this->getLength());
    return this->d_data->data[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFTime::setElement(size_t index, double value) {
    assert(index < this->getLength());
    this->d_data->data[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t MFTime::getLength() const {
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
void MFTime::setLength(size_t length) {
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

void MFTime::insertElement(size_t index, double value)
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

void MFTime::removeElement(size_t index)
{
  if (index < d_data->size)
  {
    d_data->size--;
    memcpy(d_data->data + index, d_data->data + (index + 1), 
	   (d_data->size - index) * sizeof(double));
  }
}

FieldValue * MFTime::clone() const throw (std::bad_alloc) {
    return new MFTime(*this);
}

FieldValue & MFTime::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFTime &>(value));
}

FieldValue::Type MFTime::type() const {
    return mftime;
}

ostream & MFTime::print(ostream & os) const {
    return mfdprint(os, get(), getLength(), 1);
}


/**
 * @class MFVec2f
 *
 * @brief Encapsulates an MFVec2f.
 */

class MFVec2f::FData { // reference counted float data
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
MFVec2f::MFVec2f(size_t length, const float * vecs):
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
MFVec2f::MFVec2f(const MFVec2f & mfvec2f):
        d_data(mfvec2f.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFVec2f::~MFVec2f() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfvec2f the object to copy into this one
 */
MFVec2f & MFVec2f::operator=(const MFVec2f & mfvec2f) {
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
const float * MFVec2f::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of 2-D vector values
 */
void MFVec2f::set(size_t length, const float * vecs) {
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
const float * MFVec2f::getElement(size_t index) const {
    assert((index * 2) < this->d_data->d_n);
    return (this->d_data->d_v + (index * 2));
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFVec2f::setElement(size_t index, const float value[2]) {
    assert((index * 2) < this->d_data->d_n);
    std::copy(value, value + 2, this->d_data->d_v + (index * 2));
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t MFVec2f::getLength() const {
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
void MFVec2f::setLength(size_t length) {
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

void MFVec2f::insertElement(size_t index, const float data[2])
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

void MFVec2f::removeElement(size_t index)
{
  if (2 * index < d_data->d_n)
  {
    d_data->d_n -= 2;
    memcpy(d_data->d_v + 2 * index, d_data->d_v + 2 * (index + 1), 
	   (d_data->d_n - 2 * index) * sizeof(float));
  }
}

FieldValue * MFVec2f::clone() const throw (std::bad_alloc) {
    return new MFVec2f(*this);
}

FieldValue & MFVec2f::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFVec2f &>(value));
}

FieldValue::Type MFVec2f::type() const {
    return mfvec2f;
}

ostream & MFVec2f::print(ostream & os) const
{ return mffprint(os, get(), getLength(), 2); }


/**
 * @class MFVec3f
 *
 * @brief Encapsulates an MFVec3f.
 */

class MFVec3f::FData { // reference counted float data
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
MFVec3f::MFVec3f(size_t length, const float * vecs):
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
MFVec3f::MFVec3f(const MFVec3f & mfvec3f):
        d_data(mfvec3f.d_data->ref()) {}

/**
 * @brief Destructor.
 */
MFVec3f::~MFVec3f() { d_data->deref(); }

/**
 * @brief Assignment operator.
 *
 * @param mfvec3f the object to copy into this one
 */
MFVec3f & MFVec3f::operator=(const MFVec3f & mfvec3f) {
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
const float * MFVec3f::get() const {
    return this->d_data->d_v;
}

/**
 * @brief Set the vector values.
 *
 * @param length the number of vector values in the passed array
 * @param vecs a pointer to an array of 3-D vector values
 */
void MFVec3f::set(size_t length, const float * vecs) {
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
const float * MFVec3f::getElement(size_t index) const {
    assert((index * 3) < this->d_data->d_n);
    return (this->d_data->d_v + (index * 3));
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFVec3f::setElement(size_t index, const float value[3]) {
    assert((index * 3) < this->d_data->d_n);
    std::copy(value, value + 3, this->d_data->d_v + (index * 3));
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t MFVec3f::getLength() const {
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
void MFVec3f::setLength(size_t length) {
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

void MFVec3f::insertElement(size_t index, const float value[3])
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

void MFVec3f::removeElement(size_t index)
{
  if (3 * index < d_data->d_n)
  {
    d_data->d_n -= 3;
    memcpy(d_data->d_v + 3 * index, d_data->d_v + 3 * (index + 1),
	   (d_data->d_n - 3 * index) * sizeof(float));
  }
}

FieldValue * MFVec3f::clone() const throw (std::bad_alloc) {
    return new MFVec3f(*this);
}

FieldValue & MFVec3f::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFVec3f &>(value));
}

FieldValue::Type MFVec3f::type() const {
    return mfvec3f;
}

ostream & MFVec3f::print(ostream& os) const
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

} // namespace OpenVRML
