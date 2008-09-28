// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_PROTO_H
#   define OPENVRML_LOCAL_PROTO_H

#   include <openvrml/node.h>
#   include <openvrml/event.h>

namespace openvrml {

    namespace local {

        class proto_node;

        /**
         * @internal
         *
         * @brief @c node_metatype for @c PROTO%s.
         *
         * The @c proto_node_metatype is OpenVRML's in-memory representation
         * of a @c PROTO (as opposed to a @c PROTO instance).  Through the
         * @c proto_node_type intermediary, it facilitates spawning any number
         * of @c proto_node instances.
         */
        class OPENVRML_LOCAL proto_node_metatype : public node_metatype {
            friend class proto_node;

            class proto_node_type;
            class proto_impl_cloner;

        public:
            class is_target {
            public:
                node * impl_node;
                std::string impl_node_interface;

                is_target(node & impl_node,
                          const std::string & impl_node_interface);
            };

            typedef std::multimap<std::string, is_target> is_map_t;

            class route {
            public:
                node * from;
                std::string eventout;
                node * to;
                std::string eventin;

                route(node & from, const std::string & eventout,
                      node & to, const std::string & eventin);
            };

            typedef std::vector<route> routes_t;

            typedef std::map<std::string, boost::shared_ptr<field_value> >
                default_value_map_t;

        private:
            openvrml::node_interface_set interfaces;
            default_value_map_t default_value_map;
            std::vector<boost::intrusive_ptr<openvrml::node> > impl_nodes;
            routes_t routes;
            is_map_t is_map;

        public:
            proto_node_metatype(
                const node_metatype_id & id,
                openvrml::browser & browser,
                const node_interface_set & interfaces,
                const default_value_map_t & default_value_map,
                const std::vector<boost::intrusive_ptr<node> > & impl_nodes,
                const is_map_t & is_map,
                const routes_t & routes);
            virtual ~proto_node_metatype() OPENVRML_NOTHROW;

            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
        };


        class OPENVRML_LOCAL abstract_proto_node : public openvrml::node {
        public:
            virtual ~abstract_proto_node() OPENVRML_NOTHROW = 0;

        protected:
            template <typename FieldValue>
            class proto_eventin :
                public openvrml::node_field_value_listener<FieldValue> {

                typedef std::set<openvrml::field_value_listener<FieldValue> *>
                    listeners;
                listeners listeners_;

            public:
                typedef FieldValue field_value_type;
                typedef openvrml::field_value_listener<FieldValue>
                    event_listener_type;

                explicit proto_eventin(abstract_proto_node & node);
                virtual ~proto_eventin() OPENVRML_NOTHROW;

                bool is(event_listener_type & listener)
                    OPENVRML_THROW1(std::bad_alloc);

            protected:
                virtual void do_process_event(const FieldValue & value,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);

            private:
                virtual const std::string do_eventin_id() const
                    OPENVRML_NOTHROW;
            };

            struct proto_eventin_creator {
                proto_eventin_creator(
                    const openvrml::field_value::type_id type,
                    abstract_proto_node & node,
                    boost::shared_ptr<openvrml::event_listener> & listener):
                    type_(type),
                    node_(&node),
                    listener_(&listener)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->type_) {
                        this->listener_->reset(
                            new proto_eventin<T>(*this->node_));
                    }
                }

            private:
                openvrml::field_value::type_id type_;
                abstract_proto_node * node_;
                boost::shared_ptr<openvrml::event_listener> * listener_;
            };

            static boost::shared_ptr<openvrml::event_listener>
            create_eventin(openvrml::field_value::type_id,
                           abstract_proto_node & node)
                OPENVRML_THROW1(std::bad_alloc);

