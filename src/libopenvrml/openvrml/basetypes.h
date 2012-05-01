// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001  S. K. Bose
// Copyright 2003, 2004, 2005, 2006, 2007  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_BASETYPES_H
#   define OPENVRML_BASETYPES_H

#   include <openvrml-common.h>
#   include <cassert>
#   include <cstddef>
#   include <algorithm>
#   include <iosfwd>
#   include <vector>
#   include <boost/cstdint.hpp>

namespace openvrml {

    typedef boost::int32_t int32;


    struct OPENVRML_API color {
        float rgb[3];

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;

        float r() const OPENVRML_NOTHROW;
        float g() const OPENVRML_NOTHROW;
        float b() const OPENVRML_NOTHROW;

        void r(float value) OPENVRML_NOTHROW;
        void g(float value) OPENVRML_NOTHROW;
        void b(float value) OPENVRML_NOTHROW;

        void hsv(float (&result)[3]) const OPENVRML_NOTHROW;
        void hsv(float h, float s, float v) OPENVRML_NOTHROW;
    };

    OPENVRML_API const color make_color() OPENVRML_NOTHROW;
    OPENVRML_API const color make_color(const float (&rgb)[3])
        OPENVRML_NOTHROW;
    OPENVRML_API const color make_color(float r, float g, float b)
        OPENVRML_NOTHROW;

    OPENVRML_API bool operator==(const color & lhs, const color & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const color & lhs, const color & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API std::istream & operator>>(std::istream & in, color & c);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const color & c);

