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

#include "field.h"
#include "VrmlMatrix.h"
# include "private.h"

namespace OpenVRML {

using namespace OpenVRML_;

/**
 * @brief Equality comparison operator.
 *
 * All componenents must match exactly.
 *
 * @param m1 is given VrmlMatrix object
 * @param m2 is given VrmlMatrix object
 */
bool operator==(const VrmlMatrix &m1,
                const VrmlMatrix &m2) {
    return (
            m1.matrix[0][0] == m2.matrix[0][0] &&
            m1.matrix[0][1] == m2.matrix[0][1] &&
            m1.matrix[0][2] == m2.matrix[0][2] &&
            m1.matrix[0][3] == m2.matrix[0][3] &&

            m1.matrix[1][0] == m2.matrix[1][0] &&
            m1.matrix[1][1] == m2.matrix[1][1] &&
            m1.matrix[1][2] == m2.matrix[1][2] &&
            m1.matrix[1][3] == m2.matrix[1][3] &&

            m1.matrix[2][0] == m2.matrix[2][0] &&
            m1.matrix[2][1] == m2.matrix[2][1] &&
            m1.matrix[2][2] == m2.matrix[2][2] &&
            m1.matrix[2][3] == m2.matrix[2][3] &&

            m1.matrix[3][0] == m2.matrix[3][0] &&
            m1.matrix[3][1] == m2.matrix[3][1] &&
            m1.matrix[3][2] == m2.matrix[3][2] &&
            m1.matrix[3][3] == m2.matrix[3][3]
           );
}


/**
 * Macro for checking is a matrix is idenity?
 */

#define IS_IDENTITY(matrix) ( \
    (matrix[0][0] == 1.0) && \
    (matrix[0][1] == 0.0) && \
    (matrix[0][2] == 0.0) && \
    (matrix[0][3] == 0.0) && \
    (matrix[1][0] == 0.0) && \
    (matrix[1][1] == 1.0) && \
    (matrix[1][2] == 0.0) && \
    (matrix[1][3] == 0.0) && \
    (matrix[2][0] == 0.0) && \
    (matrix[2][1] == 0.0) && \
    (matrix[2][2] == 1.0) && \
    (matrix[2][3] == 0.0) && \
    (matrix[3][0] == 0.0) && \
    (matrix[3][1] == 0.0) && \
    (matrix[3][2] == 0.0) && \
    (matrix[3][3] == 1.0))

/**
 * @class OpenVRML::VrmlMatrix
 *
 * @brief A class for all matrix operations.
 *
 * Matrices are stored in row-major order. Some parts are taken from MathUtils
 * (thanks to Chris Morley) but format is not same.
 */

/**
 * @brief Construct VrmlMatrix with given 16 elements in row-major order.
 *
 * @param aXX are 16 elements in row-major order
 *
 */
VrmlMatrix::VrmlMatrix(float a11, float a12, float a13, float a14,
                       float a21, float a22, float a23, float a24, 
                       float a31, float a32, float a33, float a34, 
                       float a41, float a42, float a43, float a44)
{
    matrix[0][0] = a11;
    matrix[0][1] = a12;
    matrix[0][2] = a13;
    matrix[0][3] = a14;

    matrix[1][0] = a21;
    matrix[1][1] = a22;
    matrix[1][2] = a23;
    matrix[1][3] = a24;

    matrix[2][0] = a31;
    matrix[2][1] = a32;
    matrix[2][2] = a33;
    matrix[2][3] = a34;

    matrix[3][0] = a41;
    matrix[3][1] = a42;
    matrix[3][2] = a43;
    matrix[3][3] = a44;
}

/**
 * Construct VrmlMatrix with given 4x4 array of elements in row-major order
 * @param m is input array
 *
 */

VrmlMatrix::VrmlMatrix(const Matrix m)
{
    memcpy(matrix, m, sizeof(float[4][4]));
}

/**
 * @brief Sets value with given 4x4 array of elements in row-major order
 * @param m is input array
 *
 */

void
VrmlMatrix::set(const Matrix m)
{
    memcpy(matrix, m, sizeof(float[4][4]));
}

