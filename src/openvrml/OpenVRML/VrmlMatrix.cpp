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

/**
 * @class VrmlMatrix
 * Some parts are taken from MathUtils (thanks to Chris Morley)
 * but format is not same.
 * A class for all matrix operations
 * Plase Notes: Matrices are stored in row-major order
 */

 
#include "field.h"
#include "VrmlMatrix.h"
# include "private.h"

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
 * Construct VrmlMatrix with given 16 elements in row-major order
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
 * @param axisAngle is the VrmlSFRotation object. This should be in normalized form
 */

void
VrmlMatrix::setRotate(const VrmlSFRotation &axisAngle)
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
 * @brief Sets matrix to scale by given VrmlSFVec3f object
 * @param s is a given vector object
 *
 */

void
VrmlMatrix::setScale(const VrmlSFVec3f &s)
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
 * @brief Sets matrix to translate by given VrmlSFVec3f object
 * @param t is a given vector object
 *
 */
void
VrmlMatrix::setTranslate(const VrmlSFVec3f &t)
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
 * @brief Returns an identity matrix
 */

VrmlMatrix
VrmlMatrix::identity()    
{
    return VrmlMatrix(1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 
                      0.0, 0.0, 0.0, 1.0);
}

/**
 * @brief Postmultiplies matrix by given matrix on right 
 * @param m is given VrmlMatrix object
 *
 */

const VrmlMatrix 
VrmlMatrix::MMright(const VrmlMatrix &m)
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
VrmlMatrix::MMleft(const VrmlMatrix &m)
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
VrmlMatrix::MatrixXVec(const float src[3], float dst[3]) const
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
 * @param in src is given VrmlSFVec3f object
 * @param out dst is output of result VrmlSFVec3f object
 */


void
VrmlMatrix::MatrixXVec(const VrmlSFVec3f &src, VrmlSFVec3f &dst) const
{
    float   sv[3],dv[3];
    sv[0] = src[0];
    sv[1] = src[1];
    sv[2] = src[2];
    MatrixXVec(sv , dv);
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
VrmlMatrix::VecXMatrix(const float src[3], float dst[3]) const
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
 * @param in src is given VrmlSFVec3f object
 * @param out dst is output of result VrmlSFVec3f object
 */

void
VrmlMatrix::VecXMatrix(const VrmlSFVec3f &src, VrmlSFVec3f &dst) const
{
    float   sv[3],dv[3];
    sv[0] = src[0];
    sv[1] = src[1];
    sv[2] = src[2];
    VecXMatrix(sv , dv);
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
 *  It aborts if input matrix is nonsingular and affine
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

/**
 * @brief Binary multiplication of matrices 
 * @param left is left matrix object
 * @param right is the right matrix object
 * @output is the resultant matrix
 */

VrmlMatrix
operator *(const VrmlMatrix &left, const VrmlMatrix &right)
{
    VrmlMatrix m = left;
    
    m *= right;
    
    return m;
}

/*
 * @brief Equality comparison operator. All componenents 
 * must match exactly.
 * @param m1 is given VrmlMatrix object
 * @param m2 is given VrmlMatrix object
 */

bool
operator ==(const VrmlMatrix &m1, const VrmlMatrix &m2)
{
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

