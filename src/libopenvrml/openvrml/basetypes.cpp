// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2001  S. K. Bose
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

# include <numeric>
# include "private.h"
# include "basetypes.h"

namespace openvrml {

/**
 * @defgroup basetypes Base Types
 */

/**
 * @typedef int32
 *
 * @ingroup basetypes
 *
 * @brief 32-bit signed integer.
 */

/**
 * @class color
 *
 * @ingroup basetypes
 *
 * @brief A color.
 *
 * VRML colors are represented as three single precision floating point
 * components--red, green, and blue--ranging from 0.0 to 1.0.
 */

/**
 * @internal
 *
 * @var float color::rgb[3]
 *
 * @brief RGB triplet.
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
    result[1] = (maxrgb > 0.0) ? ((maxrgb - minrgb) / maxrgb) : 0.0f;
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
        const float p = float(v * (1.0 - s));
        const float q = float(v * (1.0 - s * f));
        const float t = float(v * (1.0 - s * (1.0 - f)));
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
 * @relates openvrml::color
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const color & lhs, const color & rhs) throw ()
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 3, &rhs[0], fequal<float>());
}

/**
 * @fn bool operator!=(const color & lhs, const color & rhs) throw ()
 *
 * @relates openvrml::color
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
 * @internal
 *
 * @var float vec2f::vec[2]
 *
 * @brief Vector components.
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
 * @relates openvrml::vec2f
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
 * @relates openvrml::vec2f
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
 * @relates openvrml::vec2f
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
 * @relates openvrml::vec2f
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
 * @fn void vec2f::x(float value) throw ()
 *
 * @brief Set the x component.
 *
 * @param value new x component value.
 */

/**
 * @fn void vec2f::y(float value) throw ()
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
    using openvrml_::fequal;
    const float len = this->length();
    vec2f result(*this);
    if (fequal<float>()(len, 0.0)) { result /= len; }
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
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const vec2f & lhs, const vec2f & rhs) throw ()
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 2, &rhs[0], fequal<float>());
}

/**
 * @fn bool operator!=(const vec2f & lhs, const vec2f & rhs) throw ()
 *
 * @relates openvrml::vec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs dot not have the same value; @c false
 *         otherwise.
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
 * @internal
 *
 * @var float vec3f::vec[3]
 *
 * @brief Vector components.
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
 * @relates openvrml::vec3f
 *
 * @brief Cross multiply two vectors.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result vector.
 */

/**
 * @brief Multiply by a matrix.
 *
 * @param mat   matrix by which to multiply.
 *
 * @return a reference to the object.
 */
vec3f & vec3f::operator*=(const mat4f & mat) throw ()
{
    const float x = this->vec[0] * mat[0][0] + this->vec[1] * mat[1][0]
                    + this->vec[2] * mat[2][0] + mat[3][0];
    const float y = this->vec[0] * mat[0][1] + this->vec[1] * mat[1][1]
                    + this->vec[2] * mat[2][1] + mat[3][1];
    const float z = this->vec[0] * mat[0][2] + this->vec[1] * mat[1][2]
                    + this->vec[2] * mat[2][2] + mat[3][2];
    const float w = this->vec[0] * mat[0][3] + this->vec[1] * mat[1][3]
                    + this->vec[2] * mat[2][3] + mat[3][3];
    this->vec[0] = x / w;
    this->vec[1] = y / w;
    this->vec[2] = z / w;
    return *this;
}

/**
 * @fn const vec3f operator*(const vec3f & vec, const mat4f & mat) throw ()
 *
 * @relates openvrml::vec3f
 *
 * @brief Multiply a vector by a matrix.
 *
 * @param mat   a matrix.
 * @param vec   a 3-component vector.
 *
 * @return the result vector.
 */

/**
 * @relates vec3f
 *
 * @brief Multiply a matrix by a vector.
 *
 * @param mat   a matrix.
 * @param vec   a 3-component vector.
 *
 * @return the result vector.
 */
const vec3f operator*(const mat4f & mat, const vec3f & vec) throw ()
{
    const float x = mat[0][0] * vec[0] + mat[0][1] * vec[1]
                    + mat[0][2] * vec[2] + mat[0][3];
    const float y = mat[1][0] * vec[0] + mat[1][1] * vec[1]
                    + mat[1][2] * vec[2] + mat[1][3];
    const float z = mat[2][0] * vec[0] + mat[2][1] * vec[1]
                    + mat[2][2] * vec[2] + mat[2][3];
    const float w = mat[3][0] * vec[0] + mat[3][1] * vec[1]
                    + mat[3][2] * vec[2] + mat[3][3];
    return vec3f(x / w, y / w, z / w);
}

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
 * @relates openvrml::vec3f
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
 * @relates openvrml::vec3f
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
 * @relates openvrml::vec3f
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
 * @relates openvrml::vec3f
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
 * @fn void vec3f::x(float value) throw ()
 *
 * @brief Set the x component.
 *
 * @param value new x component value.
 */

/**
 * @fn void vec3f::y(float value) throw ()
 *
 * @brief Set the y component.
 *
 * @param value new y component value.
 */

/**
 * @fn void vec3f::z(float value) throw ()
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
    using openvrml_::fequal;
    const float len = this->length();
    vec3f result(*this);
    if (!fequal<float>()(len, 0.0f)) { result /= len; }
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
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 3, &rhs[0], fequal<float>());
}

/**
 * @fn bool operator!=(const vec3f & lhs, const vec3f & rhs) throw ()
 *
 * @relates openvrml::vec3f
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
 * @internal
 *
 * @var float rotation::rot[4]
 *
 * @brief Rotation components.
 *
 * The first three components define an axis through the origin. The fourth
 * component is a rotation around the axis.
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
# ifndef NDEBUG
    using openvrml_::fequal;
    fequal<float> feq;
# endif
    assert(feq(vec3f(rot[0], rot[1], rot[2]).length(), 1.0f));
    std::copy(rot, rot + 4, this->rot);
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
# ifndef NDEBUG
    using openvrml_::fequal;
    fequal<float> feq;
# endif
    assert(feq(vec3f(x, y, z).length(), 1.0f));
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
# ifndef NDEBUG
    using openvrml_::fequal;
    fequal<float> feq;
# endif
    assert(feq(axis.length(), 1.0f));
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
    this->rot[3] = float(acos(from_vec.dot(to_vec)
                              / (from_vec.length() * to_vec.length())));
}

/**
 * @brief Construct from a quaternion.
 *
 * @param quat  a quaternion.
 */
rotation::rotation(const quatf & quat) throw ()
{
    using openvrml_::fequal;

    const float val = float(acos(quat.w()));
    if (fequal<float>()(val, 0.0f)) {
        this->rot[0] = 0.0;
        this->rot[1] = 0.0;
        this->rot[2] = 1.0;
        this->rot[3] = 0.0;
    } else {
        const float sin_val = float(sin(val));
        const vec3f axis(quat.x() / sin_val,
                         quat.y() / sin_val,
                         quat.z() / sin_val);
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
    return *this = rotation(quatf(*this) * quatf(rot));
}

/**
 * @fn const rotation operator*(const rotation & lhs, const rotation & rhs) throw ()
 *
 * @relates openvrml::rotation
 *
 * @brief Multiply rotations.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the product of @p lhs and @p rhs.
 */

/**
 * @fn const float & rotation::operator[](const size_t index) const throw ()
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

    inline double length_(const float vec[3])
    {
        return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    }

    inline void normalize_(float vec[3])
    {
        using openvrml_::fequal;
        const float len = float(length_(vec));
        if (!fequal<float>()(len, 0.0f)) {
            vec[0] /= len;
            vec[1] /= len;
            vec[2] /= len;
        }
    }

    void normalize_axis_(float axis[3]) throw ()
    {
        using openvrml_::fequal;
        const float axis_length = float(length_(axis));
        if (fequal<float>()(axis_length, 0.0)) {
            axis[2] = 1.0;
        } else {
            normalize_(axis);
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
# ifndef NDEBUG
    using openvrml_::fequal;
    fequal<float> feq;
# endif
    assert(feq(axis.length(), 1.0f));
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

/**
 * @fn const rotation rotation::inverse() const throw ()
 *
 * @brief Inverse rotation.
 *
 * @return the inverse rotation.
 */

/**
 * @brief Spherical Linear intERPolation.
 *
 * @param dest_rot  destination rotation.
 * @param t         fraction representing the desired position between the
 *                  rotation and @p dest_rot.
 *
 * @return the rotation corresponding to @p t between the rotation and
 *         @p dest_rot.
 */
const rotation rotation::slerp(const rotation & dest_rot, const float t) const
    throw ()
{
    using openvrml_::fequal;

    quatf from_quat(*this), to_quat(dest_rot);

    //
    // Calculate cosine.
    //
    double cosom = std::inner_product(&from_quat[0], &from_quat[0] + 4,
                                      &to_quat[0], 0.0);

    //
    // Adjust signs (if necessary).
    //
    quatf to1;
    if (cosom < 0.0) {
        cosom = -cosom;
        to1 = to_quat * -1;
    } else {
        to1 = to_quat;
    }

    //
    // Calculate coefficients.
    //
    double scale0, scale1;
    if (!fequal<double>()(cosom, 0.0)) {
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
    const quatf result_quat =
        (from_quat * float(scale0)) + (to1 * float(scale1));
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
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 4, &rhs[0], fequal<float>());
}

/**
 * @fn bool operator!=(const rotation & lhs, const rotation & rhs) throw ()
 *
 * @relates openvrml::rotation
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


/**
 * @class mat4f
 *
 * @ingroup basetypes
 *
 * @brief A class for all matrix operations.
 *
 * Matrices are stored in row-major order.
 */

/**
 * @internal
 *
 * @var float mat4f::mat[4][4]
 *
 * @brief 4x4 float matrix.
 */

/**
 * @brief Create a rotation matrix.
 *
 * @param rot   a rotation.
 *
 * @return a matrix representation of @p rot.
 */
const mat4f mat4f::rotation(const openvrml::rotation & rot) throw ()
{
    const double s = sin(rot.angle());
    const double c = cos(rot.angle());
    const double t = 1.0 - c;
    const float x = rot.x();
    const float y = rot.y();
    const float z = rot.z();

    return mat4f(float(t * x * x + c),
                 float(t * x * y + s * z),
                 float(t * x * z - s * y),
                 0.0f,
                 float(t * x * y - s * z),
                 float(t * y * y + c),
                 float(t * y * z + s * x),
                 0.0f,
                 float(t * x * z + s * y),
                 float(t * y * z - s * x),
                 float(t * z * z + c),
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 1.0f);
}

/**
 * @brief Create a rotation matrix.
 *
 * @param quat  a quaternion.
 *
 * @return a matrix representation of @p quat.
 */
const mat4f mat4f::rotation(const quatf & quat) throw ()
{
    const float x = quat.x();
    const float y = quat.y();
    const float z = quat.z();
    const float w = quat.w();

    return mat4f(float(1.0 - 2.0 * (y * y + z * z)),
                 float(2.0 * (x * y + z * w)),
                 float(2.0 * (z * x - y * w)),
                 0.0f,
                 float(2.0 * (x * y - z * w)),
                 float(1.0 - 2.0 * (z * z + x * x)),
                 float(2.0 * (y * z + x * w)),
                 0.0f,
                 float(2.0 * (z * x + y * w)),
                 float(2.0 * (y * z - x * w)),
                 float(1.0 - 2.0 * (y * y + x * x)),
                 0.0f,
                 0.0f,
                 0.0f,
                 0.0f,
                 1.0f);
}

/**
 * @brief Create a uniform scale matrix.
 *
 * @param s scale factor.
 *
 * @return a uniform scale matrix.
 */
const mat4f mat4f::scale(const float s) throw ()
{
    return mat4f(s,   0.0, 0.0, 0.0,
                 0.0, s,   0.0, 0.0,
                 0.0, 0.0, s,   0.0,
                 0.0, 0.0, 0.0, 1.0);
}

/**
 * @brief Create a scale matrix.
 *
 * @param s a vector.
 *
 * @return a scale matrix.
 */
const mat4f mat4f::scale(const vec3f & s) throw ()
{
    return mat4f(s.x(), 0.0,   0.0,   0.0,
                 0.0,   s.y(), 0.0,   0.0,
                 0.0,   0.0,   s.z(), 0.0,
                 0.0,   0.0,   0.0,   1.0);
}

/**
 * @brief Create a translation matrix.
 *
 * @param t translation vector.
 *
 * @return a translation matrix.
 */
const mat4f mat4f::translation(const vec3f & t) throw ()
{
    return mat4f(1.0,   0.0,   0.0,   0.0,
                 0.0,   1.0,   0.0,   0.0,
                 0.0,   0.0,   1.0,   0.0,
                 t.x(), t.y(), t.z(), 1.0);
}

/**
 * @brief Create a transformation matrix from a translation, a rotation,
 *      a scale, a scaleOrientation, and a center.
 *
 * @param t     the translation.
 * @param r     the rotation.
 * @param s     the scale.
 * @param sr    the scale orientation.
 * @param c     the center.
 *
 * @return a transformation matrix.
 */
const mat4f mat4f::transformation(const vec3f & t,
                                  const openvrml::rotation & r,
                                  const vec3f & s,
                                  const openvrml::rotation & sr,
                                  const vec3f & c)
    throw ()
{
    using openvrml_::fequal;

    mat4f mat;
    if (t != vec3f(0.0, 0.0, 0.0)) {
        mat = translation(t) * mat; // M = T * M   = T
    }
    if (c != vec3f(0.0, 0.0, 0.0)) {
        mat = translation(c) * mat; // M = C * M   = C * T
    }
    if (!fequal<float>()(r.angle(), 0.0f)) {
        mat = rotation(r) * mat; // M = R * M    = R * C * T
    }
    if (s != vec3f(1.0, 1.0, 1.0)) {
        if (!fequal<float>()(sr.angle(), 0.0f)) {
            mat = rotation(sr) * mat; // M = SR * M    = SR * R * C * T
        }
        mat = scale(s) * mat; // M = S * M     = S * SR * R * C * T
        if (!fequal<float>()(sr.angle(), 0.0f)) {
            // M = -SR * M   = -SR * S * SR * R * C * T
            mat = rotation(sr.inverse()) * mat;
        }
    }
    if (c != vec3f(0.0, 0.0, 0.0)) {
        // M = -C * M    =  -C * -SR * S * SR * R * C * T
        mat = translation(-c) * mat;
    }
    return mat;
}

/**
 * @brief Construct.
 *
 * Matrix is initialized to the identity matrix.
 */
mat4f::mat4f() throw ()
{
    this->mat[0][0] = 1.0;
    this->mat[0][1] = 0.0;
    this->mat[0][2] = 0.0;
    this->mat[0][3] = 0.0;

    this->mat[1][0] = 0.0;
    this->mat[1][1] = 1.0;
    this->mat[1][2] = 0.0;
    this->mat[1][3] = 0.0;

    this->mat[2][0] = 0.0;
    this->mat[2][1] = 0.0;
    this->mat[2][2] = 1.0;
    this->mat[2][3] = 0.0;

    this->mat[3][0] = 0.0;
    this->mat[3][1] = 0.0;
    this->mat[3][2] = 0.0;
    this->mat[3][3] = 1.0;
}

/**
 * @brief Construct mat4f with given 16 elements in row-major order.
 *
 * @param f11
 * @param f12
 * @param f13
 * @param f14
 * @param f21
 * @param f22
 * @param f23
 * @param f24
 * @param f31
 * @param f32
 * @param f33
 * @param f34
 * @param f41
 * @param f42
 * @param f43
 * @param f44
 */
mat4f::mat4f(float f11, float f12, float f13, float f14,
             float f21, float f22, float f23, float f24,
             float f31, float f32, float f33, float f34,
             float f41, float f42, float f43, float f44)
    throw ()
{
    this->mat[0][0] = f11;
    this->mat[0][1] = f12;
    this->mat[0][2] = f13;
    this->mat[0][3] = f14;

    this->mat[1][0] = f21;
    this->mat[1][1] = f22;
    this->mat[1][2] = f23;
    this->mat[1][3] = f24;

    this->mat[2][0] = f31;
    this->mat[2][1] = f32;
    this->mat[2][2] = f33;
    this->mat[2][3] = f34;

    this->mat[3][0] = f41;
    this->mat[3][1] = f42;
    this->mat[3][2] = f43;
    this->mat[3][3] = f44;
}

/**
 * @brief Construct from an array of 16 values.
 *
 * @param mat   an array of values in row-major order.
 */
mat4f::mat4f(const float mat[16]) throw ()
{
    std::copy(mat, mat + 16, &this->mat[0][0]);
}

/**
 * @brief Construct from a 4x4 array.
 *
 * @param mat   a 4x4 array of elements in row-major order.
 */
mat4f::mat4f(const float (&mat)[4][4]) throw ()
{
    std::copy(&mat[0][0], &mat[0][0] + 16, &this->mat[0][0]);
}

/**
 * @brief Multiply by a scalar.
 *
 * @param scalar    value by which to multiply.
 *
 * @return a reference to the object.
 */
mat4f & mat4f::operator*=(const float scalar) throw ()
{
    this->mat[0][0] *= scalar;
    this->mat[0][1] *= scalar;
    this->mat[0][2] *= scalar;
    this->mat[0][3] *= scalar;
    this->mat[1][0] *= scalar;
    this->mat[1][1] *= scalar;
    this->mat[1][2] *= scalar;
    this->mat[1][3] *= scalar;
    this->mat[2][0] *= scalar;
    this->mat[2][1] *= scalar;
    this->mat[2][2] *= scalar;
    this->mat[2][3] *= scalar;
    this->mat[3][0] *= scalar;
    this->mat[3][1] *= scalar;
    this->mat[3][2] *= scalar;
    this->mat[3][3] *= scalar;
    return *this;
}

/**
 * @fn const mat4f operator*(const mat4f & mat, const float scalar) throw ()
 *
 * @relates openvrml::mat4f
 *
 * @brief Multiply a matrix by a scalar value.
 *
 * @param mat       matrix.
 * @param scalar    scalar.
 *
 * @return the result matrix.
 */

/**
 * @fn const mat4f operator*(const float scalar, const mat4f & mat) throw ()
 *
 * @relates openvrml::mat4f
 *
 * @brief Multiply a scalar value by matrix.
 *
 * @param scalar    scalar.
 * @param mat       matrix.
 *
 * @return the result matrix.
 */

/**
 * @brief Multiply by another matrix.
 *
 * Sets the matrix equal to the matrix multiplied by @p mat.
 *
 * @param mat   the matrix by which to multiply.
 *
 * @return a reference to the object.
 */
mat4f & mat4f::operator*=(const mat4f & mat) throw ()
{
    mat4f temp;

#define POSTMULT(i,j) (this->mat[i][0] * mat.mat[0][j] + \
                       this->mat[i][1] * mat.mat[1][j] + \
                       this->mat[i][2] * mat.mat[2][j] + \
                       this->mat[i][3] * mat.mat[3][j])

    temp[0][0] = POSTMULT(0,0);
    temp[0][1] = POSTMULT(0,1);
    temp[0][2] = POSTMULT(0,2);
    temp[0][3] = POSTMULT(0,3);
    temp[1][0] = POSTMULT(1,0);
    temp[1][1] = POSTMULT(1,1);
    temp[1][2] = POSTMULT(1,2);
    temp[1][3] = POSTMULT(1,3);
    temp[2][0] = POSTMULT(2,0);
    temp[2][1] = POSTMULT(2,1);
    temp[2][2] = POSTMULT(2,2);
    temp[2][3] = POSTMULT(2,3);
    temp[3][0] = POSTMULT(3,0);
    temp[3][1] = POSTMULT(3,1);
    temp[3][2] = POSTMULT(3,2);
    temp[3][3] = POSTMULT(3,3);

#undef POSTMULT

    *this = temp;
    return *this;
}

/**
 * @fn const mat4f operator*(const mat4f & lhs, const mat4f & rhs) throw ()
 *
 * @relates openvrml::mat4f
 *
 * @brief Multiply two matrices.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the result matrix.
 */

namespace {