            struct eventin_is {
                eventin_is(const openvrml::field_value::type_id field_type,
                           openvrml::event_listener & impl_eventin,
                           openvrml::event_listener & interface_eventin):
                    type_(field_type),
                    impl_eventin_(&impl_eventin),
                    interface_eventin_(&interface_eventin)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->type_) {
                        using openvrml::field_value_listener;
                        dynamic_cast<proto_eventin<T> &>(*interface_eventin_)
                            .is(dynamic_cast<field_value_listener<T> &>(
                                    *impl_eventin_));
                    }
                }

            private:
                openvrml::field_value::type_id type_;
                openvrml::event_listener * impl_eventin_;
                openvrml::event_listener * interface_eventin_;
            };

            template <typename FieldValue>
            class proto_eventout :
                public openvrml::field_value_emitter<FieldValue> {
            protected:
                class listener_t :
                    public openvrml::node_field_value_listener<FieldValue> {

                    proto_eventout & emitter;

                public:
                    abstract_proto_node & node;
                    FieldValue value;

                    listener_t(proto_eventout & emitter,
                               abstract_proto_node & node,
                               const FieldValue & initial_value);
                    virtual ~listener_t() OPENVRML_NOTHROW;

                private:
                    virtual const std::string do_eventin_id() const
                        OPENVRML_NOTHROW;
                    virtual void do_process_event(const FieldValue & value,
                                                  double timestamp)
                        OPENVRML_THROW1(std::bad_alloc);
                } listener;

            public:
                typedef FieldValue field_value_type;
                typedef openvrml::field_value_emitter<FieldValue>
                    event_emitter_type;
                typedef openvrml::field_value_listener<FieldValue>
                    event_listener_type;

                proto_eventout(
                    abstract_proto_node & node,
                    const FieldValue & initial_value = FieldValue());
                virtual ~proto_eventout() OPENVRML_NOTHROW;

                bool is(event_emitter_type & emitter)
                    OPENVRML_THROW1(std::bad_alloc);

            private:
                const std::string do_eventout_id() const OPENVRML_NOTHROW;
            };

            struct proto_eventout_creator {
                proto_eventout_creator(
                    const openvrml::field_value::type_id type,
                    abstract_proto_node & node,
                    boost::shared_ptr<openvrml::event_emitter> & emitter):
                    type_(type),
                    node_(&node),
                    emitter_(&emitter)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->type_) {
                        this->emitter_->reset(
                            new proto_eventout<T>(*this->node_));
                    }
                }

            private:
                openvrml::field_value::type_id type_;
                abstract_proto_node * node_;
                boost::shared_ptr<openvrml::event_emitter> * emitter_;
            };

            static boost::shared_ptr<openvrml::event_emitter>
            create_eventout(openvrml::field_value::type_id,
                            abstract_proto_node & node)
                OPENVRML_THROW1(std::bad_alloc);

            struct eventout_is {
                eventout_is(const openvrml::field_value::type_id field_type,
                            openvrml::event_emitter & impl_eventout,
                            openvrml::event_emitter & interface_eventout):
                    type_(field_type),
                    impl_eventout_(&impl_eventout),
                    interface_eventout_(&interface_eventout)
                {}

                template <typename T>
                void operator()(T) const
                {
                    if (T::field_value_type_id == this->type_) {
                        using openvrml::field_value_emitter;
                        dynamic_cast<proto_eventout<T> &>(*interface_eventout_)
                            .is(dynamic_cast<field_value_emitter<T> &>(
                                    *impl_eventout_));
                    }
                }

            private:
                openvrml::field_value::type_id type_;
                openvrml::event_emitter * impl_eventout_;
                openvrml::event_emitter * interface_eventout_;
            };

            typedef boost::shared_ptr<openvrml::event_listener> eventin_ptr;
            typedef std::map<std::string, eventin_ptr> eventin_map_t;

            struct proto_eventin_equal_to :
                std::unary_function<eventin_map_t::value_type, bool> {

                explicit proto_eventin_equal_to(
                    const openvrml::event_listener & listener):
                    listener_(&listener)
                {}

                bool operator()(const eventin_map_t::value_type & arg) const
                {
                    return this->listener_ == arg.second.get();
                }

            private:
                const openvrml::event_listener * listener_;
            };

            eventin_map_t eventin_map;

            typedef boost::shared_ptr<openvrml::event_emitter> eventout_ptr;
            typedef std::map<std::string, eventout_ptr> eventout_map_t;

            struct proto_eventout_equal_to :
                std::unary_function<eventout_map_t::value_type, bool> {

                explicit proto_eventout_equal_to(
                    const openvrml::event_emitter & emitter):
                    emitter_(&emitter)
                {}

                bool operator()(const eventout_map_t::value_type & arg) const
                {
                    return this->emitter_ == arg.second.get();
                }

            private:
                const openvrml::event_emitter * emitter_;
            };

            eventout_map_t eventout_map;

            abstract_proto_node(
                const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope)
                OPENVRML_NOTHROW;
        };
    }
}


