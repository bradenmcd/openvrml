// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2004, 2005  Braden McDaniel
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

# ifndef OPENVRML_EVENT_H
#   define OPENVRML_EVENT_H

#   include <set>
#   include <boost/cast.hpp>
#   include <boost/thread/recursive_mutex.hpp>
#   include <openvrml/field_value.h>

namespace openvrml {

    class node;

    class OPENVRML_API event_listener : boost::noncopyable {
    public:
        virtual ~event_listener() OPENVRML_NOTHROW = 0;

        virtual field_value::type_id type() const OPENVRML_NOTHROW = 0;

    protected:
        event_listener() OPENVRML_NOTHROW;
    };


    class OPENVRML_API node_event_listener : public virtual event_listener {
        openvrml::node * node_;

    public:
        virtual ~node_event_listener() OPENVRML_NOTHROW;

        openvrml::node & node() const OPENVRML_NOTHROW;
        const std::string eventin_id() const OPENVRML_NOTHROW;

    protected:
        explicit node_event_listener(openvrml::node & n) OPENVRML_NOTHROW;

    private:
        virtual const std::string do_eventin_id() const OPENVRML_NOTHROW = 0;
    };


    template <typename FieldValue>
    class field_value_listener : public virtual event_listener {
        BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

    public:
        virtual ~field_value_listener() OPENVRML_NOTHROW = 0;
        void process_event(const FieldValue & value, double timestamp)
            OPENVRML_THROW1(std::bad_alloc);

        virtual field_value::type_id type() const OPENVRML_NOTHROW;

    protected:
        explicit field_value_listener() OPENVRML_NOTHROW;

    private:
        virtual void do_process_event(const FieldValue & value,
                                      double timestamp)
            OPENVRML_THROW1(std::bad_alloc) = 0;
    };

    template <typename FieldValue>
    field_value_listener<FieldValue>::field_value_listener() OPENVRML_NOTHROW
    {}

    template <typename FieldValue>
    field_value_listener<FieldValue>::~field_value_listener() OPENVRML_NOTHROW
    {}

    template <typename FieldValue>
    void
    field_value_listener<FieldValue>::process_event(const FieldValue & value,
                                                    const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        this->do_process_event(value, timestamp);
    }

    template <typename FieldValue>
    field_value::type_id field_value_listener<FieldValue>::type() const
        OPENVRML_NOTHROW
    {
        return FieldValue::field_value_type_id;
    }

    typedef field_value_listener<sfbool> sfbool_listener;
    typedef field_value_listener<sfcolor> sfcolor_listener;
    typedef field_value_listener<sfcolorrgba> sfcolorrgba_listener;
    typedef field_value_listener<sffloat> sffloat_listener;
    typedef field_value_listener<sfdouble> sfdouble_listener;
    typedef field_value_listener<sfimage> sfimage_listener;
    typedef field_value_listener<sfint32> sfint32_listener;
    typedef field_value_listener<sfnode> sfnode_listener;
    typedef field_value_listener<sfrotation> sfrotation_listener;
    typedef field_value_listener<sfstring> sfstring_listener;
    typedef field_value_listener<sftime> sftime_listener;
    typedef field_value_listener<sfvec2f> sfvec2f_listener;
    typedef field_value_listener<sfvec2d> sfvec2d_listener;
    typedef field_value_listener<sfvec3f> sfvec3f_listener;
    typedef field_value_listener<sfvec3d> sfvec3d_listener;
    typedef field_value_listener<mfbool> mfbool_listener;
    typedef field_value_listener<mfcolor> mfcolor_listener;
    typedef field_value_listener<mfcolorrgba> mfcolorrgba_listener;
    typedef field_value_listener<mffloat> mffloat_listener;
    typedef field_value_listener<mfdouble> mfdouble_listener;
    typedef field_value_listener<mfimage> mfimage_listener;
    typedef field_value_listener<mfint32> mfint32_listener;
    typedef field_value_listener<mfnode> mfnode_listener;
    typedef field_value_listener<mfrotation> mfrotation_listener;
    typedef field_value_listener<mfstring> mfstring_listener;
    typedef field_value_listener<mftime> mftime_listener;
    typedef field_value_listener<mfvec2f> mfvec2f_listener;
    typedef field_value_listener<mfvec2d> mfvec2d_listener;
    typedef field_value_listener<mfvec3f> mfvec3f_listener;
    typedef field_value_listener<mfvec3d> mfvec3d_listener;


    template <typename FieldValue>
    class node_field_value_listener :
        public virtual node_event_listener,
        public field_value_listener<FieldValue> {
    public:
        virtual ~node_field_value_listener() OPENVRML_NOTHROW;

    protected:
        explicit node_field_value_listener(openvrml::node & n)
            OPENVRML_NOTHROW;
    };

    template <typename FieldValue>
    node_field_value_listener<FieldValue>::
    node_field_value_listener(openvrml::node & n)
        OPENVRML_NOTHROW:
        node_event_listener(n)
    {}

    template <typename FieldValue>
    node_field_value_listener<FieldValue>::~node_field_value_listener()
        OPENVRML_NOTHROW
    {}


    class OPENVRML_API event_emitter : boost::noncopyable {
        friend class node;

        mutable boost::recursive_mutex mutex_;
        const field_value & value_;
        std::set<event_listener *> listeners_;
        double last_time_;

