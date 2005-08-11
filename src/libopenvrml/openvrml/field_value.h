// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

# ifndef OPENVRML_FIELD_VALUE_H
#   define OPENVRML_FIELD_VALUE_H

#   include <memory>
#   include <string>
#   include <typeinfo>
#   include <boost/concept_check.hpp>
#   include <boost/shared_ptr.hpp>
#   include <boost/utility.hpp>
#   include <openvrml/basetypes.h>
#   include <openvrml/node_ptr.h>

namespace openvrml {

    class field_value;

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const field_value & value);

    class OPENVRML_API field_value {
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

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           field_value::type_id type_id);
    OPENVRML_API std::istream & operator>>(std::istream & in,
                                           field_value::type_id & type_id);


    template <typename T>
    struct FieldValueConcept {
        void constraints()
        {
            using boost::function_requires;
            using boost::ignore_unused_variable_warning;

            using boost::DefaultConstructibleConcept;
            using boost::CopyConstructibleConcept;
            using boost::AssignableConcept;
            using boost::EqualityComparableConcept;

            function_requires<DefaultConstructibleConcept<T> >();
            function_requires<CopyConstructibleConcept<T> >();
            function_requires<AssignableConcept<T> >();
            function_requires<EqualityComparableConcept<T> >();

            function_requires<
                DefaultConstructibleConcept<typename T::value_type> >();
            function_requires<AssignableConcept<typename T::value_type> >();

            //
            // Make sure T inherits field_value (not virtually).
            //
            field_value * base_ptr;
            T * fv = static_cast<T *>(base_ptr);
            ignore_unused_variable_warning(fv);

            //
            // Make sure T::field_value_type_id exists.
            //
            field_value::type_id id = T::field_value_type_id;
            ignore_unused_variable_warning(id);
        }
    };


    class OPENVRML_API sfbool : public field_value {
    public:
        typedef bool value_type;

        static const type_id field_value_type_id;

        bool value;

        explicit sfbool(bool value = false) throw ();
        virtual ~sfbool() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfbool & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sfbool & lhs, const sfbool & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfbool & lhs, const sfbool & rhs)
        throw ();


    class OPENVRML_API sfcolor : public field_value {
    public:
        typedef color value_type;

        static const type_id field_value_type_id;

        color value;

        explicit sfcolor(const color & value = color()) throw ();
        virtual ~sfcolor() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfcolor & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sfcolor & lhs, const sfcolor & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfcolor & lhs, const sfcolor & rhs)
        throw ();


    class OPENVRML_API sffloat : public field_value {
    public:
        typedef float value_type;

        static const type_id field_value_type_id;

        float value;

        explicit sffloat(float value = 0.0) throw ();
        virtual ~sffloat() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sffloat & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sffloat & lhs, const sffloat & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sffloat & lhs, const sffloat & rhs)
        throw ();


    class OPENVRML_API sfimage : public field_value {
    public:
        typedef image value_type;

        static const type_id field_value_type_id;

        image value;

        explicit sfimage(const image & value = image()) throw (std::bad_alloc);
        virtual ~sfimage() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfimage & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sfimage & lhs, const sfimage & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfimage & lhs, const sfimage & rhs)
        throw ();


    class OPENVRML_API sfint32 : public field_value {
    public:
        typedef int32 value_type;

        static const type_id field_value_type_id;

        int32 value;

        explicit sfint32(int32 value = 0) throw ();
        virtual ~sfint32() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfint32 & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfint32 & lhs, const sfint32 & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfint32 & lhs, const sfint32 & rhs)
        throw ();


    class OPENVRML_API sfnode : public field_value {
    public:
        typedef node_ptr value_type;

        static const type_id field_value_type_id;

        node_ptr value;

        explicit sfnode(const node_ptr & node = node_ptr(0)) throw ();
        virtual ~sfnode() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfnode & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfnode & lhs, const sfnode & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfnode & lhs, const sfnode & rhs)
        throw ();


    class OPENVRML_API sfrotation : public field_value {
    public:
        typedef rotation value_type;

        static const type_id field_value_type_id;

        rotation value;

        explicit sfrotation(const rotation & rot = rotation()) throw ();
        virtual ~sfrotation() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfrotation & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sfrotation & lhs,
                                 const sfrotation & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfrotation & lhs,
                                 const sfrotation & rhs)
        throw ();


    class OPENVRML_API sfstring : public field_value {
    public:
        typedef std::string value_type;

        static const type_id field_value_type_id;

        std::string value;

        explicit sfstring(const std::string & value = std::string())
                throw (std::bad_alloc);
        virtual ~sfstring() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfstring & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfstring & lhs, const sfstring & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfstring & lhs, const sfstring & rhs)
        throw ();


    class OPENVRML_API sftime : public field_value {
    public:
        typedef double value_type;

        static const type_id field_value_type_id;

        double value;

        explicit sftime(double value = 0.0) throw ();
        virtual ~sftime() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sftime & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sftime & lhs, const sftime & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sftime & lhs, const sftime & rhs)
        throw ();


    class OPENVRML_API sfvec2f : public field_value {
    public:
        typedef vec2f value_type;

        static const type_id field_value_type_id;

        vec2f value;

        explicit sfvec2f(const vec2f & vec = vec2f()) throw ();
        virtual ~sfvec2f() throw ();

        // Use compiler-defined copy ctor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfvec2f & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec2f & lhs, const sfvec2f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec2f & lhs, const sfvec2f & rhs)
        throw ();


    class OPENVRML_API sfvec3f : public field_value {
    public:
        typedef vec3f value_type;

        static const type_id field_value_type_id;

        vec3f value;

        explicit sfvec3f(const vec3f & vec = vec3f()) throw ();
        virtual ~sfvec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual sfvec3f & assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec3f & lhs, const sfvec3f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec3f & lhs, const sfvec3f & rhs)
        throw ();


    class OPENVRML_API mfcolor : public field_value {
    public:
        typedef std::vector<color> value_type;

        static const type_id field_value_type_id;

        std::vector<color> value;

        explicit mfcolor(std::vector<color>::size_type n = 0,
                         const color & value = color())
            throw (std::bad_alloc);
        explicit mfcolor(const std::vector<color> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfcolor(InputIterator first, InputIterator last);
        virtual ~mfcolor() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfcolor & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfcolor::mfcolor(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfcolor & lhs, const mfcolor & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfcolor & lhs, const mfcolor & rhs)
        throw ();


    class OPENVRML_API mffloat : public field_value {
    public:
        typedef std::vector<float> value_type;

        static const type_id field_value_type_id;

        std::vector<float> value;

        explicit mffloat(std::vector<float>::size_type n = 0,
                         float value = 0.0f)
            throw (std::bad_alloc);
        explicit mffloat(const std::vector<float> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mffloat(InputIterator first, InputIterator last);
        virtual ~mffloat() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mffloat & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mffloat::mffloat(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mffloat & lhs, const mffloat & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mffloat & lhs, const mffloat & rhs)
        throw ();


    class OPENVRML_API mfint32 : public field_value {
    public:
        typedef std::vector<int32> value_type;

        static const type_id field_value_type_id;

        std::vector<int32> value;

        explicit mfint32(std::vector<int32>::size_type n = 0, int32 value = 0)
            throw (std::bad_alloc);
        explicit mfint32(const std::vector<int32> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfint32(InputIterator first, InputIterator last);
        virtual ~mfint32() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfint32 & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfint32::mfint32(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfint32 & lhs, const mfint32 & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfint32 & lhs, const mfint32 & rhs)
        throw ();


    class OPENVRML_API mfnode : public field_value {
    public:
        typedef std::vector<node_ptr> value_type;

        static const type_id field_value_type_id;

        std::vector<node_ptr> value;

        explicit mfnode(std::vector<node_ptr>::size_type n = 0,
                        const node_ptr & value = node_ptr())
            throw (std::bad_alloc);
        explicit mfnode(const std::vector<node_ptr> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfnode(InputIterator first, InputIterator last);
        virtual ~mfnode() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfnode & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfnode::mfnode(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfnode & lhs, const mfnode & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfnode & lhs, const mfnode & rhs)
        throw ();


    class OPENVRML_API mfrotation : public field_value {
    public:
        typedef std::vector<rotation> value_type;

        static const type_id field_value_type_id;

        std::vector<rotation> value;

        explicit mfrotation(std::vector<rotation>::size_type n = 0,
                            const rotation & value = rotation())
            throw (std::bad_alloc);
        explicit mfrotation(const std::vector<rotation> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfrotation(InputIterator first, InputIterator last);
        virtual ~mfrotation() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfrotation & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfrotation::mfrotation(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfrotation & lhs,
                                 const mfrotation & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfrotation & lhs,
                                 const mfrotation & rhs)
        throw ();


    class OPENVRML_API mfstring : public field_value {
    public:
        typedef std::vector<std::string> value_type;

        static const type_id field_value_type_id;

        std::vector<std::string> value;

        explicit mfstring(std::vector<std::string>::size_type n = 0,
                          const std::string & value = std::string())
            throw (std::bad_alloc);
        explicit mfstring(const std::vector<std::string> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfstring(InputIterator first, InputIterator last);
        virtual ~mfstring() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfstring & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfstring::mfstring(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfstring & lhs, const mfstring & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfstring & lhs, const mfstring & rhs)
        throw ();


    class OPENVRML_API mftime : public field_value {
    public:
        typedef std::vector<double> value_type;

        static const type_id field_value_type_id;

        std::vector<double> value;

        explicit mftime(std::vector<double>::size_type n = 0,
                        double value = 0.0)
            throw (std::bad_alloc);
        explicit mftime(const std::vector<double> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mftime(InputIterator first, InputIterator last);
        virtual ~mftime() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mftime & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mftime::mftime(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mftime & lhs, const mftime & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mftime & lhs, const mftime & rhs)
        throw ();


    class OPENVRML_API mfvec2f : public field_value {
    public:
        typedef std::vector<vec2f> value_type;

        static const type_id field_value_type_id;

        std::vector<vec2f> value;

        explicit mfvec2f(std::vector<vec2f>::size_type n = 0,
                         const vec2f & value = vec2f())
            throw (std::bad_alloc);
        explicit mfvec2f(const std::vector<vec2f> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfvec2f(InputIterator first, InputIterator last);
        virtual ~mfvec2f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfvec2f & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfvec2f::mfvec2f(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfvec2f & lhs, const mfvec2f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec2f & lhs, const mfvec2f & rhs)
        throw ();


    class OPENVRML_API mfvec3f : public field_value {
    public:
        typedef std::vector<vec3f> value_type;

        static const type_id field_value_type_id;

        std::vector<vec3f> value;

        explicit mfvec3f(std::vector<vec3f>::size_type n = 0,
                         const vec3f & value = vec3f())
            throw (std::bad_alloc);
        explicit mfvec3f(const std::vector<vec3f> & value)
            throw (std::bad_alloc);
        template <typename InputIterator>
        mfvec3f(InputIterator first, InputIterator last);
        virtual ~mfvec3f() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual mfvec3f & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream &) const;
    };

    template <typename InputIterator>
    mfvec3f::mfvec3f(InputIterator first, InputIterator last):
        value(first, last)
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    OPENVRML_API bool operator==(const mfvec3f & lhs, const mfvec3f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec3f & lhs, const mfvec3f & rhs)
        throw ();
}

namespace std {

    template <>
    OPENVRML_API inline void swap(openvrml::mfcolor & a, openvrml::mfcolor & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mffloat & a, openvrml::mffloat & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfint32 & a, openvrml::mfint32 & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfnode & a, openvrml::mfnode & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfrotation & a,
                                  openvrml::mfrotation & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfstring & a,
                                  openvrml::mfstring & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mftime & a, openvrml::mftime & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec2f & a, openvrml::mfvec2f & b)
    {
        a.value.swap(b.value);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec3f & a, openvrml::mfvec3f & b)
    {
        a.value.swap(b.value);
    }
}

# endif