    inline const float & color::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 3UL);
        return this->rgb[index];
    }

    inline float color::r() const OPENVRML_NOTHROW
    {
        return this->rgb[0];
    }

    inline float color::g() const OPENVRML_NOTHROW
    {
        return this->rgb[1];
    }

    inline float color::b() const OPENVRML_NOTHROW
    {
        return this->rgb[2];
    }

    inline void color::r(const float value) OPENVRML_NOTHROW
    {
        this->rgb[0] = value;
    }

    inline void color::g(const float value) OPENVRML_NOTHROW
    {
        this->rgb[1] = value;
    }

    inline void color::b(const float value) OPENVRML_NOTHROW
    {
        this->rgb[2] = value;
    }


    struct OPENVRML_API color_rgba {
        float rgba[4];

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;

        float r() const OPENVRML_NOTHROW;
        float g() const OPENVRML_NOTHROW;
        float b() const OPENVRML_NOTHROW;
        float a() const OPENVRML_NOTHROW;

        void r(float value) OPENVRML_NOTHROW;
        void g(float value) OPENVRML_NOTHROW;
        void b(float value) OPENVRML_NOTHROW;
        void a(float value) OPENVRML_NOTHROW;

        void hsv(float (&result)[4]) const OPENVRML_NOTHROW;
        void hsv(float h, float s, float v, float a = 1.0f) OPENVRML_NOTHROW;
    };

    OPENVRML_API const color_rgba make_color_rgba() OPENVRML_NOTHROW;
    OPENVRML_API const color_rgba make_color_rgba(const float (&rgba)[4])
        OPENVRML_NOTHROW;
    OPENVRML_API const color_rgba make_color_rgba(float r,
                                                  float g,
                                                  float b,
                                                  float a)
        OPENVRML_NOTHROW;

    OPENVRML_API bool operator==(const color_rgba & lhs,
                                 const color_rgba & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const color_rgba & lhs,
                                 const color_rgba & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API std::istream & operator>>(std::istream & in, color_rgba & c);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const color_rgba & c);

    inline const float & color_rgba::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 4ul);
        return this->rgba[index];
    }

    inline float color_rgba::r() const OPENVRML_NOTHROW
    {
        return this->rgba[0];
    }

    inline float color_rgba::g() const OPENVRML_NOTHROW
    {
        return this->rgba[1];
    }

    inline float color_rgba::b() const OPENVRML_NOTHROW
    {
        return this->rgba[2];
    }

    inline float color_rgba::a() const OPENVRML_NOTHROW
    {
        return this->rgba[3];
    }

    inline void color_rgba::r(const float value) OPENVRML_NOTHROW
    {
        this->rgba[0] = value;
    }

    inline void color_rgba::g(const float value) OPENVRML_NOTHROW
    {
        this->rgba[1] = value;
    }

    inline void color_rgba::b(const float value) OPENVRML_NOTHROW
    {
        this->rgba[2] = value;
    }

    inline void color_rgba::a(const float value) OPENVRML_NOTHROW
    {
        this->rgba[3] = value;
    }


    struct OPENVRML_API vec2f {
        float vec[2];

        vec2f & operator*=(float scalar) OPENVRML_NOTHROW;
        vec2f & operator/=(float scalar) OPENVRML_NOTHROW;
        vec2f & operator+=(const vec2f & vec) OPENVRML_NOTHROW;
        vec2f & operator-=(const vec2f & vec) OPENVRML_NOTHROW;

        const vec2f operator-() const OPENVRML_NOTHROW;

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;

        float x() const OPENVRML_NOTHROW;
        void x(float value) OPENVRML_NOTHROW;

        float y() const OPENVRML_NOTHROW;
        void y(float value) OPENVRML_NOTHROW;

        float dot(const vec2f & vec) const OPENVRML_NOTHROW;
        float length() const OPENVRML_NOTHROW;
        const vec2f normalize() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const vec2f make_vec2f() OPENVRML_NOTHROW;
    OPENVRML_API const vec2f make_vec2f(const float (&vec)[2])
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2f make_vec2f(float x, float y) OPENVRML_NOTHROW;

    OPENVRML_API const vec2f operator*(const vec2f & lhs, float rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2f operator*(float lhs, const vec2f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2f operator/(const vec2f & lhs, float rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2f operator+(const vec2f & lhs, const vec2f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2f operator-(const vec2f & lhs, const vec2f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const vec2f & lhs, const vec2f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const vec2f & lhs, const vec2f & rhs)
        OPENVRML_NOTHROW;

    OPENVRML_API std::istream & operator>>(std::istream & in, vec2f & v);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec2f & v);

    inline const float & vec2f::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 2ul);
        return this->vec[index];
    }

    inline float vec2f::x() const OPENVRML_NOTHROW
    {
        return this->vec[0];
    }

    inline float vec2f::y() const OPENVRML_NOTHROW
    {
        return this->vec[1];
    }

    inline void vec2f::x(const float value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    inline void vec2f::y(const float value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }


    struct OPENVRML_API vec2d {
        double vec[2];

        vec2d & operator*=(double scalar) OPENVRML_NOTHROW;
        vec2d & operator/=(double scalar) OPENVRML_NOTHROW;
        vec2d & operator+=(const vec2d & vec) OPENVRML_NOTHROW;
        vec2d & operator-=(const vec2d & vec) OPENVRML_NOTHROW;

        const vec2d operator-() const OPENVRML_NOTHROW;

        const double & operator[](std::size_t index) const OPENVRML_NOTHROW;

        double x() const OPENVRML_NOTHROW;
        void x(double value) OPENVRML_NOTHROW;

        double y() const OPENVRML_NOTHROW;
        void y(double value) OPENVRML_NOTHROW;

        double dot(const vec2d & vec) const OPENVRML_NOTHROW;
        double length() const OPENVRML_NOTHROW;
        const vec2d normalize() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const vec2d make_vec2d() OPENVRML_NOTHROW;
    OPENVRML_API const vec2d make_vec2d(const double (&vec)[2])
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2d make_vec2d(double x, double y) OPENVRML_NOTHROW;

    OPENVRML_API const vec2d operator*(const vec2d & lhs, double rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2d operator*(double lhs, const vec2d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2d operator/(const vec2d & lhs, double rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2d operator+(const vec2d & lhs, const vec2d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec2d operator-(const vec2d & lhs, const vec2d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const vec2d & lhs, const vec2d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const vec2d & lhs, const vec2d & rhs)
        OPENVRML_NOTHROW;

    OPENVRML_API std::istream & operator>>(std::istream & in, vec2d & v);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec2d & v);

    inline const double & vec2d::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 2ul);
        return this->vec[index];
    }

    inline double vec2d::x() const OPENVRML_NOTHROW
    {
        return this->vec[0];
    }

    inline double vec2d::y() const OPENVRML_NOTHROW
    {
        return this->vec[1];
    }

    inline void vec2d::x(const double value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    inline void vec2d::y(const double value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }


    struct mat4f;

    struct OPENVRML_API vec3f {
        float vec[3];

        vec3f & operator*=(const vec3f & vec) OPENVRML_NOTHROW;
        vec3f & operator*=(const mat4f & mat) OPENVRML_NOTHROW;
        vec3f & operator*=(float scalar) OPENVRML_NOTHROW;
        vec3f & operator/=(float scalar) OPENVRML_NOTHROW;
        vec3f & operator+=(const vec3f & vec) OPENVRML_NOTHROW;
        vec3f & operator-=(const vec3f & vec) OPENVRML_NOTHROW;

        const vec3f operator-() const OPENVRML_NOTHROW;

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;

        float x() const OPENVRML_NOTHROW;
        void x(float value) OPENVRML_NOTHROW;

        float y() const OPENVRML_NOTHROW;
        void y(float value) OPENVRML_NOTHROW;

        float z() const OPENVRML_NOTHROW;
        void z(float value) OPENVRML_NOTHROW;

        float dot(const vec3f & vec) const OPENVRML_NOTHROW;
        float length() const OPENVRML_NOTHROW;
        const vec3f normalize() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const vec3f make_vec3f() OPENVRML_NOTHROW;
    OPENVRML_API const vec3f make_vec3f(const float (&vec)[3])
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f make_vec3f(float x, float y, float z)
        OPENVRML_NOTHROW;

    OPENVRML_API const vec3f operator*(const vec3f & lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator*(const vec3f & lhs, float rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator*(float lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator*(const vec3f & lhs, const mat4f & mat)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator*(const mat4f & mat, const vec3f & vec)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator/(const vec3f & lhs, float rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator+(const vec3f & lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3f operator-(const vec3f & lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const vec3f & lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const vec3f & lhs, const vec3f & rhs)
        OPENVRML_NOTHROW;

    OPENVRML_API std::istream & operator>>(std::istream & in, vec3f & v);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec3f & v);


    inline const float & vec3f::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 3ul);
        return this->vec[index];
    }

    inline float vec3f::x() const OPENVRML_NOTHROW
    {
        return this->vec[0];
    }

    inline float vec3f::y() const OPENVRML_NOTHROW
    {
        return this->vec[1];
    }

    inline float vec3f::z() const OPENVRML_NOTHROW
    {
        return this->vec[2];
    }

    inline void vec3f::x(const float value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    inline void vec3f::y(const float value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }

    inline void vec3f::z(const float value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[2] = value;
    }


    struct OPENVRML_API vec3d {
        double vec[3];

        vec3d & operator*=(const vec3d & vec) OPENVRML_NOTHROW;
        vec3d & operator*=(const mat4f & mat) OPENVRML_NOTHROW;
        vec3d & operator*=(double scalar) OPENVRML_NOTHROW;
        vec3d & operator/=(double scalar) OPENVRML_NOTHROW;
        vec3d & operator+=(const vec3d & vec) OPENVRML_NOTHROW;
        vec3d & operator-=(const vec3d & vec) OPENVRML_NOTHROW;

        const vec3d operator-() const OPENVRML_NOTHROW;

        const double & operator[](std::size_t index) const OPENVRML_NOTHROW;

        double x() const OPENVRML_NOTHROW;
        void x(double value) OPENVRML_NOTHROW;

        double y() const OPENVRML_NOTHROW;
        void y(double value) OPENVRML_NOTHROW;

        double z() const OPENVRML_NOTHROW;
        void z(double value) OPENVRML_NOTHROW;

        double dot(const vec3d & vec) const OPENVRML_NOTHROW;
        double length() const OPENVRML_NOTHROW;
        const vec3d normalize() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const vec3d make_vec3d() OPENVRML_NOTHROW;
    OPENVRML_API const vec3d make_vec3d(const double (&vec)[3])
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d make_vec3d(double x, double y, double z)
        OPENVRML_NOTHROW;

    OPENVRML_API const vec3d operator*(const vec3d & lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator*(const vec3d & lhs, double rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator*(double lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator*(const vec3d & lhs, const mat4f & mat)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator*(const mat4f & mat, const vec3d & vec)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator/(const vec3d & lhs, double rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator+(const vec3d & lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const vec3d operator-(const vec3d & lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const vec3d & lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const vec3d & lhs, const vec3d & rhs)
        OPENVRML_NOTHROW;

    OPENVRML_API std::istream & operator>>(std::istream & in, vec3d & v);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec3d & v);


    inline const double & vec3d::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 3ul);
        return this->vec[index];
    }

    inline double vec3d::x() const OPENVRML_NOTHROW
    {
        return this->vec[0];
    }

    inline double vec3d::y() const OPENVRML_NOTHROW
    {
        return this->vec[1];
    }

    inline double vec3d::z() const OPENVRML_NOTHROW
    {
        return this->vec[2];
    }

    inline void vec3d::x(const double value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    inline void vec3d::y(const double value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }

    inline void vec3d::z(const double value) OPENVRML_NOTHROW
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[2] = value;
    }


    struct quatf;

    struct OPENVRML_API rotation {
        float rot[4];

        rotation & operator*=(const rotation & rot) OPENVRML_NOTHROW;

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;

        float x() const OPENVRML_NOTHROW;
        float y() const OPENVRML_NOTHROW;
        float z() const OPENVRML_NOTHROW;
        const vec3f axis() const OPENVRML_NOTHROW;
        float angle() const OPENVRML_NOTHROW;

        void x(float value) OPENVRML_NOTHROW;
        void y(float value) OPENVRML_NOTHROW;
        void z(float value) OPENVRML_NOTHROW;
        void axis(const vec3f & axis) OPENVRML_NOTHROW;
        void angle(float value) OPENVRML_NOTHROW;

        const rotation inverse() const OPENVRML_NOTHROW;
        const rotation slerp(const rotation & dest_rot, float t) const
            OPENVRML_NOTHROW;
    };

    OPENVRML_API const rotation make_rotation() OPENVRML_NOTHROW;
    OPENVRML_API const rotation make_rotation(const float (&rot)[4])
        OPENVRML_NOTHROW;
    OPENVRML_API const rotation make_rotation(float x,
                                              float y,
                                              float z,
                                              float angle)
        OPENVRML_NOTHROW;
    OPENVRML_API const rotation make_rotation(const vec3f & axis, float angle)
        OPENVRML_NOTHROW;
    OPENVRML_API const rotation make_rotation(const vec3f & from_vec,
                                              const vec3f & to_vec)
        OPENVRML_NOTHROW;
    OPENVRML_API const rotation make_rotation(const quatf & quat)
        OPENVRML_NOTHROW;

    OPENVRML_API const rotation operator*(const rotation & lhs,
                                          const rotation & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const rotation & lhs, const rotation & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const rotation & lhs, const rotation & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API std::istream & operator>>(std::istream & in, rotation & rot);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const rotation & r);

    inline const float & rotation::operator[](const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < 4ul);
        return this->rot[index];
    }

    inline float rotation::x() const OPENVRML_NOTHROW
    {
        return this->rot[0];
    }

    inline float rotation::y() const OPENVRML_NOTHROW
    {
        return this->rot[1];
    }

    inline float rotation::z() const OPENVRML_NOTHROW
    {
        return this->rot[2];
    }

    inline const vec3f rotation::axis() const OPENVRML_NOTHROW
    {
        return make_vec3f(this->rot[0], this->rot[1], this->rot[2]);
    }

    inline float rotation::angle() const OPENVRML_NOTHROW
    {
        return this->rot[3];
    }

    inline void rotation::angle(const float value) OPENVRML_NOTHROW
    {
        this->rot[3] = value;
    }


    struct OPENVRML_API mat4f {
        float mat[4][4];

        mat4f & operator*=(float scalar) OPENVRML_NOTHROW;
        mat4f & operator*=(const mat4f & mat) OPENVRML_NOTHROW;

        float (&operator[](const std::size_t index) OPENVRML_NOTHROW)[4];
        const float (&operator[](const std::size_t index) const OPENVRML_NOTHROW)[4];

        void transformation(vec3f & t, rotation & r, vec3f & s) const
            OPENVRML_NOTHROW;
        void transformation(vec3f & t, rotation & r, vec3f & s,
                            vec3f & shear) const
            OPENVRML_NOTHROW;

        const mat4f inverse() const OPENVRML_NOTHROW;
        const mat4f transpose() const OPENVRML_NOTHROW;
        float det() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const mat4f make_mat4f() OPENVRML_NOTHROW;
    OPENVRML_API const mat4f
    make_mat4f(float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_mat4f(const float mat[16]) OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_mat4f(const float (&mat)[4][4])
        OPENVRML_NOTHROW;

    OPENVRML_API const mat4f make_rotation_mat4f(const rotation & rot)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_rotation_mat4f(const quatf & quat)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_scale_mat4f(float s)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_scale_mat4f(const vec3f & s)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_translation_mat4f(const vec3f & t)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f make_transformation_mat4f(const vec3f & t,
                                                       const rotation & r,
                                                       const vec3f & s,
                                                       const rotation & sr,
                                                       const vec3f & c)
        OPENVRML_NOTHROW;

    OPENVRML_API bool operator==(const mat4f & lhs, const mat4f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const mat4f & lhs, const mat4f & rhs)
        OPENVRML_NOTHROW;

    inline float (&mat4f::operator[](std::size_t index) OPENVRML_NOTHROW)[4]
    {
        assert(index < 4ul);
        return this->mat[index];
    }

    inline const float (&mat4f::operator[](std::size_t index) const OPENVRML_NOTHROW)[4]
    {
        assert(index < 4ul);
        return this->mat[index];
    }

    OPENVRML_API const mat4f operator*(const mat4f & lhs, const mat4f & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f operator*(const mat4f & mat, float scalar)
        OPENVRML_NOTHROW;
    OPENVRML_API const mat4f operator*(float scalar, const mat4f & mat)
        OPENVRML_NOTHROW;
    OPENVRML_API std::istream & operator>>(std::istream & in, mat4f & mat);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const mat4f & mat);


    struct OPENVRML_API quatf {
        float quat[4];

        quatf & operator*=(const quatf & quat) OPENVRML_NOTHROW;
        quatf & operator*=(float scalar) OPENVRML_NOTHROW;
        quatf & operator/=(float scalar) OPENVRML_NOTHROW;
        quatf & operator+=(const quatf & quat) OPENVRML_NOTHROW;
        quatf & operator-=(const quatf & quat) OPENVRML_NOTHROW;

        const float & operator[](std::size_t index) const OPENVRML_NOTHROW;
        float & operator[](std::size_t index) OPENVRML_NOTHROW;

        float x() const OPENVRML_NOTHROW;
        float y() const OPENVRML_NOTHROW;
        float z() const OPENVRML_NOTHROW;
        float w() const OPENVRML_NOTHROW;

        void x(float value) OPENVRML_NOTHROW;
        void y(float value) OPENVRML_NOTHROW;
        void z(float value) OPENVRML_NOTHROW;
        void w(float value) OPENVRML_NOTHROW;

        const quatf conjugate() const OPENVRML_NOTHROW;
        const quatf inverse() const OPENVRML_NOTHROW;
        float norm() const OPENVRML_NOTHROW;
        const quatf normalize() const OPENVRML_NOTHROW;
    };

    OPENVRML_API const quatf make_quatf() OPENVRML_NOTHROW;
    OPENVRML_API const quatf make_quatf(float x, float y, float z, float w)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf make_quatf(const float (&quat)[4])
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf make_quatf(const rotation & rot) OPENVRML_NOTHROW;
    OPENVRML_API const quatf make_quatf(const mat4f & mat) OPENVRML_NOTHROW;

    OPENVRML_API const quatf operator*(const quatf & lhs, const quatf & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf operator*(const quatf & quat, const float scalar)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf operator*(const float scalar, const quatf & quat)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf operator/(const quatf & quat, const float scalar)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf operator+(const quatf & lhs, const quatf & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API const quatf operator-(const quatf & lhs, const quatf & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator==(const quatf & lhs, const quatf & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const quatf & lhs, const quatf & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API std::istream & operator>>(std::istream & in, quatf & q);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const quatf & quat);

    inline const float & quatf::operator[](std::size_t index) const OPENVRML_NOTHROW
    {
        assert(index < 4ul);
        return this->quat[index];
    }

    inline float & quatf::operator[](std::size_t index) OPENVRML_NOTHROW
    {
        assert(index < 4ul);
        return this->quat[index];
    }

    inline float quatf::x() const OPENVRML_NOTHROW
    {
        return this->quat[0];
    }

    inline float quatf::y() const OPENVRML_NOTHROW
    {
        return this->quat[1];
    }

    inline float quatf::z() const OPENVRML_NOTHROW
    {
        return this->quat[2];
    }

    inline float quatf::w() const OPENVRML_NOTHROW
    {
        return this->quat[3];
    }

    inline void quatf::x(const float value) OPENVRML_NOTHROW
    {
        this->quat[0] = value;
    }

    inline void quatf::y(const float value) OPENVRML_NOTHROW
    {
        this->quat[1] = value;
    }

    inline void quatf::z(const float value) OPENVRML_NOTHROW
    {
        this->quat[2] = value;
    }

    inline void quatf::w(const float value) OPENVRML_NOTHROW
    {
        this->quat[3] = value;
    }


    class OPENVRML_API image {
        std::size_t x_;
        std::size_t y_;
        std::size_t comp_;
        std::vector<unsigned char> array_;

    public:
        image() OPENVRML_NOTHROW;
        image(std::size_t x, std::size_t y, std::size_t comp) OPENVRML_THROW1(std::bad_alloc);
        image(std::size_t x, std::size_t y, std::size_t comp,
              const std::vector<unsigned char> & array)
            OPENVRML_THROW1(std::bad_alloc);
        template <typename InputIterator>
        image(std::size_t x, std::size_t y, std::size_t comp,
              InputIterator array_begin, InputIterator array_end)
            OPENVRML_THROW1(std::bad_alloc);

        // Use compiler-defined operator= and copy constructor.

        void swap(image & img) OPENVRML_NOTHROW;

        std::size_t x() const OPENVRML_NOTHROW;
        void x(std::size_t value) OPENVRML_THROW1(std::bad_alloc);

        std::size_t y() const OPENVRML_NOTHROW;
        void y(std::size_t value) OPENVRML_THROW1(std::bad_alloc);

        void resize(std::size_t x, std::size_t y) OPENVRML_THROW1(std::bad_alloc);

        std::size_t comp() const OPENVRML_NOTHROW;
        void comp(std::size_t value) OPENVRML_THROW1(std::bad_alloc);

        const std::vector<unsigned char> & array() const OPENVRML_NOTHROW;
        void array(const std::vector<unsigned char> & value) OPENVRML_NOTHROW;
        template <typename InputIterator>
        void array(InputIterator begin, InputIterator end) OPENVRML_NOTHROW;

        int32 pixel(std::size_t index) const OPENVRML_NOTHROW;
        void pixel(std::size_t index, int32 value) OPENVRML_NOTHROW;
        int32 pixel(std::size_t x, std::size_t y) const OPENVRML_NOTHROW;
        void pixel(std::size_t x, std::size_t y, int32 value) OPENVRML_NOTHROW;
    };

    OPENVRML_API bool operator==(const image & lhs, const image & rhs)
        OPENVRML_NOTHROW;
    OPENVRML_API bool operator!=(const image & lhs, const image & rhs)
        OPENVRML_NOTHROW;

    inline image::image() OPENVRML_NOTHROW:
        x_(0),
        y_(0),
        comp_(0)
    {}

    inline image::image(const std::size_t x,
                        const std::size_t y,
                        const std::size_t comp)
        OPENVRML_THROW1(std::bad_alloc):
        x_(x),
        y_(y),
        comp_(comp),
        array_(x * y * comp)
    {}

    template <typename InputIterator>
    image::image(const std::size_t x,
                 const std::size_t y,
                 const std::size_t comp,
                 InputIterator array_begin,
                 InputIterator array_end)
        OPENVRML_THROW1(std::bad_alloc):
        x_(x),
        y_(y),
        comp_(comp),
        array_(array_begin, array_end)
    {
        using std::iterator_traits;
        using std::distance;
        typedef typename iterator_traits<InputIterator>::difference_type
            difference_type;
        assert(distance(array_begin, array_end)
               <= difference_type(x * y * comp));
    }

    inline void image::swap(image & img) OPENVRML_NOTHROW
    {
        using std::swap;
        swap(this->x_, img.x_);
        swap(this->y_, img.y_);
        swap(this->comp_, img.comp_);
        swap(this->array_, img.array_);
    }

    inline std::size_t image::x() const OPENVRML_NOTHROW
    {
        return this->x_;
    }

    inline std::size_t image::y() const OPENVRML_NOTHROW
    {
        return this->y_;
    }

    inline std::size_t image::comp() const OPENVRML_NOTHROW
    {
        return this->comp_;
    }

    inline const std::vector<unsigned char> & image::array() const
        OPENVRML_NOTHROW
    {
        return this->array_;
    }

    inline void image::array(const std::vector<unsigned char> & value)
        OPENVRML_NOTHROW
    {
        assert(value.size() <= (this->x_ * this->y_ * this->comp_));
        std::copy(value.begin(), value.end(), this->array_.begin());
    }

    template <typename InputIterator>
    void image::array(InputIterator begin, InputIterator end) OPENVRML_NOTHROW
    {
        assert(std::distance(begin, end) <= this->x_ * this->y_ * this->comp_);
        std::copy(begin, end, this->array_.begin());
    }

    inline int32 image::pixel(const std::size_t index) const
        OPENVRML_NOTHROW
    {
        assert(index < this->x_ * this->y_);
        int32 retval = 0x00000000;
        for (std::size_t component = this->comp_, i = index * this->comp_;
             component > 0;
             --component, ++i) {
            retval |=
                static_cast<int32>(this->array_[i])
                << (8 * (component - 1));
        }
        return retval;
    }

    inline void image::pixel(const std::size_t index, const int32 value)
        OPENVRML_NOTHROW
    {
        assert(index < this->x_ * this->y_);
        for (std::size_t component = this->comp_, i = index * this->comp_;
             component > 0;
             --component, ++i) {
            this->array_[i] =
                static_cast<unsigned char>(
                    (value >> (8 * (component - 1))) & 0x000000ff);
        }
    }

    inline int32 image::pixel(const std::size_t x, const std::size_t y) const
        OPENVRML_NOTHROW
    {
        assert((x * y) < this->array_.size());
        return this->pixel(y * this->x_ + x);
    }

    inline void image::pixel(const std::size_t x,
                             const std::size_t y,
                             const int32 value)
        OPENVRML_NOTHROW
    {
        assert((x * y) < this->array_.size());
        this->pixel(y * this->x_ + x, value);
    }

    OPENVRML_API std::istream & operator>>(std::istream & in, image & img);
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const image & img);

} // namespace openvrml

namespace std {

    template <>
    OPENVRML_API inline void swap(openvrml::image & a, openvrml::image & b)
    {
        a.swap(b);
    }
}

# endif // OPENVRML_BASETYPES_H