    float det3(const mat4f & mat,
               int r1, int r2, int r3, int c1, int c2, int c3) throw ()
    {
        return mat[r1][c1] * mat[r2][c2] * mat[r3][c3]
             - mat[r1][c1] * mat[r2][c3] * mat[r3][c2]
             + mat[r1][c2] * mat[r2][c3] * mat[r3][c1]
             - mat[r1][c2] * mat[r2][c1] * mat[r3][c3]
             + mat[r1][c3] * mat[r2][c1] * mat[r3][c2]
             - mat[r1][c3] * mat[r2][c2] * mat[r3][c1];
    }
}

/**
 * @brief Get the translation, rotation and scale from the orthogonal
 *        transformation matrix.
 *
 * Here any projection or shear information in the matrix is ignored. The
 * caller is responsible for assuring that the transformation is in fact
 * orthogonal, otherwise the results are undefined.
 *
 * @retval t    translation.
 * @retval r    rotation.
 * @retval s    scale.
 */
void mat4f::transformation(vec3f & t, openvrml::rotation & r, vec3f & s) const
    throw ()
{
    //
    // Some portions are taken from Graphics Gems 2.
    //
    using openvrml_::fequal;

    //
    // Check if it is singular.
    //
    assert(!fequal<float>()(this->mat[3][3], 0.0f));
    if (fequal<float>()(det3(*this, 0, 1, 2, 0, 1, 2), 0.0f)){
       OPENVRML_PRINT_MESSAGE_("Warning: matrix is singular.");
       return;
    }

    mat4f tmp_matrix(*this);
    size_t i, j;

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    t.x(tmp_matrix[3][0]);
    t.y(tmp_matrix[3][1]);
    t.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    s.x(row_0.length());
    row_0 = row_0.normalize();

    //
    // Compute Y scale factor and normalize second row.
    //
    s.y(row_1.length());
    row_1 = row_1.normalize();

    //
    // Compute Z scale factor and normalize third row.
    //
    s.z(row_2.length());
    row_2 = row_2.normalize();
    for (j = 0; j < 3; j++) {
        tmp_matrix[0][j] = row_0[j];
        tmp_matrix[1][j] = row_1[j];
        tmp_matrix[2][j] = row_2[j];
    }

    //
    // At this point, the matrix (in rows[]) is orthonormal.
    // That is, each row has length one and are mutually perpendicular.
    // Is it needed to calculate the det of this matrix (1 or -1) for checking
    // coordinate system flip ? (According to VRML standard, scale > 0.0)
    // Calculate quaternion rotation from this matrix.
    //
    quatf quat(tmp_matrix);

    //
    // now convert back to axis/angle.
    //
    r = openvrml::rotation(quat);
}

/**
 * @brief Get the translation, rotation, scale and shear from affine
 *      transformation matrix.
 *
 * Here any projection information in matrix is ignored. As VRML allows
 * non-uniform scaling, it is safe to call this routine. The caller is
 * responsible for assuring that the transformation is in fact affine,
 * otherwise the results are undefined.
 *
 * @retval t        translation.
 * @retval r        rotation.
 * @retval s        scale.
 * @retval shear    shear.
 */
void mat4f::transformation(vec3f & t,
                           openvrml::rotation & r,
                           vec3f & s,
                           vec3f & shear) const
    throw ()
{
    //
    // Some portions are taken from Graphics Gems 2.
    //
    using openvrml_::fequal;

    //
    // Check if it is singular.
    //
    assert(!fequal<float>()(this->mat[3][3], 0.0f));
    if (fequal<float>()(det3(*this, 0, 1, 2, 0, 1, 2), 0.0f)){
        OPENVRML_PRINT_MESSAGE_("Warning: matrix is singular.");
        return;
    }

    mat4f tmp_matrix(*this);
    size_t i, j;

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    t.x(tmp_matrix[3][0]);
    t.y(tmp_matrix[3][1]);
    t.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    s.x(row_0.length());
    row_0 = row_0.normalize();

    //
    // Compute XY shear factor and make 2nd row orthogonal to 1st.
    //
    float shear_xy = row_0.dot(row_1);
    row_1[0] = row_1[0] - row_0[0] * shear_xy;
    row_1[1] = row_1[1] - row_0[1] * shear_xy;
    row_1[2] = row_1[2] - row_0[2] * shear_xy;

    //
    // Compute Y scale factor and normalize second row.
    //
    s.y(row_1.length());
    row_1 = row_1.normalize();
    shear.x(shear_xy / s.y());

    //
    // Compute XZ and YZ shears, orthogonalize third row.
    //
    float shear_xz = row_0.dot(row_2);
    row_2[0] = row_2[0] - row_0[0] * shear_xz;
    row_2[1] = row_2[1] - row_0[1] * shear_xz;
    row_2[2] = row_2[2] - row_0[2] * shear_xz;
    float shear_yz = row_1.dot(row_2);
    row_2[0] = row_2[0] - row_1[0] * shear_yz;
    row_2[1] = row_2[1] - row_1[1] * shear_yz;
    row_2[2] = row_2[2] - row_1[2] * shear_yz;

    //
    // Compute Z scale factor and normalize third row.
    //
    s.z(row_2.length());
    row_2 = row_2.normalize();
    shear.y(shear_xz / s.z());
    shear.z(shear_yz / s.z());
    for (j = 0; j < 3; j++){
        tmp_matrix[0][j] = row_0[j];
        tmp_matrix[1][j] = row_1[j];
        tmp_matrix[2][j] = row_2[j];
    }

    //
    // At this point, the matrix (in rows[]) is orthonormal.
    // That is, each row has length one and are mutually perpendicular.
    // Is it needed to calculate the det of this matrix (1 or -1) for checking
    // coordinate system flip ? (According to VRML standard, scale > 0.0)
    // Calculate quaternion rotation from this matrix.
    //
    quatf quat(tmp_matrix);

    //
    // Now convert back to axis/angle.
    //
    r = openvrml::rotation(quat);
}

namespace {
    /**
     * This is taken from Graphics Gems 2, Page 603 and it is valid for only
     * affine matrix with dimension of 4x4. As here we are storing row-major
     * order, this means the last column MUST be [0 0 0 1]
     *
     * By this procedure there is a significant performance improvement over
     * a general procedure that can invert any nonsingular matrix.
     *
     *            -1
     *  -1   |    |      |  -1    |
     * M   = |A  0|  =   | A     0|
     *       |    |      |        |
     *       |    |      |   -1   |
     *       |B  1|      |-BA    1|
     *
     *  where   M is a 4 by 4 matrix,
     *          A is the 3 by 3 upper left submatrix of M,
     *          B is the 1 by 3 lower left submatrix of M.
     *
     * It aborts if input matrix is not affine.
     * It returns without doing any calculations if the input matrix is
     * singular.
     *
     * @param in   3D affine matrix.
     *
     * @retval out  inverse of 3D affine matrix.
     */
    void get_affine_inverse(const mat4f & in, mat4f & out) {
        // Check if it is an affine matrix
        assert(! ( in[0][3] != 0.0 || in[1][3] != 0.0 || in[2][3] != 0.0 ||
               in[3][3] != 1.0 ));

        double det;
        double pos, neg, temp;

        pos = neg = 0.0;
        temp =  in[0][0] * in[1][1] * in[2][2];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp =  in[0][1] * in[1][2] * in[2][0];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp =  in[0][2] * in[1][0] * in[2][1];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][2] * in[1][1] * in[2][0];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][1] * in[1][0] * in[2][2];
        if(temp >= 0.0) pos += temp; else neg += temp;
        temp = -in[0][0] * in[1][2] * in[2][1];
        if(temp >= 0.0) pos += temp; else neg += temp;
        det = pos + neg;

#define PRECISION_LIMIT (1.0e-25)

