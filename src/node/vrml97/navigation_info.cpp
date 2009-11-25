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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/browser.h>
# include <private.h>
# include "navigation_info.h"

/**
 * @var openvrml_node_vrml97::navigation_info_node * openvrml_node_vrml97::navigation_info_metatype::first
 *
 * @brief The first NavigationInfo node in the initial scene graph.
 */

/**
 * @var openvrml::node_impl_util::bound_node_stack<openvrml_node_vrml97::navigation_info_node> openvrml_node_vrml97::navigation_info_metatype::bound_nodes
 *
 * @brief The bound NavigationInfo node stack.
 */

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::navigation_info_metatype::id =
    "urn:X-openvrml:node:NavigationInfo";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::navigation_info_metatype::
navigation_info_metatype(openvrml::browser & browser):
    node_metatype(navigation_info_metatype::id, browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::navigation_info_metatype::~navigation_info_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Set the first NavigationInfo node in the world.
 *
 * The first NavigationInfo node in the world is used as the initially active
 * NavigationInfo.  This method is used by
 * @c navigation_info_node::do_initialize.
 *
 * @param nav_info    a NavigationInfo node.
 */
void
openvrml_node_vrml97::navigation_info_metatype::
set_first(navigation_info_node & nav_info) OPENVRML_NOTHROW
{
    this->first = &nav_info;
}

/**
 * @brief Reset the pointer to the first NavigationInfo node in the world
 *        to null.
 *
 * This function is called by @c navigation_info_node::do_shutdown.
 */
void openvrml_node_vrml97::navigation_info_metatype::reset_first() OPENVRML_NOTHROW
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by @c navigation_info_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false
 *         otherwise.
 */
bool openvrml_node_vrml97::navigation_info_metatype::has_first() const OPENVRML_NOTHROW
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param nav_info  a @c navigation_info_node.
 *
 * @return @c true if @p nav_info is the fist node; @c false otherwise.
 */
bool
openvrml_node_vrml97::navigation_info_metatype::
is_first(navigation_info_node & nav_info) OPENVRML_NOTHROW
{
    return &nav_info == this->first;
}

/**
 * @brief Push a navigation_info_node on the top of the bound node stack.
 *
 * @param nav_info    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::navigation_info_metatype::
bind(navigation_info_node & nav_info, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.bind(nav_info, timestamp);
    this->browser().active_navigation_info(nav_info);
}

/**
 * @brief Remove a navigation_info_node from the bound node stack.
 *
 * @param nav_info    the node to unbind.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::navigation_info_metatype::
unbind(navigation_info_node & nav_info, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.unbind(nav_info, timestamp);
    if (this->bound_nodes.empty()) {
        this->browser().reset_default_navigation_info();
    } else {
        this->browser().active_navigation_info(*this->bound_nodes.top());
    }
}

/**
 * @brief node_metatype-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound
 *                          initially.
 * @param timestamp         the current time.
 */
void
openvrml_node_vrml97::navigation_info_metatype::
do_initialize(openvrml::viewpoint_node *, const double timestamp)
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

# define NAVIGATION_INFO_INTERFACE_SEQ                                  \
    ((eventin,      sfbool,   "set_bind",        set_bind_listener_))   \
    ((exposedfield, mffloat,  "avatarSize",      avatar_size_))         \
    ((exposedfield, sfbool,   "headlight",       headlight_))           \
    ((exposedfield, sffloat,  "speed",           speed_))               \
    ((exposedfield, mfstring, "type",            type_))                \
    ((exposedfield, sffloat,  "visibilityLimit", visibility_limit_))    \
    ((eventout,     sfbool,   "isBound",         is_bound_emitter_))    \
    ((exposedfield, sfnode,   "metadata",        metadata))             \
    ((exposedfield, mfstring, "transitionType",  transition_type_))     \
    ((eventout,     sftime,   "bindTime",        bind_time_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              navigation_info_metatype,
                                              navigation_info_node,
                                              NAVIGATION_INFO_INTERFACE_SEQ)

/**
 * @class openvrml_node_vrml97::navigation_info_node
 *
 * @brief NavigationInfo node instances.
 */

/**
 * @var class openvrml_node_vrml97::navigation_info_node::navigation_info_metatype
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @internal
 *
 * @class openvrml_node_vrml97::navigation_info_node::set_bind_listener
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  navigation_info_node.
 */
openvrml_node_vrml97::navigation_info_node::set_bind_listener::
set_bind_listener(navigation_info_node & node):
    openvrml::node_event_listener(node),
    openvrml::node_impl_util::event_listener_base<navigation_info_node>(node),
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::navigation_info_node::set_bind_listener::
~set_bind_listener() OPENVRML_NOTHROW
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
openvrml_node_vrml97::navigation_info_node::set_bind_listener::
do_process_event(const openvrml::sfbool & bind,
                 const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    try {
        navigation_info_node & node =
            dynamic_cast<navigation_info_node &>(this->node());
        navigation_info_metatype & node_metatype =
            const_cast<navigation_info_metatype &>(
                static_cast<const navigation_info_metatype &>(
                    this->node().type().metatype()));
        if (bind.value()) {
            node_metatype.bind(node, timestamp);
        } else {
            node_metatype.unbind(node, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml_node_vrml97::navigation_info_node::set_bind_listener openvrml_node_vrml97::navigation_info_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::navigation_info_node>::exposedfield<openvrml::mffloat> openvrml_node_vrml97::navigation_info_node::avatar_size_
 *
 * @brief avatarSize exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::navigation_info_node>::exposedfield<openvrml::sfbool> openvrml_node_vrml97::navigation_info_node::headlight_
 *
 * @brief headlight exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::navigation_info_node>::exposedfield<openvrml::sffloat> openvrml_node_vrml97::navigation_info_node::speed_
 *
 * @brief speed exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::navigation_info_node>::exposedfield<openvrml::mfstring> openvrml_node_vrml97::navigation_info_node::type_
 *
 * @brief type exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::navigation_info_node>::exposedfield<openvrml::sffloat> openvrml_node_vrml97::navigation_info_node::visibility_limit_
 *
 * @brief visibilityLimit exposedField.
 */

/**
 * @var openvrml::sfbool openvrml_node_vrml97::navigation_info_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml_node_vrml97::navigation_info_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

const float navigation_avatar_size_[] = { 0.25f, 1.6f, 0.75f };
const std::string navigation_type_[] = { "WALK", "ANY" };

/**
 * @brief Construct.
 *
 * @param t     the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml_node_vrml97::navigation_info_node::
navigation_info_node(const openvrml::node_type & t,
                     const boost::shared_ptr<openvrml::scope> & scope):
    node(t, scope),
    bounded_volume_node(t, scope),
    child_node(t, scope),
    openvrml::node_impl_util::abstract_node<navigation_info_node>(t, scope),
    openvrml::navigation_info_node(t, scope),
    set_bind_listener_(*this),
    avatar_size_(*this, std::vector<float>(navigation_avatar_size_,
                                           navigation_avatar_size_ + 3)),
    headlight_(*this, true),
    speed_(*this, 1.0),
    type_(*this, std::vector<std::string>(navigation_type_,
                                          navigation_type_ + 2)),
    visibility_limit_(*this, 0.0),
    transition_type_(*this, std::vector<std::string>(1, "LINEAR")),
    is_bound_emitter_(*this, this->is_bound_),
    bind_time_emitter_(*this, this->bind_time_)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::navigation_info_node::~navigation_info_node()
    OPENVRML_NOTHROW
{}

/**
 * @brief Set the bind state.
 *
 * This function is called by @c bound_node_stack<BindableNode>::bind.  It
 * sets the &ldquo;bind&rdquo; state and fires an event; it does not
 * actually put the node onto the bound node stack.
 *
 * @param[in] val       the bind state.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml_node_vrml97::navigation_info_node::bind(const bool val,
                                                      const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->is_bound_.value(val);
    node::emit_event(this->is_bound_emitter_, timestamp);
}

/**
 * @brief The avatar size.
 *
 * @return the avatar size.
 */
const std::vector<float> &
openvrml_node_vrml97::navigation_info_node::do_avatar_size() const
    OPENVRML_NOTHROW
{
    return this->avatar_size_.mffloat::value();
}

/**
 * @brief The headlight state.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool openvrml_node_vrml97::navigation_info_node::do_headlight() const
    OPENVRML_NOTHROW
{
    return this->headlight_.sfbool::value();
}

/**
 * @brief The speed.
 *
 * @return the speed.
 */
float openvrml_node_vrml97::navigation_info_node::do_speed() const
    OPENVRML_NOTHROW
{
    return this->speed_.sffloat::value();
}

/**
 * @brief The navigation type.
 *
 * @return the navigation type.
 */
const std::vector<std::string> &
openvrml_node_vrml97::navigation_info_node::do_type() const OPENVRML_NOTHROW
{
    return this->type_.mfstring::value();
}

/**
 * @brief The visibility limit.
 *
 * @return the visibility limit.
 */
float openvrml_node_vrml97::navigation_info_node::do_visibility_limit() const
    OPENVRML_NOTHROW
{
    return this->visibility_limit_.sffloat::value();
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void
openvrml_node_vrml97::navigation_info_node::do_initialize(double)
    OPENVRML_NOTHROW
{
    using boost::polymorphic_downcast;

    navigation_info_metatype & node_metatype =
        const_cast<navigation_info_metatype &>(
            *polymorphic_downcast<const navigation_info_metatype *>(
                &this->node::type().metatype()));
    if (!node_metatype.has_first()) { node_metatype.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml_node_vrml97::navigation_info_node::
do_shutdown(const double timestamp) OPENVRML_NOTHROW
{
    using boost::polymorphic_downcast;

    navigation_info_metatype & node_metatype =
        const_cast<navigation_info_metatype &>(
            *polymorphic_downcast<const navigation_info_metatype *>(
                &this->node::type().metatype()));
    node_metatype.unbind(*this, timestamp);

    if (node_metatype.is_first(*this)) { node_metatype.reset_first(); }
}