/**
 * @brief Assignment operator.
 *
 * @param rhs is the 4x4 array of elements to copy into this object
 */

VrmlMatrix &
VrmlMatrix::operator =(const Matrix m)
{
    set(m);
    return *this;
}

/**
 * @brief Assignment operator.
 *
 * @param rhs the VrmlMatrix to copy into this object
 */

VrmlMatrix &
VrmlMatrix::operator =(const VrmlMatrix &m)
{
    memcpy(matrix, m.matrix, sizeof(float[4][4]));
    return *this;
}

/**
 * @brief Sets matrix to rotate by given rotation Graphics Gems p466
 * @param axisAngle[X Y Z angle] should be in normalized form
 */

void
VrmlMatrix::setRotate(const float axisAngle[4])
{
 // Make sure axis is normalized.

   assert(fpequal(length(axisAngle), 1.0));

   float aa_norm[4];
   aa_norm[0] = axisAngle[0];
   aa_norm[1] = axisAngle[1];
   aa_norm[2] = axisAngle[2];
   aa_norm[3] = axisAngle[3];

   double s = sin(aa_norm[3]);
   double c = cos(aa_norm[3]);
   double t = 1.0 - c;
   double x = aa_norm[0];
   double y = aa_norm[1];
   double z = aa_norm[2];

   matrix[0][0] = t*x*x + c;
   matrix[1][0] = t*x*y - s*z;
   matrix[2][0] = t*x*z + s*y;
   matrix[3][0] = 0.0;
   matrix[0][1] = t*x*y + s*z;
   matrix[1][1] = t*y*y + c;
   matrix[2][1] = t*y*z - s*x;
   matrix[3][1] = 0.0;
   matrix[0][2] = t*x*z - s*y;
   matrix[1][2] = t*y*z + s*x;
   matrix[2][2] = t*z*z + c;
   matrix[3][2] = 0.0;
   matrix[0][3] = matrix[1][3] = matrix[2][3] = 0.0;
   matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to rotate by given rotation Graphics Gems p466
 * @param axisAngle is the SFRotation object. This should be in normalized form
 */

void
VrmlMatrix::setRotate(const SFRotation &axisAngle)
{
   float aa_norm[4];

   aa_norm[0] = axisAngle.getX();
   aa_norm[1] = axisAngle.getY();
   aa_norm[2] = axisAngle.getZ();
   aa_norm[3] = axisAngle.getAngle();
   setRotate(aa_norm);
}

/**
 * @brief Sets matrix to scale by given uniform factor
 * @param s is uniform scale factor
 *
 */

void
VrmlMatrix::setScale(float s)
{
    matrix[0][0] = s;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = s;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = s;
    matrix[2][3] = 0.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

/**
 * @brief Sets matrix to scale by given vector in array[X Y Z}
 * @param s is a given vector
 *
 */

void 
VrmlMatrix::setScale(const float s[3])
{
  makeIdentity();
  for (int i=0; i<3; ++i)
    matrix[i][i] = s[i];
}

/**
 * @brief Sets matrix to scale by given SFVec3f object
 * @param s is a given vector object
 *
 */
void
VrmlMatrix::setScale(const SFVec3f &s)
{
  makeIdentity();
  for (int i=0; i<3; ++i)
    matrix[i][i] = s[i];
}

/**
 * @brief Sets matrix to translate by given vector in array[X Y Z]
 * @param t is a given vector 
 *
 */
void
VrmlMatrix::setTranslate(const float t[3])
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
 * @brief Sets matrix to translate by given SFVec3f object
 * @param t is a given vector object
 *
 */
void
VrmlMatrix::setTranslate(const SFVec3f &t)
{
    setTranslate(t.get());
}

/**
 * @brief Returns copy 4x4 array of elements 
 * @param m is output of 4x4 elements
 *
 */

void
VrmlMatrix::get(Matrix m) const
{
   memcpy(m, matrix, sizeof(float[4][4]));
}

/**
 * @brief Sets matrix to be identity
 */
void
VrmlMatrix::makeIdentity()
{
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      matrix[i][j] = (i == j) ? 1.0 : 0.0;
}


/**
 * @brief Postmultiplies matrix by given matrix on right 
 * @param m is given VrmlMatrix object
 *
 */

const VrmlMatrix 
VrmlMatrix::multRight(const VrmlMatrix &m)
{
    // Trivial cases
    if (IS_IDENTITY(m))
      return *this;
    else if (IS_IDENTITY(matrix))
      return (*this = m);
	
    Matrix    tmp;

#define POSTMULT(i,j) (matrix[i][0]*m.matrix[0][j] + \
                       matrix[i][1]*m.matrix[1][j] + \
                       matrix[i][2]*m.matrix[2][j] + \
                       matrix[i][3]*m.matrix[3][j])

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

    return (VrmlMatrix(tmp));
}

/**
 * @brief Premultiplies matrix by given matrix on left 
 * @param m is given VrmlMatrix object
 *
 */

const VrmlMatrix 
VrmlMatrix::multLeft(const VrmlMatrix &m)
{
    // Trivial cases
    if (IS_IDENTITY(m))
        return *this;
    else if (IS_IDENTITY(matrix))
        return (*this = m);
	
    Matrix    tmp;

#define PREMULT(i,j) (m.matrix[i][0]*matrix[0][j] + \
                      m.matrix[i][1]*matrix[1][j] + \
                      m.matrix[i][2]*matrix[2][j] + \
                      m.matrix[i][3]*matrix[3][j])

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

    return (VrmlMatrix(tmp));

}