        // Is the submatrix A singular?

        if (det * det < PRECISION_LIMIT){
            OPENVRML_PRINT_MESSAGE_("Warning : Matrix is singular");
            return;
        }

        // Calculate inverse(A) = adj(A) / det(A)

        det = 1.0 / det;
        out[0][0] =  float((in[1][1] * in[2][2] - in[1][2] * in[2][1]) * det);
        out[1][0] = -float((in[1][0] * in[2][2] - in[1][2] * in[2][0]) * det);
        out[2][0] =  float((in[1][0] * in[2][1] - in[1][1] * in[2][0]) * det);
        out[0][1] = -float((in[0][1] * in[2][2] - in[0][2] * in[2][1]) * det);
        out[1][1] =  float((in[0][0] * in[2][2] - in[0][2] * in[2][0]) * det);
        out[2][1] = -float((in[0][0] * in[2][1] - in[0][1] * in[2][0]) * det);
        out[0][2] =  float((in[0][1] * in[1][2] - in[0][2] * in[1][1]) * det);
        out[1][2] = -float((in[0][0] * in[1][2] - in[0][2] * in[1][0]) * det);
        out[2][2] =  float((in[0][0] * in[1][1] - in[0][1] * in[1][0]) * det);

        // Calculate -B * inverse(A) Do the translation part

