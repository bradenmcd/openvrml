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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <algorithm>
# include <functional>
# include <sstream>
# include <stack>
# include <regex.h>
# ifdef _WIN32
#   include <sys/timeb.h>
#   include <time.h>
# else
#   include <sys/time.h>
# endif
# include <boost/cast.hpp>
# include <boost/shared_ptr.hpp>
# include <boost/utility.hpp>
# include "private.h"
# include "browser.h"
# include "doc.h"
# include "viewer.h"
# include "scope.h"
# include "script.h"
# include "vrml97node.h"

namespace openvrml {

    namespace {
        class proto_impl_cloner;
    }

    class Vrml97RootScope : public scope {
    public:
        Vrml97RootScope(const browser & browser,
                        const std::string & uri = std::string())
            throw (std::bad_alloc);
        virtual ~Vrml97RootScope() throw ();
    };

    class proto_node_class : public node_class {
        friend class proto_node;
        friend class proto_impl_cloner;

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
        node_interface_set interfaces;
        default_value_map_t default_value_map;
        std::vector<node_ptr> impl_nodes;
        routes_t routes;
        is_map_t is_map;

        class proto_node_type : public node_type {
            node_interface_set interfaces_;

        public:
            proto_node_type(proto_node_class & node_class,
                            const std::string & id,
                            const node_interface_set & interfaces)
                throw (unsupported_interface, std::bad_alloc);
            virtual ~proto_node_type() throw ();

            virtual const node_interface_set & interfaces() const throw ();
            virtual const node_ptr
            create_node(const scope_ptr & scope,
                        const initial_value_map & initial_values =
                        initial_value_map()) const
                throw (std::bad_alloc);
        };


    public:
        proto_node_class(
            openvrml::browser & browser,
            const node_interface_set & interfaces,
            const default_value_map_t & default_value_map,
            const std::vector<node_ptr> & impl_nodes,
            const is_map_t & is_map,
            const routes_t & routes);
        virtual ~proto_node_class() throw ();

        virtual const node_type_ptr
        create_type(const std::string & id,
                    const node_interface_set & interfaces)
            throw (unsupported_interface, std::bad_alloc);
    };

    class proto_node : public node {
        template <typename FieldValue>
        class proto_eventin : public field_value_listener<FieldValue> {
            typedef std::set<field_value_listener<FieldValue> *>
                listeners;
            listeners listeners_;

        public:
            typedef FieldValue field_value_type;
            typedef field_value_listener<FieldValue> event_listener_type;

            explicit proto_eventin(proto_node & node);
            virtual ~proto_eventin() throw ();
            virtual void process_event(const FieldValue & value,
                                       double timestamp)
                throw (std::bad_alloc);

            bool is(event_listener_type & listener) throw (std::bad_alloc);
        };

        static boost::shared_ptr<openvrml::event_listener>
        create_eventin(field_value::type_id, proto_node & node)
            throw (std::bad_alloc);

        static bool
        eventin_is(field_value::type_id field_type,
                    openvrml::event_listener & impl_eventin,
                    openvrml::event_listener & interface_eventin)
            throw (std::bad_alloc);

        template <typename FieldValue>
        class proto_eventout : public field_value_emitter<FieldValue> {
        protected:
            class listener_t : public field_value_listener<FieldValue> {
                proto_eventout & emitter;
                proto_node & node;

            public:
                FieldValue value;

                explicit listener_t(proto_eventout & emitter,
                                    proto_node & node,
                                    const FieldValue & initial_value);
                virtual ~listener_t() throw ();

                virtual void process_event(const FieldValue & value,
                                           double timestamp)
                    throw (std::bad_alloc);
            } listener;

        public:
            typedef FieldValue field_value_type;
            typedef field_value_emitter<FieldValue> event_emitter_type;
            typedef field_value_listener<FieldValue> event_listener_type;

            proto_eventout(
                proto_node & node,
                const FieldValue & initial_value = FieldValue());
            virtual ~proto_eventout() throw ();

            bool is(event_emitter_type & emitter) throw (std::bad_alloc);

        private:
            double last_time() const;
        };

        static boost::shared_ptr<openvrml::event_emitter>
        create_eventout(field_value::type_id, proto_node & node)
            throw (std::bad_alloc);

        static bool
        eventout_is(field_value::type_id field_type,
                    openvrml::event_emitter & impl_eventout,
                    openvrml::event_emitter & interface_eventout)
            throw (std::bad_alloc);

        template <typename FieldValue>
        class proto_exposedfield : public proto_eventin<FieldValue>,
                                   public proto_eventout<FieldValue> {
        public:
            proto_exposedfield(proto_node & node,
                                const FieldValue & initial_value);
            virtual ~proto_exposedfield() throw ();

            virtual void process_event(const FieldValue & value,
                                        double timestamp)
                throw (std::bad_alloc);
        };

        static boost::shared_ptr<openvrml::event_listener>
        create_exposedfield(const field_value & initial_value,
                            proto_node & node)
            throw (std::bad_alloc);

        scope_ptr proto_scope;
        std::vector<node_ptr> impl_nodes;

        typedef boost::shared_ptr<openvrml::event_listener> eventin_ptr;
        typedef std::map<std::string, eventin_ptr> eventin_map_t;
        eventin_map_t eventin_map;

        typedef boost::shared_ptr<openvrml::event_emitter> eventout_ptr;
        typedef std::map<std::string, eventout_ptr> eventout_map_t;
        eventout_map_t eventout_map;

    public:
        proto_node(const node_type & type,
                   const scope_ptr & scope,
                   const initial_value_map & initial_values)
            throw (std::bad_alloc);
        virtual ~proto_node() throw ();

    private:
        virtual void do_initialize(double timestamp)
            throw (std::bad_alloc);