/**
 * @brief Postmultiplies matrix by given column vector on right 
 * @param in src is given vector in array
 * @param out dst is output of result vector in an array 
 */

void
VrmlMatrix::multMatrixVec(const float src[3], float dst[3]) const
{
    float    x,y,z,w;
    
    x = matrix[0][0]*src[0] + matrix[0][1]*src[1] +
        matrix[0][2]*src[2] + matrix[0][3];
    y = matrix[1][0]*src[0] + matrix[1][1]*src[1] +
        matrix[1][2]*src[2] + matrix[1][3];
    z = matrix[2][0]*src[0] + matrix[2][1]*src[1] +
        matrix[2][2]*src[2] + matrix[2][3];
    w = matrix[3][0]*src[0] + matrix[3][1]*src[1] +
        matrix[3][2]*src[2] + matrix[3][3];
    dst[0] = x/w;
    dst[1] = y/w;
    dst[2] = z/w;
}

/**
 * @brief Postmultiplies matrix by given column vector on right 
 * @param in src is given SFVec3f object
 * @param out dst is output of result SFVec3f object
 */
void
VrmlMatrix::multMatrixVec(const SFVec3f &src, SFVec3f &dst) const
{
    float   sv[3],dv[3];
    sv[0] = src[0];
    sv[1] = src[1];
    sv[2] = src[2];
    multMatrixVec(sv , dv);
    dst[0] = dv[0];
    dst[1] = dv[1];
    dst[2] = dv[2];
}

/**
 * @brief Premultiplies matrix by given row vector on left 
 * @param in src is given vector in array
 * @param out dst is output of result vector in an array 
 */

void
VrmlMatrix::multVecMatrix(const float src[3], float dst[3]) const
{
    float    x,y,z,w;
    
    x = src[0]*matrix[0][0] + src[1]*matrix[1][0] +
        src[2]*matrix[2][0] + matrix[3][0];
    y = src[0]*matrix[0][1] + src[1]*matrix[1][1] +
        src[2]*matrix[2][1] + matrix[3][1];
    z = src[0]*matrix[0][2] + src[1]*matrix[1][2] +
        src[2]*matrix[2][2] + matrix[3][2];
    w = src[0]*matrix[0][3] + src[1]*matrix[1][3] +
        src[2]*matrix[2][3] + matrix[3][3];
    dst[0] = x/w;
    dst[1] = y/w;
    dst[2] = z/w;
}

/**
 * @brief Premultiplies matrix by given row vector on left 
 * @param in src is given SFVec3f object
 * @param out dst is output of result SFVec3f object
 */
