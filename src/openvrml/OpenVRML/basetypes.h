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

# ifndef OPENVRML_BASETYPES_H
#   define OPENVRML_BASETYPES_H

#   include <assert.h>
#   include <math.h>
#   include <stddef.h>
#   include <algorithm>
#   include <iosfwd>
#   include <sys/types.h>
#   include <OpenVRML/common.h>

namespace OpenVRML {

#   ifdef _WIN32
    typedef __int32 int32;
#   else
    typedef int32_t int32;
#   endif

    class OPENVRML_SCOPE color {
        float rgb[3];

    public:
        color() throw ();
        explicit color(const float (&rgb)[3]) throw ();
        color(float r, float g, float b) throw ();

        const float & operator[](size_t index) const throw ();

        float r() const throw ();
        float g() const throw ();
        float b() const throw ();

        void r(float value) throw ();
        void g(float value) throw ();
        void b(float value) throw ();

        void hsv(float (&result)[3]) const throw ();
        void hsv(float h, float s, float v) throw ();
    };

    inline color::color() throw ()
    {}

    inline color::color(const float (&rgb)[3]) throw ()
    {
        assert(rgb[0] >= 0.0 && rgb[0] <= 1.0);
        this->rgb[0] = rgb[0];
        assert(rgb[1] >= 0.0 && rgb[1] <= 1.0);
        this->rgb[1] = rgb[1];
        assert(rgb[2] >= 0.0 && rgb[2] <= 1.0);
        this->rgb[2] = rgb[2];
    }

    inline color::color(float r, float g, float b) throw ()
    {
        assert(r >= 0.0 && r <= 1.0);
        this->rgb[0] = r;
        assert(g >= 0.0 && g <= 1.0);
        this->rgb[1] = g;
        assert(b >= 0.0 && b <= 1.0);
        this->rgb[2] = b;
    }

    inline const float & color::operator[](const size_t index) const throw ()
    {
        assert(index < 3);
        return this->rgb[index];
    }

    inline float color::r() const throw ()
    {
        return this->rgb[0];
    }

    inline float color::g() const throw ()
    {
        return this->rgb[1];
    }

    inline float color::b() const throw ()
    {
        return this->rgb[2];
    }

    inline void color::r(const float value) throw ()
    {
        this->rgb[0] = value;
    }

    inline void color::g(const float value) throw ()
    {
        this->rgb[1] = value;
    }

    inline void color::b(const float value) throw ()
    {
        this->rgb[2] = value;
    }

    bool OPENVRML_SCOPE operator==(const color & lhs, const color & rhs)
        throw ();

    inline bool operator!=(const color & lhs, const color & rhs) throw ()
    {
        return !(lhs == rhs);
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const color & c);


    class OPENVRML_SCOPE vec2f {
        float vec[2];

    public:
        vec2f() throw ();
        explicit vec2f(const float (&vec)[2]) throw ();
        vec2f(float x, float y) throw ();

        vec2f & operator*=(float scalar) throw ();
        vec2f & operator/=(float scalar) throw ();
        vec2f & operator+=(const vec2f & vec) throw ();
        vec2f & operator-=(const vec2f & vec) throw ();

        const vec2f operator-() const throw ();

        const float & operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();

        float x() const throw ();
        float y() const throw ();

        void x(float value) throw ();
        void y(float value) throw ();

        float dot(const vec2f & vec) const throw ();
        float length() const throw ();
        const vec2f normalize() const throw ();
    };

    inline vec2f::vec2f() throw ()
    {
        this->vec[0] = 0.0;
        this->vec[1] = 0.0;
    }

    inline vec2f::vec2f(const float (&vec)[2]) throw ()
    {
        this->vec[0] = vec[0];
        this->vec[1] = vec[1];
    }

    inline vec2f::vec2f(const float x, const float y) throw ()
    {
        this->vec[0] = x;
        this->vec[1] = y;
    }

    inline vec2f & vec2f::operator*=(const float scalar) throw ()
    {
        this->vec[0] *= scalar;
        this->vec[1] *= scalar;
        return *this;
    }

