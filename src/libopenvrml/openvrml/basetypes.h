// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2001  S. K. Bose
// Copyright 2003, 2004  Braden McDaniel
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

#   include <openvrml-common.h>
#   include <cassert>
#   include <cmath>
#   include <cstddef>
#   include <algorithm>
#   include <iosfwd>
#   include <vector>
#   include <sys/types.h>

namespace openvrml {

#   ifdef _WIN32
    typedef __int32 int32;
#   else
    typedef int32_t int32;
#   endif

    class OPENVRML_API color {
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

    OPENVRML_API bool operator==(const color & lhs, const color & rhs)
        throw ();
    OPENVRML_API bool operator!=(const color & lhs, const color & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const color & c);

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


    class OPENVRML_API color_rgba {
        float rgba[4];

    public:
        color_rgba() throw ();
        explicit color_rgba(const float (&rgba)[4]) throw ();
        color_rgba(float r, float g, float b, float a = 1.0f) throw ();

        const float & operator[](size_t index) const throw ();

        float r() const throw ();
        float g() const throw ();
        float b() const throw ();
        float a() const throw ();

        void r(float value) throw ();
        void g(float value) throw ();
        void b(float value) throw ();
        void a(float value) throw ();

        void hsv(float (&result)[4]) const throw ();
        void hsv(float h, float s, float v, float a = 1.0f) throw ();
    };

    OPENVRML_API bool operator==(const color_rgba & lhs,
                                 const color_rgba & rhs)
        throw ();
    OPENVRML_API bool operator!=(const color_rgba & lhs,
                                 const color_rgba & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const color_rgba & c);

    inline const float & color_rgba::operator[](const size_t index) const
        throw ()
    {
        assert(index < 4);
        return this->rgba[index];
    }

    inline float color_rgba::r() const throw ()
    {
        return this->rgba[0];
    }

    inline float color_rgba::g() const throw ()
    {
        return this->rgba[1];
    }

    inline float color_rgba::b() const throw ()
    {
        return this->rgba[2];
    }

    inline float color_rgba::a() const throw ()
    {
        return this->rgba[3];
    }

    inline void color_rgba::r(const float value) throw ()
    {
        this->rgba[0] = value;
    }

    inline void color_rgba::g(const float value) throw ()
    {
        this->rgba[1] = value;
    }

    inline void color_rgba::b(const float value) throw ()
    {
        this->rgba[2] = value;
    }

    inline void color_rgba::a(const float value) throw ()
    {
        this->rgba[3] = value;
    }


    template<typename T>
    class OPENVRML_API vec2 {
        T vec[2];

    public:
        vec2() throw ();
        explicit vec2(const T (&vec)[2]) throw ();
        explicit vec2(T x, T y) throw ();

        vec2<T> & operator*=(T scalar) throw ();
        vec2<T> & operator/=(T scalar) throw ();
        vec2<T> & operator+=(const vec2<T> & vec) throw ();
        vec2<T> & operator-=(const vec2<T> & vec) throw ();

        const vec2<T> operator-() const throw ();
        const vec2<T> operator*(T rhs) const throw ();
        const vec2<T> operator/(T rhs) const throw ();
        const vec2<T> operator+(const vec2<T> & rhs) const throw ();
        const vec2<T> operator-(const vec2<T> & rhs) const throw ();
        bool operator==(const vec2<T> & rhs) const throw ();
        bool operator!=(const vec2<T> & rhs) const throw ();

        const T & operator[](size_t index) const throw ();

        T x() const throw ();
        T y() const throw ();

        void x(T value) throw ();
        void y(T value) throw ();

        T dot(const vec2<T> & vec) const throw ();
        T length() const throw ();
        const vec2<T> normalize() const throw ();
    };

    OPENVRML_API const vec2<float> operator*(float lhs,
                                             const vec2<float> & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec2<float> & v);
    OPENVRML_API const vec2<double> operator*(double lhs,
                                              const vec2<double> & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec2<double> & v);

    template<typename T>
    inline const T & vec2<T>::operator[](const size_t index) const throw ()
    {
        assert(index < 2);
        return this->vec[index];
    }

    template<typename T>
    inline T vec2<T>::x() const throw ()
    {
        return this->vec[0];
    }

    template<typename T>
    inline T vec2<T>::y() const throw ()
    {
        return this->vec[1];
    }

    template<typename T>
    inline void vec2<T>::x(const T value) throw ()
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    template<typename T>
    inline void vec2<T>::y(const T value) throw ()
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }

    typedef vec2<float> vec2f;
    typedef vec2<double> vec2d;


    class mat4f;

    template<typename T>
    class OPENVRML_API vec3 {
        T vec[3];

    public:
        vec3() throw ();
        explicit vec3(const T (&vec)[3]) throw ();
        vec3(T x, T y, T z) throw ();