        virtual const field_value & do_field(const std::string & id) const
            throw (unsupported_interface);
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            throw (unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            throw (unsupported_interface);

        virtual void do_shutdown(double timestamp) throw ();

        virtual script_node * to_script() throw ();
        virtual appearance_node * to_appearance() throw ();
        virtual child_node * to_child() throw ();
        virtual color_node * to_color() throw ();
        virtual coordinate_node * to_coordinate() throw ();
        virtual font_style_node * to_font_style() throw () ;
        virtual geometry_node * to_geometry() throw ();
        virtual grouping_node * to_grouping() throw ();
        virtual material_node * to_material() throw ();
        virtual normal_node * to_normal() throw ();
        virtual sound_source_node * to_sound_source() throw ();
        virtual texture_node * to_texture() throw ();
        virtual texture_coordinate_node * to_texture_coordinate() throw ();
        virtual texture_transform_node * to_texture_transform() throw ();
        virtual transform_node * to_transform() throw ();
        virtual viewpoint_node * to_viewpoint() throw ();
    };

} // namespace openvrml




namespace openvrml {

/**
 * @brief Construct.
 *
 * @param impl_node             a node in the PROTO implementation.
 * @param impl_node_interface   an interface of @p impl_node.
 */
proto_node_class::
is_target::is_target(node & impl_node,
                     const std::string & impl_node_interface):
    impl_node(&impl_node),
    impl_node_interface(impl_node_interface)
{}

/**
 * @brief Construct.
 *
 * @param from      event source node.
 * @param eventout  eventOut of @p from.
 * @param to        event destination node.
 * @param eventin   eventIn of @p to.
 */
proto_node_class::route::route(node & from,
                               const std::string & eventout,
                               node & to,
                               const std::string & eventin):
    from(&from),
    eventout(eventout),
    to(&to),
    eventin(eventin)
{}

/**
 * @brief Construct.
 *
 * @param node_class    the proto_node_class that spawned the proto_node_type.
 * @param id            node_type identifier.
 * @param interfaces    a subset of the interfaces supported by the
 *                      @p node_class.
 *
 * @exception unsupported_interface if an interface in @p interfaces is not
 *                                  supported by the @p node_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
proto_node_class::
proto_node_type::proto_node_type(proto_node_class & node_class,
                                 const std::string & id,
                                 const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc):
    node_type(node_class, id)
{
    using std::find;
    using std::invalid_argument;
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        node_interface_set::const_iterator pos =
            find(node_class.interfaces.begin(), node_class.interfaces.end(),
                 *interface);
        if (pos == node_class.interfaces.end()) {
            throw unsupported_interface(*interface);
        }
        const bool succeeded = this->interfaces_.insert(*interface).second;
        assert(succeeded);
    }
}

/**
 * @brief Destroy.
 */
proto_node_class::proto_node_type::~proto_node_type() throw ()
{}

/**
 * @brief Interfaces.
 *
 * @return the interfaces.
 */
const node_interface_set &
proto_node_class::proto_node_type::interfaces() const throw ()
{
    return this->interfaces_;
}

const node_ptr
proto_node_class::proto_node_type::
create_node(const scope_ptr & scope,
            const initial_value_map & initial_values) const
    throw (std::bad_alloc)
{
    return node_ptr(new proto_node(*this, scope, initial_values));
}

/**
 * @class proto_node::proto_eventin
 *
 * @brief PROTO eventIn handler class template.
 */

/**
 * @typedef proto_node::proto_eventin::listeners
 *
 * @brief Set of event listeners.
 */

/**
 * @var proto_node::proto_eventin::listeners proto_node::proto_eventin::listeners_
 *
 * @brief Set of event listeners to which events are delegated for processing.
 */

/**
 * @typedef proto_node::proto_eventin::field_value_type
 *
 * @brief Field value type.
 */

/**
 * @typedef proto_node::proto_eventin::event_listener_type
 *
 * @brief Type of event listeners to which the instance delegates.
 */

/**
 * @brief Construct.
 *
 * @param node  proto_node.
 */
template <typename FieldValue>
proto_node::proto_eventin<FieldValue>::proto_eventin(proto_node & node):
    field_value_listener<FieldValue>(node)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
proto_node::proto_eventin<FieldValue>::~proto_eventin()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     field value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
void
proto_node::proto_eventin<FieldValue>::
process_event(const FieldValue & value, const double timestamp)
    throw (std::bad_alloc)
{
    for (typename listeners::const_iterator listener =
             this->listeners_.begin();
         listener != this->listeners_.end();
         ++listener) {
        (*listener)->process_event(value, timestamp);
    }
}

/**
 * @brief Add a listener to delegate to. Corresponds to an IS statement.
 *
 * @param listener  an event_listener to delegate to.
 *
 * @return @c true if @p listener is added successfully; @c false otherwise (if
 *         it already exists in the list of delegates).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
bool
proto_node::proto_eventin<FieldValue>::is(event_listener_type & listener)
    throw (std::bad_alloc)
{
    return this->listeners_.insert(&listener).second;
}

/**
 * @brief Factory function for proto_eventin<FieldValue> instances.
 *
 * @param type  field_value::type_id.
 * @param node  proto_node.
 *
 * @return a boost::shared_ptr to a proto_eventout<FieldValue> instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
boost::shared_ptr<event_listener>
proto_node::create_eventin(const field_value::type_id type,
                           proto_node & node)
    throw (std::bad_alloc)
{
    boost::shared_ptr<openvrml::event_listener> result;
    switch (type) {
    case field_value::sfbool_id:
        result.reset(new proto_eventin<sfbool>(node));
        break;
    case field_value::sfcolor_id:
        result.reset(new proto_eventin<sfcolor>(node));
        break;
    case field_value::sffloat_id:
        result.reset(new proto_eventin<sffloat>(node));
        break;
    case field_value::sfimage_id:
        result.reset(new proto_eventin<sfimage>(node));
        break;
    case field_value::sfint32_id:
        result.reset(new proto_eventin<sfint32>(node));
        break;
    case field_value::sfnode_id:
        result.reset(new proto_eventin<sfnode>(node));
        break;
    case field_value::sfrotation_id:
        result.reset(new proto_eventin<sfrotation>(node));
        break;
    case field_value::sfstring_id:
        result.reset(new proto_eventin<sfstring>(node));
        break;
    case field_value::sftime_id:
        result.reset(new proto_eventin<sftime>(node));
        break;
    case field_value::sfvec2f_id:
        result.reset(new proto_eventin<sfvec2f>(node));
        break;
    case field_value::sfvec3f_id:
        result.reset(new proto_eventin<sfvec3f>(node));
        break;
    case field_value::mfcolor_id:
        result.reset(new proto_eventin<mfcolor>(node));
        break;
    case field_value::mffloat_id:
        result.reset(new proto_eventin<mffloat>(node));
        break;
    case field_value::mfint32_id:
        result.reset(new proto_eventin<mfint32>(node));
        break;
    case field_value::mfnode_id:
        result.reset(new proto_eventin<mfnode>(node));
        break;
    case field_value::mfrotation_id:
        result.reset(new proto_eventin<mfrotation>(node));
        break;
    case field_value::mfstring_id:
        result.reset(new proto_eventin<mfstring>(node));
        break;
    case field_value::mftime_id:
        result.reset(new proto_eventin<mftime>(node));
        break;
    case field_value::mfvec2f_id:
        result.reset(new proto_eventin<mfvec2f>(node));
        break;
    case field_value::mfvec3f_id:
        result.reset(new proto_eventin<mfvec3f>(node));
        break;
    default:
        assert(false);
        break;
    }
    assert(result.get());
    return result;
}

/**
 * @brief Create an IS mapping between an event_listener in the PROTO
 *        implementation and an event_listener in the PROTO interface.
 *
 * @param field_type        field value type of the concrete listeners.
 * @param impl_eventin      event_listener of a node in the PROTO
 *                          implementation.
 * @param interface_eventin event_listener from the PROTO interface.
 *
 * @return @c true if the IS mapping is established successfully; @c false
 *         otherwise (i.e., if the mapping already exists).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool
proto_node::eventin_is(const field_value::type_id field_type,
                       openvrml::event_listener & impl_eventin,
                       openvrml::event_listener & interface_eventin)
    throw (std::bad_alloc)
{
    using boost::polymorphic_downcast;
    bool succeeded;
    switch (field_type) {
    case field_value::sfbool_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfbool> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfbool_listener *>(&impl_eventin));
        break;
    case field_value::sfcolor_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfcolor> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfcolor_listener *>(&impl_eventin));
        break;
    case field_value::sffloat_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sffloat> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sffloat_listener *>(&impl_eventin));
        break;
    case field_value::sfimage_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfimage> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfimage_listener *>(&impl_eventin));
        break;
    case field_value::sfint32_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfint32> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfint32_listener *>(&impl_eventin));
        break;
    case field_value::sfnode_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfnode> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfnode_listener *>(&impl_eventin));
        break;
    case field_value::sfrotation_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfrotation> *>(
                &interface_eventin)
            ->is(*polymorphic_downcast<sfrotation_listener *>(&impl_eventin));
        break;
    case field_value::sfstring_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfstring> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfstring_listener *>(&impl_eventin));
        break;
    case field_value::sftime_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sftime> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sftime_listener *>(&impl_eventin));
        break;
    case field_value::sfvec2f_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfvec2f> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfvec2f_listener *>(&impl_eventin));
        break;
    case field_value::sfvec3f_id:
        succeeded =
            polymorphic_downcast<proto_eventin<sfvec3f> *>(&interface_eventin)
            ->is(*polymorphic_downcast<sfvec3f_listener *>(&impl_eventin));
        break;
    case field_value::mfcolor_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfcolor> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfcolor_listener *>(&impl_eventin));
        break;
    case field_value::mffloat_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mffloat> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mffloat_listener *>(&impl_eventin));
        break;
    case field_value::mfint32_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfint32> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfint32_listener *>(&impl_eventin));
        break;
    case field_value::mfnode_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfnode> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfnode_listener *>(&impl_eventin));
        break;
    case field_value::mfrotation_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfrotation> *>(
                &interface_eventin)
            ->is(*polymorphic_downcast<mfrotation_listener *>(&impl_eventin));
        break;
    case field_value::mfstring_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfstring> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfstring_listener *>(&impl_eventin));
        break;
    case field_value::mftime_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mftime> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mftime_listener *>(&impl_eventin));
        break;
    case field_value::mfvec2f_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfvec2f> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfvec2f_listener *>(&impl_eventin));
        break;
    case field_value::mfvec3f_id:
        succeeded =
            polymorphic_downcast<proto_eventin<mfvec3f> *>(&interface_eventin)
            ->is(*polymorphic_downcast<mfvec3f_listener *>(&impl_eventin));
        break;
    default:
        assert(false);
        break;
    }
    return succeeded;
}

/**
 * @class proto_node::proto_eventout
 *
 * @brief PROTO eventOut handler class template.
 */

/**
 * @class proto_node::proto_eventout::listener_t
 *
 * @brief Listens for events emitted from nodes in the PROTO implementation
 *        in order to propagate them out of the PROTO instance.
 */

/**
 * @var proto_node::proto_eventout & proto_node::proto_eventout::listener_t::emitter
 *
 * @brief Reference to the outer proto_eventout class.
 *
 * @todo It's annoying that we need to carry this around.  Should investigate
 *       the possibility of promoting all this stuff to proto_eventout and have
 *       proto_eventout privately inherit field_value_listener<FieldValue>.
 */

/**
 * @var proto_node & proto_node::proto_eventout::listener_t::node
 *
 * @brief Reference to the proto_node instance.
 */

/**
 * @var FieldValue proto_node::proto_eventout::listener_t::value
 *
 * @brief The value of the most recently emitted event.
 */

/**
 * @brief Construct.
 *
 * @param emitter       proto_eventout.
 * @param node          proto_node.
 * @param initial_value initial value (used for exposedFields).
 */
template <typename FieldValue>
proto_node::proto_eventout<FieldValue>::listener_t::
listener_t(proto_eventout & emitter,
           proto_node & node,
           const FieldValue & initial_value):
    field_value_listener<FieldValue>(node),
    emitter(emitter),
    node(node),
    value(initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
proto_node::proto_eventout<FieldValue>::listener_t::~listener_t() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template<typename FieldValue>
void
proto_node::proto_eventout<FieldValue>::listener_t::
process_event(const FieldValue & value, const double timestamp)
    throw (std::bad_alloc)
{
    if (timestamp > this->emitter.last_time()) {
        this->value = value;
        node::emit_event(this->emitter, timestamp);
    }
}

/**
 * @var proto_node::proto_eventout::listener_t proto_node::proto_eventout::listener
 *
 * @brief Listens for events emitted from nodes in the PROTO implementation
 *        in order to propagate them out of the PROTO instance.
 */

/**
 * @typedef proto_node::proto_eventout<FieldValue>::field_value_type
 *
 * @brief Field value type.
 */

/**
 * @typedef proto_node::proto_eventout<FieldValue>::event_emitter_type
 *
 * @brief Event emitter type.
 */

/**
 * @typedef proto_node::proto_eventout<FieldValue>::event_listener_type
 *
 * @brief Event listener type.
 */

/**
 * @brief Construct.
 *
 * @param node          proto_node.
 * @param initial_value initial value.  This is used by
 *                      proto_exposedfield<FieldValue>
 */
template <typename FieldValue>
proto_node::proto_eventout<FieldValue>::
proto_eventout(proto_node & node, const FieldValue & initial_value):
    field_value_emitter<FieldValue>(this->listener.value),
    listener(*this, node, initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
proto_node::proto_eventout<FieldValue>::~proto_eventout() throw ()
{}

/**
 * @brief Create an IS mapping.
 *
 * @param emitter   the event_emitter from a node in the PROTO implementation.
 *
 * @return @c true if the IS mapping is created successfully; @c false
 *         otherwise (i.e., if it already exists).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
bool
proto_node::proto_eventout<FieldValue>::
is(event_emitter_type & emitter) throw (std::bad_alloc)
{
    return emitter.add(this->listener);
}

/**
 * @brief The timestamp of the most recently emitted event.
 *
 * @return the timestamp of the most recently emitted event.
 */
template <typename FieldValue>
double proto_node::proto_eventout<FieldValue>::last_time() const
{
    return this->event_emitter::last_time;
}

/**
 * @brief Factory function for proto_eventout<FieldValue> instances.
 *
 * @param type  field_value::type_id.
 * @param node  proto_node.
 *
 * @return a boost::shared_ptr to a proto_eventout<FieldValue> instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
boost::shared_ptr<event_emitter>
proto_node::create_eventout(const field_value::type_id type, proto_node & node)
    throw (std::bad_alloc)
{
    boost::shared_ptr<openvrml::event_emitter> result;
    switch (type) {
    case field_value::sfbool_id:
        result.reset(new proto_eventout<sfbool>(node));
        break;
    case field_value::sfcolor_id:
        result.reset(new proto_eventout<sfcolor>(node));
        break;
    case field_value::sffloat_id:
        result.reset(new proto_eventout<sffloat>(node));
        break;
    case field_value::sfimage_id:
        result.reset(new proto_eventout<sfimage>(node));
        break;
    case field_value::sfint32_id:
        result.reset(new proto_eventout<sfint32>(node));
        break;
    case field_value::sfnode_id:
        result.reset(new proto_eventout<sfnode>(node));
        break;
    case field_value::sfrotation_id:
        result.reset(new proto_eventout<sfrotation>(node));
        break;
    case field_value::sfstring_id:
        result.reset(new proto_eventout<sfstring>(node));
        break;
    case field_value::sftime_id:
        result.reset(new proto_eventout<sftime>(node));
        break;
    case field_value::sfvec2f_id:
        result.reset(new proto_eventout<sfvec2f>(node));
        break;
    case field_value::sfvec3f_id:
        result.reset(new proto_eventout<sfvec3f>(node));
        break;
    case field_value::mfcolor_id:
        result.reset(new proto_eventout<mfcolor>(node));
        break;
    case field_value::mffloat_id:
        result.reset(new proto_eventout<mffloat>(node));
        break;
    case field_value::mfint32_id:
        result.reset(new proto_eventout<mfint32>(node));
        break;
    case field_value::mfnode_id:
        result.reset(new proto_eventout<mfnode>(node));
        break;
    case field_value::mfrotation_id:
        result.reset(new proto_eventout<mfrotation>(node));
        break;
    case field_value::mfstring_id:
        result.reset(new proto_eventout<mfstring>(node));
        break;
    case field_value::mftime_id:
        result.reset(new proto_eventout<mftime>(node));
        break;
    case field_value::mfvec2f_id:
        result.reset(new proto_eventout<mfvec2f>(node));
        break;
    case field_value::mfvec3f_id:
        result.reset(new proto_eventout<mfvec3f>(node));
        break;
    default:
        assert(false);
        break;
    }
    assert(result.get());
    return result;
}

/**
 * @brief Create an IS mapping between an event_emitter in the PROTO
 *        implementation and an event_emitter in the PROTO interface.
 *
 * @param field_type            field value type of the concrete emitters.
 * @param impl_eventout         event_emitter of a node in the PROTO
 *                              implementation.
 * @param interface_eventout    event_emitter from the PROTO interface.
 *
 * @return @c true if the IS mapping is established successfully; @c false
 *         otherwise (i.e., if the mapping already exists).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
bool
proto_node::eventout_is(const field_value::type_id field_type,
                        openvrml::event_emitter & impl_eventout,
                        openvrml::event_emitter & interface_eventout)
    throw (std::bad_alloc)
{
    using boost::polymorphic_downcast;
    bool succeeded;
    switch (field_type) {
    case field_value::sfbool_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfbool> *>(&interface_eventout)
            ->is(*polymorphic_downcast<sfbool_emitter *>(&impl_eventout));
        break;
    case field_value::sfcolor_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfcolor> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfcolor_emitter *>(&impl_eventout));
        break;
    case field_value::sffloat_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sffloat> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sffloat_emitter *>(&impl_eventout));
        break;
    case field_value::sfimage_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfimage> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfimage_emitter *>(&impl_eventout));
        break;
    case field_value::sfint32_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfint32> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfint32_emitter *>(&impl_eventout));
        break;
    case field_value::sfnode_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfnode> *>(&interface_eventout)
            ->is(*polymorphic_downcast<sfnode_emitter *>(&impl_eventout));
        break;
    case field_value::sfrotation_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfrotation> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfrotation_emitter *>(&impl_eventout));
        break;
    case field_value::sfstring_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfstring> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfstring_emitter *>(&impl_eventout));
        break;
    case field_value::sftime_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sftime> *>(&interface_eventout)
            ->is(*polymorphic_downcast<sftime_emitter *>(&impl_eventout));
        break;
    case field_value::sfvec2f_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfvec2f> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfvec2f_emitter *>(&impl_eventout));
        break;
    case field_value::sfvec3f_id:
        succeeded =
            polymorphic_downcast<proto_eventout<sfvec3f> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<sfvec3f_emitter *>(&impl_eventout));
        break;
    case field_value::mfcolor_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfcolor> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfcolor_emitter *>(&impl_eventout));
        break;
    case field_value::mffloat_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mffloat> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mffloat_emitter *>(&impl_eventout));
        break;
    case field_value::mfint32_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfint32> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfint32_emitter *>(&impl_eventout));
        break;
    case field_value::mfnode_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfnode> *>(&interface_eventout)
            ->is(*polymorphic_downcast<mfnode_emitter *>(&impl_eventout));
        break;
    case field_value::mfrotation_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfrotation> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfrotation_emitter *>(&impl_eventout));
        break;
    case field_value::mfstring_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfstring> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfstring_emitter *>(&impl_eventout));
        break;
    case field_value::mftime_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mftime> *>(&interface_eventout)
            ->is(*polymorphic_downcast<mftime_emitter *>(&impl_eventout));
        break;
    case field_value::mfvec2f_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfvec2f> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfvec2f_emitter *>(&impl_eventout));
        break;
    case field_value::mfvec3f_id:
        succeeded =
            polymorphic_downcast<proto_eventout<mfvec3f> *>(
                &interface_eventout)
            ->is(*polymorphic_downcast<mfvec3f_emitter *>(&impl_eventout));
        break;
    default:
        assert(false);
        break;
    }
    return succeeded;
}

/**
 * @class proto_node::proto_exposedfield
 *
 * @brief PROTO exposedField handler class template.
 */

/**
 * @brief Construct.
 *
 * @param node          proto_node.
 * @param initial_value initial value.
 */
template <typename FieldValue>
proto_node::proto_exposedfield<FieldValue>::
proto_exposedfield(proto_node & node, const FieldValue & initial_value):
    proto_eventin<FieldValue>(node),
    proto_eventout<FieldValue>(node, initial_value)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
proto_node::proto_exposedfield<FieldValue>::~proto_exposedfield() throw ()
{}

/**
 * @brief Process an event.
 *
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
void
proto_node::proto_exposedfield<FieldValue>::
process_event(const FieldValue & value, const double timestamp)
    throw (std::bad_alloc)
{
    this->proto_eventin<FieldValue>::process_event(value, timestamp);
    this->listener.value = value;
    node::emit_event(*this, timestamp);
}

/**
 * @brief Factory function for proto_exposedfield<FieldValue> instances.
 *
 * @param type  field_value::type_id.
 * @param node  proto_node.
 *
 * @return a boost::shared_ptr to a proto_exposedfield<FieldValue> instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
boost::shared_ptr<event_listener>
proto_node::create_exposedfield(const field_value & initial_value,
                                proto_node & node)
    throw (std::bad_alloc)
{
    using boost::polymorphic_downcast;
    boost::shared_ptr<openvrml::event_listener> result;
    switch (initial_value.type()) {
    case field_value::sfbool_id:
        result.reset(
            new proto_exposedfield<sfbool>(
                node,
                *polymorphic_downcast<const sfbool *>(&initial_value)));
        break;
    case field_value::sfcolor_id:
        result.reset(
            new proto_exposedfield<sfcolor>(
                node,
                *polymorphic_downcast<const sfcolor *>(&initial_value)));
        break;
    case field_value::sffloat_id:
        result.reset(
            new proto_exposedfield<sffloat>(
                node,
                *polymorphic_downcast<const sffloat *>(&initial_value)));
        break;
    case field_value::sfimage_id:
        result.reset(
            new proto_exposedfield<sfimage>(
                node,
                *polymorphic_downcast<const sfimage *>(&initial_value)));
        break;
    case field_value::sfint32_id:
        result.reset(
            new proto_exposedfield<sfint32>(
                node,
                *polymorphic_downcast<const sfint32 *>(&initial_value)));
        break;
    case field_value::sfnode_id:
        result.reset(
            new proto_exposedfield<sfnode>(
                node,
                *polymorphic_downcast<const sfnode *>(&initial_value)));
        break;
    case field_value::sfrotation_id:
        result.reset(
            new proto_exposedfield<sfrotation>(
                node,
                *polymorphic_downcast<const sfrotation *>(&initial_value)));
        break;
    case field_value::sfstring_id:
        result.reset(
            new proto_exposedfield<sfstring>(
                node,
                *polymorphic_downcast<const sfstring *>(&initial_value)));
        break;
    case field_value::sftime_id:
        result.reset(
            new proto_exposedfield<sftime>(
                node,
                *polymorphic_downcast<const sftime *>(&initial_value)));
        break;
    case field_value::sfvec2f_id:
        result.reset(
            new proto_exposedfield<sfvec2f>(
                node,
                *polymorphic_downcast<const sfvec2f *>(&initial_value)));
        break;
    case field_value::sfvec3f_id:
        result.reset(
            new proto_exposedfield<sfvec3f>(
                node,
                *polymorphic_downcast<const sfvec3f *>(&initial_value)));
        break;
    case field_value::mfcolor_id:
        result.reset(
            new proto_exposedfield<mfcolor>(
                node,
                *polymorphic_downcast<const mfcolor *>(&initial_value)));
        break;
    case field_value::mffloat_id:
        result.reset(
            new proto_exposedfield<mffloat>(
                node,
                *polymorphic_downcast<const mffloat *>(&initial_value)));
        break;
    case field_value::mfint32_id:
        result.reset(
            new proto_exposedfield<mfint32>(
                node,
                *polymorphic_downcast<const mfint32 *>(&initial_value)));
        break;
    case field_value::mfnode_id:
        result.reset(
            new proto_exposedfield<mfnode>(
                node,
                *polymorphic_downcast<const mfnode *>(&initial_value)));
        break;
    case field_value::mfrotation_id:
        result.reset(
            new proto_exposedfield<mfrotation>(
                node,
                *polymorphic_downcast<const mfrotation *>(&initial_value)));
        break;
    case field_value::mfstring_id:
        result.reset(
            new proto_exposedfield<mfstring>(
                node,
                *polymorphic_downcast<const mfstring *>(&initial_value)));
        break;
    case field_value::mftime_id:
        result.reset(
            new proto_exposedfield<mftime>(
                node,
                *polymorphic_downcast<const mftime *>(&initial_value)));
        break;
    case field_value::mfvec2f_id:
        result.reset(
            new proto_exposedfield<mfvec2f>(
                node,
                *polymorphic_downcast<const mfvec2f *>(&initial_value)));
        break;
    case field_value::mfvec3f_id:
        result.reset(
            new proto_exposedfield<mfvec3f>(
                node,
                *polymorphic_downcast<const mfvec3f *>(&initial_value)));
        break;
    default:
        assert(false);
    }
    assert(result.get());
    return result;
}

namespace {

    class node_path_element {
    public:
        std::vector<node_ptr>::size_type index;
        std::string field_id;

        node_path_element();
    };

    node_path_element::node_path_element():
        index(0)
    {}

    typedef std::list<node_path_element> node_path_t;

    class path_getter : boost::noncopyable {
        const node & objective;
        node_path_t & node_path;
        bool found;

    public:
        path_getter(const node & objective, node_path_t & node_path) throw ();

        void get_path_from(const node_ptr & node) throw (std::bad_alloc);
        void get_path_from(const std::vector<node_ptr> & nodes)
            throw (std::bad_alloc);

    private:
        void traverse_children(node & n) throw (std::bad_alloc);
    };

    path_getter::path_getter(const node & objective, node_path_t & node_path)
        throw ():
        objective(objective),
        node_path(node_path),
        found(false)
    {}

    void path_getter::get_path_from(const node_ptr & node)
        throw (std::bad_alloc)
    {
# ifndef NDEBUG
        size_t initial_size = this->node_path.size();
# endif
        if (node) {
            this->node_path.push_back(node_path_element());
            if (node.get() == &objective) {
                this->found = true;
                return;
            }
            this->traverse_children(*node);
            if (!this->found) { this->node_path.pop_back(); }
        }
        assert(this->node_path.size() == initial_size);
    }

    void path_getter::get_path_from(const std::vector<node_ptr> & nodes)
        throw (std::bad_alloc)
    {
        this->node_path.push_back(node_path_element());
        node_path_element & back = this->node_path.back();
        while (back.index < nodes.size()) {
            assert(&back == &this->node_path.back());
            if (nodes[back.index].get() == &this->objective) {
                this->found = true;
                return;
            }
            if (nodes[back.index].get()) {
                this->traverse_children(*nodes[back.index]);
            }
            //
            // We need to bail early to avoid incrementing the counter.
            //
            if (this->found) { return; }
            ++back.index;
        }
        if (!this->found) { this->node_path.pop_back(); }
    }

    void path_getter::traverse_children(node & n) throw (std::bad_alloc)
    {
        const node_interface_set & interfaces = n.type.interfaces();
        node_path_element & back = this->node_path.back();
        for (node_interface_set::const_iterator interface = interfaces.begin();
             !this->found && interface != interfaces.end();
             ++interface) {
            assert(&back == &this->node_path.back());
            if (interface->type == node_interface::field_id
                || interface->type == node_interface::exposedfield_id) {
                if (interface->field_type == field_value::sfnode_id) {
                    back.field_id = interface->id;
                    try {
                        const sfnode & value =
                            static_cast<const sfnode &>(
                                n.field(interface->id));
                        this->get_path_from(value.value);
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                } else if (interface->field_type == field_value::mfnode_id) {
                    back.field_id = interface->id;
                    try {
                        const mfnode & value =
                            static_cast<const mfnode &>(
                                n.field(interface->id));
                        this->get_path_from(value.value);
                    } catch (unsupported_interface & ex) {
                        OPENVRML_PRINT_EXCEPTION_(ex);
                    }
                }
            }
        }
    }

    const node_path_t get_path(const std::vector<node_ptr> & root,
                               const node & objective)
        throw (std::bad_alloc)
    {
        node_path_t path;
        path_getter(objective, path).get_path_from(root);
        return path;
    }

    node * resolve_node_path(const node_path_t & path,
                             const std::vector<node_ptr> & root)
    {
        using boost::next;
        using boost::prior;
        assert(!path.empty());
        node * result = root[path.front().index].get();
        const node_path_t::const_iterator before_end = prior(path.end());
        for (node_path_t::const_iterator path_element = path.begin();
             path_element != before_end;
             ++path_element) {
            assert(result);
            try {
                const field_value & field_val =
                    result->field(path_element->field_id);
                const field_value::type_id type = field_val.type();
                if (type == field_value::sfnode_id) {
                    result =
                        static_cast<const sfnode &>(field_val).value.get();
                } else if (type == field_value::mfnode_id) {
                    result = static_cast<const mfnode &>(field_val)
                        .value[next(path_element)->index].get();
                }
            } catch (unsupported_interface & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            }
        }
        return result;
    }

    class field_value_cloner {
    protected:
        const scope_ptr & target_scope;
        std::set<node *> traversed_nodes;

    public:
        explicit field_value_cloner(const scope_ptr & target_scope):
            target_scope(target_scope)
        {
            assert(target_scope);
        }

        virtual ~field_value_cloner()
        {}

        void clone_field_value(const node_ptr & src_node,
                               const field_value & src,
                               field_value & dest)
            throw (std::bad_alloc)
        {
            assert(src.type() == dest.type());
            const field_value::type_id type = src.type();
            if (type == field_value::sfnode_id) {
                this->clone_sfnode(src_node,
                                   static_cast<const sfnode &>(src),
                                   static_cast<sfnode &>(dest));
            } else if (type == field_value::mfnode_id) {
                this->clone_mfnode(src_node,
                                   static_cast<const mfnode &>(src),
                                   static_cast<mfnode &>(dest));
            } else {
                //
                // Do a shallow copy for other types.
                //
                dest.assign(src);
            }
        }

    private:
        virtual const node_ptr clone_node(const node_ptr & n)
            throw (std::bad_alloc)
        {
            using std::set;

            assert(this->target_scope);

            node_ptr result;

            if (!n) { return result; }

            const bool already_traversed =
                (this->traversed_nodes.find(n.get())
                 != this->traversed_nodes.end());

            if (already_traversed) {
                result.reset(this->target_scope->find_node(n->id()));
                assert(result);
            } else {
                initial_value_map initial_values;
                const node_interface_set & interfaces = n->type.interfaces();
                for (node_interface_set::const_iterator interface =
                         interfaces.begin();
                     interface != interfaces.end();
                     ++interface) {
                    using std::string;
                    const node_interface::type_id type = interface->type;
                    const string & id = interface->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using boost::shared_ptr;
                        auto_ptr<field_value> val =
                            field_value::create(interface->field_type);
                        assert(val->type() == n->field(id).type());
                        this->clone_field_value(n, n->field(id), *val);
                        bool succeeded =
                            initial_values.insert(
                                make_pair(id, shared_ptr<field_value>(val)))
                            .second;
                        assert(succeeded);
                    }
                }
                result = n->type.create_node(this->target_scope,
                                             initial_values);
                if (!n->id().empty()) { result->id(n->id()); }
            }
            return result;
        }

        void clone_sfnode(const node_ptr & src_node,
                          const sfnode & src,
                          sfnode & dest)
            throw (std::bad_alloc)
        {
            dest.value = (src.value == src_node)
                       ? node_ptr::self
                       : this->clone_node(src.value);
        }

        void clone_mfnode(const node_ptr & src_node,
                          const mfnode & src,
                          mfnode & dest)
            throw (std::bad_alloc)
        {
            using std::swap;
            using std::vector;
            mfnode result(src.value.size());
            for (vector<node_ptr>::size_type i = 0;
                 i < src.value.size();
                 ++i) {
                result.value[i] = (src.value[i] == src_node)
                                ? node_ptr::self
                                : this->clone_node(src.value[i]);
            }
            swap(dest, result);
        }
    };

    //
    // Clone the implementation nodes.
    //
    class proto_impl_cloner : public field_value_cloner {
        const proto_node_class & node_class;
        const initial_value_map & initial_values_;

    public:
        proto_impl_cloner(const proto_node_class & node_class,
                          const initial_value_map & initial_values,
                          const scope_ptr & target_scope):
            field_value_cloner(target_scope),
            node_class(node_class),
            initial_values_(initial_values)
        {}

        const std::vector<node_ptr> clone() throw (std::bad_alloc)
        {
            using std::vector;

            vector<node_ptr> result(this->node_class.impl_nodes.size());

            for (vector<node_ptr>::size_type i = 0;
                 i < this->node_class.impl_nodes.size();
                 ++i) {
                result[i] = this->clone_node(this->node_class.impl_nodes[i]);
                assert(result[i]);
            }
            return result;
        }

    private:
        struct matches_is_target :
            std::unary_function<proto_node_class::is_map_t::value_type, bool> {

            explicit matches_is_target(
                const proto_node_class::is_target & is_target):
                is_target(&is_target)
            {}

            result_type operator()(const argument_type & is_map_value) const
            {
                return (is_map_value.second.impl_node
                        == this->is_target->impl_node)
                    && (is_map_value.second.impl_node_interface
                        == this->is_target->impl_node_interface);
            }

        private:
            const proto_node_class::is_target * is_target;
        };

        virtual const node_ptr clone_node(const node_ptr & n)
            throw (std::bad_alloc)
        {
            using std::set;

            assert(this->target_scope);

            node_ptr result;

            if (!n) { return result; }

            const bool already_traversed =
                (this->traversed_nodes.find(n.get())
                 != this->traversed_nodes.end());

            if (already_traversed) {
                result.reset(this->target_scope->find_node(n->id()));
                assert(result);
            } else {
                initial_value_map initial_values;
                const node_interface_set & interfaces = n->type.interfaces();
                for (node_interface_set::const_iterator interface =
                         interfaces.begin();
                     interface != interfaces.end();
                     ++interface) {
                    using std::string;
                    const node_interface::type_id type = interface->type;
                    const string & id = interface->id;
                    if (type == node_interface::exposedfield_id
                        || type == node_interface::field_id) {
                        using std::auto_ptr;
                        using std::find_if;
                        using boost::shared_ptr;
                        const field_value * src_val = 0;
                        auto_ptr<field_value> dest_val =
                            field_value::create(interface->field_type);
                        assert(dest_val->type() == n->field(id).type());

                        //
                        // If the field/exposedField is IS'd, get the value
                        // from the initial_values_, or alternatively the
                        // default_values_.
                        //
                        typedef proto_node_class::is_target is_target;
                        typedef proto_node_class::is_map_t is_map;
                        typedef proto_node_class::default_value_map_t
                            default_value_map;

                        is_map::const_iterator is_mapping =
                            find_if(this->node_class.is_map.begin(),
                                    this->node_class.is_map.end(),
                                    matches_is_target(
                                        is_target(*n, interface->id)));
                        if (is_mapping != this->node_class.is_map.end()) {
                            using openvrml_::compose2;
                            using std::logical_or;
                            //
                            // If an exposedField in the implementation is IS'd
                            // to an eventIn or an eventOut in the interface,
                            // we'll still get here.  So if the implementation
                            // node interface is an exposedField, we need to
                            // check to see if the PROTO interface is an
                            // eventIn or an eventOut.
                            //
                            node_interface_set::const_iterator
                                proto_interface =
                                find_if(this->node_class.interfaces.begin(),
                                        this->node_class.interfaces.end(),
                                        compose2(logical_or<bool>(),
                                                 bind2nd(node_interface_matches_exposedfield(),
                                                         is_mapping->first),
                                                 bind2nd(node_interface_matches_field(),
                                                         is_mapping->first)));

                            if (proto_interface
                                != this->node_class.interfaces.end()) {
                                initial_value_map::const_iterator
                                    initial_value =
                                    this->initial_values_.find(
                                        is_mapping->first);
                                if (initial_value
                                    != this->initial_values_.end()) {
                                    src_val = initial_value->second.get();
                                } else {
                                    default_value_map::const_iterator
                                        default_value =
                                        this->node_class.default_value_map
                                        .find(is_mapping->first);
                                    assert(default_value
                                           != this->node_class
                                           .default_value_map.end());
                                    src_val = default_value->second.get();
                                }
                            } else {
                                src_val = &n->field(id);
                            }
                        } else {
                            src_val = &n->field(id);
                        }

                        assert(src_val);
                        this->clone_field_value(n, *src_val, *dest_val);

                        bool succeeded =
                            initial_values.insert(
                                make_pair(id,
                                          shared_ptr<field_value>(dest_val)))
                            .second;
                        assert(succeeded);
                    }
                }
                result = n->type.create_node(this->target_scope,
                                             initial_values);
                if (!n->id().empty()) { result->id(n->id()); }
            }
            return result;
        }

        void clone_sfnode(const sfnode & src, sfnode & dest)
            throw (std::bad_alloc)
        {
            dest.value = this->clone_node(src.value);
        }

        void clone_mfnode(const mfnode & src, mfnode & dest)
            throw (std::bad_alloc)
        {
            using std::swap;
            using std::vector;
            mfnode result(src.value.size());
            for (vector<node_ptr>::size_type i = 0;
                 i < src.value.size();
                 ++i) {
                result.value[i] = this->clone_node(src.value[i]);
            }
            swap(dest, result);
        }
    };
}

/**
 * @brief Construct.
 *
 * @param type  node_type.
 * @param scope scope.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
proto_node::proto_node(const node_type & type,
                       const scope_ptr & scope,
                       const initial_value_map & initial_values)
    throw (std::bad_alloc):
    node(type, scope),
    proto_scope(scope)
{
    proto_node_class & node_class =
        static_cast<proto_node_class &>(type.node_class);

    this->impl_nodes = proto_impl_cloner(node_class,
                                         initial_values,
                                         this->proto_scope).clone();

    //
    // Establish routes.
    //
    typedef proto_node_class::routes_t routes_t;
    for (routes_t::const_iterator route = node_class.routes.begin();
         route != node_class.routes.end();
         ++route) {
        // XXX
        // XXX It would be better to store the node_paths along with the
        // XXX route instead of rebuilding them every time we instantiate
        // XXX the PROTO.
        // XXX
        node_path_t path_to_from;
        assert(!node_class.impl_nodes.empty());
        path_getter(*route->from, path_to_from)
            .get_path_from(node_class.impl_nodes);
        assert(!path_to_from.empty());
        node * const from_node = resolve_node_path(path_to_from,
                                                   this->impl_nodes);
        assert(from_node);

        node_path_t path_to_to;
        path_getter(*route->to, path_to_to)
            .get_path_from(node_class.impl_nodes);
        node * const to_node = resolve_node_path(path_to_to, this->impl_nodes);
        assert(to_node);

        try {
            add_route(*from_node, route->eventout, *to_node, route->eventin);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        } catch (field_value_type_mismatch & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    //
    // Add eventIns, eventOuts, exposedFields.
    //
    for (node_interface_set::const_iterator interface =
             node_class.interfaces.begin();
         interface != node_class.interfaces.end();
         ++interface) {
        using boost::shared_ptr;
        using boost::dynamic_pointer_cast;
        using std::pair;
        bool succeeded;
        shared_ptr<openvrml::event_listener> interface_eventin;
        shared_ptr<openvrml::event_emitter> interface_eventout;
        typedef proto_node_class::is_map_t is_map_t;
        pair<is_map_t::iterator, is_map_t::iterator> is_range;
        initial_value_map::const_iterator initial_value;
        switch (interface->type) {
        case node_interface::eventin_id:
            interface_eventin = create_eventin(interface->field_type, *this);
            is_range = node_class.is_map.equal_range(interface->id);
            for (is_map_t::const_iterator is_mapping = is_range.first;
                 is_mapping != is_range.second;
                 ++is_mapping) {
                assert(is_mapping->second.impl_node);
                node_path_t path_to_impl_node;
                path_getter(*is_mapping->second.impl_node, path_to_impl_node)
                    .get_path_from(node_class.impl_nodes);
                node * impl_node = resolve_node_path(path_to_impl_node,
                                                     this->impl_nodes);
                assert(impl_node);
                const std::string & impl_node_interface =
                    is_mapping->second.impl_node_interface;
                try {
                    openvrml::event_listener & impl_eventin =
                        impl_node->event_listener(impl_node_interface);
                    succeeded = eventin_is(interface->field_type,
                                           impl_eventin,
                                           *interface_eventin);
                    assert(succeeded);
                } catch (unsupported_interface & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            succeeded = this->eventin_map
                .insert(make_pair(interface->id, interface_eventin)).second;
            assert(succeeded);
            break;
        case node_interface::eventout_id:
            interface_eventout = create_eventout(interface->field_type, *this);
            is_range = node_class.is_map.equal_range(interface->id);
            for (is_map_t::const_iterator is_mapping = is_range.first;
                 is_mapping != is_range.second;
                 ++is_mapping) {
                assert(is_mapping->second.impl_node);
                node_path_t path_to_impl_node;
                path_getter(*is_mapping->second.impl_node, path_to_impl_node)
                    .get_path_from(node_class.impl_nodes);
                node * impl_node = resolve_node_path(path_to_impl_node,
                                                     this->impl_nodes);
                assert(impl_node);
                const std::string & impl_node_interface =
                    is_mapping->second.impl_node_interface;
                try {
                    openvrml::event_emitter & impl_eventout =
                        impl_node->event_emitter(impl_node_interface);
                    succeeded = eventout_is(interface->field_type,
                                            impl_eventout,
                                            *interface_eventout);
                    assert(succeeded);
                } catch (unsupported_interface & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            succeeded = this->eventout_map
                .insert(make_pair(interface->id, interface_eventout)).second;
            assert(succeeded);
            break;
        case node_interface::exposedfield_id:
            initial_value = initial_values.find(interface->id);
            if (initial_value == initial_values.end()) {
                initial_value =
                    node_class.default_value_map.find(interface->id);
                assert(initial_value != node_class.default_value_map.end());
            }
            interface_eventin = create_exposedfield(*initial_value->second,
                                                    *this);
            interface_eventout =
                dynamic_pointer_cast<openvrml::event_emitter>(
                    interface_eventin);
            is_range = node_class.is_map.equal_range(interface->id);
            for (is_map_t::const_iterator is_mapping = is_range.first;
                 is_mapping != is_range.second;
                 ++is_mapping) {
                assert(is_mapping->second.impl_node);
                node_path_t path_to_impl_node;
                path_getter(*is_mapping->second.impl_node, path_to_impl_node)
                    .get_path_from(node_class.impl_nodes);
                node * impl_node = resolve_node_path(path_to_impl_node,
                                                     this->impl_nodes);
                assert(impl_node);
                const std::string & impl_node_interface =
                    is_mapping->second.impl_node_interface;
                try {
                    openvrml::event_listener & impl_eventin =
                        impl_node->event_listener(impl_node_interface);
                    succeeded = eventin_is(interface->field_type,
                                           impl_eventin,
                                           *interface_eventin);
                    assert(succeeded);
                    openvrml::event_emitter & impl_eventout =
                        impl_node->event_emitter(impl_node_interface);
                    succeeded = eventout_is(interface->field_type,
                                            impl_eventout,
                                            *interface_eventout);
                    assert(succeeded);
                } catch (unsupported_interface & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            succeeded = this->eventin_map
                .insert(make_pair("set_" + interface->id,
                                  interface_eventin)).second;
            assert(succeeded);
            succeeded = this->eventout_map
                .insert(make_pair(interface->id + "_changed",
                                  interface_eventout)).second;
            assert(succeeded);
            break;
        case node_interface::field_id:
            break;
        default:
            assert(false);
        }
    }
}

/**
 * @brief Destroy.
 */
proto_node::~proto_node() throw ()
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void proto_node::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    using std::vector;
    for (vector<node_ptr>::const_iterator node = this->impl_nodes.begin();
         node != impl_nodes.end();
         ++node) {
        (*node)->initialize(*this->scene(), timestamp);
    }
}

/**
 * @brief Field accessor implementation.
 *
 * @param id    field identifier.
 *
 * @exception unsupported_interface if the node has no field @p id.
 *
 * @todo Make this function handle exposedFields.
 */
const field_value & proto_node::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    //
    // First, we need to find the implementation node that the field is
    // IS'd to.  For the accessor, we don't care if there's more than one.
    //
    proto_node_class & node_class =
        static_cast<proto_node_class &>(this->type.node_class);
    proto_node_class::is_map_t::iterator is_mapping =
        node_class.is_map.find(id);
    if (is_mapping != node_class.is_map.end()) {
        //
        // Get the path to the implementation node.
        //
        assert(is_mapping->second.impl_node);
        assert(!is_mapping->second.impl_node_interface.empty());
        node_path_t path;
        path_getter(*is_mapping->second.impl_node, path)
            .get_path_from(node_class.impl_nodes);

        //
        // Resolve the path against this instance's implementation nodes.
        //
        node * const impl_node = resolve_node_path(path, this->impl_nodes);

        //
        // Set the field value for the implementation node.
        //
        return impl_node->field(is_mapping->second.impl_node_interface);
    } else {
        //
        // If there are no IS mappings for the field, then return the
        // default value.
        //
        proto_node_class::default_value_map_t::iterator default_value =
            node_class.default_value_map.find(id);
        if (default_value == node_class.default_value_map.end()) {
            throw unsupported_interface(this->type, id);
        }
        return *default_value->second;
    }
}

/**
 * @brief event_listener accessor implementation.
 *
 * @param id    eventIn identifier.
 *
 * @return the event_listener for the eventIn @p id.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 */
event_listener &
proto_node::do_event_listener(const std::string & id)
    throw (unsupported_interface)
{
    eventin_map_t::iterator pos = this->eventin_map.find(id);
    if (pos == this->eventin_map.end()) {
        throw unsupported_interface(this->type,
                                    node_interface::eventin_id,
                                    id);
    }
    return *pos->second;
}

/**
 * @brief event_emitter accessor implementation.
 *
 * @param id    eventOut identifier.
 *
 * @return the event_emitter for the eventOut @p id.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */
event_emitter & proto_node::do_event_emitter(const std::string & id)
    throw (unsupported_interface)
{
    eventout_map_t::iterator pos = this->eventout_map.find(id);
    if (pos == this->eventout_map.end()) {
        throw unsupported_interface(this->type,
                                    node_interface::eventout_id,
                                    id);
    }
    return *pos->second;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void proto_node::do_shutdown(const double timestamp) throw ()
{
    using std::vector;
    for (vector<node_ptr>::const_iterator node = this->impl_nodes.begin();
         node != impl_nodes.end();
         ++node) {
        (*node)->shutdown(timestamp);
    }
}

/**
 * @brief Cast to a script_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a script_node, or 0 otherwise.
 */
script_node * proto_node::to_script() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<script_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to an appearance_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      an appearance_node, or 0 otherwise.
 */
appearance_node * proto_node::to_appearance() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<appearance_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a child_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a child_node, or 0 otherwise.
 */
child_node * proto_node::to_child() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<child_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a color_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a color_node, or 0 otherwise.
 */
color_node * proto_node::to_color() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<color_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a coordinate_node, or 0 otherwise.
 */
coordinate_node * proto_node::to_coordinate() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<coordinate_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a font_style_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a font_style_node, or 0 otherwise.
 */
font_style_node * proto_node::to_font_style() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<font_style_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a geometry_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a geometry_node, or 0 otherwise.
 */
geometry_node * proto_node::to_geometry() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<geometry_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a grouping_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a geometry_node, or 0 otherwise.
 */
grouping_node * proto_node::to_grouping() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<grouping_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a material_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a material_node, or 0 otherwise.
 */
material_node * proto_node::to_material() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<material_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a normal_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a normal_node, or 0 otherwise.
 */
normal_node * proto_node::to_normal() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<normal_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a sound_source_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a sound_source_node, or 0 otherwise.
 */
sound_source_node * proto_node::to_sound_source() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<sound_source_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a texture_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_node, or 0 otherwise.
 */
texture_node * proto_node::to_texture() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<texture_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a texture_coordinate_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_coordinate_node, or 0 otherwise.
 */
texture_coordinate_node * proto_node::to_texture_coordinate() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<texture_coordinate_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a texture_transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a texture_transform_node, or 0 otherwise.
 */
texture_transform_node * proto_node::to_texture_transform() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<texture_transform_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a transform_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a transform_node, or 0 otherwise.
 */
transform_node * proto_node::to_transform() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<transform_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Cast to a viewpoint_node.
 *
 * @return a pointer to the first node in the implementation if that node is
 *      a viewpoint_node, or 0 otherwise.
 */
viewpoint_node * proto_node::to_viewpoint() throw ()
{
    assert(!this->impl_nodes.empty());
    assert(this->impl_nodes[0]);
    return node_cast<viewpoint_node *>(this->impl_nodes[0].get());
}

/**
 * @brief Construct.
 *
 * @param browser
 * @param interfaces
 * @param default_value_map
 * @param impl_nodes
 * @param is_map
 * @param routes
 */
proto_node_class::
proto_node_class(openvrml::browser & browser,
                 const node_interface_set & interfaces,
                 const default_value_map_t & default_value_map,
                 const std::vector<node_ptr> & impl_nodes,
                 const is_map_t & is_map,
                 const routes_t & routes):
    node_class(browser),
    interfaces(interfaces),
    default_value_map(default_value_map),
    impl_nodes(impl_nodes),
    routes(routes),
    is_map(is_map)
{}

proto_node_class::~proto_node_class() throw ()
{}

const node_type_ptr
proto_node_class::create_type(const std::string & id,
                              const node_interface_set & interfaces)
    throw (unsupported_interface, std::bad_alloc)
{
    return node_type_ptr(new proto_node_type(*this, id, interfaces));
}

} // namespace openvrml

//
// Including a .cpp file is strange, but it's exactly what we want to do here.
// This usage lets us put the ANTLR parser in an unnamed namespace.
//
#include "Vrml97Parser.cpp"
} // Close "namespace openvrml", opened in Vrml97Parser.cpp.

// Max time in seconds between updates. Make this user
// setable to balance performance with cpu usage.
#ifndef DEFAULT_DELTA
#define DEFAULT_DELTA 0.5
#endif

/**
 * @namespace openvrml
 *
 * @brief The OpenVRML Runtime Library
 */
namespace openvrml {

class null_node_class : public node_class {
public:
    explicit null_node_class(openvrml::browser & browser) throw ();
    virtual ~null_node_class() throw ();