    inline const vec2f operator*(const vec2f & lhs, const float rhs) throw ()
    {
        vec2f result(lhs);
        return result *= rhs;
    }

    inline vec2f & vec2f::operator/=(const float scalar) throw ()
    {
        this->vec[0] /= scalar;
        this->vec[1] /= scalar;
        return *this;
    }

    inline const vec2f operator/(const vec2f & lhs, const float rhs) throw ()
    {
        vec2f result(lhs);
        return result /= rhs;
    }

    inline vec2f & vec2f::operator+=(const vec2f & vec) throw ()
    {
        this->vec[0] += vec[0];
        this->vec[1] += vec[1];
        return *this;
    }

    inline const vec2f operator+(const vec2f & lhs, const vec2f & rhs) throw ()
    {
        vec2f result(lhs);
        return result += rhs;
    }

    inline vec2f & vec2f::operator-=(const vec2f & vec) throw ()
    {
        this->vec[0] -= vec[0];
        this->vec[1] -= vec[1];
        return *this;
    }

    inline const vec2f operator-(const vec2f & lhs, const vec2f & rhs) throw ()
    {
        vec2f result(lhs);
        return result -= rhs;
    }

    inline const vec2f vec2f::operator-() const throw ()
    {
        return vec2f(-this->vec[0], -this->vec[1]);
    }

    inline const float & vec2f::operator[](const size_t index) const throw ()
    {
        assert(index < 2);
        return this->vec[index];
    }

    inline float & vec2f::operator[](const size_t index) throw ()
    {
        assert(index < 2);
        return this->vec[index];
    }

    inline float vec2f::x() const throw ()
    {
        return this->vec[0];
    }

    inline float vec2f::y() const throw ()
    {
        return this->vec[1];
    }

    inline void vec2f::x(const float value) throw ()
    {
        this->vec[0] = value;
    }

    inline void vec2f::y(const float value) throw ()
    {
        this->vec[1] = value;
    }

    inline float vec2f::dot(const vec2f & vec) const throw ()
    {
        return this->vec[0] * vec[0]
             + this->vec[1] * vec[1];
    }

    inline float vec2f::length() const throw ()
    {
        return sqrt(this->dot(*this));
    }

    bool OPENVRML_SCOPE operator==(const vec2f & lhs, const vec2f & rhs)
        throw ();

    inline bool operator!=(const vec2f & lhs, const vec2f & rhs) throw ()
    {
        return !(lhs == rhs);
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const vec2f & v);


    class mat4f;

    class OPENVRML_SCOPE vec3f {
        float vec[3];

    public:
        vec3f() throw ();
        explicit vec3f(const float (&vec)[3]) throw ();
        vec3f(float x, float y, float z) throw ();

        vec3f & operator*=(const vec3f & vec) throw ();
        vec3f & operator*=(const mat4f & mat) throw ();
        vec3f & operator*=(float scalar) throw ();
        vec3f & operator/=(float scalar) throw ();
        vec3f & operator+=(const vec3f & vec) throw ();
        vec3f & operator-=(const vec3f & vec) throw ();

        const vec3f operator-() const throw ();

        const float & operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();

        float x() const throw ();
        float y() const throw ();
        float z() const throw ();

        void x(float value) throw ();
        void y(float value) throw ();
        void z(float value) throw ();

        float dot(const vec3f & vec) const throw ();
        float length() const throw ();
        const vec3f normalize() const throw ();
    };

    inline vec3f::vec3f() throw ()
    {
        this->vec[0] = 0.0;
        this->vec[1] = 0.0;
        this->vec[2] = 0.0;
    }

    inline vec3f::vec3f(const float (&vec)[3]) throw ()
    {
        this->vec[0] = vec[0];
        this->vec[1] = vec[1];
        this->vec[2] = vec[2];
    }

    inline vec3f::vec3f(const float x, const float y, const float z) throw ()
    {
        this->vec[0] = x;
        this->vec[1] = y;
        this->vec[2] = z;
    }

