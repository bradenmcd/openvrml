//
// OpenVRML
//
// Copyright (C) 2003  Braden McDaniel
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

# include <assert.h>
# include <numeric>
# include "private.h"
# include "basetypes.h"
# include "quaternion.h"

namespace OpenVRML {

/**
 * @defgroup basetypes Base Types
 */

/**
 * @class color
 *
 * @ingroup basetypes
 *
 * @brief A color.
 *
 * VRML colors are represented as three single precision floating point components--red, blue,
 * and green--ranging from 0.0 to 1.0.
 */

/**
 * @fn color::color() throw ()
 *
 * @brief Construct.
 */

/**
 * @fn color::color(float (&rgb)[3]) throw ()
 *
 * @brief Construct from a 3-element float array.
 *
 * @param rgb   an array comprising red, green, and blue components.
 */

/**
 * @fn color::color(float r, float g, float b) throw ()
 *
 * @brief Construct from red, green, and blue components.
 *
 * @param r red component.
 * @param g green component.
 * @param b blue component.
 */

/**
 * @fn const float & color::operator[](const size_t index) const throw ()
 *
 * @brief Index-based component access.
 *
 * @param index 0 is the red component, 1 is the green component, and 2 is the
 *              blue component.
 *
 * @return the component corresponding to @p index.
 */

/**
 * @fn float color::r() const throw ()
 *
 * @brief Get the red component.
 *
 * @return the red component.
 */

/**
 * @fn float color::g() const throw ()
 *
 * @brief Get the green component.
 *
 * @return the green component.
 */

/**
 * @fn float color::b() const throw ()
 *
 * @brief Get the blue component.
 *
 * @return the blue component.
 */

/**
 * @fn void color::r(float value) throw ()
 *
 * @brief Set the red component.
 *
 * @param value the new component value.
 */

/**
 * @fn void color::g(float value) throw ()
 *
 * @brief Set the green component.
 *
 * @param value the new component value.
 */

/**
 * @fn void color::b(float value) throw ()
 *
 * @brief Set the blue component.
 *
 * @param value the new component value.
 */

// Conversion functions between RGB each in [0,1] and HSV with  
// h in [0,360), s,v in [0,1]. From Foley, van Dam p615-616.

/**
 * @brief Get the color as hue, saturation, and value.
 *
 * @retval result   the hue, saturation, and value.
 */
void color::hsv(float (&result)[3]) const throw ()
{
    const float maxrgb = *std::max_element(this->rgb, this->rgb + 3);
    const float minrgb = *std::min_element(this->rgb, this->rgb + 3);
    
    result[0] = 0.0;
    result[1] = (maxrgb > 0.0) ? ((maxrgb - minrgb) / maxrgb) : 0.0;
    result[2] = maxrgb;
    
    if (result[1] != 0.0) {
        const float rc = (maxrgb - rgb[0]) / (maxrgb - minrgb);
        const float gc = (maxrgb - rgb[1]) / (maxrgb - minrgb);
        const float bc = (maxrgb - rgb[2]) / (maxrgb - minrgb);
        
        if (this->rgb[0] == maxrgb) {
            result[0] = bc - gc;
        } else if (this->rgb[1] == maxrgb) {
            result[0] = 2 + rc - bc;
        } else {
            result[0] = 4 + gc - rc;
        }
        
        result[0] *= 60.0;
        if (result[0] < 0.0) { result[0] += 360.0; }
    }
}

/**
 * @brief Set the color from hue, saturation, and value.
 *
 * @param h hue.
 * @param s saturation.
 * @param v value.
 */
void color::hsv(float h, const float s, const float v) throw ()
{
    if (s == 0.0) {
        this->rgb[0] = this->rgb[1] = this->rgb[2] = v;
    } else {
        if (h >= 360.0) { h -= 360.0; }
        h /= 60.0;
        const double i = floor(h);
        const double f = h - i;
        const float p = v * (1.0 - s);
        const float q = v * (1.0 - s * f);
        const float t = v * (1.0 - s * (1.0 - f));
        switch (int(i)) {
	default:
	case 0: this->rgb[0] = v; this->rgb[1] = t; this->rgb[2] = p; break;
	case 1: this->rgb[0] = q; this->rgb[1] = v; this->rgb[2] = p; break;
	case 2: this->rgb[0] = p; this->rgb[1] = v; this->rgb[2] = t; break;
	case 3: this->rgb[0] = p; this->rgb[1] = q; this->rgb[2] = v; break;
	case 4: this->rgb[0] = t; this->rgb[1] = p; this->rgb[2] = v; break;
	case 5: this->rgb[0] = v; this->rgb[1] = p; this->rgb[2] = q; break;
	}
    }
}

/**
 * @relates color
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const color & lhs, const color & rhs) throw ()
{
    using OpenVRML_::fpequal;
    return fpequal(lhs[0], rhs[0])
        && fpequal(lhs[1], rhs[1])
        && fpequal(lhs[2], rhs[2]);
}

/**
 * @fn bool operator!=(const color & lhs, const color & rhs) throw ()
 *
 * @relates color
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *      otherwise.
 */

/**
 * @relates color
 *
 * @brief Stream output.
 *
 * @param out   output stream.
 * @param c     a color.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const color & c)
{
    return out << c.r() << ' ' << c.g() << ' ' << c.b();
}


/**
 * @class vec2f
 *
 * @ingroup basetypes
 *
 * @brief Two-component single precision vector.
 */

/**
 * @fn vec2f::vec2f() throw ()
 *
 * @brief Construct.
 */

/**
 * @fn vec2f::vec2f(const float (&vec)[2]) throw ()
 *
 * @brief Construct from an array.
 *
 * @param vec   an array comprising the vector components.
 */

/**
 * @fn vec2f::vec2f(float x, float y) throw ()
 *
 * @brief Construct from @p x and @p y components.
 *
 * @param x x component.
 * @param y y component.
 */

/**
 * @fn vec2f & vec2f::operator*=(float scalar) throw ()
 *
 * @brief Multiply by a scalar.
 *
 * @param scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec2f operator*(const vec2f & lhs, float rhs) throw ()
 *
 * @relates vec2f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param lhs   left-hand operand (the vector).
 * @param rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */

/**
 * @fn vec2f & vec2f::operator/=(float scalar) throw ()
 *
 * @brief Divide by a scalar.
 *
 * @param scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec2f operator/(const vec2f & lhs, float rhs) throw ()
 *
 * @relates vec2f
 *
 * @brief Divide a vector by a scalar.
 *
 * @param lhs   left-hand operand (the vector).
 * @param rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */

/**
 * @fn vec2f & vec2f::operator+=(const vec2f & vec) throw ()
 *
 * @brief Add a vector.
 *
 * @param vec   the vector to add.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec2f operator+(const vec2f & lhs, const vec2f & rhs) throw ()
 *
 * @relates vec2f
 *
 * @brief Add two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @fn vec2f & vec2f::operator-=(const vec2f & vec) throw ()
 *
 * @brief Subtract a vector.
 *
 * @param vec   the vector to subtract.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec2f operator-(const vec2f & lhs, const vec2f & rhs) throw ()
 *
 * @relates vec2f
 *
 * @brief Subtract two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @fn const vec2f vec2f::operator-() const throw ()
 *
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */

/**
 * @fn const float & vec2f::operator[](const size_t index) const throw ()
 *
 * @brief Index-based component access.
 *
 * @param index 0 corresponds to the x component; 1 corresponds to the y
 *              component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 2.
 */

/**
 * @fn float & vec2f::operator[](const size_t index) throw ()
 *
 * @brief Index-based component access.
 *
 * @param index 0 corresponds to the x component; 1 corresponds to the y
 *              component.
 *
 * @return a reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 2.
 */

/**
 * @fn float vec2f::x() const throw ()
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn float vec2f::y() const throw ()
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn void vec2f::x(float value) const throw ()
 *
 * @brief Set the x component.
 *
 * @param value new x component value.
 */

/**
 * @fn void vec2f::y(float value) const throw ()
 *
 * @brief Set the y component.
 *
 * @param value new y component value.
 */

/**
 * @fn float vec2f::dot(const vec2f & vec) const throw ()
 *
 * @brief Dot product.
 *
 * @param vec
 *
 * @return the dot product of the vector and @p vec.
 */

/**
 * @fn float vec2f::length() const throw ()
 *
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */

/**
 * @brief Normalize.
 *
 * @return a vec2f that is the vector normalized.
 */
const vec2f vec2f::normalize() const throw ()
{
    using OpenVRML_::fpzero;
    const float len = this->length();
    vec2f result(*this);
    if (fpzero(len)) { result /= len; }
    return result;
}

/**
 * @relates vec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const vec2f & lhs, const vec2f & rhs) throw ()
{
    using OpenVRML_::fpequal;
    return fpequal(lhs[0], rhs[0])
        && fpequal(lhs[1], rhs[1]);
}

/**
 * @fn bool operator!=(const vec2f & lhs, const vec2f & rhs) throw ()
 *
 * @relates vec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *      otherwise.
 */

/**
 * @relates vec2f
 *
 * @brief Stream output.
 *
 * @param out   output stream.
 * @param v     a 2-component vector.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const vec2f & v)
{
    return out << v.x() << ' ' << v.y();
}


/**
 * @class vec3f
 *
 * @ingroup basetypes
 *
 * @brief Three-component single precision vector.
 */

/**
 * @fn vec3f::vec3f() throw ()
 *
 * @brief Construct.
 */

/**
 * @fn vec3f::vec3f(const float (&vec)[3]) throw ()
 *
 * @brief Construct from an array.
 *
 * @param vec   an array comprising the vector components.
 */

/**
 * @fn vec3f::vec3f(float x, float y, float z) throw ()
 *
 * @brief Construct from @p x, @p y, and @p z components.
 *
 * @param x x component.
 * @param y y component.
 * @param z z component.
 */

/**
 * @fn vec3f & vec3f::operator*=(const vec3f & vec) throw ()
 *
 * @brief Cross multiply.
 *
 * @param vec   vector by which to multiply.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec3f operator*(const vec3f & lhs, const vec3f & rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Cross multiply two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @fn vec3f & vec3f::operator*=(float scalar) throw ()
 *
 * @brief Multiply by a scalar.
 *
 * @param scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec3f operator*(const vec2f & lhs, float rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Multiply a vector by a scalar.
 *
 * @param lhs   left-hand operand (the vector).
 * @param rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */

/**
 * @fn vec3f & vec3f::operator/=(float scalar) throw ()
 *
 * @brief Divide by a scalar.
 *
 * @param scalar    factor by which to multiply.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec3f operator/(const vec3f & lhs, float rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Divide a vector by a scalar.
 *
 * @param lhs   left-hand operand (the vector).
 * @param rhs   right-hand operand (the scalar).
 *
 * @return the result vector.
 */

/**
 * @fn vec3f & vec3f::operator+=(const vec3f & vec) throw ()
 *
 * @brief Add a vector.
 *
 * @param vec   the vector to add.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec3f operator+(const vec3f & lhs, const vec3f & rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Add two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @fn vec3f & vec3f::operator-=(const vec3f & vec) throw ()
 *
 * @brief Subtract a vector.
 *
 * @param vec   the vector to subtract.
 *
 * @return a reference to the object.
 */

/**
 * @fn const vec3f operator-(const vec3f & lhs, const vec3f & rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Subtract two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @fn const vec3f vec3f::operator-() const throw ()
 *
 * @brief Negate.
 *
 * @return the additive inverse of the vector.
 */

/**
 * @fn const float & vec3f::operator[](const size_t index) const throw ()
 *
 * @brief Index-based component access.
 *
 * @param index 0 corresponds to the x component; 1 corresponds to the y
 *              component; 2 corresponds to the z component.
 *
 * @return a const reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 3.
 */

/**
 * @fn float & vec3f::operator[](const size_t index) throw ()
 *
 * @brief Index-based component access.
 *
 * @param index 0 corresponds to the x component; 1 corresponds to the y
 *              component; 2 corresponds to the z component.
 *
 * @return a reference to the component corresponding to @p index.
 *
 * @pre @p index is less than 3.
 */

/**
 * @fn float vec3f::x() const throw ()
 *
 * @brief Get the x component.
 *
 * @return the x component.
 */

/**
 * @fn float vec3f::y() const throw ()
 *
 * @brief Get the y component.
 *
 * @return the y component.
 */

/**
 * @fn float vec3f::z() const throw ()
 *
 * @brief Get the z component.
 *
 * @return the z component.
 */

/**
 * @fn void vec3f::x(float value) const throw ()
 *
 * @brief Set the x component.
 *
 * @param value new x component value.
 */

/**
 * @fn void vec3f::y(float value) const throw ()
 *
 * @brief Set the y component.
 *
 * @param value new y component value.
 */

/**
 * @fn void vec3f::z(float value) const throw ()
 *
 * @brief Set the z component.
 *
 * @param value new z component value.
 */

/**
 * @fn float vec3f::dot(const vec3f & vec) const throw ()
 *
 * @brief Dot product.
 *
 * @param vec
 *
 * @return the dot product of the vector and @p vec.
 */

/**
 * @fn float vec3f::length() const throw ()
 *
 * @brief Geometric length.
 *
 * @return the length of the vector.
 */

/**
 * @brief Normalize.
 *
 * @return a vec3f that is the vector normalized.
 */
const vec3f vec3f::normalize() const throw ()
{
    using OpenVRML_::fpzero;
    const float len = this->length();
    vec3f result(*this);
    if (!fpzero(len)) { result /= len; }
    return result;
}

/**
 * @relates vec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false otherwise.
 */
bool operator==(const vec3f & lhs, const vec3f & rhs) throw ()
{
    using OpenVRML_::fpequal;
    return fpequal(lhs[0], rhs[0])
        && fpequal(lhs[1], rhs[1])
        && fpequal(lhs[2], rhs[2]);
}

/**
 * @fn bool operator!=(const vec3f & lhs, const vec3f & rhs) throw ()
 *
 * @relates vec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *      otherwise.
 */

/**
 * @relates vec3f
 *
 * @brief Stream output.
 *
 * @param out   output stream.
 * @param v     a 3-component vector.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const vec3f & v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}


/**
 * @class rotation
 *
 * @ingroup basetypes
 *
 * @brief A rotation.
 *
 * VRML rotations are represented with four single precision floating point
 * components. The first three are an axis of rotation, and the last is
 * rotation in radians.
 */

/**
 * @fn rotation::rotation() throw ()
 *
 * @brief Construct.
 */

/**
 * @brief Construct from an array.
 *
 * @param rot   an array comprising the rotation components.
 *
 * @pre The vector represented by the first three components of @p rot is
 *      normalized.
 */
rotation::rotation(const float (&rot)[4]) throw ()
{
    using OpenVRML_::fpequal;
    assert(fpequal(vec3f(rot[0], rot[1], rot[2]).length(), 1.0));
    this->rot[0] = rot[0];
    this->rot[1] = rot[1];
    this->rot[2] = rot[2];
    this->rot[3] = rot[3];
}

/**
 * @brief Construct from @p x, @p y, @p z, and @p angle components.
 *
 * @param x     x-component of the rotation axis.
 * @param y     y-component of the rotation axis.
 * @param z     z-component of the rotation axis.
 * @param angle rotation angle.
 *
 * @pre The vector represented by @p x, @p y, and @p z is normalized.
 */
rotation::rotation(const float x,
                   const float y,
                   const float z,
                   const float angle)
    throw ()
{
    using OpenVRML_::fpequal;
    assert(fpequal(vec3f(x, y, z).length(), 1.0));
    this->rot[0] = x;
    this->rot[1] = y;
    this->rot[2] = z;
    this->rot[3] = angle;
}

/**
 * @fn rotation::rotation(const vec3f & axis, float angle) throw ()
 *
 * @brief Construct from an axis vector and an angle.
 *
 * @param axis  rotation axis.
 * @param angle rotation angle.
 *
 * @pre @p axis is a normalized vector.
 */
rotation::rotation(const vec3f & axis, const float angle) throw ()
{
    using OpenVRML_::fpequal;
    assert(fpequal(axis.length(), 1.0));
    this->axis(axis);
    this->rot[3] = angle;
}

/**
 * @brief Construct a rotation between two vectors.
 *
 * Construct a rotation equal to the rotation between two different vectors.
 *
 * @param from_vec  the starting vector.
 * @param to_vec    the ending vector.
 */
rotation::rotation(const vec3f & from_vec, const vec3f & to_vec)
    throw ()
{
    this->axis(from_vec * to_vec);
    this->rot[3] =
            acos(from_vec.dot(to_vec) / (from_vec.length() * to_vec.length()));
}

/**
 * @brief Construct from a quaternion.
 *
 * @param quat  a quaternion.
 */
rotation::rotation(const Quaternion & quat) throw ()
{
    using OpenVRML_::fpzero;

    const float val = acos(quat.getW());
    if (fpzero(val)) {
        this->rot[0] = 0.0;
        this->rot[1] = 0.0;
        this->rot[2] = 1.0;
        this->rot[3] = 0.0;
    } else {
        const float sin_val = sin(val);
        const vec3f axis(quat.getX() / sin_val,
                         quat.getY() / sin_val,
                         quat.getZ() / sin_val);
        this->axis(axis.normalize());
        this->rot[3] = 2 * val;
    }
}

/**
 * @brief Multiply rotations.
 *
 * @param rot   the rotation by which to multiply.
 *
 * @return a reference to the object.
 */
rotation & rotation::operator*=(const rotation & rot) throw ()
{
    return *this = rotation(Quaternion(*this).multiply(Quaternion(rot)));
}

/**
 * @fn const rotation operator*(const rotation & lhs, const rotation & rhs) throw ()
 *
 * @relates rotation
 *
 * @brief Multiply rotations.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the product of @p lhs and @p rhs.
 */

/**
 * @fn const float & rotation::operator[](const size_t index) throw ()
 *
 * @brief Index-based element access.
 *
 * @param index 0 is the x axis component, 1 is the y axis component, 2 is the
 *              z axis component, and 3 is the angle.
 */

/**
 * @fn float rotation::x() const throw ()
 *
 * @brief Get the x axis component.
 *
 * @return the x axis component.
 */

/**
 * @fn float rotation::y() const throw ()
 *
 * @brief Get the y axis component.
 *
 * @return the y axis component.
 */

/**
 * @fn float rotation::z() const throw ()
 *
 * @brief Get the z axis component.
 *
 * @return the z axis component.
 */

/**
 * @fn const vec3f rotation::axis() const throw ()
 *
 * @brief Get the axis of rotation.
 *
 * @return the axis of rotation.
 */

/**
 * @fn float rotation::angle() const throw ()
 *
 * @brief Get the rotation angle.
 *
 * @return the rotation angle.
 */

namespace {

