// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
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

# ifndef OPENVRML_FIELD_H
#   define OPENVRML_FIELD_H

#   include <iosfwd>
#   include <memory>
#   include <stdexcept>
#   include <string>
#   include <typeinfo>
#   include <vector>
#   include <openvrml/basetypes.h>
#   include <openvrml/node_ptr.h>

namespace openvrml {

    class field_value;

    std::ostream & operator<<(std::ostream & out,
                              const field_value & value);

    class field_value {
        friend std::ostream & operator<<(std::ostream & out,
                                         const field_value & value);

    public:
        enum type_id {
            invalid_type_id,
            sfbool_id,
            sfcolor_id,
            sffloat_id,
            sfimage_id,
            sfint32_id,
            sfnode_id,
            sfrotation_id,
            sfstring_id,
            sftime_id,
            sfvec2f_id,
            sfvec3f_id,
            mfcolor_id,
            mffloat_id,
            mfint32_id,
            mfnode_id,
            mfrotation_id,
            mfstring_id,
            mftime_id,
            mfvec2f_id,
            mfvec3f_id
        };

        static std::auto_ptr<field_value> create(type_id type)
            throw (std::bad_alloc);

        virtual ~field_value() throw () = 0;

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc) = 0;
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc) = 0;
        virtual type_id type() const throw () = 0;

    protected:
        field_value() throw ();
        field_value(const field_value & value) throw ();
        field_value & operator=(const field_value & value) throw ();

    private:
        virtual void print(std::ostream & out) const = 0;
    };

    std::ostream & operator<<(std::ostream & out,
                              field_value::type_id type_id);
    std::istream & operator>>(std::istream & out,
                              field_value::type_id & type_id);


    class sfbool : public field_value {
    public:
        bool value;

        explicit sfbool(bool value = false) throw ();
        virtual ~sfbool() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const sfbool & lhs, const sfbool & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfbool & lhs, const sfbool & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfcolor : public field_value {
    public:
        color value;

        explicit sfcolor(const color & value = color()) throw ();
        virtual ~sfcolor() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const sfcolor & lhs, const sfcolor & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfcolor & lhs, const sfcolor & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sffloat : public field_value {
    public:
        float value;

        explicit sffloat(float value = 0.0) throw ();
        virtual ~sffloat() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const sffloat & lhs, const sffloat & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sffloat & lhs, const sffloat & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfimage : public field_value {
        size_t d_w, d_h, d_nc;
        unsigned char * d_pixels; // nc bytes/pixel, lower left to upper right

    public:
        sfimage() throw ();
        sfimage(size_t x, size_t y, size_t comp,
                const unsigned char * array = 0)
            throw (std::bad_alloc);
        sfimage(const sfimage &) throw (std::bad_alloc);
        virtual ~sfimage() throw ();

        sfimage & operator=(const sfimage & sfimage) throw (std::bad_alloc);

        size_t x() const throw ();
        size_t y() const throw ();
        size_t comp() const throw ();
        const unsigned char * array() const throw ();
        void set(size_t x, size_t y, size_t comp,
                 const unsigned char * array) throw (std::bad_alloc);

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };


    class sfint32 : public field_value {
    public:
        int32 value;

        explicit sfint32(int32 value = 0) throw ();
        virtual ~sfint32() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sfint32 & lhs, const sfint32 & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfint32 & lhs, const sfint32 & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfnode : public field_value {
    public:
        node_ptr value;

        explicit sfnode(const node_ptr & node = node_ptr(0)) throw ();
        virtual ~sfnode() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sfnode & lhs, const sfnode & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfnode & lhs, const sfnode & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfrotation : public field_value {
    public:
        rotation value;

        explicit sfrotation(const rotation & rot = rotation()) throw ();
        virtual ~sfrotation() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    inline bool operator==(const sfrotation & lhs, const sfrotation & rhs)
        throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfrotation & lhs, const sfrotation & rhs)
        throw ()
    {
        return !(lhs == rhs);
    }


    class sfstring : public field_value {
    public:
        std::string value;

        explicit sfstring(const std::string & value = std::string())
                throw (std::bad_alloc);
        virtual ~sfstring() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sfstring & lhs, const sfstring & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfstring & lhs, const sfstring & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sftime : public field_value {
    public:
        double value;

        explicit sftime(double value = 0.0) throw ();
        virtual ~sftime() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sftime & lhs, const sftime & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sftime & lhs, const sftime & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfvec2f : public field_value {
    public:
        vec2f value;

        explicit sfvec2f(const vec2f & vec = vec2f()) throw ();
        virtual ~sfvec2f() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class sfvec3f : public field_value {
    public:
        vec3f value;

        explicit sfvec3f(const vec3f & vec = vec3f()) throw ();
        virtual ~sfvec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    inline bool operator==(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
    {
        return !(lhs == rhs);
    }


    class mfcolor : public field_value {
    public:
        std::vector<color> value;

        explicit mfcolor(std::vector<color>::size_type n = 0,
                         const color & value = color())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfcolor(InputIterator first, InputIterator last);
        virtual ~mfcolor() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfcolor::mfcolor(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfcolor & lhs, const mfcolor & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfcolor & lhs, const mfcolor & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mffloat : public field_value {
    public:
        std::vector<float> value;

        explicit mffloat(std::vector<float>::size_type n = 0,
                         float value = 0.0f)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mffloat(InputIterator first, InputIterator last);
        virtual ~mffloat() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mffloat::mffloat(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mffloat & lhs, const mffloat & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mffloat & lhs, const mffloat & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfint32 : public field_value {
    public:
        std::vector<int32> value;

        explicit mfint32(std::vector<int32>::size_type n = 0, int32 value = 0)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfint32(InputIterator first, InputIterator last);
        virtual ~mfint32() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfint32::mfint32(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfint32 & lhs, const mfint32 & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfint32 & lhs, const mfint32 & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfnode : public field_value {
    public:
        std::vector<node_ptr> value;

        explicit mfnode(std::vector<node_ptr>::size_type n = 0,
                        const node_ptr & value = node_ptr())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfnode(InputIterator first, InputIterator last);
        virtual ~mfnode() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfnode::mfnode(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfnode & lhs, const mfnode & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfnode & lhs, const mfnode & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfrotation : public field_value {
    public:
        std::vector<rotation> value;

        explicit mfrotation(std::vector<rotation>::size_type n = 0,
                            const rotation & value = rotation())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfrotation(InputIterator first, InputIterator last);
        virtual ~mfrotation() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfrotation::mfrotation(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfrotation & lhs, const mfrotation & rhs)
        throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfrotation & lhs, const mfrotation & rhs)
        throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfstring : public field_value {
    public:
        std::vector<std::string> value;

        explicit mfstring(std::vector<std::string>::size_type n = 0,
                          const std::string & value = std::string())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfstring(InputIterator first, InputIterator last);
        virtual ~mfstring() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfstring::mfstring(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfstring & lhs, const mfstring & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfstring & lhs, const mfstring & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mftime : public field_value {
    public:
        std::vector<double> value;

        explicit mftime(std::vector<double>::size_type n = 0,
                        double value = 0.0)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mftime(InputIterator first, InputIterator last);
        virtual ~mftime() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mftime::mftime(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mftime & lhs, const mftime & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mftime & lhs, const mftime & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfvec2f : public field_value {
    public:
        std::vector<vec2f> value;

        explicit mfvec2f(std::vector<vec2f>::size_type n = 0,
                         const vec2f & value = vec2f())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfvec2f(InputIterator first, InputIterator last);
        virtual ~mfvec2f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfvec2f::mfvec2f(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }


    class mfvec3f : public field_value {
    public:
        std::vector<vec3f> value;

        explicit mfvec3f(std::vector<vec3f>::size_type n = 0,
                         const vec3f & value = vec3f())
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfvec3f(InputIterator first, InputIterator last);
        virtual ~mfvec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfvec3f::mfvec3f(InputIterator first, InputIterator last):
        value(first, last)
    {}

    inline bool operator==(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
    {
        return lhs.value != rhs.value;
    }
}

namespace std {

    template <>
    inline void swap(openvrml::mfcolor & a, openvrml::mfcolor & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mffloat & a, openvrml::mffloat & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfint32 & a, openvrml::mfint32 & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfnode & a, openvrml::mfnode & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfrotation & a, openvrml::mfrotation & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfstring & a, openvrml::mfstring & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mftime & a, openvrml::mftime & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfvec2f & a, openvrml::mfvec2f & b)
    {
        a.value.swap(b.value);
    }

    template <>
    inline void swap(openvrml::mfvec3f & a, openvrml::mfvec3f & b)
    {
        a.value.swap(b.value);
    }
}

# endif
