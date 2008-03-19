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
# include <private.h>
# include <openvrml/browser.h>
# include "collision.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL collision_node :
        public openvrml_node_vrml97::grouping_node_base<collision_node> {

        friend class openvrml_node_vrml97::collision_metatype;

        exposedfield<openvrml::sfbool> collide_;
        openvrml::sfnode proxy_;
        openvrml::sftime collide_time_;
        sftime_emitter collide_time_emitter_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;

    public:
        collision_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~collision_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    };


    /**
     * @class collision_node
     *
     * @brief Collision node instances.
     */

    /**
     * @var class collision_node::collision_metatype
     *
     * @brief Class object for Collision nodes.
     */

    /**
     * @var openvrml::sfbool collision_node::collide_
     *
     * @brief collide exposedField.
     */

    /**
     * @var openvrml::sfnode collision_node::proxy_
     *
     * @brief proxy field.
     */

    /**
     * @var openvrml::sftime collision_node::collide_time_
     *
     * @brief collideTime eventOut.
     */

    /**
     * @var openvrml::sftime_emitter collision_node::collide_time_emitter_
     *
     * @brief collideTime eventOut.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    collision_node::
    collision_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<collision_node>(type, scope),
        collide_(*this, true),
        collide_time_emitter_(*this, this->collide_time_),
        is_active_emitter_(*this, this->is_active_)
    {}

    /**
     * @brief Destroy.
     */
    collision_node::~collision_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool collision_node::modified() const
    {
        return (this->proxy_.value() && this->proxy_.value()->modified())
            || (this->openvrml_node_vrml97::grouping_node_base<collision_node>::
                modified());
    }
}

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::collision_metatype::id =
    "urn:X-openvrml:node:Collision";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::collision_metatype::
collision_metatype(openvrml::browser & browser):
    node_metatype(collision_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::collision_metatype::~collision_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Collision nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by collision_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::collision_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 11> supported_interfaces_t;
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
                       "collideTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive")
    };

    typedef node_impl_util::node_type_impl<collision_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & collisionNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            collisionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    collision_node::add_children_listener>(
                        &collision_node::add_children_listener_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    collision_node::remove_children_listener>(
                        &collision_node::remove_children_listener_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    collision_node::children_exposedfield>(
                        &collision_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    collision_node::children_exposedfield>(
                        &collision_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    collision_node::children_exposedfield>(
                        &collision_node::children_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &collision_node::bbox_center_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &collision_node::bbox_size_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfnode>(
                        &collision_node::proxy_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<collision_node>::sftime_emitter>(
                        &collision_node::collide_time_emitter_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<collision_node>::exposedfield<sfnode> >(
                        &collision_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<collision_node>::exposedfield<sfnode> >(
                        &collision_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<collision_node>::exposedfield<sfnode> >(
                        &collision_node::metadata)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<collision_node>::exposedfield<sfbool> >(
                        &collision_node::collide_)));
        } else if (*interface == *++supported_interface) {
            collisionNodeType.add_eventout(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<collision_node>::sfbool_emitter>(
                        &collision_node::is_active_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
