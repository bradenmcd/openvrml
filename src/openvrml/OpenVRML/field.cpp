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

namespace OpenVRML {

/**
 * @brief Stream output.
 *
 * @param out           an output stream.
 * @param fieldValue    a FieldValue.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const FieldValue & fieldValue)
{
    fieldValue.print(out);
    return out;
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
 * These tags are typically used to designate an expected type or to avoid a
 * @c dynamic_cast.
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
 * @brief Constructor.
 */
FieldValue::FieldValue() throw ()
{}

/**
 * @brief Copy constructor.
 */
FieldValue::FieldValue(const FieldValue & value) throw ()
{}

/**
 * @brief Destructor.
 */
FieldValue::~FieldValue() throw ()
{}

/**
 * @brief Assignment operator.
 */
FieldValue & FieldValue::operator=(const FieldValue & value) throw ()
{
    return *this;
}

/**
 * @fn std::auto_ptr<FieldValue> FieldValue::clone() const throw (std::bad_alloc)
 *
 * @brief Virtual copy constructor.
 *
 * @return a new FieldValue identical to this one.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn FieldValue & FieldValue::assign(const FieldValue & value) throw (std::bad_cast, std::bad_alloc)
 *
 * @brief Virtual assignment.
 *
 * @param value the value to assign to the object.
 *
 * @return this object.
 *
 * @exception std::bad_cast     if @p value is not of the same concrete type as
 *                              this object.
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn void FieldValue::print(std::ostream & out) const
 *
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */

/**
 * @fn FieldValue::Type FieldValue::type() const throw ()
 *
 * @brief Get the field type.
 *
 * @return the Type enumerant corresponding to this FieldValue's type
 */

namespace {
    const char * const fieldValueTypeId_[] = {
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
 * @brief Stream output.
 *
 * @relates FieldValue
 *
 * If @p type is FieldValue::invalidType, @c failbit is set on @p out.
 *
 * @param out   an output stream.
 * @param type  a FieldValue type identifier.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const FieldValue::Type type)
{
    if (type == FieldValue::invalidType) {
        out.setstate(std::ios_base::failbit);
    } else {
        out << fieldValueTypeId_[type];
    }
    return out;
}

/**
 * @brief Stream input.
 *
 * @relates FieldValue
 *
 * @param in    an input stream.
 * @param type  a FieldValue type identifier.
 *
 * @return @p in.
 */
std::istream & operator>>(std::istream & in, FieldValue::Type & type)
{
    std::string str;
    in >> str;
    static const char * const * const begin =
            fieldValueTypeId_ + FieldValue::sfbool;
    static const char * const * const end =
            fieldValueTypeId_ + FieldValue::mfvec3f + 1;
    const char * const * const pos = std::find(begin, end, str);
    if (pos != end) {
        type = FieldValue::Type(pos - begin);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
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
SFBool::SFBool(const bool value) throw ():
    value(value)
{}

/**
 * @brief Destructor.
 */
SFBool::~SFBool() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFBool::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFBool(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFBool.
 */
FieldValue & SFBool::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFBool &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFBool::print(std::ostream & out) const
{
    out << (this->value ? "TRUE" : "FALSE");
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfbool.
 */
FieldValue::Type SFBool::type() const throw ()
{
    return FieldValue::sfbool;
}

/**
 * @fn bool SFBool::get() const throw ()
 *
 * @brief Get the value.
 *
 * @return the value of this SFBool
 */

/**
 * @fn void SFBool::set(const bool value) throw ()
 *
 * @brief Set the value.
 *
 * @param value the new value
 */

/**
 * @fn bool operator==(const SFBool & lhs, const SFBool & rhs) throw ()
 *
 * @relates SFBool
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFBool & lhs, const SFBool & rhs) throw ()
 *
 * @relates SFBool
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class SFColor
 *
 * @brief Encapsulates an SFColor value.
 */

/**
 * @typedef SFColor::ConstArrayReference
 *
 * @brief A reference to the SFColor's 3-element array.
 */

/**
 * @brief Construct with the default value, (0, 0, 0).
 */
SFColor::SFColor() throw ()
{
    this->value[0] = 0.0f;
    this->value[1] = 0.0f;
    this->value[2] = 0.0f;
}

/**
 * @brief Construct a SFColor.
 *
 * @param rgb a 3-element array
 */
SFColor::SFColor(ConstArrayReference rgb) throw ()
{
    this->value[0] = rgb[0];
    this->value[1] = rgb[1];
    this->value[2] = rgb[2];
}

/**
 * @brief Construct a SFColor
 *
 * @param r red component
 * @param g green component
 * @param b blue component
 */
SFColor::SFColor(const float r,
                 const float g,
                 const float b)
    throw ()
{
    this->value[0] = r;
    this->value[1] = g;
    this->value[2] = b;
}

/**
 * @brief Destructor.
 */
SFColor::~SFColor() throw () {}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFColor::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFColor(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFColor.
 */
FieldValue & SFColor::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFColor &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFColor::print(std::ostream & out) const
{
    out << this->value[0] << ' ' << this->value[1] << ' ' << this->value[2];
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfcolor.
 */
FieldValue::Type SFColor::type() const throw ()
{
    return FieldValue::sfcolor;
}

/**
 * @fn float SFColor::operator[](const size_t index) const throw ()
 *
 * @brief Array element dereference operator (const version).
 *
 * @param index an index from 0 - 2.
 *
 * @pre @p index is not larger than 2.
 */

/**
 * @fn float & SFColor::operator[](const size_t index) throw ()
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param index an index from 0 - 2.
 *
 * @pre @p index is not larger than 2.
 */

/**
 * @fn float SFColor::getR() const throw ()
 *
 * @brief Get the red component.
 *
 * @return the red component value.
 */

/**
 * @fn float SFColor::getG() const throw ()
 *
 * @brief Get the green component.
 *
 * @return the green component value.
 */

/**
 * @fn float SFColor::getB() const throw ()
 *
 * @brief Get the blue component.
 *
 * @return the blue component value.
 */

/**
 * @fn SFColor::ConstArrayReference SFColor::get() const throw ()
 *
 * @brief Get the value.
 *
 * @return a reference to a 3-element array comprising the RGB value.
 */

/**
 * @brief Set the value.
 *
 * @param rgb a 3-element vector comprising a RGB value.
 */
void SFColor::set(ConstArrayReference rgb) throw ()
{
    this->value[0] = rgb[0];
    this->value[1] = rgb[1];
    this->value[2] = rgb[2];
}

// Conversion functions between RGB each in [0,1] and HSV with  
// h in [0,360), s,v in [0,1]. From Foley, van Dam p615-616.

/**
 * @brief Convert a color from HSV to RGB.
 *
 * Convert from HSV (with(with @e h in [0,360), @e s, @e v in [0,1]) to RGB
 * (with each component in [0,1]).
 *
 * @param hsv   a 3-element array comprising an HSV value
 * @retval rgb  a 3-element array comprising an RGB value
 */
void SFColor::HSVtoRGB(ConstArrayReference hsv, ArrayReference rgb) throw ()
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
 * Convert from RGB (with each component in [0,1]) to HSV (with @e h in
 * [0,360), @e s, @e v in [0,1]).
 *
 * @param rgb   a 3-element array comprising an RGB value.
 * @retval hsv  a 3-element array comprising an HSV value.
 */
void SFColor::RGBtoHSV(ConstArrayReference rgb, ArrayReference hsv) throw ()
{
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
void SFColor::setHSV(const float h, const float s, const float v) throw ()
{
    const float hsv[3] = { h, s, v };
    SFColor::HSVtoRGB(hsv, this->value);
}

/**
 * @brief Get the value expressed in HSV.
 *
 * @retval hsv a 3-element array comprising the HSV value.
 */
void SFColor::getHSV(ArrayReference hsv) const throw ()
{
    SFColor::RGBtoHSV(this->value, hsv);
}

/**
 * @fn bool operator==(const SFColor & lhs, const SFColor & rhs) throw ()
 *
 * @relates SFColor
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFColor & lhs, const SFColor & rhs) throw ()
 *
 * @relates SFColor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
SFFloat::SFFloat(const float value) throw ():
    value(value)
{}

/**
 * @brief Destructor.
 */
SFFloat::~SFFloat() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFFloat::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFFloat::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFFloat(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFFloat.
 */
FieldValue & SFFloat::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFFloat &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sffloat.
 */
FieldValue::Type SFFloat::type() const throw ()
{
    return FieldValue::sffloat;
}

/**
 * @fn float SFFloat::get() const throw ()
 *
 * @brief Get value.
 *
 * @return the SFFloat value
 */

/**
 * @fn void SFFloat::set(float value) throw ()
 *
 * @brief Set value.
 *
 * @param value the new value
 */

/**
 * @fn bool operator==(const SFFloat & lhs, const SFFloat & rhs) throw ()
 *
 * @relates SFFloat
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFFloat & lhs, const SFFloat & rhs) throw ()
 *
 * @relates SFFloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
SFImage::SFImage() throw (): d_w(0), d_h(0), d_nc(0), d_pixels(0) {}

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
 * @exception std::bad_alloc    if memory allocation fails.
 */
SFImage::SFImage(size_t width, size_t height, size_t components,
                 const unsigned char * pixels) throw (std::bad_alloc):
        d_w(0L), d_h(0L), d_nc(0L), d_pixels(0L) {
    const size_t nbytes = width * height * components;
    this->d_pixels = new unsigned char[nbytes];
    this->d_w = width;
    this->d_h = height;
    this->d_nc = components;
    std::copy(pixels, pixels + (width * height * components), this->d_pixels);
}

/**
 * @brief Copy constructor.
 *
 * @param sfimage   the SFImage object to copy.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
SFImage::SFImage(const SFImage & sfimage) throw (std::bad_alloc):
        d_w(0), d_h(0), d_nc(0), d_pixels(0) {
    const size_t nbytes = sfimage.d_w * sfimage.d_h * sfimage.d_nc;
    this->d_pixels = new unsigned char[nbytes];
    this->d_w = sfimage.d_w;
    this->d_h = sfimage.d_h;
    this->d_nc = sfimage.d_nc;
    std::copy(sfimage.d_pixels, sfimage.d_pixels + nbytes, this->d_pixels);
}

/**
 * @brief Destructor.
 */
SFImage::~SFImage() throw () { delete [] d_pixels; }

/**
 * @brief Assignment.
 *
 * @param sfimage   the SFImage value to assign to the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
SFImage & SFImage::operator=(const SFImage & sfimage) throw (std::bad_alloc) {
    if (this != &sfimage) {
        delete [] this->d_pixels;
        this->d_w = this->d_h = this->d_nc = 0L;
        const size_t nbytes = sfimage.d_w * sfimage.d_h * sfimage.d_nc;
        this->d_pixels = new unsigned char[nbytes];
        this->d_w = sfimage.d_w;
        this->d_h = sfimage.d_h;
        this->d_nc = sfimage.d_nc;
        std::copy(sfimage.d_pixels, sfimage.d_pixels + nbytes, this->d_pixels);
    }
    return *this;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFImage::print(std::ostream & out) const
{
    out << d_w << " " << d_h << " " << d_nc;

    size_t np = d_w * d_h;
    unsigned char * p = d_pixels;

    for (size_t i = 0; i < np; ++i) {
        unsigned int pixval = 0;
        for (size_t j=0; j<d_nc; ++j) { pixval = (pixval << 8) | *p++; }
        out << " " << pixval;
    }
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFImage::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFImage(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFImage.
 */
FieldValue & SFImage::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const SFImage &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfimage.
 */
FieldValue::Type SFImage::type() const throw () { return sfimage; }

/**
 * @brief Get the image width.
 *
 * @return the image width
 */
size_t SFImage::getWidth() const throw () { return this->d_w; }

/**
 * @brief Get the image height.
 *
 * @return the image height
 */
size_t SFImage::getHeight() const throw () { return this->d_h; }

/**
 * @brief Get the number of components.
 *
 * @return the number of components
 */
size_t SFImage::getComponents() const throw () { return this->d_nc; }

/**
 * @brief Get the pixel data.
 *
 * @return a pointer to the array of pixel data.
 */
const unsigned char * SFImage::getPixels() const throw () {
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
                  const unsigned char * pixels)
        throw (std::bad_alloc) {
    delete this->d_pixels;
    
    this->d_w = width;
    this->d_h = height;
    this->d_nc = components;
    
    this->d_pixels = new unsigned char[width * height * components];
    std::copy(pixels, pixels + (width * height * components), this->d_pixels);
}

/**
 * @class SFInt32
 *
 * @brief Encapsulates an SFInt32 value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value.
 */
SFInt32::SFInt32(const long value) throw ():
    value(value)
{}

/**
 * @brief Destructor.
 */
SFInt32::~SFInt32() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFInt32::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFInt32::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFInt32(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFInt32.
 */
FieldValue & SFInt32::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFInt32 &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfint32.
 */
FieldValue::Type SFInt32::type() const throw ()
{
    return FieldValue::sfint32;
}

/**
 * @fn long SFInt32::get() const throw ()
 *
 * @brief Get value.
 *
 * @return the integer value
 */

/**
 * @fn void SFInt32::set(const long value) throw ()
 *
 * @brief Set value.
 *
 * @param value the new integer value
 */

/**
 * @fn bool operator==(const SFInt32 & lhs, const SFInt32 & rhs) throw ()
 *
 * @relates SFInt32
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFInt32 & lhs, const SFInt32 & rhs) throw ()
 *
 * @relates SFInt32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
SFNode::SFNode(const NodePtr & node) throw ():
    node(node)
{}

/**
 * @brief Destructor.
 */
SFNode::~SFNode() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFNode::print(std::ostream & out) const
{
    if (this->node) {
        out << *this->node;
    } else {
        out << "NULL";
    }
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFNode::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFNode(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFNode.
 */
FieldValue & SFNode::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFNode &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfnode.
 */
FieldValue::Type SFNode::type() const throw ()
{
    return FieldValue::sfnode;
}

/**
 * @fn const NodePtr & SFNode::get() const throw ()
 *
 * @brief Get value.
 *
 * @return a smart pointer to this object's Node
 */

/**
 * @fn void SFNode::set(const NodePtr & node) throw ()
 *
 * @brief Set value.
 *
 * @param node a smart pointer to a Node, or to 0 if setting this
 *             SFNode to @c NULL.
 */

/**
 * @fn bool operator==(const SFNode & lhs, const SFNode & rhs) throw ()
 *
 * @relates SFNode
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFNode & lhs, const SFNode & rhs) throw ()
 *
 * @relates SFNode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
 * - Attempts to construct an SFRotation axis from a vector that is not
 *   normalized will yield an assertion failure (abort) unless NDEBUG is
 *   defined when compiling the library (in which case truly wacky behavior
 *   could result).
 * - Assignment to individual components of the axis will result in the
 *   axis being re-normalized upon each assignment.
 */

/**
 * @typedef SFRotation::ConstArrayReference
 *
 * @brief A reference to the SFRotation's 4-element array.
 */

/**
 * @brief Default constructor.
 *
 * Construct with the default value, (0, 0, 1, 0).
 */
SFRotation::SFRotation() throw ()
{
    this->value[0] = 0.0; // x
    this->value[1] = 0.0; // y
    this->value[2] = 1.0; // z
    this->value[3] = 0.0; // angle
}

/**
 * @brief Constructor.
 *
 * @param rot   a 4-element array.
 *
 * @pre The first three elements of the argument array constitute a
 *      normalized vector.
 */
SFRotation::SFRotation(ConstArrayReference rot) throw ()
{
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(rot), 1.0));
    
    std::copy(rot, rot + 4, this->value);
}

/**
 * @brief Constructor.
 *
 * @param x the @e x-component of the axis of rotation
 * @param y the @e y-component of the axis of rotation
 * @param z the @e z-component of the axis of rotation
 * @param angle the rotation angle
 *
 * @pre The first three arguments constitute a normalized vector.
 */
SFRotation::SFRotation(const float x,
                       const float y,
                       const float z,
                       const float angle)
    throw ()
{
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
    this->value[0] = x;
    this->value[1] = y;
    this->value[2] = z;
    
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(this->value), 1.0));
    
    this->value[3] = angle;
}

/**
 * @brief Constructor.
 *
 * @param axis a normalized vector to use as the axis of rotation
 * @param angle the rotation angle
 *
 * @pre The first argument is a normalized vector.
 */
SFRotation::SFRotation(const SFVec3f & axis, const float angle) throw ()
{
    using OpenVRML_::fpequal;
    using OpenVRML_::length;
    
    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(axis.get()), 1.0));
    
    std::copy(axis.get(), axis.get() + 3, this->value);
    this->value[3] = angle;
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
SFRotation::SFRotation(const SFVec3f & fromVector, const SFVec3f & toVector)
    throw ()
{
    this->setAxis(fromVector.cross(toVector));
    this->value[3] = acos(fromVector.dot(toVector)
                            / (fromVector.length() * toVector.length()));
}

/**
 * @brief Destructor.
 */
SFRotation::~SFRotation() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFRotation::print(std::ostream & out) const
{
    out << this->value[0] << ' ' << this->value[1] << ' ' << this->value[2]
        << ' ' << this->value[3];
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFRotation::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFRotation(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFRotation.
 */
FieldValue & SFRotation::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFRotation &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfrotation.
 */
FieldValue::Type SFRotation::type() const throw ()
{
    return FieldValue::sfrotation;
}

/**
 * @fn SFRotation::ConstArrayReference SFRotation::get() const throw ()
 *
 * @brief Get the value of this rotation.
 *
 * @return a reference to a 4-element array.
 */

/**
 * @brief Set the value of this rotation.
 *
 * @param rot a 4-element array
 *
 * @pre The first three elements of @p rot constitute a normalized
 *      vector.
 */
void SFRotation::set(ConstArrayReference rot) throw ()
{
    using OpenVRML_::fpequal;
    using OpenVRML_::length;

    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(rot), 1.0));

    std::copy(rot, rot + 4, this->value);
}

/**
 * @fn float SFRotation::getX() const throw ()
 *
 * @brief Get the @e x-component of the rotation axis.
 *
 * @return the @e x-component of the rotation axis.
 */

namespace {

    void normalizeAxis_(float axis[3]) throw ()
    {
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
 * @brief Set the @e x-component of the rotation axis.
 *
 * @param value
 */
void SFRotation::setX(const float value) throw ()
{
    this->value[0] = value;
    normalizeAxis_(this->value);
}

/**
 * @fn float SFRotation::getY() const throw ()
 *
 * @brief Get the @e y-component of the rotation axis.
 *
 * @return the @e y-component of the rotation axis.
 */

/**
 * @brief Set the @e y-component of the rotation axis.
 *
 * @param value
 */
void SFRotation::setY(const float value) throw ()
{
    this->value[1] = value;
    normalizeAxis_(this->value);
}

/**
 * @fn float SFRotation::getZ() const throw ()
 *
 * @brief Get the @e z-component of the rotation axis.
 *
 * @return the @e z-component of the rotation axis.
 */

/**
 * @brief Set the @e z-component of the rotation axis.
 *
 * @param value
 */
void SFRotation::setZ(const float value) throw ()
{
    this->value[2] = value;
    normalizeAxis_(this->value);
}

/**
 * @fn float SFRotation::getAngle() const throw ()
 *
 * @brief Get the rotation angle.
 *
 * @return the rotation angle.
 */

/**
 * @fn void SFRotation::setAngle(const float value) throw ()
 *
 * @brief Set the rotation angle.
 *
 * @param value
 */

/**
 * @brief Get the axis of rotation as a SFVec3f.
 *
 * @return the axis of rotation
 */
const SFVec3f SFRotation::getAxis() const throw ()
{
    return SFVec3f(this->value[0],
                   this->value[1],
                   this->value[2]);
}

/**
 * @brief Set the axis of rotation using a SFVec3f.
 *
 * @param axis  the new rotation axis.
 *
 * @pre @p axis is a normalized vector.
 */
void SFRotation::setAxis(const SFVec3f & axis) throw ()
{
    using OpenVRML_::fpequal;
    using OpenVRML_::length;

    //
    // Make sure axis is normalized.
    //
    assert(fpequal(length(axis.get()), 1.0));

    std::copy(axis.get(), axis.get() + 3, this->value);
}

/**
 * @brief Get the inverse.
 *
 * @return a SFRotation that is the inverse of this one.
 */
const SFRotation SFRotation::inverse() const throw ()
{
    SFRotation result(*this);
    result.value[3] = -value[3];
    return result;
}

namespace {
    void multQuat(const float quat1[4], const float quat2[4], float result[4])
        throw ()
    {
        result[3] = quat1[3] * quat2[3] - quat1[0] * quat2[0]
                    - quat1[1] * quat2[1] - quat1[2] * quat2[2];

        result[0] = quat1[3] * quat2[0] + quat1[0] * quat2[3]
                    + quat1[1] * quat2[2] - quat1[2] * quat2[1];

        result[1] = quat1[3] * quat2[1] + quat1[1] * quat2[3]
                    + quat1[2] * quat2[0] - quat1[0] * quat2[2];

        result[2] = quat1[3] * quat2[2] + quat1[2] * quat2[3]
                    + quat1[0] * quat2[1] - quat1[1] * quat2[0];
    }
    
    void sfrotToQuat(const SFRotation & rot, float quat[4]) throw ()
    {
        const float sintd2 = sin(rot.getAngle() * 0.5);
        const float len = sqrt((rot.getX() * rot.getX())
                             + (rot.getY() * rot.getY())
                             + (rot.getZ() * rot.getZ()));
        const float f = sintd2 / len;
        quat[3] = cos(rot.getAngle() * 0.5);
        quat[0] = rot.getX() * f;
        quat[1] = rot.getY() * f;
        quat[2] = rot.getZ() * f;
    }
    
    void quatToSFRot(const float quat[4], SFRotation & rot) throw ()
    {
        double sina2 = sqrt(quat[0] * quat[0]
                          + quat[1] * quat[1]
                          + quat[2] * quat[2]);
        const double angle = 2.0 * atan2(sina2, double(quat[3]));

        if (sina2 >= 1e-8) {
	    sina2 = 1.0 / sina2;
            rot.setX(quat[0] * sina2);
            rot.setY(quat[1] * sina2);
            rot.setZ(quat[2] * sina2);
            rot.setAngle(angle);
        } else {
            static const float r[4] = { 0.0, 1.0, 0.0, 0.0 };
            rot.set(r);
        }
    }
}

/**
 * @brief Multiply two rotations.
 *
 * @param rot   the rotation by which to multiply this one.
 *
 * @return the result rotation.
 */
const SFRotation SFRotation::multiply(const SFRotation & rot) const throw ()
{
    // convert to quaternions
    float quatUS[4], quatVec[4];
    sfrotToQuat(*this, quatUS);
    sfrotToQuat(rot, quatVec);
    
    // multiply quaternions
    float resultQuat[4];
    multQuat(quatUS, quatVec, resultQuat);
    
    // now convert back to axis/angle
    SFRotation result;
    quatToSFRot(resultQuat, result);
    return result;
}

/**
 * @brief Multiply the matrix corresponding to this rotation by a vector.
 *
 * @todo IMPLEMENT ME!
 *
 * @param vec   vector by which to multiply this rotation.
 *
 * @return the result of multiplying this rotation by vec.
 */
const SFVec3f SFRotation::multVec(const SFVec3f & vec) const
{
    return SFVec3f();
}

/**
 * @brief Perform a Spherical Linear IntERPolation.
 *
 * @param destRotation  the destination rotation
 * @param t             the interval fraction
 */
const SFRotation SFRotation::slerp(const SFRotation & destRotation,
                                   const float t) const
    throw ()
{
    using OpenVRML_::fptolerance;
    
    float fromQuat[4], toQuat[4];
    sfrotToQuat(*this, fromQuat);
    sfrotToQuat(destRotation, toQuat);
    
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
    quatToSFRot(resultQuat, result);
    return result;
}

/**
 * @fn bool operator==(const SFRotation & lhs, const SFRotation & rhs) throw ()
 *
 * @relates SFRotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFRotation & lhs, const SFRotation & rhs) throw ()
 *
 * @relates SFRotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class SFString
 *
 * @brief Encapsulates an SFString.
 */

/**
 * @brief Constructor.
 *
 * @param value
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
SFString::SFString(const std::string & value) throw (std::bad_alloc):
   value(value)
{}

/**
 * @brief Destructor.
 */
SFString::~SFString() throw ()
{}

/**
 * @fn const std::string & SFString::get() const throw ()
 *
 * @brief Get value.
 *
 * @return a string
 */

/**
 * @fn void SFString::set(const std::string & value) throw (std::bad_alloc)
 *
 * @brief Set value.
 *
 * @param value
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFString::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFString(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if @p value is not an SFString.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & SFString::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const SFString &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFString::print(std::ostream & out) const
{
    out << '\"' << this->value << '\"';
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfstring.
 */
FieldValue::Type SFString::type() const throw ()
{
    return FieldValue::sfstring;
}

/**
 * @fn bool operator==(const SFString & lhs, const SFString & rhs) throw ()
 *
 * @relates SFString
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFString & lhs, const SFString & rhs) throw ()
 *
 * @relates SFString
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
SFTime::SFTime(double value) throw ():
    value(value)
{}

/**
 * @brief Destructor.
 */
SFTime::~SFTime() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFTime::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFTime(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFString.
 */
FieldValue & SFTime::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFTime &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFTime::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sftime.
 */
FieldValue::Type SFTime::type() const throw ()
{
    return FieldValue::sftime;
}

/**
 * @fn double SFTime::get() const throw ()
 *
 * @brief Get value.
 *
 * @return the value.
 */

/**
 * @fn void SFTime::set(double value) throw ()
 *
 * @brief Set value.
 *
 * @param value the new value
 */

/**
 * @fn bool operator==(const SFTime & lhs, const SFTime & rhs) throw ()
 *
 * @relates SFTime
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFTime & lhs, const SFTime & rhs) throw ()
 *
 * @relates SFTime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class SFVec2f
 *
 * @brief Encapsulates a SFVec2f value.
 */

/**
 * @typedef SFVec2f::ConstArrayReference
 *
 * @brief A reference to the SFVec2f's 2-element array.
 */

/**
 * @brief Construct a SFVec2f with the default values, (0, 0).
 */
SFVec2f::SFVec2f() throw ()
{
    this->value[0] = this->value[1] = 0;
}

/**
 * @brief Construct a SFVec2f.
 *
 * @param vec a 2-element array.
 */
SFVec2f::SFVec2f(ConstArrayReference vec) throw ()
{
    this->value[0] = vec[0];
    this->value[1] = vec[1];
}

/**
 * @brief Construct a SFVec2f.
 *
 * @param x the @e x-component.
 * @param y the @e y-component.
 */
SFVec2f::SFVec2f(const float x, const float y) throw ()
{
    this->value[0] = x;
    this->value[1] = y;
}

/**
 * @brief Destructor.
 */
SFVec2f::~SFVec2f() throw ()
{}

/**
 * @fn float SFVec2f::operator[](size_t index) const throw ()
 *
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 1. 0 corresponds to the @e x-component, and 1
 *              corresponds to the @e y-component.
 */

/**
 * @fn float & SFVec2f::operator[](size_t index) throw ()
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param index a value from 0 - 1. 0 corresponds to the @e x-component, and 1
 *              corresponds to the @e y-component.
 */

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFVec2f::print(std::ostream & out) const
{
    out << this->value[0] << ' ' << this->value[1];
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFVec2f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFVec2f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFVec2f.
 */
FieldValue & SFVec2f::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFVec2f &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfvec2f.
 */
FieldValue::Type SFVec2f::type() const throw ()
{
    return FieldValue::sfvec2f;
}

/**
 * @fn float SFVec2f::getX() const throw ()
 *
 * @brief Get the @e x-component.
 *
 * @return the @e x-component.
 */
    
/**
 * @fn void SFVec2f::setX(const float value) throw ()
 *
 * @brief Set the @e x-component.
 *
 * @param value the new @e x-component value.
 */

/**
 * @fn float SFVec2f::getY() const throw ()
 *
 * @brief Get the @e y-component.
 *
 * @return the @e y-component.
 */
        
/**
 * @fn void SFVec2f::setY(const float value) throw ()
 *
 * @brief Set the @e y-component.
 *
 * @param value the new @e y-component value.
 */

/**
 * @fn SFVec2f::ConstArrayReference SFVec2f::get() const throw ()
 *
 * @brief Get the value of this vector.
 *
 * @returns a reference to a 2-element array.
 */

/**
 * @fn void SFVec2f::set(ConstArrayReference vec) throw ()
 *
 * @brief Set the value of this vector.
 *
 * @param vec a 2-element array.
 */

/**
 * @brief Add two vectors.
 *
 * @param vec   the vector to add to this one.
 *
 * @return a SFVec2f with a value that is the passed SFVec2f added,
 *         componentwise, to this object.
 */
const SFVec2f SFVec2f::add(const SFVec2f & vec) const throw ()
{
    SFVec2f result(*this);
    result.value[0] += this->value[0];
    result.value[1] += this->value[1];
    return result;
}

/**
 * @brief Divide this vector by a scalar.
 *
 * @param number    a scalar value.
 *
 * @return a SFVec2f with a value that is the object divided by the
 *      passed numeric value.
 */
const SFVec2f SFVec2f::divide(const float number) const throw ()
{
    SFVec2f result(*this);
    result.value[0] /= number;
    result.value[1] /= number;
    return result;
}

/**
 * @brief Dot product.
 *
 * @param vec   a SFVec2f value.
 *
 * @return the dot product of this vector and vec.
 */
double SFVec2f::dot(const SFVec2f & vec) const throw ()
{
    return (this->value[0] * vec.value[0]) + (this->value[1] * vec.value[1]);
}

/**
 * @brief Geometric length.
 *
 * @return the length of this vector.
 */
double SFVec2f::length() const throw ()
{
    return sqrt(this->value[0] * this->value[0]
                + this->value[1] * this->value[1]);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param number    a scalar value.
 *
 * @return a SFVec2f with a value that is the object multiplied by the
 *      passed numeric value.
 */
const SFVec2f SFVec2f::multiply(const float number) const throw ()
{
    SFVec2f result(*this);
    result.value[0] *= number;
    result.value[1] *= number;
    return result;
}

/**
 * @brief Negate.
 *
 * @return a SFVec2f that the result of negating this vector.
 */
const SFVec2f SFVec2f::negate() const throw ()
{
    SFVec2f result;
    result.value[0] = -this->value[0];
    result.value[1] = -this->value[1];
    return result;
}

/**
 * @brief Normalize.
 *
 * @return a SFVec2f that is this vector normalized.
 */
const SFVec2f SFVec2f::normalize() const throw ()
{
    using OpenVRML_::fpzero;
    
    const double len = this->length();
    if (fpzero(len)) { return *this; }
    SFVec2f result(*this);
    result.value[0] /= len;
    result.value[1] /= len;
    return result;
}

/**
 * @brief Take the difference of two vectors.
 *
 * @param vec   the vector to subtract from this one.
 *
 * @return a SFVec2f that is the difference between this vector and vec.
 */
const SFVec2f SFVec2f::subtract(const SFVec2f & vec) const throw ()
{
    SFVec2f result(*this);
    result.value[0] -= vec.value[0];
    result.value[1] -= vec.value[1];
    return result;
}

/**
 * @fn bool operator==(const SFVec2f & lhs, const SFVec2f & rhs) throw ()
 *
 * @relates SFVec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFVec2f & lhs, const SFVec2f & rhs) throw ()
 *
 * @relates SFVec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class SFVec3f
 *
 * @brief Encapsulates a SFVec3f value.
 */

/**
 * @typedef SFVec3f::ConstArrayReference
 *
 * @brief A reference to the SFVec3f's 3-element array.
 */

/**
 * @brief Construct a SFVec3f with the default value, (0, 0, 0).
 */
SFVec3f::SFVec3f() throw ()
{}

/**
 * @brief Construct a SFVec3f.
 *
 * @param vec a 3-element array
 */
SFVec3f::SFVec3f(ConstArrayReference vec) throw ()
{
    std::copy(vec, vec + 3, this->value);
}

/**
 * @brief Construct a SFVec3f.
 *
 * @param x the @e x-component.
 * @param y the @e y-component.
 * @param z the @e z-component.
 */
SFVec3f::SFVec3f(const float x, const float y, const float z) throw ()
{
    this->value[0] = x;
    this->value[1] = y;
    this->value[2] = z;
}

/**
 * @brief Destructor.
 */
SFVec3f::~SFVec3f() throw ()
{}

/**
 * @fn float SFVec3f::operator[](const size_t index) const throw ()
 *
 * @brief Array element dereference operator (const version).
 *
 * @param index a value from 0 - 2. 0 corresponds to the @e x-component, 1
 *              corresponds to the @e y-component, and 2 corresponds to the
 *              @e z-component.
 */

/**
 * @fn float & SFVec3f::operator[](const size_t index) throw ()
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param index a value from 0 - 2. 0 corresponds to the @e x-component, 1
 *              corresponds to the @e y-component, and 2 corresponds to the
 *              @e z-component.
 */

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFVec3f::print(std::ostream & out) const
{
    out << this->value[0] << ' ' << this->value[1] << ' ' << this->value[2];
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> SFVec3f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new SFVec3f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFBool.
 */
FieldValue & SFVec3f::assign(const FieldValue & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const SFVec3f &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::sfvec3f.
 */
FieldValue::Type SFVec3f::type() const throw ()
{
    return FieldValue::sfvec3f;
}

/**
 * @fn float SFVec3f::getX() const throw ()
 *
 * @brief Get the @e x-component.
 *
 * @return the @e x-component of this vector.
 */

/**
 * @fn void SFVec3f::setX(const float value) throw ()
 *
 * @brief Set the @e x-component.
 *
 * @param value the new @e x-component value.
 */

/**
 * @fn float SFVec3f::getY() const throw ()
 *
 * @brief Get the @e y-component.
 *
 * @return the @e y-component of this vector.
 */

/**
 * @fn void SFVec3f::setY(const float value) throw ()
 *
 * @brief Set the @e y-component.
 *
 * @param value the new @e y-component value.
 */

/**
 * @fn float SFVec3f::getZ() const throw ()
 *
 * @brief Get the @e z-component.
 *
 * @return the @e z-component of this vector.
 */

/**
 * @fn void SFVec3f::setZ(const float value) throw ()
 *
 * @brief Set the @e z-component.
 *
 * @param value the new @e z-component value.
 */

/**
 * @fn SFVec3f::ConstArrayReference SFVec3f::get() const throw ()
 *
 * @brief Get the vector value.
 *
 * @return a reference to a 3-element array
 */

/**
 * @fn void SFVec3f::set(ConstArrayReference vec) throw ()
 *
 * @brief Set the vector value.
 *
 * @param vec   a 3-element array
 */

/**
 * @brief Add this vector and vec component-wise.
 *
 * @param vec   a vector.
 */
const SFVec3f SFVec3f::add(const SFVec3f & vec) const throw ()
{
    SFVec3f result(*this);
    result.value[0] += vec.value[0];
    result.value[1] += vec.value[1];
    result.value[2] += vec.value[2];
    return result;
}

/**
 * @brief Get the cross product of this vector and vec.
 *
 * @param vec   a vector.
 */
const SFVec3f SFVec3f::cross(const SFVec3f & vec) const throw ()
{
    SFVec3f result;
    result.value[0] = (this->value[1] * vec.value[2])
                        - (this->value[2] * vec.value[1]);
    result.value[1] = (this->value[2] * vec.value[0])
                        - (this->value[0] * vec.value[2]);
    result.value[2] = (this->value[0] * vec.value[1])
                        - (this->value[1] * vec.value[0]);
    return result;
}

/**
 * @brief Get the result of dividing this vector by number.
 *
 * @param number    a scalar value.
 */
const SFVec3f SFVec3f::divide(float number) const throw ()
{
    SFVec3f result(*this);
    result.value[0] /= number;
    result.value[1] /= number;
    result.value[2] /= number;
    return result;
}

/**
 * @brief Get the dot product of this vector and vec.
 *
 * @param vec   a vector.
 */
double SFVec3f::dot(const SFVec3f & vec) const throw ()
{
    return ((this->value[0] * vec.value[0]) + (this->value[1] * vec.value[1])
            + (this->value[2] * vec.value[2]));
}

/**
 * @brief Get the length of this vector.
 *
 * @return the geometric length of the vector.
 */
double SFVec3f::length() const throw ()
{
    using OpenVRML_::fpzero;
    
    const double len = sqrt((this->value[0] * this->value[0])
                          + (this->value[1] * this->value[1])
                          + (this->value[2] * this->value[2]));
    return fpzero(len) ? 0.0 : len;
}

/**
 * @brief Multiply by a scalar.
 *
 * @param number
 *
 * @return the product
 */
const SFVec3f SFVec3f::multiply(float number) const throw ()
{
    SFVec3f result(*this);
    result.value[0] *= number;
    result.value[1] *= number;
    result.value[2] *= number;
    return result;
}

/**
 * @brief Negate.
 *
 * @return the negatation of this vector
 */
const SFVec3f SFVec3f::negate() const throw ()
{
    SFVec3f result(*this);
    result.value[0] = -result.value[0];
    result.value[1] = -result.value[1];
    result.value[2] = -result.value[2];
    return result;
}

/**
 * @brief Normalize.
 *
 * @return a copy of this vector normalized
 */
const SFVec3f SFVec3f::normalize() const throw ()
{
    using OpenVRML_::fpzero;
    
    const double len = this->length();
    SFVec3f result(*this);
    if (!fpzero(len)) {
        result.value[0] /= len;
        result.value[1] /= len;
        result.value[2] /= len;
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
const SFVec3f SFVec3f::subtract(const SFVec3f & vec) const throw () {
    SFVec3f result(*this);
    result.value[0] -= vec.value[0];
    result.value[1] -= vec.value[1];
    result.value[2] -= vec.value[2];
    return result;
}

/**
 * @fn bool operator==(const SFVec3f & lhs, const SFVec3f & rhs) throw ()
 *
 * @relates SFVec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const SFVec3f & lhs, const SFVec3f & rhs) throw ()
 *
 * @relates SFVec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


namespace {
    
    template <typename ElementType, size_t ArraySize>
    class array_vector {
        enum { min_capacity = 1 };
        size_t size_;
        size_t capacity_;
        ElementType (*data_)[ArraySize];

    public:
        enum {_ArraySize = ArraySize};
        typedef ElementType value_type[_ArraySize];
        typedef value_type & reference;
        typedef const value_type & const_reference;
        typedef ElementType (*iterator)[ArraySize];
        typedef const ElementType (*const_iterator)[ArraySize];
#if defined (_WIN32)
        typedef std::reverse_iterator<iterator,char,char&,char*,int> reverse_iterator;
        typedef std::reverse_iterator<const_iterator,char,char&,char*,int> const_reverse_iterator;        
#else
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#endif
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        explicit array_vector(size_type size = 0):
            size_(size),
            capacity_((size > min_capacity) ? size : min_capacity),
            data_(new ElementType[capacity_][ArraySize])
        {}

        array_vector(size_type size, const_reference value):
            size_(size),
            capacity_((size > min_capacity) ? size : min_capacity),
            data_(new ElementType[capacity_][ArraySize])
        {
            for (iterator i(this->begin()); i != this->end(); ++i) {
                std::copy(value, value + ArraySize, *i);
            }
        }

        array_vector(const_iterator begin, const_iterator end):
            size_(end - begin),
            capacity_((size_ > min_capacity) ? size_ : min_capacity),
            data_(new ElementType[capacity_][ArraySize])
        {
            for (iterator i(begin), j(this->begin()); i != end; ++i, ++j) {
                std::copy(*i, *i + ArraySize, *j);
            }
        }

        array_vector(const array_vector & av):
            size_(av.size_),
            capacity_(av.capacity_),
            data_(new ElementType[av.capacity_][ArraySize])
        {
            const_iterator i(av.begin());
            iterator j(this->begin());
            for (; i != av.end(); ++i, ++j) {
                std::copy(*i, *i + ArraySize, *j);
            }
        }

        ~array_vector()
        {
            delete [] this->data_;
        }

        size_t size() const
        {
            return this->size_;
        }

        bool empty() const
        {
            return this->size_ == 0;
        }

        size_t max_size() const
        {
            return size_type(-1) / sizeof(ElementType[ArraySize]);
        }

        size_t capacity() const
        {
            return this->capacity_;
        }

        void reserve(size_type capacity)
        {
            if (this->capacity_ < capacity) {
                value_type * data = new value_type[capacity];
                for (iterator i(this->begin()), j(data); i != this->end();
                        ++i, ++j) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                delete [] this->data_;
                this->data_ = data;
                this->capacity_ = capacity;
            }
        }

        array_vector & operator=(const array_vector & av)
        {
            array_vector<ElementType, ArraySize> temp(av);
            this->swap(temp);
            return *this;
        }

        void assign(size_t size, const_reference value)
        {
            if (this->capacity_ < size) {
                delete [] this->data_;
                this->capacity_ = size;
                this->data = new value_type[this->capacity_];
            }
            for (iterator i(begin), j(this->begin()); i != end; ++i, ++j) {
                std::copy(*i, *i + ArraySize, *j);
            }
            this->size_ = size;
        }

        void assign(const_iterator begin, const_iterator end)
        {
            const size_t size = end - begin;
            if (this->capacity_ < size) {
                delete [] this->data_;
                this->capacity_ = size;
                this->data = new value_type[this->capacity_];
            }
            for (iterator i(begin), j(this->begin()); i != end; ++i, ++j) {
                std::copy(*i, *i + ArraySize, *j);
            }
            this->size_ = size;
        }

        void swap(array_vector & av) throw ()
        {
            std::swap(this->data_, av.data_);
            std::swap(this->size_, av.size_);
            std::swap(this->capacity_, av.capacity_);
        }

        const_reference at(size_type index) const
        {
            if (!(index < this->size_)) { throw std::out_of_range(); }
            return this->data_[index];
        }

        reference at(size_type index)
        {
            if (!(index < this->size_)) { throw std::out_of_range(); }
            return this->data_[index];
        }

        const_reference operator[](size_type index) const
        {
            return this->data_[index];
        }

        reference operator[](size_type index)
        {
            return this->data_[index];
        }

        const_reference front() const
        {
            return this->data_[0];
        }

        reference front()
        {
            return this->data_[0];
        }

        const_reference back() const
        {
            return this->data_[this->size_ - 1];
        }

        reference back()
        {
            return this->data_[this->size_ - 1];
        }

        const_iterator begin() const
        {
            return this->data_;
        }

        iterator begin()
        {
            return this->data_;
        }

        const_iterator end() const
        {
            return this->data_ + this->size_;
        }

        iterator end()
        {
            return this->data_ + this->size_;
        }

        const_reverse_iterator rbegin() const
        {
            return std::const_reverse_iterator(this->end());
        }

        reverse_iterator rbegin()
        {
            return std::reverse_iterator(this->end());
        }

        const_reverse_iterator rend() const
        {
            return std::const_reverse_iterator(this->begin());
        }

        reverse_iterator rend()
        {
            return std::reverse_iterator(this->begin());
        }

        iterator insert(iterator pos, const_reference value)
        {
            size_type n = pos - this->begin();
            this->insert(pos, 1, value);
            return this->begin() + n;
        }
        
        void insert(iterator pos, size_t num, const_reference value)
        {
            if (this->capacity_ < this->size_ + num) {
                value_type * data = new value_type[this->size_ + num];
                iterator i(this->begin()), j(data);
                for (; i != pos; i++, j++) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                std::copy(value, value + ArraySize, *j);
                ++j;
                for (; i != this->end(); ++i, ++j) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                delete [] this->data_;
                this->data_ = data;
                this->capacity_ = this->size_ + num;
            } else {
                iterator i;
                for (i = this->end() - num - 1; i >= pos; --i) {
                    std::copy(*i, *i + ArraySize, *(i + num));
                }
                for (i = pos; i < pos + num; ++i) {
                    std::copy(value, value + ArraySize, *i);
                }
            }
            this->size_ += num;
        }
        
        void insert(iterator pos, const_iterator begin, const_iterator end)
        {
            const ptrdiff_t num = end - begin;
            if (this->capacity_ < this->size_ + num) {
                value_type * data = new value_type[this->size_ + num];
                iterator i(this->begin()), j(data);
                for (; i != pos; i++, j++) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                for (i = begin; i != end; i++, j++) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                for (i = this->begin() + num; i != this->end(); ++i, ++j) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                delete [] this->data_;
                this->data_ = data;
                this->capacity_ = this->size_ + num;
            } else {
                iterator i;
                for (i = this->end() - num - 1; i >= pos; --i) {
                    std::copy(*i, *i + ArraySize, *(i + num));
                }
                for (i = pos; i < pos + num; ++i) {
                    std::copy(value, value + ArraySize, *i);
                }
            }
            this->size_ += num;
        }

        void push_back(const_reference value)
        {
            if (this->size_ == this->capacity_) {
                value_type * data = new value_type[this->capacity_ * 2];
                for (iterator i(this->begin()), j(data); i != this->end();
                        ++i, ++j) {
                    std::copy(*i, *i + ArraySize, *j);
                }
                delete [] this->data_;
                this->data_ = data;
                this->capacity_ *= 2;
            }
            std::copy(value, value + ArraySize, this->data_[this->size_]);
            ++this->size_;
        }

        void pop_back()
        {
            --this->size_;
        }

        iterator erase(iterator pos)
        {
            if (pos + 1 != this->end()) {
                for (iterator i(pos + 1), j(pos); i != this->end(); ++i, ++j) {
                    std::copy(*i, *i + ArraySize, *j);
                }
            }
            --this->size_;
            return pos;
        }

        iterator erase(iterator begin, iterator end)
        {
            for (iterator i(end), j(begin); i != this->end(); ++i, ++j) {
                std::copy(*i, *i + ArraySize, *j);
            }
            this->size_ -= end - begin;
            return begin;
        }

        void resize(size_type size)
        {
            ElementType value[ArraySize];
            this->resize(size, value);
        }

        void resize(size_type size, const_reference value)
        {
            if (size < this->size_) {
                this->erase(this->begin() + size, this->end());
            } else {
                this->insert(this->end(), size - this->size_, value);
            }
        }

        void clear()
        {
            this->erase(this->begin(), this->end());
        }
    };

    template <typename ElementType, size_t ArraySize>
    struct array_equal {
        bool operator()(ElementType (&lhs)[ArraySize],
                        ElementType (&rhs)[ArraySize]) const
        {
            return std::equal(lhs, lhs + ArraySize, rhs);
        }
    };

    template <typename ElementType, size_t ArraySize>
    inline bool operator==(const array_vector<ElementType, ArraySize> & lhs,
                           const array_vector<ElementType, ArraySize> & rhs)
    {
        std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                   array_equal<ElementType, ArraySize>());
    }

    template <typename ElementType, size_t ArraySize>
    inline bool operator!=(const array_vector<ElementType, ArraySize> & lhs,
                           const array_vector<ElementType, ArraySize> & rhs)
    {
        return !(lhs == rhs);
    }
}

/**
 * @internal
 */
typedef array_vector<float, 3> ColorVec;

/**
 * @class MFColor
 *
 * @brief Encapsulates a MFColor.
 */

/**
 * @var void * MFColor::values
 *
 * @brief Internal representation.
 */

/**
 * @brief Construct from a float array.
 *
 * @param length    the number of RGB triplets in the array
 * @param values    a pointer to an array of 3-element arrays comprising color
 *                  values to initialize the MFColor.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFColor::MFColor(size_t length, SFColor::ConstArrayPointer values)
    throw (std::bad_alloc):
    values(new ColorVec(length))
{
    if (values) {
        for (size_t i(0); i < length; ++i) { this->setElement(i, values[i]); }
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfcolor   the object to copy.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFColor::MFColor(const MFColor & mfcolor) throw (std::bad_alloc):
    values(new ColorVec(*static_cast<ColorVec *>(mfcolor.values)))
{}

/**
 * @brief Destructor.
 */
MFColor::~MFColor() throw ()
{
    delete static_cast<ColorVec *>(this->values);
}

/**
 * @brief Assignment operator.
 *
 * @param mfcolor value to assign to this object
 *
 * @return a reference to this object
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFColor & MFColor::operator=(const MFColor & mfcolor) throw (std::bad_alloc)
{
    MFColor temp(mfcolor);
    static_cast<ColorVec *>(temp.values)
        ->swap(*static_cast<ColorVec *>(this->values));
    return *this;
}

/**
 * @brief Get element.
 *
 * @return a 3-element array comprising an RGB triplet
 */
SFColor::ConstArrayReference MFColor::getElement(size_t index) const throw ()
{
    assert(index < this->getLength());
    return (*static_cast<ColorVec *>(this->values))[index];
}

/**
 * @brief Set element.
 *
 * @param index the index of the element to set
 * @param value a 3-element float array comprising the new color value
 */
void MFColor::setElement(size_t index, SFColor::ConstArrayReference value)
    throw ()
{
    assert(index < this->getLength());
    std::copy(value, value + 3,
              (*static_cast<ColorVec *>(this->values))[index]);
}

/**
 * @brief Get the length.
 *
 * @return the number of color values (RGB triplets)
 */
size_t MFColor::getLength() const throw ()
{
    return static_cast<ColorVec *>(this->values)->size();
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default color, (0, 0, 0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFColor::setLength(size_t length) throw (std::bad_alloc)
{
    static_cast<ColorVec *>(this->values)->resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFColor::addElement(SFColor::ConstArrayReference value)
    throw (std::bad_alloc)
{
    static_cast<ColorVec *>(this->values)->push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value an RGB triplet.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFColor::insertElement(const size_t index,
                            SFColor::ConstArrayReference value)
    throw (std::bad_alloc)
{
    assert(index < static_cast<ColorVec *>(this->values)->size());
    ColorVec & values = *static_cast<ColorVec *>(this->values);
    values.insert(values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFColor::removeElement(const size_t index) throw ()
{
    assert(index < static_cast<ColorVec *>(this->values)->size());
    ColorVec & values = *static_cast<ColorVec *>(this->values);
    values.erase(values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFColor::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFColor(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFColor object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFColor::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFColor &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfcolor.
 */
FieldValue::Type MFColor::type() const throw ()
{
    return FieldValue::mfcolor;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFColor::print(std::ostream & out) const
{
    const ColorVec & values = *static_cast<ColorVec *>(this->values);
    out << '[';
    if (this->getLength() > 1) {
        for (ColorVec::const_iterator i(values.begin()); i != values.end() - 1;
                ++i) {
            out << (*i)[0] << ' ' << (*i)[1] << ' ' << (*i)[2] << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << values.back()[0] << ' ' << values.back()[1] << ' '
            << values.back()[2];
    }
    out << ']';
}

/**
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const MFColor & lhs, const MFColor & rhs) throw ()
{
    assert(lhs.values);
    assert(rhs.values);

    return *static_cast<ColorVec *>(lhs.values)
            == *static_cast<ColorVec *>(rhs.values);
}

/**
 * @fn bool operator!=(const MFColor & lhs, const MFColor & rhs) throw ()
 *
 * @relates MFColor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class MFFloat
 *
 * @brief Encapsulates a MFFloat.
 */

/**
 * @brief Construct from a float array.
 *
 * @param length the number of floats in the array
 * @param values a pointer to a float array
 */
MFFloat::MFFloat(const size_t length, const float * values)
    throw (std::bad_alloc):
    values(length)
{
    if (values) {
        std::copy(values, values + length, this->values.begin());
    }
}

/**
 * @brief Destructor.
 */
MFFloat::~MFFloat() throw ()
{}

/**
 * @brief Get element.
 *
 * @param index
 */
const float & MFFloat::getElement(const size_t index) const throw ()
{
    assert(index < this->getLength());
    return this->values[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFFloat::setElement(const size_t index, const float value) throw ()
{
    assert(index < this->getLength());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t MFFloat::getLength() const throw ()
{
    return this->values.size();
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default (0.0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFFloat::setLength(const size_t length) throw (std::bad_alloc)
{
    this->values.resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFFloat::addElement(const float value)
    throw (std::bad_alloc)
{
    this->values.push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a float value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFFloat::insertElement(const size_t index, const float value)
    throw (std::bad_alloc)
{
    assert(index < this->values.size());
    this->values.insert(this->values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFFloat::removeElement(const size_t index) throw ()
{
    assert(index < this->values.size());
    this->values.erase(this->values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFFloat::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFFloat(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFFloat object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFFloat::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFFloat &>(value));
}

/**
 * @brief Get the type identifier for the class.
 *
 * @return the type identifer for the class.
 */
FieldValue::Type MFFloat::type() const throw ()
{
    return FieldValue::mffloat;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFFloat::print(std::ostream & out) const
{
    out << '[';
    if (this->getLength() > 1) {
        for (std::vector<float>::const_iterator i(this->values.begin());
                i != this->values.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << this->values.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFFloat & lhs, const MFFloat & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFFloat & lhs, const MFFloat & rhs) throw ()
 *
 * @relates MFFloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class MFInt32
 *
 * @brief Encapsulates a MFInt32.
 */

/**
 * @brief Construct from a long array.
 *
 * @param length    the number of integer values
 * @param values    a pointer to a long array
 */
MFInt32::MFInt32(const size_t length, const long * const values)
    throw (std::bad_alloc):
    values(length)
{
    if (values) {
        std::copy(values, values + length, this->values.begin());
    }
}

/**
 * @brief Destructor.
 */
MFInt32::~MFInt32() throw ()
{}

/**
 * @brief Get element.
 *
 * @param index
 */
const long & MFInt32::getElement(const size_t index) const throw ()
{
    assert(index < this->getLength());
    return this->values[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFInt32::setElement(const size_t index, const long value) throw ()
{
    assert(index < this->getLength());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of integer values
 */
size_t MFInt32::getLength() const throw ()
{
    return this->values.size();
}

/**
 * @brief Set the length.
 *
 * If the new length is greater than the current length, the additional values
 * are initialized to the default (0.0). If the new length is less
 * than the current length, the array is truncated.
 *
 * @param length new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFInt32::setLength(const size_t length) throw (std::bad_alloc)
{
    this->values.resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFInt32::addElement(const long value) throw (std::bad_alloc)
{
    this->values.push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value an integer value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFInt32::insertElement(const size_t index, const long value)
    throw (std::bad_alloc)
{
    assert(index < this->values.size());
    this->values.insert(this->values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFInt32::removeElement(const size_t index) throw ()
{
    assert(index < this->values.size());
    this->values.erase(this->values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFInt32::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFInt32(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFInt32 object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFInt32::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFInt32 &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfint32.
 */
FieldValue::Type MFInt32::type() const throw ()
{
    return FieldValue::mfint32;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFInt32::print(std::ostream & out) const
{
    out << '[';
    if (this->getLength() > 1) {
        for (std::vector<long>::const_iterator i(this->values.begin());
                i != this->values.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << this->values.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFInt32 & lhs, const MFInt32 & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFInt32 & lhs, const MFInt32 & rhs) throw ()
 *
 * @relates MFInt32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class MFNode
 *
 * @brief Encapsulates a MFNode.
 */

/**
 * @brief Construct from an array of Node pointers.
 *
 * @param length the length of the array
 * @param nodes a pointer to an array of Node pointers
 */
MFNode::MFNode(const size_t length, const NodePtr * nodes)
    throw (std::bad_alloc):
    nodes(length)
{
    if (nodes) { std::copy(nodes, nodes + length, this->nodes.begin()); }
}

/**
 * @brief Destructor.
 */
MFNode::~MFNode() throw ()
{}

/**
 * @brief Get element.
 *
 * @param index
 * @return a smart pointer to a Node
 */
const NodePtr & MFNode::getElement(const size_t index) const throw ()
{
    assert(index < this->nodes.size());
    return this->nodes[index];
}

/**
 * @brief Remove all elements.
 */
void MFNode::clear() throw ()
{
    this->nodes.clear();
}

/**
 * @brief Set element.
 *
 * @param index
 * @param node
 */
void MFNode::setElement(const size_t index, const NodePtr & node) throw ()
{
    assert(index < this->nodes.size());
    this->nodes[index] = node;
}

/**
 * @brief Get the length.
 *
 * @return the number of nodes in the array
 */
size_t MFNode::getLength() const throw ()
{
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
void MFNode::setLength(const size_t length) throw (std::bad_alloc)
{
    this->nodes.resize(length);
}

/**
 * @brief Determine if a node exists in this MFNode.
 *
 * @param node
 */
bool MFNode::exists(const Node & node) const
{
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
bool MFNode::addNode(const NodePtr & node)
{
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
bool MFNode::removeNode(const Node & node)
{
    for (std::vector<NodePtr>::iterator i(this->nodes.begin());
            i != this->nodes.end(); ++i) {
        if (i->get() == &node) {
            this->nodes.erase(i);
	    return true;
        }
    }
    return false;
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param node  the node to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFNode::addElement(const NodePtr & node) throw (std::bad_alloc)
{
    this->nodes.push_back(node);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p node should be inserted.
 * @param node  a NodePtr.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFNode::insertElement(size_t index, const NodePtr & node)
    throw (std::bad_alloc)
{
    assert(index < this->nodes.size());
    this->nodes.insert(this->nodes.begin() + index, node);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFNode::removeElement(size_t index) throw ()
{
    assert(index < this->nodes.size());
    this->nodes.erase(this->nodes.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFNode::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFNode(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFNode object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFNode::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFNode &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfnode.
 */
FieldValue::Type MFNode::type() const throw ()
{
    return FieldValue::mfnode;
}

/**
 * @brief Print to an output stream.
 *
 * Any null elements in the MFNode will not get printed; VRML97 syntax does not
 * accommodate NULL in an MFNode.
 *
 * @param out   an output stream.
 */
void MFNode::print(std::ostream & out) const
{
    out << '[';
    if (this->getLength() > 1) {
        for (std::vector<NodePtr>::const_iterator i(this->nodes.begin());
                i != this->nodes.end() - 1; ++i) {
            if (*i) { out << **i << ", "; }
        }
    }
    if (this->getLength() > 0 && this->nodes.back()) {
        out << *this->nodes.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFNode & lhs, const MFNode & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFNode & lhs, const MFNode & rhs) throw ()
 *
 * @relates MFNode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @internal
 */
typedef array_vector<float, 4> RotationVec;

/**
 * @class MFRotation
 *
 * @brief Encapsulates an MFRotation.
 */

/**
 * @var void * MFRotation::values
 *
 * @brief Internal representation.
 */

/**
 * @brief Construct from an array of rotation values.
 *
 * @param length    the number of rotation values in the passed array.
 * @param values    a pointer to an array of 4-element arrays comprising
 *                  rotation values to initialize the MFRotation.
 */
MFRotation::MFRotation(const size_t length,
                       SFRotation::ConstArrayPointer values)
    throw (std::bad_alloc):
    values(new RotationVec(length))
{
    if (values) {
        for (size_t i(0); i < length; ++i) { this->setElement(i, values[i]); }
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfrotation the object to copy
 */
MFRotation::MFRotation(const MFRotation & mfrotation) throw (std::bad_alloc):
    values(new RotationVec(*static_cast<RotationVec *>(mfrotation.values)))
{}

/**
 * @brief Destructor.
 */
MFRotation::~MFRotation() throw ()
{
    delete static_cast<RotationVec *>(this->values);
}

/**
 * @brief Assignment operator.
 *
 * @param mfrotation the object to copy into this one
 */
MFRotation & MFRotation::operator=(const MFRotation & mfrotation)
    throw (std::bad_alloc)
{
    MFRotation temp(mfrotation);
    static_cast<RotationVec *>(temp.values)
        ->swap(*static_cast<RotationVec *>(this->values));
    return *this;
}

/**
 * @brief Get element.
 *
 * @param index
 */
SFRotation::ConstArrayReference MFRotation::getElement(size_t index) const
    throw ()
{
    assert(index < this->getLength());
    return (*static_cast<RotationVec *>(this->values))[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFRotation::setElement(size_t index, SFRotation::ConstArrayReference value)
    throw ()
{
    assert(index < this->getLength());
    std::copy(value, value + 4,
              (*static_cast<RotationVec *>(this->values))[index]);
}

/**
 * @brief Get the length.
 *
 * @return the number of rotation values
 */
size_t MFRotation::getLength() const throw ()
{
    return static_cast<RotationVec *>(this->values)->size();
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
void MFRotation::setLength(size_t length) throw (std::bad_alloc)
{
    static_cast<RotationVec *>(this->values)->resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFRotation::addElement(SFRotation::ConstArrayReference value)
    throw (std::bad_alloc)
{
    static_cast<RotationVec *>(this->values)->push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a rotation value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFRotation::insertElement(size_t index,
                               SFRotation::ConstArrayReference value)
        throw (std::bad_alloc)
{
    assert(index < static_cast<RotationVec *>(this->values)->size());
    RotationVec & values = *static_cast<RotationVec *>(this->values);
    values.insert(values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFRotation::removeElement(const size_t index) throw ()
{
    assert(index < static_cast<RotationVec *>(this->values)->size());
    RotationVec & values = *static_cast<RotationVec *>(this->values);
    values.erase(values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFRotation::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFRotation(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFRotation object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFRotation::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFRotation &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfrotation.
 */
FieldValue::Type MFRotation::type() const throw ()
{
    return FieldValue::mfrotation;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFRotation::print(std::ostream & out) const
{
    const RotationVec & values = *static_cast<RotationVec *>(this->values);
    out << '[';
    if (this->getLength() > 1) {
        for (RotationVec::const_iterator i(values.begin()); i != values.end() - 1;
                ++i) {
            out << (*i)[0] << ' ' << (*i)[1] << ' ' << (*i)[2] << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << values.back()[0] << ' ' << values.back()[1] << ' '
            << values.back()[2];
    }
    out << ']';
}

/**
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const MFRotation & lhs, const MFRotation & rhs) throw ()
{
    assert(lhs.values);
    assert(rhs.values);

    return *static_cast<RotationVec *>(lhs.values)
            == *static_cast<RotationVec *>(rhs.values);
}

/**
 * @fn bool operator!=(const MFRotation & lhs, const MFRotation & rhs) throw ()
 *
 * @relates MFRotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


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
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFString::MFString(const size_t length, const std::string * values)
    throw (std::bad_alloc):
    values(length)
{
    if (values) { std::copy(values, values + length, this->values.begin()); }
}

/**
 * @brief Copy constructor.
 *
 * @param mfstring  the object to copy.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFString::MFString(const MFString & mfstring) throw (std::bad_alloc):
    values(mfstring.values)
{}

/**
 * @brief Destructor.
 */
MFString::~MFString() throw ()
{}

/**
 * @brief Assignment operator.
 *
 * @param mfstring  the object to copy into this one
 */
MFString & MFString::operator=(const MFString & mfstring)
    throw (std::bad_alloc)
{
    if (this != &mfstring) { this->values = mfstring.values; }
    return *this;
}

/**
 * @brief Get an element of the array.
 *
 * @param index the index of the element to retrieve
 *
 * @return the array element
 */
const std::string & MFString::getElement(size_t index) const throw ()
{
    assert(index < this->values.size());
    return this->values[index];
}

/**
 * @brief Set an element of the array.
 *
 * @param index the index of the element to set
 * @param value the new value
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFString::setElement(const size_t index, const std::string & value)
    throw (std::bad_alloc)
{
    assert(index < this->values.size());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of values in the array
 */
size_t MFString::getLength() const throw ()
{
    return this->values.size();
}

/**
 * @brief Set the length of the vector of std::strings.
 *
 * If the new length is greater than the current length, the new positions are
 * filled with empty std::strings.
 *
 * @param length the new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFString::setLength(const size_t length) throw (std::bad_alloc)
{
    this->values.resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFString::addElement(const std::string & value) throw (std::bad_alloc)
{
    this->values.push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a string.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFString::insertElement(const size_t index, const std::string & value)
    throw (std::bad_alloc)
{
    assert(index < this->values.size());
    this->values.insert(this->values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFString::removeElement(const size_t index) throw ()
{
    assert(index < this->values.size());
    this->values.erase(this->values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFString::clone() const throw (std::bad_alloc) {
    return std::auto_ptr<FieldValue>(new MFString(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFString object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFString::assign(const FieldValue & value)
        throw (std::bad_cast, std::bad_alloc) {
    return (*this = dynamic_cast<const MFString &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfstring.
 */
FieldValue::Type MFString::type() const throw () { return mfstring; }

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFString::print(std::ostream & out) const
{
    using std::string;
    using std::vector;
    
    out << '[';
    if (this->getLength() > 1) {
        for (std::vector<string>::const_iterator i(this->values.begin());
                i != this->values.end() - 1; ++i) {
            out << '\"' << *i << "\", ";
        }
    }
    if (this->getLength() > 0) {
        out << '\"' << this->values.back() << '\"';
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFString & lhs, const MFString & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFString & lhs, const MFString & rhs) throw ()
 *
 * @relates MFString
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @class MFTime
 *
 * @brief Encapsulates a MFTime.
 */

/**
 * @brief Construct from an array of time values.
 *
 * @param length    the number of time values in the passed array.
 * @param values    a pointer to an array of time values.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFTime::MFTime(const size_t length, const double * const values)
    throw (std::bad_alloc):
    values(length)
{
    if (values) {
        std::copy(values, values + length, this->values.begin());
    }
}

/**
 * @brief Destructor.
 */
MFTime::~MFTime() throw ()
{}

/**
 * @brief Get element.
 *
 * @param index
 */
const double & MFTime::getElement(const size_t index) const throw ()
{
    assert(index < this->getLength());
    return this->values[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFTime::setElement(const size_t index, const double value) throw ()
{
    assert(index < this->getLength());
    this->values[index] = value;
}

/**
 * @brief Get the length.
 *
 * @return the number of float values
 */
size_t MFTime::getLength() const throw ()
{
    return this->values.size();
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
void MFTime::setLength(const size_t length) throw (std::bad_alloc)
{
    this->values.resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFTime::addElement(const double value) throw (std::bad_alloc)
{
    this->values.push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a time value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFTime::insertElement(const size_t index, const double value)
    throw (std::bad_alloc)
{
    assert(index < this->values.size());
    this->values.insert(this->values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFTime::removeElement(const size_t index) throw ()
{
    assert(index < this->values.size());
    this->values.erase(this->values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFTime::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFTime(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFTime object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFTime::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFTime &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mftime.
 */
FieldValue::Type MFTime::type() const throw ()
{
    return FieldValue::mftime;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFTime::print(std::ostream & out) const
{
    out << '[';
    if (this->getLength() > 1) {
        for (std::vector<double>::const_iterator i(this->values.begin());
                i != this->values.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << this->values.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFTime & lhs, const MFTime & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFTime & lhs, const MFTime & rhs) throw ()
 *
 * @relates MFTime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @internal
 */
typedef array_vector<float, 2> Vec2fVec;

/**
 * @class MFVec2f
 *
 * @brief Encapsulates an MFVec2f.
 */

/**
 * @brief Construct from an array of vector values.
 *
 * @param length the number of vector values in the passed array
 * @param values    a pointer to an array of 2-element arrays comprising vector
 *                  values to initialize the MFVec2f.
 */
MFVec2f::MFVec2f(size_t length, SFVec2f::ConstArrayPointer values)
    throw (std::bad_alloc):
    values(new Vec2fVec(length))
{
    if (values) {
        for (size_t i(0); i < length; ++i) { this->setElement(i, values[i]); }
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfvec2f the object to copy
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFVec2f::MFVec2f(const MFVec2f & mfvec2f) throw (std::bad_alloc):
    values(new Vec2fVec(*static_cast<Vec2fVec *>(mfvec2f.values)))
{}

/**
 * @brief Destructor.
 */
MFVec2f::~MFVec2f() throw ()
{
    delete static_cast<Vec2fVec *>(this->values);
}

/**
 * @brief Assignment operator.
 *
 * @param mfvec2f the object to copy into this one
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFVec2f & MFVec2f::operator=(const MFVec2f & mfvec2f) throw (std::bad_alloc)
{
    MFVec2f temp(mfvec2f);
    static_cast<Vec2fVec *>(temp.values)
        ->swap(*static_cast<Vec2fVec *>(this->values));
    return *this;
}

/**
 * @brief Get element.
 *
 * @param index
 */
SFVec2f::ConstArrayReference MFVec2f::getElement(const size_t index) const
    throw ()
{
    assert(index < this->getLength());
    return (*static_cast<Vec2fVec *>(this->values))[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFVec2f::setElement(size_t index, SFVec2f::ConstArrayReference value)
    throw ()
{
    assert(index < this->getLength());
    std::copy(value, value + 2,
              (*static_cast<Vec2fVec *>(this->values))[index]);
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t MFVec2f::getLength() const throw ()
{
    return static_cast<Vec2fVec *>(this->values)->size();
}

/**
 * @brief Set the length.
 *
 * If the new length is smaller than the current length, the array is
 * truncated. If the new length is greater than the current length, the
 * new values are initialized to the default vector (0, 0).
 *
 * @param length the new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFVec2f::setLength(size_t length) throw (std::bad_alloc) {
    static_cast<Vec2fVec *>(this->values)->resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFVec2f::addElement(SFVec2f::ConstArrayReference value)
    throw (std::bad_alloc)
{
    static_cast<Vec2fVec *>(this->values)->push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a 2D vector.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFVec2f::insertElement(size_t index, SFVec2f::ConstArrayReference value)
    throw (std::bad_alloc)
{
    assert(index < static_cast<Vec2fVec *>(this->values)->size());
    Vec2fVec & values = *static_cast<Vec2fVec *>(this->values);
    values.insert(values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFVec2f::removeElement(size_t index) throw ()
{
    assert(index < static_cast<Vec2fVec *>(this->values)->size());
    Vec2fVec & values = *static_cast<Vec2fVec *>(this->values);
    values.erase(values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFVec2f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFVec2f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFVec2f object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFVec2f::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFVec2f &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfvec2f.
 */
FieldValue::Type MFVec2f::type() const throw ()
{
    return FieldValue::mfvec2f;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFVec2f::print(std::ostream & out) const
{
    const Vec2fVec & values = *static_cast<Vec2fVec *>(this->values);
    out << '[';
    if (this->getLength() > 1) {
        for (Vec2fVec::const_iterator i(values.begin()); i != values.end() - 1;
                ++i) {
            out << (*i)[0] << ' ' << (*i)[1] << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << values.back()[0] << ' ' << values.back()[1];
    }
    out << ']';
}

/**
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const MFVec2f & lhs, const MFVec2f & rhs) throw ()
{
    assert(lhs.values);
    assert(rhs.values);

    return *static_cast<Vec2fVec *>(lhs.values)
            == *static_cast<Vec2fVec *>(rhs.values);
}

/**
 * @fn bool operator!=(const MFVec2f & lhs, const MFVec2f & rhs) throw ()
 *
 * @relates MFVec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */


/**
 * @internal
 */
typedef array_vector<float, 3> Vec3fVec;

/**
 * @class MFVec3f
 *
 * @brief Encapsulates an MFVec3f.
 */

/**
 * @brief Construct from an array of vector values.
 *
 * @param length    the number of vector values in the passed array
 * @param values    a pointer to an array of 3-element arrays comprising color
 *                  values to initialize the MFColor.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFVec3f::MFVec3f(const size_t length, SFVec3f::ConstArrayPointer values)
    throw (std::bad_alloc):
    values(new Vec3fVec(length))
{
    if (values) {
        for (size_t i(0); i < length; ++i) { this->setElement(i, values[i]); }
    }
}

/**
 * @brief Copy constructor.
 *
 * @param mfvec3f the object to copy
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
MFVec3f::MFVec3f(const MFVec3f & mfvec3f) throw (std::bad_alloc):
    values(new Vec3fVec(*static_cast<Vec3fVec *>(mfvec3f.values)))
{}

/**
 * @brief Destructor.
 */
MFVec3f::~MFVec3f() throw ()
{
    delete static_cast<Vec3fVec *>(this->values);
}

/**
 * @brief Assignment operator.
 *
 * @param mfvec3f the object to copy into this one
 */
MFVec3f & MFVec3f::operator=(const MFVec3f & mfvec3f) throw (std::bad_alloc)
{
    MFVec3f temp(mfvec3f);
    static_cast<Vec3fVec *>(temp.values)
        ->swap(*static_cast<Vec3fVec *>(this->values));
    return *this;
}

/**
 * @brief Get element.
 *
 * @param index
 */
SFVec3f::ConstArrayReference MFVec3f::getElement(const size_t index) const
    throw ()
{
    assert(index < this->getLength());
    return (*static_cast<Vec3fVec *>(this->values))[index];
}

/**
 * @brief Set element.
 *
 * @param index
 * @param value
 */
void MFVec3f::setElement(const size_t index, SFVec3f::ConstArrayReference value)
    throw ()
{
    assert(index < this->getLength());
    std::copy(value, value + 3,
              (*static_cast<Vec3fVec *>(this->values))[index]);
}

/**
 * @brief Get the length.
 *
 * @return the number of vector values
 */
size_t MFVec3f::getLength() const throw ()
{
    return static_cast<Vec3fVec *>(this->values)->size();
}

/**
 * @brief Set the length.
 *
 * If the new length is smaller than the current length, the array is
 * truncated. If the new length is greater than the current length, the
 * new values are initialized to the default vector (0, 0, 0).
 *
 * @param length the new length
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFVec3f::setLength(size_t length) throw (std::bad_alloc) {
    static_cast<Vec3fVec *>(this->values)->resize(length);
}

/**
 * @brief Add an element to the end of the sequence.
 *
 * @param value the value to add.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void MFVec3f::addElement(SFVec3f::ConstArrayReference value)
    throw (std::bad_alloc)
{
    static_cast<Vec3fVec *>(this->values)->push_back(value);
}

/**
 * @brief Insert an element into the sequence.
 *
 * @param index the index where the new @p value should be inserted.
 * @param value a 3D vector.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p index is a valid index.
 */
void MFVec3f::insertElement(size_t index, SFVec3f::ConstArrayReference value)
    throw (std::bad_alloc)
{
    assert(index < static_cast<Vec2fVec *>(this->values)->size());
    Vec3fVec & values = *static_cast<Vec3fVec *>(this->values);
    values.insert(values.begin() + index, value);
}

/**
 * @brief Remove an element from the sequence.
 *
 * @param index the index of the value to remove.
 *
 * @pre @p index is a valid index.
 */
void MFVec3f::removeElement(size_t index) throw ()
{
    assert(index < static_cast<Vec2fVec *>(this->values)->size());
    Vec2fVec & values = *static_cast<Vec2fVec *>(this->values);
    values.erase(values.begin() + index);
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<FieldValue> MFVec3f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<FieldValue>(new MFVec3f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an MFVec3f object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
FieldValue & MFVec3f::assign(const FieldValue & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const MFVec3f &>(value));
}

/**
 * @brief Get the FieldValue::Type associated with this class.
 *
 * @return @c FieldValue::mfvec3f.
 */
FieldValue::Type MFVec3f::type() const throw ()
{
    return FieldValue::mfvec3f;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void MFVec3f::print(std::ostream & out) const
{
    const Vec3fVec & values = *static_cast<Vec3fVec *>(this->values);
    out << '[';
    if (this->getLength() > 1) {
        for (Vec3fVec::const_iterator i(values.begin()); i != values.end() - 1;
                ++i) {
            out << (*i)[0] << ' ' << (*i)[1] << ' ' << (*i)[2] << ", ";
        }
    }
    if (this->getLength() > 0) {
        out << values.back()[0] << ' ' << values.back()[1] << ' '
            << values.back()[2];
    }
    out << ']';
}

/**
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
{
    assert(lhs.values);
    assert(rhs.values);

    return *static_cast<Vec3fVec *>(lhs.values)
            == *static_cast<Vec3fVec *>(rhs.values);
}

/**
 * @fn bool operator!=(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
 *
 * @relates MFVec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */

} // namespace OpenVRML