        out[3][0] = -( in[3][0] * out[0][0] +
                       in[3][1] * out[1][0] +
                       in[3][2] * out[2][0] );
        out[3][1] = -( in[3][0] * out[0][1] +
                       in[3][1] * out[1][1] +
                       in[3][2] * out[2][1] );
        out[3][2] = -( in[3][0] * out[0][2] +
                       in[3][1] * out[1][2] +
                       in[3][2] * out[2][2] );

        // Fill in last column
        out[0][3] = out[1][3] = out[2][3] = 0.0;
        out[3][3] = 1.0;

#undef PRECISION_LIMIT

    }
}

/**
 * @brief Get the inverse of an affine matrix.
 *
 * @return the inverse of the matrix.
 *
 * @pre the current matrix must be affine.
 */
const mat4f mat4f::inverse() const throw ()
{
    mat4f out;
    get_affine_inverse(*this, out);
    return out;
}

/**
 * @brief Get the transposition of the matrix.
 *
 * @return a transposed copy of the matrix.
 */
const mat4f mat4f::transpose() const throw ()
{
    return mat4f(this->mat[0][0],
                 this->mat[1][0],
                 this->mat[2][0],
                 this->mat[3][0],
                 this->mat[0][1],
                 this->mat[1][1],
                 this->mat[2][1],
                 this->mat[3][1],
                 this->mat[0][2],
                 this->mat[1][2],
                 this->mat[2][2],
                 this->mat[3][2],
                 this->mat[0][3],
                 this->mat[1][3],
                 this->mat[2][3],
                 this->mat[3][3]);
}

