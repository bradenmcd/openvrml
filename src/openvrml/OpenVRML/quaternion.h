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

# ifndef OPENVRML_QUATERNION_H
#   define OPENVRML_QUATERNION_H

#   include <assert.h>
#   include <stddef.h>
#   include <algorithm>
#   include <iosfwd>
#   include <OpenVRML/common.h>

namespace OpenVRML {

    class rotation;
    class mat4f;

    class OPENVRML_SCOPE Quaternion {
        float quat[4];

    public:
        Quaternion() throw ();
        Quaternion(float x, float y, float z, float w) throw ();
        explicit Quaternion(const float quat[4]) throw ();
        explicit Quaternion(const rotation & rot) throw ();
        explicit Quaternion(const mat4f & mat) throw ();

        // use compiler-defined operator= and copy constructor.

        float operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();
        const float (&get() const throw ())[4];
        void set(const float (&quat)[4]) throw ();
        float getX() const throw ();
        void setX(float x) throw ();
        float getY() const throw ();
        void setY(float y) throw ();
        float getZ() const throw ();
        void setZ(float z) throw ();
        float getW() const throw ();
        void setW(float w) throw ();
        const Quaternion add(const Quaternion & quat) const throw ();
        const Quaternion conjugate() const throw ();
        const Quaternion divide(float value) const throw ();
        const Quaternion inverse() const throw ();
        const Quaternion multiply(const Quaternion & quat) const throw ();
        const Quaternion multiply(float value) const throw ();
        float norm() const throw ();
        const Quaternion normalize() const throw ();
    };

    inline bool operator==(const Quaternion & lhs, const Quaternion & rhs)
        throw ()
    {
        return std::equal(lhs.get(), lhs.get() + 4, rhs.get());
    }

    inline bool operator!=(const Quaternion & lhs, const Quaternion & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }

    inline float & Quaternion::operator[](size_t index) throw ()
    {
        assert(index < 4);
        return this->quat[index];
    }

    inline float Quaternion::operator[](size_t index) const throw ()
    {
        assert(index < 4);
        return this->quat[index];
    }

    inline const float (&Quaternion::get() const throw ())[4]
    {
        return this->quat;
    }

    inline void Quaternion::set(const float (&quat)[4]) throw ()
    {
        std::copy(quat, quat + 4, this->quat);
    }

    inline float Quaternion::getX() const throw ()
    {
        return this->quat[0];
    }

    inline void Quaternion::setX(const float x) throw ()
    {
        this->quat[0] = x;
    }

    inline float Quaternion::getY() const throw ()
    {
        return this->quat[1];
    }

    inline void Quaternion::setY(const float y) throw ()
    {
        this->quat[1] = y;
    }

    inline float Quaternion::getZ() const throw ()
    {
        return this->quat[2];
    }

    inline void Quaternion::setZ(const float z) throw ()
    {
        this->quat[2] = z;
    }

    inline float Quaternion::getW() const throw ()
    {
        return this->quat[3];
    }

    inline void Quaternion::setW(const float w) throw ()
    {
        this->quat[3] = w;
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const Quaternion & quat);

}

#endif // OPENVRML_QUATERNION_H
