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
# include "private.h"
# include "field.h"
# include "node.h"

namespace OpenVRML {

/**
 * @defgroup fieldvalues Field Values
 */

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
 * @ingroup fieldvalues
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
 * @brief Destroy.
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
 * @ingroup fieldvalues
 *
 * @brief A boolean node field value.
 */

/**
 * @var SFBool::value
 *
 * @brief Boolean value.
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
 * @brief Destroy.
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
 * @ingroup fieldvalues
 *
 * @brief A color node field value.
 */

/**
 * @var SFColor::value
 *
 * @brief Color value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value.
 */
SFColor::SFColor(const color & value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
SFColor::~SFColor() throw ()
{}

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
    out << this->value;
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
 * @ingroup fieldvalues
 *
 * @brief A single precision floating point node field value.
 */

/**
 * @var SFFloat::value
 *
 * @brief Single precision floating point value.
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
 * @brief Destroy.
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
 * @ingroup fieldvalues
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
 * @brief Destroy.
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
 * @ingroup fieldvalues
 *
 * @brief A 32-bit integer node field value.
 */

/**
 * @var SFInt32::value
 *
 * @brief Signed 32-bit integer value.
 */

/**
 * @brief Constructor.
 *
 * @param value initial value.
 */
SFInt32::SFInt32(const int32 value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
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
 * @ingroup fieldvalues
 *
 * @brief A node field value to hold a single node reference.
 */

/**
 * @var SFNode::value
 *
 * @brief A Node reference.
 */

/**
 * @brief Constructor.
 *
 * @param node a NodePtr
 */
SFNode::SFNode(const NodePtr & node) throw ():
    value(node)
{}

/**
 * @brief Destroy.
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
    if (this->value) {
        out << *this->value;
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
 * @ingroup fieldvalues
 *
 * @brief A rotation node field value.
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
 * @var SFRotation::value
 *
 * @brief Rotation value.
 */

/**
 * @brief Construct.
 *
 * @param rot   initial value.
 */
SFRotation::SFRotation(const rotation & rot) throw ():
    value(rot)
{}

/**
 * @brief Destroy.
 */
SFRotation::~SFRotation() throw ()
{}

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
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFRotation::print(std::ostream & out) const
{
    out << this->value;
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
 * @ingroup fieldvalues
 *
 * @brief A string node field value.
 */

/**
 * @var SFString::value
 *
 * @brief String value.
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
 * @brief Destroy.
 */
SFString::~SFString() throw ()
{}

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
 * @ingroup fieldvalues
 *
 * @brief A double precision floating point node field value.
 */

/**
 * @var SFTime::value
 *
 * @brief Double precision floating point value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
SFTime::SFTime(double value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
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
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector node field value.
 */

/**
 * @var SFVec2f::value
 *
 * @brief 2-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
SFVec2f::SFVec2f(const vec2f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
SFVec2f::~SFVec2f() throw ()
{}

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
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFVec2f::print(std::ostream & out) const
{
    out << this->value;
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
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector node field value.
 */

/**
 * @var SFVec3f::value
 *
 * @brief 3-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
SFVec3f::SFVec3f(const vec3f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
SFVec3f::~SFVec3f() throw ()
{}

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
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void SFVec3f::print(std::ostream & out) const
{
    out << this->value;
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


/**
 * @class MFColor
 *
 * @ingroup fieldvalues
 *
 * @brief A color array node field value.
 */

/**
 * @var std::vector<color> MFColor::value
 *
 * @brief Color values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFColor with @p n copies of @p value.
 *
 * @param n     the number elements in the MFColor.
 * @param value used to initialize the MFColor.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
MFColor::MFColor(const std::vector<color>::size_type n, const color & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFColor::MFColor(InputIterator first, InputIterator last)
 *
 * @brief Create an MFColor with a copy of a range.
 *
 * Creates an MFColor with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFColor's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFColor::~MFColor() throw ()
{}

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
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<color>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFColor & lhs, const MFColor & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

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
 * @ingroup fieldvalues
 *
 * @brief A float array node field value.
 */

/**
 * @var std::vector<float> MFFloat::value
 *
 * @brief Single precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFFloat with @p n copies of @p value.
 *
 * @param n     the number of elements in the MFFloat.
 * @param value used to initialize the MFFloat.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
MFFloat::MFFloat(const std::vector<float>::size_type n, const float value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFFloat::MFFloat(InputIterator first, InputIterator last)
 *
 * @brief Create an MFFloat with a copy of a range.
 *
 * Creates an MFFloat with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFFloat's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFFloat::~MFFloat() throw ()
{}

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
    if (this->value.size() > 1) {
        for (std::vector<float>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
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
 * @ingroup fieldvalues
 *
 * @brief An integer array node field value.
 */

/**
 * @var std::vector<int32> MFInt32::value
 *
 * @brief Signed 32-bit integer values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFInt32 with @p n copies of @p value.
 *
 * @param n     the number of elements in the MFInt32.
 * @param value used to initialize the MFInt32.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
MFInt32::MFInt32(const std::vector<int32>::size_type n, const int32 value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFInt32::MFInt32(InputIterator first, InputIterator last)
 *
 * @brief Create an MFInt32 with a copy of a range.
 *
 * Creates an MFInt32 with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFInt32's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFInt32::~MFInt32() throw ()
{}

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
    if (this->value.size() > 1) {
        for (std::vector<int32>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
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
 * @ingroup fieldvalues
 *
 * @brief A node reference array node field value.
 */

/**
 * @var std::vector<NodePtr> MFNode::value
 *
 * @brief Node references.
 */

/**
 * @brief Construct.
 *
 * Creates an MFNode with @p n copies of @p value.
 *
 * @param n     the number elements in the MFNode.
 * @param value used to initialize the MFNode.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
MFNode::MFNode(const std::vector<NodePtr>::size_type n, const NodePtr & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFNode::MFNode(InputIterator first, InputIterator last)
 *
 * @brief Create an MFNode with a copy of a range.
 *
 * Creates an MFNode with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFNode's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFNode::~MFNode() throw ()
{}

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
    if (this->value.size() > 1) {
        for (std::vector<NodePtr>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            if (*i) { out << **i << ", "; }
        }
    }
    if (!this->value.empty() && this->value.back()) {
        out << *this->value.back();
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
 * @class MFRotation
 *
 * @ingroup fieldvalues
 *
 * @brief A rotation array node field value.
 */

/**
 * @var std::vector<rotation> MFRotation::value
 *
 * @brief Rotation values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFRotation with @p n copies of @p value.
 *
 * @param n     the number elements in the MFRotation.
 * @param value used to initialize the MFRotation.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
MFRotation::MFRotation(const std::vector<rotation>::size_type n,
                       const rotation & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFRotation::MFRotation(InputIterator first, InputIterator last)
 *
 * @brief Create an MFRotation with a copy of a range.
 *
 * Creates an MFRotation with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFRotation's @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
MFRotation::~MFRotation() throw ()
{}

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
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<rotation>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFRotation & lhs, const MFRotation & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

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
 * @ingroup fieldvalues
 *
 * @brief A string array node field value.
 */

/**
 * @var std::vector<std::string> MFString::value
 *
 * @brief String values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFString with @p n copies of @p value.
 *
 * @param n     the number elements in @a MFString::value.
 * @param value used to initialize @a MFString::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>MFString::value.size()</code> is @p n. Every element in
 *      @a MFString::value is a copy of @p value.
 */
MFString::MFString(const std::vector<std::string>::size_type n,
                   const std::string & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFString::MFString(InputIterator first, InputIterator last)
 *
 * @brief Create an MFString with a copy of a range.
 *
 * Creates an MFString with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFString's elements is destroyed, and memory allocated for them
 * (if any) is deallocated.
 */
MFString::~MFString() throw ()
{}

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
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<std::string>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << '\"' << *i << "\", ";
        }
    }
    if (!this->value.empty()) {
        out << '\"' << this->value.back() << '\"';
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
 * @ingroup fieldvalues
 *
 * @brief A double array node field value.
 */

/**
 * @var std::vector<double> MFTime::value
 *
 * @brief Double precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFTime with @p n copies of @p value.
 *
 * @param n     the number of elements in @a MFTime::value.
 * @param value used to initialize @a MFTime::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>MFTime::value.size()</code> is @p n. Every element in
 *      @a MFTime::value is a copy of @p value.
 */
MFTime::MFTime(const std::vector<double>::size_type n, const double value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFTime::MFTime(InputIterator first, InputIterator last)
 *
 * @brief Create an MFTime with a copy of a range.
 *
 * Creates an MFTime with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFTime's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFTime::~MFTime() throw ()
{}

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
    if (this->value.size() > 1) {
        for (std::vector<double>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
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
 * @class MFVec2f
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector array node field value.
 */

/**
 * @var std::vector<vec2f> MFVec2f::value
 *
 * @brief 2-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFVec2f with @p n copies of @p value.
 *
 * @param n     the number elements in @a MFVec2f::value.
 * @param value used to initialize @a MFVec2f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>MFVec2f::value.size()</code> is @p n. Every element in
 *      @a MFVec2f::value is a copy of @p value.
 */
MFVec2f::MFVec2f(const std::vector<vec2f>::size_type n, const vec2f & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFVec2f::MFVec2f(InputIterator first, InputIterator last)
 *
 * @brief Create an MFVec2f with a copy of a range.
 *
 * Creates an MFVec2f with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFVec2f's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFVec2f::~MFVec2f() throw ()
{}

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
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<vec2f>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFVec2f & lhs, const MFVec2f & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

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
 * @class MFVec3f
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector array node field value.
 */

/**
 * @var std::vector<vec3f> MFVec3f::value
 *
 * @brief 3-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an MFVec3f with @p n copies of @p value.
 *
 * @param n     the number elements in @a MFVec3f::value.
 * @param value used to initialize @a MFVec3f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>MFVec3f::value.size()</code> is @p n. Every element in
 *      @a MFVec3f::value is a copy of @p value.
 */
MFVec3f::MFVec3f(const std::vector<vec3f>::size_type n, const vec3f & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @fn template <typename InputIterator> MFVec3f::MFVec3f(InputIterator first, InputIterator last)
 *
 * @brief Create an MFVec3f with a copy of a range.
 *
 * Creates an MFVec3f with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the MFVec3f's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
MFVec3f::~MFVec3f() throw ()
{}

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
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<vec3f>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @fn bool operator==(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */

/**
 * @fn bool operator!=(const MFVec3f & lhs, const MFVec3f & rhs) throw ()
 *
 * @relates MFVec3f
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

namespace std {

/**
 * @fn template <> void swap(OpenVRML::MFColor & a, OpenVRML::MFColor & b)
 *
 * @relates OpenVRML::MFColor
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFFloat & a, OpenVRML::MFFloat & b)
 *
 * @relates OpenVRML::MFFloat
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFInt32 & a, OpenVRML::MFInt32 & b)
 *
 * @relates OpenVRML::MFInt32
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFNode & a, OpenVRML::MFNode & b)
 *
 * @relates OpenVRML::MFNode
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFRotation & a, OpenVRML::MFRotation & b)
 *
 * @relates OpenVRML::MFRotation
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFString & a, OpenVRML::MFString & b)
 *
 * @relates OpenVRML::MFString
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFTime & a, OpenVRML::MFTime & b)
 *
 * @relates OpenVRML::MFTime
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFVec2f & a, OpenVRML::MFVec2f & b)
 *
 * @relates OpenVRML::MFVec2f
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void swap(OpenVRML::MFVec3f & a, OpenVRML::MFVec3f & b)
 *
 * @relates OpenVRML::MFVec3f
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

}