void
VrmlMatrix::multVecMatrix(const SFVec3f &src, SFVec3f &dst) const
{
    float   sv[3],dv[3];
    sv[0] = src[0];
    sv[1] = src[1];
    sv[2] = src[2];
    multVecMatrix(sv , dv);
    dst[0] = dv[0];
    dst[1] = dv[1];
    dst[2] = dv[2];
}

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
 *  It aborts if input matrix is singular and not affine
 *  @param in - 3D affine matrix
 *  @param out - inverse of 3D affine matrix
 
*/
namespace
{
void get_affine_inverse(const float in[4][4], float out[4][4]) {
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

    assert (det*det > PRECISION_LIMIT);

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
const VrmlMatrix
VrmlMatrix::affine_inverse() const
{
    float in[4][4],out[4][4];
    memcpy(in, matrix, sizeof(float[4][4]));
    get_affine_inverse(in,out);
    return VrmlMatrix(out);
}

/**
 * @brief Return determinant of 3X3 submatrix by given
 * row and column indices
 * @param in r1, r2, r3 are row indices c1, c2, c3 are column indices
 * @param out is return value of determinant
 */

float
VrmlMatrix::det3(int r1, int r2, int r3, int c1, int c2, int c3) const
{
  return (  matrix[r1][c1] * matrix[r2][c2] * matrix[r3][c3]
          - matrix[r1][c1] * matrix[r2][c3] * matrix[r3][c2]
          + matrix[r1][c2] * matrix[r2][c3] * matrix[r3][c1]
          - matrix[r1][c2] * matrix[r2][c1] * matrix[r3][c3]
          + matrix[r1][c3] * matrix[r2][c1] * matrix[r3][c2]
          - matrix[r1][c3] * matrix[r2][c2] * matrix[r3][c1]);
}

/**
 * @brief Return determinant of entire matrix
 * @param out is return value of determinant
 */

float
VrmlMatrix::det4() const
{
  return (  matrix[0][0] * det3(1, 2, 3, 1, 2, 3)
          + matrix[0][1] * det3(1, 2, 3, 0, 2, 3)
          + matrix[0][2] * det3(1, 2, 3, 0, 1, 3)
          + matrix[0][3] * det3(1, 2, 3, 0, 1, 2));
}


/**
 * @brief Prints a formatted version of the matrix to the 
 * given output stream.
 * @param o is output streams
 */

ostream& 
VrmlMatrix::print(ostream& o)
{
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      o << matrix[i][j];
      if (j!=3) cout << ",";
    }
    o << endl;
  }
  return o;
}

/*
 * @brief To make it usual matrix (non-const)
 */

float
( & VrmlMatrix::operator [](int i))[4]
{
   assert(i < 4);
   return matrix[i]; 
}

/*
 * @brief To make it usual matrix (const)
 */

const float
( & VrmlMatrix::operator [](int i) const)[4]
{
   assert(i < 4);
   return matrix[i]; 
}

/**
 * @brief Set the matrix from  translation, rotation, scale,
 * scaleOrientation and center
 * @param in translation, rotation, scale, scaleOrientation and center as 
 * specified in VrmlNodeTransform node
 */
void
VrmlMatrix::setTransform(const SFVec3f & translation,
                         const SFRotation & rotation,
                         const SFVec3f & scale,
                         const SFRotation & scaleOrientation,
                         const SFVec3f & center)
{
    VrmlMatrix temp;
	makeIdentity();
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
    if(!((rotation.getX() == 0.0)
            && (rotation.getY() == 0.0)
            && (rotation.getZ() == 1.0)
            && (rotation.getAngle() == 0.0))) {
        temp.setRotate(rotation);
        *this = multLeft(temp);            // M = R * M    = R * C * T
    }
    if(!((scale[0] == 1.0)
            && (scale[1] == 1.0)
            && (scale[2] == 1.0))) {
        if(!((scaleOrientation.getX() == 0.0)
                && (scaleOrientation.getY() == 0.0)
                && (scaleOrientation.getZ() == 1.0)
                && (scaleOrientation.getAngle() == 0.0))) {
            temp.setRotate(scaleOrientation);
            *this = multLeft(temp);            // M = SR * M    = SR * R * C * T 
        }
        temp.setScale(scale);
        *this = multLeft(temp);            // M = S * M     = S * SR * R * C * T
        if(!((scaleOrientation.getX() == 0.0)
                && (scaleOrientation.getY() == 0.0)
                && (scaleOrientation.getZ() == 1.0)
                && (scaleOrientation.getAngle() == 0.0))) {
            temp.setRotate(scaleOrientation.inverse());
            *this = multLeft(temp);            // M = -SR * M   = -SR * S * SR * R * C * T
        }
    }
    if(!((center[0] == 0.0)
            && (center[1] == 0.0)
            && (center[2] == 0.0))) {
       temp.setTranslate(center.negate());
       *this = multLeft(temp);            // M = -C * M    =  -C * -SR * S * SR * R * C * T
    }
}