        vec3<T> & operator*=(const vec3<T> & vec) throw ();
        vec3<T> & operator*=(const mat4f & mat) throw ();
        vec3<T> & operator*=(T scalar) throw ();
        vec3<T> & operator/=(T scalar) throw ();
        vec3<T> & operator+=(const vec3<T> & vec) throw ();
        vec3<T> & operator-=(const vec3<T> & vec) throw ();

        const vec3<T> operator-() const throw ();
        const vec3<T> operator*(const vec3<T> & rhs) const throw ();
        const vec3<T> operator*(const T rhs) const throw ();
        const vec3<T> operator*(const mat4f & mat) const throw ();
        const vec3<T> operator/(const T rhs) const throw ();
        const vec3<T> operator+(const vec3<T> & rhs) const throw ();
        const vec3<T> operator-(const vec3<T> & rhs) const throw ();
        bool operator==(const vec3<T> & rhs) const throw ();
        bool operator!=(const vec3<T> & rhs) const throw ();

        const T & operator[](size_t index) const throw ();

        T x() const throw ();
        T y() const throw ();
        T z() const throw ();

        void x(T value) throw ();
        void y(T value) throw ();
        void z(T value) throw ();

        T dot(const vec3<T> & vec) const throw ();
        T length() const throw ();
        const vec3<T> normalize() const throw ();
    };

