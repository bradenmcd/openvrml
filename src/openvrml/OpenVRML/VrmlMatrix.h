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
#   include <iostream>
#   include <iosfwd>
#   include <OpenVRML/common.h>

namespace OpenVRML {

    class VrmlMatrix;

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const VrmlMatrix & mat);
    bool OPENVRML_SCOPE operator==(const VrmlMatrix & lhs,
                                   const VrmlMatrix & rhs) throw ();

    class vec3f;
    class rotation;
    class Quaternion;

    class OPENVRML_SCOPE VrmlMatrix {
        float matrix[4][4];

    public:
        VrmlMatrix() throw ();
        VrmlMatrix(float f11, float f12, float f13, float f14,
                   float f21, float f22, float f23, float f24,
                   float f31, float f32, float f33, float f34,
                   float f41, float f42, float f43, float f44) throw ();
        explicit VrmlMatrix(const float m[16]) throw ();
        explicit VrmlMatrix(const float m[4][4]) throw ();

        // use compiler-defined operator= and copy constructor.

        float (&operator[](size_t index) throw ())[4];
        const float (&operator[](size_t index) const throw ())[4];
        const float (&get() const throw ())[4][4];

        void setRotate(const rotation & axisAngle) throw ();
        void setRotate(const Quaternion & quat) throw ();
        void setScale(float s) throw ();
        void setScale(const vec3f & s) throw ();
        void setTranslate(const vec3f & t) throw ();

        void setTransform(const vec3f & translation,
                          const rotation & rotation,
                          const vec3f & scale,
                          const rotation & scaleOrientation,
                          const vec3f & center) throw ();
        void getTransform(vec3f & translation,
                          rotation & rotation,
                          vec3f & scale) const throw ();
        void getTransform(vec3f & translation,
                          rotation & rotation,
                          vec3f & scale,
                          vec3f & shear) const throw ();

        const VrmlMatrix affine_inverse() const throw ();
        const VrmlMatrix transpose() const throw ();

        const VrmlMatrix multLeft(const VrmlMatrix & mat) const throw ();
        const VrmlMatrix multRight(const VrmlMatrix & mat) const throw ();

        void multMatrixVec(const vec3f &src, vec3f &dst) const throw ();
        void multVecMatrix(const vec3f &src, vec3f &dst) const throw ();

        float det3(int r1, int r2, int r3, int c1, int c2, int c3) const
            throw ();
        float det4() const throw ();
    };

    inline bool operator!=(const VrmlMatrix & lhs, const VrmlMatrix & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }

    inline float (&VrmlMatrix::operator[](size_t index) throw ())[4]
    {
        assert(index < 4);
        return this->matrix[index];
    }

    inline const float (&VrmlMatrix::operator[](size_t index) const throw ())[4]
    {
        assert(index < 4);
        return this->matrix[index];
    }

    inline const float (&VrmlMatrix::get() const throw ())[4][4]
    {
        return this->matrix;
    }
}

#endif // OPENVRML_VRMLMATRIX_H