/**
 * @brief Return determinant of entire matrix.
 *
 * @return the determinant.
 */
float mat4f::det() const throw ()
{
    return (  this->mat[0][0] * det3(*this, 1, 2, 3, 1, 2, 3)
            + this->mat[0][1] * det3(*this, 1, 2, 3, 0, 2, 3)
            + this->mat[0][2] * det3(*this, 1, 2, 3, 0, 1, 3)
            + this->mat[0][3] * det3(*this, 1, 2, 3, 0, 1, 2));
}

/**
 * @fn float (&mat4f::operator[](size_t index) throw ())[4]
 *
 * @brief To make it usual matrix (non-const)
 */

/**
 * @fn float (&mat4f::operator[](size_t index) const throw ())[4]
 *
 * @brief To make it usual matrix (const)
 */

/**
 * @relates mat4f
 *
 * @brief Stream output.
 *
 * @param out   an output stream.
 * @param mat   a matrix.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const mat4f & mat)
{
    out << '[';
    for (size_t i = 0; i < 4; i++) {
        out << '[';
        for (size_t j = 0; j < 4; j++) {
            out << mat[i][j];
            if (j != 3) { out << ", "; }
        }
        out << ']';
        if (i != 3) { out << ", "; }
    }
    out << ']';
    return out;
}


/**
 * @relates mat4f
 *
 * @brief Equality comparison operator.
 *
 * All componenents must match exactly.
 *
 * @param lhs   a matrix.
 * @param rhs   a matrix.
 */
