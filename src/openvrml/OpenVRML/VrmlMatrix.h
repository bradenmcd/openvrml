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
 * A class for all matrix operations
 * Plase Notes: Matrices are stored in row-major order
 */

 
#ifndef VRMLMATRIX_H
#define VRMLMATRIX_H

#include <iostream.h>

typedef float Matrix[4][4];

class VrmlSFVec3f;
class VrmlSFRotation;

class VrmlMatrix {
  public:

    // Default constructor
    VrmlMatrix() {}

    // Constructor wit a given all 16 elements in row-major order
    VrmlMatrix(float a11, float a12, float a13, float a14,
               float a21, float a22, float a23, float a24, 
               float a31, float a32, float a33, float a34, 
               float a41, float a42, float a43, float a44);

    // Constructor from a 4x4 array of elements
    VrmlMatrix(const Matrix m);

    // returns pointer to storage of first element
    operator float *() 	{ return &matrix[0][0]; }

    // To make it usual matrix 
    float *       operator [](int i) 	   { return &matrix[i][0]; }
    const float * operator [](int i) const  { return &matrix[i][0]; }

    // returns reference to 4x4 array
    operator Matrix &()    { return matrix; }

    // Sets value from 4x4 array of elements
    VrmlMatrix &  operator =(const Matrix m);
    VrmlMatrix &  operator =(const VrmlMatrix &m);

    // Sets value from a rotation
    VrmlMatrix &  operator =(const VrmlSFRotation &r)
    { setRotate(r); return *this; }


    // Performs postmultiplication with another matrix
    VrmlMatrix 	operator *=(const VrmlMatrix &m)	{ return MMright(m); }

    // Sets matrix to rotate by given rotation about axis/angle 
    void    setRotate(const float axisAngle[4]);
    void    setRotate(const VrmlSFRotation &axisAngle);

    // Sets matrix to scale by given uniform factor
    void    setScale(float s);

    // Sets matrix to scale by given vector

    void    setScale(const float s[3]);
    void    setScale(const VrmlSFVec3f &s);

    // Sets matrix to translate by given vector

    void     setTranslate(const float t[3]);
    void     setTranslate(const VrmlSFVec3f &t);


    // Sets value with given 4x4 array of elements
    void     set(const Matrix );


    // Returns 4x4 array of elements
    void     get(Matrix m) const;

    // Returns a reference to 4x4 array
    const Matrix &    get() const { return matrix; }

    void     makeIdentity();

   // Returns an identity matrix 
    static VrmlMatrix   identity();

    // Multiplies matrix by given matrix on right or left
    const VrmlMatrix  MMright(const VrmlMatrix &m);	
    const VrmlMatrix  MMleft(const VrmlMatrix &m);	

    // Return inverse of nonsingular affine matrix
    const VrmlMatrix affine_inverse() const;

    // Multiplies matrix by given column vector, giving vector result
    void        MatrixXVec(const VrmlSFVec3f &src, VrmlSFVec3f &dst) const;
    void        MatrixXVec(const float src[3], float dst[3]) const;

   // Multiplies given row vector by matrix, giving vector result
    void        VecXMatrix(const VrmlSFVec3f &src, VrmlSFVec3f &dst) const;
    void        VecXMatrix(const float src[3], float dst[3]) const;

    // Prints a formatted version of the matrix to the given output stream
       ostream& print(ostream& o);

    // Binary multiplication of matrices
    friend VrmlMatrix   operator *(const VrmlMatrix &m1, const VrmlMatrix &m2);

    // Equality comparison operator
    friend bool        operator ==(const VrmlMatrix &m1, const VrmlMatrix &m2);
    friend bool        operator !=(const VrmlMatrix &m1, const VrmlMatrix &m2)
                       { return !(m1 == m2); }

  private:
    Matrix    matrix;        // Storage for 4x4 matrix

};

#endif // VRMLMATRIX_H