    public:
        typedef std::set<event_listener *> listener_set;

        virtual ~event_emitter() OPENVRML_NOTHROW = 0;

        const field_value & value() const OPENVRML_NOTHROW;
        const std::string eventout_id() const OPENVRML_NOTHROW;
        const listener_set & listeners() const OPENVRML_NOTHROW;
        double last_time() const OPENVRML_NOTHROW;

    protected:
        boost::recursive_mutex & mutex() const OPENVRML_NOTHROW;
        listener_set & listeners() OPENVRML_NOTHROW;
        void last_time(double t) OPENVRML_NOTHROW;

        explicit event_emitter(const field_value & value) OPENVRML_NOTHROW;

    private:
        virtual const std::string do_eventout_id() const OPENVRML_NOTHROW = 0;
        virtual void emit_event(double timestamp)
            OPENVRML_THROW1(std::bad_alloc) = 0;
    };


    template <typename FieldValue>
    class field_value_emitter : public virtual event_emitter {
        BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

    public:
        explicit field_value_emitter(const FieldValue & value)
            OPENVRML_NOTHROW;
        virtual ~field_value_emitter() OPENVRML_NOTHROW = 0;

        bool add(field_value_listener<FieldValue> & listener)
            OPENVRML_THROW1(std::bad_alloc);
        bool remove(field_value_listener<FieldValue> & listener)
            OPENVRML_NOTHROW;

    private:
        virtual void emit_event(double timestamp) OPENVRML_THROW1(std::bad_alloc);
    };

    template <typename FieldValue>
    inline field_value_emitter<FieldValue>::field_value_emitter(
        const FieldValue & value)
        OPENVRML_NOTHROW:
        event_emitter(value)
    {}

    template <typename FieldValue>
    inline field_value_emitter<FieldValue>::~field_value_emitter()
        OPENVRML_NOTHROW
    {}

    template <typename FieldValue>
    void field_value_emitter<FieldValue>::emit_event(const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        boost::recursive_mutex::scoped_lock lock(this->mutex());
        for (typename listener_set::iterator listener =
                 this->listeners().begin();
             listener != this->listeners().end();
             ++listener) {
            using boost::polymorphic_downcast;
            assert(*listener);
            dynamic_cast<field_value_listener<FieldValue> &>(**listener)
                .process_event(
                    *polymorphic_downcast<const FieldValue *>(&this->value()),
                    timestamp);
        }
    }

    template <typename FieldValue>
    inline bool
    field_value_emitter<FieldValue>::
    add(field_value_listener<FieldValue> & listener)
        OPENVRML_THROW1(std::bad_alloc)
    {
        boost::recursive_mutex::scoped_lock lock(this->mutex());
        return this->listeners().insert(&listener).second;
    }

    template <typename FieldValue>
    inline bool
    field_value_emitter<FieldValue>::
    remove(field_value_listener<FieldValue> & listener) OPENVRML_NOTHROW
    {
        boost::recursive_mutex::scoped_lock lock(this->mutex());
        return (this->listeners().erase(&listener) > 0);
    }

    typedef field_value_emitter<sfbool> sfbool_emitter;
    typedef field_value_emitter<sfcolor> sfcolor_emitter;
    typedef field_value_emitter<sfcolorrgba> sfcolorrgba_emitter;
    typedef field_value_emitter<sffloat> sffloat_emitter;
    typedef field_value_emitter<sfdouble> sfdouble_emitter;
    typedef field_value_emitter<sfimage> sfimage_emitter;
    typedef field_value_emitter<sfint32> sfint32_emitter;
    typedef field_value_emitter<sfnode> sfnode_emitter;
    typedef field_value_emitter<sfrotation> sfrotation_emitter;
    typedef field_value_emitter<sfstring> sfstring_emitter;
    typedef field_value_emitter<sftime> sftime_emitter;
    typedef field_value_emitter<sfvec2f> sfvec2f_emitter;
    typedef field_value_emitter<sfvec2d> sfvec2d_emitter;
    typedef field_value_emitter<sfvec3f> sfvec3f_emitter;
    typedef field_value_emitter<sfvec3d> sfvec3d_emitter;
    typedef field_value_emitter<mfbool> mfbool_emitter;
    typedef field_value_emitter<mfcolor> mfcolor_emitter;
    typedef field_value_emitter<mfcolorrgba> mfcolorrgba_emitter;
    typedef field_value_emitter<mffloat> mffloat_emitter;
    typedef field_value_emitter<mfdouble> mfdouble_emitter;
    typedef field_value_emitter<mfint32> mfint32_emitter;
    typedef field_value_emitter<mfimage> mfimage_emitter;
    typedef field_value_emitter<mfnode> mfnode_emitter;
    typedef field_value_emitter<mfrotation> mfrotation_emitter;
    typedef field_value_emitter<mfstring> mfstring_emitter;
    typedef field_value_emitter<mftime> mftime_emitter;
    typedef field_value_emitter<mfvec2f> mfvec2f_emitter;
    typedef field_value_emitter<mfvec2d> mfvec2d_emitter;
    typedef field_value_emitter<mfvec3f> mfvec3f_emitter;
    typedef field_value_emitter<mfvec3d> mfvec3d_emitter;
}

# endif
