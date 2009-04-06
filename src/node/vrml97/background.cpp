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

# include "background.h"
# include "image_stream_listener.h"
# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <openvrml/scope.h>
# include <openvrml/viewer.h>
# include <private.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

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
    first(0),
    null_texture_node_metatype_(browser),
    null_texture_node_type_(this->null_texture_node_metatype_)
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

namespace {

    class OPENVRML_LOCAL null_texture_node : public openvrml::texture_node {
    public:
        explicit null_texture_node(const openvrml::null_node_type & type)
            OPENVRML_NOTHROW;
        virtual ~null_texture_node() OPENVRML_NOTHROW;

    private:
        virtual void do_field(const std::string & id,
                              const openvrml::field_value & value)
            OPENVRML_NOTHROW;

        virtual
        const openvrml::field_value &
        do_field(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp)
            OPENVRML_NOTHROW;

        virtual
        const openvrml::field_value &
        do_eventout(const std::string & id) const
            OPENVRML_NOTHROW;

        virtual
        openvrml::event_listener &
        do_event_listener(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);

        virtual
        openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            OPENVRML_THROW1(openvrml::unsupported_interface);

        virtual const openvrml::image & do_image() const OPENVRML_NOTHROW;
        virtual bool do_repeat_s() const OPENVRML_NOTHROW;
        virtual bool do_repeat_t() const OPENVRML_NOTHROW;
    };
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
        static const null_texture_node front(this->null_texture_node_type_);
        static const null_texture_node back(this->null_texture_node_type_);
        static const null_texture_node left(this->null_texture_node_type_);
        static const null_texture_node right(this->null_texture_node_type_);
        static const null_texture_node top(this->null_texture_node_type_);
        static const null_texture_node bottom(this->null_texture_node_type_);
        v.insert_background(ground_angle, ground_color,
                            sky_angle, sky_color,
                            front, back,
                            left, right,
                            top, bottom);
    } else {
        assert(this->bound_nodes.top());
        background_node & background = *this->bound_nodes.top();

        // Background isn't selectable, so don't waste the time.
        if (v.mode() == viewer::pick_mode) { return; }

        if (background.modified()) {
            v.remove_object(background);
        }

        v.insert_background(background.ground_angle_.mffloat::value(),
                            background.ground_color_.mfcolor::value(),
                            background.sky_angle_.mffloat::value(),
                            background.sky_color_.mfcolor::value(),
                            *background.front,
                            *background.back,
                            *background.left,
                            *background.right,
                            *background.top,
                            *background.bottom);

        background.modified(false);
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
                &background_node::set_bind_listener_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::ground_angle_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::ground_color_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::back_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::bottom_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::front_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::left_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::right_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::top_url_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::sky_angle_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::sky_color_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::is_bound_emitter_);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            backgroundNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                &background_node::bind_time_emitter_);
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
    back_url_(*this),
    bottom_url_(*this),
    front_url_(*this),
    left_url_(*this),
    right_url_(*this),
    top_url_(*this),
    sky_angle_(*this),
    sky_color_(*this,
               std::vector<openvrml::color>(
                   1, openvrml::make_color(0.0, 0.0, 0.0))),
    is_bound_emitter_(*this, this->is_bound_),
    bind_time_emitter_(*this, this->bind_time_)
{
    using openvrml::mfstring;
    using openvrml::node_cast;
    using openvrml::texture_node;

    const boost::shared_ptr<openvrml::node_type> image_texture_type =
        scope->find_type("ImageTexture");
    assert(image_texture_type);

    this->front =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());
    this->back =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());
    this->left =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());
    this->right =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());
    this->top =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());
    this->bottom =
        node_cast<texture_node *>(image_texture_type->create_node(scope).get());

    //
    // We could make this more (space) efficient if we let background_node
    // know more about image_texture_node's internals.  But it's probably okay
    // if background_node is a little heavy.
    //
    this->front_url_.add(this->front->event_listener<mfstring>("url"));
    this->back_url_.add(this->back->event_listener<mfstring>("url"));
    this->left_url_.add(this->left->event_listener<mfstring>("url"));
    this->right_url_.add(this->right->event_listener<mfstring>("url"));
    this->top_url_.add(this->top->event_listener<mfstring>("url"));
    this->bottom_url_.add(this->bottom->event_listener<mfstring>("url"));
}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::background_node::~background_node() OPENVRML_NOTHROW
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

    const boost::shared_ptr<openvrml::scope> null_scope_ptr;

    null_texture_node::null_texture_node(const openvrml::null_node_type & type)
        OPENVRML_NOTHROW:
        openvrml::node(type, null_scope_ptr),
        openvrml::texture_node(type, null_scope_ptr)
    {}

    null_texture_node::~null_texture_node() OPENVRML_NOTHROW
    {}

    void null_texture_node::do_field(const std::string &,
                                     const openvrml::field_value &)
        OPENVRML_NOTHROW
    {
        assert(false && "do not call this function");
    }

    const openvrml::field_value &
    null_texture_node::do_field(const std::string &) const OPENVRML_NOTHROW
    {
        assert(false && "do not call this function");
        static const openvrml::sfbool value;
        return value;
    }

    void null_texture_node::do_process_event(const std::string &,
                                             const openvrml::field_value &,
                                             double)
        OPENVRML_NOTHROW
    {
        assert(false && "do not call this function");
    }

    const openvrml::field_value &
    null_texture_node::do_eventout(const std::string &) const
        OPENVRML_NOTHROW
    {
        assert(false && "do not call this function");
        static const openvrml::sfbool value;
        return value;
    }

    openvrml::event_listener &
    null_texture_node::do_event_listener(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false && "do not call this function");
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_listener *>(0);
    }

    openvrml::event_emitter &
    null_texture_node::do_event_emitter(const std::string & id)
        OPENVRML_THROW1(openvrml::unsupported_interface)
    {
        assert(false && "do not call this function");
        throw openvrml::unsupported_interface(this->node::type(), id);
        return *static_cast<openvrml::event_emitter *>(0);
    }

    const openvrml::image & null_texture_node::do_image() const OPENVRML_NOTHROW
    {
        static const openvrml::image img;
        return img;
    }

    bool null_texture_node::do_repeat_s() const OPENVRML_NOTHROW
    {
        return false;
    }

    bool null_texture_node::do_repeat_t() const OPENVRML_NOTHROW
    {
        return false;
    }
}