/**
 * @internal
 *
 * @class openvrml::local::abstract_proto_node::proto_eventin
 *
 * @brief @c PROTO @c eventIn handler class template.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventin::listeners
 *
 * @brief Set of event listeners.
 */

/**
 * @var openvrml::local::abstract_proto_node::proto_eventin::listeners openvrml::local::abstract_proto_node::proto_eventin::listeners_
 *
 * @brief Set of event listeners to which events are delegated for
 *        processing.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventin::field_value_type
 *
 * @brief Field value type.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventin::event_listener_type
 *
 * @brief Type of event listeners to which the instance delegates.
 */

/**
 * @brief Construct.
 *
 * @param[in] node  abstract_proto_node.
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventin<FieldValue>::
proto_eventin(abstract_proto_node & node):
    node_event_listener(node),
    node_field_value_listener<FieldValue>(node)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventin<FieldValue>::
~proto_eventin()
    OPENVRML_NOTHROW
{}

/**
 * @brief Process event.
 *
 * @param[in] value     field value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
void
openvrml::local::abstract_proto_node::proto_eventin<FieldValue>::
do_process_event(const FieldValue & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    for (typename listeners::const_iterator listener =
             this->listeners_.begin();
         listener != this->listeners_.end();
         ++listener) {
        (*listener)->process_event(value, timestamp);
    }
}

/**
 * @brief The name of the associated @c eventIn.
 *
 * @return the name of the associated @c eventIn.
 */
template <typename FieldValue>
const std::string
openvrml::local::abstract_proto_node::proto_eventin<FieldValue>::
do_eventin_id() const
    OPENVRML_NOTHROW
{
    using boost::polymorphic_downcast;
    abstract_proto_node & n =
        *polymorphic_downcast<abstract_proto_node *>(&this->node());
    eventin_map_t::const_iterator pos =
        std::find_if(n.eventin_map.begin(), n.eventin_map.end(),
                     proto_eventin_equal_to(*this));
    assert(pos != n.eventin_map.end());
    return pos->first;
}

/**
 * @brief Add a listener to delegate to.  Corresponds to an @c IS statement.
 *
 * @param[in] listener  an @c event_listener to delegate to.
 *
 * @return @c true if @p listener is added successfully; @c false
 *         otherwise (if it already exists in the list of delegates).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
bool
openvrml::local::abstract_proto_node::proto_eventin<FieldValue>::
is(event_listener_type & listener)
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->listeners_.insert(&listener).second;
}

/**
 * @internal
 *
 * @class openvrml::local::abstract_proto_node::proto_eventout
 *
 * @brief @c PROTO @c eventOut handler class template.
 */

/**
 * @internal
 *
 * @class openvrml::local::abstract_proto_node::proto_eventout::listener_t
 *
 * @brief Listens for events emitted from @c node%s in the @c PROTO
 *        implementation in order to propagate them out of the @c PROTO
 *        instance.
 */

