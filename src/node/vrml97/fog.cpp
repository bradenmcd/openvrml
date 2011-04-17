// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# include "fog.h"
# include <private.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

/**
 * @var openvrml_node_vrml97::fog_node * openvrml_node_vrml97::fog_metatype::first
 *
 * @brief The first Fog node in the initial scene graph.
 */

/**
 * @var openvrml::node_impl_util::bound_node_stack<openvrml_node_vrml97::fog_node> openvrml_node_vrml97::fog_metatype::bound_nodes
 *
 * @brief The bound Fog node stack.
 */

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::fog_metatype::id =
    "urn:X-openvrml:node:Fog";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml_node_vrml97::fog_metatype::fog_metatype(openvrml::browser & browser):
    node_metatype(fog_metatype::id, browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::fog_metatype::~fog_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Set the first Fog node in the world.
 *
 * The first Fog node in the world is used for the initial fog.
 * This method is used by fog_node::do_initialize.
 *
 * @param fog   a Fog node.
 */
void openvrml_node_vrml97::fog_metatype::set_first(fog_node & fog)
    OPENVRML_NOTHROW
{
    this->first = &fog;
}

/**
 * @brief Reset the pointer to the first Fog node in the world to null.
 *
 * This function is called by <code>fog_node::do_shutdown</code>.
 */
void openvrml_node_vrml97::fog_metatype::reset_first() OPENVRML_NOTHROW
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by fog_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false
 *         otherwise.
 */
bool openvrml_node_vrml97::fog_metatype::has_first() const OPENVRML_NOTHROW
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param fog   a fog_node.
 *
 * @return @c true if @p fog is the fist node; @c false otherwise.
 */
bool openvrml_node_vrml97::fog_metatype::is_first(fog_node & fog)
    OPENVRML_NOTHROW
{
    return &fog == this->first;
}

/**
 * @brief Push a Fog on the top of the bound node stack.
 *
 * @param fog       the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml_node_vrml97::fog_metatype::bind(fog_node & fog,
                                              const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.bind(fog, timestamp);
}

/**
 * @brief Remove a Fog from the bound node stack.
 *
 * @param fog       the node to unbind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml_node_vrml97::fog_metatype::unbind(fog_node & fog,
                                                const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.unbind(fog, timestamp);
}

/**
 * @brief node_metatype-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound
 *                          initially.
 * @param timestamp         the current time.
 */
void
openvrml_node_vrml97::fog_metatype::do_initialize(openvrml::viewpoint_node *,
                                                  const double timestamp)
    OPENVRML_NOTHROW
{
    using namespace openvrml;

    if (this->first) {
        try {
            event_listener & listener =
                this->first->event_listener("set_bind");
            dynamic_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), timestamp);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
}

/**
 * @brief node_metatype-specific rendering.
 *
 * Render the active Fog node.
 *
 * @param v a viewer.
 */
void openvrml_node_vrml97::fog_metatype::do_render(openvrml::viewer & v) const
    OPENVRML_NOTHROW
{
    if (!this->bound_nodes.empty()) {
        fog_node & fog =
            dynamic_cast<fog_node &>(*this->bound_nodes.top());
        v.set_fog(fog.color_.sfcolor::value(),
                  fog.visibility_range_.sffloat::value(),
                  fog.fog_type_.sfstring::value().c_str());
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Fog nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by fog_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::fog_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 7> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
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
                       "isBound"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime")
    };

    typedef node_impl_util::node_type_impl<fog_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & fogNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            fogNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::set_bind_listener_);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::color_);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::fog_type_);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::visibility_range_);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::is_bound_emitter_);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            fogNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &fog_node::bind_time_emitter_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}

/**
 * @class openvrml_node_vrml97::fog_node
 *
 * @brief Fog node instances.
 */

/**
 * @var class openvrml_node_vrml97::fog_node::fog_metatype
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @internal
 *
 * @class openvrml_node_vrml97::fog_node::set_bind_listener
 *
 * @brief set_bind event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  fog_node.
 */
openvrml_node_vrml97::fog_node::set_bind_listener::
set_bind_listener(fog_node & node):
    openvrml::node_event_listener(node),
    openvrml::node_impl_util::event_listener_base<fog_node>(node),
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::fog_node::set_bind_listener::~set_bind_listener()
    OPENVRML_NOTHROW
{}

/**
 * @brief Process event.
 *
 * @param bind      bind state.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::fog_node::set_bind_listener::
do_process_event(const openvrml::sfbool & bind, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    try {
        fog_node & fog = dynamic_cast<fog_node &>(this->node());
        fog_metatype & node_metatype =
            const_cast<fog_metatype &>(
                static_cast<const fog_metatype &>(fog.type().metatype()));
        if (bind.value()) {
            node_metatype.bind(fog, timestamp);
        } else {
            node_metatype.unbind(fog, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml_node_vrml97::fog_node::set_bind_listener openvrml_node_vrml97::fog_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::fog_node>::exposedfield<openvrml::sfcolor> openvrml_node_vrml97::fog_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::fog_node>::exposedfield<openvrml::sfstring> openvrml_node_vrml97::fog_node::fog_type_
 *
 * @brief fogType exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::fog_node>::exposedfield<openvrml::sffloat> openvrml_node_vrml97::fog_node::visibility_range_
 *
 * @brief visibilityRange exposedField.
 */

/**
 * @var openvrml::sfbool openvrml_node_vrml97::fog_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool openvrml_node_vrml97::fog_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml_node_vrml97::fog_node::
fog_node(const openvrml::node_type & type,
         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    openvrml::node_impl_util::abstract_node<fog_node>(type, scope),
    child_node(type, scope),
    set_bind_listener_(*this),
    color_(*this, openvrml::make_color(1.0, 1.0, 1.0)),
    fog_type_(*this, "LINEAR"),
    visibility_range_(*this, 0.0),
    is_bound_emitter_(*this, this->is_bound_),
    bind_time_emitter_(*this, this->bind_time_)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::fog_node::~fog_node() OPENVRML_NOTHROW
{}

/**
 * @brief Set the bind state.
 *
 * This function is called by
 * <code>bound_node_stack<BindableNode>::bind</code>.  It sets the
 * "bind" state and fires an event; it does not actually put the
 * node onto the bound node stack.
 *
 * @param[in] val       the bind state.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml_node_vrml97::fog_node::bind(const bool val,
                                          const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->is_bound_.value(val);
    node::emit_event(this->is_bound_emitter_, timestamp);
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void openvrml_node_vrml97::fog_node::do_initialize(double)
    OPENVRML_NOTHROW
{
    fog_metatype & nodeClass =
        const_cast<fog_metatype &>(
            static_cast<const fog_metatype &>(this->type().metatype()));
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls fog_metatype::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void openvrml_node_vrml97::fog_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    fog_metatype & node_metatype =
        const_cast<fog_metatype &>(
            static_cast<const fog_metatype &>(this->type().metatype()));
    node_metatype.unbind(*this, timestamp);

    if (node_metatype.is_first(*this)) { node_metatype.reset_first(); }
}
