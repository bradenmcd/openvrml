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

#ifndef VRMLMATRIX_H
#define VRMLMATRIX_H

#include <iostream.h>

typedef float Matrix[4][4];

namespace OpenVRML {
    class VrmlMatrix;
}

bool operator==(const OpenVRML::VrmlMatrix & m1,
                const OpenVRML::VrmlMatrix & m2);

inline bool operator!=(const OpenVRML::VrmlMatrix & m1,
                       const OpenVRML::VrmlMatrix & m2) {
    return !(m1 == m2);
}


namespace OpenVRML {

    class SFVec3f;
    class SFRotation;

    class VrmlMatrix {
      public:

        // Default constructor
        VrmlMatrix() {makeIdentity();}

        // Constructor wit a given all 16 elements in row-major order
        explicit VrmlMatrix(float a11, float a12, float a13, float a14,
                            float a21, float a22, float a23, float a24, 
                            float a31, float a32, float a33, float a34, 
                            float a41, float a42, float a43, float a44);

        // Constructor from a 4x4 array of elements
        VrmlMatrix(const Matrix m);

        // returns pointer to storage of first element
        operator float *() { return &matrix[0][0]; }

        // To make it usual matrix 
        float        ( &operator [](int i))[4]; 	   
        const float ( &operator [](int i) const)[4];  

        // Sets value from 4x4 array of elements
        VrmlMatrix &  operator =(const Matrix m);
        VrmlMatrix &  operator =(const VrmlMatrix &m);

        // Sets value from a rotation
        VrmlMatrix &  operator=(const SFRotation & r)
        { setRotate(r); return *this; }

        // Sets matrix to rotate by given rotation about axis/angle 
        void    setRotate(const float axisAngle[4]);
        void    setRotate(const SFRotation & axisAngle);

        // Sets matrix to scale by given uniform factor
        void    setScale(float s);

        // Sets matrix to scale by given vector

        void    setScale(const float s[3]);
        void    setScale(const SFVec3f & s);

        // Sets matrix to translate by given vector

        void     setTranslate(const float t[3]);
        void     setTranslate(const SFVec3f & t);


        // Sets value with given 4x4 array of elements
        void     set(const Matrix );

        // Returns 4x4 array of elements
        void     get(Matrix m) const;

        // Returns a reference to 4x4 array
        const Matrix &    get() const { return matrix; }

        void     makeIdentity();

        // Multiplies matrix by given matrix on right or left
        const VrmlMatrix  multRight(const VrmlMatrix &m);	
        const VrmlMatrix  multLeft(const VrmlMatrix &m);	

        // Return inverse of nonsingular affine matrix
        const VrmlMatrix affine_inverse() const;

        // Return the transpose of matrix
        const VrmlMatrix transpose() const;

        // Multiplies matrix by given column vector, giving vector result
        void        multMatrixVec(const SFVec3f &src, SFVec3f &dst) const;
        void        multMatrixVec(const float src[3], float dst[3]) const;

       // Multiplies given row vector by matrix, giving vector result
        void        multVecMatrix(const SFVec3f &src, SFVec3f &dst) const;
        void        multVecMatrix(const float src[3], float dst[3]) const;

        // Set the matrix from translation, rotation, scale, scaleOrientation and center
        void        setTransform(const SFVec3f & translation,
                                 const SFRotation & rotation,
                                 const SFVec3f & scale,
                                 const SFRotation & scaleOrientation,
                                 const SFVec3f & center);

        // Prints a formatted version of the matrix to the given output stream
           ostream& print(ostream& o);

        // Equality comparison operator
        friend bool ::operator==(const VrmlMatrix & m1, const VrmlMatrix & m2);
        friend bool ::operator!=(const VrmlMatrix & m1, const VrmlMatrix & m2);

      private:
        Matrix    matrix;        // Storage for 4x4 matrix

    };
}

#endif // VRMLMATRIX_H