bool operator==(const mat4f & lhs, const mat4f & rhs) throw ()
{
    using openvrml_::fequal;
    return std::equal(&lhs[0][0], &lhs[0][0] + 16, &rhs[0][0], fequal<float>());
}

/**
 * @fn bool operator!=(const mat4f & lhs, const mat4f & rhs) throw ()
 *
 * @relates openvrml::mat4f
 *
 * @brief Inequality comparison operator.
 *
 * @param lhs   a matrix.
 * @param rhs   a matrix.
 */


/**
 * @class quatf
 *
 * @ingroup basetypes
 *
 * @brief A quaternion.
 */

/**
 * @internal
 *
 * @var float quatf::quat[4]
 *
 * @brief An array comprising the quaternion components.
 */

/**
 * @brief Default constructor.
 */
quatf::quatf() throw ()
{
    this->quat[0] = 0.0;
    this->quat[1] = 0.0;
    this->quat[2] = 0.0;
    this->quat[3] = 1.0;
}

/**
 * @brief Construct from four values.
 *
 * @param x the x vector component.
 * @param y the y vector component.
 * @param z the z vector component.
 * @param w the scalar value w.
 */
quatf::quatf(const float x, const float y, const float z, const float w)
    throw ()
{
    this->quat[0] = x;
    this->quat[1] = y;
    this->quat[2] = z;
    this->quat[3] = w;
}