    inline vec3f & vec3f::operator*=(const vec3f & vec) throw ()
    {
        vec3f temp;
        temp.x(this->y() * vec.z() - this->z() * vec.y());
        temp.y(this->z() * vec.x() - this->x() * vec.z());
        temp.z(this->x() * vec.y() - this->y() * vec.x());
        *this = temp;
        return *this;
    }

    inline const vec3f operator*(const vec3f & lhs, const vec3f & rhs) throw ()
    {
        vec3f result(lhs);
        return result *= rhs;
    }

    inline vec3f & vec3f::operator*=(const float scalar) throw ()
    {
        this->vec[0] *= scalar;
        this->vec[1] *= scalar;
        this->vec[2] *= scalar;
        return *this;
    }

    inline const vec3f operator*(const vec3f & lhs, const float rhs) throw ()
    {
        vec3f result(lhs);
        return result *= rhs;
    }

    inline const vec3f operator*(const vec3f & vec, const mat4f & mat) throw ()
    {
        vec3f result(vec);
        return result *= mat;
    }

    const vec3f OPENVRML_SCOPE operator*(const mat4f & mat, const vec3f & vec)
        throw ();

    inline vec3f & vec3f::operator/=(const float scalar) throw ()
    {
        this->vec[0] /= scalar;
        this->vec[1] /= scalar;
        this->vec[2] /= scalar;
        return *this;
    }

    inline const vec3f operator/(const vec3f & lhs, const float rhs) throw ()
    {
        vec3f result(lhs);
        return result /= rhs;
    }

    inline vec3f & vec3f::operator+=(const vec3f & vec) throw ()
    {
        this->vec[0] += vec[0];
        this->vec[1] += vec[1];
        this->vec[2] += vec[2];
        return *this;
    }

    inline const vec3f operator+(const vec3f & lhs, const vec3f & rhs) throw ()
    {
        vec3f result(lhs);
        return result += rhs;
    }

    inline vec3f & vec3f::operator-=(const vec3f & vec) throw ()
    {
        this->vec[0] -= vec[0];
        this->vec[1] -= vec[1];
        this->vec[2] -= vec[2];
        return *this;
    }

    inline const vec3f operator-(const vec3f & lhs, const vec3f & rhs) throw ()
    {
        vec3f result(lhs);
        return result -= rhs;
    }

    inline const vec3f vec3f::operator-() const throw()
    {
        return vec3f(-this->vec[0], -this->vec[1], -this->vec[2]);
    }

    inline const float & vec3f::operator[](const size_t index) const throw ()
    {
        assert(index < 3);
        return this->vec[index];
    }

    inline float & vec3f::operator[](const size_t index) throw ()
    {
        assert(index < 3);
        return this->vec[index];
    }

    inline float vec3f::x() const throw ()
    {
        return this->vec[0];
    }

    inline float vec3f::y() const throw ()
    {
        return this->vec[1];
    }

    inline float vec3f::z() const throw ()
    {
        return this->vec[2];
    }

    inline void vec3f::x(const float value) throw ()
    {
        this->vec[0] = value;
    }

    inline void vec3f::y(const float value) throw ()
    {
        this->vec[1] = value;
    }

    inline void vec3f::z(const float value) throw ()
    {
        this->vec[2] = value;
    }

    inline float vec3f::dot(const vec3f & vec) const throw()
    {
        return this->vec[0] * vec[0]
             + this->vec[1] * vec[1]
             + this->vec[2] * vec[2];
    }

    inline float vec3f::length() const throw ()
    {
        return sqrt(this->dot(*this));
    }

    bool OPENVRML_SCOPE operator==(const vec3f & lhs, const vec3f & rhs)
        throw ();

    inline bool operator!=(const vec3f & lhs, const vec3f & rhs) throw ()
    {
        return !(lhs == rhs);
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const vec3f & v);


    class quatf;

    class OPENVRML_SCOPE rotation {
        float rot[4];