/**
 * @var openvrml::local::abstract_proto_node::proto_eventout & abstract_proto_node::proto_eventout::listener_t::emitter
 *
 * @brief Reference to the outer @c proto_eventout class.
 *
 * @todo It's annoying that we need to carry this around.  Should investigate
 *       the possibility of promoting all this stuff to @c proto_eventout and
 *       have @c proto_eventout privately inherit
 *       @c field_value_listener<FieldValue>.
 */

/**
 * @var openvrml::local::abstract_proto_node & openvrml::local::abstract_proto_node::proto_eventout::listener_t::node
 *
 * @brief Reference to the @c abstract_proto_node instance.
 */

/**
 * @var FieldValue openvrml::local::abstract_proto_node::proto_eventout::listener_t::value
 *
 * @brief The value of the most recently emitted event.
 */

/**
 * @brief Construct.
 *
 * @param[in] emitter       @c proto_eventout.
 * @param[in] node          @c abstract_proto_node.
 * @param[in] initial_value initial value (used for @c exposedFields).
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::listener_t::
listener_t(proto_eventout & emitter,
           abstract_proto_node & node,
           const FieldValue & initial_value):
    node_event_listener(node),
    node_field_value_listener<FieldValue>(node),
    emitter(emitter),
    node(node),
    value(initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::listener_t::
~listener_t()
    OPENVRML_NOTHROW
{}

/**
 * @brief Get the associated @c eventIn identifier.
 */
template <typename FieldValue>
const std::string
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::listener_t::
do_eventin_id() const
    OPENVRML_NOTHROW
{
    return "<proto_node::proto_eventout::listener_t>";
}

/**
 * @brief Process event.
 *
 * @param[in] value     new value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template<typename FieldValue>
void
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::listener_t::
do_process_event(const FieldValue & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    if (timestamp > this->emitter.last_time()) {
        this->value = value;
        node::emit_event(this->emitter, timestamp);
    }
}

/**
 * @var openvrml::local::abstract_proto_node::proto_eventout::listener_t abstract_proto_node::proto_eventout::listener
 *
 * @brief Listens for events emitted from nodes in the @c PROTO
 *        implementation in order to propagate them out of the @c PROTO
 *        instance.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::field_value_type
 *
 * @brief Field value type.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::event_emitter_type
 *
 * @brief Event emitter type.
 */

/**
 * @typedef openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::event_listener_type
 *
 * @brief Event listener type.
 */

/**
 * @brief Construct.
 *
 * @param[in] node          @c abstract_proto_node.
 * @param[in] initial_value initial value.  This is used by
 *                          @c proto_exposedfield<FieldValue>
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::
proto_eventout(abstract_proto_node & node,
               const FieldValue & initial_value):
    openvrml::event_emitter(this->listener.value),
    field_value_emitter<FieldValue>(this->listener.value),
    listener(*this, node, initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::
~proto_eventout()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create an @c IS mapping.
 *
 * @param[in,out] emitter   the @c event_emitter from a node in the @c PROTO
 *                          implementation.
 *
 * @return @c true if the @c IS mapping is created successfully; @c false
 *         otherwise (i.e., if it already exists).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
bool
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::
is(event_emitter_type & emitter) OPENVRML_THROW1(std::bad_alloc)
{
    return emitter.add(this->listener);
}

/**
 * @brief The name of the associated @c eventOut.
 *
 * @return the name of the associated @c eventOut.
 */
template <typename FieldValue>
const std::string
openvrml::local::abstract_proto_node::proto_eventout<FieldValue>::
do_eventout_id() const
    OPENVRML_NOTHROW
{
    abstract_proto_node & n = this->listener.node;
    eventout_map_t::const_iterator pos =
        std::find_if(n.eventout_map.begin(), n.eventout_map.end(),
                     proto_eventout_equal_to(*this));
    assert(pos != n.eventout_map.end());
    return pos->first;
}

# endif // ifndef OPENVRML_LOCAL_PROTO_H
