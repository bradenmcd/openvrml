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

# include "quaternion.h"
# include "private.h"
# include "basetypes.h"

namespace OpenVRML {

/**
 * @class Quaternion
 *
 * @brief A quaternion.
 */

/**
 * @var Quaterion::quat
 *
 * @brief An array comprising the quaternion components.
 */

/**
 * @brief Default constructor.
 */
Quaternion::Quaternion() throw ()
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
Quaternion::Quaternion(const float x,
                       const float y,
                       const float z,
                       const float w)
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
Quaternion::Quaternion(const float quat[4]) throw ()
{
    this->quat[0] = quat[0];
    this->quat[1] = quat[1];
    this->quat[2] = quat[2];
    this->quat[3] = quat[3];
}

Quaternion::Quaternion(const mat4f & mat) throw ()
{
    float diagonal, s;
    diagonal = mat[0][0] + mat[1][1] + mat[2][2];
    // check the diagonal
    if (diagonal > 0.0) {
        s = sqrt(diagonal + 1.0);
        this->quat[3] = s / 2.0;
        s = 0.5 / s;
        this->quat[0] = (mat[1][2] - mat[2][1]) * s;
        this->quat[1] = (mat[2][0] - mat[0][2]) * s;
        this->quat[2] = (mat[0][1] - mat[1][0]) * s;
    } else {
        size_t i, j, k;
        static const size_t next[3] = { 1, 2, 0 };
        // diagonal is negative
        i = 0;
        if (mat[1][1] > mat[0][0]) { i = 1; }
        if (mat[2][2] > mat[i][i]) { i = 2; }
        j = next[i];
        k = next[j];
        s = sqrt ((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0);
        this->quat[i] = s * 0.5;

        if (s != 0.0) { s = 0.5 / s; }
        this->quat[3] = (mat[j][k] - mat[k][j]) * s;
        this->quat[j] = (mat[i][j] + mat[j][i]) * s;
        this->quat[k] = (mat[i][k] + mat[k][i]) * s;
    }
}

/**
 * @brief Construct from an SFRotation.
 *
 * @param rot   a rotation.
 */
Quaternion::Quaternion(const rotation & rot) throw ()
{
    using OpenVRML_::fpequal;
    assert(fpequal(rot.axis().length(), 1.0));

    const float sin_angle = sin(rot.angle() / 2.0);
    this->quat[0] = rot.x() * sin_angle;
    this->quat[1] = rot.y() * sin_angle;
    this->quat[2] = rot.z() * sin_angle;
    this->quat[3] = cos(rot.angle() / 2.0);
}

/**
 * @fn float Quaternion::operator[](size_t index) const throw ()
 *
 * @brief Array element dereference operator (const version).
 *
 * @param index an index from 0 - 3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn float & Quaternion::operator[](size_t index) throw ()
 *
 * @brief Array element dereference operator (non-const version).
 *
 * @param index an index from 0 - 3.
 *
 * @pre @p index is not larger than 3.
 */

/**
 * @fn const float (&Quaternion::get() const throw ())[4]
 *
 * @brief Get the value.
 *
 * @return a reference to a 4-element array comprising the quaternion value.
 */

/**
 * @fn void Quaternion::set(const float (&quat)[4]) const throw ()
 *
 * @brief Set the value.
 *
 * @param quat  a 4-element array comprising a quaternion value, ordered
 *              <var>x</var>, <var>y</var>, <var>z</var>, <var>w</var>.
 */

/**
 * @fn float Quaternion::getX() const throw ()
 *
 * @brief Get the x component.
 *
 * @return the x component value.
 */

/**
 * @fn void Quaternion::setX(float x) throw ()
 *
 * @brief Set the x component.
 *
 * @param x x component value.
 */

/**
 * @fn float Quaternion::getY() const throw ()
 *
 * @brief Get the y component.
 *
 * @return the y component value.
 */

/**
 * @fn void Quaternion::setY(float y) throw ()
 *
 * @brief Set the y component.
 *
 * @param y y component value.
 */

/**
 * @fn float Quaternion::getZ() const throw ()
 *
 * @brief Get the z component.
 *
 * @return the z component value.
 */

/**
 * @fn void Quaternion::setZ(float z) throw ()
 *
 * @brief Set the z component.
 *
 * @param z z component value.
 */

/**
 * @fn float Quaternion::getW() const throw ()
 *
 * @brief Get the w component.
 *
 * @return the w component value.
 */

/**
 * @fn void Quaternion::setW(float w) throw ()
 *
 * @brief Set the w component.
 *
 * @param w w component value.
 */

/**
 * @brief Addition.
 *
 * @param quat  the quaternion to add.
 *
 * @return the sum of the quaternion and @p quat.
 */
const Quaternion Quaternion::add(const Quaternion & quat) const throw ()
{
    const Quaternion q(this->quat[0] + quat[0],
                       this->quat[1] + quat[1],
                       this->quat[2] + quat[2],
                       this->quat[3] + quat[3]);
    return q;
}

/**
 * @brief Get the conjugate.
 *
 * @note <b>q</b><sup>*</sup> = (-<b>q</b><sub><i>v</i></sub>,
 *                              <i>q<sub>w</sub></i>)
 *
 * @return the conjugate of the quaternion.
 */
const Quaternion Quaternion::conjugate() const throw ()
{
    const Quaternion q(-this->quat[0],
                       -this->quat[1],
                       -this->quat[2],
                       this->quat[3]);
    return q;
}

/**
 * @brief Division by a scalar value.
 *
 * @param value a scalar value by which to divide.
 *
 * @return the result of divding the quaternion by @p value.
 */
const Quaternion Quaternion::divide(const float value) const throw ()
{
    const Quaternion q(this->quat[0] / value,
                       this->quat[1] / value,
                       this->quat[2] / value,
                       this->quat[3] / value);
    return q;
}

/**
 * @brief Get the inverse.
 *
 * @note <b>q</b><sup>-1</sup> = <b>q</b><sup>*</sup> / <i>n</i>(<b>q</b>)
 *
 * @return the multiplicative inverse.
 */
const Quaternion Quaternion::inverse() const throw ()
{
    return this->conjugate().divide(this->norm());
}

/**
 * @brief Quaternion multiplication.
 *
 * @note <b>qr</b> = (<b>q</b><sub><i>v</i></sub> x <b>r</b><sub><i>v</i></sub>
 *                   + <i>r<sub>w</sub></i><b>q</b><sub><i>v</i></sub>
 *                   + <i>q<sub>w</sub></i><b>r</b><sub><i>v</i></sub>,
 *                   <i>q<sub>w</sub>r<sub>w</sub></i> - <b>q</b><sub><i>v</i></sub>
 *                   . <b>r</b><sub><i>v</i></sub>)
 *
 * @param quat  the quaternion by which to multiply.
 *
 * @return the product of the quaternion and @p quat.
 */
const Quaternion Quaternion::multiply(const Quaternion & quat) const throw ()
{
    const Quaternion q(this->quat[1] * quat[2] - this->quat[2] * quat[1]
                        + quat[3] * this->quat[0] + this->quat[3] * quat[0],
                       this->quat[2] * quat[0] - this->quat[0] * quat[2]
                        + quat[3] * this->quat[1] + this->quat[3] * quat[1],
                       this->quat[0] * quat[1] - this->quat[1] * quat[0]
                        + quat[3] * this->quat[2] + this->quat[3] * quat[2],
                       this->quat[3] * quat[3]
                            - (this->quat[0] * quat[0]
                            + this->quat[1] * quat[1]
                            + this->quat[2] * quat[2]));
    return q;
}

/**
 * @brief Multiplication by a scalar value.
 *
 * @param value a scalar value by which to multiply.
 *
 * @return the product of the quaternion and @p value.
 */
const Quaternion Quaternion::multiply(const float value) const throw ()
{
    const Quaternion q(this->quat[0] * value,
                       this->quat[1] * value,
                       this->quat[2] * value,
                       this->quat[3] * value);
    return q;
}

/**
 * @brief Get the norm.
 *
 * @note <i>n</i>(<b>q</b>) = <i>q<sub>x</sub><i><sup>2</sup>
 *                            + <i>q<sub>y</sub><i><sup>2</sup>
 *                            + <i>q<sub>z</sub><i><sup>2</sup>
 *                            + <i>q<sub>w</sub><i><sup>2</sup>
 *
 * @return the norm.
 */
float Quaternion::norm() const throw ()
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
const Quaternion Quaternion::normalize() const throw ()
{
    const float n = this->norm();
    const Quaternion q(this->quat[0] / n,
                       this->quat[1] / n,
                       this->quat[2] / n,
                       this->quat[3] / n);
    return q;
}

/**
 * @brief Stream output.
 *
 * @param out   an output stream.
 * @param quat  a quaternion.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const Quaternion & quat)
{
    return out << quat[0] << ' ' << quat[1] << ' ' << quat[2] << ' ' << quat[3];
}

} // namespace OpenVRML
