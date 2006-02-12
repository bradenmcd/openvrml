// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2004  Braden McDaniel
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

# ifndef OPENVRML_EXPOSEDFIELD_H
#   define OPENVRML_EXPOSEDFIELD_H

#   include <openvrml/node.h>
#   include <openvrml/event.h>

namespace openvrml {

    template <typename FieldValue>
    class OPENVRML_API exposedfield :
        public FieldValue,
        public virtual node_field_value_listener<FieldValue>,
        public field_value_emitter<FieldValue> {
    public:
        virtual ~exposedfield() OPENVRML_NOTHROW;

    protected:
        exposedfield(openvrml::node & node,
                     const typename FieldValue::value_type & value =
                     typename FieldValue::value_type());
        exposedfield(const exposedfield<FieldValue> & obj);

    private:
        virtual void do_process_event(const FieldValue & value,
                                      double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
        virtual void event_side_effect(const FieldValue & value,
                                       double timestamp)
            OPENVRML_THROW1(std::bad_alloc);
    };

    template <typename FieldValue>
    inline exposedfield<FieldValue>::exposedfield(
        openvrml::node & node,
        const typename FieldValue::value_type & value):
        node_event_listener(node),
        node_field_value_listener<FieldValue>(node),
        event_emitter(static_cast<const field_value &>(*this)),
        FieldValue(value),
        field_value_emitter<FieldValue>(static_cast<FieldValue &>(*this))
    {}

    template <typename FieldValue>
    inline exposedfield<FieldValue>::exposedfield(
        const exposedfield<FieldValue> & obj):
        event_listener(),
        node_event_listener(obj.node()),
        node_field_value_listener<FieldValue>(obj.node()),
        event_emitter(static_cast<const field_value &>(*this)),
        FieldValue(obj),
        field_value_emitter<FieldValue>(static_cast<FieldValue &>(*this))
    {}

    template <typename FieldValue>
    inline exposedfield<FieldValue>::~exposedfield() OPENVRML_NOTHROW
    {}

    template <typename FieldValue>
    inline void
    exposedfield<FieldValue>::do_process_event(const FieldValue & value,
                                               const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        static_cast<FieldValue &>(*this) = value;
        this->event_side_effect(value, timestamp);
        this->node().modified(true);
        node::emit_event(*this, timestamp);
    }

    template <typename FieldValue>
    inline void
    exposedfield<FieldValue>::event_side_effect(const FieldValue &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {}
}

# endif
