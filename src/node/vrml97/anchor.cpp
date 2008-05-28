// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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
# include "anchor.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL anchor_node :
        public openvrml_node_vrml97::grouping_node_base<anchor_node>,
        public openvrml::pointing_device_sensor_node {

        friend class openvrml_node_vrml97::anchor_metatype;

        exposedfield<openvrml::sfstring> description_;
        exposedfield<openvrml::mfstring> parameter_;
        exposedfield<openvrml::mfstring> url_;

    public:
        anchor_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~anchor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual void do_activate(double timestamp, bool over, bool active,
                                 const double (&point)[3]);
    };

    /**
     * @class anchor_node
     *
     * @brief Represents Anchor node instances.
     */

    /**
     * @var class anchor_node::anchor_metatype
     *
     * @brief Class object for Anchor nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<anchor_node>::exposedfield<openvrml::sfstring> anchor_node::description_
     *
     * @brief description exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<anchor_node>::exposedfield<openvrml::mfstring> anchor_node::parameter_
     *
     * @brief parameter exposedField
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<anchor_node>::exposedfield<openvrml::mfstring> anchor_node::url_
     *
     * @brief url exposedField
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    anchor_node::
    anchor_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<anchor_node>(type, scope),
        pointing_device_sensor_node(type, scope),
        description_(*this),
        parameter_(*this),
        url_(*this)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    anchor_node::~anchor_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    anchor_node::
    do_render_child(openvrml::viewer & viewer,
                    const openvrml::rendering_context context)
    {
        viewer.set_sensitive(this);

        // Render children
        this->openvrml_node_vrml97::grouping_node_base<anchor_node>::
            do_render_child(viewer, context);

        viewer.set_sensitive(0);
    }

    /**
     * @brief Handle a click by loading the url.
     */
    void anchor_node::do_activate(double,
                                  const bool over,
                                  const bool active,
                                  const double (&)[3])
    {
        assert(this->scene());
        //
        // @todo This should really be (is_over && !is_active && n->was_active)
        // (ie, button up over the anchor after button down over the
        // anchor)
        //
        if (over && active) {
            this->scene()->load_url(this->url_.mfstring::value(),
                                    this->parameter_.mfstring::value());
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::anchor_metatype::id =
    "urn:X-openvrml:node:Anchor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c anchor_metatype.
 */
openvrml_node_vrml97::anchor_metatype::
anchor_metatype(openvrml::browser & browser):
    node_metatype(anchor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::anchor_metatype::~anchor_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Anchor nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by anchor_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::anchor_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 9> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
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
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<anchor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & anchorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_ = interfaces.begin();
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            anchorNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    anchor_node::add_children_listener>(
                        &anchor_node::add_children_listener_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    anchor_node::remove_children_listener>(
                        &anchor_node::remove_children_listener_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    anchor_node::children_exposedfield>(
                        &anchor_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    anchor_node::children_exposedfield>(
                        &anchor_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    anchor_node::children_exposedfield>(
                        &anchor_node::children_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<anchor_node>::exposedfield<sfstring> >(
                        &anchor_node::description_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<anchor_node>::exposedfield<sfstring> >(
                        &anchor_node::description_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<anchor_node>::exposedfield<sfstring> >(
                        &anchor_node::description_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::parameter_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::parameter_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::parameter_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<anchor_node>::exposedfield<mfstring> >(
                        &anchor_node::url_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &anchor_node::bbox_center_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &anchor_node::bbox_size_)));
        } else if (*interface_ == *++supported_interface) {
            anchorNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<anchor_node>::exposedfield<sfnode> >(
                        &anchor_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<anchor_node>::exposedfield<sfnode> >(
                        &anchor_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<anchor_node>::exposedfield<sfnode> >(
                        &anchor_node::metadata)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
