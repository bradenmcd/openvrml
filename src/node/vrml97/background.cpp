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
# include "background.h"
# include "image_stream_listener.h"

/**
 * @var openvrml_node_vrml97::background_node * openvrml_node_vrml97::background_metatype::first
 *
 * @brief The first Background node in the initial scene graph.
 */

/**
 * @var openvrml::node_impl_util::bound_node_stack<openvrml_node_vrml97::background_node> openvrml_node_vrml97::background_metatype::bound_nodes
 *
 * @brief The bound Background node stack.
 */

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::background_metatype::id = "urn:X-openvrml:node:Background";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::background_metatype::
background_metatype(openvrml::browser & browser):
    node_metatype(background_metatype::id, browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::background_metatype::~background_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Set the first Background node in the world.
 *
 * The first Background node in the world is used as the initial
 * background. This method is used by @c background_node::do_initialize.
 *
 * @param background    a Background node.
 */
void
openvrml_node_vrml97::background_metatype::
set_first(background_node & background) OPENVRML_NOTHROW
{
    this->first = &background;
}

/**
 * @brief Reset the pointer to the first Background node in the world to
 *        null.
 *
 * This function is called by @c background_node::do_shutdown.
 */
void openvrml_node_vrml97::background_metatype::reset_first() OPENVRML_NOTHROW
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by @c background_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false
 *         otherwise.
 */
bool openvrml_node_vrml97::background_metatype::has_first() const
    OPENVRML_NOTHROW
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param background    a background_node.
 *
 * @return @c true if @p background is the fist node; @c false otherwise.
 */
bool
openvrml_node_vrml97::background_metatype::
is_first(background_node & background)
    OPENVRML_NOTHROW
{
    return &background == this->first;
}

/**
 * @brief Push a @c background_node on the top of the bound node stack.
 *
 * @param background    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::background_metatype::bind(background_node & background,
                                                const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.bind(background, timestamp);
}

/**
 * @brief Remove a @c background_node from the bound node stack.
 *
 * @param background    the node to unbind.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::background_metatype::unbind(background_node & background,
                                                  const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    this->bound_nodes.unbind(background, timestamp);
}

/**
 * @brief @c node_metatype%-specific initialization.
 *
 * @param initial_viewpoint the @c viewpoint_node that should be bound
 *                          initially.
 * @param timestamp         the current time.
 */
void
openvrml_node_vrml97::background_metatype::
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

/**
 * @brief @c node_metatype%-specific rendering.
 *
 * Render the active Background node.
 *
 * @param v viewer.
 */
void
openvrml_node_vrml97::background_metatype::
do_render(openvrml::viewer & v) const
    OPENVRML_NOTHROW
{
    using namespace openvrml;

    if (this->bound_nodes.empty()) {
        //
        // Default background.
        //
        using std::vector;
        static const vector<float> ground_angle;
        static const vector<color> ground_color;
        static const vector<float> sky_angle;
        static const vector<color> sky_color;
        static const image front;
        static const image back;
        static const image left;
        static const image right;
        static const image top;
        static const image bottom;
        v.insert_background(ground_angle,
                            ground_color,
                            sky_angle,
                            sky_color,
                            front,
                            back,
                            left,
                            right,
                            top,
                            bottom);
    } else {
        assert(this->bound_nodes.top());
        background_node & background = *this->bound_nodes.top();

        // Background isn't selectable, so don't waste the time.
        if (v.mode() == viewer::pick_mode) { return; }

        if (background.viewerObject && background.modified()) {
            v.remove_object(background.viewerObject);
            background.viewerObject = 0;
        }

        if (background.viewerObject) {
            v.insert_reference(background.viewerObject);
        } else {
            background.update_textures();

            read_write_mutex::scoped_read_lock
                front_lock(background.front_mutex_),
                back_lock(background.back_mutex_),
                left_lock(background.left_mutex_),
                right_lock(background.right_mutex_),
                top_lock(background.top_mutex_),
                bottom_lock(background.bottom_mutex_);

            background.viewerObject =
                v.insert_background(
                    background.ground_angle_.mffloat::value(),
                    background.ground_color_.mfcolor::value(),
                    background.sky_angle_.mffloat::value(),
                    background.sky_color_.mfcolor::value(),
                    background.front,
                    background.back,
                    background.left,
                    background.right,
                    background.top,
                    background.bottom);

            background.modified(false);
        }
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c boost::shared_ptr<node_type> to a @c node_type capable of
 *         creating Background nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c background_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::background_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 14> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
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
                       "isBound"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "bindTime")
    };

    typedef node_impl_util::node_type_impl<background_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & backgroundNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::set_bind_listener>(
                        &background_node::set_bind_listener_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::ground_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::ground_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::ground_angle_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::ground_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::ground_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::ground_color_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::back_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::back_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::back_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::bottom_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::bottom_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::bottom_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::front_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::front_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::front_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::left_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::left_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::left_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::right_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::right_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::right_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::top_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::top_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::texture_url_exposedfield>(
                        &background_node::top_url_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::sky_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::sky_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::exposedfield<mffloat> >(
                        &background_node::sky_angle_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::sky_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::sky_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::exposedfield<mfcolor> >(
                        &background_node::sky_color_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::sfbool_emitter>(
                        &background_node::is_bound_emitter_)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<background_node>::exposedfield<sfnode> >(
                        &background_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<background_node>::exposedfield<sfnode> >(
                        &background_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::exposedfield<sfnode> >(
                        &background_node::metadata)));
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<background_node>::sftime_emitter>(
                        &background_node::bind_time_emitter_)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}

/**
 * @class openvrml_node_vrml97::background_node
 *
 * @brief Background node instances.
 */

/**
 * @var class openvrml_node_vrml97::background_node::background_metatype
 *
 * @brief Class object for Background nodes.
 */

/**
 * @internal
 *
 * @class openvrml_node_vrml97::background_node::set_bind_listener
 *
 * @brief set_bind eventIn listener.
 */

/**
 * @brief Construct.
 *
 * @param node background_node.
 */
openvrml_node_vrml97::background_node::set_bind_listener::
set_bind_listener(background_node & node):
    openvrml::node_event_listener(node),
    openvrml::node_impl_util::event_listener_base<background_node>(node),
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::background_node::set_bind_listener::~set_bind_listener()
    OPENVRML_NOTHROW
{}

/**
 * @brief Process an event.
 *
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::background_node::set_bind_listener::
do_process_event(const openvrml::sfbool & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    try {
        background_node & node =
            dynamic_cast<background_node &>(this->node());
        background_metatype & node_metatype =
            const_cast<background_metatype &>(
                static_cast<const background_metatype &>(
                    node.type().metatype()));
        if (value.value()) {
            node_metatype.bind(node, timestamp);
        } else {
            node_metatype.unbind(node, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml_node_vrml97::background_node::texture_url_exposedfield
 *
 * @brief Texture URL @c exposedField implementation.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::* openvrml_node_vrml97::background_node::texture_url_exposedfield::needs_update_
 *
 * @brief Pointer to a @c bool member that serves as a flag to indicate
 *        whether a texture needs to be updated.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml_node_vrml97::background_node::texture_url_exposedfield::
texture_url_exposedfield(background_node & node,
                         bool background_node::* const needs_update)
OPENVRML_NOTHROW:
openvrml::node_event_listener(node),
    event_emitter(static_cast<const openvrml::field_value &>(*this)),
    mfstring_listener(node),
    exposedfield<openvrml::mfstring>(node),
    needs_update_(needs_update)
{}

/**
 * @brief Construct a copy.
 *
 * @param obj   instance to copy.
 */
openvrml_node_vrml97::background_node::texture_url_exposedfield::
texture_url_exposedfield(const texture_url_exposedfield & obj) OPENVRML_NOTHROW:
openvrml::event_listener(),
    openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
    openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
    mfstring_listener(obj.openvrml::node_event_listener::node()),
    exposedfield<openvrml::mfstring>(obj),
    needs_update_(obj.needs_update_)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::background_node::texture_url_exposedfield::
~texture_url_exposedfield() OPENVRML_NOTHROW
{}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a copy of the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml_node_vrml97::background_node::texture_url_exposedfield::
do_clone() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::auto_ptr<openvrml::field_value>(
        new texture_url_exposedfield(*this));
}

/**
 * @brief Process event.
 *
 * @param value     new value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml_node_vrml97::background_node::texture_url_exposedfield::
event_side_effect(const openvrml::mfstring &, double)
    OPENVRML_THROW1(std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node_event_listener::node())
            .*this->needs_update_ = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml_node_vrml97::background_node::set_bind_listener openvrml_node_vrml97::background_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::background_node>::exposedfield<openvrml::mffloat> openvrml_node_vrml97::background_node::ground_angle_
 *
 * @brief groundAngle exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::background_node>::exposedfield<openvrml::mfcolor> openvrml_node_vrml97::background_node::ground_color_
 *
 * @brief groundColor exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::back_url_
 *
 * @brief backUrl exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::bottom_url_
 *
 * @brief bottomUrl exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::front_url_
 *
 * @brief frontUrl exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::left_url_
 *
 * @brief leftUrl exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::right_url_
 *
 * @brief rightUrl exposedField.
 */

/**
 * @var openvrml_node_vrml97::background_node::texture_url_exposedfield openvrml_node_vrml97::background_node::top_url_
 *
 * @brief topUrl exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::background_node>::exposedfield<openvrml::mffloat> openvrml_node_vrml97::background_node::sky_angle_
 *
 * @brief skyAngle exposedField.
 */

/**
 * @var openvrml::node_impl_util::abstract_node<openvrml_node_vrml97::background_node>::exposedfield<openvrml::mfcolor> openvrml_node_vrml97::background_node::sky_color_
 *
 * @brief skyColor exposedField.
 */

/**
 * @var openvrml::sfbool openvrml_node_vrml97::background_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml_node_vrml97::background_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::front
 *
 * @brief Front image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::front_needs_update
 *
 * @brief Flag to indicate that the front image needs to be updated.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::back
 *
 * @brief Back image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::back_needs_update
 *
 * @brief Flag to indicate that the back image needs to be updated.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::left
 *
 * @brief Left image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::left_needs_update
 *
 * @brief Flag to indicate that the left image needs to be updated.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::right
 *
 * @brief Right image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::right_needs_update
 *
 * @brief Flag to indicate that the right image needs to be updated.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::top
 *
 * @brief Top image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::top_needs_update
 *
 * @brief Flag to indicate that the top image needs to be updated.
 */

/**
 * @var openvrml::image openvrml_node_vrml97::background_node::bottom
 *
 * @brief Bottom image.
 */

/**
 * @var bool openvrml_node_vrml97::background_node::bottom_needs_update
 *
 * @brief Flag to indicate that the bottom image needs to be updated.
 */

/**
 * @var openvrml::viewer::object_t openvrml_node_vrml97::background_node::viewerObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml_node_vrml97::background_node::
background_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    openvrml::node_impl_util::abstract_node<background_node>(type, scope),
    child_node(type, scope),
    set_bind_listener_(*this),
    ground_angle_(*this),
    ground_color_(*this),
    back_url_(*this, &background_node::back_needs_update),
    bottom_url_(*this, &background_node::bottom_needs_update),
    front_url_(*this, &background_node::front_needs_update),
    left_url_(*this, &background_node::left_needs_update),
    right_url_(*this, &background_node::right_needs_update),
    top_url_(*this, &background_node::top_needs_update),
    sky_angle_(*this),
    sky_color_(*this,
               std::vector<openvrml::color>(
                   1, openvrml::make_color(0.0, 0.0, 0.0))),
    is_bound_emitter_(*this, this->is_bound_),
    bind_time_emitter_(*this, this->bind_time_),
    front_needs_update(true),
    back_needs_update(true),
    left_needs_update(true),
    right_needs_update(true),
    top_needs_update(true),
    bottom_needs_update(true),
    viewerObject(0)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::background_node::~background_node() OPENVRML_NOTHROW
{
    // remove d_viewerObject...
}

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
void openvrml_node_vrml97::background_node::bind(const bool val,
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
void openvrml_node_vrml97::background_node::do_initialize(double)
    OPENVRML_NOTHROW
{
    using boost::polymorphic_downcast;
    background_metatype & nodeClass =
        const_cast<background_metatype &>(
            *polymorphic_downcast<const background_metatype *>(
                &this->type().metatype()));
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls background_metatype::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void
openvrml_node_vrml97::background_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    background_metatype & node_metatype =
        const_cast<background_metatype &>(
            static_cast<const background_metatype &>(
                this->type().metatype()));
    node_metatype.unbind(*this, timestamp);

    if (node_metatype.is_first(*this)) { node_metatype.reset_first(); }
}

namespace {

    OPENVRML_LOCAL void
    update_texture(openvrml_node_vrml97::background_node & node,
                   openvrml::read_write_mutex & img_mutex,
                   const openvrml::mfstring & url,
                   openvrml::image & img)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using openvrml::image;
            using openvrml_node_vrml97::image_stream_listener;

            if (url.value().empty()) {
                img = image();
            } else {
                using std::auto_ptr;

                auto_ptr<openvrml::resource_istream> in(
                    node.scene()->get_resource(url.value()));
                auto_ptr<openvrml::stream_listener> listener(
                    new image_stream_listener(in->url(),
                                              img,
                                              node,
                                              img_mutex));
                node.scene()->read_stream(in, listener);
            }
        } catch (const openvrml::no_alternative_url &) {}
    }
}

/**
 * @brief Called lazily to update texture data.
 */
void openvrml_node_vrml97::background_node::update_textures()
    OPENVRML_THROW1(std::bad_alloc)
{
    if (this->front_needs_update) {
        update_texture(*this,
                       this->front_mutex_,
                       this->front_url_,
                       this->front);
        this->front_needs_update = false;
    }
    if (this->back_needs_update) {
        update_texture(*this,
                       this->back_mutex_,
                       this->back_url_,
                       this->back);
        this->back_needs_update = false;
    }
    if (this->left_needs_update) {
        update_texture(*this,
                       this->left_mutex_,
                       this->left_url_,
                       this->left);
        this->left_needs_update = false;
    }
    if (this->right_needs_update) {
        update_texture(*this,
                       this->right_mutex_,
                       this->right_url_,
                       this->right);
        this->right_needs_update = false;
    }
    if (this->top_needs_update) {
        update_texture(*this,
                       this->top_mutex_,
                       this->top_url_,
                       this->top);
        this->top_needs_update = false;
    }
    if (this->bottom_needs_update) {
        update_texture(*this,
                       this->bottom_mutex_,
                       this->bottom_url_,
                       this->bottom);
        this->bottom_needs_update = false;
    }
}