    public:
        rotation() throw ();
        explicit rotation(const float (&rot)[4]) throw ();
        rotation(float x, float y, float z, float angle) throw ();
        rotation(const vec3f & axis, float angle) throw ();
        rotation(const vec3f & from_vec, const vec3f & to_vec) throw ();
        explicit rotation(const quatf & quat) throw ();

        rotation & operator*=(const rotation & rot) throw ();

        const float & operator[](size_t index) const throw ();

        float x() const throw ();
        float y() const throw ();
        float z() const throw ();
        const vec3f axis() const throw ();
        float angle() const throw ();

        void x(float value) throw ();
        void y(float value) throw ();
        void z(float value) throw ();
        void axis(const vec3f & vec) throw ();
        void angle(float value) throw ();

        const rotation inverse() const throw ();
        const rotation slerp(const rotation & dest_rot, float t) const throw ();
    };

    inline const rotation operator*(const rotation & lhs, const rotation & rhs)
        throw ()
    {
        rotation result(lhs);
        return result *= rhs;
    }

    inline rotation::rotation() throw ()
    {
        this->rot[0] = 0.0;
        this->rot[1] = 0.0;
        this->rot[2] = 1.0;
        this->rot[3] = 0.0;
    }

    inline const float & rotation::operator[](const size_t index) const throw ()
    {
        assert(index < 4);
        return this->rot[index];
    }

    inline float rotation::x() const throw ()
    {
        return this->rot[0];
    }

    inline float rotation::y() const throw ()
    {
        return this->rot[1];
    }

    inline float rotation::z() const throw ()
    {
        return this->rot[2];
    }

    inline const vec3f rotation::axis() const throw ()
    {
        return vec3f(this->rot[0], this->rot[1], this->rot[2]);
    }

    inline float rotation::angle() const throw ()
    {
        return this->rot[3];
    }

    inline void rotation::angle(const float value) throw ()
    {
        this->rot[3] = value;
    }

    inline const rotation rotation::inverse() const throw ()
    {
        rotation result(*this);
        result.rot[3] = -this->rot[3];
        return result;
    }

    bool OPENVRML_SCOPE operator==(const rotation & lhs, const rotation & rhs)
        throw ();

    inline bool operator!=(const rotation & lhs, const rotation & rhs) throw ()
    {
        return !(lhs == rhs);
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const rotation & r);


    class OPENVRML_SCOPE mat4f {
        float mat[4][4];

    public:
        static const mat4f rotation(const OpenVRML::rotation & rot) throw ();
        static const mat4f rotation(const quatf & quat) throw ();
        static const mat4f scale(float s) throw ();
        static const mat4f scale(const vec3f & s) throw ();
        static const mat4f translation(const vec3f & t) throw ();
        static const mat4f transformation(const vec3f & t,
                                          const OpenVRML::rotation & r,
                                          const vec3f & s,
                                          const OpenVRML::rotation & sr,
                                          const vec3f & c) throw ();

        mat4f() throw ();
        mat4f(float f11, float f12, float f13, float f14,
              float f21, float f22, float f23, float f24,
              float f31, float f32, float f33, float f34,
              float f41, float f42, float f43, float f44) throw ();
        explicit mat4f(const float mat[16]) throw ();
        explicit mat4f(const float (&mat)[4][4]) throw ();

        // Use compiler-defined operator= and copy constructor.

        mat4f & operator*=(float scalar) throw ();
        mat4f & operator*=(const mat4f & mat) throw ();

        float (&operator[](size_t index) throw ())[4];
        const float (&operator[](size_t index) const throw ())[4];

        void transformation(vec3f & t, OpenVRML::rotation & r, vec3f & s) const
            throw ();
        void transformation(vec3f & t, OpenVRML::rotation & r, vec3f & s,
                            vec3f & shear) const
            throw ();

        const mat4f inverse() const throw ();
        const mat4f transpose() const throw ();
        float det() const throw ();
    };

    bool OPENVRML_SCOPE operator==(const mat4f & lhs, const mat4f & rhs)
        throw ();

    inline bool operator!=(const mat4f & lhs, const mat4f & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }

    inline float (&mat4f::operator[](size_t index) throw ())[4]
    {
        assert(index < 4);
        return this->mat[index];
    }

