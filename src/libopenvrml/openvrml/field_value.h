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
#   include <boost/cast.hpp>
#   include <boost/concept_check.hpp>
#   include <boost/intrusive_ptr.hpp>
#   include <boost/shared_ptr.hpp>
#   include <boost/utility.hpp>
#   include <boost/thread/mutex.hpp>
#   include <openvrml/basetypes.h>

namespace openvrml {

    class field_value;

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           const field_value & value);

    class OPENVRML_API field_value {
        friend std::ostream & operator<<(std::ostream & out,
                                         const field_value & value);

    protected:
        class counted_impl_base {
        public:
            virtual ~counted_impl_base() throw ();
            std::auto_ptr<counted_impl_base> clone() const
                throw (std::bad_alloc);

        private:
            virtual std::auto_ptr<counted_impl_base> do_clone() const
                throw (std::bad_alloc) = 0;
        };

        template <typename ValueType>
        class counted_impl : public counted_impl_base {
            mutable boost::mutex mutex_;
            boost::shared_ptr<ValueType> value_;

        public:
            explicit counted_impl(const ValueType & value)
                throw (std::bad_alloc);
            counted_impl(const counted_impl<ValueType> & ci) throw ();
            virtual ~counted_impl() throw ();

            const ValueType & value() const throw ();
            void value(const ValueType & val) throw (std::bad_alloc);

        private:
            counted_impl<ValueType> &
            operator=(const counted_impl<ValueType> &);

            virtual std::auto_ptr<counted_impl_base> do_clone() const
                throw (std::bad_alloc);
        };

    private:
        std::auto_ptr<counted_impl_base> counted_impl_;

    public:
        enum type_id {
            invalid_type_id,
            sfbool_id,
            sfcolor_id,
            sffloat_id,
            sfdouble_id,
            sfimage_id,
            sfint32_id,
            sfnode_id,
            sfrotation_id,
            sfstring_id,
            sftime_id,
            sfvec2f_id,
            sfvec2d_id,
            sfvec3f_id,
            sfvec3d_id,
            mfcolor_id,
            mffloat_id,
            mfdouble_id,
            mfint32_id,
            mfnode_id,
            mfrotation_id,
            mfstring_id,
            mftime_id,
            mfvec2f_id,
            mfvec2d_id,
            mfvec3f_id,
            mfvec3d_id
        };

        static std::auto_ptr<field_value> create(type_id type)
            throw (std::bad_alloc);

        virtual ~field_value() throw () = 0;

        std::auto_ptr<field_value> clone() const throw (std::bad_alloc);
        field_value & assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        type_id type() const throw ();

        template <typename FieldValue>
        const typename FieldValue::value_type & value() const throw ();

        template <typename FieldValue>
        void value(const typename FieldValue::value_type & val)
            throw (std::bad_alloc);

        template <typename FieldValue>
        void swap(FieldValue & val) throw ();

    protected:
        struct value_type_constructor_tag {};

        template <typename ValueType>
        field_value(const ValueType & value,
                    const value_type_constructor_tag &)
            throw (std::bad_alloc);

        field_value(const field_value & value) throw (std::bad_alloc);

        template <typename FieldValue>
        FieldValue & operator=(const FieldValue & value)
            throw (std::bad_alloc);

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc) = 0;

        virtual field_value & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc) = 0;

        virtual type_id do_type() const throw () = 0;

        virtual void print(std::ostream & out) const = 0;
    };

    template <typename ValueType>
    field_value::counted_impl<ValueType>::
    counted_impl(const ValueType & value) throw (std::bad_alloc):
        value_(new ValueType(value))
    {}

    template <typename ValueType>
    field_value::counted_impl<ValueType>::
    counted_impl(const counted_impl<ValueType> & ci) throw ():
        counted_impl_base()
    {
        boost::mutex::scoped_lock lock(ci.mutex_);
        value_ = ci.value_;
    }

    template <typename ValueType>
    field_value::counted_impl<ValueType>::~counted_impl() throw ()
    {}

    template <typename ValueType>
    const ValueType & field_value::counted_impl<ValueType>::value() const
        throw ()
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        assert(this->value_);
        return *this->value_;
    }

    template <typename ValueType>
    void field_value::counted_impl<ValueType>::value(const ValueType & val)
        throw (std::bad_alloc)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        assert(this->value_);
        if (!this->value_.unique()) {
            this->value_.reset(new ValueType(val));
        } else {
            *this->value_ = val;
        }
    }

    template <typename ValueType>
    std::auto_ptr<field_value::counted_impl_base>
    field_value::counted_impl<ValueType>::do_clone() const
        throw (std::bad_alloc)
    {
        return std::auto_ptr<counted_impl_base>(
            new counted_impl<ValueType>(*this));
    }

    template <typename ValueType>
    field_value::field_value(const ValueType & value,
                             const value_type_constructor_tag &)
        throw (std::bad_alloc):
        counted_impl_(new counted_impl<ValueType>(value))
    {}

    template <typename FieldValue>
    FieldValue & field_value::operator=(const FieldValue & fv)
        throw (std::bad_alloc)
    {
        if (this != &fv) {
            counted_impl_ = fv.counted_impl_->clone();
        }
        return *boost::polymorphic_downcast<FieldValue *>(this);
    }

    template <typename FieldValue>
    const typename FieldValue::value_type & field_value::value() const throw ()
    {
        assert(this->counted_impl_.get());
        return boost::polymorphic_downcast<
        counted_impl<typename FieldValue::value_type> *>(
            this->counted_impl_.get())->value();
    }

    template <typename FieldValue>
    void field_value::value(const typename FieldValue::value_type & val)
        throw (std::bad_alloc)
    {
        assert(this->counted_impl_.get());
        boost::polymorphic_downcast<
        counted_impl<typename FieldValue::value_type> *>(
            this->counted_impl_.get())->value(val);
    }

    template <typename FieldValue>
    void field_value::swap(FieldValue & val) throw ()
    {
        std::auto_ptr<counted_impl_base> temp = this->counted_impl_;
        this->counted_impl_ = val.counted_impl_;
        val.counted_impl_ = temp;
    }

    OPENVRML_API std::ostream & operator<<(std::ostream & out,
                                           field_value::type_id type_id);
    OPENVRML_API std::istream & operator>>(std::istream & in,
                                           field_value::type_id & type_id);


    template <typename T>
    struct FieldValueConcept {
        field_value * base_ptr;
        T * fv;
        field_value::type_id id;

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
            fv = static_cast<T *>(base_ptr);

            //
            // Make sure T::field_value_type_id exists.
            //
            id = T::field_value_type_id;
        }
    };


    class OPENVRML_API sfbool : public field_value {
    public:
        typedef bool value_type;

        static const type_id field_value_type_id;

        explicit sfbool(value_type value = false) throw ();
        sfbool(const sfbool & sfb);
        virtual ~sfbool() throw ();

        sfbool & operator=(const sfbool & sfb) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfbool & sfb) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfbool & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
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

        explicit sfcolor(const value_type & value = color()) throw ();
        sfcolor(const sfcolor & sfc);
        virtual ~sfcolor() throw ();

        sfcolor & operator=(const sfcolor & sfc) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfcolor & sfc) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfcolor & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
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

        explicit sffloat(float value = 0.0) throw ();
        sffloat(const sffloat & sff);
        virtual ~sffloat() throw ();

        sffloat & operator=(const sffloat & sff) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sffloat & sff) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sffloat & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sffloat & lhs, const sffloat & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sffloat & lhs, const sffloat & rhs)
        throw ();

    class OPENVRML_API sfdouble : public field_value {
    public:
        typedef double value_type;

        static const type_id field_value_type_id;

        explicit sfdouble(double value = 0.0) throw ();
        sfdouble(const sfdouble & sfd);
        virtual ~sfdouble() throw ();

        sfdouble & operator=(const sfdouble & sfd) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfdouble & sfd) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfdouble & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream & out) const;
    };

    OPENVRML_API bool operator==(const sfdouble & lhs, const sfdouble & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfdouble & lhs, const sfdouble & rhs)
        throw ();

    class OPENVRML_API sfimage : public field_value {
    public:
        typedef image value_type;

        static const type_id field_value_type_id;

        explicit sfimage(const image & value = image()) throw (std::bad_alloc);
        sfimage(const sfimage & sfi);
        virtual ~sfimage() throw ();

        sfimage & operator=(const sfimage & sfi) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfimage & sfi) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfimage & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
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

        explicit sfint32(int32 value = 0) throw ();
        sfint32(const sfint32 & sfi);
        virtual ~sfint32() throw ();

        sfint32 & operator=(const sfint32 & sfi) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfint32 & sfi) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfint32 & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfint32 & lhs, const sfint32 & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfint32 & lhs, const sfint32 & rhs)
        throw ();


    class node;
    void intrusive_ptr_add_ref(const node *) throw ();
    void intrusive_ptr_release(const node *) throw ();

    class OPENVRML_API sfnode : public field_value {
    public:
        typedef boost::intrusive_ptr<node> value_type;

        static const type_id field_value_type_id;

        explicit sfnode(const value_type & node = value_type(0)) throw ();
        sfnode(const sfnode & sfn);
        virtual ~sfnode() throw ();

        sfnode & operator=(const sfnode & sfn) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfnode & sfn) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfnode & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
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

        explicit sfrotation(const rotation & rot = rotation()) throw ();
        sfrotation(const sfrotation & sfr);
        virtual ~sfrotation() throw ();

        sfrotation & operator=(const sfrotation & sfr) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfrotation & sfr) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfrotation & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
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

        explicit sfstring(const std::string & value = std::string())
            throw (std::bad_alloc);
        sfstring(const sfstring & sfs);
        virtual ~sfstring() throw ();

        sfstring & operator=(const sfstring & sfs) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfstring & sfs) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfstring & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
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

        explicit sftime(double value = 0.0) throw ();
        sftime(const sftime & sft);
        virtual ~sftime() throw ();

        sftime & operator=(const sftime & sft) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sftime & sft) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sftime & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
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

        explicit sfvec2f(const vec2f & vec = vec2f()) throw ();
        sfvec2f(const sfvec2f & sfv);
        virtual ~sfvec2f() throw ();

        sfvec2f & operator=(const sfvec2f & sfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfvec2f & sfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfvec2f & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec2f & lhs, const sfvec2f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec2f & lhs, const sfvec2f & rhs)
        throw ();

    class OPENVRML_API sfvec2d : public field_value {
    public:
        typedef vec2d value_type;

        static const type_id field_value_type_id;

        explicit sfvec2d(const vec2d & vec = vec2d()) throw ();
        sfvec2d(const sfvec2d & sfv);
        virtual ~sfvec2d() throw ();

        sfvec2d & operator=(const sfvec2d & sfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfvec2d & sfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfvec2d & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec2d & lhs, const sfvec2d & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec2d & lhs, const sfvec2d & rhs)
        throw ();


    class OPENVRML_API sfvec3f : public field_value {
    public:
        typedef vec3f value_type;

        static const type_id field_value_type_id;

        explicit sfvec3f(const vec3f & vec = vec3f()) throw ();
        sfvec3f(const sfvec3f & sfv);
        virtual ~sfvec3f() throw ();

        sfvec3f & operator=(const sfvec3f & sfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfvec3f & sfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfvec3f & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec3f & lhs, const sfvec3f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec3f & lhs, const sfvec3f & rhs)
        throw ();

    class OPENVRML_API sfvec3d : public field_value {
    public:
        typedef vec3d value_type;

        static const type_id field_value_type_id;

        explicit sfvec3d(const vec3d & vec = vec3d()) throw ();
        sfvec3d(const sfvec3d & sfv);
        virtual ~sfvec3d() throw ();

        sfvec3d & operator=(const sfvec3d & sfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(sfvec3d & sfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual sfvec3d & do_assign(const field_value & value)
            throw (std::bad_cast);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const sfvec3d & lhs, const sfvec3d & rhs)
        throw ();
    OPENVRML_API bool operator!=(const sfvec3d & lhs, const sfvec3d & rhs)
        throw ();

    class OPENVRML_API mfcolor : public field_value {
    public:
        typedef std::vector<color> value_type;

        static const type_id field_value_type_id;

        explicit mfcolor(std::vector<color>::size_type n = 0,
                         const color & value = color())
            throw (std::bad_alloc);
        explicit mfcolor(const value_type & value) throw (std::bad_alloc);
        mfcolor(const mfcolor & mfc);
        virtual ~mfcolor() throw ();

        mfcolor & operator=(const mfcolor & mfc) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfcolor & mfc) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfcolor & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfcolor & lhs, const mfcolor & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfcolor & lhs, const mfcolor & rhs)
        throw ();


    class OPENVRML_API mffloat : public field_value {
    public:
        typedef std::vector<float> value_type;

        static const type_id field_value_type_id;

        explicit mffloat(std::vector<float>::size_type n = 0,
                         float value = 0.0f)
            throw (std::bad_alloc);
        explicit mffloat(const value_type & value) throw (std::bad_alloc);
        mffloat(const mffloat & mff);
        virtual ~mffloat() throw ();

        mffloat & operator=(const mffloat & mff) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mffloat & mff) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mffloat & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mffloat & lhs, const mffloat & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mffloat & lhs, const mffloat & rhs)
        throw ();

    class OPENVRML_API mfdouble : public field_value {
    public:
        typedef std::vector<double> value_type;

        static const type_id field_value_type_id;

        explicit mfdouble(std::vector<double>::size_type n = 0,
                          double value = 0.0f)
            throw (std::bad_alloc);
        explicit mfdouble(const value_type & value) throw (std::bad_alloc);
        mfdouble(const mfdouble & mfd);
        virtual ~mfdouble() throw ();

        mfdouble & operator=(const mfdouble & mfd) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfdouble & mfd) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfdouble & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfdouble & lhs, const mfdouble & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfdouble & lhs, const mfdouble & rhs)
        throw ();

    class OPENVRML_API mfint32 : public field_value {
    public:
        typedef std::vector<int32> value_type;

        static const type_id field_value_type_id;

        explicit mfint32(std::vector<int32>::size_type n = 0, int32 value = 0)
            throw (std::bad_alloc);
        explicit mfint32(const value_type & value) throw (std::bad_alloc);
        mfint32(const mfint32 & mfi);
        virtual ~mfint32() throw ();

        mfint32 & operator=(const mfint32 &) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfint32 & mfi) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfint32 & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfint32 & lhs, const mfint32 & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfint32 & lhs, const mfint32 & rhs)
        throw ();


    class OPENVRML_API mfnode : public field_value {
    public:
        typedef std::vector<boost::intrusive_ptr<node> > value_type;

        static const type_id field_value_type_id;

        explicit mfnode(value_type::size_type n = 0,
                        const value_type::value_type & value =
                        value_type::value_type())
            throw (std::bad_alloc);
        explicit mfnode(const value_type & value) throw (std::bad_alloc);
        mfnode(const mfnode & mfn);
        virtual ~mfnode() throw ();

        mfnode & operator=(const mfnode & mfn) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfnode & mfn) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfnode & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfnode & lhs, const mfnode & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfnode & lhs, const mfnode & rhs)
        throw ();


    class OPENVRML_API mfrotation : public field_value {
    public:
        typedef std::vector<rotation> value_type;

        static const type_id field_value_type_id;

        explicit mfrotation(std::vector<rotation>::size_type n = 0,
                            const rotation & value = rotation())
            throw (std::bad_alloc);
        explicit mfrotation(const value_type & value) throw (std::bad_alloc);
        mfrotation(const mfrotation & mfr);
        virtual ~mfrotation() throw ();

        mfrotation & operator=(const mfrotation & mfr) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfrotation & mfr) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfrotation & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

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

        explicit mfstring(std::vector<std::string>::size_type n = 0,
                          const std::string & value = std::string())
            throw (std::bad_alloc);
        explicit mfstring(const value_type & value) throw (std::bad_alloc);
        mfstring(const mfstring & mfs);
        virtual ~mfstring() throw ();

        mfstring & operator=(const mfstring & mfs) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfstring & mfs) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfstring & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfstring & lhs, const mfstring & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfstring & lhs, const mfstring & rhs)
        throw ();


    class OPENVRML_API mftime : public field_value {
    public:
        typedef std::vector<double> value_type;

        static const type_id field_value_type_id;

        explicit mftime(std::vector<double>::size_type n = 0,
                        double value = 0.0)
            throw (std::bad_alloc);
        explicit mftime(const value_type & value) throw (std::bad_alloc);
        mftime(const mftime & mft);
        virtual ~mftime() throw ();

        mftime & operator=(const mftime & mft) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mftime & mft) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mftime & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mftime & lhs, const mftime & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mftime & lhs, const mftime & rhs)
        throw ();


    class OPENVRML_API mfvec2f : public field_value {
    public:
        typedef std::vector<vec2f> value_type;

        static const type_id field_value_type_id;

        explicit mfvec2f(std::vector<vec2f>::size_type n = 0,
                         const vec2f & value = vec2f())
            throw (std::bad_alloc);
        explicit mfvec2f(const value_type & value) throw (std::bad_alloc);
        mfvec2f(const mfvec2f & mfv);
        virtual ~mfvec2f() throw ();

        mfvec2f & operator=(const mfvec2f & mfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfvec2f & mfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfvec2f & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfvec2f & lhs, const mfvec2f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec2f & lhs, const mfvec2f & rhs)
        throw ();


    class OPENVRML_API mfvec2d : public field_value {
    public:
        typedef std::vector<vec2d> value_type;

        static const type_id field_value_type_id;

        explicit mfvec2d(std::vector<vec2d>::size_type n = 0,
                         const vec2d & value = vec2d())
            throw (std::bad_alloc);
        explicit mfvec2d(const value_type & value) throw (std::bad_alloc);
        mfvec2d(const mfvec2d & mfv);
        virtual ~mfvec2d() throw ();

        mfvec2d & operator=(const mfvec2d & mfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfvec2d & mfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfvec2d & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfvec2d & lhs, const mfvec2d & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec2d & lhs, const mfvec2d & rhs)
        throw ();


    class OPENVRML_API mfvec3f : public field_value {
    public:
        typedef std::vector<vec3f> value_type;

        static const type_id field_value_type_id;

        explicit mfvec3f(std::vector<vec3f>::size_type n = 0,
                         const vec3f & value = vec3f())
            throw (std::bad_alloc);
        explicit mfvec3f(const value_type & value) throw (std::bad_alloc);
        mfvec3f(const mfvec3f & mfv);
        virtual ~mfvec3f() throw ();

        mfvec3f & operator=(const mfvec3f & mfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfvec3f & mfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfvec3f & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfvec3f & lhs, const mfvec3f & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec3f & lhs, const mfvec3f & rhs)
        throw ();

    class OPENVRML_API mfvec3d : public field_value {
    public:
        typedef std::vector<vec3d> value_type;

        static const type_id field_value_type_id;

        explicit mfvec3d(std::vector<vec3d>::size_type n = 0,
                         const vec3d & value = vec3d())
            throw (std::bad_alloc);
        explicit mfvec3d(const value_type & value) throw (std::bad_alloc);
        mfvec3d(const mfvec3d & mfv);
        virtual ~mfvec3d() throw ();

        mfvec3d & operator=(const mfvec3d & mfv) throw (std::bad_alloc);

        const value_type & value() const throw ();
        void value(const value_type & val) throw (std::bad_alloc);
        void swap(mfvec3d & mfv) throw ();

    private:
        virtual std::auto_ptr<field_value> do_clone() const
            throw (std::bad_alloc);
        virtual mfvec3d & do_assign(const field_value & value)
            throw (std::bad_cast, std::bad_alloc);
        virtual type_id do_type() const throw ();
        virtual void print(std::ostream &) const;
    };

    OPENVRML_API bool operator==(const mfvec3d & lhs, const mfvec3d & rhs)
        throw ();
    OPENVRML_API bool operator!=(const mfvec3d & lhs, const mfvec3d & rhs)
        throw ();
}

namespace std {

    template <>
    OPENVRML_API inline void swap(openvrml::sfbool & a, openvrml::sfbool & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfcolor & a, openvrml::sfcolor & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfdouble & a,
                                  openvrml::sfdouble & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sffloat & a, openvrml::sffloat & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfimage & a, openvrml::sfimage & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfint32 & a, openvrml::sfint32 & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfnode & a, openvrml::sfnode & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfrotation & a,
                                  openvrml::sfrotation & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfstring & a,
                                  openvrml::sfstring & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sftime & a, openvrml::sftime & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfvec2d & a, openvrml::sfvec2d & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfvec2f & a, openvrml::sfvec2f & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfvec3d & a, openvrml::sfvec3d & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::sfvec3f & a, openvrml::sfvec3f & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfcolor & a, openvrml::mfcolor & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfdouble & a,
                                  openvrml::mfdouble & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mffloat & a, openvrml::mffloat & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfint32 & a, openvrml::mfint32 & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfnode & a, openvrml::mfnode & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfrotation & a,
                                  openvrml::mfrotation & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfstring & a,
                                  openvrml::mfstring & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mftime & a, openvrml::mftime & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec2d & a, openvrml::mfvec2d & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec2f & a, openvrml::mfvec2f & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec3d & a, openvrml::mfvec3d & b)
    {
        a.swap(b);
    }

    template <>
    OPENVRML_API inline void swap(openvrml::mfvec3f & a, openvrml::mfvec3f & b)
    {
        a.swap(b);
    }
}

# endif