    virtual const node_type_ptr
    create_type(const std::string & id,
                const node_interface_set & interfaces)
        throw ();
};


class null_node_type : public node_type {
public:
    explicit null_node_type(null_node_class & nodeClass) throw ();
    virtual ~null_node_type() throw ();

    virtual const node_interface_set & interfaces() const throw ();
    virtual const node_ptr
    create_node(const scope_ptr & scope,
                const initial_value_map & initial_values) const
        throw ();
};


class DefaultViewpoint : public viewpoint_node {
    mat4f userViewTransform;

public:
    explicit DefaultViewpoint(const null_node_type & nodeType) throw ();
    virtual ~DefaultViewpoint() throw ();

    virtual const mat4f & transformation() const throw ();
    virtual const mat4f & user_view_transform() const throw ();
    virtual void user_view_transform(const mat4f & transform) throw ();
    virtual const std::string & description() const throw ();
    virtual float field_of_view() const throw ();

private:
    virtual void do_field(const std::string & id, const field_value & value)
        throw ();
    virtual const field_value & do_field(const std::string & id) const
        throw ();
    virtual void do_process_event(const std::string & id,
                                  const field_value & value,
                                  double timestamp)
        throw ();
    virtual const field_value & do_eventout(const std::string & id) const
        throw ();

