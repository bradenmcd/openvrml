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

# ifndef OPENVRML_VRMLMATRIX_H
#   define OPENVRML_VRMLMATRIX_H

#   include <assert.h>
#   include <stddef.h>
#   include <iostream.h>
#   include "common.h"

namespace OpenVRML {
    class VrmlMatrix;

    bool operator==(const VrmlMatrix & lhs, const VrmlMatrix & rhs);
    bool operator!=(const VrmlMatrix & lhs, const VrmlMatrix & rhs);

    class SFVec3f;
    class SFRotation;

    class OPENVRML_SCOPE VrmlMatrix {
        friend bool operator==(const VrmlMatrix & lhs, const VrmlMatrix & rhs);
        friend bool operator!=(const VrmlMatrix & lhs, const VrmlMatrix & rhs);
        
        float matrix[4][4];
    
    public:
        VrmlMatrix();
        VrmlMatrix(float f11, float f12, float f13, float f14,
                   float f21, float f22, float f23, float f24, 
                   float f31, float f32, float f33, float f34, 
                   float f41, float f42, float f43, float f44);
        explicit VrmlMatrix(const float m[4][4]);

        // use compiler-defined operator= and copy constructor.
        
        float (&operator[](size_t index))[4];
        const float (&operator[](size_t index) const)[4];

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

        // Returns a reference to 4x4 array
        const float (&get() const)[4][4] { return matrix; }

        // Multiplies matrix by given matrix on right or left
        const VrmlMatrix  multRight(const VrmlMatrix &m);	
        const VrmlMatrix  multLeft(const VrmlMatrix &m);	

        // Return inverse of nonsingular affine matrix
        const VrmlMatrix affine_inverse() const;

        // Return the transpose of matrix
        const VrmlMatrix transpose() const;

        // Return determinant of 3 X 3 submatrix
        float det3(int r1, int r2, int r3, int c1, int c2, int c3) const;

        // Return determinant of matrix
        float det4() const;

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

        //Decomposes VrmlMatrix into translation, rotation and scale.
        void        getTransform(SFVec3f & translation,
                                 SFRotation & rotation,
                                 SFVec3f & scale) const;

        //Decomposes VrmlMatrix into translation, rotation, scale and shear.
        void        getTransform(SFVec3f & translation,
                                 SFRotation & rotation,
                                 SFVec3f & scale,
                                 SFVec3f & shear) const;

        // Prints a formatted version of the matrix to the given output stream
        ostream& print(ostream& o);
    };

    inline bool operator!=(const VrmlMatrix & lhs, const VrmlMatrix & rhs) {
        return !(lhs == rhs);
    }
    
    inline float (&VrmlMatrix::operator[](size_t index))[4] {
        assert(index < 4);
        return this->matrix[index]; 
    }

    inline const float (&VrmlMatrix::operator[](size_t index) const)[4] {
        assert(index < 4);
        return this->matrix[index]; 
    }
}

#endif // OPENVRML_VRMLMATRIX_H