    inline const float (&mat4f::operator[](size_t index) const throw ())[4]
    {
        assert(index < 4);
        return this->mat[index];
    }

    inline const mat4f operator*(const mat4f & lhs, const mat4f & rhs) throw ()
    {
        mat4f result(lhs);
        return result *= rhs;
    }

    inline const mat4f operator*(const mat4f & mat, const float scalar) throw ()
    {
        mat4f result(mat);
        return result *= scalar;
    }

    inline const mat4f operator*(const float scalar, const mat4f & mat) throw ()
    {
        mat4f result(mat);
        return result *= scalar;
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const mat4f & mat);


    class OPENVRML_SCOPE quatf {
        float quat[4];

    public:
        quatf() throw ();
        quatf(float x, float y, float z, float w) throw ();
        explicit quatf(const float (&quat)[4]) throw ();
        explicit quatf(const rotation & rot) throw ();
        explicit quatf(const mat4f & mat) throw ();

        // use compiler-defined operator= and copy constructor.

        quatf & operator*=(const quatf & quat) throw ();
        quatf & operator*=(float scalar) throw ();
        quatf & operator/=(float scalar) throw ();
        quatf & operator+=(const quatf & quat) throw ();
        quatf & operator-=(const quatf & quat) throw ();

        const float & operator[](size_t index) const throw ();
        float & operator[](size_t index) throw ();

        float x() const throw ();
        float y() const throw ();
        float z() const throw ();
        float w() const throw ();

        void x(float value) throw ();
        void y(float value) throw ();
        void z(float value) throw ();
        void w(float value) throw ();

        const quatf conjugate() const throw ();
        const quatf inverse() const throw ();
        float norm() const throw ();
        const quatf normalize() const throw ();
    };

    bool OPENVRML_SCOPE operator==(const quatf & lhs, const quatf & rhs)
        throw ();

    inline bool operator!=(const quatf & lhs, const quatf & rhs) throw ()
    {
        return !(lhs == rhs);
    }

    inline const quatf operator*(const quatf & lhs, const quatf & rhs) throw ()
    {
        quatf result(lhs);
        return result *= rhs;
    }

    inline const quatf operator*(const quatf & quat, const float scalar)
        throw ()
    {
        quatf result(quat);
        return result *= scalar;
    }

    inline const quatf operator*(const float scalar, const quatf & quat)
        throw ()
    {
        quatf result(quat);
        return result *= scalar;
    }

    inline const quatf operator/(const quatf & quat, const float scalar)
        throw ()
    {
        quatf result(quat);
        return result /= scalar;
    }

    inline const quatf operator+(const quatf & lhs, const quatf & rhs) throw ()
    {
        quatf result(lhs);
        return result += rhs;
    }

    inline const quatf operator-(const quatf & lhs, const quatf & rhs) throw ()
    {
        quatf result(lhs);
        return result -= rhs;
    }

    inline const float & quatf::operator[](size_t index) const throw ()
    {
        assert(index < 4);
        return this->quat[index];
    }

    inline float & quatf::operator[](size_t index) throw ()
    {
        assert(index < 4);
        return this->quat[index];
    }

    inline float quatf::x() const throw ()
    {
        return this->quat[0];
    }

    inline float quatf::y() const throw ()
    {
        return this->quat[1];
    }

    inline float quatf::z() const throw ()
    {
        return this->quat[2];
    }

    inline float quatf::w() const throw ()
    {
        return this->quat[3];
    }

    inline void quatf::x(const float value) throw ()
    {
        this->quat[0] = value;
    }

    inline void quatf::y(const float value) throw ()
    {
        this->quat[1] = value;
    }

    inline void quatf::z(const float value) throw ()
    {
        this->quat[2] = value;
    }

    inline void quatf::w(const float value) throw ()
    {
        this->quat[3] = value;
    }

    std::ostream & OPENVRML_SCOPE operator<<(std::ostream & out,
                                             const quatf & quat);

} // namespace OpenVRML

# endif // OPENVRML_BASETYPES_H
