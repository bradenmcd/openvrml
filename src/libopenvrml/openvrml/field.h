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

# ifndef OPENVRML_FIELD_H
#   define OPENVRML_FIELD_H

#   include <memory>
#   include <set>
#   include <stdexcept>
#   include <string>
#   include <typeinfo>
#   include <boost/cast.hpp>
#   include <boost/concept_check.hpp>
#   include <boost/shared_ptr.hpp>
#   include <boost/utility.hpp>
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

    typedef boost::shared_ptr<field_value> field_value_ptr;

    std::ostream & operator<<(std::ostream & out,
                              field_value::type_id type_id);
    std::istream & operator>>(std::istream & in,
                              field_value::type_id & type_id);


    template <typename T>
    struct FieldValueConcept {
        void constraints()
        {
            boost::function_requires<boost::DefaultConstructibleConcept<T> >();
            boost::function_requires<boost::CopyConstructibleConcept<T> >();
            boost::function_requires<boost::AssignableConcept<T> >();
            boost::function_requires<boost::EqualityComparableConcept<T> >();

            field_value * base_ptr;
            static_cast<T *>(base_ptr); // Make sure T inherits field_value.

            //
            // Make sure T::value_type is DefaultConstructible.
            //
            typename T::value_type v1;
            typename T::value_type v2;

            //
            // Make sure T::value_type is Assignable.
            //
            v1 = v2;
        }
    };


    class sfbool : public field_value {
    public:
        typedef bool value_type;

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

    bool operator==(const sfbool & lhs, const sfbool & rhs) throw ();
    bool operator!=(const sfbool & lhs, const sfbool & rhs) throw ();


    class sfcolor : public field_value {
    public:
        typedef color value_type;

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

    bool operator==(const sfcolor & lhs, const sfcolor & rhs) throw ();
    bool operator!=(const sfcolor & lhs, const sfcolor & rhs) throw ();


    class sffloat : public field_value {
    public:
        typedef float value_type;

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

    bool operator==(const sffloat & lhs, const sffloat & rhs) throw ();
    bool operator!=(const sffloat & lhs, const sffloat & rhs) throw ();


    class sfimage : public field_value {
    public:
        typedef image value_type;

        image value;

        explicit sfimage(const image & value = image()) throw (std::bad_alloc);
        virtual ~sfimage() throw ();

        // Use compiler-defined copy constructor and operator=.

        virtual std::auto_ptr<field_value> clone() const
            throw (std::bad_alloc);
        virtual field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id type() const throw ();

    private:
        virtual void print(std::ostream & out) const;
    };

    bool operator==(const sfimage & lhs, const sfimage & rhs) throw ();
    bool operator!=(const sfimage & lhs, const sfimage & rhs) throw ();


    class sfint32 : public field_value {
    public:
        typedef int32 value_type;

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

    bool operator==(const sfint32 & lhs, const sfint32 & rhs) throw ();
    bool operator!=(const sfint32 & lhs, const sfint32 & rhs) throw ();


    class sfnode : public field_value {
    public:
        typedef node_ptr value_type;

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

    bool operator==(const sfnode & lhs, const sfnode & rhs) throw ();
    bool operator!=(const sfnode & lhs, const sfnode & rhs) throw ();


    class sfrotation : public field_value {
    public:
        typedef rotation value_type;

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

    bool operator==(const sfrotation & lhs, const sfrotation & rhs) throw ();
    bool operator!=(const sfrotation & lhs, const sfrotation & rhs) throw ();


    class sfstring : public field_value {
    public:
        typedef std::string value_type;

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

    bool operator==(const sfstring & lhs, const sfstring & rhs) throw ();
    bool operator!=(const sfstring & lhs, const sfstring & rhs) throw ();


    class sftime : public field_value {
    public:
        typedef double value_type;

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

    bool operator==(const sftime & lhs, const sftime & rhs) throw ();
    bool operator!=(const sftime & lhs, const sftime & rhs) throw ();


    class sfvec2f : public field_value {
    public:
        typedef vec2f value_type;

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

    bool operator==(const sfvec2f & lhs, const sfvec2f & rhs) throw ();
    bool operator!=(const sfvec2f & lhs, const sfvec2f & rhs) throw ();


    class sfvec3f : public field_value {
    public:
        typedef vec3f value_type;

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

    bool operator==(const sfvec3f & lhs, const sfvec3f & rhs) throw ();
    bool operator!=(const sfvec3f & lhs, const sfvec3f & rhs) throw ();


    class mfcolor : public field_value {
    public:
        typedef std::vector<color> value_type;

        std::vector<color> value;

        explicit mfcolor(std::vector<color>::size_type n = 0,
                         const color & value = color())
            throw (std::bad_alloc);
        explicit mfcolor(const value_type & value)
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
    {
        using boost::function_requires;
        using boost::InputIteratorConcept;
        function_requires<InputIteratorConcept<InputIterator> >();
    }

    bool operator==(const mfcolor & lhs, const mfcolor & rhs) throw ();
    bool operator!=(const mfcolor & lhs, const mfcolor & rhs) throw ();


    class mffloat : public field_value {
    public:
        typedef std::vector<float> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mffloat & lhs, const mffloat & rhs) throw ();
    bool operator!=(const mffloat & lhs, const mffloat & rhs) throw ();


    class mfint32 : public field_value {
    public:
        typedef std::vector<int32> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfint32 & lhs, const mfint32 & rhs) throw ();
    bool operator!=(const mfint32 & lhs, const mfint32 & rhs) throw ();


    class mfnode : public field_value {
    public:
        typedef std::vector<node_ptr> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfnode & lhs, const mfnode & rhs) throw ();
    bool operator!=(const mfnode & lhs, const mfnode & rhs) throw ();


    class mfrotation : public field_value {
    public:
        typedef std::vector<rotation> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfrotation & lhs, const mfrotation & rhs) throw ();
    bool operator!=(const mfrotation & lhs, const mfrotation & rhs) throw ();


    class mfstring : public field_value {
    public:
        typedef std::vector<std::string> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfstring & lhs, const mfstring & rhs) throw ();
    bool operator!=(const mfstring & lhs, const mfstring & rhs) throw ();


    class mftime : public field_value {
    public:
        typedef std::vector<double> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mftime & lhs, const mftime & rhs) throw ();
    bool operator!=(const mftime & lhs, const mftime & rhs) throw ();


    class mfvec2f : public field_value {
    public:
        typedef std::vector<vec2f> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfvec2f & lhs, const mfvec2f & rhs) throw ();
    bool operator!=(const mfvec2f & lhs, const mfvec2f & rhs) throw ();


    class mfvec3f : public field_value {
    public:
        typedef std::vector<vec3f> value_type;

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
        virtual field_value & assign(const field_value & value)
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

    bool operator==(const mfvec3f & lhs, const mfvec3f & rhs) throw ();
    bool operator!=(const mfvec3f & lhs, const mfvec3f & rhs) throw ();


    class event_listener : boost::noncopyable {
    public:
        openvrml::node & node;

        virtual ~event_listener() throw () = 0;

    protected:
        explicit event_listener(openvrml::node & node) throw ();
    };


    template <typename FieldValue>
    class field_value_listener : public event_listener {
        BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

    public:
        virtual ~field_value_listener() throw () = 0;
        virtual void process_event(const FieldValue & value, double timestamp)
            throw (std::bad_alloc) = 0;

    protected:
        explicit field_value_listener(openvrml::node & node) throw ();
    };

    template <typename FieldValue>
    field_value_listener<FieldValue>::field_value_listener(
        openvrml::node & node)
        throw ():
        event_listener(node)
    {}

    template <typename FieldValue>
    field_value_listener<FieldValue>::~field_value_listener() throw ()
    {}

    typedef field_value_listener<sfbool> sfbool_listener;
    typedef field_value_listener<sfcolor> sfcolor_listener;
    typedef field_value_listener<sffloat> sffloat_listener;
    typedef field_value_listener<sfimage> sfimage_listener;
    typedef field_value_listener<sfint32> sfint32_listener;
    typedef field_value_listener<sfnode> sfnode_listener;
    typedef field_value_listener<sfrotation> sfrotation_listener;
    typedef field_value_listener<sfstring> sfstring_listener;
    typedef field_value_listener<sftime> sftime_listener;
    typedef field_value_listener<sfvec2f> sfvec2f_listener;
    typedef field_value_listener<sfvec3f> sfvec3f_listener;
    typedef field_value_listener<mfcolor> mfcolor_listener;
    typedef field_value_listener<mffloat> mffloat_listener;
    typedef field_value_listener<mfint32> mfint32_listener;
    typedef field_value_listener<mfnode> mfnode_listener;
    typedef field_value_listener<mfrotation> mfrotation_listener;
    typedef field_value_listener<mfstring> mfstring_listener;
    typedef field_value_listener<mftime> mftime_listener;
    typedef field_value_listener<mfvec2f> mfvec2f_listener;
    typedef field_value_listener<mfvec3f> mfvec3f_listener;


    class event_emitter : boost::noncopyable {
        friend class node;

    public:
        typedef std::set<event_listener *> listener_set;

        static std::auto_ptr<event_emitter> create(const field_value & value)
            throw (std::bad_alloc);

        const field_value & value;

        virtual ~event_emitter() throw () = 0;

        const listener_set & listeners() const throw ();

    protected:
        listener_set listeners_;
        double last_time;

        explicit event_emitter(const field_value & value) throw ();

    private:
        virtual void emit_event(double timestamp) throw (std::bad_alloc) = 0;
    };


    template <typename FieldValue>
    class field_value_emitter : public event_emitter {
        BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

    public:
        explicit field_value_emitter(const FieldValue & value) throw ();
        virtual ~field_value_emitter() throw ();

        bool add(field_value_listener<FieldValue> & listener)
            throw (std::bad_alloc);
        bool remove(field_value_listener<FieldValue> & listener) throw ();

    private:
        virtual void emit_event(double timestamp) throw (std::bad_alloc);
    };

    template <typename FieldValue>
    inline field_value_emitter<FieldValue>::field_value_emitter(
        const FieldValue & value)
        throw ():
        event_emitter(value)
    {}

    template <typename FieldValue>
    inline field_value_emitter<FieldValue>::~field_value_emitter() throw ()
    {}

    template <typename FieldValue>
    void field_value_emitter<FieldValue>::emit_event(const double timestamp)
        throw (std::bad_alloc)
    {
        for (typename listener_set::iterator listener =
                 this->listeners_.begin();
             listener != this->listeners_.end();
             ++listener) {
            using boost::polymorphic_downcast;
            assert(*listener);
            polymorphic_downcast<field_value_listener<FieldValue> *>(*listener)
                ->process_event(
                    *polymorphic_downcast<const FieldValue *>(&this->value),
                    timestamp);
        }
    }

    template <typename FieldValue>
    inline bool
    field_value_emitter<FieldValue>::
    add(field_value_listener<FieldValue> & listener) throw (std::bad_alloc)
    {
        return this->listeners_.insert(&listener).second;
    }

    template <typename FieldValue>
    inline bool
    field_value_emitter<FieldValue>::
    remove(field_value_listener<FieldValue> & listener) throw ()
    {
        return (this->listeners_.erase(&listener) > 0);
    }

    typedef field_value_emitter<sfbool> sfbool_emitter;
    typedef field_value_emitter<sfcolor> sfcolor_emitter;
    typedef field_value_emitter<sffloat> sffloat_emitter;
    typedef field_value_emitter<sfimage> sfimage_emitter;
    typedef field_value_emitter<sfint32> sfint32_emitter;
    typedef field_value_emitter<sfnode> sfnode_emitter;
    typedef field_value_emitter<sfrotation> sfrotation_emitter;
    typedef field_value_emitter<sfstring> sfstring_emitter;
    typedef field_value_emitter<sftime> sftime_emitter;
    typedef field_value_emitter<sfvec2f> sfvec2f_emitter;
    typedef field_value_emitter<sfvec3f> sfvec3f_emitter;
    typedef field_value_emitter<mfcolor> mfcolor_emitter;
    typedef field_value_emitter<mffloat> mffloat_emitter;
    typedef field_value_emitter<mfint32> mfint32_emitter;
    typedef field_value_emitter<mfnode> mfnode_emitter;
    typedef field_value_emitter<mfrotation> mfrotation_emitter;
    typedef field_value_emitter<mfstring> mfstring_emitter;
    typedef field_value_emitter<mftime> mftime_emitter;
    typedef field_value_emitter<mfvec2f> mfvec2f_emitter;
    typedef field_value_emitter<mfvec3f> mfvec3f_emitter;


    template <typename FieldValue>
    class exposedfield : public FieldValue,
                         public field_value_listener<FieldValue>,
                         public field_value_emitter<FieldValue> {
    public:
        exposedfield(openvrml::node & node,
                     const typename FieldValue::value_type & value =
                     typename FieldValue::value_type());
        virtual ~exposedfield() throw ();

        virtual void process_event(const FieldValue & value, double timestamp)
            throw (std::bad_alloc);

    private:
        virtual void do_process_event(const FieldValue & value,
                                      double timestamp)
            throw (std::bad_alloc);
    };

    template <typename FieldValue>
    inline exposedfield<FieldValue>::exposedfield(
        openvrml::node & node,
        const typename FieldValue::value_type & value):
        FieldValue(value),
        field_value_listener<FieldValue>(node),
        field_value_emitter<FieldValue>(static_cast<FieldValue &>(*this))
    {}

    template <typename FieldValue>
    inline exposedfield<FieldValue>::~exposedfield() throw ()
    {}

    template <typename FieldValue>
    inline void
    exposedfield<FieldValue>::process_event(const FieldValue & value,
                                            const double timestamp)
        throw (std::bad_alloc)
    {
        static_cast<FieldValue &>(*this) = value;
        this->do_process_event(value, timestamp);
        this->node.modified(true);
        node::emit_event(*this, timestamp);
    }

    template <typename FieldValue>
    inline void
    exposedfield<FieldValue>::do_process_event(const FieldValue & value,
                                               const double timestamp)
        throw (std::bad_alloc)
    {}
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