    void normalize_axis_(float axis[3]) throw ()
    {
        using OpenVRML_::length;
        using OpenVRML_::fpequal;
        using OpenVRML_::normalize;
    
        const float axis_length = length(axis);
        if (fpequal(axis_length, 0.0)) {
            axis[2] = 1.0;
        } else {
            normalize(axis);
        }
    }
}

/**
 * @brief Set the x axis component.
 *
 * @param value new x axis component value.
 */
void rotation::x(const float value) throw ()
{
    this->rot[0] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the y axis component.
 *
 * @param value new y axis component value.
 */
void rotation::y(const float value) throw ()
{
    this->rot[1] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the z axis component.
 *
 * @param value new z axis component value.
 */
void rotation::z(const float value) throw ()
{
    this->rot[2] = value;
    normalize_axis_(this->rot);
}

/**
 * @brief Set the axis of rotation.
 *
 * @param axis  new axis of rotation.
 */
void rotation::axis(const vec3f & axis) throw ()
{
    using OpenVRML_::fpequal;
    assert(fpequal(axis.length(), 1.0));
    this->rot[0] = axis.x();
    this->rot[1] = axis.y();
    this->rot[2] = axis.z();
}

/**
 * @fn void rotation::angle(float value) throw ()
 *
 * @brief Set the rotation angle.
 *
 * @param value new rotation angle value.
 */

const rotation rotation::slerp(const rotation & dest_rot, const float t) const
    throw ()
{
    using OpenVRML_::fptolerance;
    
    Quaternion from_quat(*this), to_quat(dest_rot);
    
    //
    // Calculate cosine.
    //
    double cosom = std::inner_product(from_quat.get(), from_quat.get() + 4,
                                      to_quat.get(), 0.0);
    
    //
    // Adjust signs (if necessary).
    //
    Quaternion to1;
    if (cosom < 0.0) {
        cosom = -cosom;
        to1 = to_quat.multiply(-1);
    } else {
        to1 = to_quat;
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
    const Quaternion result_quat = from_quat.multiply(scale0)
                                    .add(to1.multiply(scale1));
    return rotation(result_quat);
}

/**
 * @relates rotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs is equal to @p rhs; @c false otherwise.
 */
bool operator==(const rotation & lhs, const rotation & rhs) throw ()
{
    using OpenVRML_::fpequal;
    return fpequal(lhs.x(), rhs.x())
        && fpequal(lhs.y(), rhs.y())
        && fpequal(lhs.z(), rhs.z())
        && fpequal(lhs.angle(), rhs.angle());
}

/**
 * @fn bool operator!=(const rotation & lhs, const rotation & rhs) throw ()
 *
 * @relates rotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs is not equal to @p rhs; @c false otherwise.
 */

/**
 * @relates rotation
 *
 * @brief Stream output.
 *
 * @param out   output stream.
 * @param r     a rotation.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const rotation & r)
{
    return out << r.x() << ' ' << r.y() << ' ' << r.z() << ' ' << r.angle();
}

}