/**
 * @brief Construct from an array of four values.
 *
 * @param quat  the first three values in the array are used for the x, y, and
 *              z vector components, respectively. The fourth value in the
 *              array is used for the scalar part of the quaternion.
 */
quatf::quatf(const float (&quat)[4]) throw ()
{
    this->quat[0] = quat[0];
    this->quat[1] = quat[1];
    this->quat[2] = quat[2];
    this->quat[3] = quat[3];
}

/**
 * @brief Construct from a rotation matrix.
 *
 * @param mat   a rotation matrix.
 */
quatf::quatf(const mat4f & mat) throw ()
{
    float diagonal, s;
    diagonal = mat[0][0] + mat[1][1] + mat[2][2];
    // check the diagonal
    if (diagonal > 0.0) {
        s = float(sqrt(diagonal + 1.0));
        this->quat[3] = s / 2.0f;
        s = 0.5f / s;
        this->quat[0] = (mat[1][2] - mat[2][1]) * s;
        this->quat[1] = (mat[2][0] - mat[0][2]) * s;
        this->quat[2] = (mat[0][1] - mat[1][0]) * s;
    } else {
        using openvrml_::fequal;
        size_t i, j, k;
        static const size_t next[3] = { 1, 2, 0 };
        // diagonal is negative
        i = 0;
        if (mat[1][1] > mat[0][0]) { i = 1; }
        if (mat[2][2] > mat[i][i]) { i = 2; }
        j = next[i];
        k = next[j];
        s = float(sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0));
        this->quat[i] = s * 0.5f;

        if (!fequal<float>()(s, 0.0f)) { s = 0.5f / s; }
        this->quat[3] = (mat[j][k] - mat[k][j]) * s;
        this->quat[j] = (mat[i][j] + mat[j][i]) * s;
        this->quat[k] = (mat[i][k] + mat[k][i]) * s;
    }
}

/**
 * @brief Construct from an rotation.
 *
 * @param rot   a rotation.
 */
quatf::quatf(const rotation & rot) throw ()
{
# ifndef NDEBUG
    using openvrml_::fequal;
    fequal<float> feq;
# endif
    assert(feq(rot.axis().length(), 1.0));

    const float sin_angle = float(sin(rot.angle() / 2.0));
    this->quat[0] = rot.x() * sin_angle;
    this->quat[1] = rot.y() * sin_angle;
    this->quat[2] = rot.z() * sin_angle;
    this->quat[3] = float(cos(rot.angle() / 2.0));
}

/**
 * @brief Multiply by a quaternion.
 *
 * @note <b>qr</b> = (<b>q</b><sub><i>v</i></sub> x <b>r</b><sub><i>v</i></sub>
 *                   + <i>r<sub>w</sub></i><b>q</b><sub><i>v</i></sub>
 *                   + <i>q<sub>w</sub></i><b>r</b><sub><i>v</i></sub>,
 *                   <i>q<sub>w</sub>r<sub>w</sub></i>
 *                   - <b>q</b><sub><i>v</i></sub>
 *                   . <b>r</b><sub><i>v</i></sub>)
 *
 * @param quat  the quaternion by which to multiply.
 *
 * @return a reference to the object.
 */
quatf & quatf::operator*=(const quatf & quat) throw ()
{
    *this = quatf(this->quat[1] * quat[2] - this->quat[2] * quat[1]
                    + quat[3] * this->quat[0] + this->quat[3] * quat[0],
                  this->quat[2] * quat[0] - this->quat[0] * quat[2]
                    + quat[3] * this->quat[1] + this->quat[3] * quat[1],
                  this->quat[0] * quat[1] - this->quat[1] * quat[0]
                    + quat[3] * this->quat[2] + this->quat[3] * quat[2],
                  this->quat[3] * quat[3]
                    - (this->quat[0] * quat[0] + this->quat[1] * quat[1]
                        + this->quat[2] * quat[2]));
    return *this;
}

/**
 * @fn const quatf operator*(const quatf & lhs, const quatf & rhs) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Multiply two quaternions.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the product of @p lhs and @p rhs.
 */

/**
 * @brief Multiply by a scalar.
 *
 * @param scalar    value by which to multiply.
 *
 * @return a reference to the object.
 */
quatf & quatf::operator*=(const float scalar) throw ()
{
    this->quat[0] *= scalar;
    this->quat[1] *= scalar;
    this->quat[2] *= scalar;
    this->quat[3] *= scalar;
    return *this;
}

/**
 * @fn const quatf operator*(const quatf & quat, float scalar) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Multiply a quaternion by a scalar.
 *
 * @param quat      quaternion.
 * @param scalar    scalar.
 *
 * @return the product of @p quat and @p scalar.
 */

/**
 * @fn const quatf operator*(float scalar, const quatf & quat) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Multiply a scalar by a quaternion.
 *
 * @param scalar    scalar.
 * @param quat      quaternion.
 *
 * @return the product of @p scalar and @p quat.
 */