    OPENVRML_API const vec3<float> operator*(const float lhs,
                                             const vec3<float> & rhs)
        throw ();
    OPENVRML_API const vec3<float> operator*(const mat4f & mat,
                                             const vec3<float> & vec)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec3<float> & v);
    OPENVRML_API const vec3<double> operator*(const double lhs,
                                              const vec3<double> & rhs)
        throw ();
    OPENVRML_API const vec3<double> operator*(const mat4f & mat,
                                              const vec3<double> & vec)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const vec3<double> & v);


    template<typename T>
    inline const T & vec3<T>::operator[](const size_t index) const throw ()
    {
        assert(index < 3);
        return this->vec[index];
    }

    template<typename T>
    inline T vec3<T>::x() const throw ()
    {
        return this->vec[0];
    }

    template<typename T>
    inline T vec3<T>::y() const throw ()
    {
        return this->vec[1];
    }

    template<typename T>
    inline T vec3<T>::z() const throw ()
    {
        return this->vec[2];
    }

    template<typename T>
    inline void vec3<T>::x(const T value) throw ()
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[0] = value;
    }

    template<typename T>
    inline void vec3<T>::y(const T value) throw ()
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[1] = value;
    }

    template<typename T>
    inline void vec3<T>::z(const T value) throw ()
    {
        //
        // Ensure value is not NaN.
        //
        assert(value == value);

        this->vec[2] = value;
    }

    typedef vec3<float> vec3f;
    typedef vec3<double> vec3d;


    class quatf;

    class OPENVRML_API rotation {
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
        void axis(const vec3f & axis) throw ();
        void angle(float value) throw ();

        const rotation inverse() const throw ();
        const rotation slerp(const rotation & dest_rot, float t) const
            throw ();
    };

    const rotation OPENVRML_API operator*(const rotation & lhs,
                                          const rotation & rhs)
        throw ();
    OPENVRML_API bool operator==(const rotation & lhs, const rotation & rhs)
        throw ();
    OPENVRML_API bool operator!=(const rotation & lhs, const rotation & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const rotation & r);

    inline const float & rotation::operator[](const size_t index) const
        throw ()
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


    class OPENVRML_API mat4f {
        float mat[4][4];

    public:
        static const mat4f rotation(const openvrml::rotation & rot) throw ();
        static const mat4f rotation(const quatf & quat) throw ();
        static const mat4f scale(float s) throw ();
        static const mat4f scale(const vec3f & s) throw ();
        static const mat4f translation(const vec3f & t) throw ();
        static const mat4f transformation(const vec3f & t,
                                          const openvrml::rotation & r,
                                          const vec3f & s,
                                          const openvrml::rotation & sr,
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

        float (&operator[](const size_t index) throw ())[4];
        const float (&operator[](const size_t index) const throw ())[4];

        void transformation(vec3f & t, openvrml::rotation & r, vec3f & s) const
            throw ();
        void transformation(vec3f & t, openvrml::rotation & r, vec3f & s,
                            vec3f & shear) const
            throw ();

        const mat4f inverse() const throw ();
        const mat4f transpose() const throw ();
        float det() const throw ();
    };

    OPENVRML_API bool operator==(const mat4f & lhs, const mat4f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mat4f & lhs, const mat4f & rhs)
        throw ();

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

    OPENVRML_API const mat4f operator*(const mat4f & lhs, const mat4f & rhs)
        throw ();
    OPENVRML_API const mat4f operator*(const mat4f & mat, float scalar)
        throw ();
    OPENVRML_API const mat4f operator*(float scalar, const mat4f & mat)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const mat4f & mat);


    class OPENVRML_API quatf {
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

    OPENVRML_API const quatf operator*(const quatf & lhs, const quatf & rhs)
        throw ();
    OPENVRML_API const quatf operator*(const quatf & quat, const float scalar)
        throw ();
    OPENVRML_API const quatf operator*(const float scalar, const quatf & quat)
        throw ();
    OPENVRML_API const quatf operator/(const quatf & quat, const float scalar)
        throw ();
    OPENVRML_API const quatf operator+(const quatf & lhs, const quatf & rhs)
        throw ();
    OPENVRML_API const quatf operator-(const quatf & lhs, const quatf & rhs)
        throw ();
    OPENVRML_API bool operator==(const quatf & lhs, const quatf & rhs)
        throw ();
    OPENVRML_API bool operator!=(const quatf & lhs, const quatf & rhs)
        throw ();
    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const quatf & quat);

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


    class OPENVRML_API image {
        size_t x_;
        size_t y_;
        size_t comp_;
        std::vector<unsigned char> array_;

    public:
        image() throw ();
        image(size_t x, size_t y, size_t comp) throw (std::bad_alloc);
        image(size_t x, size_t y, size_t comp,
              const std::vector<unsigned char> & array) throw (std::bad_alloc);
        template <typename InputIterator>
        image(size_t x, size_t y, size_t comp,
              InputIterator array_begin, InputIterator array_end)
            throw (std::bad_alloc);

        // Use compiler-defined operator= and copy constructor.

        void swap(image & img) throw ();

        size_t x() const throw ();
        void x(size_t value) throw (std::bad_alloc);

        size_t y() const throw ();
        void y(size_t value) throw (std::bad_alloc);

        void resize(size_t x, size_t y) throw (std::bad_alloc);

        size_t comp() const throw ();
        void comp(size_t value) throw (std::bad_alloc);

        const std::vector<unsigned char> & array() const throw ();
        void array(const std::vector<unsigned char> & value) throw ();
        template <typename InputIterator>
        void array(InputIterator begin, InputIterator end) throw ();

        unsigned long pixel(size_t index) const throw ();
        void pixel(size_t index, unsigned long value) throw ();
        unsigned long pixel(size_t x, size_t y) const throw ();
        void pixel(size_t x, size_t y, unsigned long value) throw ();
    };

    OPENVRML_API bool operator==(const image & lhs, const image & rhs)
        throw ();
    OPENVRML_API bool operator!=(const image & lhs, const image & rhs)
        throw ();

    template <typename InputIterator>
    image::image(const size_t x,
                 const size_t y,
                 const size_t comp,
                 InputIterator array_begin,
                 InputIterator array_end)
        throw (std::bad_alloc):
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

    inline size_t image::x() const throw ()
    {
        return this->x_;
    }

    inline size_t image::y() const throw ()
    {
        return this->y_;
    }

    inline size_t image::comp() const throw ()
    {
        return this->comp_;
    }

    inline const std::vector<unsigned char> & image::array() const throw ()
    {
        return this->array_;
    }

    inline void image::array(const std::vector<unsigned char> & value) throw ()
    {
        assert(value.size() <= (this->x_ * this->y_ * this->comp_));
        std::copy(value.begin(), value.end(), this->array_.begin());
    }

    template <typename InputIterator>
    void image::array(InputIterator begin, InputIterator end) throw ()
    {
        assert(std::distance(begin, end) <= this->x_ * this->y_ * this->comp_);
        std::copy(begin, end, this->array_.begin());
    }

    inline unsigned long image::pixel(const size_t index) const throw ()
    {
        assert(index < this->x_ * this->y_);
        unsigned long retval = 0x00000000;
        for (size_t component = this->comp_, i = index * this->comp_;
             component > 0;
             --component, ++i) {
            retval |=
                static_cast<unsigned long>(this->array_[i])
                << (8 * (component - 1));
        }
        return retval;
    }

    inline void image::pixel(const size_t index, const unsigned long value)
        throw ()
    {
        assert(index < this->x_ * this->y_);
        for (size_t component = this->comp_, i = index * this->comp_;
             component > 0;
             --component, ++i) {
            this->array_[i] =
                static_cast<unsigned char>(
                    (value >> (8 * (component - 1))) & 0x000000ff);
        }
    }

    inline unsigned long image::pixel(const size_t x, const size_t y) const
        throw ()
    {
        assert((x * y) < this->array_.size());
        return this->pixel(y * this->x_ + x);
    }

    inline void image::pixel(const size_t x,
                             const size_t y,
                             const unsigned long value)
        throw ()
    {
        assert((x * y) < this->array_.size());
        this->pixel(y * this->x_ + x, value);
    }

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