    virtual openvrml::event_listener &
    do_event_listener(const std::string & id)
        throw (unsupported_interface);
    virtual openvrml::event_emitter &
    do_event_emitter(const std::string & id)
        throw (unsupported_interface);
};


/**
 * @var const double pi
 *
 * @brief pi
 */

/**
 * @var const double pi_2
 *
 * @brief pi/2
 */

/**
 * @var const double pi_4
 *
 * @brief pi/4
 */

/**
 * @var const double inv_pi
 *
 * @brief 1/pi
 */

/**
 * @class invalid_vrml
 *
 * @brief Exception thrown when the parser fails due to errors in the VRML
 *      input.
 */

/**
 * @var const std::string invalid_vrml::url
 *
 * @brief Resource identifier.
 */

/**
 * @var size_t invalid_vrml::line
 *
 * @brief Line number.
 */

/**
 * @var size_t invalid_vrml::column
 *
 * @brief Column number.
 */

/**
 * @brief Construct.
 *
 * @param url       resource identifier of the stream.
 * @param line      line number where the error was detected.
 * @param column    column number where the error was detected.
 * @param message   description of the error.
 */
invalid_vrml::invalid_vrml(const std::string & url,
                           const size_t line,
                           const size_t column,
                           const std::string & message):
    std::runtime_error(message),
    url(url),
    line(line),
    column(column)
{}

/**
 * @brief Destroy.
 */
invalid_vrml::~invalid_vrml() throw ()
{}


/**
 * @class viewer_in_use
 *
 * @brief Exception thrown when attempting to associate a <code>viewer</code>
 *        with a <code>browser</code> when the <code>viewer</code> is already
 *        associated with a <code>browser</code>.
 */

/**
 * @brief Construct.
 */
viewer_in_use::viewer_in_use():
    std::invalid_argument("viewer in use")
{}

/**
 * @brief Destroy.
 */
viewer_in_use::~viewer_in_use() throw ()
{}


/**
 * @class browser
 *
 * @brief Encapsulates a VRML browser.
 */

/**
 * @enum browser::cb_reason
 *
 * @brief Valid reasons for browser callback.
 */

/**
 * @var browser::cb_reason browser::destroy_world_id
 *
 * @brief Destroy the world.
 */

/**
 * @var browser::cb_reason browser::replace_world_id
 *
 * @brief Replace the world.
 */

/**
 * @typedef browser::scene_cb
 *
 * @brief A pointer to a browser callback function.
 *
 * The callback function provoides a way to let the app know when a world is
 * loaded, changed, etc.
 */

/**
 * @var std::auto_ptr<null_node_class> browser::null_node_class_
 *
 * @brief "Null" class object for default nodes (e.g., default_viewpoint).
 */

/**
 * @var std::auto_ptr<null_node_type> browser::null_node_type_
 *
 * @brief "Null" type object for default nodes (e.g., default_viewpoint).
 */

/**
 * @typedef browser::node_class_map_t
 *
 * @brief Maps URIs to @link node_class node_classes@endlink.
 */

/**
 * @var browser::node_class_map_t browser::node_class_map
 *
 * @brief A map of URIs to node implementations.
 */

/**
 * @var script_node_class browser::script_node_class_
 *
 * @brief node_class for Script nodes in the browser.
 */

/**
 * @var scene * browser::scene_
 *
 * @brief Pointer to the root scene.
 */

/**
 * @var node_ptr browser::default_viewpoint
 *
 * @brief The "default" viewpoint_node used when no viewpoint_node in the scene
 *        is bound.
 */

/**
 * @var viewpoint_node * browser::active_viewpoint_
 *
 * @brief The currently "active" viewpoint_node.
 */

/**
 * @var std::list<viewpoint_node *> browser::viewpoint_list
 *
 * @brief A list of all the Viewpoint nodes in the browser.
 */

/**
 * @typedef browser::bind_stack_t
 *
 * @brief A list of bound nodes.
 */

/**
 * @var browser::bind_stack_t browser::navigation_info_stack
 *
 * @brief The stack of bound NavigationInfo nodes.
 */

/**
 * @var std::list<node *> browser::navigation_infos
 *
 * @brief A list of all the NavigationInfo nodes in the browser.
 */

/**
 * @var std::list<node *> browser::scoped_lights
 *
 * @brief A list of all the scoped light nodes in the browser.
 */

/**
 * @var std::list<script_node *> browser::scripts
 *
 * @brief A list of all the Script nodes in the browser.
 */

/**
 * @var std::list<node *> browser::timers
 *
 * @brief A list of all the TimeSensor nodes in the browser.
 */

/**
 * @var std::list<node *> browser::audio_clips
 *
 * @brief A list of all the AudioClip nodes in the browser.
 */

/**
 * @var std::list<node *> browser::movies
 *
 * @brief A list of all the MovieTexture nodes in the browser.
 */

/**
 * @var bool browser::modified_
 *
 * @brief Flag to indicate whether the browser has been modified.
 */

/**
 * @var bool browser::new_view
 *
 * @brief Flag to indicate if the user has changed to a new view.
 */

/**
 * @var double browser::delta_time
 *
 * @brief Time elapsed since the last update.
 */

/**
 * @var openvrml::viewer * browser::viewer_
 *
 * @brief The current <code>viewer</code>.
 */

/**
 * @typedef browser::scene_cb_list_t
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @struct browser::event
 *
 * @brief An event.
 *
 * An event has a value and a destination, and is associated with a time.
 */

/**
 * @var double browser::event::timestamp
 *
 * @brief The timestamp of the event.
 */

/**
 * @var field_value * browser::event::value
 *
 * @brief The value associated with the event.
 */

/**
 * @var node_ptr browser::event::to_node
 *
 * @brief The node the event is going to.
 */

/**
 * @var std::string browser::event::to_eventin
 *
 * @brief The eventIn of @a to_node the event is going to.
 */

/**
 * @var browser::scene_cb_list_t browser::scene_callbacks
 *
 * @brief List of functions to call when the world is changed.
 */

/**
 * @var double browser::frame_rate_
 *
 * @brief Frame rate.
 */

/**
 * @var browser::max_events
 *
 * @brief The maximum number of events which may be queued.
 *
 * Each browser can have a limited number of pending events.
 * Repeatedly allocating/freeing events is slow (it would be
 * nice to get rid of the field cloning, too), and if there are
 * so many events pending, we are probably running too slow to
 * handle them effectively anyway.
 */

/**
 * @var browser::event browser::event_mem
 *
 * @brief The event queue.
 *
 * @todo The event queue ought to be sorted by timestamp.
 */

/**
 * @var size_t browser::first_event
 *
 * @brief Index of the first pending event.
 */

/**
 * @var size_t browser::last_event
 *
 * @brief Index of the last pending event.
 */

/**
 * @brief Get the current time.
 */
double browser::current_time() throw ()
{
    double currentTime;
# ifdef _WIN32
    _timeb timebuffer;
    _ftime(&timebuffer);
    currentTime = double(timebuffer.time) + 1.0e-3 * double(timebuffer.millitm);
# else
    timeval tv;
    const int result = gettimeofday(&tv, 0);
    assert(result == 0);

    currentTime = double(tv.tv_sec) + 1.0e-6 * double(tv.tv_usec);
# endif
    return currentTime;
}

/**
 * @var std::ostream & browser::out
 *
 * @brief Output stream, generally for console output.
 */

/**
 * @var std::ostream & browser::err
 *
 * @brief Error output stream.
 */

/**
 * @var bool browser::flags_need_updating
 *
 * @brief Set by node::setBVolumeDirty on any node in this browser graph,
 *      cleared by update_flags.
 *
 * @c true if the bvolume dirty flag has been set on a node in the
 * browser graph, but has not yet been propegated to that node's
 * ancestors.
 */

/**
 * @brief Constructor.
 *
 * @param out   output stream for console output.
 * @param err   output stream for error console output.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
browser::browser(std::ostream & out, std::ostream & err)
    throw (std::bad_alloc):
    null_node_class_(new null_node_class(*this)),
    null_node_type_(new null_node_type(*null_node_class_)),
    script_node_class_(*this),
    scene_(0),
    default_viewpoint(new DefaultViewpoint(*null_node_type_)),
    active_viewpoint_(node_cast<viewpoint_node *>(default_viewpoint.get())),
    modified_(false),
    new_view(false),
    delta_time(DEFAULT_DELTA),
    viewer_(0),
    frame_rate_(0.0),
    first_event(0),
    last_event(0),
    out(out),
    err(err),
    flags_need_updating(false)
{
    assert(this->active_viewpoint_);
    this->init_node_class_map();
}

/**
 * @brief Destructor.
 */
browser::~browser() throw ()
{
    const double now = browser::current_time();

    if (this->scene_) { this->scene_->shutdown(now); }
    delete this->scene_;
    this->scene_ = 0;
    this->navigation_info_stack.clear();
    assert(this->viewpoint_list.empty());
    assert(this->navigation_infos.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());
    assert(this->audio_clips.empty());
    assert(this->movies.empty());
    this->node_class_map.clear();
}

/**
 * @brief Get the root nodes for the browser.
 *
 * @return the root nodes for the browser.
 */
const std::vector<node_ptr> & browser::root_nodes() const throw ()
{
    assert(this->scene_);
    return this->scene_->nodes();
}

/**
 * @brief Get the path to a node in the scene graph.
 *
 * @param n  the objective node.
 *
 * @return the path to @p node, starting with a root node, and ending with
 *         @p node. If @p node is not in the scene graph, the returned
 *         node_path is empty.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const node_path browser::find_node(const node & n) const
    throw (std::bad_alloc)
{
    assert(this->scene_);

    class FindNodeTraverser : public node_traverser {
        const node & objectiveNode;
        node_path & nodePath;

    public:
        FindNodeTraverser(const node & objectiveNode, node_path & nodePath)
            throw ():
            objectiveNode(objectiveNode),
            nodePath(nodePath)
        {}

        virtual ~FindNodeTraverser() throw()
        {}

    private:
        virtual void on_entering(node & n) throw (std::bad_alloc)
        {
            this->nodePath.push_back(&n);
            if (&n == &this->objectiveNode) { this->halt_traversal(); }
        }

        virtual void on_leaving(node & n) throw ()
        {
            if (&n != &this->objectiveNode) { this->nodePath.pop_back(); }
        }
    };

    node_path nodePath;
    FindNodeTraverser(n, nodePath).traverse(this->scene_->nodes());
    return nodePath;
}

/**
 * @brief Get the active viewpoint_node.
 *
 * The active viewpoint_node is the one currently associated with the user
 * view.
 *
 * @return the active viewpoint_node.
 */
viewpoint_node & browser::active_viewpoint() const throw ()
{
    return *this->active_viewpoint_;
}

/**
 * @brief Set the active viewpoint_node.
 *
 * @param viewpoint a viewpoint_node.
 */
void browser::active_viewpoint(viewpoint_node & viewpoint) throw ()
{
    this->active_viewpoint_ = &viewpoint;
}

/**
 * @brief Reset the active viewpoint_node to the default.
 */
void browser::reset_default_viewpoint() throw ()
{
    assert(this->default_viewpoint);
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint.get());
    assert(this->active_viewpoint_);
}

/**
 * @brief Add a viewpoint_node to the list of viewpoint_nodes for the browser.
 *
 * @param viewpoint a viewpoint_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @pre @p viewpoint is not in the list of viewpoint_nodes for the browser.
 */
void browser::add_viewpoint(viewpoint_node & viewpoint) throw (std::bad_alloc)
{
    assert(std::find(this->viewpoint_list.begin(), this->viewpoint_list.end(),
                     &viewpoint) == this->viewpoint_list.end());
    this->viewpoint_list.push_back(&viewpoint);
}

/**
 * @brief Remove a viewpoint_node from the list of viewpoint_nodes for the
 *      browser.
 *
 * @param viewpoint a viewpoint_node.
 *
 * @pre @p viewpoint is in the list of viewpoint_nodes for the browser.
 */
void browser::remove_viewpoint(viewpoint_node & viewpoint) throw ()
{
    assert(!this->viewpoint_list.empty());
    typedef std::list<viewpoint_node *> viewpoint_list_t;
    const viewpoint_list_t::iterator end = this->viewpoint_list.end();
    const viewpoint_list_t::iterator pos =
            std::find(this->viewpoint_list.begin(), end, &viewpoint);
    assert(pos != end);
    this->viewpoint_list.erase(pos);
}

/**
 * @brief Get the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 *
 * @return the list of @link viewpoint_node viewpoint_nodes@endlink for the
 *      world.
 */
const std::list<viewpoint_node *> & browser::viewpoints() const throw ()
{
    return this->viewpoint_list;
}

/**
 * @brief Set the current <code>viewer</code>.
 *
 * @param v <code>viewer</code>.
 *
 * @exception viewer_in_use if @p v is already associated with a
 *                          <code>browser</code>.
 */
void browser::viewer(openvrml::viewer * v) throw (viewer_in_use)
{
    if (v && v->browser_) { throw viewer_in_use(); }
    if (this->viewer_) { this->viewer_->browser_ = 0; }
    this->viewer_ = v;
    if (v) { v->browser_ = this; }
}

/**
 * @brief The current <code>viewer</code>.
 *
 * @return the current <code>viewer</code>.
 */
viewer * browser::viewer() throw ()
{
    return this->viewer_;
}

/**
 * @brief Get the browser name.
 *
 * @return "OpenVRML"
 *
 * Specific browsers may wish to override this method.
 */
const char * browser::name() const throw ()
{
    return "OpenVRML";
}

/**
 * @brief Get the browser version.
 *
 * @return the version of openvrml.
 *
 * Specific browsers may wish to override this method.
 */
const char * browser::version() const throw ()
{
    return PACKAGE_VERSION;
}

/**
 * @brief Get the average navigation speed in meters per second.
 *
 * @return the average navigation speed.
 */
float browser::current_speed()
{
    vrml97_node::navigation_info_node * const navInfo =
        bindable_navigation_info_top();
    if (navInfo) { return navInfo->speed(); }
    return 0.0f;
}

/**
 * @brief Get the URI for the world.
 *
 * @return the URI for the world.
 */
const std::string  browser::world_url() const throw (std::bad_alloc)
{
    static const std::string empty_string;
    return this->scene_
        ? this->scene_->url() // Throws std::bad_alloc.
        : empty_string;
}

/**
 * @todo Implement me!
 */
void browser::replace_world(const std::vector<node_ptr> & nodes)
{}

namespace {
    typedef std::map<std::string, node_class_ptr> node_class_map_t;

