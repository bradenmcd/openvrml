// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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
# include "cad_assembly.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL cad_assembly_node :
        public openvrml_node_vrml97::grouping_node_base<cad_assembly_node> {

        friend class openvrml_node_vrml97::cad_assembly_metatype;

        //for CADAssembly node
        exposedfield<openvrml::sfstring> name_;

    public:
        cad_assembly_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cad_assembly_node() OPENVRML_NOTHROW;
    };

    /**
     * @class cad_assembly_node
     *
     * @brief Represents Group node instances.
     */

    /**
     * @var class cad_assembly_node::cad_assembly_metatype
     *
     * @brief Class object for Group nodes.
     */

    /**
     * @brief Construct.
     */
    cad_assembly_node::
    cad_assembly_node(const openvrml::node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<cad_assembly_node>(type, scope),
        name_(*this)
    {}

    /**
     * @brief Destroy.
     */
    cad_assembly_node::~cad_assembly_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cad_assembly_metatype::id =
    "urn:X-openvrml:node:CADAssembly";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::cad_assembly_metatype::
cad_assembly_metatype(openvrml::browser & browser):
    node_metatype(cad_assembly_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cad_assembly_metatype::~cad_assembly_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Group nodes.
 *
 * @exception openvrml::unsupported_interface
 *                                  if @p interfaces includes an interface
 *                                  not supported by cad_assembly_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::cad_assembly_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 7> supported_interfaces_t;
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
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name")
    };

    typedef node_impl_util::node_type_impl<cad_assembly_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & theNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            theNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    cad_assembly_node::add_children_listener>(
                        &cad_assembly_node::add_children_listener_)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    cad_assembly_node::remove_children_listener>(
                        &cad_assembly_node::remove_children_listener_)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    cad_assembly_node::children_exposedfield>(
                        &cad_assembly_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    cad_assembly_node::children_exposedfield>(
                        &cad_assembly_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    cad_assembly_node::children_exposedfield>(
                        &cad_assembly_node::children_)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &cad_assembly_node::bbox_center_)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &cad_assembly_node::bbox_size_)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfnode> >(
                        &cad_assembly_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfnode> >(
                        &cad_assembly_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfnode> >(
                        &cad_assembly_node::metadata)));
        } else if (*interface_ == *++supported_interface) {
            theNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfstring> >
                    (&cad_assembly_node::name_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfstring> >
                    (&cad_assembly_node::name_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<cad_assembly_node>::exposedfield<sfstring> >
                    (&cad_assembly_node::name_)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
