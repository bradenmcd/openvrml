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
# include "directional_light.h"
# include "abstract_light.h"

namespace {

    class OPENVRML_LOCAL directional_light_node :
        public openvrml_node_vrml97::abstract_light_node<directional_light_node> {

        friend class openvrml_node_vrml97::directional_light_metatype;

        exposedfield<openvrml::sfvec3f> direction_;

    public:
        directional_light_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~directional_light_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class directional_light_node
     *
     * @brief DirectionalLight node instances.
     */

    /**
     * @var class directional_light_node::directional_light_metatype
     *
     * @brief Class object for DirectionalLight nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<directional_light_node>::exposedfield<openvrml::sfvec3f> directional_light_node::direction_
     *
     * @brief direction exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope     the scope to which the node belongs.
     */
    directional_light_node::
    directional_light_node(const openvrml::node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        light_node(type, scope),
        openvrml_node_vrml97::abstract_light_node<directional_light_node>(
            type, scope),
        direction_(*this, openvrml::make_vec3f(0.0, 0.0, -1.0))
    {}

    /**
     * @brief Destroy.
     */
    directional_light_node::~directional_light_node()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * This should be called before rendering any sibling nodes.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    directional_light_node::
    do_render_child(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        if (this->on_.sfbool::value()) {
            viewer.insert_dir_light(this->ambient_intensity_.sffloat::value(),
                                    this->intensity_.sffloat::value(),
                                    this->color_.sfcolor::value(),
                                    this->direction_.sfvec3f::value());
        }
        this->node::modified(false);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::directional_light_metatype::id =
    "urn:X-openvrml:node:DirectionalLight";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::directional_light_metatype::
directional_light_metatype(openvrml::browser & browser):
    node_metatype(directional_light_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::directional_light_metatype::~directional_light_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating DirectionalLight nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  directional_light_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::directional_light_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
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
                       "on"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<directional_light_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & directionalLightNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::
                    exposedfield<sffloat> >(
                        &directional_light_node::ambient_intensity_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sffloat> >(
                            &directional_light_node::ambient_intensity_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sffloat> >(
                            &directional_light_node::ambient_intensity_)));
    } else if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::
                    exposedfield<sfcolor> >(
                        &directional_light_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfcolor> >(
                            &directional_light_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfcolor> >(
                            &directional_light_node::color_)));
        } else if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::
                    exposedfield<sfvec3f> >(
                        &directional_light_node::direction_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfvec3f> >(
                            &directional_light_node::direction_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfvec3f> >(
                            &directional_light_node::direction_)));
        } else if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::
                    exposedfield<sffloat> >(
                        &directional_light_node::intensity_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sffloat> >(
                            &directional_light_node::intensity_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sffloat> >(
                            &directional_light_node::intensity_)));
        } else if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::
                    exposedfield<sfbool> >(&directional_light_node::on_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfbool> >(&directional_light_node::on_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<directional_light_node>::
                        exposedfield<sfbool> >(&directional_light_node::on_)));
        } else if (*interface == *++supported_interface) {
            directionalLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<directional_light_node>::exposedfield<sfnode> >(
                        &directional_light_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<directional_light_node>::exposedfield<sfnode> >(
                        &directional_light_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<directional_light_node>::exposedfield<sfnode> >(
                        &directional_light_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