/**
 * @brief Divide by a scalar.
 *
 * @param scalar    value by which to divide.
 *
 * @return a reference to the object.
 */
quatf & quatf::operator/=(const float scalar) throw ()
{
    this->quat[0] /= scalar;
    this->quat[1] /= scalar;
    this->quat[2] /= scalar;
    this->quat[3] /= scalar;
    return *this;
}

/**
 * @fn const quatf operator/(const quatf & quat, float scalar) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Divide a quaternion by a scalar.
 *
 * @param quat      quaternion.
 * @param scalar    scalar.
 *
 * @return the result of dividing @p quat by @p scalar.
 */

/**
 * @brief Add a quaternion.
 *
 * @param quat  the quaternion to add.
 *
 * @return a reference to the object.
 */
quatf & quatf::operator+=(const quatf & quat) throw ()
{
    this->quat[0] += quat[0];
    this->quat[1] += quat[1];
    this->quat[2] += quat[2];
    this->quat[3] += quat[3];
    return *this;
}

/**
 * @fn const quatf operator+(const quatf & lhs, const quatf & rhs) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Add two quaternions.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the sum of @p lhs and @p rhs.
 */

/**
 * @brief Subtract a quaternion.
 *
 * @param quat  the quaternion to subtract.
 *
 * @return a reference to the object.
 */
quatf & quatf::operator-=(const quatf & quat) throw ()
{
    this->quat[0] -= quat[0];
    this->quat[1] -= quat[1];
    this->quat[2] -= quat[2];
    this->quat[3] -= quat[3];
    return *this;
}

/**
 * @fn const quatf operator-(const quatf & lhs, const quatf & rhs) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Take the difference between two quaternions.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return the difference between @p lhs and @p rhs.
 */

/**
 * @fn float quatf::operator[](size_t index) const throw ()
 *
 * @brief Array element dereference operator (const version).
 *
 * @param index an index from 0 - 3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn float & quatf::operator[](size_t index) throw ()
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param index an index from 0 - 3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn float quatf::x() const throw ()
 *
 * @brief Get the x component.
 *
 * @return the x component value.
 */

/**
 * @fn void quatf::x(float value) throw ()
 *
 * @brief Set the x component.
 *
 * @param value x component value.
 */

/**
 * @fn float quatf::y() const throw ()
 *
 * @brief Get the y component.
 *
 * @return the y component value.
 */

/**
 * @fn void quatf::y(float value) throw ()
 *
 * @brief Set the y component.
 *
 * @param value y component value.
 */

/**
 * @fn float quatf::z() const throw ()
 *
 * @brief Get the z component.
 *
 * @return the z component value.
 */

/**
 * @fn void quatf::z(float value) throw ()
 *
 * @brief Set the z component.
 *
 * @param value z component value.
 */

/**
 * @fn float quatf::w() const throw ()
 *
 * @brief Get the w component.
 *
 * @return the w component value.
 */

/**
 * @fn void quatf::w(float value) throw ()
 *
 * @brief Set the w component.
 *
 * @param value w component value.
 */

/**
 * @brief Get the conjugate.
 *
 * @note <b>q</b><sup>*</sup> = (-<b>q</b><sub><i>v</i></sub>,
 *                              <i>q<sub>w</sub></i>)
 *
 * @return the conjugate of the quaternion.
 */
const quatf quatf::conjugate() const throw ()
{
    const quatf q(-this->quat[0],
                  -this->quat[1],
                  -this->quat[2],
                  this->quat[3]);
    return q;
}

/**
 * @brief Get the inverse.
 *
 * @note <b>q</b><sup>-1</sup> = <b>q</b><sup>*</sup> / <i>n</i>(<b>q</b>)
 *
 * @return the multiplicative inverse.
 */
const quatf quatf::inverse() const throw ()
{
    return this->conjugate() / this->norm();
}

/**
 * @brief Get the norm.
 *
 * @note <i>n</i>(<b>q</b>) = <i>q<sub>x</sub></i><sup>2</sup>
 *                            + <i>q<sub>y</sub></i><sup>2</sup>
 *                            + <i>q<sub>z</sub></i><sup>2</sup>
 *                            + <i>q<sub>w</sub></i><sup>2</sup>
 *
 * @return the norm.
 */
float quatf::norm() const throw ()
{
    return this->quat[0] * this->quat[0]
            + this->quat[1] * this->quat[1]
            + this->quat[2] * this->quat[2]
            + this->quat[3] * this->quat[3];
}

/**
 * @brief Normalize the quaternion.
 *
 * @return a unit quaternion derived from the quaternion.
 */
const quatf quatf::normalize() const throw ()
{
    const float n = this->norm();
    const quatf q(this->quat[0] / n,
                       this->quat[1] / n,
                       this->quat[2] / n,
                       this->quat[3] / n);
    return q;
}

/**
 * @relates quatf
 *
 * @brief Stream output.
 *
 * @param out   an output stream.
 * @param quat  a quaternion.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const quatf & quat)
{
    return out << quat.x() << ' ' << quat.y() << ' ' << quat.z() << ' '
               << quat.w();
}

/**
 * @relates quatf
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are equal; @c false otherwise.
 */
bool operator==(const quatf & lhs, const quatf & rhs) throw ()
{
    using openvrml_::fequal;
    return std::equal(&lhs[0], &lhs[0] + 4, &rhs[0], fequal<float>());
}

/**
 * @fn bool operator!=(const quatf & lhs, const quatf & rhs) throw ()
 *
 * @relates openvrml::quatf
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs are not equal; @c false otherwise.
 */

}
