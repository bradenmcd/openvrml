//
// OpenVRML
//
// Copyright (C) 2001  S. K. Bose
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

# include "field.h"
# include "VrmlMatrix.h"
# include "quaternion.h"
# include "private.h"

namespace OpenVRML {

using namespace OpenVRML_;

/**
 * @class VrmlMatrix
 *
 * @brief A class for all matrix operations.
 *
 * Matrices are stored in row-major order. Some parts are taken from MathUtils
 * (thanks to Chris Morley) but format is not same.
 */

/**
 * @relates VrmlMatrix
 *
 * @brief Stream output.
 *
 * @param out   an output stream.
 * @param mat   a matrix.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const VrmlMatrix & mat)
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
 * @relates VrmlMatrix
 *
 * @brief Equality comparison operator.
 *
 * All componenents must match exactly.
 *
 * @param lhs   a matrix.
 * @param rhs   a matrix.
 */
bool operator==(const VrmlMatrix & lhs, const VrmlMatrix & rhs) throw ()
{
    return std::equal(&lhs[0][0], &lhs[0][0] + 16,
                      &rhs[0][0]);
}

/**
 * @fn bool operator!=(const VrmlMatrix & lhs, const VrmlMatrix & rhs) throw ()
 *
 * @relates VrmlMatrix
 *
 * @brief Inequality comparison operator.
 *
 * @param lhs   a matrix.
 * @param rhs   a matrix.
 */

/**
 * @brief Default constructor.
 *
 * Matrix is initialized to the identity matrix.
 */
VrmlMatrix::VrmlMatrix() throw ()
{
    this->matrix[0][0] = 1.0;
    this->matrix[0][1] = 0.0;
    this->matrix[0][2] = 0.0;
    this->matrix[0][3] = 0.0;

    this->matrix[1][0] = 0.0;
    this->matrix[1][1] = 1.0;
    this->matrix[1][2] = 0.0;
    this->matrix[1][3] = 0.0;

    this->matrix[2][0] = 0.0;
    this->matrix[2][1] = 0.0;
    this->matrix[2][2] = 1.0;
    this->matrix[2][3] = 0.0;

    this->matrix[3][0] = 0.0;
    this->matrix[3][1] = 0.0;
    this->matrix[3][2] = 0.0;
    this->matrix[3][3] = 1.0;
}

/**
 * @brief Construct VrmlMatrix with given 16 elements in row-major order.
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
VrmlMatrix::VrmlMatrix(float f11, float f12, float f13, float f14,
                       float f21, float f22, float f23, float f24,
                       float f31, float f32, float f33, float f34,
                       float f41, float f42, float f43, float f44)
    throw ()
{
    this->matrix[0][0] = f11;
    this->matrix[0][1] = f12;
    this->matrix[0][2] = f13;
    this->matrix[0][3] = f14;

    this->matrix[1][0] = f21;
    this->matrix[1][1] = f22;
    this->matrix[1][2] = f23;
    this->matrix[1][3] = f24;

    this->matrix[2][0] = f31;
    this->matrix[2][1] = f32;
    this->matrix[2][2] = f33;
    this->matrix[2][3] = f34;

    this->matrix[3][0] = f41;
    this->matrix[3][1] = f42;
    this->matrix[3][2] = f43;
    this->matrix[3][3] = f44;
}

/**
 * @brief Construct from an array of 16 values.
 *
 * @param m an array of values in row-major order.
 */
VrmlMatrix::VrmlMatrix(const float m[16]) throw ()
{
    std::copy(m, m + 16, &this->matrix[0][0]);
}

/**
 * @brief Construct from a 4x4 array.
 *
 * @param m a 4x4 array of elements in row-major order.
 */
VrmlMatrix::VrmlMatrix(const float m[4][4]) throw ()
{
    std::copy(&m[0][0], &m[0][0] + 16, &this->matrix[0][0]);
}

/**
 * @brief Sets matrix to rotate by given rotation Graphics Gems p466
 *
 * @param axisAngle the rotation object.
 */
void VrmlMatrix::setRotate(const rotation & axisAngle) throw ()
{
    double s = sin(axisAngle[3]);
    double c = cos(axisAngle[3]);
    double t = 1.0 - c;
    double x = axisAngle[0];
    double y = axisAngle[1];
    double z = axisAngle[2];

    this->matrix[0][0] = t*x*x + c;
    this->matrix[1][0] = t*x*y - s*z;
    this->matrix[2][0] = t*x*z + s*y;
    this->matrix[3][0] = 0.0;
    this->matrix[0][1] = t*x*y + s*z;
    this->matrix[1][1] = t*y*y + c;
    this->matrix[2][1] = t*y*z - s*x;
    this->matrix[3][1] = 0.0;
    this->matrix[0][2] = t*x*z - s*y;
    this->matrix[1][2] = t*y*z + s*x;
    this->matrix[2][2] = t*z*z + c;
    this->matrix[3][2] = 0.0;
    this->matrix[0][3] = this->matrix[1][3] = this->matrix[2][3] = 0.0;
    this->matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to a rotation matrix corresponding to a quaternion.
 *
 * @param quat  a quaternion.
 */
void VrmlMatrix::setRotate(const Quaternion & quat) throw ()
{
    this->matrix[0][0] = 1.0 - 2.0 * (quat[1] * quat[1] + quat[2] * quat[2]);
    this->matrix[0][1] = 2.0 * (quat[0] * quat[1] + quat[2] * quat[3]);
    this->matrix[0][2] = 2.0 * (quat[2] * quat[0] - quat[1] * quat[3]);
    this->matrix[0][3] = 0.0;

    this->matrix[1][0] = 2.0 * (quat[0] * quat[1] - quat[2] * quat[3]);
    this->matrix[1][1]= 1.0 - 2.0 * (quat[2] * quat[2] + quat[0] * quat[0]);
    this->matrix[1][2] = 2.0 * (quat[1] * quat[2] + quat[0] * quat[3]);
    this->matrix[1][3] = 0.0;

    this->matrix[2][0] = 2.0 * (quat[2] * quat[0] + quat[1] * quat[3]);
    this->matrix[2][1] = 2.0 * (quat[1] * quat[2] - quat[0] * quat[3]);
    this->matrix[2][2] = 1.0 - 2.0 * (quat[1] * quat[1] + quat[0] * quat[0]);
    this->matrix[2][3] = 0.0;

    this->matrix[3][0] = 0.0;
    this->matrix[3][1] = 0.0;
    this->matrix[3][2] = 0.0;
    this->matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to scale by given uniform factor
 *
 * @param s scale factor.
 */
void VrmlMatrix::setScale(float s) throw ()
{
    this->matrix[0][0] = s;
    this->matrix[0][1] = 0.0;
    this->matrix[0][2] = 0.0;
    this->matrix[0][3] = 0.0;
    this->matrix[1][0] = 0.0;
    this->matrix[1][1] = s;
    this->matrix[1][2] = 0.0;
    this->matrix[1][3] = 0.0;
    this->matrix[2][0] = 0.0;
    this->matrix[2][1] = 0.0;
    this->matrix[2][2] = s;
    this->matrix[2][3] = 0.0;
    this->matrix[3][0] = 0.0;
    this->matrix[3][1] = 0.0;
    this->matrix[3][2] = 0.0;
    this->matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to scale by given vec3f object
 *
 * @param s a vector
 */
void VrmlMatrix::setScale(const vec3f & s) throw ()
{
    this->matrix[0][0] = s[0];
    this->matrix[0][1] = 0.0;
    this->matrix[0][2] = 0.0;
    this->matrix[0][3] = 0.0;
    this->matrix[1][0] = 0.0;
    this->matrix[1][1] = s[1];
    this->matrix[1][2] = 0.0;
    this->matrix[1][3] = 0.0;
    this->matrix[2][0] = 0.0;
    this->matrix[2][1] = 0.0;
    this->matrix[2][2] = s[2];
    this->matrix[2][3] = 0.0;
    this->matrix[3][0] = 0.0;
    this->matrix[3][1] = 0.0;
    this->matrix[3][2] = 0.0;
    this->matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to translate by given vec3f object
 *
 * @param t is a given vector object
 */
void VrmlMatrix::setTranslate(const vec3f & t) throw ()
{
    matrix[0][0] = 1.0;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = 1.0;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
    matrix[2][3] = 0.0;
    matrix[3][0] = t[0];
    matrix[3][1] = t[1];
    matrix[3][2] = t[2];
    matrix[3][3] = 1.0;
}

/**
 * @brief Set the matrix from translation, rotation, scale, scaleOrientation,
 *      and center.
 *
 * @param translation       the translation.
 * @param rotation          the rotation.
 * @param scale             the scale.
 * @param scaleOrientation  the scale orientation.
 * @param center            the center.
 */
void VrmlMatrix::setTransform(const vec3f & translation,
                              const rotation & rotation,
                              const vec3f & scale,
                              const rotation & scaleOrientation,
                              const vec3f & center)
    throw ()
{
    VrmlMatrix temp;
    *this = VrmlMatrix(); // Set to identity.
    if(!((translation[0] == 0.0)
            && (translation[1] == 0.0)
            && (translation[2] == 0.0))) {
        temp.setTranslate(translation);
        *this = multLeft(temp);            // M = T * M   = T
    }
    if(!((center[0] == 0.0)
            && (center[1] == 0.0)
            && (center[2] == 0.0))) {
        temp.setTranslate(center);
        *this = multLeft(temp);            // M = C * M   = C * T
    }
    if(!((rotation.x() == 0.0)
            && (rotation.y() == 0.0)
            && (rotation.z() == 1.0)
            && (rotation.angle() == 0.0))) {
        temp.setRotate(rotation);
        *this = multLeft(temp);            // M = R * M    = R * C * T
    }
    if(!((scale[0] == 1.0)
            && (scale[1] == 1.0)
            && (scale[2] == 1.0))) {
        if(!((scaleOrientation.x() == 0.0)
                && (scaleOrientation.y() == 0.0)
                && (scaleOrientation.z() == 1.0)
                && (scaleOrientation.angle() == 0.0))) {
            temp.setRotate(scaleOrientation);
            *this = multLeft(temp);            // M = SR * M    = SR * R * C * T
        }
        temp.setScale(scale);
        *this = multLeft(temp);            // M = S * M     = S * SR * R * C * T
        if(!((scaleOrientation.x() == 0.0)
                && (scaleOrientation.y() == 0.0)
                && (scaleOrientation.z() == 1.0)
                && (scaleOrientation.angle() == 0.0))) {
            temp.setRotate(scaleOrientation.inverse());
            *this = multLeft(temp);            // M = -SR * M   = -SR * S * SR * R * C * T
        }
    }
    if(!((center[0] == 0.0)
            && (center[1] == 0.0)
            && (center[2] == 0.0))) {
       temp.setTranslate(-center);
       *this = multLeft(temp);            // M = -C * M    =  -C * -SR * S * SR * R * C * T
    }
}

namespace {
    /**
     * @brief Given a rotation matrix, build a quaternion rotation.
     *
     * @param m     orthogonal matrix.
     * @param quat  quaternion rotation.
     */
    void build_quaternion(const float m[4][4], float quat[4]) throw ()
    {
        float  diagonal, s, q[4];
        int    i, j, k;
        int next[3] = { 1, 2, 0 };
        diagonal = m[0][0] + m[1][1] + m[2][2];
        // check the diagonal
        if (diagonal > 0.0) {
            s = sqrt (diagonal + 1.0);
            quat[3] = s / 2.0;
            s = 0.5 / s;
            quat[0] = (m[1][2] - m[2][1]) * s;
            quat[1] = (m[2][0] - m[0][2]) * s;
            quat[2] = (m[0][1] - m[1][0]) * s;
        } else {
            // diagonal is negative
            i = 0;
            if (m[1][1] > m[0][0]) { i = 1; }
            if (m[2][2] > m[i][i]) { i = 2; }
            j = next[i];
            k = next[j];
            s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
            q[i] = s * 0.5;

            if (s != 0.0) { s = 0.5 / s; }
            q[3] = (m[j][k] - m[k][j]) * s;
            q[j] = (m[i][j] + m[j][i]) * s;
            q[k] = (m[i][k] + m[k][i]) * s;

            quat[0] = q[0];
            quat[1] = q[1];
            quat[2] = q[2];
            quat[3] = q[3];
        }
    }
    /**
     * @brief Convert a quaternion to a rotation axis and angle.
     *
     * @param q         input quaternion
     * @param axisAngle output normalized rotation axis and angle.
     */
    void quat_to_axis(const float q[4], float axisAngle[4]) throw ()
    {
        float val = acos(q[3]);
        if (fpzero(val)) {
            axisAngle[0] = 0.0;
            axisAngle[1] = 1.0;
            axisAngle[2] = 0.0;
            axisAngle[3] = 0.0;
        } else {
            float sinval = 1/sin(val);
            axisAngle[0] = q[0]*sinval;
            axisAngle[1] = q[1]*sinval;
            axisAngle[2] = q[2]*sinval;
            axisAngle[3] = 2*val;
            normalize(axisAngle);
        }
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
 * Some portions are taken from Graphics Gems 2.
 *
 * @param translation   output translation.
 * @param rot           output rotation.
 * @param scale         output scale.
 */
void VrmlMatrix::getTransform(vec3f & translation,
                              rotation & rot,
                              vec3f & scale) const
    throw ()
{
    float tmp_matrix[4][4];
    int i, j;

    //
    // Check if it is singular.
    //
    assert(!fpzero(matrix[3][3]));
    if(fpzero(det3(0, 1, 2, 0, 1, 2))){
       OPENVRML_PRINT_MESSAGE_("Warning : Matrix is singular");
       return;
    }
    std::copy(&this->matrix[0][0], &this->matrix[0][0] + 16, &tmp_matrix[0][0]);

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4;i++) {
        for (j = 0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    translation.x(tmp_matrix[3][0]);
    translation.y(tmp_matrix[3][1]);
    translation.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    scale[0] = row_0.length();
    row_0 = row_0.normalize();

    //
    // Compute Y scale factor and normalize second row.
    //
    scale[1] = row_1.length();
    row_1 = row_1.normalize();

    //
    // Compute Z scale factor and normalize third row.
    //
    scale[2] = row_2.length();
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
    float quat[4];
    build_quaternion(tmp_matrix,quat);

    //
    // now convert back to axis/angle.
    //
    float axisAngle[4];
    quat_to_axis(quat,axisAngle);
    rot = rotation(axisAngle);
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
 * Some portions are taken from Graphics Gems 2.
 *
 * @param translation   output translation.
 * @param rot           output rotation.
 * @param scale         output scale.
 * @param shear         output shear.
 */
void VrmlMatrix::getTransform(vec3f & translation,
                              rotation & rot,
                              vec3f & scale,
                              vec3f & shear) const
    throw ()
{
    float tmp_matrix[4][4];
    int i, j;

    //
    // Check if it is singular.
    //
    assert(!fpzero(matrix[3][3]));
    if(fpzero(det3(0, 1, 2, 0, 1, 2))){
        OPENVRML_PRINT_MESSAGE_("Warning : Matrix is singular");
        return;
    }
    std::copy(&this->matrix[0][0], &this->matrix[0][0] + 16, &tmp_matrix[0][0]);

    //
    // Ignore perspective.
    //
    tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;

    //
    // Normalize the matrix.
    //
    for (i = 0; i < 4; i++) {
        for (j=0; j < 4; j++) {
            tmp_matrix[i][j] /= tmp_matrix[3][3];
        }
    }
    translation.x(tmp_matrix[3][0]);
    translation.y(tmp_matrix[3][1]);
    translation.z(tmp_matrix[3][2]);
    tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
    vec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
    vec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
    vec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);

    //
    // Compute X scale factor and normalize first row.
    //
    scale[0] = row_0.length();
    row_0 = row_0.normalize();

    //
    // Compute XY shear factor and make 2nd row orthogonal to 1st.
    //
    float shear_xy = row_0.dot(row_1);
    row_1[0] = row_1[0] - row_0[0]*shear_xy;
    row_1[1] = row_1[1] - row_0[1]*shear_xy;
    row_1[2] = row_1[2] - row_0[2]*shear_xy;

    //
    // Compute Y scale factor and normalize second row.
    //
    scale[1] = row_1.length();
    row_1 = row_1.normalize();
    shear[0] = shear_xy /scale[1];

    //
    // Compute XZ and YZ shears, orthogonalize third row.
    //
    float shear_xz = row_0.dot(row_2);
    row_2[0] = row_2[0] - row_0[0]*shear_xz;
    row_2[1] = row_2[1] - row_0[1]*shear_xz;
    row_2[2] = row_2[2] - row_0[2]*shear_xz;
    float shear_yz = row_1.dot(row_2);
    row_2[0] = row_2[0] - row_1[0]*shear_yz;
    row_2[1] = row_2[1] - row_1[1]*shear_yz;
    row_2[2] = row_2[2] - row_1[2]*shear_yz;

    //
    // Compute Z scale factor and normalize third row.
    //
    scale[2] = row_2.length();
    row_2 = row_2.normalize();
    shear[1] = shear_xz / scale[2];
    shear[2] = shear_yz / scale[2];
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
    float quat[4];
    build_quaternion(tmp_matrix,quat);

    //
    // Now convert back to axis/angle.
    //
    float axisAngle[4];
    quat_to_axis(quat,axisAngle);
    rot = rotation(axisAngle);
}

namespace {
    /**
     * This is taken from Graphics Gems 2, Page 603 and it is valid for only
     * affine matrix with dimension of 4x4. As here we are storing row-major order,
     * this means the last column ***MUST** be [0 0 0 1]
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
     * It returns without doing any calculations if the input matrix is singular.
     *
     * @param in   3D affine matrix.
     *
     * @retval out  inverse of 3D affine matrix.
     */
    void get_affine_inverse(const VrmlMatrix & in, VrmlMatrix & out) {
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

        if(det*det < PRECISION_LIMIT){
            OPENVRML_PRINT_MESSAGE_("Warning : Matrix is singular");
            return;
        }

        // Calculate inverse(A) = adj(A) / det(A)

        det = 1.0 / det;
        out[0][0] =  (in[1][1] * in[2][2] - in[1][2] * in[2][1]) * det;
        out[1][0] = -(in[1][0] * in[2][2] - in[1][2] * in[2][0]) * det;
        out[2][0] =  (in[1][0] * in[2][1] - in[1][1] * in[2][0]) * det;
        out[0][1] = -(in[0][1] * in[2][2] - in[0][2] * in[2][1]) * det;
        out[1][1] =  (in[0][0] * in[2][2] - in[0][2] * in[2][0]) * det;
        out[2][1] = -(in[0][0] * in[2][1] - in[0][1] * in[2][0]) * det;
        out[0][2] =  (in[0][1] * in[1][2] - in[0][2] * in[1][1]) * det;
        out[1][2] = -(in[0][0] * in[1][2] - in[0][2] * in[1][0]) * det;
        out[2][2] =  (in[0][0] * in[1][1] - in[0][1] * in[1][0]) * det;

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
const VrmlMatrix VrmlMatrix::affine_inverse() const throw ()
{
    VrmlMatrix out;
    get_affine_inverse(*this, out);
    return out;
}

/**
 * @brief Get the transposition of the matrix.
 *
 * @return a transposed copy of the matrix.
 */
const VrmlMatrix VrmlMatrix::transpose() const throw ()
{
    return VrmlMatrix(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                      matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                      matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
                      matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
}

/**
 * @brief Postmultiplies matrix by given matrix on the right.
 *
 * @param mat   a matrix.
 *
 * @return the result matrix.
 */
const VrmlMatrix VrmlMatrix::multRight(const VrmlMatrix & mat) const throw ()
{
    VrmlMatrix tmp;

#define POSTMULT(i,j) (matrix[i][0]*mat.matrix[0][j] + \
                       matrix[i][1]*mat.matrix[1][j] + \
                       matrix[i][2]*mat.matrix[2][j] + \
                       matrix[i][3]*mat.matrix[3][j])

    tmp[0][0] = POSTMULT(0,0);
    tmp[0][1] = POSTMULT(0,1);
    tmp[0][2] = POSTMULT(0,2);
    tmp[0][3] = POSTMULT(0,3);
    tmp[1][0] = POSTMULT(1,0);
    tmp[1][1] = POSTMULT(1,1);
    tmp[1][2] = POSTMULT(1,2);
    tmp[1][3] = POSTMULT(1,3);
    tmp[2][0] = POSTMULT(2,0);
    tmp[2][1] = POSTMULT(2,1);
    tmp[2][2] = POSTMULT(2,2);
    tmp[2][3] = POSTMULT(2,3);
    tmp[3][0] = POSTMULT(3,0);
    tmp[3][1] = POSTMULT(3,1);
    tmp[3][2] = POSTMULT(3,2);
    tmp[3][3] = POSTMULT(3,3);

#undef POSTMULT

    return tmp;
}

/**
 * @brief Premultiplies matrix by given matrix on the left.
 *
 * @param mat   a matrix.
 *
 * @return the result matrix.
 */
const VrmlMatrix VrmlMatrix::multLeft(const VrmlMatrix & mat) const throw ()
{
    VrmlMatrix tmp;

#define PREMULT(i,j) (mat.matrix[i][0]*matrix[0][j] + \
                      mat.matrix[i][1]*matrix[1][j] + \
                      mat.matrix[i][2]*matrix[2][j] + \
                      mat.matrix[i][3]*matrix[3][j])

    tmp[0][0] = PREMULT(0,0);
    tmp[0][1] = PREMULT(0,1);
    tmp[0][2] = PREMULT(0,2);
    tmp[0][3] = PREMULT(0,3);
    tmp[1][0] = PREMULT(1,0);
    tmp[1][1] = PREMULT(1,1);
    tmp[1][2] = PREMULT(1,2);
    tmp[1][3] = PREMULT(1,3);
    tmp[2][0] = PREMULT(2,0);
    tmp[2][1] = PREMULT(2,1);
    tmp[2][2] = PREMULT(2,2);
    tmp[2][3] = PREMULT(2,3);
    tmp[3][0] = PREMULT(3,0);
    tmp[3][1] = PREMULT(3,1);
    tmp[3][2] = PREMULT(3,2);
    tmp[3][3] = PREMULT(3,3);

#undef PREMULT

    return tmp;

}

/**
 * @brief Postmultiplies matrix by a column vector on the right.
 *
 * @param src   a column vector.
 *
 * @retval dst  the result vector.
 */
void VrmlMatrix::multMatrixVec(const vec3f & src, vec3f & dst) const
    throw ()
{
    float x, y, z, w;

    x = this->matrix[0][0] * src[0] + this->matrix[0][1] * src[1]
        + this->matrix[0][2] * src[2] + this->matrix[0][3];
    y = this->matrix[1][0] * src[0] + this->matrix[1][1] * src[1]
        + this->matrix[1][2] * src[2] + this->matrix[1][3];
    z = this->matrix[2][0] * src[0] + this->matrix[2][1] * src[1]
        + this->matrix[2][2] * src[2] + this->matrix[2][3];
    w = this->matrix[3][0] * src[0] + this->matrix[3][1] * src[1]
        + this->matrix[3][2] * src[2] + this->matrix[3][3];
    dst.x(x / w);
    dst.y(y / w);
    dst.z(z / w);
}

/**
 * @brief Premultiplies matrix by given row vector on left
 *
 * @param src   a row vector.
 *
 * @retval dst  the result vector.
 */
void VrmlMatrix::multVecMatrix(const vec3f & src, vec3f & dst) const
    throw ()
{
    float x, y, z, w;

    x = src[0]*matrix[0][0] + src[1]*matrix[1][0] +
        src[2]*matrix[2][0] + matrix[3][0];
    y = src[0]*matrix[0][1] + src[1]*matrix[1][1] +
        src[2]*matrix[2][1] + matrix[3][1];
    z = src[0]*matrix[0][2] + src[1]*matrix[1][2] +
        src[2]*matrix[2][2] + matrix[3][2];
    w = src[0]*matrix[0][3] + src[1]*matrix[1][3] +
        src[2]*matrix[2][3] + matrix[3][3];
    dst.x(x / w);
    dst.y(y / w);
    dst.z(z / w);
}

/**
 * @brief Return determinant of 3x3 submatrix by given row and column indices.
 *
 * @param r1    row index.
 * @param r2    row index.
 * @param r3    row index.
 * @param c1    column index.
 * @param c2    column index.
 * @param c3    column index.
 *
 * @return the determinant.
 */
float VrmlMatrix::det3(int r1, int r2, int r3, int c1, int c2, int c3) const
    throw ()
{
    return (  matrix[r1][c1] * matrix[r2][c2] * matrix[r3][c3]
            - matrix[r1][c1] * matrix[r2][c3] * matrix[r3][c2]
            + matrix[r1][c2] * matrix[r2][c3] * matrix[r3][c1]
            - matrix[r1][c2] * matrix[r2][c1] * matrix[r3][c3]
            + matrix[r1][c3] * matrix[r2][c1] * matrix[r3][c2]
            - matrix[r1][c3] * matrix[r2][c2] * matrix[r3][c1]);
}

/**
 * @brief Return determinant of entire matrix.
 *
 * @return the determinant.
 */
float VrmlMatrix::det4() const throw ()
{
    return (  matrix[0][0] * det3(1, 2, 3, 1, 2, 3)
            + matrix[0][1] * det3(1, 2, 3, 0, 2, 3)
            + matrix[0][2] * det3(1, 2, 3, 0, 1, 3)
            + matrix[0][3] * det3(1, 2, 3, 0, 1, 2));
}

/**
 * @fn float (&VrmlMatrix::operator[](size_t index) throw ())[4]
 *
 * @brief To make it usual matrix (non-const)
 */

/**
 * @fn float (&VrmlMatrix::operator[](size_t index) const throw ())[4]
 *
 * @brief To make it usual matrix (const)
 */

} // namespace OpenVRML