    struct InitNodeClass : std::unary_function<void,
                                               node_class_map_t::value_type>
    {
        explicit InitNodeClass(viewpoint_node * initialViewpoint,
                               const double time)
            throw ():
            initialViewpoint(initialViewpoint),
            time(time)
        {}

        void operator()(const node_class_map_t::value_type & value) const
            throw ()
        {
            assert(value.second);
            value.second->initialize(this->initialViewpoint, this->time);
        }

    private:
        viewpoint_node * initialViewpoint;
        double time;
    };


    struct HasId_ : std::unary_function<bool, viewpoint_node *> {
        explicit HasId_(const std::string & nodeId) throw ():
            nodeId(&nodeId)
        {}

        bool operator()(viewpoint_node * const node) const throw ()
        {
            return node->id() == *this->nodeId;
        }

    private:
        const std::string * nodeId;
    };


    class URI {
        std::string str;
        enum { nmatch = 11 };
        regmatch_t regmatch[nmatch];

    public:
        URI(const std::string & str) throw (invalid_url, std::bad_alloc);

        operator std::string() const throw (std::bad_alloc);

        const std::string getScheme() const throw (std::bad_alloc);
        const std::string getSchemeSpecificPart() const throw (std::bad_alloc);
        const std::string getAuthority() const throw (std::bad_alloc);
        const std::string getUserinfo() const throw (std::bad_alloc);
        const std::string getHost() const throw (std::bad_alloc);
        const std::string getPort() const throw (std::bad_alloc);
        const std::string getPath() const throw (std::bad_alloc);
        const std::string getQuery() const throw (std::bad_alloc);
        const std::string getFragment() const throw (std::bad_alloc);

        const URI resolveAgainst(const URI & absoluteURI) const
                throw (std::bad_alloc);
    };
}

/**
 * @brief Load a resource into the browser.
 *
 * The default implementation of this method simply loads a VRML world
 * into the browser.  If the resource at @p url is not a VRML world, this
 * method as no effect.
 *
 * Implementations should override this method to handle non-VRML resources,
 * handing them off to a Web browser or other appropriate handler.
 *
 * @param url       a URI.
 * @param parameter parameters for @p url.
 *
 * @todo Make this asynchronous.
 */
void browser::load_url(const std::vector<std::string> & url,
                       const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::for_each;
    using std::list;
    using std::string;

    const double now = browser::current_time();

    //
    // Clear out the current scene.
    //
    if (this->scene_) { this->scene_->shutdown(now); }
    delete this->scene_;
    this->scene_ = 0;
    this->active_viewpoint_ =
        node_cast<viewpoint_node *>(this->default_viewpoint.get());
    this->navigation_info_stack.clear();
    assert(this->viewpoint_list.empty());
    assert(this->navigation_infos.empty());
    assert(this->scoped_lights.empty());
    assert(this->scripts.empty());
    assert(this->timers.empty());
    assert(this->audio_clips.empty());
    assert(this->movies.empty());
    this->node_class_map.clear();

    //
    // Create the new scene.
    //
    this->init_node_class_map();
    try {
        this->scene_ = new scene(*this, url);
        this->scene_->initialize(now);

        //
        // Get the initial viewpoint_node, if any was specified.
        //
        viewpoint_node * initialViewpoint = 0;
        const string viewpointNodeId = URI(this->scene_->url()).getFragment();
        if (!viewpointNodeId.empty()) {
            if (!this->scene_->nodes().empty()) {
                const node_ptr & n = this->scene_->nodes()[0];
                if (n) {
                    node * const vp = n->scope()->find_node(viewpointNodeId);
                    initialViewpoint = dynamic_cast<viewpoint_node *>(vp);
                }
            }
        }

        //
        // Initialize the node_classes.
        //
        for_each(this->node_class_map.begin(), this->node_class_map.end(),
                 InitNodeClass(initialViewpoint, now));

        //
        // Send initial bind events to bindable nodes.
        //
        if (!this->navigation_infos.empty()) {
            assert(this->navigation_infos.front());
            event_listener & listener =
                navigation_infos.front()->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), now);
        }

        if (this->active_viewpoint_
            != node_cast<viewpoint_node *>(this->default_viewpoint.get())) {
            event_listener & listener =
                this->active_viewpoint_->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), now);
        }
    } catch (invalid_vrml & ex) {
        this->err << ex.url << ':' << ex.line << ':' << ex.column
                  << ": error: " << ex.what() << std::endl;
    } catch (no_alternative_url & ex) {
        this->err << ex.what() << std::endl;
    }
    this->modified(true);
    this->new_view = true; // Force resetUserNav
}

/**
 * @brief Send a string to the user interface.
 *
 * The default implementation of this method simply prints @p description to
 * @a out.  Subclasses can override this method to direct messages to an
 * application's UI; for instance, a status bar.
 *
 * @param description   a string.
 */
void browser::description(const std::string & description)
{
    this->out << description << std::endl;
}

/**
 * @brief Generate nodes from a stream of VRML syntax.
 *
 * In addition to the exceptions listed, this method may throw any
 * exception that may result from reading the input stream.
 *
 * @param in    an input stream.
 *
 * @return the root nodes generated from @p in.
 *
 * @exception invalid_vrml      if @p in has invalid VRML syntax.
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::vector<node_ptr> browser::create_vrml_from_stream(std::istream & in)
{
    std::vector<node_ptr> nodes;
    try {
        Vrml97Scanner scanner(in);
        Vrml97Parser parser(scanner, "");
        parser.vrmlScene(*this, nodes);
    } catch (antlr::RecognitionException & ex) {
        throw invalid_vrml(ex.getFilename(),
                           ex.getLine(),
                           ex.getColumn(),
                           ex.getMessage());
    } catch (antlr::ANTLRException & ex) {
        throw std::runtime_error(ex.getMessage());
    }
    return nodes;
}

/**
 * @todo Implement me!
 */
void browser::create_vrml_from_url(const std::vector<std::string> & url,
                                   const node_ptr & node,
                                   const std::string & event)
{}

/**
 * @brief Initialize the node_class map with the available node
 *        implementations.
 */