/**
 * @brief Build a quaternion rotation , given a rotation matrix.
 * @param in Orthogonal Matrix.
 * @param out quaternion rotation.
 */
namespace
{ 
void build_quaternion(const float m[4][4], float quat[4])
{
  float  diagonal, s, q[4];
  int    i, j, k;
  int next[3] = {1, 2, 0};
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
      if (m[1][1] > m[0][0]) i = 1;
      if (m[2][2] > m[i][i]) i = 2;
      j = next[i];
      k = next[j];
      s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
      q[i] = s * 0.5;
 
      if (s != 0.0) s = 0.5 / s;
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
 * @brief Convert a quaternion to a rotation axis and angle
 * @param in quaternion
 * @param out rotation axis in normalize form and angle.
 */
void quat_to_axis(const float q[4], float axisAngle[4])
{
  float val = acos(q[3]);
  if (fpzero(val))
  {
    axisAngle[0] = 0.0;
    axisAngle[1] = 1.0;
    axisAngle[2] = 0.0;
    axisAngle[3] = 0.0;
  }
  else
  {
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
 * @brief get translation, rotation and scale from orthogonal
 * transformation matrix. Here any projection or shear information in the
 * matrix is ignored. The caller is responsible for assuring that the 
 * transformation is in fact orthogonal, otherwise the results are 
 * undefined. 
 * Some portions are taken from Graphics Gems 2
 * @param out translation, rotation and scale.
 */
void
VrmlMatrix::getTransform(SFVec3f & translation,
                         SFRotation & rotation,
                         SFVec3f & scale) const
{
   float tmp_matrix[4][4];
   int i,j;
// Check if it is singular
   assert(!fpzero(matrix[3][3]));
   assert(!fpzero(det3(0, 1, 2, 0, 1, 2)));
   memcpy(tmp_matrix, matrix, sizeof(float[4][4]));
// Ignore Perspective 
   tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;
// Normalize the matrix
   for (i=0; i<4;i++ )
     for (j=0; j<4; j++ )
       tmp_matrix[i][j] /= tmp_matrix[3][3];
   translation.setX(tmp_matrix[3][0]);
   translation.setY(tmp_matrix[3][1]);
   translation.setZ(tmp_matrix[3][2]); 
   tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
   SFVec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
   SFVec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
   SFVec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);
// Compute X scale factor and normalize first row.
   scale[0] = row_0.length();
   row_0 = row_0.normalize();
// Compute Y scale factor and normalize second row.
   scale[1] = row_1.length();
   row_1 = row_1.normalize();
// Compute Z scale factor and normalize third row.
   scale[2] = row_2.length();
   row_2 = row_2.normalize();
   for (j=0; j<3; j++){
     tmp_matrix[0][j] = row_0[j];
     tmp_matrix[1][j] = row_1[j];
     tmp_matrix[2][j] = row_2[j];
   }
// At this point, the matrix (in rows[]) is orthonormal. 
// That is, each row has length one and are mutually perpendicular.
// Is it needed to calculate the det of this matrix (1 or -1) for checking 
// coordinate system flip ? (According to VRML standard, scale > 0.0)
// Calculate quaternion rotation from this matrix.
   float quat[4];
   build_quaternion(tmp_matrix,quat);
// now convert back to axis/angle
   float axisAngle[4];
   quat_to_axis(quat,axisAngle);
   rotation.set(axisAngle);
}

/**
 * @brief get translation, rotation, scale and shear from affine
 * transformation VrmlMatrix. Here any projection information in
 * matrix is ignored. As VRML allows non-uniform scaling, it is safe to
 * call this routine. The caller is responsible for assuring that the 
 * transformation is in fact affine, otherwise the results are 
 * undefined.  
 * Some portions are taken from Graphics Gems 2
 * @param out translation, rotation, scale and shear.
 */
void
VrmlMatrix::getTransform(SFVec3f & translation,
                         SFRotation & rotation,
                         SFVec3f & scale,
                         SFVec3f & shear) const
{
   float tmp_matrix[4][4];
   int i,j;
// Check if it is singular
   assert(!fpzero(matrix[3][3]));
   assert(!fpzero(det3(0, 1, 2, 0, 1, 2)));
   memcpy(tmp_matrix, matrix, sizeof(float[4][4]));
// Ignore Perspective 
   tmp_matrix[0][3] = tmp_matrix[1][3] = tmp_matrix[2][3] = 0.0;
// Normalize the matrix
   for (i=0; i<4;i++ )
     for (j=0; j<4; j++ )
       tmp_matrix[i][j] /= tmp_matrix[3][3];
   translation.setX(tmp_matrix[3][0]);
   translation.setY(tmp_matrix[3][1]);
   translation.setZ(tmp_matrix[3][2]); 
   tmp_matrix[3][0] = tmp_matrix[3][1] = tmp_matrix[3][2] = 0.0;
   SFVec3f row_0(tmp_matrix[0][0], tmp_matrix[0][1], tmp_matrix[0][2]);
   SFVec3f row_1(tmp_matrix[1][0], tmp_matrix[1][1], tmp_matrix[1][2]);
   SFVec3f row_2(tmp_matrix[2][0], tmp_matrix[2][1], tmp_matrix[2][2]);
// Compute X scale factor and normalize first row.
   scale[0] = row_0.length();
   row_0 = row_0.normalize();
// Compute XY shear factor and make 2nd row orthogonal to 1st.
   float shear_xy = row_0.dot(row_1);
   row_1[0] = row_1[0] - row_0[0]*shear_xy;
   row_1[1] = row_1[1] - row_0[1]*shear_xy;
   row_1[2] = row_1[2] - row_0[2]*shear_xy;
// Compute Y scale factor and normalize second row.
   scale[1] = row_1.length();
   row_1 = row_1.normalize();
   shear[0] = shear_xy /scale[1];
// Compute XZ and YZ shears, orthogonalize third row. 
   float shear_xz = row_0.dot(row_2);
   row_2[0] = row_2[0] - row_0[0]*shear_xz;
   row_2[1] = row_2[1] - row_0[1]*shear_xz;
   row_2[2] = row_2[2] - row_0[2]*shear_xz;
   float shear_yz = row_1.dot(row_2);
   row_2[0] = row_2[0] - row_1[0]*shear_yz;
   row_2[1] = row_2[1] - row_1[1]*shear_yz;
   row_2[2] = row_2[2] - row_1[2]*shear_yz;
// Compute Z scale factor and normalize third row.
   scale[2] = row_2.length();
   row_2 = row_2.normalize();
   shear[1] = shear_xz / scale[2];
   shear[2] = shear_yz / scale[2];
   for (j=0; j<3; j++){
     tmp_matrix[0][j] = row_0[j];
     tmp_matrix[1][j] = row_1[j];
     tmp_matrix[2][j] = row_2[j];
   }
// At this point, the matrix (in rows[]) is orthonormal. 
// That is, each row has length one and are mutually perpendicular.
// Is it needed to calculate the det of this matrix (1 or -1) for checking 
// coordinate system flip ? (According to VRML standard, scale > 0.0)
// Calculate quaternion rotation from this matrix.
   float quat[4];
   build_quaternion(tmp_matrix,quat);
// now convert back to axis/angle
   float axisAngle[4];
   quat_to_axis(quat,axisAngle);
   rotation.set(axisAngle);
}

/**
 * Returns transpose of matrix
 */

const VrmlMatrix
VrmlMatrix::transpose() const
{
   return VrmlMatrix(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                     matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                     matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
                     matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
}

} // namespace OpenVRML