void browser::init_node_class_map() {
    using namespace vrml97_node;
    this->node_class_map["urn:X-openvrml:node:Anchor"] =
        node_class_ptr(new anchor_class(*this));
    this->node_class_map["urn:X-openvrml:node:Appearance"] =
        node_class_ptr(new appearance_class(*this));
    this->node_class_map["urn:X-openvrml:node:AudioClip"] =
        node_class_ptr(new audio_clip_class(*this));
    this->node_class_map["urn:X-openvrml:node:Background"] =
        node_class_ptr(new background_class(*this));
    this->node_class_map["urn:X-openvrml:node:Billboard"] =
        node_class_ptr(new billboard_class(*this));
    this->node_class_map["urn:X-openvrml:node:Box"] =
        node_class_ptr(new box_class(*this));
    this->node_class_map["urn:X-openvrml:node:Collision"] =
        node_class_ptr(new collision_class(*this));
    this->node_class_map["urn:X-openvrml:node:Color"] =
        node_class_ptr(new color_class(*this));
    this->node_class_map["urn:X-openvrml:node:ColorInterpolator"] =
        node_class_ptr(new color_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:Cone"] =
        node_class_ptr(new cone_class(*this));
    this->node_class_map["urn:X-openvrml:node:Coordinate"] =
        node_class_ptr(new coordinate_class(*this));
    this->node_class_map["urn:X-openvrml:node:CoordinateInterpolator"] =
        node_class_ptr(new coordinate_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:Cylinder"] =
        node_class_ptr(new cylinder_class(*this));
    this->node_class_map["urn:X-openvrml:node:CylinderSensor"] =
        node_class_ptr(new cylinder_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:DirectionalLight"] =
        node_class_ptr(new directional_light_class(*this));
    this->node_class_map["urn:X-openvrml:node:ElevationGrid"] =
        node_class_ptr(new elevation_grid_class(*this));
    this->node_class_map["urn:X-openvrml:node:Extrusion"] =
        node_class_ptr(new extrusion_class(*this));
    this->node_class_map["urn:X-openvrml:node:Fog"] =
        node_class_ptr(new fog_class(*this));
    this->node_class_map["urn:X-openvrml:node:FontStyle"] =
        node_class_ptr(new font_style_class(*this));
    this->node_class_map["urn:X-openvrml:node:Group"] =
        node_class_ptr(new group_class(*this));
    this->node_class_map["urn:X-openvrml:node:ImageTexture"] =
        node_class_ptr(new image_texture_class(*this));
    this->node_class_map["urn:X-openvrml:node:IndexedFaceSet"] =
        node_class_ptr(new indexed_face_set_class(*this));
    this->node_class_map["urn:X-openvrml:node:IndexedLineSet"] =
        node_class_ptr(new indexed_line_set_class(*this));
    this->node_class_map["urn:X-openvrml:node:Inline"] =
        node_class_ptr(new inline_class(*this));
    this->node_class_map["urn:X-openvrml:node:LOD"] =
        node_class_ptr(new lod_class(*this));
    this->node_class_map["urn:X-openvrml:node:Material"] =
        node_class_ptr(new material_class(*this));
    this->node_class_map["urn:X-openvrml:node:MovieTexture"] =
        node_class_ptr(new movie_texture_class(*this));
    this->node_class_map["urn:X-openvrml:node:NavigationInfo"] =
        node_class_ptr(new navigation_info_class(*this));
    this->node_class_map["urn:X-openvrml:node:Normal"] =
        node_class_ptr(new normal_class(*this));
    this->node_class_map["urn:X-openvrml:node:NormalInterpolator"] =
        node_class_ptr(new normal_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:OrientationInterpolator"] =
        node_class_ptr(new orientation_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:PixelTexture"] =
        node_class_ptr(new pixel_texture_class(*this));
    this->node_class_map["urn:X-openvrml:node:PlaneSensor"] =
        node_class_ptr(new plane_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:PointLight"] =
        node_class_ptr(new point_light_class(*this));
    this->node_class_map["urn:X-openvrml:node:PointSet"] =
        node_class_ptr(new point_set_class(*this));
    this->node_class_map["urn:X-openvrml:node:PositionInterpolator"] =
        node_class_ptr(new position_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:ProximitySensor"] =
        node_class_ptr(new proximity_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:ScalarInterpolator"] =
        node_class_ptr(new scalar_interpolator_class(*this));
    this->node_class_map["urn:X-openvrml:node:Shape"] =
        node_class_ptr(new shape_class(*this));
    this->node_class_map["urn:X-openvrml:node:Sound"] =
        node_class_ptr(new sound_class(*this));
    this->node_class_map["urn:X-openvrml:node:Sphere"] =
        node_class_ptr(new sphere_class(*this));
    this->node_class_map["urn:X-openvrml:node:SphereSensor"] =
        node_class_ptr(new sphere_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:SpotLight"] =
        node_class_ptr(new spot_light_class(*this));
    this->node_class_map["urn:X-openvrml:node:Switch"] =
        node_class_ptr(new switch_class(*this));
    this->node_class_map["urn:X-openvrml:node:Text"] =
        node_class_ptr(new text_class(*this));
    this->node_class_map["urn:X-openvrml:node:TextureCoordinate"] =
        node_class_ptr(new texture_coordinate_class(*this));
    this->node_class_map["urn:X-openvrml:node:TextureTransform"] =
        node_class_ptr(new texture_transform_class(*this));
    this->node_class_map["urn:X-openvrml:node:TimeSensor"] =
        node_class_ptr(new time_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:TouchSensor"] =
        node_class_ptr(new touch_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:Transform"] =
        node_class_ptr(new transform_class(*this));
    this->node_class_map["urn:X-openvrml:node:Viewpoint"] =
        node_class_ptr(new viewpoint_class(*this));
    this->node_class_map["urn:X-openvrml:node:VisibilitySensor"] =
        node_class_ptr(new visibility_sensor_class(*this));
    this->node_class_map["urn:X-openvrml:node:WorldInfo"] =
        node_class_ptr(new world_info_class(*this));
}

/**
 * @brief Execute browser callback functions.
 *
 * @param reason    the cb_reason to pass to the callback functions.
 */
void browser::do_callbacks(const cb_reason reason)
{
    scene_cb_list_t::iterator cb, cbend = this->scene_callbacks.end();
    for (cb = this->scene_callbacks.begin(); cb != cbend; ++cb) {
        (*cb)(reason);
    }
}

/**
 * @brief Add a callback function to be called when the world changes.
 *
 * @param cb    a browser callback function.
 */
void browser::add_world_changed_callback(const scene_cb cb)
{
    this->scene_callbacks.push_front(cb);
}

/**
 * @brief Get the current frame rate.
 *
 * @return the current frame rate.
 */
double browser::frame_rate() const
{
    return this->frame_rate_;
}

/**
 * @brief Queue an event for a node.
 *
 * Current events are in the array @a event_mem[first_event, last_event). If
 * @a first_event == @a last_event, the queue is empty. There is a fixed
 * maximum number of events. If we are so far behind that the queue is filled,
 * the oldest events get overwritten.
 */
void browser::queue_event(double timestamp,
                          field_value * value,
                          const node_ptr & to_node,
                          const std::string & to_eventin)
{
    event * e = &this->event_mem[this->last_event];
    e->timestamp = timestamp;
    e->value = value;
    e->to_node = to_node;
    e->to_eventin = to_eventin;
    this->last_event = (this->last_event + 1) % max_events;

    // If the event queue is full, discard the oldest (in terms of when it
    // was put on the queue, not necessarily in terms of earliest timestamp).
    if (this->last_event == this->first_event) {
        e = &this->event_mem[this->last_event];
        delete e->value;
        this->first_event = (this->first_event + 1) % max_events;
    }
}

/**
 * @brief Check if any events are waiting to be distributed.
 *
 * @return @c true if there are pending events, @c false otherwise.
 */
bool browser::events_pending()
{
    return this->first_event != this->last_event;
}


/**
 * @brief Discard all pending events
 */
void browser::flush_events()
{
    while (this->first_event != this->last_event) {
        event *e = &this->event_mem[this->first_event];
        this->first_event = (this->first_event + 1) % max_events;
        delete e->value;
    }
}

/**
 * Called by the viewer when the cursor passes over, clicks, drags, or
 * releases a sensitive object (an Anchor or another grouping node with
 * an enabled TouchSensor child).
 */
void browser::sensitive_event(node * const n,
                              const double timestamp,
                              const bool is_over,
                              const bool is_active,
                              double * const point)
{
    if (n) {
        vrml97_node::anchor_node * a = n->to_anchor();
        if (a) {
            //
            // This should really be (isOver && !isActive && n->wasActive)
            // (ie, button up over the anchor after button down over the
            // anchor)
            //
            if (is_active && is_over) { a->activate(); }
        } else {
            //
            // The parent grouping node is registered for Touch/Drag Sensors.
            //
            grouping_node * const g = node_cast<grouping_node *>(n);
            if (g) {
                g->activate(timestamp, is_over, is_active, point);
                this->modified(true);
            }
        }
    }
}

namespace {
    template <typename T>
    struct UpdatePolledNode_ : std::unary_function<T, void> {
        explicit UpdatePolledNode_(double time): time(time) {}

        void operator()(T node) const { node->update(time); }

    private:
        double time;
    };

    template <typename FieldValue>
    void process_event(event_listener & listener,
                       const field_value & value,
                       const double timestamp)
    {
        assert(dynamic_cast<field_value_listener<FieldValue> *>(&listener));
        assert(dynamic_cast<const FieldValue *>(&value));
        static_cast<field_value_listener<FieldValue> &>(listener)
            .process_event(static_cast<const FieldValue &>(value), timestamp);
    }
}

/**
 * @brief Process events (update the browser).
 *
 * This method should be called after each frame is rendered.
 *
 * @return @c true if the browser needs to be rerendered, @c false otherwise.
 */
bool browser::update(double current_time)
{
    if (current_time <= 0.0) { current_time = browser::current_time(); }

    this->delta_time = DEFAULT_DELTA;

    // Update each of the timers.
    std::list<node *>::iterator i, end = this->timers.end();
    for (i = this->timers.begin(); i != end; ++i) {
        vrml97_node::time_sensor_node * t = (*i)->to_time_sensor();
        if (t) { t->update(current_time); }
    }

    // Update each of the clips.
    end = this->audio_clips.end();
    for (i = this->audio_clips.begin(); i != end; ++i) {
        vrml97_node::audio_clip_node * c = (*i)->to_audio_clip();
        if (c) { c->update(current_time); }
    }

    // Update each of the movies.
    end = this->movies.end();
    for (i = this->movies.begin(); i != end; ++i) {
        vrml97_node::movie_texture_node * m = (*i)->to_movie_texture();
        if (m) { m->update(current_time); }
    }

    //
    // Update each of the scripts.
    //
    std::for_each(this->scripts.begin(), this->scripts.end(),
                  UpdatePolledNode_<script_node *>(current_time));

    // Pass along events to their destinations
    while (this->first_event != this->last_event) {
        event * const e = &this->event_mem[this->first_event];
        this->first_event = (this->first_event + 1) % max_events;

        event_listener & listener = e->to_node->event_listener(e->to_eventin);
        switch (e->value->type()) {
        case field_value::sfbool_id:
            process_event<sfbool>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfcolor_id:
            process_event<sfcolor>(listener, *e->value, e->timestamp);
            break;
        case field_value::sffloat_id:
            process_event<sffloat>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfimage_id:
            process_event<sfimage>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfint32_id:
            process_event<sfint32>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfnode_id:
            process_event<sfnode>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfrotation_id:
            process_event<sfrotation>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfstring_id:
            process_event<sfstring>(listener, *e->value, e->timestamp);
            break;
        case field_value::sftime_id:
            process_event<sftime>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfvec2f_id:
            process_event<sfvec2f>(listener, *e->value, e->timestamp);
            break;
        case field_value::sfvec3f_id:
            process_event<sfvec3f>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfcolor_id:
            process_event<mfcolor>(listener, *e->value, e->timestamp);
            break;
        case field_value::mffloat_id:
            process_event<mffloat>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfint32_id:
            process_event<mfint32>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfnode_id:
            process_event<mfnode>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfrotation_id:
            process_event<mfrotation>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfstring_id:
            process_event<mfstring>(listener, *e->value, e->timestamp);
            break;
        case field_value::mftime_id:
            process_event<mftime>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfvec2f_id:
            process_event<mfvec2f>(listener, *e->value, e->timestamp);
            break;
        case field_value::mfvec3f_id:
            process_event<mfvec3f>(listener, *e->value, e->timestamp);
            break;
        default:
            assert(false);
        }

        delete e->value;
    }

    // Signal a redisplay if necessary
    return this->modified();
}

/**
 * @brief Indicate whether the headlight is on.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool browser::headlight_on()
{
    vrml97_node::navigation_info_node * const navInfo =
        this->bindable_navigation_info_top();
    if (navInfo) { return navInfo->headlight(); }
    return true;
}

namespace {

    struct RenderNodeClass :
            std::unary_function<void, node_class_map_t::value_type> {
        explicit RenderNodeClass(openvrml::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_class_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        openvrml::viewer * viewer;
    };
}

/**
 * @brief Draw this browser into the specified viewer
 */
void browser::render()
{
    if (!this->viewer_) { return; }

    if (this->new_view) {
        this->viewer_->reset_user_navigation();
        this->new_view = false;
    }
    float avatarSize = 0.25;
    float visibilityLimit = 0.0;
    vrml97_node::navigation_info_node * ni =
        this->bindable_navigation_info_top();
    if (ni) {
        avatarSize = ni->avatar_size()[0];
        visibilityLimit = ni->visibility_limit();
    }

    // Activate the headlight.
    // ambient is supposed to be 0 according to the spec...
    if (this->headlight_on()) {
        static const color color(1.0, 1.0, 1.0);
        static const vec3f direction(0.0, 0.0, -1.0);
        static const float ambientIntensity = 0.3f;
        static const float intensity = 1.0;

        this->viewer_->insert_dir_light(ambientIntensity,
                                        intensity,
                                        color,
                                        direction);
    }

    // sets the viewpoint transformation
    //
    const mat4f t = this->active_viewpoint_->user_view_transform()
                    * this->active_viewpoint_->transformation();
    vec3f position, scale;
    rotation orientation;
    t.transformation(position, orientation, scale);
    this->viewer_->set_viewpoint(position,
                                 orientation,
                                 this->active_viewpoint_->field_of_view(),
                                 avatarSize,
                                 visibilityLimit);

    std::for_each(this->node_class_map.begin(), this->node_class_map.end(),
                  RenderNodeClass(*this->viewer_));

    // Top level object

    this->viewer_->begin_object(0);
    mat4f modelview = t.inverse();
    rendering_context rc(bounding_volume::partial, modelview);
    rc.draw_bounding_spheres = true;

    // Do the browser-level lights (Points and Spots)
    std::list<node *>::iterator li, end = this->scoped_lights.end();
    for (li = this->scoped_lights.begin(); li != end; ++li) {
        vrml97_node::abstract_light_node * x = (*li)->to_light();
        if (x) { x->renderScoped(*this->viewer_); }
    }

    //
    // Render the nodes.  scene_ may be 0 if the world failed to load.
    //
    if (this->scene_) {
        this->scene_->render(*this->viewer_, rc);
    }

    this->viewer_->end_object();

    // This is actually one frame late...
    this->frame_rate_ = this->viewer_->frame_rate();

    this->modified(false);

    // If any events were generated during render (ugly...) do an update
    if (this->events_pending()) { this->delta(0.0); }
}

/**
 * @brief Indicate whether rendering is necessary.
 *
 * @param value @c true to indicate that the browser state has changed and
 *              rerendering is necessary; @c false once rendering has occurred.
 */
void browser::modified(const bool value)
{
    this->modified_ = value;
}

/**
 * @brief Check if the browser has been modified.
 *
 * @return @c true if the browser has been modified, @c false otherwise.
 */
bool browser::modified() const
{
    return this->modified_;
}

/**
 * @brief Set the time until the next update is needed.
 *
 * @param d a time interval.
 */
void browser::delta(const double d)
{
    if (d < this->delta_time) { this->delta_time = d; }
}

/**
 * @brief Get the time interval between browser updates.
 *
 * @return the time interval between browser updates.
 */
double browser::delta() const
{
    return this->delta_time;
}

/**
 * @brief Get the top node of a bind_stack_t.
 *
 * @return the top node of @p stack.
 */
const node_ptr browser::bindable_top(const bind_stack_t & stack)
{
    return stack.empty() ? node_ptr(0) : stack.front();
}

/**
 * @brief Push a node onto a bind_stack_t.
 *
 * @param stack the bind_stack_t onto which to push @p node.
 * @param node  the node to push onto @p stack.
 */
void browser::bindable_push(bind_stack_t & stack, const node_ptr & node)
{
    this->bindable_remove(stack, node); // Remove any existing reference
    stack.push_front(node);
    this->modified(true);
}

/**
 * @brief Remove a node from a bind_stack_t.
 *
 * @param stack the bind_stack_t from which to remove @p node.
 * @param node  the node to remove from @p stack.
 */
void browser::bindable_remove(bind_stack_t & stack, const node_ptr & node)
{
    const bind_stack_t::iterator pos =
        std::find(stack.begin(), stack.end(), node);
    if (pos != stack.end()) {
        stack.erase(pos);
        this->modified(true);
    }
}

/**
 * @brief Add a NavigationInfo node to the list of NavigationInfo nodes for the
 *      browser.
 *
 * @param node a NavigationInfo node.
 *
 * @pre @p node is not in the list of NavigationInfo nodes for the browser.
 */
void browser::add_navigation_info(vrml97_node::navigation_info_node & node)
{
    assert(std::find(this->navigation_infos.begin(),
                     this->navigation_infos.end(), &node)
            == this->navigation_infos.end());
    this->navigation_infos.push_back(&node);
}

/**
 * @brief Remove a NavigationInfo node from the list of NavigationInfo nodes
 *      for the browser.
 *
 * @param n  a NavigationInfo node.
 *
 * @pre @p n is in the list of NavigationInfo nodes for the browser.
 */
void browser::remove_navigation_info(vrml97_node::navigation_info_node & n)
{
    assert(!this->navigation_infos.empty());
    const std::list<node *>::iterator end = this->navigation_infos.end();
    const std::list<node *>::iterator pos =
            std::find(this->navigation_infos.begin(), end, &n);
    assert(pos != end);
    this->navigation_infos.erase(pos);
}

/**
 * @brief Get the active node on the bound NavigationInfo stack.
 *
 * @return the active node on the bound NavigationInfo stack.
 */
vrml97_node::navigation_info_node * browser::bindable_navigation_info_top()
{
    node * const n = this->bindable_top(this->navigation_info_stack).get();
    return n ? n->to_navigation_info() : 0;
}

/**
 * @brief Push a NavigationInfo node onto the bound NavigationInfo node stack.
 *
 * @param n a NavigationInfo node.
 */
void browser::bindable_push(vrml97_node::navigation_info_node * n)
{
    this->bindable_push(this->navigation_info_stack, node_ptr(n));
}

/**
 * @brief Remove a NavigationInfo node from the bound NavigationInfo node
 *        stack.
 *
 * @param n a NavigationInfo node.
 */
void browser::bindable_remove(vrml97_node::navigation_info_node * n)
{
    this->bindable_remove(this->navigation_info_stack, node_ptr(n));
}

/**
 * @brief Add a scoped light node to the browser.
 *
 * @param light a light node.
 *
 * @pre @p light is not in the list of light nodes for the browser.
 */
void browser::add_scoped_light(vrml97_node::abstract_light_node & light) {
    assert(std::find(this->scoped_lights.begin(), this->scoped_lights.end(),
                     &light) == this->scoped_lights.end());
    this->scoped_lights.push_back(&light);
}

/**
 * @brief Remove a scoped light node from the browser.
 *
 * @param light the light node to remove.
 *
 * @pre @p light is in the list of light nodes for the browser.
 */
void browser::remove_scoped_light(vrml97_node::abstract_light_node & light)
{
    assert(!this->scoped_lights.empty());
    const std::list<node *>::iterator end = this->scoped_lights.end();
    const std::list<node *>::iterator pos =
            std::find(this->scoped_lights.begin(), end, &light);
    assert(pos != end);
    this->scoped_lights.erase(pos);
}

/**
 * @brief Add a MovieTexture node to the browser.
 *
 * @param movie a MovieTexture node.
 *
 * @pre @p movie is not in the list of MovieTexture nodes for the browser.
 */
void browser::add_movie(vrml97_node::movie_texture_node & movie) {
    assert(std::find(this->movies.begin(), this->movies.end(), &movie)
            == this->movies.end());
    this->movies.push_back(&movie);
}

/**
 * @brief Remove a movie_texture node from the browser.
 *
 * @param movie the movie_texture node to remove.
 *
 * @pre @p movie is in the list of movie_texture nodes for the browser.
 */
void browser::remove_movie(vrml97_node::movie_texture_node & movie)
{
    assert(!this->movies.empty());
    const std::list<node *>::iterator end = this->movies.end();
    const std::list<node *>::iterator pos =
            std::find(this->movies.begin(), end, &movie);
    assert(pos != end);
    this->movies.erase(pos);
}

/**
 * @brief Add a Script node to the browser.
 *
 * @param script    a Script node.
 *
 * @pre @p script is not in the list of Script nodes for the browser.
 */
void browser::add_script(script_node & script) {
    assert(std::find(this->scripts.begin(), this->scripts.end(), &script)
            == this->scripts.end());
    this->scripts.push_back(&script);
}

/**
 * @brief Remove a Script node from the browser.
 *
 * @param script    the Script node to remove.
 *
 * @pre @p script is in the list of Script nodes for the browser.
 */
void browser::remove_script(script_node & script) {
    assert(!this->scripts.empty());
    typedef std::list<script_node *> script_node_list_t;
    const script_node_list_t::iterator end = this->scripts.end();
    const script_node_list_t::iterator pos =
            std::find(this->scripts.begin(), end, &script);
    assert(pos != end);
    this->scripts.erase(pos);
}

/**
 * @brief Add a TimeSensor node to the browser.
 *
 * @param timer a TimeSensor node.
 *
 * @pre @p timer is not in the list of TimeSensor nodes for the browser.
 */
void browser::add_time_sensor(vrml97_node::time_sensor_node & timer)
{
    assert(std::find(this->timers.begin(), this->timers.end(), &timer)
           == this->timers.end());
    this->timers.push_back(&timer);
}

/**
 * @brief Remove a time_sensor node from the browser.
 *
 * @param timer the time_sensor node to remove.
 *
 * @pre @p timer is in the list of time_sensor nodes for the browser.
 */
void browser::remove_time_sensor(vrml97_node::time_sensor_node & timer)
{
    assert(!this->timers.empty());
    const std::list<node *>::iterator end = this->timers.end();
    const std::list<node *>::iterator pos =
            std::find(this->timers.begin(), end, &timer);
    assert(pos != end);
    this->timers.erase(pos);
}


/**
 * @brief Add an AudioClip node to the browser.
 *
 * @param audio_clip    an audio_clip node.
 *
 * @pre @p audio_clip is not in the list of audio_clip nodes for the browser.
 */
void browser::add_audio_clip(vrml97_node::audio_clip_node & audio_clip)
{
    assert(std::find(this->audio_clips.begin(), this->audio_clips.end(),
                     &audio_clip) == this->audio_clips.end());
    this->audio_clips.push_back(&audio_clip);
}

/**
 * @brief Remove an audio_clip node from the browser.
 *
 * @param audio_clip    the audio_clip node to remove.
 *
 * @pre @p audio_clip is in the list of audio_clip nodes for the browser.
 */
void browser::remove_audio_clip(vrml97_node::audio_clip_node & audio_clip)
{
    assert(!this->audio_clips.empty());
    const std::list<node *>::iterator end = this->audio_clips.end();
    const std::list<node *>::iterator pos =
            std::find(this->audio_clips.begin(), end, &audio_clip);
    assert(pos != end);
    this->audio_clips.erase(pos);
}


/**
 * @brief Propagate the bvolume dirty flag from children to ancestors.
 *
 * The invariant is that if a node's bounding volume is out of date,
 * then the bounding volumes of all that nodes's ancestors must be
 * out of date. However, Node does not maintain a parent pointer. So
 * we must do a traversal of the entire browser graph to do the propagation.
 *
 * @see node::setBVolumeDirty
 * @see node::isBVolumeDirty
 */
void browser::update_flags()
{
//  Node* root = this->getRoot();
//  root->updateModified(0x002);
}


/**
 * @class bad_url
 *
 * @brief Thrown when there is a problem resolving a URI.
 */

/**
 * @brief Construct.
 *
 * @param message   Informative text.
 */
bad_url::bad_url(const std::string & message):
    std::runtime_error(message)
{}

/**
 * @brief Destroy.
 */
bad_url::~bad_url() throw ()
{}


/**
 * @class invalid_url
 *
 * @brief Thrown when parsing a URI fails.
 */

/**
 * @brief Construct.
 */
invalid_url::invalid_url():
    bad_url("Invalid URI.")
{}

/**
 * @brief Destroy.
 */
invalid_url::~invalid_url() throw ()
{}


/**
 * @class unreachable_url
 *
 * @brief Thrown when a URI cannot be reached.
 */

/**
 * @brief Construct.
 */
unreachable_url::unreachable_url():
    bad_url("Unreachable URI.")
{}

/**
 * @brief Destroy.
 */
unreachable_url::~unreachable_url() throw ()
{}


/**
 * @class no_alternative_url
 *
 * @brief Exception thrown when no URI in an alternative URI list can be
 *        resolved.
 */

/**
 * @brief Construct.
 */
no_alternative_url::no_alternative_url():
    bad_url("No alternative URI could be resolved.")
{}

/**
 * @brief Destroy.
 */
no_alternative_url::~no_alternative_url() throw ()
{}


/**
 * @class scene
 *
 * @brief A scene in the VRML world.
 */

/**
 * @var mfnode scene::nodes_
 *
 * @brief The nodes for the scene.
 */

/**
 * @var const std::string scene::url_
 *
 * @brief The URI for the scene.
 *
 * @a uri may be a relative or an absolute reference.
 */

/**
 * @var browser & scene::browser
 *
 * @brief A reference to the browser associated with the scene.
 */

/**
 * @var scene * const scene::parent
 *
 * @brief A pointer to the parent scene.
 *
 * If the scene is the root scene, @a parent will be 0.
 */

namespace {

# ifdef _WIN32
    struct IsBackslash : std::unary_function<char, bool> {
        bool operator()(const char c) const throw ()
        {
            return c == '\\';
        }
    };
# endif

    const URI createFileURL(const URI & uri) throw ()
    {
        assert(uri.getScheme().empty());

        using std::string;

        string result = "file://";

# ifdef _WIN32
        //
        // _fullpath returns a string starting with the drive letter; for the
        // URL, the path must begin with a '/'. So we simply put one at the
        // beginning of the buffer.
        //
        char buffer[_MAX_PATH] = { '/' };
        char * resolvedPath =
                _fullpath(buffer + 1, uri.getPath().c_str(), _MAX_PATH);
        if (!resolvedPath) {
            //
            // XXX Failed; need to check errno to see what we should throw.
            //
            return result;
        }
        std::replace_if(resolvedPath, resolvedPath + strlen(resolvedPath) + 1,
                        IsBackslash(), '/');
        --resolvedPath;
        assert(resolvedPath == buffer);
# else
        char buffer[PATH_MAX];
        const char * resolvedPath = realpath(uri.getPath().c_str(), buffer);
        if (!resolvedPath) {
            //
            // XXX Failed; need to check errno to see what we should throw.
            //
            return result;
        }
# endif

        result += resolvedPath;

        string query = uri.getQuery();
        if (!query.empty()) { result += '?' + query; }

        string fragment = uri.getFragment();
        if (!fragment.empty()) { result += '#' + fragment; }

        return result;
    }
}

/**
 * @brief Construct a scene from a URI.
 *
 * @param browser   the browser associated with the scene.
 * @param url       the URI for the scene.
 * @param parent    the parent scene.
 *
 * @exception invalid_vrml      if there is a syntax error in the VRML input.
 * @exception std::bad_alloc    if memory allocation fails.
 */
scene::scene(openvrml::browser & browser,
             const std::vector<std::string> & url,
             scene * parent)
    throw (invalid_vrml, no_alternative_url, std::bad_alloc):
    browser(browser),
    parent(parent)
{
    using std::istream;
    using std::string;
    using std::vector;

    string absoluteURI;
    for (vector<string>::size_type i = 0; i < url.size(); ++i) {
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            URI testURI(url[i]);

            const bool absolute = !testURI.getScheme().empty();
            if (absolute) {
                absoluteURI = testURI;
            } else if (!parent) {
                //
                // If we have a relative reference and the parent is null, then
                // assume the reference is to the local file system: convert
                // the relative reference to a file URL.
                //
                absoluteURI = createFileURL(testURI);
            } else {
                //
                // If we have a relative URI and parent is not null, try to
                // resolve the relative reference against the parent's URI.
                //
                absoluteURI = testURI.resolveAgainst(URI(parent->url()));
            }

            doc2 doc(absoluteURI);
            istream & in = doc.input_stream();
            if (!in) { throw unreachable_url(); }
            try {
                Vrml97Scanner scanner(in);
                Vrml97Parser parser(scanner, absoluteURI);
                parser.vrmlScene(browser, this->nodes_);
            } catch (antlr::RecognitionException & ex) {
                throw invalid_vrml(ex.getFilename(),
                                   ex.getLine(),
                                   ex.getColumn(),
                                   ex.getMessage());
            } catch (antlr::ANTLRException & ex) {
                browser.err << ex.getMessage() << std::endl;
            } catch (std::bad_alloc &) {
                throw;
            } catch (...) {
                throw unreachable_url();
            }
        } catch (bad_url & ex) {
            browser.err << ex.what() << std::endl;
            continue;
        }
        //
        // If this is the root scene (that is, the parent is null), then
        // this->uri must be the absolute URI.
        //
        this->url_ = parent
            ? url[i]
            : absoluteURI;
        break;
    }
    if (this->url_.empty()) { throw no_alternative_url(); }
}

/**
 * @brief Initialize the scene.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void scene::initialize(const double timestamp) throw (std::bad_alloc)
{
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        (*node)->initialize(*this, timestamp);
        child_node * const child = node_cast<child_node *>(node->get());
        assert(child);
        child->relocate();
    }
}

/**
 * @fn const std::vector<node_ptr> & scene::nodes() const throw ()
 *
 * @brief Root nodes for the scene.
 *
 * @return the root nodes for the scene.
 */

/**
 * @brief Get the absolute URI for the scene.
 *
 * @return the absolute URI for the scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string scene::url() const throw (std::bad_alloc)
{
    using std::string;
    return this->parent
            ? string(URI(this->url_).resolveAgainst(URI(this->parent->url())))
            : this->url_;
}

/**
 * @brief Render the scene.
 *
 * @param viewer    a Viewer to render to.
 * @param context   a rendering_context.
 */
void scene::render(openvrml::viewer & viewer, rendering_context context)
{
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        child_node * child = node_cast<child_node *>(node->get());
        if (child) { child->render_child(viewer, context); }
    }
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * browser::load_url.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form "#NodeId" and it is not
 *         the first URI in the list, this URI will be loaded as if it were a
 *         new world rather than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form "#NodeId" and no
 *         Viewpoint named "NodeId" exists in the scene, this method will not
 *         try any subsequent URIs in the list.
 *
 * @param url       an array of URIs. Per VRML97 convention, the first resource
 *                  in the sequence that can be reached will be loaded into the
 *                  browser.
 * @param parameter an array of parameters to be associated with the URIs in
 *                  @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p url is
 *      invalid. Should this throw invalid_url?
 */
void scene::load_url(const std::vector<std::string> & url,
                     const std::vector<std::string> & parameter)
    throw (std::bad_alloc)
{
    using std::string;

    if (!url.empty()) {
        if (url[0][0] == '#') {
# if 0
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser.setViewpoint(uri[0].substr(1));
# endif
        } else {
            std::vector<std::string> absoluteURIs(url.size());
            for (size_t i = 0; i < absoluteURIs.size(); ++i) {
                try {
                    const URI urlElement(url[i]);
                    const string value =
                        urlElement.getScheme().empty()
                            ? urlElement.resolveAgainst(URI(this->url()))
                            : urlElement;
                    absoluteURIs[i] = value;
                } catch (invalid_url & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser.load_url(absoluteURIs, parameter);
        }
    }
}

/**
 * @brief Shut down the nodes in the scene.
 *
 * This function @b must be called before the scene is destroyed.
 *
 * @param timestamp the current time.
 */
void scene::shutdown(const double timestamp) throw ()
{
    for (std::vector<node_ptr>::iterator node(this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        if (*node) { (*node)->shutdown(timestamp); }
    }
}


/**
 * @internal
 *
 * @class Vrml97RootScope
 *
 * @brief Root namespace for VRML97 browsers.
 *
 * Vrml97RootScope is initialized with the VRML97 node types.
 */

namespace {
    /**
     * @internal
     */
    class vrml97_node_interface_set_ : public node_interface_set {
    public:
        vrml97_node_interface_set_(const node_interface * const begin,
                                   const node_interface * const end)
        {
            this->insert(begin, end);
        }
    };
}

/**
 * @brief Constructor.
 *
 * @param browser   the browser object.
 * @param uri       the URI associated with the stream being read.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
Vrml97RootScope::Vrml97RootScope(const browser & browser,
                                 const std::string & uri)
    throw (std::bad_alloc):
    scope(uri)
{
    const browser::node_class_map_t & nodeClassMap = browser.node_class_map;
    browser::node_class_map_t::const_iterator pos;

    try {
        //
        // Anchor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "parameter"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:Anchor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Anchor", interface_set));
        }

        //
        // Appearance node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "material"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texture"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "textureTransform")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 3);
            pos = nodeClassMap.find("urn:X-openvrml:node:Appearance");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Appearance",
                                                    interface_set));
        }

        //
        // AudioClip node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "pitch"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "duration_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:AudioClip");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("AudioClip",
                                                    interface_set));
        }

        //
        // Background node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "groundAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "groundColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "backUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "bottomUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "frontUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "leftUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "rightUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "topUrl"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "skyAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "skyColor"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 12);
            pos = nodeClassMap.find("urn:X-openvrml:node:Background");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Background",
                                                    interface_set));
        }

        //
        // Billboard node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "axisOfRotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 6);
            pos = nodeClassMap.find("urn:X-openvrml:node:Billboard");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Billboard",
                                                    interface_set));
        }

        //
        // Box node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "size");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:Box");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Box", interface_set));
        }

        //
        // Collision node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "collide"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize"),
                node_interface(node_interface::field_id,
                               field_value::sfnode_id,
                               "proxy"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "collideTime")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:Collision");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Collision",
                                                    interface_set));
        }

        //
        // Color node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "color");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:Color");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Color", interface_set));
        }

        //
        // ColorInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfcolor_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfcolor_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:ColorInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("ColorInterpolator",
                                                    interface_set));
        }

        //
        // Cone node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "bottomRadius"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "side"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "bottom")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:Cone");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Cone", interface_set));
        }

        //
        // Coordinate node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "point");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:Coordinate");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Coordinate",
                                                    interface_set));
        }

        //
        // CoordinateInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::mfvec3f_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap
                .find("urn:X-openvrml:node:CoordinateInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("CoordinateInterpolator",
                                                    interface_set));
        }

        //
        // Cylinder node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "bottom"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "radius"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "side"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "top")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 5);
            pos = nodeClassMap.find("urn:X-openvrml:node:Cylinder");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Cylinder",
                                                    interface_set));
        }

        //
        // CylinderSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "diskAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "rotation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 9);
            pos = nodeClassMap.find("urn:X-openvrml:node:CylinderSensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("CylinderSensor",
                                                    interface_set));
        }

        //
        // DirectionalLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "direction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "on")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 5);
            pos = nodeClassMap.find("urn:X-openvrml:node:DirectionalLight");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("DirectionalLight",
                                                    interface_set));
        }

        //
        // ElevationGrid node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mffloat_id,
                               "set_height"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "normal"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texCoord"),
                node_interface(node_interface::field_id,
                               field_value::mffloat_id,
                               "height"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "normalPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::sfint32_id,
                               "xDimension"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "xSpacing"),
                node_interface(node_interface::field_id,
                               field_value::sfint32_id,
                               "zDimension"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "zSpacing")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 14);
            pos = nodeClassMap.find("urn:X-openvrml:node:ElevationGrid");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("ElevationGrid",
                                                    interface_set));
        }

        //
        // Extrusion node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfvec2f_id,
                               "set_crossSection"),
                node_interface(node_interface::eventin_id,
                               field_value::mfrotation_id,
                               "set_orientation"),
                node_interface(node_interface::eventin_id,
                               field_value::mfvec2f_id,
                               "set_scale"),
                node_interface(node_interface::eventin_id,
                               field_value::mfvec3f_id,
                               "set_spine"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "beginCap"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "convex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::mfvec2f_id,
                               "crossSection"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "endCap"),
                node_interface(node_interface::field_id,
                               field_value::mfrotation_id,
                               "orientation"),
                node_interface(node_interface::field_id,
                               field_value::mfvec2f_id,
                               "scale"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::mfvec3f_id,
                               "spine")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 14);
            pos = nodeClassMap.find("urn:X-openvrml:node:Extrusion");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Extrusion",
                                                    interface_set));
        }

        //
        // Fog node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfstring_id,
                               "fogType"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "visibilityRange"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 5);
            pos = nodeClassMap.find("urn:X-openvrml:node:Fog");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Fog", interface_set));
        }

        //
        // FontStyle node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "family"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "horizontal"),
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "justify"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "language"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "leftToRight"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "size"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "spacing"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "style"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "topToBottom")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 9);
            pos = nodeClassMap.find("urn:X-openvrml:node:FontStyle");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("FontStyle",
                                                    interface_set));
        }

        //
        // Group node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 5);
            pos = nodeClassMap.find("urn:X-openvrml:node:Group");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Group",
                                                    interface_set));
        }

        //
        // ImageTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 3);
            pos = nodeClassMap.find("urn:X-openvrml:node:ImageTexture");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("ImageTexture",
                                                    interface_set));
        }

        //
        // IndexedFaceSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_colorIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_coordIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_normalIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_texCoordIndex"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "normal"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "texCoord"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "ccw"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "colorIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "convex"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "coordIndex"),
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "creaseAngle"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "normalIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "normalPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "solid"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "texCoordIndex")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 18);
            pos = nodeClassMap.find("urn:X-openvrml:node:IndexedFaceSet");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("IndexedFaceSet",
                                                    interface_set));
        }

        //
        // IndexedLineSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_colorIndex"),
                node_interface(node_interface::eventin_id,
                               field_value::mfint32_id,
                               "set_coordIndex"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "colorIndex"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "colorPerVertex"),
                node_interface(node_interface::field_id,
                               field_value::mfint32_id,
                               "coordIndex")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 7);
            pos = nodeClassMap.find("urn:X-openvrml:node:IndexedLineSet");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("IndexedLineSet",
                                                    interface_set));
        }

        //
        // Inline node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 3);
            pos = nodeClassMap.find("urn:X-openvrml:node:Inline");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Inline", interface_set));
        }

        //
        // LOD node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "level"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::field_id,
                               field_value::mffloat_id,
                               "range")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 3);
            pos = nodeClassMap.find("urn:X-openvrml:node:LOD");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("LOD", interface_set));
        }

        //
        // Material node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "diffuseColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "emissiveColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "shininess"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "specularColor"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "transparency")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 6);
            pos = nodeClassMap.find("urn:X-openvrml:node:Material");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Material",
                                                    interface_set));
        }

        //
        // MovieTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "speed"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "url"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "duration_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 9);
            pos = nodeClassMap.find("urn:X-openvrml:node:MovieTexture");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("MovieTexture",
                                                    interface_set));
        }

        //
        // NavigationInfo node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "avatarSize"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "headlight"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "speed"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "type"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "visibilityLimit"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 7);
            pos = nodeClassMap.find("urn:X-openvrml:node:NavigationInfo");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("NavigationInfo",
                                                    interface_set));
        }

        //
        // Normal node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "vector");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:Normal");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Normal",
                                                    interface_set));
        }

        //
        // NormalInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::mfvec3f_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:NormalInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("NormalInterpolator",
                                                    interface_set));
        }

        //
        // OrientationInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfrotation_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap
                .find("urn:X-openvrml:node:OrientationInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("OrientationInterpolator",
                                                    interface_set));
        }

        //
        // PixelTexture node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfimage_id,
                               "image"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatS"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "repeatT")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 3);
            pos = nodeClassMap.find("urn:X-openvrml:node:PixelTexture");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("PixelTexture",
                                                    interface_set));
        }

        //
        // PlaneSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "maxPosition"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "minPosition"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "translation_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:PlaneSensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("PlaneSensor",
                                                    interface_set));
        }

        //
        // PointLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "attenuation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "on"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "radius")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 7);
            pos = nodeClassMap.find("urn:X-openvrml:node:PointLight");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("PointLight",
                                                    interface_set));
        }

        //
        // PointSet node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "coord")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 2);
            pos = nodeClassMap.find("urn:X-openvrml:node:PointSet");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("PointSet",
                                                    interface_set));
        }

        //
        // PositionInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec3f_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap
                .find("urn:X-openvrml:node:PositionInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("PositionInterpolator",
                                                    interface_set));
        }

        //
        // ProximitySensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "size"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "position_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "orientation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "enterTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "exitTime")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:ProximitySensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("ProximitySensor",
                                                    interface_set));
        }

        //
        // ScalarInterpolator node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sffloat_id,
                               "set_fraction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "key"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "keyValue"),
                node_interface(node_interface::eventout_id,
                               field_value::sffloat_id,
                               "value_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:ScalarInterpolator");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("ScalarInterpolator",
                                                    interface_set));
        }

        //
        // Shape node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "appearance"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "geometry")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 2);
            pos = nodeClassMap.find("urn:X-openvrml:node:Shape");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Shape", interface_set));
        }

        //
        // Sound node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "direction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxBack"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxFront"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minBack"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "minFront"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "priority"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "source"),
                node_interface(node_interface::field_id,
                               field_value::sfbool_id,
                               "spatialize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 10);
            pos = nodeClassMap.find("urn:X-openvrml:node:Sound");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Sound", interface_set));
        }

        //
        // Sphere node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::field_id,
                               field_value::sffloat_id,
                               "radius");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:Sphere");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Sphere", interface_set));
        }

        //
        // SphereSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "autoOffset"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "offset"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfrotation_id,
                               "rotation_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "trackPoint_changed")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 6);
            pos = nodeClassMap.find("urn:X-openvrml:node:SphereSensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("SphereSensor",
                                                    interface_set));
        }

        //
        // SpotLight node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "ambientIntensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "attenuation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "beamWidth"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfcolor_id,
                               "color"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "cutOffAngle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "direction"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "intensity"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "location"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "on"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "radius")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 10);
            pos = nodeClassMap.find("urn:X-openvrml:node:SpotLight");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("SpotLight",
                                                    interface_set));
        }

        //
        // Switch node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "choice"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfint32_id,
                               "whichChoice")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 2);
            pos = nodeClassMap.find("urn:X-openvrml:node:Switch");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Switch", interface_set));
        }

        //
        // Text node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::mfstring_id,
                               "string"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfnode_id,
                               "fontStyle"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mffloat_id,
                               "length"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "maxExtent")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:Text");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Text", interface_set));
        }

        //
        // TextureCoordinate node
        //
        {
            static const node_interface interface =
                node_interface(node_interface::exposedfield_id,
                               field_value::mfvec2f_id,
                               "point");
            static const vrml97_node_interface_set_
                interface_set(&interface, &interface + 1);
            pos = nodeClassMap.find("urn:X-openvrml:node:TextureCoordinate");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("TextureCoordinate",
                                                    interface_set));
        }

        //
        // TextureTransform node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "rotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "scale"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec2f_id,
                               "translation")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 4);
            pos = nodeClassMap.find("urn:X-openvrml:node:TextureTransform");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("TextureTransform",
                                                    interface_set));
        }

        //
        // TimeSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "cycleInterval"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "loop"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "startTime"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sftime_id,
                               "stopTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "cycleTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sffloat_id,
                               "fraction_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "time")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 9);
            pos = nodeClassMap.find("urn:X-openvrml:node:TimeSensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("TimeSensor",
                                                    interface_set));
        }

        //
        // TouchSensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "hitNormal_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec3f_id,
                               "hitPoint_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfvec2f_id,
                               "hitTexCoord_changed"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isOver"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "touchTime")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 7);
            pos = nodeClassMap.find("urn:X-openvrml:node:TouchSensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("TouchSensor",
                                                    interface_set));
        }

        //
        // Transform node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "addChildren"),
                node_interface(node_interface::eventin_id,
                               field_value::mfnode_id,
                               "removeChildren"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::mfnode_id,
                               "children"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "rotation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "scale"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "scaleOrientation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "translation"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxCenter"),
                node_interface(node_interface::field_id,
                               field_value::sfvec3f_id,
                               "bboxSize")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 10);
            pos = nodeClassMap.find("urn:X-openvrml:node:Transform");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Transform",
                                                    interface_set));
        }

        //
        // Viewpoint node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::eventin_id,
                               field_value::sfbool_id,
                               "set_bind"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sffloat_id,
                               "fieldOfView"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "jump"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfrotation_id,
                               "orientation"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "position"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "description"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "bindTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isBound")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 8);
            pos = nodeClassMap.find("urn:X-openvrml:node:Viewpoint");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("Viewpoint",
                                                    interface_set));
        }

        //
        // VisibilitySensor node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "center"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfbool_id,
                               "enabled"),
                node_interface(node_interface::exposedfield_id,
                               field_value::sfvec3f_id,
                               "size"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "enterTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sftime_id,
                               "exitTime"),
                node_interface(node_interface::eventout_id,
                               field_value::sfbool_id,
                               "isActive")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 6);
            pos = nodeClassMap.find("urn:X-openvrml:node:VisibilitySensor");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("VisibilitySensor",
                                                    interface_set));
        }

        //
        // WorldInfo node
        //
        {
            static const node_interface interfaces[] = {
                node_interface(node_interface::field_id,
                               field_value::mfstring_id,
                               "info"),
                node_interface(node_interface::field_id,
                               field_value::sfstring_id,
                               "title")
            };
            static const vrml97_node_interface_set_
                interface_set(interfaces, interfaces + 2);
            pos = nodeClassMap.find("urn:X-openvrml:node:WorldInfo");
            assert(pos != nodeClassMap.end());
            this->add_type(pos->second->create_type("WorldInfo",
                                                    interface_set));
        }
    } catch (std::invalid_argument & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

Vrml97RootScope::~Vrml97RootScope() throw () {}


null_node_class::null_node_class(openvrml::browser & browser) throw ():
    node_class(browser)
{}

null_node_class::~null_node_class() throw ()
{}

const node_type_ptr
null_node_class::create_type(const std::string & id,
                             const node_interface_set & interfaces)
    throw ()
{
    assert(false);
    static const node_type_ptr nodeType;
    return nodeType;
}


null_node_type::null_node_type(null_node_class & nodeClass) throw ():
    node_type(nodeClass, std::string())
{}

null_node_type::~null_node_type() throw ()
{}

const node_interface_set & null_node_type::interfaces() const throw ()
{
    assert(false);
    static const node_interface_set interfaces;
    return interfaces;
}

const node_ptr
null_node_type::create_node(const scope_ptr & scope,
                            const initial_value_map & initial_values) const
    throw ()
{
    assert(false);
    static const node_ptr node;
    return node;
}


/**
 * @brief Constructor.
 *
 * @param nodeType  the browser's null_node_type instance.
 */
DefaultViewpoint::DefaultViewpoint(const null_node_type & nodeType) throw ():
    node(nodeType, scope_ptr()),
    child_node(nodeType, scope_ptr()),
    viewpoint_node(nodeType, scope_ptr())
{}

/**
 * @brief Destructor.
 */
DefaultViewpoint::~DefaultViewpoint() throw ()
{}

const mat4f & DefaultViewpoint::transformation() const throw ()
{
    static const vec3f position(0.0, 0.0, 10.0);
    static const rotation orientation;
    static const vec3f scale(1.0, 1.0, 1.0);
    static const rotation scaleOrientation;
    static const vec3f center;
    static const mat4f t(mat4f::transformation(position,
                                               orientation,
                                               scale,
                                               scaleOrientation,
                                               center));
    return t;
}

const mat4f & DefaultViewpoint::user_view_transform() const throw ()
{
    return this->userViewTransform;
}

void DefaultViewpoint::user_view_transform(const mat4f & transform)
    throw ()
{
    this->userViewTransform = transform;
}

const std::string & DefaultViewpoint::description() const throw ()
{
    static const std::string desc;
    return desc;
}

float DefaultViewpoint::field_of_view() const throw ()
{
    static const float fieldOfView = 0.785398f;
    return fieldOfView;
}

void DefaultViewpoint::do_field(const std::string & id,
                                const field_value & value)
    throw ()
{
    assert(false);
}

const field_value & DefaultViewpoint::do_field(const std::string & id) const
    throw ()
{
    assert(false);
    static const sfbool value;
    return value;
}

void DefaultViewpoint::do_process_event(const std::string & id,
                                        const field_value & value,
                                        double timestamp)
    throw ()
{
    assert(false);
}

const field_value &
DefaultViewpoint::do_eventout(const std::string & id) const throw ()
{
    assert(false);
    static const sfbool value;
    return value;
}

event_listener & DefaultViewpoint::do_event_listener(const std::string & id)
    throw (unsupported_interface)
{
    assert(false);
    throw unsupported_interface(this->type, id);
    return *static_cast<openvrml::event_listener *>(0);
}

event_emitter &
DefaultViewpoint::do_event_emitter(const std::string & id)
    throw (unsupported_interface)
{
    assert(false);
    throw unsupported_interface(this->type, id);
    return *static_cast<openvrml::event_emitter *>(0);
}


namespace {

    const char * const expression =
            "^(([^:/?#]+):)?((//([^/?#]*))?([^?#]*)([?]([^#]*))?(#(.*))?)";
    //        |+- scheme    ||  |          |       |   |        | +- fragment-id
    //        +- scheme ':' ||  |          |       |   |        +- '#' fragment-id
    //                      ||  |          |       |   +- query
    //                      ||  |          |       +- '?' query
    //                      ||  |          +- path
    //                      ||  +- authority
    //                      |+- "//" authority
    //                      +- scheme-specific-part

    class URIRegex {
        regex_t regex;

    public:
        URIRegex() throw (std::bad_alloc);
        ~URIRegex() throw ();

        int exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                 int eflags);
    };

    URIRegex::URIRegex() throw (std::bad_alloc) {
        int err = regcomp(&this->regex, expression, REG_EXTENDED);
        if (err == REG_ESPACE) { throw std::bad_alloc(); }
        assert(err == 0);
    }

    URIRegex::~URIRegex() throw () { regfree(&this->regex); }

    int URIRegex::exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                       int eflags) {
        return regexec(&this->regex, str, nmatch, pmatch, eflags);
    }

    URIRegex uriRegex;

    URI::URI(const std::string & str) throw (invalid_url, std::bad_alloc):
            str(str) {
        int err = uriRegex.exec(str.c_str(), URI::nmatch, this->regmatch, 0);
        if (err != 0) { throw invalid_url(); }
    }

    URI::operator std::string() const throw (std::bad_alloc) {
        return this->str;
    }

    const std::string URI::getScheme() const throw (std::bad_alloc) {
        return (this->regmatch[2].rm_so > -1)
                ? this->str.substr(this->regmatch[2].rm_so,
                                   this->regmatch[2].rm_eo - this->regmatch[2].rm_so)
                : std::string();
    }

    const std::string URI::getSchemeSpecificPart() const
            throw (std::bad_alloc) {
        return (this->regmatch[3].rm_so > -1)
                ? this->str.substr(this->regmatch[3].rm_so,
                                   this->regmatch[3].rm_eo - this->regmatch[3].rm_so)
                : std::string();
    }

    const std::string URI::getAuthority() const throw (std::bad_alloc) {
        return (this->regmatch[5].rm_so > -1)
                ? this->str.substr(this->regmatch[5].rm_so,
                                   this->regmatch[5].rm_eo - this->regmatch[5].rm_so)
                : std::string();
    }

    const std::string URI::getUserinfo() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getHost() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPort() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPath() const throw (std::bad_alloc) {
        return (this->regmatch[6].rm_so > -1)
                ? this->str.substr(this->regmatch[6].rm_so,
                                   this->regmatch[6].rm_eo - this->regmatch[6].rm_so)
                : std::string();
    }

    const std::string URI::getQuery() const throw (std::bad_alloc) {
        return (this->regmatch[7].rm_so > -1)
                ? this->str.substr(this->regmatch[7].rm_so,
                                   this->regmatch[7].rm_eo - this->regmatch[7].rm_so)
                : std::string();
    }

    const std::string URI::getFragment() const throw (std::bad_alloc) {
        return (this->regmatch[10].rm_so > -1)
                ? this->str.substr(this->regmatch[10].rm_so,
                                   this->regmatch[10].rm_eo - this->regmatch[10].rm_so)
                : std::string();
    }

    const URI URI::resolveAgainst(const URI & absoluteURI) const
            throw (std::bad_alloc) {
        using std::list;
        using std::string;

        assert(this->getScheme().empty());
        assert(!absoluteURI.getScheme().empty());

        string result = absoluteURI.getScheme() + ':';

        if (!this->getAuthority().empty()) {
            return result + this->getSchemeSpecificPart();
        } else {
            result += "//" + absoluteURI.getAuthority();
        }

        string path = absoluteURI.getPath();
        const string::size_type lastSlashIndex = path.find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        path = path.substr(1, lastSlashIndex);

        //
        // Append the relative path.
        //
        path += this->getPath();

        //
        // Put the path segments in a list to process them.
        //
        list<string> pathSegments;
        string::size_type slashIndex = 0;
        string::size_type segmentStartIndex = 0;
        do {
            slashIndex = path.find('/', segmentStartIndex);
            string segment = path.substr(segmentStartIndex,
                                         slashIndex - segmentStartIndex);
            if (!segment.empty()) {
                pathSegments.push_back(segment);
            }
            segmentStartIndex = slashIndex + 1;
        } while (slashIndex != string::npos);

        //
        // Remove any "." segments.
        //
        pathSegments.remove(".");

        //
        // Remove any ".." segments along with the segment that precedes them.
        //
        const list<string>::iterator begin(pathSegments.begin());
        list<string>::iterator pos;
        for (pos = begin; pos != pathSegments.end(); ++pos) {
            if (pos != begin && *pos == "..") {
                --(pos = pathSegments.erase(pos));
                --(pos = pathSegments.erase(pos));
            }
        }

        //
        // Reconstruct the path.
        //
        path = string();
        for (pos = pathSegments.begin(); pos != pathSegments.end(); ++pos) {
            path += '/' + *pos;
        }

        //
        // End in a slash?
        //
        if (*(this->getPath().end() - 1) == '/') { path += '/'; }

        result += path;

        const string query = this->getQuery();
        if (!query.empty()) { result += '?' + query; }

        const string fragment = this->getFragment();
        if (!fragment.empty()) { result += '#' + fragment; }

        return result;
    }

} // namespace

} // namespace openvrml
